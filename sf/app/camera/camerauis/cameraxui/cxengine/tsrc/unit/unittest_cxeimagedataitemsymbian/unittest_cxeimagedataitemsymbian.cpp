/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QDate>
#include <QTest>
#include <QDir>
#include <QDebug>
#include <QSignalSpy>

#include <bitdev.h>

#include "unittest_cxeimagedataitemsymbian.h"
#include "cxeimagedataitemunit.h"
#include "cxestillimagesymbian.h"
#include "cxutils.h"
#include "cxeerror.h"


UnitTestCxeImageDataItemSymbian::UnitTestCxeImageDataItemSymbian()
: mImageDataItem(NULL),
  mImageCounter(-1)
{
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
}

UnitTestCxeImageDataItemSymbian::~UnitTestCxeImageDataItemSymbian()
{
    delete mImageDataItem;
}

/*!
* Initializes resources.
*/
void UnitTestCxeImageDataItemSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();
    mFilename = generateImageFileName();
    QByteArray data = "1234";
    mImageDataItem = new CxeImageDataItemUnit(mImageCounter, data, mFilename, false);
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SavePending);
	QCOMPARE(mImageDataItem->isLocationEnabled(), false);

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* cleans up resources for each test case
*/
void UnitTestCxeImageDataItemSymbian::cleanup()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    delete mImageDataItem;
    mImageDataItem = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Initializes resources before any test cases.
*/
void UnitTestCxeImageDataItemSymbian::initTestCase()
{
    CX_DEBUG_ENTER_FUNCTION();

    // create folder for storing temporary image files
    QDir dir;
    dir.mkpath(path());

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* cleans up resources after last test case
*/
void UnitTestCxeImageDataItemSymbian::cleanupTestCase()
{
    CX_DEBUG_ENTER_FUNCTION();

    // remove temporary image files and folder
    QDir dir;
    QString filepath = path();
    dir.setPath(filepath);

    QStringList filters;
    filters << "*.jpg";
    dir.setNameFilters(filters);
    
    QStringList filenames = dir.entryList(filters);
    
    // delete temporary created files
    foreach(const QString &file, filenames) {
        dir.remove(filepath + file);
    }
    
    // delete the directory created for temporary saving image files.
    dir.rmpath(filepath);

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* UnitTestCxeImageDataItemSymbian::testState
*/
void UnitTestCxeImageDataItemSymbian::testSave()
{
    CX_DEBUG_ENTER_FUNCTION();
    const int id = mImageDataItem->id();

    QSignalSpy deviceStateSpy(mImageDataItem, SIGNAL(imageSaved(CxeError::Id, const QString&, int)));
    QVERIFY(deviceStateSpy.isValid());

    CxeError::Id err = mImageDataItem->save();

    QVERIFY(mImageDataItem->state() == CxeImageDataItem::Saved);
    QCOMPARE(err, CxeError::None);

    QCOMPARE( deviceStateSpy.count(), 1 );
    if (deviceStateSpy.count() > 0) {
        QList<QVariant> initModeArguments = deviceStateSpy.takeFirst();
        QCOMPARE(initModeArguments.at(0).value<CxeError::Id>(), CxeError::None);
        QVERIFY( initModeArguments.at(1).toString().compare(mFilename) == 0);
        QVERIFY( initModeArguments.at(2).toInt() == id);
    }

    // try to test possible fail cases.
    QString filename = NULL;
    QByteArray data = "";

    CX_DEBUG(("UnitTestCxeImageDataItemSymbian::testSave <> Invalid arguments"));
    cleanup();
    mImageDataItem = new CxeImageDataItemUnit(mImageCounter, data, filename, false);
    err = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SaveFailed);
    
    // Arugments are not valid, leaves with KErrArgument which is mapped internally to CxError::General.
    QCOMPARE(err, CxeError::General);

    CX_DEBUG(("UnitTestCxeImageDataItemSymbian::testSave <> Invalid path"));
    cleanup();
    filename = dummyPath("C");
    mImageDataItem = new CxeImageDataItemUnit(mImageCounter, data, filename, false);
    err = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SaveFailed);
    QVERIFY(err != CxeError::None);

    CX_DEBUG(("UnitTestCxeImageDataItemSymbian::testSave <> invalid drive"));
    cleanup();
    filename = dummyPath("");
    mImageDataItem = new CxeImageDataItemUnit(mImageCounter, data, filename, false);
    err = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SaveFailed);
    QVERIFY(err != CxeError::None);

    CX_DEBUG(("UnitTestCxeImageDataItemSymbian::testSave <> invalid drive - 2"));
    cleanup();
    filename = dummyPath("12");
    mImageDataItem = new CxeImageDataItemUnit(mImageCounter, data, filename, false);
    err = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SaveFailed);
    QVERIFY(err != CxeError::None);

    CX_DEBUG(("UnitTestCxeImageDataItemSymbian::testSave <> invalid drive - 3"));
    cleanup();
    filename = dummyPath("Edata");
    mImageDataItem = new CxeImageDataItemUnit(mImageCounter, data, filename, false);
    err = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SaveFailed);
    QVERIFY(err != CxeError::None);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeImageDataItemSymbian::testPath()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(mImageDataItem->path().compare(mFilename) == 0);
    CX_DEBUG_EXIT_FUNCTION();
}




