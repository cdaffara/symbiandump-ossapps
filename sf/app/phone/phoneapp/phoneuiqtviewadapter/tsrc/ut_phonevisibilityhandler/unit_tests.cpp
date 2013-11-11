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
* Description:  Unit tests for PhoneUIQtViewAdapter.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <QList>
#include <QKeyEvent>
#include <QSignalSpy>
#include <HbApplication>
#include <xqcallinfo.h>
#include <callinformation.h>
#include <MediatorDomainUIDs.h>
#include <telinformationpskeys.h>
#include <coreapplicationuisdomainpskeys.h>
#include <ccallinfoiter.h>
#include <ccallinformation.h>
#include <keyguardaccessapi.h>
#include <phoneuitestmain.h>
#include "phoneuiqtviewif.h"
#define private public
#include "phonevisibilityhandler.h"
#undef public

int m_callCount = 0;
int g_keyGuardEnabled = false;
int g_enableKeyGuardCalled = false;
int g_disableKeyGuardCalled = false;
CCallInformation* CCallInformation::NewL() {};
CCallInfoIter * CCallInfoIter::NewL(class CCallInfos &) {};

CCallInformation::CCallInformation() {}
CCallInformation::~CCallInformation() {}
CCallInfoIter& CCallInformation::GetCallsL( ){}

TInt CCallInfoIter::Count() const
{
    return m_callCount;
}

class TestPhoneVisibilityHandler : public QObject, public PhoneUIQtViewIF
{
    Q_OBJECT
public:
    TestPhoneVisibilityHandler();
    virtual ~TestPhoneVisibilityHandler();

    // From PhoneUIQtViewIF
    BubbleManagerIF& bubbleManager () {};
    void addBubbleCommand (int , const PhoneAction& ) {};
    void clearBubbleCommands (int ) {};
    void addParticipantListAction(
            int ,  
            const QString &, 
            const HbIcon &) {};
    void clearParticipantListActions() {};
    void hideToolbar () {};
    void showToolbar () {};
    void setToolbarActions (const QList<PhoneAction*>& ) {};
    int volumeSliderValue () {};
    void removeVolumeSlider () {};
    void setVolumeSliderValue (
            int , 
            int , 
            int , 
            int  ) {};

    void setExpandAction(int , int ) {};
    void removeExpandAction(int ) {};
    void showDialpad() {};
    void hideDialpad() {};
    bool isDialpadVisible() {};
    QString dialpadText() {};
    void clearAndHideDialpad() {};
    void clearDialpad() {};
    void bringToForeground() { m_bringToForegroundCalled = true;};
    void setMenuActions(const QList<PhoneAction*>& ) {};
    void shutdownPhoneApp() {};
    void setBackButtonVisible(bool ) {};
    HbMenu &menuReference() {};
    void captureKey(Qt::Key , bool ) {};
    void setRestrictedMode(bool ) {};
    
public slots:
    void initTestCase();

    void cleanupTestCase();

    void init();

    void cleanup();
    
private slots:
    void t_memleak();
    
    void t_normal();
    
    void t_carmode();
    
    void t_devicelock();
    
    void t_keyGuardHandling();

    
private:
    PhoneVisibilityHandler *m_handler;
    bool m_bringToForegroundCalled;
    int m_startPriority;
    int m_normalPriority;
};

CKeyguardAccessApi* CKeyguardAccessApi::NewL()
{
    return new CKeyguardAccessApi;
}
CKeyguardAccessApi::~CKeyguardAccessApi( ) {}
CKeyguardAccessApi::CKeyguardAccessApi( ) {}


TBool CKeyguardAccessApi::IsKeyguardEnabled()
{
    return g_keyGuardEnabled;
}

TInt CKeyguardAccessApi::EnableKeyguard( TBool  )
{
    g_enableKeyGuardCalled = true;
    return 0;
}

TInt CKeyguardAccessApi::DisableKeyguard( TBool  )
{
    g_disableKeyGuardCalled = true;
    return 0;
}



TestPhoneVisibilityHandler::TestPhoneVisibilityHandler ()
{
}

TestPhoneVisibilityHandler::~TestPhoneVisibilityHandler ()
{
}

void TestPhoneVisibilityHandler::initTestCase ()
{
    m_normalPriority = CEikonEnv::Static()->RootWin().OrdinalPriority();
    
    m_handler = new PhoneVisibilityHandler (*this, this);
    
    // Expect OrdinalPosition higher than 0
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    m_startPriority = CEikonEnv::Static()->RootWin().OrdinalPriority();
    QCOMPARE(m_startPriority, (int)ECoeWinPriorityNeverAtFront);
}

void TestPhoneVisibilityHandler::cleanupTestCase ()
{
    delete m_handler;
}

void TestPhoneVisibilityHandler::init ()
{
}

void TestPhoneVisibilityHandler::cleanup ()
{
    g_keyGuardEnabled = false;
    g_enableKeyGuardCalled = false;
    g_disableKeyGuardCalled = false;
    m_bringToForegroundCalled = false;
}

void TestPhoneVisibilityHandler::t_memleak()
{
}

void TestPhoneVisibilityHandler::t_normal()
{
    // Test hide device dialogs when background ( false )
    m_handler->hideDeviceDialogs(false);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
    
    // Test hide device dialogs when background ( true )
    m_handler->hideDeviceDialogs(true);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
    
    // bringToForeground is call
    m_handler->bringToForeground();
    QVERIFY(m_bringToForegroundCalled);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);
    
    // Test hide device dialogs when foreground ( false )
    m_handler->hideDeviceDialogs(false);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_normalPriority);

    // Test hide device dialogs when foreground ( true )
    m_handler->hideDeviceDialogs(true);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);
    
    // send backgroud is called last
    m_handler->sendToBackground(false);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
}

