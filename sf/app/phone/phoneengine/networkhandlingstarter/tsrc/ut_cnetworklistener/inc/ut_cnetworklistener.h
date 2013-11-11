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

#ifndef UT_CNETWORKLISTENER_H
#define UT_CNETWORKLISTENER_H

#include <QtTest/QtTest>
#include <mockservice.h>
#include <mnetworklistenerobserver.h>
#include <mock_cpsetrefreshhandler.h>

class CNetworkListener;
class CPSetRefreshHandler;
class CPsetNetwork;

class ut_cnetworklistener : public QObject, public MockService, public MNetworkListenerObserver
{
    Q_OBJECT

public:
    
    ut_cnetworklistener();
    ~ut_cnetworklistener();
    
public: 
    // From MNetworkListenerObserver
    void ShowNote();
    void RemoveNote();
    
private slots:

    void init();
    void cleanup();
    
    void t_HandleNetworkMessage();
    void t_HandleNetworkError();
    void t_NWLostDelayCallBack();
    void t_NWSimRefreshCallBack();
    void t_AllowRefresh();
    void t_Refresh();
    void t_HandleNetworkInfoReceivedL();
    void t_HandleCurrentNetworkInfoL();
    void t_HandleNetworkChangedL_1();
    void t_HandleNetworkChangedL_2();
    void t_HandleSearchingNetworksL();
    void t_HandleRequestingSelectedNetworkL();
    void t_HandleCallActivatedL();
    void t_HandleNetworkErrorL();
    
private:
    
    CNetworkListener *m_networkListener;
    CPSetRefreshHandler *m_refreshHandler;
    CPsetNetwork* m_settingsEngine;
    

};

#endif // UT_CNETWORKLISTENER_H
