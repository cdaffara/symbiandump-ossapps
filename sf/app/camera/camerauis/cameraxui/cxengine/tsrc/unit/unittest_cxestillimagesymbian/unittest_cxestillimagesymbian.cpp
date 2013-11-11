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

#include <QImage>
#include "cxutils.h"

#include "unittest_cxestillimagesymbian.h"
#include "cxestillimagesymbian.h"
#include "cxeimagedataitemsymbian.h"

// Enums


UnitTestCxeStillImageSymbian::UnitTestCxeStillImageSymbian()
    : mCxeStillImageSymbian(0)
{
}

UnitTestCxeStillImageSymbian::~UnitTestCxeStillImageSymbian()
{
    cleanup();
}

// Run before each individual test case
void UnitTestCxeStillImageSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();
    mCxeStillImageSymbian = new CxeStillImageSymbian();
    CX_DEBUG_EXIT_FUNCTION();
}

// Run after each individual test case
void UnitTestCxeStillImageSymbian::cleanup()
{
    CX_DEBUG_ENTER_FUNCTION();
    delete mCxeStillImageSymbian;
    mCxeStillImageSymbian = 0;
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeStillImageSymbian::testdataItem()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(!mCxeStillImageSymbian->dataItem());
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeStillImageSymbian::testfilename()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(mCxeStillImageSymbian->filename() == QString(""));
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeStillImageSymbian::testsnapshot()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(!mCxeStillImageSymbian->snapshot());
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeStillImageSymbian::testsaved()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(mCxeStillImageSymbian->saved() == false);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeStillImageSymbian::testsetSnapshot()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(!mCxeStillImageSymbian->snapshot());

    QPixmap snapshot(3,3);
    mCxeStillImageSymbian->setSnapshot(snapshot);
    QCOMPARE(mCxeStillImageSymbian->snapshot(), snapshot);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeStillImageSymbian::testsetFilename()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(mCxeStillImageSymbian->filename().isEmpty());
    QString imagename = QString("imagefile01.jpg");
    mCxeStillImageSymbian->setFilename(imagename);
    //QVERIFY(mCxeStillImageSymbian->filename().compare(QString("imagefile01.jpg")) == 0);
    QCOMPARE(mCxeStillImageSymbian->filename(), imagename);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeStillImageSymbian::testsetSaved()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(mCxeStillImageSymbian->saved() == false);
    mCxeStillImageSymbian->setSaved(true);
    QVERIFY(mCxeStillImageSymbian->saved() == true);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeStillImageSymbian::testsetDataItem()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(!mCxeStillImageSymbian->dataItem());
    QByteArray data("124343423423456e74576");
    QString path = "\\Images\\Camera\\blaah.jpg";
    CxeImageDataItemSymbian* dataItem = new CxeImageDataItemSymbian(data, path, mCxeStillImageSymbian->id(), false);
    mCxeStillImageSymbian->setDataItem(dataItem);
    QVERIFY(mCxeStillImageSymbian->dataItem() == dataItem);
    CX_DEBUG_EXIT_FUNCTION();
}


// main() function
// QPixmap needs QApplication for QPaintDevice,
// so can't use QTEST_APPLESS_MAIN.
QTEST_MAIN(UnitTestCxeStillImageSymbian);

// end of file

