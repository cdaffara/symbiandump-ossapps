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
* Description:  Unit tests for StringLoader.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <hbapplication.h>
#include <QSignalSpy>
//#include <hbglobal_p.h>
#include <stringloader.h>
#include "phoneaction.h"
#include "phoneresourceadapter.h"

class TestPhoneStringLoader : public QObject
{
    Q_OBJECT
public:
    TestPhoneStringLoader();
    virtual ~TestPhoneStringLoader();

public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup (); 
    
private slots:
    void testLoad ();
    void testLoadL1 ();
    void testLoadL2 ();
    void testLoadL3 ();
    void testLoadL4 ();
    void testLoadL5 ();
    void testLoadL6 ();
    void testLoadL7 ();
    void testLoadLC1 ();
    void testLoadLC2 ();
    void testLoadLC3 ();
    void testLoadLC4 ();
    void testLoadLC5 ();
    void testLoadLC6 ();
    void testLoadLC7 ();
};

int m_resourceId;
bool m_convertCalled;
bool m_set_text_map;
PhoneResourceAdapter* m_resource_adapter;

PhoneResourceAdapter* PhoneResourceAdapter::Instance (QObject *parent)
{

    Q_UNUSED (parent);
    if ( 0 == m_resource_adapter )
        m_resource_adapter = new PhoneResourceAdapter;
    
    return m_resource_adapter;
}

QMap<PhoneAction::ActionType, PhoneAction *> PhoneResourceAdapter::convert (int symbianResourceId, ...)
{
    m_resourceId = symbianResourceId;
    m_convertCalled = true;
    QMap<PhoneAction::ActionType, PhoneAction *> map;
    if (m_set_text_map) {
        PhoneAction *text = new PhoneAction;
        text->setText(tr("Test"));
        map[PhoneAction::Text] = text; 
    }
        
    return map;
}

TestPhoneStringLoader::TestPhoneStringLoader ()
{
}

TestPhoneStringLoader::~TestPhoneStringLoader ()
{
}

void TestPhoneStringLoader::initTestCase ()
{
}

void TestPhoneStringLoader::cleanupTestCase ()
{
}

void TestPhoneStringLoader::init ()
{
    m_resourceId = -1;
    m_convertCalled = false;
    m_set_text_map = false;
}

void TestPhoneStringLoader::cleanup ()
{
    delete m_resource_adapter;
    m_resource_adapter = 0;
}

void TestPhoneStringLoader::testLoad ()
{
    TBuf<255> string(KNullDesC);
    StringLoader::Load(string, 1000);
    QCOMPARE (m_resourceId, 1000);
    QCOMPARE (m_convertCalled, true);
    
    m_set_text_map = true;
    StringLoader::Load(string, 1000);
    QCOMPARE (m_resourceId, 1000);
    QCOMPARE (m_convertCalled, true);
    
}

void TestPhoneStringLoader::testLoadL1 ()
{
    HBufC *buf = StringLoader::LoadL(1000);
    QCOMPARE (m_resourceId, 1000);
    QCOMPARE (m_convertCalled, true);
    delete buf;
    
    m_set_text_map = true;
    buf = StringLoader::LoadL(1000);
    QCOMPARE (m_resourceId, 1000);
    QCOMPARE (m_convertCalled, true);
    delete buf;
}

void TestPhoneStringLoader::testLoadL2 ()
{
    HBufC *buf = StringLoader::LoadL(1234, 3);
    QCOMPARE (m_resourceId, 1234);
    QCOMPARE (m_convertCalled, true);	
    delete buf;
    
    m_set_text_map = true;
    buf = StringLoader::LoadL(1234, 3);
    QCOMPARE (m_resourceId, 1234);
    QCOMPARE (m_convertCalled, true);   
    delete buf;
}

void TestPhoneStringLoader::testLoadL3 ()
{
    TBuf<255> string(KNullDesC);
    HBufC *buf = StringLoader::LoadL(1234, string);
    QCOMPARE (m_resourceId, 1234);
    QCOMPARE (m_convertCalled, true);	
    delete buf;
    
    m_set_text_map = true;
    buf = StringLoader::LoadL(1234, string);
    QCOMPARE (m_resourceId, 1234);
    QCOMPARE (m_convertCalled, true);   
    delete buf;
}

void TestPhoneStringLoader::testLoadL4 ()
{
    TBuf<255> string(KNullDesC);
    HBufC *buf = StringLoader::LoadL(666, string, 5);
    QCOMPARE (m_resourceId, 666);
    QCOMPARE (m_convertCalled, true);	
    delete buf;
    
    m_set_text_map = true;
    buf = StringLoader::LoadL(666, string, 5);
    QCOMPARE (m_resourceId, 666);
    QCOMPARE (m_convertCalled, true);   
    delete buf;
}

