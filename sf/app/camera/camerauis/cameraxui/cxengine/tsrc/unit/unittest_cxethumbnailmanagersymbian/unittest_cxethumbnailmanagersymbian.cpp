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
#include <QSignalSpy>

#include "unittest_cxethumbnailmanagersymbian.h"
#include "cxethumbnailmanagersymbian.h"
#include "cxutils.h"
#include "cxetestutils.h"


UnitTestCxeThumbnailManagerSymbian::UnitTestCxeThumbnailManagerSymbian()
{
}


UnitTestCxeThumbnailManagerSymbian::~UnitTestCxeThumbnailManagerSymbian()
{
    cleanup();
}


void UnitTestCxeThumbnailManagerSymbian::init()
{
    mThumbnailManager = new CxeThumbnailManagerSymbian();
}

void UnitTestCxeThumbnailManagerSymbian::cleanup()
{
    delete mThumbnailManager;
    mThumbnailManager = NULL;
}

void UnitTestCxeThumbnailManagerSymbian::testCreateThumbnail()
{
    CX_DEBUG_ENTER_FUNCTION();

    // case 1: Testing with wrong dummy file. Check only that call is made successfully.
    mThumbnailManager->createThumbnail(QString(""), QImage());

    // case 2: Testing with proper file name.
    mThumbnailManager->createThumbnail(QString("C:\\test.jpg"), QImage());

    CX_DEBUG_EXIT_FUNCTION();
}


void UnitTestCxeThumbnailManagerSymbian::testCancelThumbnail()
{
    CX_DEBUG_ENTER_FUNCTION();

    // case 1: Cancel with empty string.
    mThumbnailManager->cancelThumbnail(QString());

    // case 2: Cancel non-existent file / request.
    mThumbnailManager->cancelThumbnail(QString("filename"));

    // case 3: Cancel actual request.
    QString file("C:\\test2.jpg");
    mThumbnailManager->createThumbnail(file, QImage());
    mThumbnailManager->cancelThumbnail(file);

    CX_DEBUG_EXIT_FUNCTION();
}

// main() function non-GUI testing
QTEST_MAIN(UnitTestCxeThumbnailManagerSymbian);

// End of file
