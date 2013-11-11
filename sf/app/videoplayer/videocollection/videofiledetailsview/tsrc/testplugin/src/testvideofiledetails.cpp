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
* Description:   TestVideoFileDetails class implementation
* 
*/

// INCLUDE FILES
#include "hbmessagebox.h"

#include <QtTest/QtTest>
#include <qdebug.h>
#include <hbglobal.h>
#include <hbinstance.h>
#include <hbwidget.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbapplication.h>
#include <hbmarqueeitem.h>
#include <hbstackedwidget.h>
#include <hbparameterlengthlimiter.h>

#include "videodetailslabel.h"
#include "hbmessagebox.h"
#include "shareui.h"
#include "thumbnailmanager_qt.h"
#include "videocollectioncommon.h"
#include "mpxhbvideocommondefs.h"
#include "videocollectionwrapper.h"
#include "videoproxymodelgeneric.h"
#include "testvideofiledetails.h"

// trick to get access to protected/private members.
#define private public
#include "videofiledetailsviewplugin.h"
#undef private

const char *TEST_VIDEO_DETAILS_GFX_DEFAULT = "qtg_large_video";
const char *TEST_VIDEO_DETAILS_VIEW = "videofiledetailsview";
const char *TEST_VIDEO_DETAILS_WIDGET = "mContent";
const char *TEST_VIDEO_DETAILS_TITLE = "mLblTitle";
const char *TEST_VIDEO_DETAILS_LISTWIDGET ="mDetailsList";
const char* const VIDEO_DETAILS_THUMBNAIL = "mDetailsLabel";
const char* const VIDEO_DETAILS_BUTTON = "mButton";
const char *TEST_VIDEO_DETAILS_MENUACTION_DELETE = "mOptionsDelete";

// ---------------------------------------------------------------------------
// initTestCase
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::initTestCase()
{
   mDummyModel = 0;
   mDummyModel = new VideoProxyModelGeneric();
   
   connect(this, SIGNAL(shortDetailsReady(TMPXItemId)), mDummyModel, SIGNAL(shortDetailsReady(TMPXItemId)));
   connect(this, SIGNAL(fullDetailsReady(QVariant&)), mDummyModel, SIGNAL(fullVideoDetailsReady(QVariant&)));

   connect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), 
           mDummyModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
   
   connect(this, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
           mDummyModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)));
   
   VideoCollectionWrapper::instance().setModel(mDummyModel);
}

// ---------------------------------------------------------------------------
// cleanupTestCase
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::cleanupTestCase()
{
    disconnect(this, SIGNAL(shortDetailsReady(TMPXItemId)), mDummyModel, SIGNAL(shortDetailsReady(TMPXItemId)));
    disconnect(this, SIGNAL(fullDetailsReady(QVariant&)),mDummyModel, SIGNAL(fullVideoDetailsReady(QVariant&)));
    disconnect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), 
            mDummyModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
    disconnect(this, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            mDummyModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)));
    delete mDummyModel; mDummyModel = 0;
    delete mPlugin; mPlugin = 0;
}

// ---------------------------------------------------------------------------
// init before every test function
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::init()
{
    mDummyModel->reset();
    
    mPlugin = new VideoFileDetailsViewPlugin();
    mPlugin->createView();
    mCommandReceived = false;
    mReceivedCommand = -1;
    ThumbnailManager::mRequests.clear();
}

// ---------------------------------------------------------------------------
// cleanup after every test function
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::cleanup()
{
    if(mPlugin->getView()) {
        hbInstance->allMainWindows().at(0)->removeView(mPlugin->getView());
    }
    mDummyModel->reset();
    delete mPlugin; mPlugin = 0;
}

// ---------------------------------------------------------------------------
// Helper function that activates view after init()
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::activateView()
{
    hbInstance->allMainWindows().at(0)->addView(mPlugin->getView());
    mPlugin->activateView();

    // without this the xml loaded objects don't have the correct geometry.
    mPlugin->getView()->setGeometry(hbInstance->allMainWindows().at(0)->rect());
}

