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
* Description:  Unit tests for PhoneCommandExtensionWrapper.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <QList>
#include <QMap>
#include <hbapplication.h>
#include <phoneappcommands.hrh>
#include "phonecommandextensionwrapper.h"
#include "pevirtualengine.h"
#include "menuextension_stub.h"
#include "phonetestmain.h"

extern bool m_modifyMenuCommandListCalled;
extern bool m_modifyPushButtonCommandListCalled;
extern bool m_modifyToolBarCommandListCalled;
extern bool m_addMenuActionsCalled;
extern bool m_releaseMenuCalled;
extern bool m_releaseCalled;
extern bool m_useExtensionStub;

class TestPhoneCommandExtensionWrapper : public QObject
{
    Q_OBJECT
public:
    TestPhoneCommandExtensionWrapper();
    virtual ~TestPhoneCommandExtensionWrapper();

public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup ();
    
private slots:
    void testNullPlugin();
    void testPlugin();
    void testGetCallInfo();

private:
    PhoneCommandExtensionWrapper *m_wrapper; // class under test

};

TestPhoneCommandExtensionWrapper::TestPhoneCommandExtensionWrapper()
{
}

TestPhoneCommandExtensionWrapper::~TestPhoneCommandExtensionWrapper()
{
}

void TestPhoneCommandExtensionWrapper::initTestCase()
{
    
}

void TestPhoneCommandExtensionWrapper::cleanupTestCase()
{
    
}

void TestPhoneCommandExtensionWrapper::init()
{

}

void TestPhoneCommandExtensionWrapper::cleanup()
{
}



void TestPhoneCommandExtensionWrapper::testNullPlugin()
{
    QList<XQTelUiCommandExtension::CallInfo> callInfo; 
    QList<int> menuCmdList;
    QList<HbAction*> menuActions;
    QList<XQTelUiCommandExtension::ToolBarCommand> toolBar;
    m_useExtensionStub = false;
    m_wrapper = new PhoneCommandExtensionWrapper(123456);
    
    m_wrapper->modifyMenuCommandList(callInfo, menuCmdList);
    QVERIFY( false == m_modifyMenuCommandListCalled );
    
    m_wrapper->modifyPushButtonCommandList(callInfo, menuCmdList);
    QVERIFY( false == m_modifyPushButtonCommandListCalled );
   
    m_wrapper->modifyToolBarCommandList(callInfo, toolBar);
    QVERIFY( false == m_modifyToolBarCommandListCalled );
    
    m_wrapper->addMenuActions(callInfo, menuActions);
    QVERIFY( false == m_addMenuActionsCalled );
    
    m_wrapper->releaseMenu();
    QVERIFY( false == m_releaseMenuCalled );
    
    delete m_wrapper;
    
    QVERIFY( false == m_releaseCalled );
}

void TestPhoneCommandExtensionWrapper::testPlugin()
{
    QList<XQTelUiCommandExtension::CallInfo> callInfo; 
    QList<int> menuCmdList;
    QList<HbAction*> menuActions;
    QList<XQTelUiCommandExtension::ToolBarCommand> toolBar;
    m_useExtensionStub = true;
    m_wrapper = new PhoneCommandExtensionWrapper(123456);
    
    QVERIFY( 123456 == m_wrapper->pluginUid() );
    
    m_wrapper->modifyMenuCommandList(callInfo, menuCmdList);
    QVERIFY( m_modifyMenuCommandListCalled );
    
    m_wrapper->modifyPushButtonCommandList(callInfo, menuCmdList);
    QVERIFY( m_modifyPushButtonCommandListCalled );
    
    m_wrapper->modifyToolBarCommandList(callInfo, toolBar);
    QVERIFY( m_modifyToolBarCommandListCalled );
    
    m_wrapper->addMenuActions(callInfo, menuActions);
    QVERIFY( m_addMenuActionsCalled );
    
    m_wrapper->releaseMenu();
    QVERIFY( m_releaseMenuCalled );
    
    delete m_wrapper;

    QVERIFY( m_releaseCalled );
}

void TestPhoneCommandExtensionWrapper::testGetCallInfo()
{
    m_wrapper = new PhoneCommandExtensionWrapper(123456);
    QList<XQTelUiCommandExtension::CallInfo> callInfo; 
    QMap<int,int> callStates;
    QMap<int,int> serviceIds;
    int expandedCall(0);
    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(0, callInfo.count());
    
    callStates[0] = EPEStateDisconnecting; 
    serviceIds[0] = 3;   
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::Disconnecting == callInfo.at(0).mCallState);
    QVERIFY(true == callInfo.at(0).mIsExpanded);
    QVERIFY(3 == callInfo.at(0).mServiceId);
    
    callInfo.clear();
    callStates[0] = EPEStateRinging;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::Incoming == callInfo.at(0).mCallState);
    
    callInfo.clear();
    callStates[0] = EPEStateDialing;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::Outgoing == callInfo.at(0).mCallState);
    
    callInfo.clear();
    callStates[0] = EPEStateConnecting;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::Outgoing == callInfo.at(0).mCallState);
    
    callInfo.clear();
    callStates[0] = EPEStateConnected;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::Active == callInfo.at(0).mCallState);
    
    callInfo.clear();
    callStates[0] = EPEStateConnectedConference;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::Active == callInfo.at(0).mCallState);
    
    callInfo.clear();
    callStates[0] = EPEStateHeld;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::OnHold == callInfo.at(0).mCallState);
    
    callInfo.clear();
    callStates[0] = EPEStateHeldConference;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::OnHold == callInfo.at(0).mCallState);
    
    callInfo.clear();
    callStates[0] = EPEStateUnknown;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::None == callInfo.at(0).mCallState);
    
    callInfo.clear();
    callStates[0] = EPEStateIdle;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::None == callInfo.at(0).mCallState);
    
    callInfo.clear();
    callStates[0] = EPEStateConferenceIdle;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::None == callInfo.at(0).mCallState);
    
    callInfo.clear();
    callStates[0] = EPEStateMaximumState;    
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(1, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::None == callInfo.at(0).mCallState);
    
    //////////////////////////////////////////////////////////////////////////
    // Two calls
    //////////////////////////////////////////////////////////////////////////
    callInfo.clear();
    callStates[0] = EPEStateConnected;
    callStates[1] = EPEStateHeld;
    serviceIds[1] = 2;
    m_wrapper->getCallInfoList(callInfo, callStates, serviceIds, expandedCall);
    QCOMPARE(2, callInfo.count());
    QVERIFY(XQTelUiCommandExtension::Active == callInfo.at(0).mCallState);
    QVERIFY(true == callInfo.at(0).mIsExpanded);
    QVERIFY(3 == callInfo.at(0).mServiceId);
    QVERIFY(XQTelUiCommandExtension::OnHold == callInfo.at(1).mCallState);
    QVERIFY(false == callInfo.at(1).mIsExpanded);
    QVERIFY(2 == callInfo.at(1).mServiceId);
    
    delete m_wrapper;
}


PHONE_TEST_MAIN(TestPhoneCommandExtensionWrapper)
#include "unit_tests.moc"
