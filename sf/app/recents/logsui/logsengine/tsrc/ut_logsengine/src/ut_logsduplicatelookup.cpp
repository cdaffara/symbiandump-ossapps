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


#include "ut_logsduplicatelookup.h"
#include "logsduplicatelookup.h"
#include "logsevent.h"
#include <QtTest/QtTest>

QString testDir1key1 = "remote1/t1111";
QString testDir1key2 = "remote2/t1111";

QString testDir2key1 = "remote1/t2222";

QString testDir3key1 = "remote1/t3333";
QString testDir3key2 = "remote2/t3333";
QString testDir3key3 = "remote3/t3333";


void UT_LogsDuplicateLookup::initTestCase()
{
    mLookup = new LogsDuplicateLookup(); 
}

void UT_LogsDuplicateLookup::cleanupTestCase()
{
    delete mLookup;
    mLookup = NULL;
}

void UT_LogsDuplicateLookup::init()
{
}
     
void UT_LogsDuplicateLookup::cleanup()
{
}
    
void UT_LogsDuplicateLookup::testConstructor()
{
    QVERIFY( mLookup );
    QCOMPARE( mLookup->mDirectionLookupTables.count(), 3 );
}

void UT_LogsDuplicateLookup::testInvalidate()
{
    createTestData();
    LogsEvent ev1;
    mLookup->mDirectionLookupTables.at(1)->find(testDir2key1).value() = &ev1;
    LogsEvent ev2;
    mLookup->mDirectionLookupTables.at(2)->find(testDir3key2).value() = &ev2;
    mLookup->invalidate();
    QVERIFY( mLookup->mDirectionLookupTables.at(0)->find(testDir1key1).value() == 0 );
    QVERIFY( mLookup->mDirectionLookupTables.at(1)->find(testDir2key1).value() == 0 );
    QVERIFY( mLookup->mDirectionLookupTables.at(2)->find(testDir3key2).value() == 0 );
    
}
void UT_LogsDuplicateLookup::testCleanup()
{
    createTestData();
    
    // All expect one with value are cleared
    QCOMPARE( mLookup->mDirectionLookupTables.at(0)->count(), 2 );
    QCOMPARE( mLookup->mDirectionLookupTables.at(1)->count(), 1 );
    QCOMPARE( mLookup->mDirectionLookupTables.at(2)->count(), 3 );
    
    LogsEvent ev1;
    mLookup->mDirectionLookupTables.at(2)->find(testDir3key2).value() = &ev1;
    mLookup->cleanup();
    QCOMPARE( mLookup->mDirectionLookupTables.at(0)->count(), 0 );
    QCOMPARE( mLookup->mDirectionLookupTables.at(1)->count(), 0 );
    QCOMPARE( mLookup->mDirectionLookupTables.at(2)->count(), 1 );
    QVERIFY( mLookup->mDirectionLookupTables.at(2)->find(testDir3key2).value() == &ev1 );
    
}
void UT_LogsDuplicateLookup::testAddLookupEntry()
{
    // Event does not have remote name
    LogsEvent ev;
    ev.setLogId(100);
    QVERIFY( mLookup->addLookupEntry(ev) == -1 );
    
    // Event contains wrong direction
    ev.setRemoteParty("remote1");
    ev.setNumber("1234");
    QVERIFY( mLookup->addLookupEntry(ev) == -1 );
    
    // No key yet for the event, key is generated from remote name and number
    ev.setDirection(LogsEvent::DirIn);
    QVERIFY( mLookup->addLookupEntry(ev) == 0 );
    QVERIFY( mLookup->mDirectionLookupTables.at(LogsEvent::DirIn)->find( "remote1/t1234" ).value() == &ev );
    
    // Several keys exists and value for matching key exists, value not changed
    createTestData();
    QVERIFY( mLookup->addLookupEntry(ev) == 0 );
    LogsEvent ev2;
    ev2.setDirection(LogsEvent::DirIn);
    ev2.setRemoteParty("remote1");
    ev2.setNumber("1234");
    ev2.setLogId(200);
    QVERIFY( mLookup->addLookupEntry(ev2) == 0 );
    QVERIFY( mLookup->mDirectionLookupTables.at(LogsEvent::DirIn)->find( "remote1/t1234" ).value() == &ev );
    
    // This direction entry does not exists, entry is added
    ev2.setDirection(LogsEvent::DirOut);
    QVERIFY( mLookup->addLookupEntry(ev2) == 0 );
    QVERIFY( mLookup->mDirectionLookupTables.at(LogsEvent::DirOut)->find( "remote1/t1234" ).value() == &ev2 );
    
    // Several keys exists and no value for matching key, value is set
    mLookup->mDirectionLookupTables.at(LogsEvent::DirIn)->find( "remote1/t1234" ).value() = 0;
    ev2.setDirection(LogsEvent::DirIn);
    QVERIFY( mLookup->addLookupEntry(ev2) == 0 );
    QVERIFY( mLookup->mDirectionLookupTables.at(LogsEvent::DirIn)->find( "remote1/t1234" ).value() == &ev2 );
}
void UT_LogsDuplicateLookup::testFindDuplicate()
{
    // Not valid search term
    LogsEvent ev;
    ev.setLogId(100);
    QVERIFY( mLookup->findDuplicate(ev) == 0 );
    
    createTestData();
    LogsEvent ev1;
    mLookup->mDirectionLookupTables.at(2)->find(testDir3key2).value() = &ev1;
    LogsEvent ev2;
    mLookup->mDirectionLookupTables.at(2)->find(testDir3key3).value() = &ev2;
    LogsEvent searchTerm;
    searchTerm.setDirection(LogsEvent::DirIn);
    searchTerm.setRemoteParty("remote1");
    searchTerm.setNumber("1111");
    QVERIFY( mLookup->findDuplicate(searchTerm) == 0 );
    
    searchTerm.setDirection(LogsEvent::DirMissed);
    searchTerm.setRemoteParty("remote2");
    searchTerm.setNumber("3333");
    QVERIFY( mLookup->findDuplicate(searchTerm) == &ev1 );

    searchTerm.setRemoteParty("remote3");
    QVERIFY( mLookup->findDuplicate(searchTerm) == &ev2 );
    
    // Check that duplicate lookup works with international format also
    LogsEvent ev5;
    ev5.setDirection(LogsEvent::DirOut);
    ev5.setRemoteParty("remote2");
    ev5.setNumber("+358504443333");
    mLookup->addLookupEntry(ev5);
    LogsEvent searchTerm2;
    searchTerm2.setDirection(LogsEvent::DirOut);
    searchTerm2.setRemoteParty("remote2");
    searchTerm2.setNumber("0504443333");
    QVERIFY( mLookup->findDuplicate(searchTerm2) == &ev5 );
    
}

void UT_LogsDuplicateLookup::createTestData()
{
    foreach( LogsLookupHash* dir, mLookup->mDirectionLookupTables ){
        dir->clear();
    }
    mLookup->mDirectionLookupTables.at(0)->insert(testDir1key1, 0);
    mLookup->mDirectionLookupTables.at(0)->insert(testDir1key2, 0);
    
    mLookup->mDirectionLookupTables.at(1)->insert(testDir2key1, 0);
    
    mLookup->mDirectionLookupTables.at(2)->insert(testDir3key1, 0);
    mLookup->mDirectionLookupTables.at(2)->insert(testDir3key2, 0);
    mLookup->mDirectionLookupTables.at(2)->insert(testDir3key3, 0);
}
