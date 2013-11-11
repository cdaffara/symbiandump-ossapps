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
#include "st_logscntfinderqwerty.h"
#include "testresultxmlparser.h"

#include <qtcontacts.h>
#include <QtTest/QtTest>
#include <hbinputsettingproxy.h>


#define ADD_CONTACT( id, first, last, pnumber, mail1, mail2, mail3 )\
    QContact phonecontact##id;\
    QContactName contactName##id;\
    if ( QString( first).length() ) {\
       contactName##id.setFirstName( first );\
    }\
    if ( QString( last ).length() ) {\
       contactName##id.setLastName( last );\
    }\
    QVERIFY(phonecontact##id.saveDetail(&contactName##id));\
    if ( QString( pnumber ).length() ) {\
       QContactPhoneNumber number;\
       number.setContexts("Home");\
       number.setSubTypes("Mobile");\
       number.setNumber( pnumber );\
       phonecontact##id.saveDetail(&number);\
    }\
    if ( QString( mail1 ).length() ) {\
       QContactEmailAddress email;\
       email.setEmailAddress( mail1 );\
       phonecontact##id.saveDetail(&email);\
    }\
    if ( QString( mail2 ).length() ) {\
       QContactEmailAddress email;\
       email.setEmailAddress( mail2 );\
       phonecontact##id.saveDetail(&email);\
    }\
    if ( QString( mail3 ).length() ) {\
       QContactEmailAddress email;\
       email.setEmailAddress( mail3 );\
       phonecontact##id.saveDetail(&email);\
    }\
    QVERIFY(m_manager->saveContact(&phonecontact##id))


void ST_LogsCntFinderQwerty::initTestCase()
{
}

void ST_LogsCntFinderQwerty::cleanupTestCase()
{
    
}


void ST_LogsCntFinderQwerty::init()
{   
    HbInputLanguage eng( QLocale::English );
    HbInputSettingProxy::instance()->setGlobalInputLanguage( eng );
    
    //open symbian database
    m_manager = new QContactManager("symbian");
    
    // Remove all contacts from the database
    QList<QContactLocalId> cnt_ids = m_manager->contactIds();
    m_manager->removeContacts(cnt_ids, 0 );
    cnt_ids = m_manager->contactIds();
    QCOMPARE(cnt_ids.count(), 0);
    
}

void ST_LogsCntFinderQwerty::cleanup()
{
    delete m_manager;
    m_manager = 0;
    HbInputLanguage eng( QLocale::English );
    HbInputSettingProxy::instance()->setGlobalInputLanguage( eng );
    
}

void ST_LogsCntFinderQwerty::createContacts()
{
    
    // Remove all contacts from the database
    QList<QContactLocalId> cnt_ids = m_manager->contactIds();
    m_manager->removeContacts(cnt_ids, 0 );
    cnt_ids = m_manager->contactIds();
    QVERIFY(0 == cnt_ids.count());
    
    //         'id' first      last           phonenumber        email1                        email2                      email3  
    ADD_CONTACT( 1, "Stefann", "Yadira",      "0035893424558",   "syadira@gmail.com",          "stefann.yadira@nokia.com", "" );
    ADD_CONTACT( 2, "",        "",            "00861048294754",  "company1@sohu.com",          "",                         "" );
    ADD_CONTACT( 3, "Olga",    "",            "0938485604",      "olga089@ovi.com",            "test1@yahoo.com",          "olga.baranik.s@nordea.com" );
    ADD_CONTACT( 4, "",        "Fesko",       "+35892874754",    "test2@ovi.com",              "",                         "" );
    ADD_CONTACT( 5, "Petter",  "Harhai",      "0402348850",      "",                           "",                         "" );
    ADD_CONTACT( 6, "Levis",   "Augustin Zi", "",                "levis.augustin.z@nokia.com", "laugustin@ovi.com",        "" );
    ADD_CONTACT( 7, "Matt",    "Cook",        "",                "shymatt@live.com",           "",                         "" );
    
}


/* expected results   
    Search steps: search "s", contact (1 & 7) match;
                  search "s ya", contact (1) match;
                  search "syadira", contact (1) match;
                  search "pett", no match;
                  search "levis.augu_whatever", contact (6) match;
                  search "olga.b", contact (3) match;
*/


//search "s", contact (1 & 7) match
void ST_LogsCntFinderQwerty::testPredictiveEmailSearchNameAndEmailMatch()
{

    createContacts();
    
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress );
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    QContactLocalId cid;
    QContact contact;
    QContactName contactName;

    QString pattern = QString("s") + QChar(30) + QString("vqwerty");
    df.setValue( pattern );
    QList<QContactLocalId> cnt_ids = m_manager->contactIds( df );
    QCOMPARE( cnt_ids.count(), 2 );
    cid = cnt_ids.at( 0 );    
    contact = m_manager->contact( cid );
    contactName = contact.detail( QContactName::DefinitionName );
    QCOMPARE( contactName.value( QContactName::FieldFirstName ), QString("Matt" ) );
    QCOMPARE( contact.details( QContactEmailAddress::DefinitionName ).count(), 1 );
    QCOMPARE( contact.details( QContactEmailAddress::DefinitionName )[0].value( QContactEmailAddress::FieldEmailAddress ), QString( "shymatt@live.com" ) );
    cid = cnt_ids.at( 1 );    
    contact = m_manager->contact( cid  );
    contactName = contact.detail( QContactName::DefinitionName );
    QCOMPARE( contactName.value( QContactName::FieldFirstName ), QString("Stefann" ) );
    QCOMPARE( contact.details( QContactEmailAddress::DefinitionName ).count(), 2 );
    QCOMPARE( contact.details( QContactEmailAddress::DefinitionName )[0].value( QContactEmailAddress::FieldEmailAddress ), QString( "syadira@gmail.com" ) );
    QCOMPARE( contact.details( QContactEmailAddress::DefinitionName )[1].value( QContactEmailAddress::FieldEmailAddress ), QString( "stefann.yadira@nokia.com" ) );
}

//search "s ya", contact (1) match
void ST_LogsCntFinderQwerty::testPredictiveEmailSearchWithSpace()
{

    createContacts();
    QContactDetailFilter df;
    
    df.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress );
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    QContactLocalId cid;
    QContact contact;
    QContactName contactName;

    QString pattern = QString("s ya") + QChar(30) + QString("vqwerty");
    df.setValue( pattern );
    QList<QContactLocalId> cnt_ids = m_manager->contactIds( df );
    QCOMPARE( cnt_ids.count(), 1 );
    cid = cnt_ids.at( 0 );    
    contact = m_manager->contact( cid  );
    contactName = contact.detail( QContactName::DefinitionName );
    QCOMPARE( contactName.value( QContactName::FieldFirstName ), QString("Stefann" ) );
    
}

//search "syadira", contact (1) match
void ST_LogsCntFinderQwerty::testPredictiveEmailSearchEmailMatch()
{

    createContacts();
    
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress );
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    QContactLocalId cid;
    QContact contact;
    QContactName contactName;

    QString pattern = QString("syadira") + QChar(30) + QString("vqwerty");
    df.setValue( pattern );
    QList<QContactLocalId> cnt_ids = m_manager->contactIds( df );
    QCOMPARE( cnt_ids.count(), 1 );
    cid = cnt_ids.at( 0 );    
    contact = m_manager->contact( cid  );
    contactName = contact.detail( QContactName::DefinitionName );
    QCOMPARE( contactName.value( QContactName::FieldFirstName ), QString("Stefann" ) );
}

