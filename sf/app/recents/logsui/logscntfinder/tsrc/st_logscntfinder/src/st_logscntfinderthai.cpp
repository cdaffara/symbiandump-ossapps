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
#include "st_logscntfinderthai.h"
#include "logscntfinder.h"
#include "testresultxmlparser.h"

#include <qtcontacts.h>
#include <QtTest/QtTest>
#include <hbinputsettingproxy.h>

void ST_LogsCntFinderThai::initTestCase()
{
}

void ST_LogsCntFinderThai::cleanupTestCase()
{
    
}


void ST_LogsCntFinderThai::init()
{   
    HbInputLanguage eng( QLocale::Thai );
    HbInputSettingProxy::instance()->setGlobalInputLanguage( eng );
    
    m_finder = 0;
    //open symbian database
    m_manager = new QContactManager("symbian");
    m_finder = new LogsCntFinder(*m_manager);
    QVERIFY(m_finder);
    
    // Remove all contacts from the database
    QList<QContactLocalId> cnt_ids = m_manager->contactIds();
    QVERIFY( m_manager->removeContacts(cnt_ids, 0 ) );
    
    cnt_ids = m_manager->contactIds();
    QCOMPARE(cnt_ids.count(), 0);

    for (int i = 0; i < 10; ++i) {
        m_finder->predictiveSearchQuery( QString::number(i) );
        QCOMPARE( m_finder->resultsCount(), 0 );
    }
}

void ST_LogsCntFinderThai::cleanup()
{
    delete m_manager;
    m_manager = 0;
    delete m_finder;
    m_finder = 0;
    HbInputLanguage eng( QLocale::English );
    HbInputSettingProxy::instance()->setGlobalInputLanguage( eng );
    
}

void ST_LogsCntFinderThai::createThaiContacts()
{
    /*Create contacts in Contacts DB
    */
		
    const QChar thaiFName1[] = {0x0E06,0x0E0A}; // map:23
    const QChar thaiLName1[] = {0x0E0E,0x0E14,0x0E19,0x0E1E,0x0E23,0x0E2A };//map 456789
    
    QString first ( thaiFName1, 2 );
    QString last ( thaiFName1, 6 );
    createOneContact( first, last, QString("932472398") );
		
    const QChar thaiFName2[] = {0x0E30,0x0E0A};//#3, 
    const QChar thaiLName2[] = {0x0E2F,0x0E14,0x0E19,0x0E1E,0x0E23,0x0E2A };//*56789
    
    QString first2 ( thaiFName1, 2 );
    QString last2 ( thaiFName1, 6 );
    createOneContact( first2, last2, QString("932472398") );
   
    int contactsCount = m_manager->contactIds().count();
    QCOMPARE(contactsCount, 2);

    
}



void ST_LogsCntFinderThai::createOneContact(QString firstname, QString Lastname, 
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
}



//
// Tests
//

void ST_LogsCntFinderThai::testPredictiveThaiSearchQuery()
{
    HbInputLanguage thai( QLocale::Thai );
    HbInputSettingProxy::instance()->setGlobalInputLanguage( thai );
    		  
    createThaiContacts();

    QEXPECT_FAIL("", "Not supported yet. Issues", Abort );
    
    //There is no recent call in logs UI
    //case 1
    m_finder->predictiveSearchQuery( QString("2") );
    QCOMPARE( m_finder->resultsCount(), 1 );

    m_finder->predictiveSearchQuery( QString("4") );
    QCOMPARE( m_finder->resultsCount(), 1 );

    m_finder->predictiveSearchQuery( QString("402") );
    QCOMPARE( m_finder->resultsCount(), 1 );

    m_finder->predictiveSearchQuery( QString("*4") );
    QCOMPARE( m_finder->resultsCount(), 1 );

    m_finder->predictiveSearchQuery( QString("*402###") );
    QCOMPARE( m_finder->resultsCount(), 1 );

    m_finder->predictiveSearchQuery( QString("*4***00002###") );
    QCOMPARE( m_finder->resultsCount(), 1 );
    
    
}


