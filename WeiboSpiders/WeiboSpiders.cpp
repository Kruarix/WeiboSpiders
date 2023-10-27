#include "WeiboSpiders.h"


WeiboSpiders::WeiboSpiders(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    //设置全局
    //当前时间
    QDate currentDate = QDate::currentDate();
    ui.startDate->setDate(currentDate);
    ui.endDate->setDate(currentDate);

    //删除框
    deleteDialogShown = false;

    //设置定时器
    rqTimer = new QTimer(this);
    rqTimer->setSingleShot(50);

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
    //下载按钮
    connect(ui.downloadButton, &QPushButton::clicked, this, &WeiboSpiders::onDownloadButton);
    
    //Test

    

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
    manager = new QNetworkAccessManager(this);

    QNetworkRequest request;
    request.setUrl(QUrl("https://weibo.com/ajax/profile/info?uid="+uid)); // 设置请求的URL
    // 如果需要设置HTTP请求头，可以使用request.setHeader()方法
    // 设置Cookie，将你的Cookie字符串替换成实际的Cookie值
    
    QByteArray cookie = GetCookie().toUtf8();

    request.setRawHeader("Cookie", cookie);


    QNetworkReply* reply = manager->get(request);
    


    // 连接finished信号以处理响应
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {

            // 在这里处理data，例如解析JSON等
            QJsonObject mainObj = QJsonDocument::fromJson(reply->readAll()).object();

            //
            /*QJsonDocument jsonDoc(mainObj);
            QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);
            QMessageBox::information(this, "reply", jsonString);*/

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
        manager->clearAccessCache();

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

    listView->setContextMenuPolicy(Qt::CustomContextMenu);   //自定义右键菜单
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 禁用编辑触发方式
    listView->setSpacing(3);    //设置数据间隔

    //自定义滚动条
    QScrollBar* verticalScrollBar = new QScrollBar(Qt::Vertical, this);
    verticalScrollBar->setStyleSheet(
        "QScrollBar:vertical { background: #ECECEC; width: 6px; }"
        "QScrollBar::handle:vertical { background: #3498db; border-radius: 3px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background: #ECECEC; }"
    );

    listView->setVerticalScrollBar(verticalScrollBar);

    // 连接点击信号到槽函数
    connect(listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onItemClicked(const QModelIndex&)));

    // 右键删除
    connect(listView, &QListView::customContextMenuRequested, [this, listView, listModel](const QPoint& pos) {
        QModelIndex index = listView->indexAt(pos);
        if (index.isValid()) {

            if (!deleteDialogShown) {
                QString itemText = index.data().toString();

                // 在这里添加确认删除的对话框
                int result = QMessageBox::question(nullptr, "确认", "你正在删除: " + itemText, QMessageBox::Yes | QMessageBox::No);
                if (result == QMessageBox::Yes) {
                    listModel->removeRow(index.row());
                    RemoveUid(itemText);
                    UserListRender();
                }

                deleteDialogShown = true;
            }

            
        }
        });

    connect(listView, &QListView::entered, [this]() {
        deleteDialogShown = false;
        });

}

QString WeiboSpiders::GetFilePath()
{
    char buf[0xff];
    GetPrivateProfileStringA("main", "filePath", "none", buf, 0xff, ".\\config.ini");

    QString filePath = QString::fromUtf8(buf);
    if (filePath.isEmpty()) {
        return QString("none");
    }
    else {
        return filePath;
    }

}

