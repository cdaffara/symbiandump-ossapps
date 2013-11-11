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

#include "unittest_imagedecoderwrapper.h"
#include "glximagedecoderwrapper.h"
#include <e32base.h>

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestGlxImageDecoderWrapper::initTestCase()
{
    mTestObject = 0;
    TRAP_IGNORE(mTestObject = new GlxImageDecoderWrapper());
    QVERIFY(mTestObject);
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestGlxImageDecoderWrapper::init()
{
    
}

// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestGlxImageDecoderWrapper::cleanup()
{
    
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestGlxImageDecoderWrapper::cleanupTestCase()
{
    if(mTestObject)
    {
        delete mTestObject;
        mTestObject = 0;
    }
}

void TestGlxImageDecoderWrapper::testgetPixmap() 
{
    QVERIFY(mTestObject->getPixmap().isNull());
}

void TestGlxImageDecoderWrapper::testDecodeImage()
{
    QString imagePath = "c:\\data\\images\\Battle.jpg";
    TRAP_IGNORE(mTestObject->decodeImage(imagePath));
    QTest::qWait(1000);
    //QEXPECT_FAIL("", "Will fix in the next release", Continue);
    QVERIFY(!mTestObject->getPixmap().isNull());
} 

void TestGlxImageDecoderWrapper::testResetDecoder()
{
    QString imagePath = "c:\\data\\images\\Battle.jpg";
    TRAP_IGNORE(mTestObject->decodeImage(imagePath));
    QTest::qWait(1000);
    //QEXPECT_FAIL("", "This should fail", Continue);
    QVERIFY(!mTestObject->getPixmap().isNull());

    mTestObject->resetDecoder();
    QVERIFY(mTestObject->getPixmap().isNull());
} 

QTEST_MAIN(TestGlxImageDecoderWrapper)
#include "moc_unittest_imagedecoderwrapper.cpp"
