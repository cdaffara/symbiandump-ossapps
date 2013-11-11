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
#include "st_logscntfinder.h"
#include "logscntfinder.h"
#include "testresultxmlparser.h"

#include <qtcontacts.h>
#include <QtTest/QtTest>
#include <hbinputsettingproxy.h>

#define IGNORE_TEST ""
#define IGNORE_TEST_I -1

#define NOT_EXEC_IF_NOCNTS_SET if ( !mCreateContacts ) return

#define CHECK_HIGHLIGHTS( index, expected )\
        QVERIFY( checkHighlights( index, expected ) )

#define CHECK_RESULTS( count, first, last, highlights )\
    for(int i=0;i<count; i++ ) {\
        if ( highlights != IGNORE_TEST_I && i == 0 ) {\
            QCOMPARE(m_finder->resultAt(0).firstName().at(0).text(), QString(first));\
            QCOMPARE(m_finder->resultAt(0).lastName().at(0).text(), QString(last));\
            CHECK_HIGHLIGHTS( 0,highlights );\
        } else {\
            m_finder->resultAt(i);\
        }\
    }


bool ST_LogsCntFinder::checkHighlights( int index,int expected )
{
    bool foundOne = false;
    const LogsCntEntry& entry = m_finder->resultAt( index );
    int ndx = 0;
    while( !foundOne && ndx < entry.firstName().count() ) {
        foundOne = entry.firstName().at( ndx++ ).highlights() == expected;
    }
    
    ndx = 0;
    while( !foundOne && ndx < entry.lastName().count() ) {
        foundOne = entry.lastName().at( ndx++ ).highlights() == expected;
    }
    
    foundOne = !foundOne ? entry.phoneNumber().highlights() == expected : foundOne;
    
    
    return foundOne;
}

             
void ST_LogsCntFinder::initTestCase()
{
    //open symbian database
    m_manager = new QContactManager("symbian");
}

void ST_LogsCntFinder::cleanupTestCase()
{
    delete m_manager;
    m_manager = 0;
    
}


void ST_LogsCntFinder::init()
{   
    HbInputLanguage eng( QLocale::English );
    HbInputSettingProxy::instance()->setGlobalInputLanguage( eng );
    
    m_finder = 0;
    m_finder = new LogsCntFinder(*m_manager);
    QVERIFY(m_finder);
    
    if ( mCreateContacts ) {
        // Remove all contacts from the database
        QList<QContactLocalId> cnt_ids = m_manager->contactIds();
        qDebug() << "contacts now before deleting" << cnt_ids.count();
    
        m_manager->removeContacts(cnt_ids, 0 );
        cnt_ids = m_manager->contactIds();
        QCOMPARE(cnt_ids.count(), 0);

        for (int i = 0; i < 10; ++i) {
            m_finder->predictiveSearchQuery( QString::number(i) );
            QCOMPARE( m_finder->resultsCount(), 0 );
        }
        m_finder->predictiveSearchQuery( QString("") );
        
    }
    
}

void ST_LogsCntFinder::cleanup()
{
    delete m_finder;
    m_finder = 0;
    HbInputLanguage eng( QLocale::English );
    HbInputSettingProxy::instance()->setGlobalInputLanguage( eng );
    
}

void ST_LogsCntFinder::createContacts()
{
    if ( mCreateContacts ) {

        /*Create contacts in Contacts DB for keymap testing
                    Stefann    Yadira
                    Jonn         Ennon
                    Maria-Zola     Jones
                    Levis         Augustin Zi
                    Nancy       Csoma
                    Olga          Baraniktestteste
                    Petter       Harhai
                    Queen      Fesko
                    Rose        Galisin
                    Sasha      Dofzin
                    Tisha       Iatzkovits
                    Wilda       Lazar
                    Una Vivi   Kantsak
                    */
        createOneContact( QString("Stefann"), QString("Yadira "), QString("932472398") );
        createOneContact( QString("Jonn"), QString("Ennon"), QString("932472398") );
        createOneContact( QString("Maria-Zola"), QString("Jones"), QString("932472398") );
        createOneContact( QString("Levis"), QString("Augustin Zi"), QString("932472398") );
        createOneContact( QString("Nancy"), QString("Csoma"), QString("932472398") );
        createOneContact( QString("Olga"), QString("Baraniktestteste"), QString("932472398") );
        createOneContact( QString("Petter"), QString("Harhai"), QString("932472398") );
        createOneContact( QString("Queen"), QString("Fesko"), QString("932472398") );
        createOneContact( QString("Rose"), QString("Galisin"), QString("932472398") );
        createOneContact( QString("Sasha"), QString("Dofzin"), QString("932472398") );
        createOneContact( QString("Tisha"), QString("Iatzkovits"), QString("932472398") );
        createOneContact( QString("Wilda"), QString("Lazar"), QString("932472398") );
        createOneContact( QString("Una Vivi"), QString("Kantsak"), QString("932472398") );

        int contactsCount = m_manager->contactIds().count();
        QCOMPARE(contactsCount, 13);

    }    
}

