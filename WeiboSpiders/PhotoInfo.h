#pragma once

#include <qstring.h>

class PhotoInfo
{
public:
	PhotoInfo(const QString &pid, const QString &mid);
	QString GetPid() const; 
	QString GetMid() const;

private:
    QString pid;
    QString mid;
};

