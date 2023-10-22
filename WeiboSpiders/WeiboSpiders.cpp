#include "WeiboSpiders.h"


WeiboSpiders::WeiboSpiders(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    //设置全局
    manager = new QNetworkAccessManager(this);

    //去掉标题栏
    setWindowFlag(Qt::FramelessWindowHint);
    //保持窗口大小不变(去掉标题框其实已经把拉伸功能去掉了)
    /*setMaximumSize(750, 450);
    setMinimumSize(750, 450);*/

    //设置窗口透明度
    setWindowOpacity(0.98);

    //列表渲染
    UserListRender();

    //链接
    //添加UId
    connect(ui.addUidButton, &QPushButton::clicked, this, &WeiboSpiders::onAddUidButtonClicked);
    //设置
    connect(ui.configButton, &QPushButton::clicked, this, &WeiboSpiders::toConfig);

}

WeiboSpiders::~WeiboSpiders()
{}

bool WeiboSpiders::toConfig() {

    config.show();

    return true;
}

void WeiboSpiders::mousePressEvent(QMouseEvent * e)
{
    //e->button()获取点击的按键，如果为鼠标左键
    if (e->button() & Qt::LeftButton) {
        //保存一下状态
        m_islBtn = true;
        //记录鼠标的位置（鼠标在屏幕的位置 - 窗口在屏幕的位置）
        m_prePoint = e->globalPos() - frameGeometry().topLeft();
    }
    //如果为鼠标右键
    else if (e->button() & Qt::RightButton)
    {
        //其他功能
    }
}

void WeiboSpiders::mouseMoveEvent(QMouseEvent* e)
{
    //判断
    if (m_islBtn) {
        //移动（全局鼠标位置 - 相对窗口的鼠标位置 = 窗口在全局的位置）
        move(e->globalPos() - m_prePoint);
    }
}

void WeiboSpiders::mouseReleaseEvent(QMouseEvent* e)
{
    //鼠标左键释放
    if (e->button() & Qt::LeftButton) {
        m_islBtn = false;
    }
}

void WeiboSpiders::closeEvent(QCloseEvent* e)
{
    QMessageBox::StandardButton result = QMessageBox::question(this, "确认", "确定要退出吗？",
        QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        e->accept();
        //关闭设置窗口
        config.close();
    }
    else
        e->ignore();

}


void WeiboSpiders::GetUserInfo(QString uid)
{

    QNetworkRequest request;
    request.setUrl(QUrl("https://weibo.com/ajax/profile/info?uid="+uid)); // 设置请求的URL
    // 如果需要设置HTTP请求头，可以使用request.setHeader()方法
    // 设置Cookie，将你的Cookie字符串替换成实际的Cookie值
    

    request.setRawHeader("Cookie", "SINAGLOBAL=912869638359.6813.1697710139455; ULV=1697710139491:1:1:1:912869638359.6813.1697710139455:; SUBP=0033WrSXqPxfM725Ws9jqgMF55529P9D9Wh7bdwKck40wuI.BMIIyA.S5JpX5KMhUgL.Fo-cShM7eK-4eo.2dJLoIf2LxKqL1KqLBo5LxKqL1KMLBoqLxKBLB.zLBKeLxKML1KBLBo-LxK-LB-BL1K5LxKnL1-zL1hqLxKnL1K.LB-zLxK-L1h-L1h.LxKMLB.-LB.Bt; XSRF-TOKEN=Z-BDG57eYpv923SfaI4jRnMz; ALF=1700548297; SSOLoginState=1697956298; SCF=AtAS8Ui1iUt5xJACpuSK9NNprafl8bSgDInJr8QRvx3euFN9I7X2Tbqi0Y3-FMDnG_SiDyNIgCGbW3q-zv9tiy4.; SUB=_2A25IMLGaDeRhGeNI71UR8SvFyTWIHXVrR6RSrDV8PUNbmtAbLXXukW9NSHPv23gMU122o1xcETOnhJdzH-rcnWiE; WBPSESS=3v-qpO0mmLJXfcpbEMQPP69e6clsA8vJkSjyOErpzGjEYS6cpiv5s5UrUds3STE8tESiUuxKal_AlXhoymsAlsbY9pIq3G9FyL7wXfj-COuJMzbelNxFtKEFfyjlTxiIz3YX4W8lb2FrWc21nCJlmQ==");



    QNetworkReply* reply = manager->get(request);

    // 连接finished信号以处理响应
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 在这里处理data，例如解析JSON等
            QJsonObject mainObj = QJsonDocument::fromJson(reply->readAll()).object();

            if (mainObj.contains("data")) {

                QJsonObject dataObj = mainObj["data"].toObject();

                if (dataObj.contains("user")) {

                    QJsonObject userObj = dataObj["user"].toObject();

                    QString name = userObj["screen_name"].toString();
                    QString tips = "正在添加: " + name;
                    QMessageBox::information(this, "提示", tips);
                    
                    //写文件
                    WriteToFile(userObj);
                    UserListRender();

                }
            }
            else {
                QMessageBox::information(this, "提示", "可能不存在该用户.");
            }

            
        }
        else {
            QMessageBox::information(this, "网络错误", "获取用户信息失败.");
            qDebug() << "ok";
        }
        reply->deleteLater(); // 释放资源

        // 请求完成后执行需要的操作
        });

    
}

