#include "Config.h"

Config::Config(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);


    //去掉标题栏
    setWindowFlag(Qt::FramelessWindowHint);

    //设置窗口透明度
    setWindowOpacity(0.98);
    
    

    ConfigRender();

    //链接
    //应用按钮
    connect(ui.applyButton, &QPushButton::clicked, this, &Config::ApplyConfig);
    //取消按钮
    connect(ui.cancelButton, &QPushButton::clicked, this, &Config::CancelConfig);
    //文件路径设置
    connect(ui.filePathButton, &QPushButton::clicked, this, &Config::SetFilePath);

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

void Config::SetCookie()
{
    //获取cookie
    QString cookie = ui.cookieLineEdit->text();

    WritePrivateProfileStringA("main", "cookie", (LPCSTR)cookie.toLocal8Bit(), ".\\config.ini");


}

QString Config::GetCookie()
{
    char buf[0xfff];
    GetPrivateProfileStringA("main", "cookie", "none", buf, 0xfff, ".\\config.ini");

    QString cookie = QString::fromUtf8(buf);

    return cookie;
}

bool Config::SetStart()
{
    return false;
}

bool Config::GetStart()
{
    return false;
}

bool Config::SetDownload()
{
    return false;
}

bool Config::GetDownload()
{
    return false;
}

bool Config::ConfigRender()
{
    QString cookie = GetCookie();
    //设置cookie
    ui.cookieLineEdit->setText(cookie);
    QString filePath = GetFilePath();
    ui.filePathLineEdit->setText(filePath);

    return false;
}

void Config::SetFilePath()
{
    QString filePath = QFileDialog::getExistingDirectory(this, "选择文件路径", QDir::homePath());

    if (!filePath.isEmpty()) {
        // 在QLineEdit中显示选择的文件路径
        ui.filePathLineEdit->setText(filePath);

        WritePrivateProfileStringA("main", "filePath", (LPCSTR)filePath.toLocal8Bit(), ".\\config.ini");

    }
}

QString Config::GetFilePath()
{

    char buf[0xfff];
    GetPrivateProfileStringA("main", "filePath", "none", buf, 0xfff, ".\\config.ini");

    QString filePath = QString::fromUtf8(buf);

    return filePath;
}

void Config::ApplyConfig()
{
    //设置Cookie
    SetCookie();



    this->close();

}

void Config::CancelConfig()
{
    this->close();
}
