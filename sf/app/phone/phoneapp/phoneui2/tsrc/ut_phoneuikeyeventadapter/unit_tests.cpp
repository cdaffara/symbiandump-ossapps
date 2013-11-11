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
#include "phoneuikeyeventadapter.h"
#include "cphoneuicontroller_stub.h"
#include "phoneconstants.h"


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

class TestPhoneKeyEventAdapter : public QObject
{
    Q_OBJECT
    
public:
    TestPhoneKeyEventAdapter ();
    ~TestPhoneKeyEventAdapter ();


public slots:
    void initTestCase ();
    void cleanupTestCase (); 
    void init ();
    void cleanup ();
    
private slots:
    void testKeyPressEvents();
    void testKeyReleaseEvents();
    void testKeyPressEventsByQtKeys();
    void testLeave();



private:
    PhoneUIKeyEventAdapter *m_keyadapter; // class under test
    CPhoneUIController_Stub *m_uicontrol;

};


TestPhoneKeyEventAdapter::TestPhoneKeyEventAdapter ()
{    
}

TestPhoneKeyEventAdapter::~TestPhoneKeyEventAdapter ()
{
}

void TestPhoneKeyEventAdapter::initTestCase ()
{	
}

void TestPhoneKeyEventAdapter::cleanupTestCase ()
{
}

void TestPhoneKeyEventAdapter::init ()
{
    m_uicontrol = new CPhoneUIController_Stub(); 
    m_keyadapter = new PhoneUIKeyEventAdapter(*m_uicontrol);
}

void TestPhoneKeyEventAdapter::cleanup ()
{
    delete m_uicontrol;
    delete m_keyadapter;
}

void TestPhoneKeyEventAdapter::testKeyPressEvents ()
{
    quint32 scanCode = 123;
    quint32 code = 123;
    quint32 modifiers = 0; 
    QKeyEvent *keyEvent = QKeyEvent::createExtendedKeyEvent( 
        QEvent::KeyPress, 123, Qt::NoModifier, scanCode, code, modifiers );
        	
    m_keyadapter->keyPressed(keyEvent);
    
    QVERIFY( 0 == m_repeats );
    QVERIFY( scanCode == m_scan_code_down );
    QVERIFY( code == m_code_down );
    
    delete keyEvent;
}

void TestPhoneKeyEventAdapter::testKeyReleaseEvents ()
{
    quint32 scanCode = 100;
    quint32 code = 100;
    quint32 modifiers = 0; 
    QKeyEvent *keyEvent = QKeyEvent::createExtendedKeyEvent( 
        QEvent::KeyRelease, code, Qt::NoModifier, scanCode, code, modifiers );
        	
    m_keyadapter->keyReleased(keyEvent);
    
    QVERIFY( 0 == m_repeats );
    QVERIFY( scanCode == m_scan_code );
    QVERIFY( code == m_code );
    
    delete keyEvent;
    
}

void TestPhoneKeyEventAdapter::testKeyPressEventsByQtKeys ()
{
    quint32 scanCode = 100;
    quint32 code = 20;
    quint32 modifiers = 0; 
    QKeyEvent *keyEvent = QKeyEvent::createExtendedKeyEvent( 
        QEvent::KeyRelease, Qt::Key_Yes, Qt::NoModifier, scanCode, code, modifiers );
            
    m_keyadapter->keyPressed(keyEvent);
    
    QVERIFY( 0 == m_repeats );
    QVERIFY( EKeyYes == m_scan_code_down );
    QVERIFY( EKeyYes == m_code_down );
    
    delete keyEvent;
    
    keyEvent = QKeyEvent::createExtendedKeyEvent( 
            QEvent::KeyRelease, Qt::Key_No, Qt::NoModifier, scanCode, code, modifiers );
                
    m_keyadapter->keyPressed(keyEvent);
        
    QVERIFY( 0 == m_repeats );
    QVERIFY( EKeyNo == m_scan_code_down );
    QVERIFY( EKeyNo == m_code_down );
    
    delete keyEvent;
    
    keyEvent = QKeyEvent::createExtendedKeyEvent( 
            QEvent::KeyRelease, Qt::Key_NumberSign, Qt::NoModifier, scanCode, code, modifiers );
                
    m_keyadapter->keyPressed(keyEvent);
        
    QVERIFY( 0 == m_repeats );
    QVERIFY( KPhoneDtmfHashCharacter == m_scan_code_down );
    QVERIFY( KPhoneDtmfHashCharacter == m_code_down );
    
    delete keyEvent;
    
}

void TestPhoneKeyEventAdapter::testLeave ()
{
    m_leave = true;	
    quint32 scanCode = 100;
    quint32 code = 20;
    quint32 modifiers = 0; 
    QKeyEvent *keyEvent = QKeyEvent::createExtendedKeyEvent( 
        QEvent::KeyRelease, 0, Qt::NoModifier, scanCode, code, modifiers );
    
    m_keyadapter->keyPressed(keyEvent);
    
    QVERIFY( 0 == m_scan_code );
    QVERIFY( 0 == m_code );
        	
    m_keyadapter->keyReleased(keyEvent);
    
    QVERIFY( 0 == m_scan_code );
    QVERIFY( 0 == m_code );
    
    m_leave = false;
    
    delete keyEvent;
}

PHONE_QT_TEST_MAIN(TestPhoneKeyEventAdapter)
#include "unit_tests.moc"
