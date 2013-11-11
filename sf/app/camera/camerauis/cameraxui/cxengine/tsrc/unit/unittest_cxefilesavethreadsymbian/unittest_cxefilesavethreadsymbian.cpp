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

#include <QTest>
#include <QByteArray>
#include <QSignalSpy>
#include <QThread>

#include "unittest_cxefilesavethreadsymbian.h"
#include "cxefilesavethreadsymbian.h"
#include "cxefakeimagedataitem.h"
#include "cxetestutils.h"



/**
* UnitTestCxeFileSaveThreadSymbian::UnitTestCxeFileSaveThreadSymbian()
*/
UnitTestCxeFileSaveThreadSymbian::UnitTestCxeFileSaveThreadSymbian()
: mFileSaveThread(NULL)
{
}

/**
* UnitTestCxeFileSaveThreadSymbian::~UnitTestCxeFileSaveThreadSymbian()
*/
UnitTestCxeFileSaveThreadSymbian::~UnitTestCxeFileSaveThreadSymbian()
{
    cleanup();
}

/**
* UnitTestCxeFileSaveThreadSymbian::init()
*/
void UnitTestCxeFileSaveThreadSymbian::init()
{
    mFileSaveThread = new CxeFileSaveThreadSymbian(this);
}

/**
* UnitTestCxeFileSaveThreadSymbian::cleanup()
*/
void UnitTestCxeFileSaveThreadSymbian::cleanup()
{
    delete mFileSaveThread;
    mFileSaveThread = NULL;
}


/**
* UnitTestCxeFileSaveThreadSymbian::testSave()
*/
void UnitTestCxeFileSaveThreadSymbian::testSave()
{
    QString filename("e:\\testdata.txt");

    // Case 0: Checking if a file is saved with all valid parameters
    const int index(2);
    const int id(7);
    CxeImageDataItem *imageDataItem = new CxeFakeImageDataItem(index, QByteArray("unit test successful"), filename, id, false);

    QSignalSpy imageItemStateSpy(imageDataItem, SIGNAL(imageSaved(CxeError::Id, const QString&, int)));

    QVERIFY(imageItemStateSpy.isValid());

    QCOMPARE(imageDataItem->state(), CxeImageDataItem::SavePending);
    //Saving thread takes ownership of dataItem
    mFileSaveThread->save(imageDataItem);
    // wait for imageSavedSignal
    QVERIFY(CxeTestUtils::waitForSignal(imageItemStateSpy, 100));

    QCOMPARE(imageItemStateSpy.count(), 1);
    if (imageItemStateSpy.count() > 0) {
        QList<QVariant> initModeArguments = imageItemStateSpy.takeFirst();
        QCOMPARE(initModeArguments.at(0).toInt(), (int)CxeError::None);
        QCOMPARE(initModeArguments.at(1).toString(), filename);
        QCOMPARE(initModeArguments.at(2).toInt(), id);
    }

    QVERIFY(mFileSaveThread->isRunning());

    // case 1: checking with CxeImageDataItem = NULL
    mFileSaveThread->save(NULL);

}

// main() function
QTEST_MAIN(UnitTestCxeFileSaveThreadSymbian);

// end of file
