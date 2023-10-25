#pragma once

#include <QDialog>
#include "ui_Config.h"



//QMouseEvent
#include <qevent.h>
//ini存储
#include <iostream>
#include <Windows.h>
//QMessageBox
#include <qmessagebox.h>
//QFilePath
#include <qfiledialog.h>



class Config : public QDialog
{
	Q_OBJECT

public:
	Config(QWidget *parent = nullptr);
	~Config();

private:
	Ui::ConfigClass ui;



private:
    //记录鼠标左键点击时的位置
    QPoint m_prePoint;
    //记录鼠标左键是否按下状态
    bool m_islBtn{};

    //自动启动
    bool isAutoStart;
    //自动下载
    bool isAutoDownload;


    //鼠标移动、点击和释放方法重写
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);


    //设置获取Cookie
    void SetCookie();
    QString GetCookie();
    //设置获取自动启动
    bool SetStart();
    bool GetStart();
    //设置获取自动下载
    bool SetDownload();
    bool GetDownload();

    //渲染数据
    bool ConfigRender();

    //文件路径设置、获取
    void SetFilePath();
    QString GetFilePath();


    //应用
    void ApplyConfig();
    //取消
    void CancelConfig();

};
