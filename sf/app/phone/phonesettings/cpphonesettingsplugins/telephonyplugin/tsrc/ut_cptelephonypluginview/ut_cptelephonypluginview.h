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

#ifndef UT_CPTELEPHONYPLUGINVIEW_H
#define UT_CPTELEPHONYPLUGINVIEW_H

#include <QtTest/QtTest>
#include <mockservice.h>

class CpTelephonyPluginView;

class UT_CpTelephonyPluginView : public QObject, MockService
{
    Q_OBJECT

public:    
    UT_CpTelephonyPluginView();
    ~UT_CpTelephonyPluginView();
    
private slots:

    void init();

    void cleanup();
    
    void t_memleak();

private:
    CpTelephonyPluginView *m_callspluginview;

};

#endif // UT_CPTELEPHONYPLUGINVIEW_H
