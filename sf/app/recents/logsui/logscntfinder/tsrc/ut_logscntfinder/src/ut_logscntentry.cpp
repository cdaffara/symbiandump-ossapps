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
#include <hbinputsettingproxy.h>

#include "ut_logscntentry.h"
#include "logscntentry.h"
#include "logspredictivetranslator.h"

#include <QtTest/QtTest>

#define PATTERN( pattern )\
    LogsPredictiveTranslator::instance()->translatePattern( QString( pattern ) )
    

void UT_LogsCntEntry::initTestCase()
{
    mOrigLang = HbInputSettingProxy::instance()->globalInputLanguage();
    
}

void UT_LogsCntEntry::cleanupTestCase()
{
    HbInputSettingProxy::instance()->setGlobalInputLanguage( mOrigLang );
    
}


void UT_LogsCntEntry::init()
{
    mEntry = new LogsCntEntry( 0 );
}

void UT_LogsCntEntry::cleanup()
{
    delete mEntry;
    mEntry = 0;
    LogsPredictiveTranslator::deleteInstance();
    HbInputSettingProxy::instance()->setGlobalInputLanguage( mOrigLang );
    
}

void UT_LogsCntEntry::testConstructor()
{
    QVERIFY( !mEntry->isCached() );
    QVERIFY( mEntry->contactId() == 0 );
    QVERIFY( mEntry->type() == LogsCntEntry::EntryTypeContact );
    QVERIFY( mEntry->firstName()[0].text() == QString("") );
    QVERIFY( mEntry->lastName()[0].text() == QString("") );
    QVERIFY( mEntry->phoneNumber().text() == QString("") );
    QVERIFY( mEntry->avatarPath() == QString("") );
    QVERIFY( mEntry->handle() == 0 );
    QVERIFY( mEntry->speedDial() == QString("") );
        
    
    mEntry->setFirstName( QString( "foo" ));
    mEntry->setLastName( QString( "bar" ));
    
    LogsCntEntry* e = new LogsCntEntry( *mEntry );
    QVERIFY( e->isCached() );
    QVERIFY( e->contactId() == mEntry->contactId() );
    QVERIFY( e->type() == mEntry->type() );
    QVERIFY( e->firstName()[0].text() == mEntry->firstName()[0].text() );
    QVERIFY( e->lastName()[0].text() == mEntry->lastName()[0].text() );
    QVERIFY( e->phoneNumber().text() == mEntry->phoneNumber().text() );
    QVERIFY( e->phoneNumber().mTranslatedText == mEntry->phoneNumber().mTranslatedText );
    QVERIFY( e->mHandle == mEntry->mHandle );
    QVERIFY( e->speedDial() == mEntry->speedDial() );
    
    mEntry->setFirstName( QString( "Nokia" ));
    QVERIFY( e->firstName()[0].text() != mEntry->firstName()[0].text() );
    
    delete e;
    e = 0;
    
    LogsCntEntryHandle* foo = (LogsCntEntryHandle*)1;
    e = new LogsCntEntry( *foo, 0 );
    QVERIFY( e->isCached() );
    QVERIFY( e->contactId() == 0 );
    QVERIFY( e->type() == LogsCntEntry::EntryTypeHistory );
    QVERIFY( e->mHandle == foo );
    
    delete e;
    e = 0;
    
}