/* 
Dlice 00202       Qwerty        45789348
#Paula 2003       Augustin Ci   0078945617 
Paula 02010       Ezerty Adam   78945617
Ced               Y,g           +78945617
Jari-Pekka        Baraniktestteste 78945617
*/
void ST_LogsCntFinder::createContactsForQueryZero()
{
    if ( mCreateContacts ) {
        createContacts();
        createOneContact( QString("Dlice 00202"), QString("Qwerty"), QString("45789348") );
        createOneContact( QString("#Paula 2003"), QString("Augustin Ci"), QString("0078945617") );
        createOneContact( QString("Paula 002010"), QString("Ezerty Adam"), QString("78945617") );
        createOneContact( QString("Ced"), QString("Y0g"), QString("+78945617") );
        createOneContact( QString("Jari-Pekka"), QString(" "), QString("78945617") );
    
        int contactsCount = m_manager->contactIds().count();
        QCOMPARE(contactsCount, 18);
    }
}

void ST_LogsCntFinder::createContactsWithSpecialChars()
{
    if ( mCreateContacts ) {
        
        createOneContact( QString("Hannu%"), QString(""), QString("932472398") );
        createOneContact( QString("%Hannu"), QString(""), QString("932472398") );
    }    
}

void ST_LogsCntFinder::createHistoryEvents()
{ 
  //inserted inreverse order
  createLogEvent( QString("Stefann Albert"), QString("Fedrernn"), QString("932472398") );
  createLogEvent( QString("Jonn"), QString("Lennon"), QString("932472398") );
  createLogEvent( QString("Dim-Petter"), QString("Jones"), QString("932472398") );
  
}

void ST_LogsCntFinder::createLogEvent(QString firstname, QString lastname, 
                                      QString phnumber)
{
  LogsCntEntryHandle* dummy = 0;
  
  LogsCntEntry* logEvent = new LogsCntEntry( *dummy, 0 );
  logEvent->setFirstName( firstname );
  logEvent->setLastName( lastname );
  logEvent->setPhoneNumber( phnumber );
  
  m_finder->insertEntry(0, logEvent );
  
  
}

void ST_LogsCntFinder::createContactsForQueryOrder()
{
    if ( mCreateContacts ) {
        createContacts();
        createOneContact( QString("Anna"), QString("Qwerty"), QString("45789348") );
        createOneContact( QString("Paula"), QString("Qwerty"), QString("78945617") );
        createOneContact( QString("Paula"), QString("Azerty"), QString("78945617") );
    
        int contactsCount = m_manager->contactIds().count();
        QCOMPARE(contactsCount, 16);
    }
}



void ST_LogsCntFinder::createOneContact(QString firstname, QString Lastname, 
                                                 QString phnumber)
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    QContact phonecontact;
    // Stefann Fedrernn +02644424423 ste.Fed@nokia.com
    // Contact details
    QContactName contactName;
    contactName.setFirstName(firstname);
    contactName.setLastName(Lastname);
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber(phnumber);
    phonecontact.saveDetail(&number);
    
    m_manager->saveContact(&phonecontact);
    qDebug() << "ST_LogsCntFinder::createOneContact done";
}



//
// Tests
//

