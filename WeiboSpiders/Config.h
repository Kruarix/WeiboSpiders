#pragma once

#include <QDialog>
#include "ui_Config.h"



//QMouseEvent
#include <qevent.h>
//ini�洢
#include <iostream>
#include <Windows.h>



class Config : public QDialog
{
	Q_OBJECT

public:
	Config(QWidget *parent = nullptr);
	~Config();

private:
	Ui::ConfigClass ui;



private:
    //��¼���������ʱ��λ��
    QPoint m_prePoint;
    //��¼�������Ƿ���״̬
    bool m_islBtn{};

    //�Զ�����
    bool isAutoStart;
    //�Զ�����
    bool isAutoDownload;


    //����ƶ���������ͷŷ�����д
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);


    //����Cookie
    bool SetCookie();
    //�����Զ�����
    bool SetStart();
    //�����Զ�����
    bool SetDownload();

    //Ӧ��
    void ApplyConfig();
    //ȡ��
    void CancelConfig();

};
