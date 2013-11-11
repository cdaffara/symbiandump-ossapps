/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#ifndef QSYSTEMINFO_H
#define QSYSTEMINFO_H

#include <QObject>

namespace QtMobility {
	
}

class QSystemNetworkInfo : public QObject
{
    Q_OBJECT

public:

    enum NetworkMode {
        UnknownMode=0,
        GsmMode,
        CdmaMode,
        WcdmaMode,
        WlanMode,
        EthernetMode,
        BluetoothMode,
        WimaxMode
    };
    
    QSystemNetworkInfo (QObject *parent = 0);
    virtual ~QSystemNetworkInfo () {};
    static QString networkName(QSystemNetworkInfo::NetworkMode mode);
    	
signals:
	  void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);

private:
    
};

#endif // QSYSTEMINFO_H
