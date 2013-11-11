/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   tester for methods in TestFileDetailsWidget
*
*/

// Version : %version:  7 %


#include <QDateTime>
#include <QFileInfo>

#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbextendedlocale.h>

#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"
#include "testfiledetailswidget.h"

#define private public
#include "videoplaybackfiledetailswidget.h"
#undef private

const QString KFILEPATH = "C:\\sample1.wav";


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestFileDetailsWidget tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testfiledetailswidget.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::init()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::init()"));

    mController = new VideoPlaybackControlsController();
    mWidget = new VideoPlaybackFileDetailsWidget( mController );
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::cleanup()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mWidget )
    {
        delete mWidget;
        mWidget = NULL;
    }
}

// ---------------------------------------------------------------------------
// testTitle
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testTitle()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testTitle()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // For Audio only view
    //
    mController->mViewMode = EAudioOnlyView;

    // If title information is available
    details->mTitle = "Title for Test clip";
    mWidget->updateWithFileDetails( details );
    verifyResult( "Title", true, details->mTitle );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    // If title informatio is not available
    details->mTitle = "";
    details->mClipName = KFILEPATH;

    mWidget->updateWithFileDetails( details );
    verifyResult( "Title", true, "sample1" );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // For Details view
    //
    mController->mViewMode = EFullScreenView;

    mWidget->updateWithFileDetails( details );

    verifyResult( "Title", false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testDescription
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testDescription()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testDescription()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mDescription = "This is for unit test";

    mWidget->updateWithFileDetails( details );

    verifyResult( "Description", true, details->mDescription );

    cleanup();
}

// ---------------------------------------------------------------------------
// testDate
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testDuration()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testDuration()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // If it's local mode, need to show the duration
    //
    details->mPlaybackMode = EMPXVideoLocal;
    details->mDuration = 3700000;

    mWidget->updateWithFileDetails( details );

    verifyResult( "Duration", true, "1:01:40" );

    cleanup();
}

// ---------------------------------------------------------------------------
// testDate
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testDate()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testDate()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();

    QDateTime dateTime;
    QString created("");
    QString modified("");
    details->mClipName = KFILEPATH;
    QFileInfo fileInfo( details->mClipName );

    //
    // If it is streaming case, we don't show size information
    //
    details->mPlaybackMode = EMPXVideoStreaming;
    mWidget->updateWithFileDetails( details );

    //
    // verify results for streaming clip
    //
    verifyResult( "Date", false );
    verifyResult( "Modified", false );

    //
    // clear the widget list
    //
    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // local clip, with no date/time metadata
    //
    details->mPlaybackMode = EMPXVideoLocal;
    mWidget->updateWithFileDetails( details );

    //
    // compare and verify 'Date' for non-metadata local clip
    //
    dateTime = fileInfo.created();
    created = dateTimeStringFormat( dateTime );
    verifyResult( "Date", true, created, true );

    //
    // compare and verify 'Modified' for non-metadata local clip
    //
    dateTime = fileInfo.lastModified();
    modified = dateTimeStringFormat( dateTime );
    verifyResult( "Modified", true, modified, true );

    //
    // clear the widget list
    //
    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // local clip, with date/time metadata
    //
    details->mPlaybackMode = EMPXVideoLocal;
    details->mCreationTime = 1242367251;        // POSIX creation time
    details->mModificationTime = 1270773249;    // POSIX modification time
    mWidget->updateWithFileDetails( details );

    //
    // compare and verify 'Date' for metadata local clip
    //
    dateTime.setTime_t( details->mCreationTime );
    created = dateTimeStringFormat( dateTime );
    verifyResult( "Date", true, created, true );

    //
    // compare and verify 'Modified' for metadata local clip
    //
    dateTime.setTime_t( details->mModificationTime );
    modified = dateTimeStringFormat( dateTime );
    verifyResult( "Modified", true, modified, true );

    //
    // clean up
    //
    cleanup();
}

// ---------------------------------------------------------------------------
// testLocation
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testLocation()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testLocation()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mLocation = "C:\\data\\Videos\\";

    mWidget->updateWithFileDetails( details );

    verifyResult( "Location", true, details->mLocation );

    cleanup();
}

// ---------------------------------------------------------------------------
// testAuthor
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testAuthor()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testAuthor()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mArtist = "Fusion";

    mWidget->updateWithFileDetails( details );

    verifyResult( "Author", true, details->mArtist );

    cleanup();
}

// ---------------------------------------------------------------------------
// testCopyright
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testCopyright()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testCopyright()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mCopyright = "Fusion team";

    mWidget->updateWithFileDetails( details );

    verifyResult( "Copyright", true, details->mCopyright );

    cleanup();
}

// ---------------------------------------------------------------------------
// testLanguage
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testLanguage()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testLanguage()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mLanguage = "English";

    mWidget->updateWithFileDetails( details );

    verifyResult( "Language", true, details->mLanguage );

    cleanup();
}

// ---------------------------------------------------------------------------
// testKeywords
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testKeywords()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testKeywords()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mKeywords = "QTest";

    mWidget->updateWithFileDetails( details );

    verifyResult( "Keywords", true, details->mKeywords );

    cleanup();
}

