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
#include "ut_logscntfinder.h"
#include "logscntfinder.h"
#include "qtcontacts_stubs.h"
#include <qcontact.h>
#include <QtTest/QtTest>
#include <QSignalSpy>

#define CACHE_ALL_RESULTS()\
        for( int i=0;i<mFinder->resultsCount();i++) {\
            mFinder->resultAt( i );\
        }


void UT_LogsCntFinder::initTestCase()
{
}

void UT_LogsCntFinder::cleanupTestCase()
{
}


void UT_LogsCntFinder::init()
{
    mCntResults = ContactQueryResults::instance();
    mFinder = new LogsCntFinder();
}

void UT_LogsCntFinder::cleanup()
{
    delete mFinder;
    mFinder = 0;
    ContactQueryResults::deleteInstance();
    mCntResults = 0;
}

void UT_LogsCntFinder::testConstructor()
{
    QVERIFY( mFinder );
    QVERIFY( mFinder->mCurrentPredictivePattern == "" );
    QVERIFY( mFinder->mResults.count() == 0 );
    QVERIFY( mFinder->mHistoryEvents.count() == 0 );
    QVERIFY( mFinder->mContactManager );
    
    LogsCntFinder finder( *mFinder->mContactManager ); 
    QVERIFY( finder.mCurrentPredictivePattern == "" );
    QVERIFY( finder.mCurrentInputPattern == "" );
    QVERIFY( finder.mResults.count() == 0 );
    QVERIFY( finder.mHistoryEvents.count() == 0 );
    QVERIFY( finder.mContactManager );
    QVERIFY( finder.mCachedCounter == 0 );
    
}