void UT_LogsCntEntry::testSetName()
{
    QString empty("");
    
    QVERIFY( !mEntry->isCached() );
    QVERIFY( mEntry->type() == LogsCntEntry::EntryTypeContact );
    
    mEntry->setFirstName( QString( "first" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString( "first" ) );
    QVERIFY( !mEntry->firstName()[0].highlights());
    QVERIFY( mEntry->firstName()[0].mTranslatedText != empty );
    
    mEntry->setLastName( QString( "last" ) );
    QVERIFY( mEntry->lastName()[0].text() == QString( "last" ) );
    QVERIFY( !mEntry->lastName()[0].highlights());
    QVERIFY( mEntry->lastName()[0].mTranslatedText != empty );
    
    mEntry->mType = LogsCntEntry::EntryTypeHistory;
    mEntry->setFirstName( QString( "first1" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString( "first1" ) );
    QVERIFY( !mEntry->firstName()[0].highlights());
    QVERIFY( mEntry->firstName()[0].mTranslatedText != empty );
    
    mEntry->setLastName( QString( "last1" ) );
    QVERIFY( mEntry->lastName()[0].text() == QString( "last1" ) );
    QVERIFY( !mEntry->lastName()[0].highlights());
    QVERIFY( mEntry->lastName()[0].mTranslatedText != empty );
    
    mEntry->setFirstName( QString( "first last" ) );
    QVERIFY( mEntry->firstName().count() == 2 );
    QVERIFY( mEntry->firstName()[0].text() == QString( "first" ) );
    QVERIFY( mEntry->firstName()[1].text() == QString( "last" ) );
    QVERIFY( !mEntry->firstName()[0].highlights() );
    QVERIFY( !mEntry->firstName()[1].highlights() );
    QVERIFY( mEntry->firstName()[0].mTranslatedText != empty );
    QVERIFY( mEntry->firstName()[1].mTranslatedText != empty );

    mEntry->setFirstName( QString( "" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString("") );

}

void UT_LogsCntEntry::testSetHighlights_latin12k()
{
    
    mEntry->setHighlights( PATTERN("665") );
    QVERIFY( mEntry->firstName()[0].text() == QString("") );
    QVERIFY( mEntry->lastName()[0].text() == QString("") );
    
    mEntry->setFirstName( QString( "Nokia" ) );
    mEntry->setLastName( QString( "Test" ) );
    
    mEntry->setHighlights( PATTERN( "665" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString( "Nokia" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 3 );
    QVERIFY( mEntry->lastName()[0].text() == QString( "Test" ) );
    QVERIFY( !mEntry->lastName()[0].highlights());

    mEntry->mType = LogsCntEntry::EntryTypeContact;
    mEntry->setFirstName( QString( "Jim" ) );
    mEntry->setLastName( QString( "Johnson" ) );
    
    mEntry->setHighlights( PATTERN( "5" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString( "Jim" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1 );
    QVERIFY( mEntry->lastName()[0].text() == QString( "Johnson" ) );
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setFirstName( QString( "Big Jim" ) );
    mEntry->setLastName( QString( "Johnson" ) );
    
    mEntry->setHighlights( PATTERN( "5" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString( "Big" ) );
    QVERIFY( mEntry->firstName()[1].text() == QString( "Jim" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0 );
    QVERIFY( mEntry->firstName()[1].highlights() == 1 );
    QVERIFY( mEntry->lastName()[0].text() == QString( "Johnson" ) );
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setFirstName( QString( "John" ) );
    mEntry->setLastName( QString( "Johnson" ) );
    mEntry->setHighlights( PATTERN( "5646" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 4 );
    QVERIFY( mEntry->lastName()[0].highlights() == 4);
    
    mEntry->setHighlights( PATTERN( "56467" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0 );
    QVERIFY( mEntry->lastName()[0].highlights() == 5);
    
    mEntry->setHighlights( PATTERN( "505" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setHighlights( PATTERN( "506" ) );//entry is not a match
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);//506 means 5 or 6 in higlighting
    
    mEntry->setFirstName( QString( "Big Jim Johnson" ) );
    mEntry->setLastName( QString( "" ) );
    
    mEntry->setHighlights( PATTERN( "205" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 1);
    QVERIFY( mEntry->firstName()[2].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "20505" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->firstName()[2].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "50205" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 1);
    QVERIFY( mEntry->firstName()[2].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "50502" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 1);
    QVERIFY( mEntry->firstName()[2].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "00200" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->firstName()[2].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "002400" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 2);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->firstName()[2].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "00240054" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 2);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->firstName()[2].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "240054" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 2);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->firstName()[2].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setFirstName( QString( "Big John" ) );
    mEntry->setLastName( QString( "Johnson" ) );
    
    mEntry->setHighlights( PATTERN( "2056" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 2);
    
    mEntry->setHighlights( PATTERN( "5602" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 2);
    
    mEntry->setHighlights( PATTERN( "564602" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 4);
    QVERIFY( mEntry->lastName()[0].highlights() == 4);
    
    mEntry->setHighlights( PATTERN( "5646702" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 5);
    
    mEntry->setHighlights( PATTERN( "5646" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 4);
    QVERIFY( mEntry->lastName()[0].highlights() == 4);
    
    mEntry->setHighlights( PATTERN( "50" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setHighlights( PATTERN( "05" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setFirstName( QString( "John 0John" ) );
    mEntry->setLastName( QString( "Malkovich" ) );
    mEntry->setHighlights( PATTERN( "05" ) );
    
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);

    mEntry->setHighlights( PATTERN( "0506" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);

    mEntry->setHighlights( PATTERN( "506" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setHighlights( PATTERN( "5060" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setHighlights( PATTERN( "05060" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setHighlights( PATTERN( "050506" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setPhoneNumber( QString( "+1234567" ) );
    QVERIFY( mEntry->phoneNumber().text() == QString( "+1234567" ) );
    mEntry->setHighlights( PATTERN( "+12" ) );
    QVERIFY( mEntry->phoneNumber().mHighlights == 3 );
    
    mEntry->setPhoneNumber( QString( "1234567" ) );
    QVERIFY( mEntry->phoneNumber().text() == QString( "1234567" ) );
    mEntry->setHighlights( PATTERN( "+12" ) );
    QVERIFY( mEntry->phoneNumber().mHighlights == 0 );
    
    mEntry->setHighlights( PATTERN( "12" ) );
    QVERIFY( mEntry->phoneNumber().mHighlights == 2 );

    mEntry->setFirstName( QString( "Alice 028 Ming" ) );
    mEntry->setHighlights( PATTERN( "028" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 3);
    QVERIFY( mEntry->firstName()[2].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);

    mEntry->setFirstName( QString( "Alice +Ming" ) );
    
    mEntry->setHighlights( PATTERN( "+6" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "#6" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);

    mEntry->setHighlights( PATTERN( "*6" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "16" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setFirstName( QString( "Alice M*ing" ) );
    mEntry->setHighlights( PATTERN( "6+" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "6*" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "6#" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);

    mEntry->setFirstName( QString( "Alice M-ing" ) );
    mEntry->setHighlights( PATTERN( "6+" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "6*" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "6#" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
        
    mEntry->setHighlights( PATTERN( "61" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setFirstName( QString( "Alice M#ing" ) );
    mEntry->setHighlights( PATTERN( "6+" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "6*" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "6#" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
        
    mEntry->setHighlights( PATTERN( "61" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
}

void UT_LogsCntEntry::testSetPhoneNumber()
{
    QString empty("");

    mEntry->setPhoneNumber( QString( "1234567" ) );
    QVERIFY( mEntry->phoneNumber().text() == QString( "1234567" ) );
    QVERIFY( mEntry->phoneNumber().mTranslatedText != empty );
    QVERIFY( mEntry->phoneNumber().mHighlights == 0 );

    mEntry->setPhoneNumber( QString( "+1234567" ) );
    QVERIFY( mEntry->phoneNumber().text() == QString( "+1234567" ) );
    QVERIFY( mEntry->phoneNumber().mTranslatedText != empty );
    
    mEntry->setPhoneNumber( QString( "#1234567" ) );
    QVERIFY( mEntry->phoneNumber().text() == QString( "#1234567" ) );
    QVERIFY( mEntry->phoneNumber().mTranslatedText != empty );

    mEntry->setPhoneNumber( QString( "*1234567" ) );
    QVERIFY( mEntry->phoneNumber().text() == QString( "*1234567" ) );
    QVERIFY( mEntry->phoneNumber().mTranslatedText != empty );

    mEntry->setPhoneNumber( QString( "*12+345#67" ) );
    QVERIFY( mEntry->phoneNumber().text() == QString( "*12+345#67" ) );
    QVERIFY( mEntry->phoneNumber().mTranslatedText != empty );
    
}

void UT_LogsCntEntry::testSetSpeedDial()
{
    mEntry->setSpeedDial( QString( "1" ) );
    QVERIFY( mEntry->speedDial() == QString( "1" ) );
    
}

void UT_LogsCntEntry::testSetAvatarPath()
{
    mEntry->setAvatarPath( QString( "c:\\data\\images\\logstest1.jpg" ) );
    QVERIFY( mEntry->avatarPath() == QString( "c:\\data\\images\\logstest1.jpg" ) );
    
}

void UT_LogsCntEntry::testRichText_latin12k()
{
    
    mEntry->setFirstName( QString( "John" ) );
    mEntry->setHighlights( PATTERN( "5" ) );
    QVERIFY( mEntry->firstName()[0].richText() == QString( "<b><u>J</u></b>ohn" ) );
    QVERIFY( mEntry->firstName()[0].richText( QString("<u>"),QString("</u>")  ) 
                                               == QString( "<u>J</u>ohn" ) );
    
    mEntry->setFirstName( QString( "aaa" ) );
    QVERIFY( mEntry->firstName()[0].richText() == QString( "aaa" ) );
    
    mEntry->setFirstName( QString( "" ) );
    QVERIFY( mEntry->firstName()[0].richText() == QString( "" ) );
    
    mEntry->setPhoneNumber( QString( "1234567" ) );
    mEntry->setHighlights( PATTERN( "5" ) );
    
    QVERIFY( mEntry->phoneNumber().mHighlights == 0 );
    QVERIFY( mEntry->phoneNumber().text() == QString( "1234567" ) );
    QVERIFY( mEntry->phoneNumber().richText() == QString( "1234567" ) );
    
    mEntry->setHighlights( PATTERN( "1" ) );
    
    QVERIFY( mEntry->phoneNumber().mHighlights == 1 );
    QVERIFY( mEntry->phoneNumber().text() == QString( "1234567" ) );
    QVERIFY( mEntry->phoneNumber().richText() == QString( "<b><u>1</u></b>234567" ) );
    
}

void UT_LogsCntEntry::testMatch_latin12k()
{
    
    mEntry->mType = LogsCntEntry::EntryTypeHistory;
    
    mEntry->setFirstName( QString( "John" ) );
    QVERIFY( !mEntry->match( PATTERN( "" ) ) );
    QVERIFY( mEntry->match( PATTERN( "5" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "6" ) ) );
    
    mEntry->setPhoneNumber( QString( "11111111111" ) );
    mEntry->setFirstName( QString( "John" ) );
    mEntry->setLastName( QString( "Malkovich" ) );
    QVERIFY( mEntry->match( PATTERN( "6" ) ) );
    QVERIFY( mEntry->match( PATTERN( "5" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "2" ) ) );
    QVERIFY( mEntry->match( PATTERN( "56" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "566" ) ) );

    mEntry->setPhoneNumber( QString( "5669876566" ) );
    QVERIFY( mEntry->match( PATTERN( "566" ) ) );

// - zero cases -
    
    mEntry->setPhoneNumber( QString( "20298457698576" ) );
    mEntry->setFirstName( QString( "John" ) );
    mEntry->setLastName( QString( "Malkovich" ) );
    
    QVERIFY( !mEntry->match( PATTERN( "00" ) ) );
    QVERIFY( mEntry->match( PATTERN( "202" ) ) );//match not John and match not Malkovich match phone
    QVERIFY( !mEntry->match( PATTERN( "507" ) ) );//match John and match not Malkovich
    QVERIFY( !mEntry->match( PATTERN( "206" ) ) );//match not John and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "506" ) ) );//match John and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "605" ) ) );//match John and macth Malkovich
    QVERIFY( !mEntry->match( PATTERN( "505" ) ) );//match John "two times"
    QVERIFY( mEntry->match( PATTERN( "60005" ) ) );//match John and match Malkovich
    QVERIFY( mEntry->match( PATTERN( "6000500" ) ) );//match John and match Malkovich
    QVERIFY( !mEntry->match( PATTERN( "6000501" ) ) );//match John and match Malkovich
    
    mEntry->setFirstName( QString( "John Malkovich" ) );
    mEntry->setLastName( QString( "" ) );
    
    QVERIFY( mEntry->match( PATTERN( "202" ) ) );//match not John and match not Malkovich match phone
    QVERIFY( !mEntry->match( PATTERN( "507" ) ) );//match John and match not Malkovich
    QVERIFY( !mEntry->match( PATTERN( "206" ) ) );//match not John and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "506" ) ) );//match John and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "605" ) ) );//match John and macth Malkovich
    QVERIFY( !mEntry->match( PATTERN( "505" ) ) );//match John "two times"
    QVERIFY( mEntry->match( PATTERN( "60005" ) ) );//match John and match Malkovich
    QVERIFY( mEntry->match( PATTERN( "6000500" ) ) );//match John and match Malkovich

    mEntry->setFirstName( QString( "" ) );
    mEntry->setLastName( QString( "John Malkovich" ) );
    
    QVERIFY( mEntry->match( PATTERN( "202" ) ) );//match not John and match not Malkovich match phone
    QVERIFY( !mEntry->match( PATTERN( "507" ) ) );//match John and match not Malkovich
    QVERIFY( !mEntry->match( PATTERN( "206" ) ) );//match not John and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "506" ) ) );//match John and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "605" ) ) );//match John and macth Malkovich
    QVERIFY( !mEntry->match( PATTERN( "505" ) ) );//match John "two times"
    QVERIFY( mEntry->match( PATTERN( "60005" ) ) );//match John and match Malkovich
    QVERIFY( mEntry->match( PATTERN( "6000500" ) ) );//match John and match Malkovich

    mEntry->setFirstName( QString( "John Peter" ) );
    mEntry->setLastName( QString( "Malkovich" ) );
    
    QVERIFY( !mEntry->match( PATTERN( "50607" ) ) );//match John and match Malkovich match Peter
    QVERIFY( mEntry->match( PATTERN( "506" ) ) );//match John and macth Malkovich match ignore Peter
    QVERIFY( !mEntry->match( PATTERN( "50608" ) ) );//match John and match Malkovich match not Peter
    QVERIFY( !mEntry->match( PATTERN( "5060702" ) ) );//match John and match Malkovich match Peter + no match extra
    QVERIFY( !mEntry->match( PATTERN( "5060706" ) ) );//match John and match Malkovich match Peter + macth extra 
    QVERIFY( mEntry->match( PATTERN( "5060" ) ) );//match John and macth Malkovich match ignore Peter
    
    mEntry->setFirstName( QString( "John Peter" ) );
    mEntry->setLastName( QString( "M0rkovich" ) );
    QVERIFY( mEntry->match( PATTERN( "50607" ) ) );//match John and match M0rkovich 
    QVERIFY( !mEntry->match( PATTERN( "60705" ) ) ); 
    QVERIFY( mEntry->match( PATTERN( "50600" ) ) );//match John and match M0rkovich 
    QVERIFY( mEntry->match( PATTERN( "5060" ) ) );//match John and match M0rkovich 
    QVERIFY( mEntry->match( PATTERN( "506" ) ) );//match John and match M0rkovich 
    QVERIFY( mEntry->match( PATTERN( "605" ) ) );//match John and match M0rkovich 
    
    
    mEntry->setFirstName( QString( "John Kalkovich" ) );
    mEntry->setLastName( QString( "" ) );
    
    QVERIFY( mEntry->match( PATTERN( "505" ) ) );//match John and match Kalkovich
    
    mEntry->setFirstName( QString( "John John Malkovich" ) );
    mEntry->setLastName( QString( "" ) );
    
    QVERIFY( !mEntry->match( PATTERN( "50605" ) ) );//match John and match Malkovich and match John
    QVERIFY( mEntry->match( PATTERN( "506" ) ) );//match Johns and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "505" ) ) );//match Johns and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "005050" ) ) );//match Johns and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "05050" ) ) );//match Johns and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "5050" ) ) );//match Johns and macth Malkovich
    QVERIFY( mEntry->match( PATTERN( "0505" ) ) );//match Johns and macth Malkovich
    QVERIFY( !mEntry->match( PATTERN( "50608" ) ) );//match John and match Malkovich match not Peter
    QVERIFY( mEntry->match( PATTERN( "5060" ) ) );//match John and match Malkovich match not Peter
    
    mEntry->setFirstName( QString( "John 0John Malkovich" ) );
    mEntry->setLastName( QString( "" ) );
    
    QVERIFY( !mEntry->match( PATTERN( "50605" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "505" ) ) );
    QVERIFY( mEntry->match( PATTERN( "5" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0" ) ) );
    QVERIFY( mEntry->match( PATTERN( "05" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0505" ) ) );
    QVERIFY( mEntry->match( PATTERN( "05005" ) ) );
    QVERIFY( mEntry->match( PATTERN( "6005" ) ) );
    QVERIFY( mEntry->match( PATTERN( "05050" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "00505" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "005050" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0506" ) ) );
    QVERIFY( mEntry->match( PATTERN( "05060" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "050506" ) ) );
    
    mEntry->setPhoneNumber( QString( "+20298457698576" ) );
    mEntry->setFirstName( QString( "John" ) );
    mEntry->setLastName( QString( "Malkovich" ) );
    
    QVERIFY( mEntry->match( PATTERN( "+202" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "#202" ) ) );
    QVERIFY( mEntry->match( PATTERN( "*202" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "202" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "+202#98" ) ) );
    
    QVERIFY( mEntry->match( PATTERN( "50" ) ) );
    QVERIFY( mEntry->match( PATTERN( "05" ) ) );
    QVERIFY( mEntry->match( PATTERN( "506" ) ) );
    
    mEntry->setFirstName( QString( "John" ) );
    mEntry->setLastName( QString( "#Malkovich" ) );
    QVERIFY( mEntry->match( PATTERN( "5" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "6" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "506" ) ) );
    QVERIFY( mEntry->match( PATTERN( "#6" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "+6" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "16" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "*6" ) ) );
    QVERIFY( mEntry->match( PATTERN( "50#6" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "5016" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "50*6" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "50+6" ) ) );
    QVERIFY( mEntry->match( PATTERN( "+" ) ) );//phone
    QVERIFY( mEntry->match( PATTERN( "*" ) ) );
    QVERIFY( mEntry->match( PATTERN( "#" ) ) );

    mEntry->setFirstName( QString( "J*hn" ) );
    mEntry->setLastName( QString( "#Malkovich" ) );
    QVERIFY( mEntry->match( PATTERN( "5" ) ) );
    QVERIFY( mEntry->match( PATTERN( "5*" ) ) );
    QVERIFY( mEntry->match( PATTERN( "5+" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "51" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "5#" ) ) );
    QVERIFY( mEntry->match( PATTERN( "50#" ) ) );
    QVERIFY( mEntry->match( PATTERN( "5+0#6" ) ) );
    QVERIFY( mEntry->match( PATTERN( "5*0#6" ) ) );
    QVERIFY( mEntry->match( PATTERN( "5*0#" ) ) );
    
    mEntry->setFirstName( QString( "J-hn" ) );
    mEntry->setLastName( QString( "#Malkovich" ) );
    QVERIFY( mEntry->match( PATTERN( "5" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "5*" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "5+" ) ) );
    QVERIFY( mEntry->match( PATTERN( "51" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "5#" ) ) );
    QVERIFY( mEntry->match( PATTERN( "50#" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "5+0#6" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "5*0#6" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "5*0#" ) ) );
    QVERIFY( mEntry->match( PATTERN( "510#" ) ) );

    mEntry->setFirstName( QString("Alice 028") );
    mEntry->setLastName( QString( "Ming" ) );
    QVERIFY( mEntry->match( PATTERN( "02806" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0" ) ) );
    QVERIFY( mEntry->match( PATTERN( "02" ) ) );
    QVERIFY( mEntry->match( PATTERN( "028" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0280" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0250" ) ) );
    QVERIFY( mEntry->match( PATTERN( "64" ) ) );
    QVERIFY( mEntry->match( PATTERN( "640" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0640" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "202" ) ) );
    QVERIFY( mEntry->match( PATTERN( "200" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "20028" ) ) );
    QVERIFY( mEntry->match( PATTERN( "02802" ) ) );
    QVERIFY( mEntry->match( PATTERN( "028002" ) ) );
    

    mEntry->setFirstName( QString("Ali") + QString( QChar(3) ) + QString("ce") );
    QVERIFY( mEntry->match( PATTERN( "2" ) ) );//A
    QVERIFY( mEntry->match( PATTERN( "254" ) ) );//Ali
    QVERIFY( !mEntry->match( PATTERN( "25423" ) ) );//Alice
    
    mEntry->mType = LogsCntEntry::EntryTypeContact;
    QVERIFY( !mEntry->match( PATTERN( "+202" ) ) );
    
    mEntry->setFirstName( QString( "0J0hn" ) );
    mEntry->setLastName( QString( "Malkovich" ) );
    QVERIFY( mEntry->match( PATTERN( "000" ) ) );
    
    mEntry->setFirstName( QString("Zlice 00234") );
    mEntry->setLastName( QString( "Ming" ) );
    QVERIFY( mEntry->match( PATTERN( "002" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0020" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0023" ) ) );
    QVERIFY( mEntry->match( PATTERN( "00209" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0000950" ) ) );
    
    mEntry->setFirstName( QString("Alice") );
    mEntry->setLastName( QString( "Ming" ) );
    QVERIFY( mEntry->match( PATTERN( "002" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0020" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "00202" ) ) );
    QVERIFY( mEntry->match( PATTERN( "0025000000000" ) ) );
    
}

void UT_LogsCntEntry::testStartsWith_latin12k()
{
    //just using numbers as name
    mEntry->setFirstName( QString( "" ) );
    QCOMPARE( mEntry->startsWith( mEntry->firstName()[0], QString("") ), 0 );
    QCOMPARE( mEntry->startsWith( mEntry->firstName()[0], QString("123") ), 0 );
    
    mEntry->setFirstName( QString( "123" ) );
    QCOMPARE( mEntry->startsWith( mEntry->firstName()[0], QString("") ), 0 );
    QCOMPARE( mEntry->startsWith( mEntry->firstName()[0], QString("1234") ), 0 );
    QCOMPARE( mEntry->startsWith( mEntry->firstName()[0], QString("1") ), 1 );
    QCOMPARE( mEntry->startsWith( mEntry->firstName()[0], QString("123") ), 3 );
    QCOMPARE( mEntry->startsWith( mEntry->firstName()[0], QString("12")), 2 );
    QCOMPARE( mEntry->startsWith( mEntry->firstName()[0], QString("12")), 2 );
    
}


void UT_LogsCntEntry::testMatch_thai12k()
{
    
    LogsPredictiveTranslator::deleteInstance();
    HbInputLanguage thai( QLocale::Thai );
    HbInputSettingProxy::instance()->setGlobalInputLanguage( thai );
    
    mEntry->mType = LogsCntEntry::EntryTypeHistory;
    
    mEntry->setFirstName( QString( "5643" ) );
    QVERIFY( !mEntry->match( PATTERN( "" ) ) );
    QVERIFY( mEntry->match( PATTERN( "5" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "6" ) ) );
    
    mEntry->setLastName( QString( "6787" ) );
    QVERIFY( mEntry->match( PATTERN( "5" ) ) );
    QVERIFY( mEntry->match( PATTERN( "05" ) ) );
    QVERIFY( mEntry->match( PATTERN( "6" ) ) );
    
    QEXPECT_FAIL("", "No proper Thai keymap yet", Abort );
    
    QVERIFY( mEntry->match( PATTERN( "*#*#*#*5*#*#*#**#*#" ) ) );
    QVERIFY( mEntry->match( PATTERN( "*#*#*****#6" ) ) );
    QVERIFY( mEntry->match( PATTERN( "06" ) ) );
    
    QVERIFY( mEntry->match( PATTERN( "605" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "606" ) ) );
    
    QVERIFY( mEntry->match( PATTERN( "#*#*#6*#*#**0*#*#**5*****#" ) ) );
    QVERIFY( !mEntry->match( PATTERN( "**#*#**#*#6*#*0*#*#*6*#**#****#" ) ) );
    
}


void UT_LogsCntEntry::testSetHighlights_thai12k()
{
    QEXPECT_FAIL("", "No proper Thai keymap yet", Abort );
    
    LogsPredictiveTranslator::deleteInstance();
    HbInputLanguage thai( QLocale::Thai );
    HbInputSettingProxy::instance()->setGlobalInputLanguage( thai );
    
    mEntry->mType = LogsCntEntry::EntryTypeHistory;
    mEntry->setFirstName( QString( "5643 456456" ) );
    mEntry->setLastName( QString( "6787" ) );
    
    mEntry->setHighlights( PATTERN( "****56#*#*#" ) );
    QVERIFY( mEntry->firstName().length() == 1 );
    QVERIFY( mEntry->lastName().length() == 1 );
    QVERIFY( mEntry->firstName()[0].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( PATTERN( "****56#*#*#06" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    
}