void WeiboSpiders::UserAvatarRender()
{
    
    if (!currentUser.isEmpty()) {

        manager = new QNetworkAccessManager(this);

        QNetworkRequest request;
        request.setUrl(QUrl(currentUser["avatar"].toString())); // 设置请求的URL
        // 如果需要设置HTTP请求头，可以使用request.setHeader()方法
        // 设置Cookie，将你的Cookie字符串替换成实际的Cookie值

        QByteArray cookie = GetCookie().toUtf8();

        request.setRawHeader("Cookie", cookie);
        
        //反防爬虫机制
        //request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
        request.setRawHeader("User-Agent", "Apifox/1.0.0 (https://apifox.com)");
        request.setRawHeader("Accept", "*/*");
        request.setRawHeader("Host", "tvax2.sinaimg.cn");
        request.setRawHeader("Connection", "keep-alive");

        QNetworkReply* reply = manager->get(request);

        // 连接finished信号以处理响应
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {

                // 从网络响应中读取图像数据
                QByteArray imageData = reply->readAll();

                // 加载图像数据到 QPixmap
                QPixmap pixmap;
                pixmap.loadFromData(imageData);

                if (!pixmap.isNull()) {

                    // 调整QLabel以适应图片大小
                    ui.avatarImg->setScaledContents(true);

                    // 设置图片
                    ui.avatarImg->setPixmap(pixmap);

                    // 设置名称
                    ui.nameShow->setText(currentUser["name"].toString());
                }
                else {
                    qWarning("无法加载图片");
                }
        
                

            }
            else {
                QMessageBox::information(this, "网络错误", "获取用户头像失败.");
                QMessageBox::information(this, "t", reply->readAll());
                qDebug() << "ok";
            }
            reply->deleteLater(); // 释放资源
            manager->clearAccessCache();

            // 请求完成后执行需要的操作
            });

    }
}

QString WeiboSpiders::GetCookie()
{
    char buf[0xfff];
    GetPrivateProfileStringA("main", "cookie", "", buf, 0xfff, ".\\config.ini");

    QString cookie = QString::fromUtf8(buf);

    return cookie;
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

void WeiboSpiders::onDownloadButton()
{
    //禁用下载按钮
    ui.downloadButton->setEnabled(false);

    //检查路径
    QString filePath = GetFilePath();

    QDir directory(filePath);

    if (directory.exists()) {
        qDebug() << "Directory exists and the path is valid.";
    }
    else {
        QMessageBox::information(this, "tips", "文件夹路径有问题");
        ui.downloadButton->setEnabled(true);
        return;
    }

    if ( currentUser.isEmpty()) {
        QMessageBox::information(this, "tips", "你还没有选择用户.");
        ui.downloadButton->setEnabled(true);
        return;
    }


    //设置日期范围
    startDate = ui.startDate->date();
    endDate = ui.endDate->date();
    //初始化计数
    fileNum = 0;
    ui.fileNum->setText(QString::number(0));
    currentNum = 0;
    ui.downloadNum->setText(QString::number(0));
    errorNum = 0;
    ui.errorNum->setText(QString::number(0));
    //初始化进度条
    ui.progressBar->setValue(0);
    proc = 0;
    //初始化图片列表
    photoList.clear();
    //获取图片地址与下载
    catchPicInfo();

}

void WeiboSpiders::onItemClicked(const QModelIndex& index)
{
    // 获取用户点击的项的数据
    QString selectedItem = index.data(Qt::DisplayRole).toString();

    // 在这里执行你希望的操作，例如显示选中项的详细信息或处理其他逻辑
    // 你可以使用selectedItem变量来访问选中的项的数据

    //读取文件
    QJsonArray uidList = ReadFromFile();
    //根据名称查找
    for ( const QJsonValue &value : uidList) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            if (obj["name"] == selectedItem) {
                //设置当前选中
                currentUser = obj;
                
            }
        }
    }

    //获取选中头像
    UserAvatarRender();


}

void WeiboSpiders::catchPicInfo()
{

    //判断合法性
    if (startDate > endDate) {
        QMessageBox::warning(this, "警告", "日期设定不合法.");
    }
    else {

        //获取下载数据
        getImageWall(QString("0"));
        

    }

}

