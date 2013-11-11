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

#ifndef UT_INFOWIDGETNETWORKHANDLER_H
#define UT_INFOWIDGETNETWORKHANDLER_H

#include <QtTest/QtTest>
#include <mockservice.h>

class MNWMessageObserver;
class InfoWidgetNetworkHandler;
class TNWInfo;



class UT_InfoWidgetNetworkHandler : public QObject, public MockService
{
    Q_OBJECT

public:    
    
    UT_InfoWidgetNetworkHandler();
    ~UT_InfoWidgetNetworkHandler();
    
public slots:
    void generateException();
    void doLeaveL();
    
private slots:

    void init();
    void cleanup();
    
    void t_homeZoneTextTag();
    void t_homeZoneIndicatorType();
    void t_mcnName();
    void t_mcnIndicatorType();
    void t_isOnline();
    void t_networkRegistrationStatus();
    void t_HandleNetworkMessage();
    void t_HandleNetworkMessageExceptionWhileEmittingSignal();
    void t_HandleNetworkError();
    void t_HandleNetworkErrorExceptionWhileEmittingSignal();
    void t_resume();
    void t_suspend();
    void t_serviceProviderName();
    void t_serviceProviderNameDisplayRequired();
    void t_createLeave();
    
private:
    
    void initializeNetworkInfo(
        MNWMessageObserver& aMessage, 
        TNWInfo& aTNWInfo);
    
private:
    InfoWidgetNetworkHandler *m_networkHandler;
};

#endif // UT_INFOWIDGETNETWORKHANDLER_H
