#include "qsysteminfo.h"

QSystemNetworkInfo::QSystemNetworkInfo(QObject *parent) : QObject(parent)
{
	
}

QString QSystemNetworkInfo::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    QString networkName = "Unit test network";
    return networkName;
}