void UT_LogsCntFinder::testPredictiveSearchQuery_latin12k()
{
    QSignalSpy queryReadySpy(mFinder, SIGNAL(queryReady() ) );
    QSignalSpy resultsAddedSpy(mCntResults, SIGNAL(resultsAdded() ) );
    
    LogsCntEntryHandle* handle1 = (LogsCntEntryHandle*)1;
    LogsCntEntryHandle* handle2 = (LogsCntEntryHandle*)2;
    LogsCntEntryHandle* handle3 = (LogsCntEntryHandle*)3;
    
    LogsCntEntry* entry1 = new LogsCntEntry( *handle1,0 );
    entry1->setFirstName( QString("Tim Roth") );
    entry1->setAvatarPath( QString("c:\\data\\images\\logstest1.jpg") );
    mFinder->insertEntry( 0, entry1 );
    QCOMPARE( mFinder->mHistoryEvents.count(), 1 );
    
    LogsCntEntry* entry2 = new LogsCntEntry( *handle2,0 );
    entry2->setFirstName( QString("Kai") );
    entry2->setLastName( QString("Öistämö") );
    entry2->setAvatarPath( QString("c:\\data\\images\\logstest2.jpg") );
    mFinder->insertEntry( 0, entry2 );
    QVERIFY( mFinder->mHistoryEvents.count() == 2 );    
    QCOMPARE( mFinder->mCachedCounter, 0 );

    LogsCntEntry* entry3 = new LogsCntEntry( *handle3,0 );
    entry3->setFirstName( QString("Kaizu") );
    entry3->setLastName( QString("Oistamo") );
    mFinder->insertEntry( 1, entry3 );
    QVERIFY( mFinder->mHistoryEvents.count() == 3 );    
    QCOMPARE( mFinder->mCachedCounter, 0 );
    
    mCntResults->set( 9 );//John1 Malkovich1 ...
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("5") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QVERIFY( mFinder->mCurrentPredictivePattern == QString("5") );
    QVERIFY( mFinder->mCurrentInputPattern == QString("5") );
    QCOMPARE( mFinder->resultsCount(), 2 + 9 );
    QVERIFY( mFinder->mResults[1]->isCached() );
    QVERIFY( mFinder->mResults[0]->isCached() );
    QVERIFY( mFinder->mResults[0]->handle() == handle2 );
    QVERIFY( mFinder->mResults[0]->firstName()[0].text() == QString("Kai") );
    QVERIFY( mFinder->mResults[0]->firstName()[0].highlights() == 1 );
    QVERIFY( mFinder->mResults[0]->avatarPath() == QString("c:\\data\\images\\logstest2.jpg") );
    QVERIFY( !mFinder->mResults[2]->isCached() );
    QVERIFY( mFinder->mResults[2]->handle() == 0 );
    QVERIFY( mFinder->mResults[2]->contactId() == 1 );
    QCOMPARE( mFinder->mCachedCounter, 2 );
    
    const LogsCntEntry* firstE = &mFinder->resultAt( 2 );
    QCOMPARE( mFinder->mCachedCounter, 3 );
    QVERIFY( mFinder->mResults[2]->firstName()[0].text() == QString("John1") );
    QVERIFY( mFinder->mResults[2]->firstName()[0].highlights() == 1 );

    
    mCntResults->set( 9 );//John1 Malkovich1 ...
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("56") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 0 + 9 );
    QVERIFY( mFinder->mResults[0]->isCached() );
    QVERIFY( mFinder->mResults[0]->handle() == 0 );
    QVERIFY( mFinder->mResults[0]->firstName()[0].text() == QString("John1") );
    QVERIFY( mFinder->mResults[0]->firstName()[0].highlights() == 2 );
    QVERIFY( &mFinder->resultAt(0) == firstE );
    QCOMPARE( mFinder->mCachedCounter, 1 );
    

    mCntResults->set( 0 );//no match
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("562") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 0 );
    QCOMPARE( mFinder->mCachedCounter, 0 );

    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("5627") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 0 );
    QCOMPARE( mFinder->resultsCount(), 0 );
    QCOMPARE( mFinder->mCachedCounter, 0 );
    
    mCntResults->set( 9 );//John1 Malkovich1 ...
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("56") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 0 + 9 );
    QVERIFY( !mFinder->mResults[0]->isCached() );
    QVERIFY( mFinder->mResults[0]->handle() == 0 );
    QVERIFY( mFinder->mResults[0]->contactId() == 1 );
    //QVERIFY( mFinder->mResults[0] != firstE ); might be the same
    QCOMPARE( mFinder->mCachedCounter, 0 );
    
    mCntResults->set( 9 );//John1 Malkovich1 ...
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("5") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 2 + 9 );
    QVERIFY( mFinder->mResults[0]->isCached() );
    QVERIFY( mFinder->mResults[0]->handle() == handle2 );
    QVERIFY( mFinder->mResults[0]->firstName()[0].text() == QString("Kai") );
    QVERIFY( mFinder->mResults[0]->firstName()[0].highlights() == 1 );
    QVERIFY( !mFinder->mResults[2]->isCached() );
    QVERIFY( mFinder->mResults[2]->contactId() == 1 );
    QCOMPARE( mFinder->mCachedCounter, 2 );
    
    CACHE_ALL_RESULTS();
    QCOMPARE( mFinder->mCachedCounter, mFinder->resultsCount() );
    
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("56") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 0 );
    QCOMPARE( mFinder->resultsCount(), 0 + 9 );
    QVERIFY( mFinder->mResults[0]->isCached() );
    QVERIFY( mFinder->mResults[0]->handle() == 0 );
    QVERIFY( mFinder->mResults[0]->firstName()[0].text() == QString("John1") );
    QVERIFY( mFinder->mResults[0]->firstName()[0].highlights() == 2 );
    QCOMPARE( mFinder->mCachedCounter, 9 );
    
    mCntResults->set( 9 );//same results ( contact ids )
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("5") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 2 + 9 );
    QVERIFY( mFinder->mResults[0]->isCached() );
    QVERIFY( mFinder->mResults[0]->handle() == handle2 );
    QVERIFY( mFinder->mResults[0]->firstName()[0].text() == QString("Kai") );
    QVERIFY( mFinder->mResults[0]->firstName()[0].highlights() == 1 );
    //althoug had to go to db, contact ids were reused from prev seacrh
    QVERIFY( mFinder->mResults[2]->isCached() );
    QVERIFY( mFinder->mResults[2]->handle() == 0 );
    QVERIFY( mFinder->mResults[2]->firstName()[0].text() == QString("John1") );
    QVERIFY( mFinder->mResults[2]->firstName()[0].highlights() == 1 );
    QCOMPARE( mFinder->mCachedCounter, 11 );
    
    //refresh
    mCntResults->set( 9 );
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( mFinder->mCurrentPredictivePattern );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 2 + 9 );

    //reset
    mCntResults->set( 9 );
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 0 );
    QCOMPARE( mFinder->resultsCount(),0 );
    QCOMPARE( mFinder->mCachedCounter, 0 );
    
    //15 digits query
    mCntResults->set( 9 );
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("123456789112345" ) );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->mCurrentPredictivePattern.length(), 15 );
    int resCount = mFinder->resultsCount();
    QCOMPARE( resCount, 9 );
    
    //15 digits query
    mCntResults->set( 9 );
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("222222222222222" ) );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->mCurrentPredictivePattern.length(), 15 );
    resCount = mFinder->resultsCount();
    QCOMPARE( resCount, 9 );
    
    //16 digits query
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("1234567891123456") );
    QCOMPARE( queryReadySpy.count(), 0 );
    QCOMPARE( resultsAddedSpy.count(), 0 );
    QCOMPARE( mFinder->mCurrentPredictivePattern.length(), 15 );
    QCOMPARE( mFinder->resultsCount(), resCount );
    
    //20 digits query
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("12345678911234567891") );
    QCOMPARE( queryReadySpy.count(), 0 );
    QCOMPARE( resultsAddedSpy.count(), 0 );
    QCOMPARE( mFinder->mCurrentPredictivePattern.length(), 15 );
    QCOMPARE( mFinder->resultsCount(), resCount );
    
    //step 1: if query is "20", this is matched (OK)
    //step 2: if query is "206", this is matched (OK)
    //Step 3: remove 6 in query and query "20", this is matched (OK)
    
    qDeleteAll( mFinder->mHistoryEvents );
    mFinder->mHistoryEvents.clear();
    mFinder->predictiveSearchQuery( QString("") );
    
    mCntResults->set( 1, QString("Alice 028"), QString("Ming") );
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("20") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 1 );

    CACHE_ALL_RESULTS();
    QCOMPARE( mFinder->mCachedCounter, mFinder->resultsCount() );
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("206") );
    QCOMPARE( queryReadySpy.count(), 1);
    QCOMPARE( resultsAddedSpy.count(), 0 );
    QCOMPARE( mFinder->resultsCount(), 1 );
    
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("20") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 1 );
    
