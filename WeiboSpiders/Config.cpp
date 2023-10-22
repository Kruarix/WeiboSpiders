#include "Config.h"

Config::Config(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);


    //ȥ��������
    setWindowFlag(Qt::FramelessWindowHint);

    //���ô���͸����
    setWindowOpacity(0.98);


    //����
    connect(ui.applyButton, &QPushButton::clicked, this, &Config::ApplyConfig);

}

Config::~Config()
{}

void Config::mousePressEvent(QMouseEvent * e)
{
    //e->button()��ȡ����İ��������Ϊ������
    if (e->button() & Qt::LeftButton) {
        //����һ��״̬
        m_islBtn = true;
        //��¼����λ�ã��������Ļ��λ�� - ��������Ļ��λ�ã�
        m_prePoint = e->globalPos() - frameGeometry().topLeft();
    }
    //���Ϊ����Ҽ�
    else if (e->button() & Qt::RightButton)
    {
        //��������
    }
}

void Config::mouseMoveEvent(QMouseEvent* e)
{
    //�ж�
    if (m_islBtn) {
        //�ƶ���ȫ�����λ�� - ��Դ��ڵ����λ�� = ������ȫ�ֵ�λ�ã�
        move(e->globalPos() - m_prePoint);
    }
}

void Config::mouseReleaseEvent(QMouseEvent* e)
{
    //�������ͷ�
    if (e->button() & Qt::LeftButton) {
        m_islBtn = false;
    }
}

bool Config::SetCookie()
{
    //��ȡcookie
    QString cookie = ui.cookieLineEdit->text();

    WritePrivateProfileStringA("main", "cookie", (LPCSTR)cookie.toLocal8Bit(), ".\\config.ini");


    return true;
}

bool Config::SetStart()
{
    return false;
}

bool Config::SetDownload()
{
    return false;
}

void Config::ApplyConfig()
{
    SetCookie();


    this->close();

}

void Config::CancelConfig()
{
    this->close();
}
