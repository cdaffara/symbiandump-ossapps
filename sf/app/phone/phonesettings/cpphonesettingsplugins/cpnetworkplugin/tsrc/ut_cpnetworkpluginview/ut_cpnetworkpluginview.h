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

class CpNetworkPluginView;

class ut_cpnetworkpluginview : public QObject, MockService
{
    Q_OBJECT

public:    
    ut_cpnetworkpluginview ();
    ~ut_cpnetworkpluginview ();
    
private slots:

    void init();
    
    void t_memleak();
    
    void cleanup();
    
private:
    CpNetworkPluginView *m_cpnetworkpluginview;

};

#endif // UT_CPNETWORKPLUGIN_H
