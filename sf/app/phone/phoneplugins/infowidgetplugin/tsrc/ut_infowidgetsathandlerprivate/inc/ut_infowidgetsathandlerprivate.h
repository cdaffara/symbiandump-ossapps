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

#ifndef UT_INFOWIDGETSATHANDLERPRIVATE_H
#define UT_INFOWIDGETSATHANDLERPRIVATE_H

#include <QtTest/QtTest>
#include <mockservice.h>
#include <rsatservice.h>


class InfoWidgetSatHandlerPrivate;
class InfoWidgetSatHandler;

class UT_InfoWidgetSatHandlerPrivate : public QObject, public MockService
{
    Q_OBJECT

public:    
    
    UT_InfoWidgetSatHandlerPrivate();
    ~UT_InfoWidgetSatHandlerPrivate();
    
private slots:

    void init();
    void cleanup();
    
    void t_connect();
    void t_disconnect();
    void t_startObserving();

    void t_runL();
    void t_doCancel();
    
private:
    
    InfoWidgetSatHandlerPrivate *m_satHandlerPrivate;
    InfoWidgetSatHandler* m_iwSatHandler;
    RSatService m_satService;

};

#endif // UT_INFOWIDGETSATHANDLERPRIVATE_H
