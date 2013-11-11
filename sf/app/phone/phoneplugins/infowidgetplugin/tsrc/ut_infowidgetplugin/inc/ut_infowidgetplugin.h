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

#ifndef UT_INFOWIDGETPLUGIN_H
#define UT_INFOWIDGETPLUGIN_H

#include <QtTest/QtTest>
#include <mockservice.h>
#include <qserviceplugininterface.h>

QTM_USE_NAMESPACE


class UT_InfoWidgetPlugin : public QObject, public MockService
{
    Q_OBJECT

public:    
    
    UT_InfoWidgetPlugin();
    ~UT_InfoWidgetPlugin();
    
private slots:

    void init();
    void cleanup();
    void t_createInstance(); 
    
};

#endif // UT_INFOWIDGETPLUGIN_H
