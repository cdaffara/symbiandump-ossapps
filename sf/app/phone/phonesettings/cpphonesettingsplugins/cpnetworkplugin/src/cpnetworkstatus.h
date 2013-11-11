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

#ifndef CPNETWORKSTATUS_H
#define CPNETWORKSTATUS_H

#include <qobject.h>
#include <qsysteminfo.h>

using namespace QtMobility;

class CpSettingFormItemData;
class HbIcon;
class SettingsWrapper;

class CpNetworkStatus 
    : public QObject

{
    Q_OBJECT

public:
    
    CpNetworkStatus();
    
    ~CpNetworkStatus();
    
    QString statusText() const;
    
    HbIcon statusIcon() const;
    
    void setSettingFormItemData(CpSettingFormItemData *data);
    
public slots:

    void networkNameChanged(
            QSystemNetworkInfo::NetworkMode mode, 
            const QString &netName);
    
    void networkStatusChanged(
            QSystemNetworkInfo::NetworkMode mode, 
            QSystemNetworkInfo::NetworkStatus status);
    
private:
    
    QString statusIconLocicalName() const;
    
    bool connectedToNetwork() const;
    
private:
    
    QScopedPointer<QSystemNetworkInfo> m_networkInfo;
    QScopedPointer<QSystemDeviceInfo> m_deviceInfo;
    SettingsWrapper *m_cpSettingsWrapper;
        
    // Not own
    CpSettingFormItemData *m_settingFormItemData;
    
};

#endif  // CPNETWORKSTATUS_H