// ---------------------------------------------------------------------------
// Helper function that populates a qmap with dummy data.
// ---------------------------------------------------------------------------
//
QMap<QString, QVariant> TestVideoFileDetails::createDummyMetadata()
{
    using namespace VideoCollectionCommon;
    
    QMap<QString, QVariant> map;
    
    int detailCount = sizeof(VideoDetailLabelKeys) / sizeof(int);
    QString txt;
    for(int i = 0; i<detailCount; i++) {
        txt = QString(VideoDetailLabelKeys[i]);
        if(i%2 == 0)
        {            
            map[VideoDetailLabelKeys[i]] = txt;
        }
        else
        {
            txt.append(QString("test test test test test test test test test test test test test test test test test test test"));
            map[VideoDetailLabelKeys[i]] = txt;
        }
    }
    return map;
}

// ---------------------------------------------------------------------------
// One round for the activate view test
// ---------------------------------------------------------------------------
//
inline void TestVideoFileDetails::activateViewTestRound()
    {
    init();
    
    activateView();
    
    QVERIFY( mPlugin->getView() != 0 );
    QVERIFY( mPlugin->viewPlugin() == mPlugin );
    QCOMPARE( mPlugin->mVideoId, TMPXItemId::InvalidId() );
    
    // verify that actions are currently disabled.
    HbStackedWidget* thumbWidget = findWidget<HbStackedWidget>(VIDEO_DETAILS_THUMBNAIL);
    VideoDetailsLabel* thumbLabel = (VideoDetailsLabel*)thumbWidget->widgetAt(0);
    HbAction* deleteAction = findObject<HbAction>(TEST_VIDEO_DETAILS_MENUACTION_DELETE);
    
    HbPushButton* button = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON);
    
    QVERIFY( button != 0 );
    QVERIFY( thumbLabel != 0 );
    QVERIFY( deleteAction != 0 );
    QVERIFY( deleteAction->isEnabled() );
    
    QVERIFY(mPlugin->mActivated);    
     // second activation should not affect
    activateView();
    QVERIFY(mPlugin->mActivated);
    QVERIFY( button != 0 );
    QVERIFY( thumbLabel != 0 );
    QVERIFY( deleteAction != 0 );
    QVERIFY( deleteAction->isEnabled() );
    
    cleanup();
    }

// ---------------------------------------------------------------------------
// Slot: create view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::handleCommand(int command)
{
    mCommandReceived = true;
    mReceivedCommand = command;
}

