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
* Description: Unit Test for mpdetailsview.
*
*/
#include <QSignalSpy>
#include <QGraphicsWebView>
#include <QWebSettings>
#include <QGraphicsWidget>
#include <QGraphicsLayoutItem>
#include <QUrl>
#include <QSslError>
#include <QDomElement>
#include <QWebPage>
#include <QWebFrame>
#include <QList>
#include <QFile>
#include <QTranslator>
#include <QLocale>

#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbgroupbox.h>
#include <hbdocumentloader.h>
#include <hbwidget.h>
#include <hbpushbutton.h>
#include <hbpopup.h>
#include <hbdialog.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbapplication.h>
#include <hbprogressbar.h>

#include "unittest_mpdetailsview.h"
#include "stub/inc/mpsongdata.h"
#include "stub/inc/thumbnailmanager_qt.h"
#include "mpcommondefs.h"
#include "mpsettingsmanager.h"

// Do this so we can access all member variables.
#define private public
#include "mpdetailsview.h"
#undef private

// This so we can test private functions
#include "../../src/mpdetailsview.cpp"

#include "../moc_mpdetailsview.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpDetailsView tv;

if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpdetailsview.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

//Constructor
TestMpDetailsView::TestMpDetailsView()
    : mTest(0)
{

}

//Destructor
TestMpDetailsView::~TestMpDetailsView()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpDetailsView::initTestCase()
{

}

/*!
 Called after the last testfunction was executed.
 */
void TestMpDetailsView::cleanupTestCase()
{
QCoreApplication::processEvents();
}

/*!
 Called before each testfunction is executed.
 */
void TestMpDetailsView::init()
{
    mTest = new MpDetailsView();
    mTest->initializeView();
}

/*!
 Called after every testfunction.
 */
void TestMpDetailsView::cleanup()
{
    delete mTest;
    mTest = 0;
}

void TestMpDetailsView::testactivateView()
{
    mTest->activateView();
    QCOMPARE( mTest->mActivated, true );
}

void TestMpDetailsView::testdeactivateView()
{
    mTest->deactivateView();
    QCOMPARE( mTest->mActivated, false );
}

void TestMpDetailsView::testbackSlot()
{
    connect( this, SIGNAL( back() ), mTest->mSoftKeyBack, SIGNAL( triggered() ) );
    QSignalSpy spy( mTest, SIGNAL( command(int) ) );
    QVERIFY( spy.isValid() );
    QCOMPARE( spy.count(),0 );

    emit back();
    QCOMPARE( spy.count(), 1 );
}

void TestMpDetailsView::testAlbumArtChanged()
{
    connect( this, SIGNAL( albumArtChanged() ), mTest->mSongData, SIGNAL( albumArtReady() ) );

    emit albumArtChanged();
    QVERIFY( mTest->mAlbumArt->icon().isNull() == false );
}

void TestMpDetailsView::testHandlePlaybackInfoChanged()
{
    mTest->mSongData = new MpSongData();
    mTest->mInspireMeQueryRendered = true;
    mTest->mSongData->mAlbum = QString( "album" );
    mTest->mSongData->mArtist = QString( "artist" );
    mTest->handlePlaybackInfoChanged();
    QVERIFY( mTest->mInspireMeQueryRendered == false );
    QVERIFY( mTest->mSongData->link().isEmpty() == true );
    QCOMPARE( mTest->mAlbumText->plainText(), QString( "album" ) );
    QCOMPARE( mTest->mArtistText->plainText(), QString( "artist" ) );

    mTest->mInspireMeQueryRendered = true;
    mTest->mSongData->mAlbum = QString("");
    mTest->mSongData->mArtist = QString("");
    mTest->handlePlaybackInfoChanged();
    QVERIFY( mTest->mInspireMeQueryRendered == true );
    QVERIFY( mTest->mSongData->link().isEmpty() == true );
    QCOMPARE( mTest->mAlbumText->plainText(), QString( "txt_mus_other_unknown7" ) );
    QCOMPARE( mTest->mArtistText->plainText(), QString( "txt_mus_other_unknown6" ) );
}

void TestMpDetailsView::testHandleDetailsGroupBoxToggled()
{
    mTest->handleDetailsGroupBoxToggled( false );
    QVERIFY( mTest->mInspireMeGroupBox->isCollapsed() == true );
}

void TestMpDetailsView::testHandleInspireMeGroupBoxToggled()
{
    mTest->mSongData = new MpSongData();

    mTest->mInspireMeQueryOngoing = true;
    mTest->handleInspireMeGroupBoxToggled( false );
    QVERIFY( mTest->mSongDetailsGroupBox->isCollapsed() == true );
    QVERIFY ( mTest->mInspireMeProgressBar->isVisible() == true );

    mTest->mInspireMeQueryOngoing = false;
    mTest->mInspireMeQueryRendered = true;
    mTest->handleInspireMeGroupBoxToggled( false );
    QVERIFY( mTest->mSongDetailsGroupBox->isCollapsed() == true );

    mTest->mInspireMeQueryOngoing = false;
    mTest->mInspireMeQueryRendered = false;
    mTest->mSongData->mAlbum = QString( "album" );
    mTest->mSongData->mArtist = QString( "artist" );
    mTest->handleInspireMeGroupBoxToggled( false );
    QVERIFY( mTest->mSongDetailsGroupBox->isCollapsed() == true );
    QVERIFY ( mTest->mInspireMeProgressBar->isVisible() == true );
    QVERIFY ( mTest->mInspireMeQueryOngoing == true );
}



