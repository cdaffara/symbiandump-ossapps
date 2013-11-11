/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// THIS IS A STUB CLASS FOR WIN32 ENVIRONMENT!
// It is only meant to compile, not to actually do anything

#ifndef Q_SYSTEMINFO_H
#define Q_SYSTEMINFO_H

#include <QObject>

namespace QtMobility {
    
class QSystemDeviceInfo : public QObject
{
    Q_OBJECT
public:

    QSystemDeviceInfo( QObject* parent = 0 );
    virtual ~QSystemDeviceInfo();

    enum Profile { GeneralProfile, OfflineProfile };
    Profile currentProfile() { return GeneralProfile; }

signals:

    void currentProfileChanged(QSystemDeviceInfo::Profile);
    
};


}

#endif // Q_SYSTEMINFO_H 
