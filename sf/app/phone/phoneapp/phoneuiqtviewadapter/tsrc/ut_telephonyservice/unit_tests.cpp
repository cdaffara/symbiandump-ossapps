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
* Description:  Unit tests for telephonyservice.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <hbapplication.h>
#include <QSignalSpy>
#include <xqservicerequest.h>
#include <hbmenu.h>
#include <mockservice.h>

#include "telephonyservice.h"
#include "phoneuiqtviewif.h"
#include "phoneuiqtviewadapter.h"
#include "tphonecmdparaminteger.h"
#include "phonetestmain.h"

class TestTelephonyService : public QObject, PhoneUIQtViewIF, MockService
{
    Q_OBJECT
public:
    TestTelephonyService();
    virtual ~TestTelephonyService();
    
public: // PhoneUIQtViewIF
    
        BubbleManagerIF& bubbleManager (){};
        void addBubbleCommand ( 
                int ,
                const PhoneAction&  ){};
        void clearBubbleCommands (int ){};
        void addParticipantListAction(
                int ,  
                const QString &, 
                const HbIcon &){};
        void clearParticipantListActions(){};
        void hideToolbar () {};
        void showToolbar () {};
        void setToolbarActions (const QList<PhoneAction*>& ){};
        int volumeSliderValue () { return 0; };
        void removeVolumeSlider () {};
        void setVolumeSliderValue (
                int , 
                int , 
                int , 
                int  ) { };

        void setExpandAction(int , int ) {};
        void removeExpandAction(int ) {};
        void showDialpad() {};
        void hideDialpad() {};
        bool isDialpadVisible() { return false; };
        QString dialpadText() {return QString("1234567");};
        void clearAndHideDialpad(){};
        void clearDialpad() {};
        void bringToForeground() {};
        void setMenuActions(const QList<PhoneAction*>& ){};
        void shutdownPhoneApp() {};
        void setBackButtonVisible(bool ) {};        
        HbMenu &menuReference() {};
        void captureKey(Qt::Key , bool ) {};
        void setRestrictedMode(bool ) {};
        
public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup (); 
    
private slots:
    void testConstructionDestruction ();
    void startPhoneApp ();
    void startInCallDialer ();
    
private:
    PhoneUIQtViewAdapter* m_viewAdapter;
    TelephonyService *m_service; // class under test
};

TestTelephonyService::TestTelephonyService ()
{
}

TestTelephonyService::~TestTelephonyService ()
{
}

void TestTelephonyService::initTestCase ()
{
    m_viewAdapter = new PhoneUIQtViewAdapter(*this);
    m_service = new TelephonyService(m_viewAdapter, this);
}

void TestTelephonyService::cleanupTestCase ()
{
    delete m_viewAdapter;
    delete m_service;
}

void TestTelephonyService::init ()
{
    initialize();
}

void TestTelephonyService::cleanup ()
{
    reset();
}

void TestTelephonyService::testConstructionDestruction ()
{
// empty
}

void TestTelephonyService::startPhoneApp ()
{
    int phoneApp(0);    
    EXPECT( PhoneUIQtViewAdapter, ExecuteCommandL ).with<TPhoneViewCommandId>( EPhoneViewBringPhoneAppToForeground );
    m_service->start(phoneApp);
    QVERIFY(verify());
}

void TestTelephonyService::startInCallDialer ()
{
    int dialer(1);
    
    EXPECT( PhoneUIQtViewAdapter, ExecuteCommandL ).with( EPhoneViewOpenDialer );
    EXPECT( PhoneUIQtViewAdapter, ExecuteCommandL ).with<TPhoneViewCommandId>( EPhoneViewBringPhoneAppToForeground );
    m_service->start(dialer);
    QVERIFY(verify());
}

PHONE_TEST_MAIN(TestTelephonyService)
#include "unit_tests.moc"
