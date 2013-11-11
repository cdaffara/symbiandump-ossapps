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
#include <ut_cnetworklistener.h>
#include <qtestmains60.h>
#include <cnetworklistener.h>
#include <networkhandlingproxy.h>
#include <cnwsession.h>
#include <nwhandlingengine.h>
#include <mock_cpsetrefreshhandler.h>
#include <psetnetwork.h> 

TNWInfo* PoiterNWInfo;

CNWSession* CreateL( MNWMessageObserver& /*aMessage*/, TNWInfo& aTNWInfo )
{
    PoiterNWInfo = &aTNWInfo;
    aTNWInfo.iSelectionSetting = ENWNetworkSelectionManual;
    aTNWInfo.iRegistrationStatus = ENWRegistrationDenied;
    CNWSession* nullPointer(NULL);
    return nullPointer;
}

TPSetChangedCspSetting g_changedCspSettings;
TPSetChangedCspSetting g_newValues;


void ChangedCspSettings( 
        TPSetChangedCspSetting & aChangedCspSettings,
        TPSetChangedCspSetting & aNewValues )
{
    aChangedCspSettings = g_changedCspSettings;
    aNewValues = g_newValues;
}

MPsetNetworkSelect::TSelectMode g_mode;
void GetNetworkSelectMode( 
        MPsetNetworkSelect::TSelectMode & aMode )
{
    aMode = g_mode;
}

/*!
  ut_cnetworklistener::ut_cnetworklistener
 */
ut_cnetworklistener::ut_cnetworklistener() 
    : m_networkListener(0)
{
}

/*!
  ut_cnetworklistener::~ut_cnetworklistener
 */
ut_cnetworklistener::~ut_cnetworklistener()
{
    delete m_networkListener;
}

/*!
  ut_cnetworklistener::init
 */
void ut_cnetworklistener::init()
{
    initialize();
    
    m_refreshHandler = CPSetRefreshHandlerMock::NewL();
    QT_TRAP_THROWING(SmcDefaultValue<CPSetRefreshHandler *>::SetL(m_refreshHandler));
    RMobilePhone *phone(NULL); 
    MPsetNetworkInfoObserver *observer(NULL);
    m_settingsEngine = CPsetNetwork::NewL(*phone, *observer);
    QT_TRAP_THROWING(SmcDefaultValue<CPsetNetwork *>::SetL(m_settingsEngine));
    m_networkListener = CNetworkListener::NewL(*this);
    
    QT_TRAP_THROWING(SmcDefaultValue<QSystemDeviceInfo::Profile>::SetL(
        QSystemDeviceInfo::NormalProfile))
    
    QVERIFY(verify());
}

/*!
  ut_cnetworklistener::cleanup
 */
void ut_cnetworklistener::cleanup()
{
    delete m_networkListener;
    m_networkListener = NULL;
    
    SmcDefaultValue<CPSetRefreshHandler *>::Reset();
    // CNetworkListener delete m_refreshHandler object
    m_refreshHandler = NULL;
    
    SmcDefaultValue<CPsetNetwork *>::Reset();
    // CNetworkListener delete m_settingsEngine object
    m_settingsEngine = NULL;
    
    SmcDefaultValue<QSystemDeviceInfo::Profile>::Reset();
    
    reset();
}

/*!
    From MNetworkListenerObserver
 */
void ut_cnetworklistener::ShowNote()
{
}

/*!
    From MNetworkListenerObserver
 */
void ut_cnetworklistener::RemoveNote()
{
}

/*!
  ut_cnetworklistener::t_HandleNetworkMessage
 */
void ut_cnetworklistener::t_HandleNetworkMessage()
{
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    
    PoiterNWInfo->iRegistrationStatus = ENWNotRegisteredNoService;
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    
    PoiterNWInfo->iRegistrationStatus = ENWNotRegisteredEmergencyOnly;
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    PoiterNWInfo->iRegistrationStatus = ENWNotRegisteredSearching;
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    
    PoiterNWInfo->iRegistrationStatus = ENWRegisteredBusy;
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    
    PoiterNWInfo->iRegistrationStatus = ENWRegisteredOnHomeNetwork;
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    
    PoiterNWInfo->iRegistrationStatus = ENWRegisteredRoaming;
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    
    PoiterNWInfo->iRegistrationStatus = ENWRegistrationUnknown;
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    
    PoiterNWInfo->iSelectionSetting = ENWNetworkSelectionAutomatic;
    PoiterNWInfo->iRegistrationStatus = ENWNotRegisteredNoService;
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    PoiterNWInfo->iRegistrationStatus = ENWRegisteredOnHomeNetwork;
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkModeChange);
    
    PoiterNWInfo->iRegistrationStatus = (TNWRegistrationStatus)123456789;
    m_networkListener->HandleNetworkMessage(MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);  
}

/*!
  ut_cnetworklistener::t_HandleNetworkError
 */
void ut_cnetworklistener::t_HandleNetworkError()
{
    m_networkListener->HandleNetworkError( 
        MNWMessageObserver::ENWGetServiceProviderName, KErrGeneral );
}