void WeiboSpiders::getImageWall(QString sinceId)
{
    //网络请求管理
    manager = new QNetworkAccessManager(this);
    QNetworkRequest request;

    QString uid = currentUser["uid"].toString();

    QString url{};
    if (sinceId == "0") {
        url = "https://weibo.com/ajax/profile/getImageWall?uid=" + uid + "&sinceid=0&has_album=true";
    }
    else {
        url = "https://weibo.com/ajax/profile/getImageWall?uid=" + uid + "&sinceid=" + sinceId;
    }


    request.setUrl(QUrl(url)); // 设置请求的URL
    // 设置Cookie，将你的Cookie字符串替换成实际的Cookie值
    QByteArray cookie = GetCookie().toUtf8();

    request.setRawHeader("Cookie", cookie);

    //反防爬虫机制
    //request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
    request.setRawHeader("User-Agent", "Apifox/1.0.0 (https://apifox.com)");
    request.setRawHeader("Accept", "application/json, text/plain, */*");
    request.setRawHeader("Host", "tvax2.sinaimg.cn");
    request.setRawHeader("Connection", "keep-alive");

    QNetworkReply* reply = manager->get(request);

    // 连接finished信号以处理响应
    connect(reply, &QNetworkReply::finished, this, [this, reply, sinceId]() {
        if (reply->error() == QNetworkReply::NoError) {

            //JSON格式
            QJsonObject mainObj = QJsonDocument::fromJson(reply->readAll()).object();

            if (mainObj.contains("data")) {

                QJsonObject dataObj = mainObj["data"].toObject();

                
                if (dataObj.contains("since_id")) {
                    QString nextSinceId = dataObj["since_id"].toString();

                    //正则表达式
                    // 使用正则表达式匹配日期部分
                    QRegularExpression regex("_(\\d{8})_-1"); // 匹配8位数字
                    QRegularExpressionMatch match = regex.match(nextSinceId);

                    if (match.hasMatch()) {
                        QString matchedText = match.captured(1);

                        qDebug() << "匹配的字段：" << matchedText;


                        if (matchedText.length() == 8) {
                            QDate date = QDate::fromString(matchedText, "yyyyMMdd");
                            if (date.isValid()) {
                                // 现在 "date" 包含了提取的日期
                                qDebug() << "提取的日期：" << date;

                                //判断日期范围
                                if ( startDate <= date && date <= endDate  ) {

                                    if (dataObj.contains("list")) {

                                        QJsonArray listArray = dataObj["list"].toArray();

                                        for (const QJsonValue& value : listArray) {
                                            if (value.isObject()) {
                                                QJsonObject item = value.toObject();

                                                PhotoInfo pInfo(item["pid"].toString(), item["mid"].toString());

                                                //加入缓存列表
                                                photoList.append(pInfo);
                                                //计数
                                                fileNum++;
                                            }
                                        }

                                    }

                                    ui.fileNum->setText(QString::number(fileNum));
                                    //执行下一分页
                                    qDebug() << "执行下一分页" << nextSinceId;
                                    rqTimer->start();
                                    rqTimer->singleShot(5, this, [this, nextSinceId]() {
                                        getImageWall(nextSinceId);
                                        rqTimer->stop();
                                        });
                                   



                                }
                                else
                                {
                                    //跳出循环
                                    //调用下载函数
                                    qDebug() << "进入下载";
                                    downloadPic();
                                }


                            }
                            else {
                                qDebug() << "日期无效。";
                            }
                        }
                        else {
                            qDebug() << "找到的日期部分不是8位数字。";
                        }
                    }
                    else {
                        qDebug() << "未找到日期部分。进入下载";
                        downloadPic();
                    }
                    
                }
            
            }



        }
        else {
            /*QMessageBox::information(this, "网络错误", "获取相册数据失败.");
            QMessageBox::information(this, "t", reply->readAll());*/
            qDebug() << "获取相册数据失败.";
            rqTimer->start();
            rqTimer->singleShot(50, this, [this, sinceId]() {
                getImageWall(sinceId);
                rqTimer->stop();
                });

        }
        reply->deleteLater(); // 释放资源
        // 请求完成后执行需要的操作
        });

}

void WeiboSpiders::downloadPic()
{
    //QMessageBox::information(this, "下载下载", QString::number(fileNum));

    //初始化下载数
    currentNum = 0;
    errorNum = 0;

    //获取用户名
    QString currentName = currentUser["name"].toString();

    QString folderPath = GetFilePath() + "/" + currentName;

    //创建新文件夹
    QDir currentDir(folderPath);
    if (!currentDir.exists()) {
        currentDir.mkpath(".");
    }

    //循环下载
    for(const PhotoInfo &info : photoList) {
        savePic(info, folderPath, 1);
    }

}

void WeiboSpiders::RemoveUid(QString name)
{
    //打开一个文件以写入JSON数据
    QFile file("uidData.json");

    //读取文件数据
    QJsonArray dataArray = ReadFromFile();


    // 遍历 JSON 数组，查找要删除的元素
    for (int i = 0; i < dataArray.size(); ++i) {
        QJsonObject jsonObject = dataArray[i].toObject();

        if (name == jsonObject["name"].toString()) {
            // 找到匹配的元素，删除它
            dataArray.removeAt(i);
            qDebug() << "Deleted element with name: " << name;
            break;  // 如果只想删除一个匹配的元素，可以使用break
        }
    }

    //写入更新后的JSON数据到文件
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QJsonDocument updateDoc(dataArray);
        file.write(updateDoc.toJson());
        file.close();
        qDebug() << "数据已成功追加到文件.";
    }
    else {
        qCritical() << "无法打开文件进行写入.";
    }

}

