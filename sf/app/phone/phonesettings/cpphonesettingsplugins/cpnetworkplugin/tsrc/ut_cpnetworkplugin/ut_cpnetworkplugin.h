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

#ifndef UT_CPNETWORKPLUGIN_H
#define UT_CPNETWORKPLUGIN_H

#include <QtTest/QtTest>
#include <mockservice.h>

class CpNetworkPlugin;

class UT_CpNetworkPlugin : public QObject, MockService
{
    Q_OBJECT

public:    
    UT_CpNetworkPlugin();
    ~UT_CpNetworkPlugin();
    
private slots:

    void init();
    
    void t_memleak();

    void t_createSettingFormItemData();
    
    void t_createSettingView();
    
    void cleanup();
    
private:
    CpNetworkPlugin *m_networkPlugin;

};

#endif // UT_CPNETWORKPLUGIN_H