/* Test itut keymap predictive search, checking that press key "2", records with names starting letters "A, B, C" are matched;
Press key "3", records with names starting letters "D,E,F" are matched;
Press key "4", records with names starting letters "G,H,I" are matched;
Press key "5", records with names starting letters "J,K,L" are matched;
Press key "6", records with names starting letters "M,O,N" are matched;
Press key "7", records with names starting letters "P,Q,R,S" are matched;
Press key "8", records with names starting letters "T,U,V" are matched;
Press key "9", records with names starting letters "W,X,Y,Z" are matched;
Press key "0", records with names starting letters "space" etc. are matched;
Press key "1", records with names starting letters "-,Ä,Ö" etc. are matched;
*/
void ST_LogsCntFinder::testKeymap_2()
{
    NOT_EXEC_IF_NOCNTS_SET;
    
    createContacts();
    const LogsCntEntry* data; 
    m_finder->predictiveSearchQuery( QString("2") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    data = &m_finder->resultAt( 0 );
    QCOMPARE( data->firstName().count(), 1 );
}

void ST_LogsCntFinder::testKeymap_3()
{
    NOT_EXEC_IF_NOCNTS_SET;
    
    createContacts();
    const LogsCntEntry* data; 
    m_finder->predictiveSearchQuery( QString("3") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    data = &m_finder->resultAt( 0 );
    QCOMPARE( data->firstName().count(), 1 );
}

void ST_LogsCntFinder::testKeymap_4()
{
    NOT_EXEC_IF_NOCNTS_SET;
    
    createContacts();
    const LogsCntEntry* data; 
    m_finder->predictiveSearchQuery( QString("4") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    data = &m_finder->resultAt( 0 );
    QCOMPARE( data->firstName().count(), 1 );
}

void ST_LogsCntFinder::testKeymap_5()
{
    NOT_EXEC_IF_NOCNTS_SET;
    
    createContacts();
    const LogsCntEntry* data; 
    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 5 );
    data = &m_finder->resultAt( 0 );
    QCOMPARE( data->firstName().count(), 1 );
}

void ST_LogsCntFinder::testKeymap_6()
{
    NOT_EXEC_IF_NOCNTS_SET;
    
    createContacts();
    const LogsCntEntry* data; 
    m_finder->predictiveSearchQuery( QString("6") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    data = &m_finder->resultAt( 0 );
    QCOMPARE( data->firstName().count(), 1 );
}

void ST_LogsCntFinder::testKeymap_7()
{
    NOT_EXEC_IF_NOCNTS_SET;
    
    createContacts();
    const LogsCntEntry* data; 
    m_finder->predictiveSearchQuery( QString("7") );
    QCOMPARE( m_finder->resultsCount(), 5 );
    data = &m_finder->resultAt( 0 );
    QCOMPARE( data->firstName().count(),1 );
}

void ST_LogsCntFinder::testKeymap_8()
{
    NOT_EXEC_IF_NOCNTS_SET;
    
    createContacts();
    const LogsCntEntry* data; 
    m_finder->predictiveSearchQuery( QString("8") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    data = &m_finder->resultAt( 0 );
    QCOMPARE( data->firstName().count(), 1 );
}

void ST_LogsCntFinder::testKeymap_9()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createContacts();
    const LogsCntEntry* data; 
    m_finder->predictiveSearchQuery( QString("9") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    data = &m_finder->resultAt( 0 );
    QCOMPARE( data->firstName().count(), 1 );
}


/* Test zero query search: 1. zero between "1-9" numbers, then first zero works as "AND" statement; 
2. (multiple) zero at beginning; 3. (multiple) zero at the end; 
4-5. multi-zeros between "1-9" numbers, only the first works as "AND" statement;
6. Query limit is 15, the 16th is ignored, and first 0 works as "AND" statement */


void ST_LogsCntFinder::testPredictiveSearchQueryZeroStart()
{
    createContactsForQueryZero();
    
    m_finder->predictiveSearchQuery( QString("002") );
    QCOMPARE( m_finder->resultsCount(), 7 );//002 + 2 matches
    //Levis, Augustin Zi
    //Nancy, Csoma
    //Olga, Baraniktestteste
    //Dlice 00202, Qwerty
    //#Paula 2003, Augustin Ci
    //Paula 002010, Ezerty Adam
    //Ced, Y0g

    m_finder->predictiveSearchQuery( QString("") );
    
    m_finder->predictiveSearchQuery( QString("00202") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    //Dlice 00202, Qwerty
    //#Paula 2003, Augustin Ci <= 2003 was found with pattern 002
    //Paula 002010, Ezerty Adam
    
    
}

void ST_LogsCntFinder::testPredictiveSearchQueryZeroStartZeroEnd()
{
    createContactsForQueryZero();

    m_finder->predictiveSearchQuery( QString("200") );//db
    QCOMPARE( m_finder->resultsCount(), 6 );
    //Levis, Augustin Zi
    //Nancy, Csoma
    //Olga, Baraniktestteste
    //#Paula 2003, Augustin Ci
    //Paula 002010, Ezerty Adam
    //Ced, Y0g
    
    m_finder->predictiveSearchQuery( QString("") );
    
    m_finder->predictiveSearchQuery( QString("20020") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    //#Paula 2003, Augustin Ci
    
    m_finder->predictiveSearchQuery( QString("") );
    
    m_finder->predictiveSearchQuery( QString("200200") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    //#Paula 2003, Augustin Ci
    
    m_finder->predictiveSearchQuery( QString("") );
    
    m_finder->predictiveSearchQuery( QString("2002000") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    //#Paula 2003, Augustin Ci
    
}

void ST_LogsCntFinder::testPredictiveSearchQueryZeroMiddle()
{
    createContactsForQueryZero();

    m_finder->predictiveSearchQuery( QString("56603") );
    QCOMPARE( m_finder->resultsCount(), 1 );
}

void ST_LogsCntFinder::testPredictiveSearchQueryMultiZerosMiddle()
{
    createContactsForQueryZero();

    m_finder->predictiveSearchQuery( QString("2003") );
    QCOMPARE( m_finder->resultsCount(), 2 );
}

void ST_LogsCntFinder::testPredictiveSearchQueryZeroMiddleLong()
{
    createContactsForQueryZero();

    m_finder->predictiveSearchQuery( QString("227264583788065") );
    QCOMPARE( m_finder->resultsCount(), 1 );

    m_finder->predictiveSearchQuery( QString("227264583788065090") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    
}

void ST_LogsCntFinder::testPredictiveSearchQueryMultiZerosAndZeroMiddle()
{
    createContactsForQueryZero();

    m_finder->predictiveSearchQuery( QString("200904") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    
}


// Test basic predictive search, all records with names starting letters "J, K, L" are matched

// 5 -> 56 -> 5 -> 56    (not all cached)
void ST_LogsCntFinder::testPredictiveSearchQueryPartialCached()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createContacts();

    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 5 );
    //see half of matched results
    CHECK_RESULTS( 3, "Jonn", "Ennon", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
    m_finder->predictiveSearchQuery( QString("56") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    //see half of matched results
    CHECK_RESULTS( 1, "Jonn", "Ennon", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 5 );
    CHECK_RESULTS( 3, "Jonn", "Ennon", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights

    m_finder->predictiveSearchQuery( QString("56") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    //see half of matched results
    CHECK_RESULTS( 1, "Jonn", "Ennon", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
}

// 5 -> 56 -> 5 -> 56   (all cached)
void ST_LogsCntFinder::testPredictiveSearchQueryFullyCached()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createContacts();

    //case 1
    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 5 );
    //see all matched results
    CHECK_RESULTS( 5, "Jonn", "Ennon", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
    m_finder->predictiveSearchQuery( QString("56") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    //see all of matched results
    CHECK_RESULTS( 2, "Jonn", "Ennon", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 5 );
    //see all of matched results
    CHECK_RESULTS( 5, "Jonn", "Ennon", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights

    m_finder->predictiveSearchQuery( QString("56") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    //see all of matched results
    CHECK_RESULTS( 2, "Jonn", "Ennon", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
}

// 6 -> 69 (no match) -> 692 (no match) -> 69 (no match) -> 6 -> 69    (not all cached)
void ST_LogsCntFinder::testPredictiveSearchQueryPartialCachedNoResults()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createContacts();
		
    m_finder->predictiveSearchQuery( QString("6") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    //see half of matched results
    CHECK_RESULTS( 2, "Maria-Zola", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
    m_finder->predictiveSearchQuery( QString("69") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    m_finder->predictiveSearchQuery( QString("692") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    m_finder->predictiveSearchQuery( QString("69") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    
    m_finder->predictiveSearchQuery( QString("6") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    //see half of matched results
    CHECK_RESULTS( 2, "Maria-Zola", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
    m_finder->predictiveSearchQuery( QString("69") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    
}

// 6 -> 69 (no match) -> 692 (no match) -> 69 (no match) -> 6 -> 69    (all cached)
void ST_LogsCntFinder::testPredictiveSearchQueryFullyCachedNoResults()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createContacts();
                
    m_finder->predictiveSearchQuery( QString("6") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    //see all of matched results
    CHECK_RESULTS( 3, "Maria-Zola", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
    m_finder->predictiveSearchQuery( QString("69") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    m_finder->predictiveSearchQuery( QString("692") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    m_finder->predictiveSearchQuery( QString("69") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    
    m_finder->predictiveSearchQuery( QString("6") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    //see all of matched results
    CHECK_RESULTS( 3, "Maria-Zola", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
    m_finder->predictiveSearchQuery( QString("69") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    
    
}

// 
void ST_LogsCntFinder::testPredictiveSearchQueryPartialCachedZeroCase()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createContacts();
    
    m_finder->predictiveSearchQuery( QString("2") );//db
    QCOMPARE( m_finder->resultsCount(), 3 );
    //Augustin Zi
    CHECK_RESULTS( 2, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

    m_finder->predictiveSearchQuery( QString("20") );//db
    QCOMPARE( m_finder->resultsCount(), 3 );
    //Augustin Zi
    CHECK_RESULTS( 2, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

    
    m_finder->predictiveSearchQuery( QString("209") );//db
    QCOMPARE( m_finder->resultsCount(), 1 );
    //Augustin Zi
    CHECK_RESULTS( 1, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

    
    m_finder->predictiveSearchQuery( QString("20") );//db
    QCOMPARE( m_finder->resultsCount(), 3 );
    //Augustin Zi
    CHECK_RESULTS( 2, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("2") );//db
    QCOMPARE( m_finder->resultsCount(), 3 );
    //Augustin Zi
    CHECK_RESULTS( 2, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

       
}


void ST_LogsCntFinder::testPredictiveSearchQueryFullyCachedZerosStartCase()
{
    createContactsForQueryZero();
    
    m_finder->predictiveSearchQuery( QString("0") ); //db
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("00") );//db
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("002") );//db
    QCOMPARE( m_finder->resultsCount(), 7 );//002 + 2 matches
    CHECK_RESULTS( 7, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("0020") );//db
    QCOMPARE( m_finder->resultsCount(), 7 );
    CHECK_RESULTS( 7, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("00202") );//cache
    QCOMPARE( m_finder->resultsCount(), 3 );
    CHECK_RESULTS( 3, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    //Dlice 00202, Qwerty
    //#Paula 2003, Augustin Ci
    //Paula 002010, Ezerty Adam
    
    m_finder->predictiveSearchQuery( QString("0020") );//db
    QCOMPARE( m_finder->resultsCount(), 7 );
    CHECK_RESULTS( 7, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("002") );//db
    QCOMPARE( m_finder->resultsCount(), 7 );//002 + 2 matches
    CHECK_RESULTS( 7, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("00") );//db
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
       
    m_finder->predictiveSearchQuery( QString("0") );//db
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
}

void ST_LogsCntFinder::testPredictiveSearchQueryFullyCachedZerosStartCase_2()
{
    createContactsForQueryZero();
    
    m_finder->predictiveSearchQuery( QString("0") ); //db
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("00") );//db
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("002") );//db
    QCOMPARE( m_finder->resultsCount(), 7 );//002 + 2 matches
    CHECK_RESULTS( 7, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("0023") );//cache
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights
    
    //Paula 002010, Ezerty Adam
    //Ced, Y0g
    
    
    m_finder->predictiveSearchQuery( QString("00230") );//cache
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("0023") );//db
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("002") );//db
    QCOMPARE( m_finder->resultsCount(), 7 );//002 + 2 matches
    CHECK_RESULTS( 7, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("00") );//db
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("0") ); //db
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, IGNORE_TEST, IGNORE_TEST, IGNORE_TEST_I ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
}

// 
void ST_LogsCntFinder::testPredictiveSearchQueryFullyCachedZerosEndCase()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createContacts();
    
    m_finder->predictiveSearchQuery( QString("2") ); //db
    QCOMPARE( m_finder->resultsCount(), 3 );
    CHECK_RESULTS( 3, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

    m_finder->predictiveSearchQuery( QString("20") );//cache
    QCOMPARE( m_finder->resultsCount(), 3 );
    CHECK_RESULTS( 3, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("200") );//cache
    QCOMPARE( m_finder->resultsCount(), 3 );
    CHECK_RESULTS( 3, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("2009") );//cache
    QCOMPARE( m_finder->resultsCount(), 1 );
    CHECK_RESULTS( 1, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("200") );//db
    QCOMPARE( m_finder->resultsCount(), 3 );
    CHECK_RESULTS( 3, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("20") );//db
    QCOMPARE( m_finder->resultsCount(), 3 );
    CHECK_RESULTS( 3, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("2") );//db
    QCOMPARE( m_finder->resultsCount(), 3 );
    CHECK_RESULTS( 3, "Levis", "Augustin", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
       
}

//There is recent call in logs, no contacts DB
void ST_LogsCntFinder::testPredictiveSearchQueryLogs()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createHistoryEvents();

    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

    m_finder->predictiveSearchQuery( QString("53") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    CHECK_RESULTS( 1, "Jonn", "Lennon", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("539") );
    QCOMPARE( m_finder->resultsCount(), 0 );

    m_finder->predictiveSearchQuery( QString("53") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    CHECK_RESULTS( 1, "Jonn", "Lennon", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
}

void ST_LogsCntFinder::testPredictiveSearchQueryLogsZeroCase()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createHistoryEvents();
    
    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

    m_finder->predictiveSearchQuery( QString("50") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("503") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    CHECK_RESULTS( 1, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("50") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
        
}

//There is recent call in logs, and contacts DB
void ST_LogsCntFinder::testPredictiveSearchQueryLogsContactsPartialCached()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createHistoryEvents();
    createContacts();

    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 7 );
    CHECK_RESULTS( 5, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("56") );
    QCOMPARE( m_finder->resultsCount(), 4 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("569") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    
    m_finder->predictiveSearchQuery( QString("56") );
    QCOMPARE( m_finder->resultsCount(), 4 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 7 );
    CHECK_RESULTS( 5, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    
}

void ST_LogsCntFinder::testPredictiveSearchQueryLogsContactsFullyCached()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createHistoryEvents();
    createContacts();

    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 7 );
    CHECK_RESULTS( 7, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("56") );
    QCOMPARE( m_finder->resultsCount(), 4 );
    CHECK_RESULTS( 4, "Dim-Petter", "Jones", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("569") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    
    m_finder->predictiveSearchQuery( QString("56") );
    QCOMPARE( m_finder->resultsCount(), 4 );
    CHECK_RESULTS( 4, "Dim-Petter", "Jones", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 7 );
    CHECK_RESULTS( 7, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
}

void ST_LogsCntFinder::testPredictiveSearchQueryLogsContactsZeroCase()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createHistoryEvents();
    createContacts();

    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 7 );
    CHECK_RESULTS( 5, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

    m_finder->predictiveSearchQuery( QString("50") );
    QCOMPARE( m_finder->resultsCount(), 7 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("503") );
    QCOMPARE( m_finder->resultsCount(), 2 );
    CHECK_RESULTS( 2, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("50") );
    QCOMPARE( m_finder->resultsCount(), 7 );
    CHECK_RESULTS( 5, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

    m_finder->predictiveSearchQuery( QString("5") );
    QCOMPARE( m_finder->resultsCount(), 7 );
    CHECK_RESULTS( 5, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
}

void ST_LogsCntFinder::testPredictiveSearchQueryLogsContactsPhoneNumberMatch()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createHistoryEvents();
    createContacts();

    m_finder->predictiveSearchQuery( QString("9") );
    QCOMPARE( m_finder->resultsCount(), 6 ); //3 history + 3 contacts
    CHECK_RESULTS( 5, "Dim-Petter", "Jones", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 

    m_finder->predictiveSearchQuery( QString("93") );
    QCOMPARE( m_finder->resultsCount(), 3 );
    CHECK_RESULTS( 1, "Dim-Petter", "Jones", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
}


// Test query limit is 15, the 16th digit is ignored
void ST_LogsCntFinder::testPredictiveSearchQueryLimit()
{
    NOT_EXEC_IF_NOCNTS_SET;
    createContacts();

    // 9 digits
    m_finder->predictiveSearchQuery( QString("227264583") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    // 10 digits
    m_finder->predictiveSearchQuery( QString("2272645837") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    // 11 digits
    m_finder->predictiveSearchQuery( QString("22726458378") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    // 12 digits
    m_finder->predictiveSearchQuery( QString("227264583788") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    // 13 digits
    m_finder->predictiveSearchQuery( QString("2272645837883") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    // 14 digits
    m_finder->predictiveSearchQuery( QString("22726458378837") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    // 15 digits
    m_finder->predictiveSearchQuery( QString("227264583788378") );
    QCOMPARE( m_finder->resultsCount(), 1 );

    // 16 digits
    m_finder->predictiveSearchQuery( QString("2272645837883783") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    QCOMPARE(m_finder->resultAt(0).firstName().at(0).text(), QString("Olga"));
    QCOMPARE(m_finder->resultAt(0).lastName().at(0).text(), QString("Baraniktestteste"));
}

void ST_LogsCntFinder::testQueryOrder()
{
    NOT_EXEC_IF_NOCNTS_SET;
    
    createContactsForQueryOrder();

    m_finder->predictiveSearchQuery( QString("7") );
    QCOMPARE( m_finder->resultsCount(), 8 );
    
    QCOMPARE(m_finder->resultAt(0).firstName().at(0).text(), QString("Anna"));
    QCOMPARE(m_finder->resultAt(0).lastName().at(0).text(), QString("Qwerty"));

    QCOMPARE(m_finder->resultAt(1).firstName().at(0).text(), QString("Paula"));
    QCOMPARE(m_finder->resultAt(1).lastName().at(0).text(), QString("Azerty"));

    QCOMPARE(m_finder->resultAt(2).firstName().at(0).text(), QString("Paula"));
    QCOMPARE(m_finder->resultAt(2).lastName().at(0).text(), QString("Qwerty"));

    QCOMPARE(m_finder->resultAt(3).firstName().at(0).text(), QString("Petter"));
    QCOMPARE(m_finder->resultAt(3).lastName().at(0).text(), QString("Harhai"));

    QCOMPARE(m_finder->resultAt(4).firstName().at(0).text(), QString("Queen"));
    QCOMPARE(m_finder->resultAt(4).lastName().at(0).text(), QString("Fesko"));

    QCOMPARE(m_finder->resultAt(5).firstName().at(0).text(), QString("Rose"));
    QCOMPARE(m_finder->resultAt(5).lastName().at(0).text(), QString("Galisin"));

    QCOMPARE(m_finder->resultAt(6).firstName().at(0).text(), QString("Sasha"));
    QCOMPARE(m_finder->resultAt(6).lastName().at(0).text(), QString("Dofzin"));

    QCOMPARE(m_finder->resultAt(7).firstName().at(0).text(), QString("Stefann"));
    QCOMPARE(m_finder->resultAt(7).lastName().at(0).text(), QString("Yadira"));
}

void ST_LogsCntFinder::testContactWithSpecialChars()
{
    NOT_EXEC_IF_NOCNTS_SET;
    
    //Hannu%
    //%Hannu
    createContactsWithSpecialChars();
    
    m_finder->predictiveSearchQuery( QString("4") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    CHECK_RESULTS( 1, "Hannu%", "", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("42") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    CHECK_RESULTS( 1, "Hannu%", "", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("4") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    CHECK_RESULTS( 1, "Hannu%", "", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("") );
    QCOMPARE( m_finder->resultsCount(), 0 );
    
    m_finder->predictiveSearchQuery( QString("1") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    CHECK_RESULTS( 1, "%Hannu", "", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("14") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    CHECK_RESULTS( 1, "%Hannu", "", 2 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
    m_finder->predictiveSearchQuery( QString("1") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    CHECK_RESULTS( 1, "%Hannu", "", 1 ); //SET: cache size, CHECK: 1. result fn, 1. result ln, highlights 
    
}