void TestPhoneStringLoader::testLoadL5 ()
{
    CArrayFixFlat<TInt>* list = new(ELeave)CArrayFixFlat<TInt>(2);
    HBufC *buf = StringLoader::LoadL(4321, *list);
    QCOMPARE (m_resourceId, 4321);
    QCOMPARE (m_convertCalled, true);
    delete buf;
    
    m_set_text_map = true;
    buf = StringLoader::LoadL(4321, *list);
    QCOMPARE (m_resourceId, 4321);
    QCOMPARE (m_convertCalled, true);
    delete list;
    delete buf;
}

void TestPhoneStringLoader::testLoadL6 ()
{
    CDesCArray *list = new ( ELeave )CDesCArrayFlat(2);
    HBufC *buf = StringLoader::LoadL(555, *list);
    QCOMPARE (m_resourceId, 555);
    QCOMPARE (m_convertCalled, true);
    delete buf;
    
    m_set_text_map = true;
    buf = StringLoader::LoadL(555, *list);
    QCOMPARE (m_resourceId, 555);
    QCOMPARE (m_convertCalled, true);
    delete list;
    delete buf;
}

void TestPhoneStringLoader::testLoadL7 ()
{
	CDesCArray *list = new ( ELeave )CDesCArrayFlat(2);
    CArrayFixFlat<TInt>* list2 = new(ELeave)CArrayFixFlat<TInt>(2);
    HBufC *buf = StringLoader::LoadL(5678, *list, *list2);
    QCOMPARE (m_resourceId, 5678);
    QCOMPARE (m_convertCalled, true);	
    delete buf;
    
    m_set_text_map = true;
    buf = StringLoader::LoadL(5678, *list, *list2);
    QCOMPARE (m_resourceId, 5678);
    QCOMPARE (m_convertCalled, true);   
    delete list;
    delete list2;
    delete buf;
}

void TestPhoneStringLoader::testLoadLC1 ()
{
    HBufC *buf = 0;
    TRAP_IGNORE( 
            buf = StringLoader::LoadLC(1001); 
            QCOMPARE (m_resourceId, 1001);
            QCOMPARE (m_convertCalled, true);
            CleanupStack::PopAndDestroy( buf );
    );
    
    m_set_text_map = true;
    TRAP_IGNORE(
            buf = StringLoader::LoadLC(1001);
            QCOMPARE (m_resourceId, 1001);
            QCOMPARE (m_convertCalled, true);
            CleanupStack::PopAndDestroy( buf );
    );
}

void TestPhoneStringLoader::testLoadLC2 ()
{
    HBufC *buf = 0;
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(1235, 2);
        QCOMPARE (m_resourceId, 1235);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
    
    m_set_text_map = true;
    
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(1235, 2);
        QCOMPARE (m_resourceId, 1235);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
}

void TestPhoneStringLoader::testLoadLC3 ()
{
    HBufC *buf = 0;
    TBuf<255> string(KNullDesC);
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(1234, string);
        QCOMPARE (m_resourceId, 1234);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
    
    m_set_text_map = true;
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(1234, string);
        QCOMPARE (m_resourceId, 1234);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
}

void TestPhoneStringLoader::testLoadLC4 ()
{
    HBufC *buf = 0;
    TBuf<255> string(KNullDesC);
    
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(667, string, 9);
        QCOMPARE (m_resourceId, 667);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
    
    m_set_text_map = true;
    
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(667, string, 9);
        QCOMPARE (m_resourceId, 667);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
}

void TestPhoneStringLoader::testLoadLC5 ()
{
    HBufC *buf = 0;
    CArrayFixFlat<TInt>* list = new(ELeave)CArrayFixFlat<TInt>(2);
    
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(4322, *list);
        QCOMPARE (m_resourceId, 4322);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
    
    m_set_text_map = true;
    
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(4322, *list);
        QCOMPARE (m_resourceId, 4322);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
    delete list;
}

void TestPhoneStringLoader::testLoadLC6 ()
{
    HBufC *buf = 0;
    CDesCArray *list = new ( ELeave )CDesCArrayFlat(5);
    
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(556, *list);
        QCOMPARE (m_resourceId, 556);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
    
    m_set_text_map = true;
    
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(556, *list);
        QCOMPARE (m_resourceId, 556);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
    delete list;
}

void TestPhoneStringLoader::testLoadLC7 ()
{
    HBufC *buf = 0;
    CDesCArray *list = new ( ELeave )CDesCArrayFlat(3);
    CArrayFixFlat<TInt>* list2 = new(ELeave)CArrayFixFlat<TInt>(2);
    
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(5679, *list, *list2);
        QCOMPARE (m_resourceId, 5679);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
    
    m_set_text_map = true;
    
    TRAP_IGNORE(
        buf = StringLoader::LoadLC(5679, *list, *list2);
        QCOMPARE (m_resourceId, 5679);
        QCOMPARE (m_convertCalled, true);
        CleanupStack::PopAndDestroy( buf );
    );
    
    delete list;
    delete list2;
}

QTEST_MAIN(TestPhoneStringLoader)
#include "unit_tests.moc"
