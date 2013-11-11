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
* Description: Unit test for mpcollectiondocumentloader
*
*/

#include <qnamespace.h>

#include "unittest_mpcollectiondocumentloader.h"
#include "mpnowplayingwidget.h"
#include "hgmediawall.h"

// Do this so we can access all member variables.
#define private public
#include "mpcollectiondocumentloader.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpCollectionDocumentLoader tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpcollectiondocumentloader.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpCollectionDocumentLoader::TestMpCollectionDocumentLoader()
    : mTest(0)
{
}

TestMpCollectionDocumentLoader::~TestMpCollectionDocumentLoader()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpCollectionDocumentLoader::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpCollectionDocumentLoader::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpCollectionDocumentLoader::init()
{
    mTest = new MpCollectionDocumentLoader();
}

/*!
 Called after every testfunction.
 */
void TestMpCollectionDocumentLoader::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests the correct creation of MpNowPlayingWidget.
 */
void TestMpCollectionDocumentLoader::testCreateMpNowPlayingWidget()
{
    QObject *theObject;
    theObject = mTest->createObject(QString("MpNowPlayingWidget"), QString("myMpNowPlayingWidget"));
    QCOMPARE(theObject->metaObject()->className(), "MpNowPlayingWidget");
    QCOMPARE(theObject->objectName(), QString("myMpNowPlayingWidget"));
    QVERIFY(qobject_cast<MpNowPlayingWidget*>(theObject));
    delete theObject;
}

/*!
 Tests the correct creation of HgMediawall.
 */
void TestMpCollectionDocumentLoader::testCreateHgMediawall()
{
    QObject *theObject;
    theObject = mTest->createObject(QString("HgMediawall"), QString("HgMediawall"));
    QCOMPARE(theObject->metaObject()->className(),"HgMediawall");
    QCOMPARE(theObject->objectName(),QString("HgMediawall"));
    QVERIFY(qobject_cast<HgMediawall*>(theObject));
    delete theObject;
}

/*!
 Tests the correct creation of QObject, this should be pased to the
 base class and base class should return a named object.
 */
void TestMpCollectionDocumentLoader::testCreateQObject()
{
    QObject *theObject;
    theObject = mTest->createObject(QString("QObject"), QString("myQObject"));
    QCOMPARE(theObject->metaObject()->className(), "QObject");
    QCOMPARE(theObject->objectName(), QString("myQObject"));
    delete theObject;
}