/*!
  ut_cnetworklistener::t_NWLostDelayCallBack
 */
void ut_cnetworklistener::t_NWLostDelayCallBack()
{
    TAny* pointer(NULL); 
    m_networkListener->NWLostDelayCallBack(pointer);
    
    m_networkListener->NWLostDelayCallBack(m_networkListener);
}

/*!
  ut_cnetworklistener::t_NWSimRefreshCallBack
 */
void ut_cnetworklistener::t_NWSimRefreshCallBack()
{
    TAny* pointer(NULL); 
    m_networkListener->NWSimRefreshCallBack(pointer);
    
    m_networkListener->NWSimRefreshCallBack(m_networkListener);
}

/*!
  ut_cnetworklistener::t_AllowRefresh
 */
void ut_cnetworklistener::t_AllowRefresh()
{
    TSatRefreshType type;
    TSatElementaryFiles files;
    m_networkListener->AllowRefresh(type, files);
}

/*!
  ut_cnetworklistener::t_Refresh
 */
void ut_cnetworklistener::t_Refresh()
{
    TSatRefreshType type(ESimInitFileChangeNotification);
    TSatElementaryFiles files;
    g_mode = MPsetNetworkSelect::ENetSelectModeManual;
    g_changedCspSettings = EPSetNetSelSup;
    g_newValues = EPSetALS;
    expect("CPsetNetwork::GetNetworkSelectMode").willOnce(invoke(GetNetworkSelectMode)).returns(KErrNone);
    expect("CPSetRefreshHandlerMock::ChangedCspSettings").willOnce(invoke(ChangedCspSettings)).returns(KErrNone);
    m_networkListener->Refresh(type, files);
    QVERIFY(verify());
    
    g_newValues = EPSetNetSelSup;
    expect("CPsetNetwork::GetNetworkSelectMode").willOnce(invoke(GetNetworkSelectMode)).returns(KErrNone);
    expect("CPSetRefreshHandlerMock::ChangedCspSettings").willOnce(invoke(ChangedCspSettings)).returns(KErrNone);
    m_networkListener->Refresh(type, files);
    QVERIFY(verify());
    
    g_mode = MPsetNetworkSelect::ENetSelectModeAutomatic;
    expect("CPsetNetwork::GetNetworkSelectMode").willOnce(invoke(GetNetworkSelectMode)).returns(KErrNone);
    m_networkListener->Refresh(type, files);
    QVERIFY(verify());
}

/*!
  ut_cnetworklistener::t_HandleNetworkInfoReceivedL
 */
void ut_cnetworklistener::t_HandleNetworkInfoReceivedL()
{
    CNetworkInfoArray* infoArray;
    TInt result;
    m_networkListener->HandleNetworkInfoReceivedL(infoArray, result);
}

/*!
  ut_cnetworklistener::t_HandleCurrentNetworkInfoL
 */
void ut_cnetworklistener::t_HandleCurrentNetworkInfoL()
{
    MPsetNetworkSelect::TCurrentNetworkInfo currentInfo;
    TInt result;
    m_networkListener->HandleCurrentNetworkInfoL(currentInfo, result);
}

/*!
  ut_cnetworklistener::t_HandleNetworkChangedL
 */
void ut_cnetworklistener::t_HandleNetworkChangedL_1()
{
    MPsetNetworkSelect::TNetworkInfo currentInfo;
    MPsetNetworkSelect::TCurrentNetworkStatus status;
    TInt result;
    m_networkListener->HandleNetworkChangedL(currentInfo, status, result);
}

/*!
  ut_cnetworklistener::t_HandleNetworkChangedL
 */
void ut_cnetworklistener::t_HandleNetworkChangedL_2()
{
    MPsetNetworkSelect::TNetworkInfo currentInfo;
    RMobilePhone::TMobilePhoneRegistrationStatus status;
    TInt result;
    m_networkListener->HandleNetworkChangedL(currentInfo, status, result);
}

/*!
  ut_cnetworklistener::t_HandleSearchingNetworksL
 */
void ut_cnetworklistener::t_HandleSearchingNetworksL()
{
    MPsetNetworkInfoObserver::TServiceRequest request;
    m_networkListener->HandleSearchingNetworksL(request);
}

/*!
  ut_cnetworklistener::t_HandleRequestingSelectedNetworkL
 */
void ut_cnetworklistener::t_HandleRequestingSelectedNetworkL()
{
    TBool ongoing;
    m_networkListener->HandleRequestingSelectedNetworkL(ongoing);
}

/*!
  ut_cnetworklistener::t_HandleCallActivatedL
 */
void ut_cnetworklistener::t_HandleCallActivatedL()
{
    m_networkListener->HandleCallActivatedL();
}

/*!
  ut_cnetworklistener::t_HandleNetworkErrorL
 */
void ut_cnetworklistener::t_HandleNetworkErrorL()
{
    MPsetNetworkInfoObserver::TServiceRequest request;
    TInt error;
    m_networkListener->HandleNetworkErrorL(request, error);
}

QTEST_MAIN_S60(ut_cnetworklistener)