/*!
* UnitTestCxeImageDataItemSymbian::testState
*/
void UnitTestCxeImageDataItemSymbian::testState()
{
    CX_DEBUG_ENTER_FUNCTION();
    mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::Saved);
    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* UnitTestCxeImageDataItemSymbian::testId
*/
void UnitTestCxeImageDataItemSymbian::testId()
{
    CX_DEBUG_ENTER_FUNCTION();
	// checking if initialization of image data item is successful.
    QCOMPARE(mImageDataItem->id(), mImageCounter);
    mImageDataItem->save();
    QCOMPARE(mImageDataItem->id(), mImageCounter);
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* UnitTestCxeImageDataItemSymbian::testIsLocationEnabled
*/
void UnitTestCxeImageDataItemSymbian::testIsLocationEnabled()
{
    CX_DEBUG_ENTER_FUNCTION();

	// spl case to check if location is enabled	
	cleanup();
    bool enableLocation(true);
    QByteArray data = "1234";
    QString filename = generateImageFileName();
    mImageDataItem = new CxeImageDataItemUnit(mImageCounter, data, filename, enableLocation);
    QCOMPARE(mImageDataItem->isLocationEnabled(), enableLocation);

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Generates filename.
* @param counter is associated with image file name.
*/
QString UnitTestCxeImageDataItemSymbian::generateImageFileName()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    mImageCounter++;
    QString filename;
    filename.sprintf("%04d_Nokia.jpg", mImageCounter);
    filename = path() + filename;

    CX_DEBUG_EXIT_FUNCTION();
    return filename;
}

/*!
* Create path for saving images to be used in testcases.
*/
QString UnitTestCxeImageDataItemSymbian::path()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    QString path;

#ifdef __WINSCW__
    path.sprintf("c:\\data\\testimages\\");
#else
    path.sprintf("e:\\testimages\\");
#endif

    CX_DEBUG_EXIT_FUNCTION();
    
    return path;
}



/*!
* Create a dummy invalid path for testcases.
*/
QString UnitTestCxeImageDataItemSymbian::dummyPath(const QString &drive)
{
    CX_DEBUG_ENTER_FUNCTION();
    mImageCounter++;
    QString filename;
    filename.sprintf("%s:\\Dummy\\Camera\\%04d_Nokia.jpg",
            drive.toAscii().data(),
            mImageCounter);
    CX_DEBUG_EXIT_FUNCTION();
    return filename;
}

// main() function non-GUI testing
QTEST_APPLESS_MAIN(UnitTestCxeImageDataItemSymbian);
