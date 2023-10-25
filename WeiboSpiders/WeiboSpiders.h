#pragma once

#include <QtWidgets/QWidget>
#include "ui_WeiboSpiders.h"

//Config
#include "Config.h"

//QMouseEvent
#include <qevent.h>
//QMessageBox
#include <qmessagebox.h>
//QListView
#include <qlistview.h>
//QStringListModel
#include <qstringlistmodel.h>
//
#include <qscrollbar.h>
//QFilePath
#include <qfiledialog.h>

//网络
#include <qnetworkaccessmanager.h>//网络管理
#include <qnetworkreply.h>//网络应答
#include <qnetworkrequest.h>//网络请求

//JSON
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
//文件流
#include <qfile.h>
//QLabel
#include <qlabel.h>
//QPixmap
#include <qpixmap.h>
////QPainter
//#include <qpainter.h>


class WeiboSpiders : public QWidget
{
    Q_OBJECT

public:
    WeiboSpiders(QWidget *parent = nullptr);
    ~WeiboSpiders();

private:
    Ui::WeiboSpidersClass ui;



private:
    //记录鼠标左键点击时的位置
    QPoint m_prePoint;
    //记录鼠标左键是否按下状态
    bool m_islBtn{};
    //网络模块
    QNetworkAccessManager* manager;
    //设置窗口
    Config config;
    //当前选择用户
    QJsonObject currentUser{};

    //鼠标移动、点击和释放方法重写
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

    //用户列表渲染
    void UserListRender();
    //用户头像渲染
    void UserAvatarRender();


    //网络请求
    void GetUserInfo(QString uid); //获取uid用户信息
    
    //写文件
    bool WriteToFile(QJsonObject userJson);
    //读文件
    QJsonArray ReadFromFile();
    //文件路径获取
    QString GetFilePath();



    //获取Cookie
    QString GetCookie();
    
    

//槽函数
public slots:
    //打开Config窗口
    bool toConfig();
    //获取ULE内容
    bool onAddUidButtonClicked();
    //获取ListView点击事件
    void onItemClicked(const QModelIndex& index);



protected:
    //关闭窗口事件，重写：可以询问是否退出
    void closeEvent(QCloseEvent* e);

};