void WeiboSpiders::UserListRender()
{
    QListView* listView = ui.listView;  //创建QListVIew对象
    QStringList list;   //创建数据显示列表
    QJsonArray userList = ReadFromFile();

    for (const QJsonValue &value : userList) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            list.append(obj["name"].toString());
        }
    }
    //使用
    QStringListModel* listModel = new QStringListModel(list);
    listView->setModel(listModel);  //设置模型到listview上

    //选中
    QItemSelectionModel* selectionModel = listView->selectionModel();
    int index = 0;
    for (const QJsonValue &value : userList) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            if (obj["pick"] == 1) {
                QModelIndex indexToSelect = listModel->index(index, 0); // 用你的实际行和列替换row和column
                selectionModel->select(indexToSelect, QItemSelectionModel::Select); // 设置项为选中状态
            }
        }
        index++;
    }

    

    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 禁用编辑触发方式
    listView->setSelectionMode(QAbstractItemView::MultiSelection);  // 启用多选
    listView->setSpacing(3);    //设置数据间隔

    //自定义滚动条
    QScrollBar* verticalScrollBar = new QScrollBar(Qt::Vertical, this);
    verticalScrollBar->setStyleSheet(
        "QScrollBar:vertical { background: #ECECEC; width: 6px; }"
        "QScrollBar::handle:vertical { background: #3498db; border-radius: 3px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background: #ECECEC; }"
    );

    listView->setVerticalScrollBar(verticalScrollBar);

}

bool  WeiboSpiders::onAddUidButtonClicked()
{
    //获取ULE中的文本
    QString uid = ui.uidLineEdit->text();

    //移除所有空格
    uid.remove(' ');

    //检查文本是否为空
    if (!uid.isEmpty()) {

        GetUserInfo(uid);

        //清空ULE
        ui.uidLineEdit->clear();
        return true;
    }
    else {
        //清空ULE
        ui.uidLineEdit->clear();
        return false;
    }
}

bool WeiboSpiders::WriteToFile(QJsonObject userJson)
{
    //打开一个文件以写入JSON数据
    QFile file("uidData.json");


    //读取文件数据
    QJsonArray dataArray = ReadFromFile();

    //获取uid
    QString uid = userJson["idstr"].toString();

    //检查是否已经存在相同的uid
    bool uidExists = false;
    for (const QJsonValue &value : dataArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            if (obj["uid"] == uid) {
                uidExists = true;
                break;
            }
        }
    }

    if (!uidExists) {
        //存在不相同的uid,追加数据
        QJsonObject newData;

        //网络请求博主信息


        if (!userJson.isEmpty())
        {
            QMessageBox::information(this, "你好", "你过了name");

            newData["uid"] = uid;
            newData["name"] = userJson["screen_name"];
            newData["pick"] = 1;

            dataArray.append(newData);

            //写入更新后的JSON数据到文件
            if (file.open(QFile::WriteOnly | QFile::Text)) {
                QJsonDocument updateDoc(dataArray);
                file.write(updateDoc.toJson());
                file.close();
                qDebug() << "数据已成功追加到文件.";
                return true;
            }
            else {
                qCritical() << "无法打开文件进行写入.";
                return false;
            }

        }

        

    }
    else {
        qDebug() << "相同的uid已存在，不进行增加.";
        return false;
    }





    return false;
}

QJsonArray WeiboSpiders::ReadFromFile()
{
    //打开文件以读取数据
    QFile file("uidData.json");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QByteArray data = file.readAll();
        file.close();

        //将文件数据解析为JSON数组
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isArray()) {
            QJsonArray dataArray = doc.array();
            return dataArray;
        }
    }

    // 如果doc.isArray()不成立或文件读取失败，返回一个空的QJsonArray
    return QJsonArray();

}

