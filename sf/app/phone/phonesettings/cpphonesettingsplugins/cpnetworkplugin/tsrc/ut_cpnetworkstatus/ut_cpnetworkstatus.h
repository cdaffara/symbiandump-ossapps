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

#ifndef UT_CPNETWORKSTATUS_H
#define UT_CPNETWORKSTATUS_H

#include <QtTest/QtTest>
#include <mockservice.h>

class CpNetworkStatus;

class ut_CpNetworkStatus : public QObject, MockService
{
    Q_OBJECT

public:    
    ut_CpNetworkStatus();
    ~ut_CpNetworkStatus();
    
private slots:

    void init();
    
    void t_memleak();

    void t_statusText();
    
    void t_statusIcon();
    
    void t_networkNameChanged();
    
    void t_networkStatusChanged();
    
    void t_setSettingFormItemData();
    
    void cleanup();
    
private:
    
    CpNetworkStatus *m_networkStatus;

};

#endif // UT_CPNETWORKSTATUS_H
