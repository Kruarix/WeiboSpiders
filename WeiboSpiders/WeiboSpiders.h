#pragma once

#include <QtWidgets/QWidget>
#include "ui_WeiboSpiders.h"

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

//网络

//JSON
#include <qjsondocument.h>
#include <qjsonobject.h>
//文件流
#include <qfile.h>


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

    //鼠标移动、点击和释放方法重写
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

    //网络请求


//槽函数
public slots:
    //写文件
    bool WriteToFile(QString uid);
    //读文件
    bool ReadFromFile();
    //获取ULE内容
    bool onAddUidButtonClicked();



protected:
    //关闭窗口事件，重写：可以询问是否退出
    void closeEvent(QCloseEvent* e);

};
