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

#ifndef UT_INFOWIDGETSATHANDLER_H
#define UT_INFOWIDGETSATHANDLER_H

#include <QtTest/QtTest>
#include <mockservice.h>

class InfoWidgetSatHandler;

class UT_InfoWidgetSatHandler : public QObject, public MockService
{
    Q_OBJECT

public:    
    
    UT_InfoWidgetSatHandler();
    ~UT_InfoWidgetSatHandler();
    
private slots:

    void init();
    void cleanup();
    
    void t_satDisplayText(); 
    void t_setSatDisplayText();
    
    void t_connect();
    void t_handleIdleModeTxtMessage();
    void t_handleSatError();
    
private:
    
    InfoWidgetSatHandler *m_satHandler;

};

#endif // UT_INFOWIDGETSATHANDLER_H
