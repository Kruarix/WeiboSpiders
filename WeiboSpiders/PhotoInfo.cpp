#include "PhotoInfo.h"


PhotoInfo::PhotoInfo(const QString& pid, const QString& mid)
{
	this->pid = pid;
	this->mid = mid;
}

QString PhotoInfo::GetPid() const
{
	return pid;
}

QString PhotoInfo::GetMid() const
{
	return mid;
}

