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

#ifndef UT_CPBARRINGPLUGIN_H
#define UT_CPBARRINGPLUGIN_H

#include <QtTest/QtTest>
#include <mockservice.h>

class CpBarringPlugin;

class UT_CpBarringPlugin : public QObject, MockService
{
    Q_OBJECT

public:    
    UT_CpBarringPlugin();
    ~UT_CpBarringPlugin();
    
private slots:

    void init();

    void t_createSettingFormItemData();
    
    void cleanup();
    
    void t_memleak();
private:
    CpBarringPlugin *m_barringplugin;

};

#endif // UT_CPBARRINGPLUGIN_H