//search "pett", no match
void ST_LogsCntFinderQwerty::testPredictiveEmailSearchNoEmails()
{

    createContacts();
    
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress );
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    QContact contact;
    QContactName contactName;

    QString pattern = QString("pett") + QChar(30) + QString("vqwerty");
    df.setValue( pattern );
    QList<QContactLocalId> cnt_ids = m_manager->contactIds( df );
    QCOMPARE( cnt_ids.count(), 0 );
    
}

//search "levis.augu_whatever", contact (6) match
void ST_LogsCntFinderQwerty::testPredictiveEmailSearchOverLongPattern()
{

    createContacts();
    
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress );
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    QContactLocalId cid;
    QContact contact;
    QContactName contactName;

    QString pattern = QString("levis.augu_whatever") + QChar(30) + QString("vqwerty");
    df.setValue( pattern );
    QList<QContactLocalId> cnt_ids = m_manager->contactIds( df );
    QCOMPARE( cnt_ids.count(), 1 );
    cid = cnt_ids.at( 0 );    
    contact = m_manager->contact( cid  );
    contactName = contact.detail( QContactName::DefinitionName );
    QCOMPARE( contactName.value( QContactName::FieldFirstName ), QString("Levis" ) );
}

//search "olga.b", contact (3) match
void ST_LogsCntFinderQwerty::testPredictiveEmailSearchSpecialCharsInPattern()
{

    createContacts();
    
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress );
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    QContactLocalId cid;
    QContact contact;
    QContactName contactName;

    //search "olga.b", contact (3) match
    QString pattern = QString("olga.b") + QChar(30) + QString("vqwerty");
    df.setValue( pattern );
    QList<QContactLocalId> cnt_ids = m_manager->contactIds( df );
    QCOMPARE( cnt_ids.count(), 1 );
    cid = cnt_ids.at( 0 );    
    contact = m_manager->contact( cid  );
    contactName = contact.detail( QContactName::DefinitionName );
    QCOMPARE( contactName.value( QContactName::FieldFirstName ), QString("Olga" ) );

}

//search "5sya' contact match
void ST_LogsCntFinderQwerty::testPredictiveEmailSearchNumberAsFirstInPattern()
{
    // Remove all contacts from the database
    QList<QContactLocalId> cnt_ids = m_manager->contactIds();
    m_manager->removeContacts(cnt_ids, 0 );
    QVERIFY(0 == cnt_ids.count());
    
    //         'id' first      last           phonenumber        email1                        email2                      email3  
    ADD_CONTACT( 1, "Stefann", "Yadira",      "0035893424558",   "5syadira@gmail.com",          "stefann.yadira@nokia.com", "" );
    
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress );
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    QContactLocalId cid;
    QContact contact;
    QContactName contactName;

    //search "5sya' contact match
    QString pattern = QString("5sya") + QChar(30) + QString("vqwerty");
    df.setValue( pattern );
    cnt_ids = m_manager->contactIds( df );
    QCOMPARE( cnt_ids.count(), 1 );
    cid = cnt_ids.at( 0 );    
    contact = m_manager->contact( cid  );
    contactName = contact.detail( QContactName::DefinitionName );
    QCOMPARE( contactName.value( QContactName::FieldFirstName ), QString("Stefann" ) );


}