// ---------------------------------------------------------------------------
// testSize
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testSize()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testSize()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // If it is streaming case, we don't show size information
    //
    details->mPlaybackMode = EMPXVideoStreaming;
    details->mClipName = KFILEPATH;

    mWidget->updateWithFileDetails( details );

    verifyResult( "Size", false );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // If it is local, we show size information
    //
    details->mPlaybackMode = EMPXVideoLocal;
    mWidget->updateWithFileDetails( details );

    verifyResult( "Size", true, "", false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testResolution
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testResolution()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testResolution()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // If it is audio only clip, don't need to show resolution
    //
    details->mVideoEnabled = false;

    mWidget->updateWithFileDetails( details );

    verifyResult( "Resolution", false );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // If it has video track
    //
    details->mVideoEnabled = true;
    details->mVideoWidth = 640;
    details->mVideoHeight = 360;

    mWidget->updateWithFileDetails( details );

    QString resolution = QString("%1x%2")
            .arg( details->mVideoWidth ).arg( details->mVideoHeight );

    verifyResult( "Resolution", true, resolution );

    cleanup();
}

// ---------------------------------------------------------------------------
// testFormat
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testFormat()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testFormat()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mMimeType = "Nothing";

    mWidget->updateWithFileDetails( details );

    verifyResult( "Format", true, details->mMimeType );

    cleanup();
}

// ---------------------------------------------------------------------------
// testBitrate
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testBitrate()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testBitrate()"));


    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mBitRate = 512;

    mWidget->updateWithFileDetails( details );

    verifyResult( "Bitrate", true, "512 kbps" );

    cleanup();
}

// ---------------------------------------------------------------------------
// testFolder
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testFolder()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testFolder()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // If it is streaming case, we don't show folder information
    //
    details->mPlaybackMode = EMPXVideoStreaming;
    details->mClipName = KFILEPATH;

    mWidget->updateWithFileDetails( details );

    verifyResult( "Collection", false );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // If it is local case, we show folder information
    //
    details->mPlaybackMode = EMPXVideoLocal;

    mWidget->updateWithFileDetails( details );

    verifyResult( "Collection", false );

    cleanup();

}

// ---------------------------------------------------------------------------
// verifyResult
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::verifyResult(
        QString primaryText, bool exist, QString expectedSecondaryText, bool needToBeCompared )
{
    MPX_DEBUG(_L("TestFileDetailsWidget::verifyResult(%s, %d, %s, %d)"),
            primaryText.data(), exist, expectedSecondaryText.data(), needToBeCompared );

    QString secondaryText = "";
    int i = 0;

    for( ; i < mWidget->mListWidget->count() ; i++ )
    {
        HbListWidgetItem *item = mWidget->mListWidget->item( i );

        if ( item->text() == primaryText )
        {
            secondaryText = item->secondaryText();
            break;
        }
    }

    //
    // If the item needs to be existed in HbListWidget, check the result
    //
    if ( exist )
    {
        if ( needToBeCompared )
        {
            MPX_DEBUG(_L("TestFileDetailsWidget::verifyResult() : secondaryText(%s)"),
                    secondaryText.data() );

            QVERIFY( expectedSecondaryText == secondaryText );
        }
        else
        {
            MPX_DEBUG(_L("TestFileDetailsWidget::verifyResult() : i(%d) count(%d)"),
                    i, mWidget->mListWidget->count() );

            //
            // Doens't need to compare the secondary text. Just make sure it's in the list
            //
            QVERIFY( i < mWidget->mListWidget->count() );
        }

    }
    //
    // If not, just make sure that item is not in the list
    //
    else
    {
        QVERIFY( i == mWidget->mListWidget->count() );
    }
}


// -------------------------------------------------------------------------------------------------
// dateTimeStringFormat
// -------------------------------------------------------------------------------------------------
//
QString TestFileDetailsWidget::dateTimeStringFormat( QDateTime dateTime )
{
    HbExtendedLocale locale = HbExtendedLocale::system();
    QString date = locale.format( dateTime.date(), r_qtn_date_usual );
    QString time = locale.format( dateTime.time(), r_qtn_time_long_with_zero );
    QString dateTimeString( date + "  " + time );

    MPX_DEBUG(_L("TestFileDetailsWidget::dateTimeStringFormat() ret '%s'"),
            dateTimeString.data() );

    return dateTimeString;
}

// ---------------------------------------------------------------------------
// testFilename
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testFilename()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testFilename()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();

    details->mPlaybackMode = EMPXVideoLocal;
    details->mClipName = KFILEPATH;
    
    mController->mViewMode = EDetailsView;

    mWidget->updateWithFileDetails( details );

    verifyResult( "File name", true,  "sample1" );

    cleanup();
}


// ---------------------------------------------------------------------------
// testFilePath
// ---------------------------------------------------------------------------
//
void TestFileDetailsWidget::testFilePath()
{
    MPX_ENTER_EXIT(_L("TestFileDetailsWidget::testFilePath()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();

    details->mPlaybackMode = EMPXVideoLocal;
    details->mClipName = KFILEPATH;
    
    mController->mViewMode = EDetailsView;

    mWidget->updateWithFileDetails( details );

    verifyResult( "File path", true,  "C:/sample1.wav" );

    cleanup();
    
}

// End of file