// --
    
    qDeleteAll( mFinder->mHistoryEvents );
    mFinder->mHistoryEvents.clear();
    mFinder->predictiveSearchQuery( QString("") );
    
    entry1 = new LogsCntEntry( *handle1,0 );
    entry1->setFirstName( QString("+Tim #Roth") );
    mFinder->insertEntry( 0, entry1 );
    QCOMPARE( mFinder->mHistoryEvents.count(), 1 );
    
    mCntResults->reset();
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("+80#7") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 1 );
    QVERIFY( mFinder->mResults[0]->firstName()[0].text() == QString("+Tim") );
    QVERIFY( mFinder->mResults[0]->firstName()[0].highlights() == 2 );
    QVERIFY( mFinder->mResults[0]->firstName()[1].text() == QString("#Roth") );
    QVERIFY( mFinder->mResults[0]->firstName()[1].highlights() == 2 );
    QCOMPARE( mFinder->mCurrentInputPattern, QString("+80#7") );
    QCOMPARE( mFinder->mCurrentPredictivePattern, QString("*80#7") );
    QCOMPARE( mCntResults->query(), QString("+80#7") );

// --
    
    qDeleteAll( mFinder->mHistoryEvents );
    mFinder->mHistoryEvents.clear();
    mFinder->predictiveSearchQuery( QString("") );
    
    entry1 = new LogsCntEntry( *handle1,0 );
    entry1->setFirstName( QString("James Bond") );
    entry1->setPhoneNumber( QString("0401234566") );
    mFinder->insertEntry( 0, entry1 );
    QCOMPARE( mFinder->mHistoryEvents.count(), 1 );
    
    entry2 = new LogsCntEntry( *handle2,0 );
    entry2->setFirstName( QString("Mika Häkkinen") );
    entry2->setPhoneNumber( QString("0401234999") );
    mFinder->insertEntry( 0, entry2 );
    QCOMPARE( mFinder->mHistoryEvents.count(), 2 );
    
    mCntResults->reset();
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("0") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 2 );
    
    mCntResults->reset();
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("00") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 0 );
    
    mCntResults->reset();
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("000") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 0 );
    
