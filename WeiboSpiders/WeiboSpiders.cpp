#include "WeiboSpiders.h"



WeiboSpiders::WeiboSpiders(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    //设置全局编码
    

    //去掉标题栏
    setWindowFlag(Qt::FramelessWindowHint);
    //保持窗口大小不变(去掉标题框其实已经把拉伸功能去掉了)
    /*setMaximumSize(750, 450);
    setMinimumSize(750, 450);*/

    //设置窗口透明度
    setWindowOpacity(0.98);

    //测试
    QListView* listView = ui.listView;  //创建QListVIew对象
    QStringList list;   //创建数据显示列表
    for (int i = 0; i < 5; i++) {
        list.append("index");
    }


    //使用
    QStringListModel* listModel = new QStringListModel(list);
    listView->setModel(listModel);  //设置模型到listview上

    listView->setSpacing(3);    //设置数据间隔

    //自定义滚动条
    QScrollBar* verticalScrollBar = new QScrollBar(Qt::Vertical, this);
    verticalScrollBar->setStyleSheet(
        "QScrollBar:vertical { background: #ECECEC; width: 6px; }"
        "QScrollBar::handle:vertical { background: #3498db; border-radius: 3px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background: #ECECEC; }"
    );

    listView->setVerticalScrollBar(verticalScrollBar);



    //链接
    connect(ui.addUidButton, &QPushButton::clicked, this, &WeiboSpiders::onAddUidButtonClicked);

}

WeiboSpiders::~WeiboSpiders()
{}

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

    if (result == QMessageBox::Yes)
        e->accept();
    else
        e->ignore();

}

bool  WeiboSpiders::onAddUidButtonClicked()
{
    //获取ULE中的文本
    QString uid = ui.uidLineEdit->text();

    //移除所有空格
    uid.remove(' ');

    //检查文本是否为空
    if (!uid.isEmpty()) {

        WriteToFile(uid);

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

bool WeiboSpiders::WriteToFile(QString uid)
{
    ////创建一个JSON对象
    //QJsonObject jsonObject;
    //jsonObject["uid"] = uid;
    //jsonObject["time"] = "1034:4951349381759059_20231019_-1";

    ////将JSON对象转为QJsonDocument
    //QJsonDocument jsonDoc(jsonObject);

    //打开一个文件以写入JSON数据
    QFile file("uidData.json");
    //if (file.open(QFile::WriteOnly | QFile::Text)) {
    //    file.write(jsonDoc.toJson());
    //    file.close();
    //    qDebug() << "JSON数据已成功写入文件.";
    //}
    //else
    //{
    //    qCritical() << "无法打开文件进行写入.";
    //}

    //读取文件数据
    QJsonArray dataArray = ReadFromFile();

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


        newData["uid"] = uid;
        newData["name"] = "测试daw@da|;;dkaw";

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
