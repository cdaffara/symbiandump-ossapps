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

#ifndef UT_CALLSPLUGIN_H
#define UT_CALLSPLUGIN_H

#include <QtTest/QtTest>
#include <mockservice.h>

class CpCallsPlugin;

class UT_CpCallsPlugin : public QObject, MockService
{
    Q_OBJECT

public:    
    UT_CpCallsPlugin();
    ~UT_CpCallsPlugin();
    
private slots:

    void init();

    void t_createSettingFormItemData();
    
    void cleanup();
    
    void t_memleak();
private:
    CpCallsPlugin *m_callsplugin;

};

#endif // UT_CALLSPLUGIN_H
