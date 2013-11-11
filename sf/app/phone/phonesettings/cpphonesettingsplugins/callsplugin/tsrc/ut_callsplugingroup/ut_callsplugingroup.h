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

#ifndef UT_CALLSPLUGINGROUP_H
#define UT_CALLSPLUGINGROUP_H

#include <QtTest/QtTest>
#include <mockservice.h>


class CpCallsPluginGroup;

class UT_CpCallsPluginGroup : public QObject, MockService
{
    Q_OBJECT

public:    
    UT_CpCallsPluginGroup();
    ~UT_CpCallsPluginGroup();
    
private slots:

    void init();

    void t_showCallDurationStateChanged();
    
    void t_callWaitingCurrentIndexChanged();
    
    void t_cliCurrentIndexChanged();
    
    void t_handleCallWaitingChanged();
    
    void t_handleCallWaitingRequesting();
    
    void t_handleCallWaitingError();
    
    void t_softRejectTextChanged();
    
    void t_handleCallWaitingGetStatus();
    
    void t_handleCallWaitingGetStatusDistinguishEnabled();
    
    void t_ownVideoInReceivedCallStateChanged();
    
    void cleanup();
    
    void t_memleak();
    
private:
    CpCallsPluginGroup *m_callspluginGroup;

};

#endif // UT_CALLSPLUGINGROUP_H
