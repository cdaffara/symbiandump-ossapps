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

#include <QTest>

#include "cxeerror.h"
#include "cxutils.h"
#include "cxeimagedataitemsymbian.h"
#include "cxeimagedataqueueunit.h"
#include "unittest_cxeimagedataqueuesymbian.h"


/**
* UnitTestCxeImageDataQueueSymbian::UnitTestCxeImageDataQueueSymbian()
*/
UnitTestCxeImageDataQueueSymbian::UnitTestCxeImageDataQueueSymbian()
    : mImageDataQueue(NULL)
{
}

/**
* UnitTestCxeImageDataQueueSymbian::~UnitTestCxeImageDataQueueSymbian()
*/
UnitTestCxeImageDataQueueSymbian::~UnitTestCxeImageDataQueueSymbian()
{
    cleanup();
}

/**
* UnitTestCxeImageDataQueueSymbian::init()
*/
void UnitTestCxeImageDataQueueSymbian::init()
{
    mImageDataQueue = new CxeImageDataQueueUnit();
}

/**
* UnitTestCxeImageDataQueueSymbian::cleanup()
*/
void UnitTestCxeImageDataQueueSymbian::cleanup()
{
    delete mImageDataQueue;
    mImageDataQueue = NULL;
}


/**
* UnitTestCxeImageDataQueueSymbian::testSize()
*/
void UnitTestCxeImageDataQueueSymbian::testSize()
{
    QVERIFY(mImageDataQueue->size() == 0);
}

/**
* UnitTestCxeImageDataQueueSymbian::testClear1()
*/
void UnitTestCxeImageDataQueueSymbian::testClear1()
{
    // Call to empty queue, expect no errors.
    mImageDataQueue->clear();
    QVERIFY(mImageDataQueue->size() == 0);
}

/**
* UnitTestCxeImageDataQueueSymbian::testClear2()
*/
void UnitTestCxeImageDataQueueSymbian::testClear2()
{
    // Add some items
    QByteArray data("test");
    mImageDataQueue->startSave(data, "C:\\Images\\unittest1.jpeg", 0, false);
    mImageDataQueue->startSave(data, "C:\\Images\\unittest2.jpeg", 1, false);

    // Test that clearing works also with some content.
    mImageDataQueue->clear();
    QVERIFY(mImageDataQueue->size() == 0);
}

/**
* UnitTestCxeImageDataQueueSymbian::testOperatorAtIndex()
*/
void UnitTestCxeImageDataQueueSymbian::testOperatorAtIndex()
{
    QByteArray data("test");
    CxeImageDataItemSymbian* itemIn1  = mImageDataQueue->startSave(data, "C:\\Images\\unittest1.jpeg", 0, false);
    CxeImageDataItemSymbian* itemIn2  = mImageDataQueue->startSave(data, "C:\\Images\\unittest2.jpeg", 1, false);
    CxeImageDataItem&        itemOut1 = (*mImageDataQueue)[0];
    CxeImageDataItem&        itemOut2 = (*mImageDataQueue)[1];
    QVERIFY(itemIn1 == &itemOut1);
    QVERIFY(itemIn2 == &itemOut2);
}

/**
* UnitTestCxeImageDataQueueSymbian::testStartSave()
*/
void UnitTestCxeImageDataQueueSymbian::testStartSave()
{
    QByteArray data("test");
    CxeImageDataItemSymbian* itemIn = mImageDataQueue->startSave(data, "C:\\Images\\unittest.jpeg", 0, false);
    QVERIFY(itemIn != NULL);
}

// main() function
QTEST_APPLESS_MAIN(UnitTestCxeImageDataQueueSymbian);

// end of file
