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
* Description: Unit test for MpPlaybackDocumentLoader
*
*/

#include <qnamespace.h>
#include <hbtoolbutton.h>

#include "mpalbumcoverwidget.h"
#include "unittest_mpplaybackdocumentloader.h"



// Do this so we can access all member variables.
#define private public
#include "mpplaybackdocumentloader.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpPlaybackDocumentLoader tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpplaybackdocumentloader.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpPlaybackDocumentLoader::TestMpPlaybackDocumentLoader()
    : mTest(0)
{
}

TestMpPlaybackDocumentLoader::~TestMpPlaybackDocumentLoader()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpPlaybackDocumentLoader::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpPlaybackDocumentLoader::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpPlaybackDocumentLoader::init()
{
    mTest = new MpPlaybackDocumentLoader();
}

/*!
 Called after every testfunction.
 */
void TestMpPlaybackDocumentLoader::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests the correct creation of MpAlbumCoverWidget.
 */
void TestMpPlaybackDocumentLoader::testCreateMpAlbumCoverWidget()
{
    cleanup();
    init();
    QObject *theObject;
    theObject = mTest->createObject(QString("MpAlbumCoverWidget"),QString("myMpAlbumCoverWidget"));
    QCOMPARE(theObject->metaObject()->className(),"MpAlbumCoverWidget");
    QCOMPARE(theObject->objectName(),QString("myMpAlbumCoverWidget"));
    QVERIFY(qobject_cast<MpAlbumCoverWidget*>(theObject));
    delete theObject;

}

/*!
 Tests the correct creation of QObject, this should be pased to the
 base class and base class should return a named object.
 */
void TestMpPlaybackDocumentLoader::testCreateQObject()
{
    cleanup();
    init();
    QObject *theObject;
    theObject = mTest->createObject(QString("QObject"),QString("myQObject"));
    QCOMPARE(theObject->metaObject()->className(),"QObject");
    QCOMPARE(theObject->objectName(),QString("myQObject"));
    delete theObject;
}

// End of file
