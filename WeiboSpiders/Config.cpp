#include "Config.h"

Config::Config(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);


    //去掉标题栏
    setWindowFlag(Qt::FramelessWindowHint);

    //设置窗口透明度
    setWindowOpacity(0.98);


    //链接
    connect(ui.applyButton, &QPushButton::clicked, this, &Config::ApplyConfig);

}

Config::~Config()
{}

void Config::mousePressEvent(QMouseEvent * e)
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

void Config::mouseMoveEvent(QMouseEvent* e)
{
    //判断
    if (m_islBtn) {
        //移动（全局鼠标位置 - 相对窗口的鼠标位置 = 窗口在全局的位置）
        move(e->globalPos() - m_prePoint);
    }
}

void Config::mouseReleaseEvent(QMouseEvent* e)
{
    //鼠标左键释放
    if (e->button() & Qt::LeftButton) {
        m_islBtn = false;
    }
}

bool Config::SetCookie()
{
    //获取cookie
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