// --
        
    qDeleteAll( mFinder->mHistoryEvents );
    mFinder->mHistoryEvents.clear();
    mFinder->predictiveSearchQuery( QString("") );
    
    mCntResults->set( 1, QString("Alice 028"), QString("Ming") );
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("0") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 1 );
    CACHE_ALL_RESULTS();
    
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("00") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 1 );
    CACHE_ALL_RESULTS();
    
    mCntResults->set( 1, QString("James"), QString("Bond") );
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("005") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 1 );
    CACHE_ALL_RESULTS();
    
    mCntResults->set( 1, QString("Alice 028"), QString("Ming") );
    queryReadySpy.clear();
    resultsAddedSpy.clear();
    
    mFinder->predictiveSearchQuery( QString("00") );
    QCOMPARE( queryReadySpy.count(), 1 );
    QCOMPARE( resultsAddedSpy.count(), 1 );
    QCOMPARE( mFinder->resultsCount(), 1 );
    CACHE_ALL_RESULTS();
    
    
    
}

void UT_LogsCntFinder::testResultAt_latin12k()
{
    mCntResults->set( 9 , QString( "first"), QString( "last") );
    
    QVERIFY( mFinder->resultsCount() == 0 );
    QString pattern( "5" );
    mFinder->predictiveSearchQuery( pattern );
    QVERIFY( mFinder->resultsCount() > 0 );
    
    const LogsCntEntry& e = mFinder->resultAt( 0 );
    QVERIFY( e.firstName()[0].text() == QString("first1") );
    QVERIFY( e.lastName()[0].text() == QString("last1") );
    QVERIFY( e.speedDial() == QString("") );
    QVERIFY( e.phoneNumber().text() == QString("555789987") );//stub
    QVERIFY( e.phoneNumber().highlights() == 1 );
    QVERIFY( e.avatarPath() != QString("") );
    QVERIFY( e.mType == LogsCntEntry::EntryTypeContact );
    QVERIFY( e.isCached() );
    
    const LogsCntEntry& f = mFinder->resultAt( 0 );
    QVERIFY( &e == &f );
    
    pattern = QString("2");
    mFinder->predictiveSearchQuery( pattern );
    QVERIFY( mFinder->resultsCount() > 0 );
    
}

void UT_LogsCntFinder::testInsertEntry()
{
    
    LogsCntEntryHandle* handle1 = (LogsCntEntryHandle*)1;
    LogsCntEntryHandle* handle2 = (LogsCntEntryHandle*)2;
    LogsCntEntryHandle* handle3 = (LogsCntEntryHandle*)3;
    
    LogsCntEntry* entry1 = new LogsCntEntry( *handle1,0 );
    mFinder->insertEntry( 0, entry1 );
    QVERIFY( mFinder->mHistoryEvents.count() == 1 );    
    LogsCntEntry* entry2 = new LogsCntEntry( *handle2,0 );
    mFinder->insertEntry( 0, entry2 );
    QVERIFY( mFinder->mHistoryEvents.count() == 2 );    
    LogsCntEntry* entry3 = new LogsCntEntry( *handle3,0 );
    mFinder->insertEntry( 1, entry3 );
    QVERIFY( mFinder->mHistoryEvents.count() == 3 );    
    
    QVERIFY( mFinder->getEntry( *handle1 ) == entry1 );
    QVERIFY( mFinder->getEntry( *handle2 ) == entry2 );
    QVERIFY( mFinder->getEntry( *handle3 ) == entry3 );
    
    
}

