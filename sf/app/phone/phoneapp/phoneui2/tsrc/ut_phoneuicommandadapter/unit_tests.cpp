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
* Description:  Unit tests for PhoneUIQtView.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <QVariant>
#include <QKeyEvent>
#include <hbapplication.h>
#include "phoneuicommandadapter.h"
#include "cphoneuicontroller_stub.h"


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

class TestPhoneCommandAdapter : public QObject
{
    Q_OBJECT
    
public:
    TestPhoneCommandAdapter ();
    ~TestPhoneCommandAdapter ();


public slots:
    void initTestCase ();
    void cleanupTestCase (); 
    void init ();
    void cleanup ();
    
private slots:
    void testKeyHandleCommand ();
    void testLeave ();



private:
    PhoneUiCommandAdapter *m_command_adapter; // class under test
    CPhoneUIController_Stub *m_uicontrol;

};


TestPhoneCommandAdapter::TestPhoneCommandAdapter ()
{    
}

TestPhoneCommandAdapter::~TestPhoneCommandAdapter ()
{
}

void TestPhoneCommandAdapter::initTestCase ()
{	
}

void TestPhoneCommandAdapter::cleanupTestCase ()
{
}

void TestPhoneCommandAdapter::init ()
{
    m_uicontrol = new CPhoneUIController_Stub(); 
    m_command_adapter = new PhoneUiCommandAdapter(*m_uicontrol);
}

void TestPhoneCommandAdapter::cleanup ()
{
    delete m_uicontrol;
    delete m_command_adapter;
}

void TestPhoneCommandAdapter::testKeyHandleCommand ()
{
    int command = 1001;
        	
    m_command_adapter->handleCommand(command);

    QVERIFY( command == m_command );

}

void TestPhoneCommandAdapter::testLeave ()
{
    m_leave = true;	
    int command = 1001;
    
    m_command_adapter->handleCommand(command);
    
    QVERIFY( 0 == m_command );
    m_leave = false;

}

PHONE_QT_TEST_MAIN(TestPhoneCommandAdapter)
#include "unit_tests.moc"
