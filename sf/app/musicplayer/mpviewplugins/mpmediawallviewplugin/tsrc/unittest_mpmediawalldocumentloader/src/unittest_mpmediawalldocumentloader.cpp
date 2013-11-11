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
* Description: Unit test for MpMediaWallDocumentLoader
*
*/

#include <qnamespace.h>

#include "stub/inc/hgmediawall.h"
#include "unittest_mpmediawalldocumentloader.h"



// Do this so we can access all member variables.
#define private public
#include "mpmediawalldocumentloader.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpMediaWallDocumentLoader tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmediawalldocumentloader.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpMediaWallDocumentLoader::TestMpMediaWallDocumentLoader()
    : mTest(0)
{
}

TestMpMediaWallDocumentLoader::~TestMpMediaWallDocumentLoader()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMediaWallDocumentLoader::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMediaWallDocumentLoader::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMediaWallDocumentLoader::init()
{
    mTest = new MpMediaWallDocumentLoader();
}

/*!
 Called after every testfunction.
 */
void TestMpMediaWallDocumentLoader::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests the correct creation of HgMediawall.
 */
void TestMpMediaWallDocumentLoader::testCreateHgMediawall()
{
    cleanup();
    init();
    QObject *theObject;
    theObject = mTest->createObject(QString("HgMediawall"),QString("myHgMediawall"));
    QCOMPARE(theObject->metaObject()->className(),"HgMediawall");
    QCOMPARE(theObject->objectName(),QString("myHgMediawall"));
    QVERIFY(qobject_cast<HgMediawall*>(theObject));
    delete theObject;

}

/*!
 Tests the correct creation of QObject, this should be pased to the
 base class and base class should return a named object.
 */
void TestMpMediaWallDocumentLoader::testCreateQObject()
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
