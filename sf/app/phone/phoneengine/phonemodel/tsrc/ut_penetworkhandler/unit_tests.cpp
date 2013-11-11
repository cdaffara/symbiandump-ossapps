/*!
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
* Description:  Unit tests for PhoneUINetworkHandler.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <QVariant>
#include <QKeyEvent>
#include <hbapplication.h>
#include <qsysteminfo.h>
#include "cpeengineinfo.h"
#include "penetworkhandler.h"


//CONSTANTS


#define PHONE_QT_TEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    HbApplication app(argc, argv); \
    TestObject tc; \
    QResource::registerResource("../hbcore.rcc"); \
    int ret = QTest::qExec(&tc, argc, argv); \
    /* Core dump if HbIconLoader instance is not destroyed before the application instance. */ \
    /* HbIconLoader uses QCoreApplication::aboutToQuit() signal to destroy itself. */ \
    /* app.exec() where the signal is normally emitted is not called here. */ \
    /* So, invoking the signal explicitly. */ \
    QMetaObject::invokeMethod(&app, "aboutToQuit", Qt::DirectConnection); \
    return ret; \
}

class TestNetworkHandler : public QObject, public MPEPhoneModelInternal
{
    Q_OBJECT
    
public:
    TestNetworkHandler ();
    ~TestNetworkHandler ();

// Functions from MPEPhoneModelInternal base class
        
    MPEDataStore* DataStore() {return iEngineInfo;};

    MPEExternalDataHandler* DataStoreExt() {;}; 
        
    CPERemotePartyInfoMediator* MediatorCommunicationHandler() {return 0;};
        
    void HandleInternalMessage( const TInt aMessage ) {;};

    void SendMessage( const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage ) 
        {
        if ( MEngineMonitor::EPEMessageNetworkRegistrationStatusChange == aMessage ) 
            {
            m_networkStatusChanged = true;
            }
        ;};

    void SendMessage( const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
                          const TInt aCallId ) {;};

public slots:
    void initTestCase ();
    void cleanupTestCase (); 
    void init ();
    void cleanup ();
    
private slots:
    void testNetworkStatusChanged();



private:
    PeNetworkHandler *m_nwHandler; // class under test
    CPEEngineInfo *iEngineInfo;
    bool m_networkStatusChanged;

};


TestNetworkHandler::TestNetworkHandler()
{    
}

TestNetworkHandler::~TestNetworkHandler()
{
}

void TestNetworkHandler::initTestCase()
{	
}

void TestNetworkHandler::cleanupTestCase()
{
}

void TestNetworkHandler::init()
{	
    TRAP_IGNORE( iEngineInfo = CPEEngineInfo::NewL() );	
    m_nwHandler = new PeNetworkHandler(*this);    
}

void TestNetworkHandler::cleanup()
{
    delete m_nwHandler;
    delete iEngineInfo;
}

void TestNetworkHandler::testNetworkStatusChanged()
{
    m_networkStatusChanged = false;
    m_nwHandler->networkStatusChanged(QSystemNetworkInfo::UnknownMode,
            QSystemNetworkInfo::UndefinedStatus);
    QVERIFY( false == m_networkStatusChanged );
    
    m_nwHandler->networkStatusChanged(QSystemNetworkInfo::CdmaMode,
            QSystemNetworkInfo::UndefinedStatus);
    QVERIFY( ENWStatusRegistrationUnknown == 
             iEngineInfo->NetworkRegistrationStatus() );
    QVERIFY( true == m_networkStatusChanged );
       
    m_nwHandler->networkStatusChanged(QSystemNetworkInfo::WcdmaMode,
            QSystemNetworkInfo::NoNetworkAvailable);
    QVERIFY( ENWStatusNotRegisteredNoService == 
             iEngineInfo->NetworkRegistrationStatus() );
    
    m_nwHandler->networkStatusChanged(QSystemNetworkInfo::GsmMode,
            QSystemNetworkInfo::EmergencyOnly);
    QVERIFY( ENWStatusNotRegisteredEmergencyOnly == 
             iEngineInfo->NetworkRegistrationStatus() );
    
    m_nwHandler->networkStatusChanged(QSystemNetworkInfo::GsmMode,
            QSystemNetworkInfo::Searching);
    QVERIFY( ENWStatusNotRegisteredSearching == 
             iEngineInfo->NetworkRegistrationStatus() );
    
    m_nwHandler->networkStatusChanged(QSystemNetworkInfo::GsmMode,
            QSystemNetworkInfo::Busy);
    QVERIFY( ENWStatusRegisteredBusy == 
             iEngineInfo->NetworkRegistrationStatus() );
    
    m_nwHandler->networkStatusChanged(QSystemNetworkInfo::GsmMode,
            QSystemNetworkInfo::HomeNetwork);
    QVERIFY( ENWStatusRegisteredOnHomeNetwork == 
             iEngineInfo->NetworkRegistrationStatus() );
    
    m_nwHandler->networkStatusChanged(QSystemNetworkInfo::GsmMode,
            QSystemNetworkInfo::Denied);
    QVERIFY( ENWStatusRegistrationDenied == 
             iEngineInfo->NetworkRegistrationStatus() );
    
    m_nwHandler->networkStatusChanged(QSystemNetworkInfo::GsmMode,
            QSystemNetworkInfo::Roaming);
    QVERIFY( ENWStatusRegisteredRoaming == 
             iEngineInfo->NetworkRegistrationStatus() );
    
    m_nwHandler->networkStatusChanged(QSystemNetworkInfo::GsmMode,
            (QSystemNetworkInfo::NetworkStatus)100);
    QVERIFY( ENWStatusRegistrationUnknown == 
             iEngineInfo->NetworkRegistrationStatus() );
}


PHONE_QT_TEST_MAIN(TestNetworkHandler)
#include "unit_tests.moc"