void UT_LogsCntFinder::testDeleteEntry()
{

    LogsCntEntryHandle* handle1 = (LogsCntEntryHandle*)1;
    LogsCntEntryHandle* handle2 = (LogsCntEntryHandle*)2;
    LogsCntEntryHandle* handle3 = (LogsCntEntryHandle*)3;
    
    LogsCntEntry* entry1 = new LogsCntEntry( *handle1,0 );
    mFinder->insertEntry( 0, entry1 );
    LogsCntEntry* entry2 = new LogsCntEntry( *handle2,0 );
    mFinder->insertEntry( 0, entry2 );
    LogsCntEntry* entry3 = new LogsCntEntry( *handle3,0 );
    mFinder->insertEntry( 1, entry3 );
    QVERIFY( mFinder->mHistoryEvents.count() == 3 );    
    
    mFinder->deleteEntry( *handle1 );
    QVERIFY( mFinder->mHistoryEvents.count() == 2 );
    mFinder->deleteEntry( *handle2 );
    QVERIFY( mFinder->mHistoryEvents.count() == 1 );
    mFinder->deleteEntry( *handle3 );
    QVERIFY( mFinder->mHistoryEvents.count() == 0 );
 
    entry1 = new LogsCntEntry( *handle1,0 );
    mFinder->insertEntry( 0, entry1 );
    QVERIFY( mFinder->mHistoryEvents.count() == 1 );
    entry2 = new LogsCntEntry( *entry1 );
    mFinder->mResults.append( entry2 );
    mFinder->deleteEntry( *handle1 );
    QVERIFY( mFinder->mHistoryEvents.count() == 0 );
    QVERIFY( mFinder->mResults.count() == 1 );
    
}

void UT_LogsCntFinder::testGetEntry()
{
    LogsCntEntryHandle* handle1 = (LogsCntEntryHandle*)1;
    
    LogsCntEntry* entry1 = new LogsCntEntry( *handle1,0 );
    QVERIFY( entry1->firstName()[0].text() == QString("") );
    QVERIFY( entry1->lastName()[0].text() == QString("") );
    entry1->setFirstName( QString("first") );
    entry1->setLastName( QString("last") );
    QVERIFY( entry1->firstName()[0].text() == QString("first") );
    QVERIFY( entry1->lastName()[0].text() == QString("last") );
    mFinder->insertEntry( 0, entry1 );

    LogsCntEntry* entry = mFinder->getEntry( *handle1 );
    QVERIFY( entry == entry1 );
    entry->setFirstName( QString("foo") );
    entry->setLastName( QString("bar") );
    
    LogsCntEntry* entry3 = mFinder->getEntry( *handle1 );
    QVERIFY( entry3 == entry1 );
    QVERIFY( entry3->firstName()[0].text() == QString("foo") );
    QVERIFY( entry3->lastName()[0].text() == QString("bar") );   
}

void UT_LogsCntFinder::testPhoneNumber()
{
    QContact contact;
    
    //Prefer default is not set, first number from the list returned
    QVERIFY( !mFinder->mPreferDefaultNumber );
    QCOMPARE( mFinder->phoneNumber(contact), cntDetailPhoneNumber );
    
    //Prefer default is set
    mFinder->mPreferDefaultNumber = true;
    QCOMPARE( mFinder->phoneNumber(contact), ContactStubHelper::preferredNumber() );
    
    //Prefer default is set, but no preferred number
    ContactStubHelper::setPreferredNumber("");
    QCOMPARE( mFinder->phoneNumber(contact), cntPhoneNumberWithActionCall );
}

void UT_LogsCntFinder::testSetPreferDefaultNumber()
{
    LogsCntEntryHandle* handle = (LogsCntEntryHandle*)1;
    LogsCntEntry* entry = new LogsCntEntry(*handle, 0);
    mFinder->mResults.append(entry);
    
    // Search results exist, mPreferDefaultNumber not changed
    QVERIFY( !mFinder->mPreferDefaultNumber );
    mFinder->setPreferDefaultNumber(false);
    QVERIFY( !mFinder->mPreferDefaultNumber );
    QCOMPARE( mFinder->mResults.count(), 1 );
    
    // Search results exist, mPreferDefaultNumber is changed => results are reset
    mFinder->setPreferDefaultNumber(true);
    QVERIFY( mFinder->mPreferDefaultNumber );
    QCOMPARE( mFinder->mResults.count(), 0 );
} 
