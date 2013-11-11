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
#include <networkhandlingproxy.h>
#include <nwhandlingengine.h>
#include <cnwsession.h>
#include "qtestmains60.h"
#include <xqsettingskey.h>
#include <xqsettingsmanager.h>
#include "ut_infowidgetnetworkhandler.h"
#define private public
#include "infowidgetnetworkhandler.h"

class CMyNWSession : public CNWSession 
{
public: 
    CMyNWSession(){};
    ~CMyNWSession(){};
}; 


#define EXPECT_EXCEPTION(statements)    \
{                                       \
    bool exceptionDetected = false;     \
    try {                               \
        statements                      \
    } catch (...) {                     \
        exceptionDetected = true;       \
    }                                   \
    QVERIFY(true == exceptionDetected); \
}

const QString KHomeZoneTextTag("HomeZoneText0");    // max length 13
const TNWViagIndicatorType 
    KHomeZoneIndicatorType = ENWViagIndicatorTypeHomeZone;

const QString KMcnName("McnNameMcnNameMcnNam");     // max length 20 
const TNWMCNIndicatorType 
    KMcnIndicatorType = ENWMCNIndicatorTypeActive;

const QString KSpnName("SpnNameSpnName..");     // max length 16 

/*!
  UT_InfoWidgetNetworkHandler::UT_InfoWidgetNetworkHandler
 */
UT_InfoWidgetNetworkHandler::UT_InfoWidgetNetworkHandler() 
    : 
    m_networkHandler(NULL)
{   
}


/*!
  UT_InfoWidgetNetworkHandler::~UT_InfoWidgetNetworkHandler
 */
UT_InfoWidgetNetworkHandler::~UT_InfoWidgetNetworkHandler()
{
    delete m_networkHandler;
}

/*!
  UT_InfoWidgetNetworkHandler::doLeaveL
 */
void UT_InfoWidgetNetworkHandler::doLeaveL()
{
    User::Leave(KErrGeneral);
}

/*!
  UT_InfoWidgetNetworkHandler::generateException
 */
void UT_InfoWidgetNetworkHandler::generateException()
{
    throw std::bad_alloc();
}

/*!
  UT_InfoWidgetNetworkHandler::init
 */
void UT_InfoWidgetNetworkHandler::init()
{
    initialize();
    
    SmcDefaultValue<QString>::SetL("");
    SmcDefaultValue<const QString & >::SetL("");

    // Ownership is transferred to InfoWidgetNetworkHandler
    CMyNWSession *session= new (ELeave)CMyNWSession(); 
    expect("CreateL").returns(session);
    m_networkHandler =  new InfoWidgetNetworkHandler();

    m_networkHandler->m_nwInfo.iViagTextTag.Copy(KHomeZoneTextTag.utf16());
    m_networkHandler->m_nwInfo.iViagIndicatorType = KHomeZoneIndicatorType;
    m_networkHandler->m_nwInfo.iMCNName.Copy(KMcnName.utf16());
    m_networkHandler->m_nwInfo.iMCNIndicatorType = KMcnIndicatorType;
    m_networkHandler->m_nwInfo.iSPName.Copy(KSpnName.utf16()); 
    
    QVERIFY(verify());
}


/*!
  UT_InfoWidgetNetworkHandler::cleanup
 */