void TestPhoneVisibilityHandler::t_carmode()
{
    QValueSpaceSubscriber* subscriber = new QValueSpaceSubscriber("/phone/carmode", this);
    QValueSpacePublisher* publisher = new QValueSpacePublisher("/phone",this);

    QVERIFY(m_handler->m_carModeEnabled == false);
    
    // 1 ongoing call and car mode switched on
    m_callCount = 1;
    publisher->setValue(QString("/carmode"),QVariant(true));
    QTest::qWait(100);
    QVERIFY(m_handler->m_carModeEnabled == true);
    QVERIFY(m_bringToForegroundCalled == false);

    // 1 ongoing call and car mode switched off
    publisher->setValue(QString("/carmode"),QVariant(false));
    QTest::qWait(100);
    QVERIFY(m_handler->m_carModeEnabled == false);
    QVERIFY(m_bringToForegroundCalled == true);
    
    // 0 ongoing calls and car mode switched on
    m_callCount = 0;
    m_bringToForegroundCalled = false;
    publisher->setValue(QString("/carmode"),QVariant(true));
    QTest::qWait(100);
    QVERIFY(m_handler->m_carModeEnabled == true);
    QVERIFY(m_bringToForegroundCalled == false);

    // 0 ongoing calls and car mode switched off
    publisher->setValue(QString("/carmode"),QVariant(false));
    QTest::qWait(100);
    QVERIFY(m_handler->m_carModeEnabled == false);
    QVERIFY(m_bringToForegroundCalled == false);
    
    // send backgroud is called last
    m_handler->sendToBackground(false);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
}

void TestPhoneVisibilityHandler::t_devicelock()
{
    m_handler->HandlePropertyChangedL(KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, ETimerLocked);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
    
    // Test hide device dialogs when background ( false )
    m_handler->hideDeviceDialogs(false);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
    
    // Test hide device dialogs when background ( true )
    m_handler->hideDeviceDialogs(true);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
    
    // bringToForeground is call
    m_handler->bringToForeground();
    QVERIFY(m_bringToForegroundCalled);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);
    
    // Test hide device dialogs when foreground ( false )
    m_handler->hideDeviceDialogs(false);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 1);

    // Test hide device dialogs when foreground ( true )
    m_handler->hideDeviceDialogs(true);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);
    
    // Test lock status change
    m_handler->HandlePropertyChangedL(KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, EAutolockOff);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);

    
    // send backgroud is called last
    m_handler->sendToBackground(false);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
    
}


void TestPhoneVisibilityHandler::t_keyGuardHandling()
{
    // Test when keyguard is not enabled before call
    g_keyGuardEnabled = false; 
    
    // bringToForeground is call
    m_handler->bringToForeground();
    QVERIFY(m_bringToForegroundCalled);
    QVERIFY(g_disableKeyGuardCalled);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);
    
    cleanup();
    // send backgroud is called last
    m_handler->sendToBackground(false);
    QVERIFY(!g_enableKeyGuardCalled);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
    
    
    // Test when keyguard is enabled before call
    cleanup();
    g_keyGuardEnabled = true; 
    
    // bringToForeground is call
    m_handler->bringToForeground();
    QVERIFY(m_bringToForegroundCalled);
    QVERIFY(g_disableKeyGuardCalled);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);
    
    cleanup();
    // send backgroud is called last
    m_handler->sendToBackground(false);
    QVERIFY(g_enableKeyGuardCalled);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
    
    // Test when keyguard is enabled before call +
    // keyguard is not enabled when phone is background
    cleanup();
    g_keyGuardEnabled = true;
    
    // bringToForeground is call
    m_handler->bringToForeground();
    QVERIFY(m_bringToForegroundCalled);
    QVERIFY(g_disableKeyGuardCalled);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);

    cleanup();
    // Test that keyguard is not enabled when phone is background
    CEikonEnv::Static()->RootWin().SetOrdinalPosition(-1, ECoeWinPriorityNormal);
    m_handler->sendToBackground(false);
    QVERIFY(!g_enableKeyGuardCalled);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
    
    
    // Test case where two calls is created and key guard is enabled
    cleanup();
    g_keyGuardEnabled = true; 
    
    // bringToForeground is call
    m_handler->bringToForeground();
    QVERIFY(m_bringToForegroundCalled);
    QVERIFY(g_disableKeyGuardCalled);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);
    
    cleanup();
    m_handler->bringToForeground();
    QVERIFY(m_bringToForegroundCalled);
    QVERIFY(g_disableKeyGuardCalled);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);
    
    cleanup();
    m_handler->sendToBackground(false);
    QVERIFY(g_enableKeyGuardCalled);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
    
    // Test case where two calls is created and key guard is enabled during middle
    cleanup();
    
    // bringToForeground is call
    m_handler->bringToForeground();
    QVERIFY(m_bringToForegroundCalled);
    QVERIFY(g_disableKeyGuardCalled);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);
    
    cleanup();
    g_keyGuardEnabled = true; 
    m_handler->bringToForeground();
    QVERIFY(m_bringToForegroundCalled);
    QVERIFY(g_disableKeyGuardCalled);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPosition(), 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), ECoeWinPriorityAlwaysAtFront + 100);
    
    cleanup();
    m_handler->sendToBackground(false);
    QVERIFY(g_enableKeyGuardCalled);
    QVERIFY(CEikonEnv::Static()->RootWin().OrdinalPosition() > 0);
    QCOMPARE(CEikonEnv::Static()->RootWin().OrdinalPriority(), m_startPriority);
}


PHONE_UITEST_MAIN(TestPhoneVisibilityHandler)
#include "unit_tests.moc"
