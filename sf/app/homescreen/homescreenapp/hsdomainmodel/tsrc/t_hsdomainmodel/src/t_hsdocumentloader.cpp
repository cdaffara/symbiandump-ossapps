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
* Description:  Homescreen document loader class.
*
*/

#include "t_hsdomainmodel.h"
#include "hsdocumentloader.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestHsDomainModel::testDocumentLoaderConstructors()
{
    HsDocumentLoader *loaderFromHeap = new HsDocumentLoader();
    QVERIFY(loaderFromHeap);
    delete loaderFromHeap;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestHsDomainModel::testDocumentLoaderCreateObjects()
{
    HsDocumentLoader *loader = new HsDocumentLoader();
    QVERIFY(loader);

    QObject *testObject = loader->createObject(QString("UnknownWidgetType"), QString("testNullReturn"));
    QVERIFY(!testObject);

    testObject = loader->createObject(QString("HbPushButton"), QString("testNotAnyHsWidget"));
    QVERIFY(testObject);
    QCOMPARE(testObject->objectName(), QString("testNotAnyHsWidget"));
    delete testObject;
    testObject = 0;

    testObject = loader->createObject(QString("HsPageIndicator"), QString("HsPageIndicator"));
    QVERIFY(testObject);
    QCOMPARE(testObject->objectName(), QString("HsPageIndicator"));
    delete testObject;
    testObject = 0;

    testObject = loader->createObject(QString("HsTrashBinWidget"), QString("HsTrashBinWidget"));
    QVERIFY(testObject);
    QCOMPARE(testObject->objectName(), QString("HsTrashBinWidget"));
    delete testObject;
    testObject = 0;

    delete loader;
}