void UT_InfoWidgetNetworkHandler::cleanup()
{
    reset();
    
    QVariant i(1);
    EXPECT(XQSettingsManager, readItemValue).returns(i);  
    delete m_networkHandler;
    m_networkHandler = 0;
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetNetworkHandler::t_homeZoneTextTag
 */
void UT_InfoWidgetNetworkHandler::t_homeZoneTextTag()
{
    QVariant i(1);
    EXPECT(XQSettingsManager, readItemValue).returns(i);
    
    QVERIFY(m_networkHandler->homeZoneTextTag() == KHomeZoneTextTag);
}


/*!
  UT_InfoWidgetNetworkHandler::t_homeZoneIndicatorType
 */
void UT_InfoWidgetNetworkHandler::t_homeZoneIndicatorType()
{
    QVERIFY(
        m_networkHandler->homeZoneIndicatorType() == KHomeZoneIndicatorType);
}


/*!
  UT_InfoWidgetNetworkHandler::t_mcnName
 */
void UT_InfoWidgetNetworkHandler::t_mcnName()
{
    QVERIFY(m_networkHandler->mcnName() == KMcnName);
}


/*!
  UT_InfoWidgetNetworkHandler::t_mcnIndicatorType
 */
void UT_InfoWidgetNetworkHandler::t_mcnIndicatorType()
{
    QVERIFY(m_networkHandler->mcnIndicatorType() == KMcnIndicatorType);
}


/*!
  UT_InfoWidgetNetworkHandler::t_isOnline
 */
void UT_InfoWidgetNetworkHandler::t_isOnline()
{
    m_networkHandler->m_nwInfo.iRegistrationStatus = 
            static_cast<TNWRegistrationStatus>(ENWRegisteredBusy);
    QVERIFY(m_networkHandler->isOnline() == true);
    m_networkHandler->m_nwInfo.iRegistrationStatus = 
            static_cast<TNWRegistrationStatus>(ENWRegisteredOnHomeNetwork);
    QVERIFY(m_networkHandler->isOnline() == true);
    m_networkHandler->m_nwInfo.iRegistrationStatus = 
            static_cast<TNWRegistrationStatus>(ENWRegisteredRoaming);
    QVERIFY(m_networkHandler->isOnline() == true);
    
    m_networkHandler->m_nwInfo.iRegistrationStatus = 
            static_cast<TNWRegistrationStatus>(ENWRegistrationUnknown);
    QVERIFY(m_networkHandler->isOnline() == false);
}


/*!
  UT_InfoWidgetNetworkHandler::t_networkRegistrationStatus
 */
void UT_InfoWidgetNetworkHandler::t_networkRegistrationStatus()
{
    m_networkHandler->m_nwInfo.iRegistrationStatus = 
                static_cast<TNWRegistrationStatus>(ENWRegistrationUnknown);
    QVERIFY(m_networkHandler->networkRegistrationStatus() == 0);
}


/*!
  UT_InfoWidgetNetworkHandler::t_HandleNetworkMessage
 */
void UT_InfoWidgetNetworkHandler::t_HandleNetworkMessage()
{
    QSignalSpy spy(m_networkHandler, SIGNAL(networkDataChanged()));
    
    // TODO: investigate, which messages indicate cell info change
// ACCEPTED MESSAGES    
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageCurrentCellInfoMessage);
    
// MESSAGES TO BE FILTERED OUT
// COMMON NETWORK MESSAGES                      0 - 99
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageNetworkInfoChange);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageNetworkModeChange);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageNetworkProviderNameChange);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageServiceProviderNameChange);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageProgrammableOperatorInfoUpdating);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageNetworkProviderNameUpdating);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageServiceProviderNameUpdating);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageAllowRefresh);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageNetworkSelectionSettingChange);
// GSM SPECIFIC MESSAGES                        100 - 199
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageNetworkCellReselection);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageNetworkConnectionFailure);
//    m_networkHandler->HandleNetworkMessage(
//        MNWMessageObserver::ENWMessageCurrentCellInfoMessage);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageCurrentHomeZoneMessage);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageNetworkIndicatorChange);
// CDMA SPECIFIC MESSAGES                       200 - 299
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageNetworkEmergencyMode); 
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageVoicePrivacyStatusChange);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageStartSystemSearchRequestCompleteOk);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageStartSystemSearchRequestCompleteFail);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageStopProtocolStackRequestCompleteOk);
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageStopProtocolStackRequestCompleteFail);
    
    const int KExpectedNumOfSignalEmissions = 3;
    QCOMPARE(spy.count(), KExpectedNumOfSignalEmissions);
    
    const QList<QVariant> &arguments = spy.at(0);
    QCOMPARE(arguments.count(), 0);
}


/*!
  UT_InfoWidgetNetworkHandler::t_HandleNetworkMessageExceptionWhileEmittingSignal
 */
void UT_InfoWidgetNetworkHandler::t_HandleNetworkMessageExceptionWhileEmittingSignal()
{
    QObject::connect(
        m_networkHandler, SIGNAL(networkDataChanged()),
        this, SLOT(generateException())); 
    
    m_networkHandler->HandleNetworkMessage(
        MNWMessageObserver::ENWMessageCurrentCellInfoMessage);
    
    QVERIFY(verify());
}


