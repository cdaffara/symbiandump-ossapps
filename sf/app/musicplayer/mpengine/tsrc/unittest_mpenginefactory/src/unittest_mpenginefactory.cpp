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
* Description: Unit test for mpenginefactory
*
*/

#include <QSignalSpy>

#include "unittest_mpenginefactory.h"

// Do this so we can access all member variables.
#define private public
#include "mpenginefactory.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestMpEngineFactory tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpenginefactory.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpEngineFactory::TestMpEngineFactory()
    : mTest(0)
{
}

TestMpEngineFactory::~TestMpEngineFactory()
{
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpEngineFactory::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpEngineFactory::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpEngineFactory::init()
{

}

/*!
 Called after every testfunction.
 */
void TestMpEngineFactory::cleanup()
{

}


void TestMpEngineFactory::testInstance()
{
    mTest =  MpEngineFactory::instance();
    QVERIFY( mTest != 0 );
    MpEngineFactory *factory = MpEngineFactory::instance();
    QVERIFY( mTest == factory );
}


void TestMpEngineFactory::testCreateSharedEngine()
{
    MpEngine *sharedEngine = MpEngineFactory::createSharedEngine();
    QVERIFY( sharedEngine != 0 );
    MpEngine *engine = MpEngineFactory::createSharedEngine();
    QVERIFY( sharedEngine == engine );
    
    delete MpEngineFactory::instance()->mSharedEngine;
    MpEngineFactory::instance()->mSharedEngine = 0;
}

void TestMpEngineFactory::testCreateIsolatedEngine()
{
    MpEngine *engineOne = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QVERIFY( engineOne != 0 );
    MpEngine *engineTwo = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QVERIFY( engineTwo != 0 );
    QVERIFY( engineOne != engineTwo );
    QVERIFY( MpEngineFactory::instance()->mEngines.count() == 2 );
        
    MpEngine *ptr;
    foreach ( ptr, MpEngineFactory::instance()->mEngines ) {
        delete ptr;
        ptr = 0;
    }
    // This might be a potential bug in MpEngineFactory, need to call clear in close() function
    MpEngineFactory::instance()->mEngines.clear();
}

void TestMpEngineFactory::testSharedEngine() 
{    
    MpEngine *sharedEngine = MpEngineFactory::createSharedEngine();
    QVERIFY( sharedEngine != 0 );
    MpEngine *engine = MpEngineFactory::sharedEngine();
    QVERIFY( sharedEngine == engine );
}

void TestMpEngineFactory::testClose() 
{
    MpEngine *sharedEngine = MpEngineFactory::createSharedEngine();
    MpEngine *engineOne = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    MpEngine *engineTwo = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
       
    MpEngineFactory::close();
    MpEngineFactory::instance()->mEngines.clear();
    QVERIFY( MpEngineFactory::instance()->mSharedEngine == 0 );
}


// End of file