void WeiboSpiders::savePic(PhotoInfo info, QString folderPath, int retryCount)
{
    manager = new QNetworkAccessManager(this);

    QNetworkRequest request;

    QString mid = info.GetMid();
    QString subFolderPath = folderPath + "/" + mid; // 子文件夹路径

    // 创建子文件夹
    QDir subDir(subFolderPath);
    if (!subDir.exists()) {
        subDir.mkpath(".");
    }

    // 使用 info.GetPid() 下载文件并保存到子文件夹
    // 请根据您的网络请求方式和文件保存逻辑来实现下载

    QString uid = currentUser["uid"].toString();

    QString pid = info.GetPid();

    QString url = "https://wx1.sinaimg.cn/large/" + pid + ".jpg";

    request.setUrl(QUrl(url)); // 设置请求的URL
    // 设置Cookie，将你的Cookie字符串替换成实际的Cookie值
    QByteArray cookie = GetCookie().toUtf8();

    request.setRawHeader("Cookie", cookie);

    //反防爬虫机制
    //request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
    request.setRawHeader("User-Agent", "Apifox/1.0.0 (https://apifox.com)");
    request.setRawHeader("Accept", "application/json, text/plain, */*");
    request.setRawHeader("Host", "tvax2.sinaimg.cn");
    request.setRawHeader("Connection", "keep-alive");

    QNetworkReply* reply = manager->get(request);

    // 连接finished信号以处理响应
    connect(reply, &QNetworkReply::finished, this, [this, info, reply, pid, subFolderPath, folderPath, retryCount]() {
        int reC = retryCount;

        if (reply->error() == QNetworkReply::NoError) {

            QByteArray imageData = reply->readAll();
            QString localFilePath = subFolderPath + "/" + pid + ".jpg"; // 本地文件路径

            QFile localFile(localFilePath);
            if (localFile.open(QIODevice::WriteOnly)) {
                localFile.write(imageData);
                localFile.close();

                currentNum++;
                ui.downloadNum->setText(QString::number(currentNum));

                qDebug() << "图片已下载到本地：" << localFilePath;

            }
            else {
                qDebug() << "无法打开本地文件：" << localFilePath;
            }

            reply->deleteLater(); // 释放资源

        }
        else {

            // 处理错误的响应
            if (reC < 5) {
                // 如果重试次数小于5，发起重试
                savePic(info, folderPath, reC++);
            }
            else {
                // 重试达到5次，将照片存入txt文件
                errorNum++;
                ui.errorNum->setText(QString::number(errorNum));

                QString errorFilePath = subFolderPath + "/" + pid + ".txt"; // 本地文件路径
                QString imgUrl = "https://wx1.sinaimg.cn/large/" + pid + ".jpg";
                QByteArray imgData = imgUrl.toUtf8(); // 将字符串转换为字节数组

                QFile errorFile(errorFilePath);
                if (errorFile.open(QIODevice::WriteOnly)) {
                    errorFile.write(imgData);
                    errorFile.close();
                    qDebug() << "错误图片已缓存到本地：" << errorFilePath;
                }
                else {
                    qDebug() << "无法打开本地文件：" << errorFilePath;
                }
            }
            reply->deleteLater(); // 释放资源

        }
        reply->deleteLater(); // 释放资源

        proc = (double)(currentNum + errorNum) / fileNum * 100;
        ui.progressBar->setValue(proc);


        if (currentNum + errorNum == fileNum) {
            QMessageBox::information(this, "通知", "下载完毕！");
            //启用下载按钮
            ui.downloadButton->setEnabled(true);

            rqTimer->start();
            rqTimer->singleShot(1000, this, [this]() {
                manager->clearAccessCache();
                rqTimer->stop();
                });

        }


        });

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

            newData["uid"] = uid;
            newData["name"] = userJson["screen_name"];
            newData["pick"] = 0;
            newData["avatar"] = userJson["avatar_large"];

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