/*!
  UT_InfoWidgetNetworkHandler::t_HandleNetworkError
 */
Q_DECLARE_METATYPE(MNWMessageObserver::TNWOperation)
void UT_InfoWidgetNetworkHandler::t_HandleNetworkError()
{
    QSignalSpy spy(m_networkHandler, SIGNAL(networkError(int, int)));
    qRegisterMetaType<MNWMessageObserver::TNWOperation>
        ("MNWMessageObserver::TNWOperation");
    
    const int errorCode = -4;
    MNWMessageObserver::TNWOperation operation = 
            MNWMessageObserver::ENWGetServiceProviderName;
    m_networkHandler->HandleNetworkError(operation, errorCode);
        
    operation = MNWMessageObserver::ENWNone;
    m_networkHandler->HandleNetworkError(operation, errorCode);
    
    const int KExpectedNumOfSignalEmissions = 2;
    QCOMPARE(spy.count(), KExpectedNumOfSignalEmissions);
    const QList<QVariant> &arguments = spy.at(0);
    QCOMPARE(arguments.count(), 2);
    
    QCOMPARE(
        qvariant_cast<MNWMessageObserver::TNWOperation>(arguments.at(0)), 
        operation);
    QCOMPARE(arguments.at(1).toInt(), errorCode);
}


/*!
  UT_InfoWidgetNetworkHandler::t_HandleNetworkErrorExceptionWhileEmittingSignal
 */
void UT_InfoWidgetNetworkHandler::t_HandleNetworkErrorExceptionWhileEmittingSignal()
{
    QObject::connect(
        m_networkHandler, SIGNAL(networkError(int, int)),
        this, SLOT(generateException())); 
    
    MNWMessageObserver::TNWOperation operation = MNWMessageObserver::ENWNone;
    const int errorCode = -4;
    m_networkHandler->HandleNetworkError(operation, errorCode);
    
    QVERIFY(verify());
}


/*!
  UT_InfoWidgetNetworkHandler::initializeNetworkInfo
 */
void UT_InfoWidgetNetworkHandler::initializeNetworkInfo(
    MNWMessageObserver& /*aObserver*/, TNWInfo& aTNWInfo)
{
    aTNWInfo.iViagTextTag.Copy(KHomeZoneTextTag.utf16());
    aTNWInfo.iViagIndicatorType = KHomeZoneIndicatorType;
    
    aTNWInfo.iMCNName.Copy(KMcnName.utf16());
    aTNWInfo.iMCNIndicatorType = KMcnIndicatorType;
}

/*!
  UT_InfoWidgetNetworkHandler::t_suspend
 */
void UT_InfoWidgetNetworkHandler::t_suspend()
{
    m_networkHandler->suspend();
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetNetworkHandler::t_resume
 */
void UT_InfoWidgetNetworkHandler::t_resume()
{
    m_networkHandler->resume();
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetNetworkHandler::t_serviceProviderName
 */
void UT_InfoWidgetNetworkHandler::t_serviceProviderName()
{
    QCOMPARE(m_networkHandler->serviceProviderName(), KSpnName);
}

/*!
  UT_InfoWidgetNetworkHandler::t_serviceProviderNameDisplayRequired
 */
void UT_InfoWidgetNetworkHandler::t_serviceProviderNameDisplayRequired()
{
    m_networkHandler->m_nwInfo.iServiceProviderNameDisplayReq = 
                RMobilePhone::KDisplaySPNRequired; 
    QVERIFY(m_networkHandler->serviceProviderNameDisplayRequired() == true);

    m_networkHandler->m_nwInfo.iServiceProviderNameDisplayReq = 
            RMobilePhone::KDisplaySPNNotRequired; 
    QVERIFY(m_networkHandler->serviceProviderNameDisplayRequired() == false);
}

/*!
  UT_InfoWidgetNetworkHandler::t_createLeave
 */
void UT_InfoWidgetNetworkHandler::t_createLeave()
{
    QVariant i(1);
    EXPECT(XQSettingsManager, readItemValue).returns(i);
    expect("CreateL")
            .willOnce(invokeWithoutArguments(this, &doLeaveL));
    
    EXPECT_EXCEPTION(InfoWidgetNetworkHandler * p = new InfoWidgetNetworkHandler(););
}

QTEST_MAIN_S60(UT_InfoWidgetNetworkHandler)