// ---------------------------------------------------------------------------
// Slot: create view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testCreateView()
{
    init();
    // actual create view is done in the init.
    QVERIFY( mPlugin->mModel == mDummyModel );
    QVERIFY( mPlugin->mIsService == false);
    QVERIFY( mPlugin->mVideoServices == 0);
    // testing the special case where the model is null.
    cleanup();
    VideoCollectionWrapper::instance().setModel(0);
    init();
    QVERIFY( mPlugin->mModel == 0 );
    QVERIFY( mPlugin->mIsService == false);
    QVERIFY( mPlugin->mVideoServices == 0);
    
    // restoring the proper model.
    VideoCollectionWrapper::instance().setModel(mDummyModel);
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: destroy view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testDestroyView()
{
    init();
    activateView();
    mPlugin->destroyView();
    QVERIFY( mPlugin->getView() == 0 );
    // plugin is still alive
    QVERIFY( mPlugin->viewPlugin() == mPlugin );
    QVERIFY( mPlugin->mNavKeyBackAction == 0 );
    QVERIFY( mPlugin->mActivated == false );
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: activate view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testActivateViewPortrait()
{
    hbInstance->allMainWindows().at(0)->setOrientation(Qt::Vertical);
    activateViewTestRound();  
}

// ---------------------------------------------------------------------------
// Slot: activate view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testActivateViewLandscape()
{
    hbInstance->allMainWindows().at(0)->setOrientation(Qt::Horizontal);
    activateViewTestRound();
}

// ---------------------------------------------------------------------------
// Slot: activate view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testDeactivateView()
{
    init();
    activateView();
    mPlugin->deactivateView();
    QVERIFY( mPlugin->mActivated == false );
    QCOMPARE( mPlugin->mVideoId, TMPXItemId::InvalidId() );
    mPlugin->deactivateView();
    QVERIFY( mPlugin->mActivated == false );
    QCOMPARE( mPlugin->mVideoId, TMPXItemId::InvalidId() );
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: data for orientation change test
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testOrientationChange_data()
{
    QTest::addColumn<int>("start");
    QTest::addColumn<int>("switchTo");
    
    QTest::newRow("from portrait to landscape") 
            << static_cast<int>(Qt::Vertical) 
            << static_cast<int>(Qt::Horizontal);
    QTest::newRow("from landscape to portrait") 
            << static_cast<int>(Qt::Horizontal) 
            << static_cast<int>(Qt::Vertical);
}

// ---------------------------------------------------------------------------
// Slot: test for the orientationChange slot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testOrientationChange()
{
    QFETCH(int, start);
    QFETCH(int, switchTo);
    
    init();
    hbInstance->allMainWindows().at(0)->setOrientation(static_cast<Qt::Orientation>(start));
    activateView();
    
    hbInstance->allMainWindows().at(0)->setOrientation(static_cast<Qt::Orientation>(switchTo));
    
    // without this the view doesn't have the correct geometry.
    mPlugin->getView()->setGeometry(hbInstance->allMainWindows().at(0)->rect());
    // give fw some time to update content
    QTest::qWait(100);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the back slot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testBack()
{
    init();
    connect(this, SIGNAL(testSignal()), mPlugin, SLOT(back()));
    connect(mPlugin, SIGNAL(command(int)), this, SLOT(handleCommand(int)));
    
    emit testSignal();
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    mPlugin->mNavKeyBackAction->trigger();
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    activateView();
    
    emit testSignal();
    QCOMPARE( mCommandReceived, true );
    QCOMPARE( mReceivedCommand, static_cast<int>(MpxHbVideoCommon::ActivateCollectionView) );
    
    mCommandReceived = false;
    mReceivedCommand = -1;

    mPlugin->mNavKeyBackAction->trigger();
    QCOMPARE( mCommandReceived, true );
    QCOMPARE( mReceivedCommand, static_cast<int>(MpxHbVideoCommon::ActivateCollectionView) );
    
    mCommandReceived = false;
    mReceivedCommand = -1;
    
    mPlugin->deactivateView();
    
    emit testSignal();
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    mPlugin->mNavKeyBackAction->trigger();
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );

    disconnect(mPlugin, SIGNAL(command(int)), this, SLOT(handleCommand(int)));
    disconnect(this, SIGNAL(testSignal()), mPlugin, SLOT(back()));
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the shortDetailsReadySlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testShortDetailsReadySlot()
{
    TMPXItemId testIndex(5,0);
    QString title("title");
    QString filepath("filepath");
    
    init();
    activateView();
    
     // no data 
    emit shortDetailsReady(testIndex);
    
    QCOMPARE( mPlugin->mVideoId, testIndex );
    QVERIFY( mPlugin->mTitleAnim->text().isEmpty() );
    QCOMPARE( mPlugin->mThumbnailManager->mRequests.count(), 0 );
    
    mDummyModel->setData(VideoCollectionCommon::KeyTitle, title);
    mDummyModel->setData(VideoCollectionCommon::KeyFilePath, filepath);
    
    // no tn manager
    ThumbnailManager *tmpTnManager = mPlugin->mThumbnailManager;
    mPlugin->mThumbnailManager = 0;
    emit shortDetailsReady(testIndex);
    QCOMPARE( mPlugin->mVideoId, testIndex );
    QCOMPARE( mPlugin->mTitleAnim->text(), title );
    mPlugin->mThumbnailManager = tmpTnManager;
    
    // thumbnail fetching fails
    tmpTnManager->mGetThumbFails = true;
    emit shortDetailsReady(testIndex);
    QCOMPARE( mPlugin->mVideoId, testIndex );
    QCOMPARE( mPlugin->mTitleAnim->text(), title );
    QCOMPARE( mPlugin->mThumbLabel->icon().iconName(), QString(TEST_VIDEO_DETAILS_GFX_DEFAULT) );
    tmpTnManager->mGetThumbFails = false;
    
    // data exists
    emit shortDetailsReady(testIndex);
    
    QCOMPARE( mPlugin->mVideoId, testIndex );
    QCOMPARE( mDummyModel->lastId(), testIndex );
    QCOMPARE( mPlugin->mTitleAnim->text(), title );
    QVERIFY( ThumbnailManager::mRequests.contains(0) );
    ThumbnailManager::TnRequest request = ThumbnailManager::mRequests[0];
    QCOMPARE( request.name, filepath );
    // 20 == priorityHight in this case
    QCOMPARE( request.priority, 5000 );

    
    QVariant variant = QVariant(createDummyMetadata());
    emit fullDetailsReady(variant);
             
    emit shortDetailsReady(testIndex);
    
    int detailCount = sizeof(VideoCollectionCommon::VideoDetailLabelKeys) / sizeof(int);

    HbListWidget* list = findWidget<HbListWidget>(TEST_VIDEO_DETAILS_LISTWIDGET);
   
    QVERIFY(list);
    QVERIFY(list->count() == 0);

    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the fullDetailsReadySlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testFullDetailsReadySlot()
{
    TMPXItemId testIndex(6,0);
    init();
    activateView();
    
    int detailCount = sizeof(VideoCollectionCommon::VideoDetailLabelKeys) / sizeof(int);

    HbListWidget* list = findWidget<HbListWidget>(TEST_VIDEO_DETAILS_LISTWIDGET);
    QVERIFY(list);
    QVERIFY(list->count() == 0);
    
    QVariant variant = QVariant(createDummyMetadata());
       
    emit fullDetailsReady(variant);
    
    // verify that actions are currently enabled.
    HbStackedWidget* thumbWidget = findWidget<HbStackedWidget>(VIDEO_DETAILS_THUMBNAIL);
    VideoDetailsLabel* thumbLabel = (VideoDetailsLabel*)thumbWidget->widgetAt(0);
    HbAction* deleteAction = findObject<HbAction>(TEST_VIDEO_DETAILS_MENUACTION_DELETE);
    
    QVERIFY( thumbLabel != 0 );
    QVERIFY( deleteAction != 0 );
    QVERIFY( deleteAction->isEnabled() );
    
    QVERIFY(list->count() == detailCount);

    for(int i = 0; i<detailCount; i++) 
    {        
        QString expected = tr(VideoCollectionCommon::VideoDetailLabels[i]).arg(
                VideoCollectionCommon::VideoDetailLabelKeys[i]);
        HbListWidgetItem* detail = list->item(i);
        QVERIFY( detail != 0 );
        QVERIFY( detail->text().contains(expected) );
    }
    
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the testStartPlaybackSlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testStartPlaybackSlot()
{
    TMPXItemId testIndex(6,0);
    mDummyModel->reset();
    init();
    activateView();
    
    // Note that if the details view has not received signal in it's 
    // fullDetailsReadySlot, the button is disabled and should not do anything.
    
    HbStackedWidget* thumbWidget = findWidget<HbStackedWidget>(VIDEO_DETAILS_THUMBNAIL);
    VideoDetailsLabel* thumbLabel = (VideoDetailsLabel*)thumbWidget->widgetAt(0);
    QVERIFY( thumbLabel != 0 );
    thumbLabel->click();
    
    QCOMPARE( mDummyModel->startPlaybackIndex(), TMPXItemId::InvalidId() );
    
    QVariant variant = QVariant(createDummyMetadata());
    emit fullDetailsReady(variant);
    mPlugin->mVideoId = testIndex;
    
    thumbLabel->click();
    
    QCOMPARE( mDummyModel->startPlaybackIndex(), testIndex );
    
    // invalid index
    mPlugin->mVideoId = TMPXItemId::InvalidId();
    thumbLabel->click();
    // startplayback index has not changed since previous
    QCOMPARE( mDummyModel->startPlaybackIndex(), testIndex );
    
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the sendVideoSlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testSendVideoSlot()
{
    
    mDummyModel->reset();
    init();
    mPlugin->activateView();
    
    HbPushButton* shareAction = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON);
    
    QVERIFY( shareAction != 0 );
    shareAction->click();
    QCOMPARE( mDummyModel->dataAccessCount(), 0 );
    mPlugin->mVideoId = (0,0);
    
    mDummyModel->setDataReturnInvalid(true);
    QModelIndex expected = mDummyModel->index(0, 0);
    shareAction->click();
    QCOMPARE( mDummyModel->dataAccessCount(), 1 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( ShareUi::mFileList.count(), 0 );
    QVERIFY( ShareUi::mEmbedded == false );
    
    mDummyModel->setDataReturnInvalid(false);
    QString filePath("dummyfilepath");
    mDummyModel->setData(VideoCollectionCommon::KeyFilePath, filePath);
    shareAction->click();
    QCOMPARE( mDummyModel->dataAccessCount(), 2 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QVERIFY( ShareUi::mEmbedded );
    QCOMPARE( ShareUi::mFileList.count(), 1 );
    QCOMPARE( ShareUi::mFileList.at(0), filePath );
    
    mPlugin->deactivateView();
    
    QVERIFY(!mPlugin->mShareUi);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the deleteVideoSlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testDeleteVideoSlot()
{
    mDummyModel->reset();
    init();
    
    HbAction* deleteAction = findObject<HbAction>(TEST_VIDEO_DETAILS_MENUACTION_DELETE);
    
    QVERIFY( deleteAction != 0 ); 
    deleteAction->trigger();
    QCOMPARE( mDummyModel->dataAccessCount(), 0 );
    mPlugin->mVideoId = (0,0);

    mDummyModel->setDataReturnInvalid(true);
    QModelIndex expected = mDummyModel->index(0, 0);
    deleteAction->trigger();
    QCOMPARE( mDummyModel->dataAccessCount(), 1 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( mDummyModel->deleteFileIndex(), -1 ); // verify that no file was deleted.
    mDummyModel->setDataReturnInvalid(false);

    mPlugin->mVideoId = (0,0);
    expected = mDummyModel->index(0, 0);
    deleteAction->trigger();
    QCOMPARE( mDummyModel->dataAccessCount(), 2 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( mDummyModel->deleteFileIndex(), -1 ); // verify that no file was deleted.

    HbMessageBox *msgBox = new HbMessageBox();
    
    QStringList display;
    display.append("first row");
    display.append("second row");
    mDummyModel->setData(VideoCollectionCommon::KeyTitle, display);
    
    QString expectedText = hbTrId("txt_videos_info_do_you_want_to_delete_1").arg(
            display.first());
    deleteAction->trigger();
    msgBox->emitDialogFinished(mPlugin, SLOT(deleteVideoDialogFinished(int)), HbMessageBox::Yes); // Yes selected
    QCOMPARE( mDummyModel->dataAccessCount(), 3 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( mDummyModel->deleteFileIndex(), expected.row() );
    QCOMPARE( HbMessageBox::mLatestTxt, expectedText );
    
    deleteAction->trigger();
    msgBox->emitDialogFinished(mPlugin, SLOT(deleteVideoDialogFinished(int)), HbMessageBox::No); // No selected
    QCOMPARE( mDummyModel->dataAccessCount(), 4 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( mDummyModel->deleteFileIndex(), expected.row() );
    QCOMPARE( HbMessageBox::mLatestTxt, expectedText );
    

    mDummyModel->reset();
    deleteAction->trigger();
    msgBox->emitDialogFinished(mPlugin, SLOT(deleteVideoDialogFinished(int)), HbMessageBox::No); // No selected
    QCOMPARE( mDummyModel->dataAccessCount(), 1 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( mDummyModel->deleteFileIndex(), -1 ); // verify that no file was deleted.
    QCOMPARE( HbMessageBox::mLatestTxt, expectedText );
    
    delete msgBox;
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the testRowsRemovedSlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testRowsRemovedSlot()
{
    const int testIndex = 9;
    TMPXItemId testID(9,0);
    init();
    connect(mPlugin, SIGNAL(command(int)), this, SLOT(handleCommand(int)));
    
    emit rowsRemoved(QModelIndex(), testIndex, testIndex);
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    activateView();
    
    emit rowsRemoved(QModelIndex(), testIndex, testIndex);
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    QVariant variant = QVariant(createDummyMetadata());
    emit fullDetailsReady(variant);
    mPlugin->mVideoId = testID;
    mPlugin->mDeletedIndex = testIndex;
    
    emit rowsRemoved(QModelIndex(), testIndex+1, testIndex-1);
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );

    emit rowsRemoved(QModelIndex(), testIndex-3, testIndex-1);
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    emit rowsRemoved(QModelIndex(), testIndex+1, testIndex+3);
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    emit rowsRemoved(QModelIndex(), testIndex, testIndex);
    QCOMPARE( mCommandReceived, true );
    QCOMPARE( mReceivedCommand, static_cast<int>(MpxHbVideoCommon::ActivateCollectionView) );
    
    disconnect(mPlugin, SIGNAL(command(int)), this, SLOT(handleCommand(int)));
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: testHandleErrorSlot test error messages
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testHandleErrorSlot()
{
    init();
    connect(this, SIGNAL(testErrorSignal(int, QVariant&)), mPlugin, SLOT(handleErrorSlot(int, QVariant&)));

    HbMessageBox::mLatestTxt = "";
    QVariant additional =  QVariant();
    // invalid msg,
    emit testErrorSignal( VideoCollectionCommon::statusMultipleDeleteFail + 1, additional);
    
    QVERIFY(HbMessageBox::mLatestTxt.isEmpty());
     
    // VideoCollectionCommon::statusSingleDeleteFail
    // - without additional data
    emit testErrorSignal( VideoCollectionCommon::statusSingleDeleteFail , additional);
    QVERIFY(HbMessageBox::mLatestTxt.isEmpty());
    
    // - with additional data
    QString txt = "testdata";
    additional = txt;
    emit testErrorSignal( VideoCollectionCommon::statusSingleDeleteFail , additional);
    QString expected = HbParameterLengthLimiter(hbTrId("txt_videos_info_unable_to_delete_1_it_is_current")).arg(txt);
    QCOMPARE(HbMessageBox::mLatestTxt, expected);
    
    HbMessageBox::mLatestTxt = "";
    // VideoCollectionCommon::statusMultipleDeleteFail
    emit testErrorSignal( VideoCollectionCommon::statusMultipleDeleteFail , additional);
    QVERIFY(HbMessageBox::mLatestTxt.isEmpty());
    
    disconnect(this, SIGNAL(testErrorSignal(int, QVariant&)), mPlugin, SLOT(handleErrorSlot(int, QVariant&)));
    
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the testThumbnailReadySlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testThumbnailReadySlot()
{
    TMPXItemId testIndex(9,0);
    QStringList display;
    display.append("first row");
    display.append("second row");
    QString filepath("filepath");
    
    init();
    activateView();
    
    mDummyModel->setData(VideoCollectionCommon::KeyTitle, display);
    mDummyModel->setData(VideoCollectionCommon::KeyFilePath, filepath);

    emit shortDetailsReady(testIndex);
    
    mPlugin->mThumbnailManager->mThumbnailReadyError = -1;
    mPlugin->mThumbnailManager->emitThumbnailReady(0);
    
    HbStackedWidget* thumbWidget = findWidget<HbStackedWidget>(VIDEO_DETAILS_THUMBNAIL);
    VideoDetailsLabel* thumbLabel = (VideoDetailsLabel*)thumbWidget->widgetAt(0);
    QVERIFY( thumbLabel->icon().qicon().isNull() == false );

    mPlugin->mThumbnailManager->mThumbnailReadyError = 0;
    emit shortDetailsReady(testIndex);
    mPlugin->mThumbnailManager->emitThumbnailReady(0);
    
    QVERIFY( thumbLabel->icon().qicon().isNull() == false );
    
    cleanup();
}


// ---------------------------------------------------------------------------
// findWidget
// ---------------------------------------------------------------------------
//
template<class T> 
T* TestVideoFileDetails::findWidget(QString name)
{
    return qobject_cast<T *>(mPlugin->mLoader.findWidget(name));
}

// ---------------------------------------------------------------------------
// findObject
// ---------------------------------------------------------------------------
//
template<class T> 
T* TestVideoFileDetails::findObject(QString name)
{
    return qobject_cast<T *>(mPlugin->mLoader.findObject(name));
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbApplication::setKeypadNavigationEnabled(false);
    HbMainWindow mainWindow;
    mainWindow.show();
    
    TestVideoFileDetails tc;
    
    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideofiledetailsviewplugin.txt";

    return QTest::qExec(&tc, 3, pass);
}

// end of file
