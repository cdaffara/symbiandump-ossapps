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
* Description: Unit test for mpalbumcoverwidget
*
*/


#include <hbicon.h>

#include "unittest_mpalbumcoverwidget.h"

// Do this so we can access all member variables.
#define private public
#include "mpalbumcoverwidget.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestMpAlbumCoverWidget tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpalbumcoverwidget.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpAlbumCoverWidget::TestMpAlbumCoverWidget()
    : mTest(0)
{
}

TestMpAlbumCoverWidget::~TestMpAlbumCoverWidget()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpAlbumCoverWidget::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpAlbumCoverWidget::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpAlbumCoverWidget::init()
{
    mTest = new MpAlbumCoverWidget();
}

/*!
 Called after every testfunction.
 */
void TestMpAlbumCoverWidget::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 test setIcon
 */
void TestMpAlbumCoverWidget::testSetIcon()
{
    HbIcon *icon = new HbIcon();
    mTest->setIcon( *icon );
    
    QVERIFY( *icon == mTest->mIcon );
    delete icon;
}

/*!
 test setDefaultIcon
 */
void TestMpAlbumCoverWidget::testSetDefaultIcon()
{
    HbIcon *icon = new HbIcon();
    mTest->setDefaultIcon( *icon );
    
    QVERIFY( *icon == mTest->mDefaultIcon);
    delete icon;
}



// End of file
