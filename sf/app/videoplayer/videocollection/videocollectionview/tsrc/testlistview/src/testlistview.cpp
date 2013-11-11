/**
* Copyright	(c)	2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials	are	made available
* under	the	terms of "Eclipse Public License v1.0"
* which	accompanies	this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia	Corporation	- initial contribution.
*
* Contributors:
*
* Description:	 tester	for	methods	in VideoListView
*
*/

// Version : %version: 60 %

#define private public
#include "videoservices.h"
#include "videoproxymodelgeneric.h"
#include "videolistwidget.h"
#include "videolistview.h"
#undef private

#include <qdebug.h>
#include <xqserviceutil.h>
#include <QtTest/QtTest>
#include "hbapplication.h"
#include "hbeffect.h"
#include "hbinstance.h"
#include "hbmenu.h"
#include "hbmenudata.h"
#include "hbtoolbar.h"
#include "hbaction.h"
#include "hbstackedwidget.h"
#include "hbgroupbox.h"
#include "hbdocumentloader.h"
#include "hbview.h"
#include "hblistview.h"
#include "hbinputdialog.h"
#include "hbtoolbarextension.h"
#include "hblabel.h"
#include <qactiongroup.h>
#include <videocollectioncommon.h>
#include <qhash.h>
#include <qmap.h>
#include <mpxitemid.h>
#include <vcxmyvideosdefs.h>

#include "videocollectionuiloader.h"
#include "videocollectionwrapper.h"
#include "testlistview.h"
#include "videocollectionviewutils.h"
#include "videolistselectiondialog.h"
#include "videocollectionwrapper.h"
#include "videolistwidgetdata.h"
#include "videocollectionwrapperdata.h"
#include "videocollectionviewutilsdata.h"
#include "videolistselectiondialogdata.h"
#include "videoproxymodeldata.h"
#include "videocollectionuiloaderdata.h"
#include "videolistdatamodeldata.h"
#include "videolisttoolbardata.h"
#include "xqserviceutilxtra.h"
#include "videooperatorservicedata.h"
#include "videooperatorservice.h"

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int	main(int argc, char	*argv[])
{
	HbApplication app(argc,	argv);
	HbMainWindow window;
	hbInstance->mWindowses.append(&window);

	TestListView tv;

    int res;
    if(argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
    	char *pass[3];
    	pass[0]	= argv[0];
    	pass[1]	= "-o";
    	pass[2]	= "c:\\data\\testlistview.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

	return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestListView::init(bool initTestView)
{
    XQServiceUtilXtra::service = false;
	mUiLoader = new VideoCollectionUiLoader();
	mUiLoader->reset();

	bool ok(false);
	mUiLoader->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    if (!ok)
    {
    	return;
    }

    mTestView = mUiLoader->findWidget<VideoListView>( DOCML_NAME_VIEW );
    QVERIFY(mTestView);
    if(initTestView)
    {
        VideoOperatorServiceData::mIcons.clear();
        VideoOperatorServiceData::mUris.clear();
        VideoOperatorServiceData::mIcons.append("qtg_mono_ovistore");
        VideoOperatorServiceData::mUris.append("testuri");
        QVERIFY(mTestView->initializeView() == 0);
    }
    
    mListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY(mListWidget);

	mWrapper = &VideoCollectionWrapper::instance();
	QVERIFY(mWrapper);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestListView::cleanup()
{
	delete mUiLoader;
	mUiLoader = 0;
	
	mWrapper = 0;
	mTestView = 0;
	disconnect();

    VideoListWidgetData::reset();
    VideoCollectionWrapperData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoListSelectionDialogData::reset();
    VideoProxyModelData::reset();
    HbDocumentLoader::cleanup();
}

// ---------------------------------------------------------------------------
// visibleMenuActions
// ---------------------------------------------------------------------------
//
int	TestListView::visibleMenuActions()
{
	int	visibleActions(0);
	VideoCollectionUiLoader::ActionIds key;
	HbAction *action(0);
	foreach (key,VideoCollectionUiLoaderData::mMenuActions.keys())
	{
		action = dynamic_cast<HbAction*>(VideoCollectionUiLoaderData::mMenuActions[key]);
		if (action && action->isVisible())
		{
            visibleActions++;
		}
	}
	return visibleActions;
}

// ---------------------------------------------------------------------------
// setRowCount
// ---------------------------------------------------------------------------
//
void TestListView::setRowCount(int count, VideoProxyModelGeneric *model)
{
    if(!model)
    {
        if (mTestView)
        {
            if (!mTestView->mCurrentList)
            {
                TMPXItemId tmpId = TMPXItemId::InvalidId();
                mTestView->activateView(tmpId);
            }
            model = mTestView->mCurrentList->mModel;
        }
    }

    if (model)
    {
        VideoListDataModel *sourceModel =
            qobject_cast<VideoListDataModel*>(
                model->sourceModel());
        if(sourceModel)
        {
            VideoListDataModelData::setRowCount(*sourceModel, count);
        }
    }
}

// ---------------------------------------------------------------------------
// isActionVisible
// ---------------------------------------------------------------------------
//
bool TestListView::isActionVisible(const char *name) const
{
    bool isVisible = false;
    
    HbAction *action = mUiLoader->findObject<HbAction>(name);
    if (!action)
    {
        // must be menu widget
        HbMenu *menu = mUiLoader->findWidget<HbMenu>(name);
        if (menu)
        {
            action = menu->menuAction();
        }
    }
    
    // check if action is visible
    if (action)
    {
        isVisible = action->isVisible();
    }
    
    return isVisible;
}

// ---------------------------------------------------------------------------
// testCreateDelete
// ---------------------------------------------------------------------------
//
void TestListView::testCreateDelete()
{
    mWrapper = 0;

    mUiLoader = new VideoCollectionUiLoader();

	mUiLoader->reset();

	bool ok(false);

	mUiLoader->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);

    if (!ok)
    {
    	return;
    }

    mTestView = mUiLoader->findWidget<VideoListView>( DOCML_NAME_VIEW );

    delete mTestView;
	mTestView =	0;
}

// ---------------------------------------------------------------------------
// Slot: test initialize view
// ---------------------------------------------------------------------------
//
void TestListView::testInitializeView()
{
    VideoListWidget *videoListWidget = 0;
    
    init(false);
	// Test mUiLoader is null
    VideoCollectionUiLoader *tmp = mTestView->mUiLoader;
	mTestView->mUiLoader = 0;
	QVERIFY( mTestView->initializeView() < 0 );	
	QVERIFY( mTestView->mVideoServices == 0 );
	mTestView->mUiLoader = tmp;
	
	// service flag is false and mVideoServices is != 0
	VideoCollectionViewUtilsData::mIsServiceValue = false;
	VideoCollectionViewUtils::instance().setIsService();
	VideoServices *tmpService = VideoServices::instance(); 
	mTestView->mVideoServices = tmpService;
	QVERIFY( mTestView->initializeView() == 0 );    
    QVERIFY( mTestView->mVideoServices == 0 );
    tmpService->decreaseReferenceCount();
    
    // service flag is true, mVideoServices is 0
    mTestView->mVideoServices = 0;
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();   
    QVERIFY( mTestView->initializeView() == 0 );    
    QVERIFY( mTestView->mVideoServices != 0 );
    VideoCollectionViewUtilsData::mIsServiceValue = false;
    VideoCollectionViewUtils::instance().setIsService();
    
	cleanup();	
}

// ---------------------------------------------------------------------------
// Slot: test createToolbar
// ---------------------------------------------------------------------------
//
void TestListView::testCreateToolbar()
{
    init();
    // TODO
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test activate view
// ---------------------------------------------------------------------------
//
void TestListView::testActivateView()
{
    VideoListWidget* videoListWidget = 0;
    TMPXItemId invalidId = TMPXItemId::InvalidId();
	init();
	
	HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
	mainWnd->unsetOrientation(false);
    VideoListWidget *backup = mTestView->mCurrentList;
	mTestView->mCurrentList = 0;
	HbDocumentLoader::mFindWidgetFails = true;
	QCOMPARE( mTestView->activateView(invalidId), -1	);
    QVERIFY( VideoListWidgetData::mActive == false );
    QVERIFY( mTestView->mCurrentList == 0 );
    QCOMPARE( VideoListWidgetData::mActivateCount, 0 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 0 );
	QVERIFY( mainWnd->mOrientationSet == false );
	mTestView->mCurrentList = backup;
	
    HbDocumentLoader::mFindWidgetFails = false;
    videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY(videoListWidget);
	VideoListWidgetData::mActivateReturnValue =	-1;
	QCOMPARE( mTestView->activateView(invalidId), -1 );
	QVERIFY( VideoListWidgetData::mActive == false );
	QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
    QVERIFY( mainWnd->mOrientationSet == false );

	VideoListWidgetData::mActivateReturnValue =	0;
	QCOMPARE( mTestView->activateView(invalidId), 0 );
	QVERIFY( VideoListWidgetData::mActive );
	QCOMPARE( VideoListWidgetData::mActivateCount, 2 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
    QVERIFY( mainWnd->mOrientationSet );
    QCOMPARE( mainWnd->mOrientation, Qt::Vertical );

	QCOMPARE( mTestView->activateView(invalidId), 0 );
	QVERIFY( VideoListWidgetData::mActive );
	QCOMPARE( VideoListWidgetData::mActivateCount, 3 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
    QVERIFY( mainWnd->mOrientationSet );
    QCOMPARE( mainWnd->mOrientation, Qt::Vertical );
    
    cleanup();

    // activate collection content view:
    // -captured category
    TMPXItemId itemId = TMPXItemId::InvalidId();
    init();
    VideoListWidgetData::reset();
    VideoProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_captured"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoProxyModelData::mLastItemId, itemId);
    cleanup();
    
    // activate collection content view::
    // -downloaded category
    init();
    VideoListWidgetData::reset();
    VideoProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdDownloads;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_downloaded"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoProxyModelData::mLastItemId, itemId);
    cleanup();

    // activate collection content view:
    // -other category
    init();
    videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    VideoListWidgetData::reset();
    VideoProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdOther;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->activateView(itemId), -1);
    QVERIFY(VideoListWidgetData::mActive == false);
    QCOMPARE(VideoListWidgetData::mActivateCount, 0);
    QCOMPARE(mTestView->mCollectionName.length(), 0);
    // for not activated widget, defaultlevel is ELevelVideos 
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelVideos);
    QCOMPARE(VideoProxyModelData::mLastItemId, TMPXItemId::InvalidId());
    cleanup();

    // activate collection content view:
    // -invalid category but item id is ok
    init();
    VideoListWidgetData::reset();
    VideoProxyModelData::reset();
    itemId.iId1 = 0;
    itemId.iId2 = KVcxMvcMediaTypeVideo;
    QCOMPARE(mTestView->activateView(itemId), -1);
    QVERIFY(VideoListWidgetData::mActive == false);
    QCOMPARE(VideoListWidgetData::mActivateCount, 0);
    QCOMPARE(mTestView->mCollectionName.length(), 0);
    // for not activated widget, defaultlevel is ELevelVideos 
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelVideos);
    QCOMPARE(VideoProxyModelData::mLastItemId, TMPXItemId::InvalidId());
    cleanup();

    // activate view:
    // -browse service
    // -captured category
    // -sort role 0 (default, date & time)
    init(false);
    XQServiceUtilXtra *serviceUtilXtra = XQServiceUtilXtra::instance();
    serviceUtilXtra->setCurrentService(true);
    VideoServices *videoServices = VideoServices::instance(0);
    videoServices->mCurrentService = VideoServices::EBrowse;
    videoServices->mSortRole = 0;
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    VideoListWidgetData::reset();
    VideoProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->initializeView(), 0);
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_captured"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoProxyModelData::mLastItemId, itemId);
    QCOMPARE(VideoProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(VideoProxyModelData::mSortOrder, Qt::AscendingOrder);
    serviceUtilXtra->decreaseReferenceCount();
    videoServices->decreaseReferenceCount();
    cleanup();

    // activate view:
    // -browse service
    // -captured category
    // -sort role 1 (date & time)
    init(false);
    serviceUtilXtra = XQServiceUtilXtra::instance();
    serviceUtilXtra->setCurrentService(true);
    videoServices = VideoServices::instance(0);
    videoServices->mCurrentService = VideoServices::EBrowse;
    videoServices->mSortRole = 1;
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    VideoListWidgetData::reset();
    VideoProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->initializeView(), 0);
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_captured"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoProxyModelData::mLastItemId, itemId);
    QCOMPARE(VideoProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(VideoProxyModelData::mSortOrder, Qt::AscendingOrder);
    cleanup();
    serviceUtilXtra->decreaseReferenceCount();
    videoServices->decreaseReferenceCount();

    // activate view:
    // -browse service
    // -captured category
    // -sort role 2 (title)
    init(false);
    serviceUtilXtra = XQServiceUtilXtra::instance();
    serviceUtilXtra->setCurrentService(true);
    videoServices = VideoServices::instance(0);
    videoServices->mCurrentService = VideoServices::EBrowse;
    videoServices->mSortRole = 2;
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    VideoListWidgetData::reset();
    VideoProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->initializeView(), 0);
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_captured"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoProxyModelData::mLastItemId, itemId);
    QCOMPARE(VideoProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(VideoProxyModelData::mSortOrder, Qt::AscendingOrder);
    cleanup();
    serviceUtilXtra->decreaseReferenceCount();
    videoServices->decreaseReferenceCount();

    // activate view:
    // -browse service
    // -captured category
    // -sort role 3 (size) 
    init(false);
    serviceUtilXtra = XQServiceUtilXtra::instance();
    serviceUtilXtra->setCurrentService(true);
    videoServices = VideoServices::instance(0);
    videoServices->mCurrentService = VideoServices::EBrowse;
    videoServices->mSortRole = 3;
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    VideoListWidgetData::reset();
    VideoProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->initializeView(), 0);
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_captured"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoProxyModelData::mLastItemId, itemId);
    QCOMPARE(VideoProxyModelData::mSortRole, (int)VideoCollectionCommon::KeySizeValue);
    QCOMPARE(VideoProxyModelData::mSortOrder, Qt::AscendingOrder);
    cleanup();
    serviceUtilXtra->decreaseReferenceCount();
    videoServices->decreaseReferenceCount();
    VideoCollectionViewUtilsData::mIsServiceValue = false;
    VideoCollectionViewUtils::instance().setIsService();
}

// ---------------------------------------------------------------------------
// Slot: test deactivate view
// ---------------------------------------------------------------------------
//
void TestListView::testDeactivateView()
{
    VideoListWidget* videoListWidget = 0;
    TMPXItemId invalidId = TMPXItemId::InvalidId();
    init();

    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

    videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY(videoListWidget);
    mTestView->mCurrentList = 0;
    mainWnd->mOrientationSet = true;

	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 0 );
    QVERIFY( mainWnd->mOrientationSet == false );
    
    mTestView->mCurrentList = videoListWidget;
    mainWnd->mOrientationSet = true;
    
	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
    QVERIFY( mainWnd->mOrientationSet == false );

	VideoListWidgetData::mActivateReturnValue =	0;
    mainWnd->mOrientationSet = true;
	mTestView->activateView(invalidId);
	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 2 );
    QVERIFY( mainWnd->mOrientationSet == false );

	cleanup();
}

// ---------------------------------------------------------------------------
// testBack
// ---------------------------------------------------------------------------
//
void TestListView::testBack()
{
	init();

	mTestView->back();

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test open all videos slot
// ---------------------------------------------------------------------------
//
void TestListView::testOpenAllVideosViewSlot()
{
    VideoListWidget *videoListWidget = 0;
    HbGroupBox *subLabel = 0;
    TMPXItemId invalidId = TMPXItemId::InvalidId();
	init();
	mTestView->activateView(invalidId);
	
    videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY( videoListWidget );
    
    connect(this, SIGNAL(testSignal()), mTestView, SLOT(openAllVideosViewSlot()));
    
    emit testSignal();
	QCOMPARE( mTestView->mCurrentList, videoListWidget );

	mTestView->mCurrentList = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);
	QVERIFY( mTestView->mCurrentList );
    VideoListWidgetData::mActivateCount = 0;
	videoListWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    
    emit testSignal();
    QCOMPARE( mTestView->mCurrentList, videoListWidget );
    QCOMPARE( mTestView->mCurrentList->mCurrentLevel, VideoCollectionCommon::ELevelVideos );
    QVERIFY( VideoListWidgetData::mActive == true );
	QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
	
	disconnect(this, SIGNAL(testSignal()));
	
	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test open collections view	slot
// ---------------------------------------------------------------------------
//
void TestListView::testOpenCollectionViewSlot()
{
    VideoListWidget *collectionWidget = 0;
    HbGroupBox *subLabel = 0;
    TMPXItemId invalidId = TMPXItemId::InvalidId();
    init();
    mTestView->activateView(invalidId);

    connect(this, SIGNAL(testSignal()), mTestView, SLOT(openCollectionViewSlot()));

	collectionWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);
    QVERIFY(collectionWidget);

    emit testSignal();
    QCOMPARE(mTestView->mCurrentList, collectionWidget);

    mTestView->mCurrentList = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY( mTestView->mCurrentList );
    VideoListWidgetData::mActivateCount = 0;
    VideoListWidgetData::mDeactivateCount = 0;
    collectionWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    
    emit testSignal();
    QCOMPARE(mTestView->mCurrentList, collectionWidget);
    QCOMPARE( mTestView->mCurrentList->mCurrentLevel, VideoCollectionCommon::ELevelCategory );
    QVERIFY( VideoListWidgetData::mActive == true );
    QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
    QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test orientation related slots
// ---------------------------------------------------------------------------
//
void TestListView::testOrientationSlots()
{
	init();

    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
	mainWnd->addView(mTestView);
	mainWnd->setOrientation(Qt::Horizontal);
	mainWnd->setOrientation(Qt::Vertical);
	
	cleanup();
}

// ---------------------------------------------------------------------------
// testModelReadySlot
// ---------------------------------------------------------------------------
//
void TestListView::testModelReadySlot()
{
    init();
    mTestView->mModelReady = false;
    mTestView->mViewReady = false;
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(modelReadySlot()) );
    QSignalSpy spy(mTestView, SIGNAL(viewReady()));
    
    // test where level is ELevelCategory
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelCategory);
    emit testSignal();
    QVERIFY(mTestView->mModelReady == false);
    QVERIFY(mTestView->mViewReady == false);
    QCOMPARE(spy.count(), 0);
    spy.clear();
    
    // test where level is ELevelVideos
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelVideos);
    emit testSignal();
    QVERIFY(mTestView->mModelReady);
    QVERIFY(mTestView->mViewReady);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    
    // test that view ready is not emitted second time.
    emit testSignal();
    QCOMPARE(spy.count(), 0);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testAlbumListReadySlot
// ---------------------------------------------------------------------------
//
void TestListView::testAlbumListReadySlot()
{
    init();
    mTestView->mModelReady = false;
    mTestView->mViewReady = false;
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(albumListReadySlot()) );
    QSignalSpy spy(mTestView, SIGNAL(viewReady()));
    
    // test where level is ELevelVideos
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelVideos);
    emit testSignal();
    QVERIFY(mTestView->mModelReady == false);
    QVERIFY(mTestView->mViewReady == false);
    QCOMPARE(spy.count(), 0);
    spy.clear();
    
    // test where level is ELevelCategory
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelCategory);
    emit testSignal();
    QVERIFY(mTestView->mModelReady);
    QVERIFY(mTestView->mViewReady);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    
    // test that view ready is not emitted second time.
    emit testSignal();
    QCOMPARE(spy.count(), 0);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testHandleErrorSlot
// ---------------------------------------------------------------------------
//
void TestListView::testHandleStatusSlot()
{
	QVariant *variant =	new	QVariant();

	// Unknown error code.
	init();
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(0, *variant);
	QVERIFY(VideoCollectionViewUtilsData::mLastError ==	0);
	cleanup();

	// Multiple	deletion error.
	init();
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(VideoCollectionCommon::statusMultipleDeleteFail, *variant);
	QVERIFY(VideoCollectionViewUtilsData::mLastError ==	VideoCollectionCommon::statusMultipleDeleteFail);
	cleanup();

	// Deletion	error.
	init();
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(VideoCollectionCommon::statusSingleDeleteFail, *variant);
	QVERIFY(VideoCollectionViewUtilsData::mLastError ==	VideoCollectionCommon::statusSingleDeleteFail);
	cleanup();

	// status multidelete succeed
    init();
    connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
    emit testStatusSignal(VideoCollectionCommon::statusDeleteSucceed, *variant);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusDeleteSucceed);
    cleanup();

	// No model.
	init(false);
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(1,	*variant);
	QVERIFY(VideoCollectionViewUtilsData::mLastError ==	1);
	cleanup();

	delete variant;
}

// ---------------------------------------------------------------------------
// testCollectionOpenedSlot
// ---------------------------------------------------------------------------
//
void TestListView::testCollectionOpenedSlot()
{
	QString testString("");
	TMPXItemId itemId = TMPXItemId::InvalidId();
	int listWidgetActivateCount = 0;
	
	init(false);
	// collection content widget does not exist	
	VideoCollectionUiLoaderData::mFindFailure = true;
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&, const TMPXItemId &)),
	    mTestView, SLOT(collectionOpenedSlot(bool, const QString&,  const TMPXItemId &)));
	emit testCollectionOpenedSignal(true, testString, itemId);
	VideoCollectionUiLoaderData::mFindFailure = false;
	QVERIFY(!mTestView->mCurrentList);
	cleanup();
	
	init(true);
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&, const TMPXItemId &)),
	        mTestView, SLOT(collectionOpenedSlot(bool, const QString&,  const TMPXItemId &)));
	QVERIFY(mTestView->mCurrentList->getLevel() < VideoCollectionCommon::ELevelDefaultColl );
	VideoListWidget *listWidget = mTestView->mCurrentList;
	listWidgetActivateCount = VideoListWidgetData::mActivateCount;
	
	// id is invalid
	emit testCollectionOpenedSignal(true, testString, itemId);
	QVERIFY(mTestView->mCurrentList == listWidget);
	QVERIFY(mTestView->mCurrentList->getLevel() < VideoCollectionCommon::ELevelDefaultColl );
	QVERIFY(listWidgetActivateCount == VideoListWidgetData::mActivateCount);
	itemId = TMPXItemId(1,1);
	listWidget->activate();
	listWidget->getModel()->open(VideoCollectionCommon::ELevelDefaultColl);
	
	VideoListWidget *collectionContent = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
	
	// current list is collection content
	VideoListWidget *tmpList = mTestView->mCurrentList;
	mTestView->mCurrentList = collectionContent;	
	listWidgetActivateCount = VideoListWidgetData::mActivateCount;
	emit testCollectionOpenedSignal(true, testString, itemId);
	QVERIFY(listWidgetActivateCount == VideoListWidgetData::mActivateCount);
	mTestView->mCurrentList = tmpList;
	
	// collection list widget has no model
	VideoProxyModelGeneric *nullModel = 0;
	VideoProxyModelGeneric *tempModel = collectionContent->getModel();
	collectionContent->initialize(*nullModel, false, VideoCollectionCommon::ELevelDefaultColl);
	listWidget = mTestView->mCurrentList;
	listWidgetActivateCount = VideoListWidgetData::mActivateCount;
	emit testCollectionOpenedSignal(true, testString, itemId);
	QVERIFY(listWidgetActivateCount == VideoListWidgetData::mActivateCount);
	QVERIFY(mTestView->mCurrentList == listWidget);
	QVERIFY(mTestView->mCurrentList->getLevel() < VideoCollectionCommon::ELevelDefaultColl );	
	cleanup();
	init(true);
	itemId = TMPXItemId::InvalidId();
	VideoCollectionViewUtilsData::mWidgetLevel = VideoCollectionCommon::ELevelCategory;
	mTestView->activateView(itemId);
    connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&, const TMPXItemId &)),
            mTestView, SLOT(collectionOpenedSlot(bool, const QString&,  const TMPXItemId &)));
    
    collectionContent = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
	// Collection opened with default collection.
    VideoListToolbarData::reset();
    itemId = TMPXItemId(1,1);
    listWidgetActivateCount = VideoListWidgetData::mActivateCount;
    emit testCollectionOpenedSignal(true, testString, itemId);
    QVERIFY(VideoListWidgetData::mActivateCount == listWidgetActivateCount + 1);
    QVERIFY(mTestView->mCurrentList == collectionContent);
    QVERIFY(mTestView->mCurrentList->getLevel() == VideoCollectionCommon::ELevelDefaultColl );  
    QCOMPARE(VideoListToolbarData::mViewStateChangedCount, 1);
    QCOMPARE(VideoListToolbarData::mViewStateChangedLevel, VideoCollectionCommon::ELevelDefaultColl);
    
    // returning collection widget
    emit testCollectionOpenedSignal(false, testString, itemId);
    QVERIFY(mTestView->mCurrentList != collectionContent);
    QVERIFY(mTestView->mCurrentList->getLevel() == VideoCollectionCommon::ELevelCategory );  
    QCOMPARE(VideoListToolbarData::mViewStateChangedCount, 2);
    QCOMPARE(VideoListToolbarData::mViewStateChangedLevel, VideoCollectionCommon::ELevelCategory);
    
    // Collection opened with album.
    itemId = TMPXItemId(1,2);
    listWidgetActivateCount = VideoListWidgetData::mActivateCount;
    emit testCollectionOpenedSignal(true, testString, itemId);
    QVERIFY(VideoListWidgetData::mActivateCount == listWidgetActivateCount + 1);
    QVERIFY(mTestView->mCurrentList == collectionContent);
    QVERIFY(mTestView->mCurrentList->getLevel() == VideoCollectionCommon::ELevelAlbum );  
    QCOMPARE(VideoListToolbarData::mViewStateChangedCount, 3);
    QCOMPARE(VideoListToolbarData::mViewStateChangedLevel, VideoCollectionCommon::ELevelAlbum);
    
    // returning collection widget
    HbView::mSetItemVisibleLast = false;
    emit testCollectionOpenedSignal(false, testString, itemId);
    QVERIFY(mTestView->mCurrentList != collectionContent);
    QVERIFY(mTestView->mCurrentList->getLevel() == VideoCollectionCommon::ELevelCategory ); 
    QCOMPARE(VideoListToolbarData::mViewStateChangedCount, 4);
    QCOMPARE(VideoListToolbarData::mViewStateChangedLevel, VideoCollectionCommon::ELevelCategory);
    
    // service
    VideoServices *tmpService = VideoServices::instance(); 
    mTestView->mVideoServices = tmpService;
    itemId = TMPXItemId(1,1);
    listWidgetActivateCount = VideoListWidgetData::mActivateCount;
    emit testCollectionOpenedSignal(true, testString, itemId);
    QVERIFY(VideoListWidgetData::mActivateCount == listWidgetActivateCount + 1);
    QVERIFY(mTestView->mCurrentList == collectionContent);
    QVERIFY(mTestView->mCurrentList->getLevel() == VideoCollectionCommon::ELevelDefaultColl );  
    QCOMPARE(VideoListToolbarData::mViewStateChangedCount, 5);
    QCOMPARE(VideoListToolbarData::mViewStateChangedLevel, VideoCollectionCommon::ELevelDefaultColl);
    
    // returning collection widget
    emit testCollectionOpenedSignal(false, testString, itemId);
    QVERIFY(mTestView->mCurrentList != collectionContent);
    QVERIFY(mTestView->mCurrentList->getLevel() == VideoCollectionCommon::ELevelCategory ); 
    QCOMPARE(VideoListToolbarData::mViewStateChangedCount, 6);
    QCOMPARE(VideoListToolbarData::mViewStateChangedLevel, VideoCollectionCommon::ELevelCategory);

	cleanup();
}

// ---------------------------------------------------------------------------
// testUpdateSubLabel
// ---------------------------------------------------------------------------
//
void TestListView::testUpdateSubLabel()
{
	init(true);
    setRowCount(1);
    mTestView->mModelReady = true;
	connect(this, SIGNAL(testLayoutChangedSignal()), mTestView, SLOT(layoutChangedSlot()));

    VideoListWidget *videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);   
    VideoListWidget *collectionWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);   
    VideoListWidget *collectionContentWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);   
	
    videoListWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    collectionWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    collectionContentWidget->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
    
    HbGroupBox* label = mUiLoader->findWidget<HbGroupBox>(DOCML_NAME_VC_HEADINGBANNER);
    
	mTestView->mCurrentList = videoListWidget; 
	emit testLayoutChangedSignal();
	QCOMPARE( label->heading(), QString("txt_videos_subtitle_all_videos_l1") );

    mTestView->mCurrentList = collectionWidget; 
	emit testLayoutChangedSignal();
    QCOMPARE( label->heading(), QString("txt_videos_subtitle_collections_l1") );

    mTestView->mCurrentList = collectionContentWidget; 
	emit testLayoutChangedSignal();
    QCOMPARE( label->heading(), QString("txt_videos_subtitle_1_l2") );

    // how to test the collection name?
	
	cleanup();
}

// ---------------------------------------------------------------------------
// testShowHint
// ---------------------------------------------------------------------------
//
void TestListView::testShowHint()
{
    TMPXItemId invalidId = TMPXItemId::InvalidId();
    init(false);
    connect(this, SIGNAL(testLayoutChangedSignal()), mTestView, SLOT(layoutChangedSlot()));
    mTestView->mModelReady = true;
    
    // current list is null. (cannot be verified, run for coverity    
    emit testLayoutChangedSignal();   
    mTestView->activateView(invalidId);
    disconnect(this, SIGNAL(testLayoutChangedSignal()), mTestView, SLOT(layoutChangedSlot()));
    
    cleanup();
    init(true);
    mTestView->mModelReady = true;
    connect(this, SIGNAL(testLayoutChangedSignal()), mTestView, SLOT(layoutChangedSlot()));
    
    // no content label cannot be loaded. (cannot be tested, run for coverity)
    VideoCollectionUiLoaderData::mFindFailureNameList.append(DOCML_NAME_NO_CONTENT_LABEL);
    emit testLayoutChangedSignal();
    VideoCollectionUiLoaderData::mFindFailureNameList.clear();
    
    /////
    // no content label showing
    // model not ready, row count zero.
    HbLabel *noContentLabel =
        mUiLoader->findWidget<HbLabel>(
            DOCML_NAME_NO_CONTENT_LABEL);
    noContentLabel->setVisible(false);
    mTestView->mModelReady = false;
    setRowCount(0);
    VideoListWidget *backup = mTestView->mCurrentList;
    mTestView->mCurrentList =  mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);
    emit testLayoutChangedSignal();
    QVERIFY(noContentLabel->isVisible() == false);
    mTestView->mCurrentList = backup;
    
    // model not ready, row count not zero.
    setRowCount(1);
    noContentLabel->setVisible(false);
    emit testLayoutChangedSignal();    
    QVERIFY(noContentLabel->isVisible() == false );
    QVERIFY(mTestView->mModelReady);
    
    // model ready, row count not zero
    mTestView->mModelReady = true;
    noContentLabel->setVisible(false);
    emit testLayoutChangedSignal();    
    QVERIFY(noContentLabel->isVisible() == false );

    // model ready, row count is zero
    setRowCount(0);
    emit testLayoutChangedSignal();    
    QVERIFY(noContentLabel->isVisible() == true );
    
    // model ready, row count is zero, show to be false
    connect(this, SIGNAL(testSignal()), mTestView, SLOT(openCollectionViewSlot()));
    emit testSignal();
    QVERIFY(noContentLabel->isVisible() == false );
    disconnect(this, SIGNAL(testSignal()), mTestView, SLOT(openCollectionViewSlot()));
   
    mTestView->mCurrentList =  mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    
    ////////
    // toolbar setup
    mTestView->activateView(invalidId);
    mTestView->mModelReady = true;
    VideoListToolbarData::reset();

    // show -flag is true, currentlist level = VideoCollectionCommon::ELevelAlbum
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelAlbum);
    emit testLayoutChangedSignal();
    QCOMPARE(VideoListToolbarData::mViewStateChangedCount, 1);
    QCOMPARE(VideoListToolbarData::mViewStateChangedLevel, VideoCollectionCommon::ELevelAlbum);
    QCOMPARE(VideoListToolbarData::mViewStateChangedNoVideos, true);
    QCOMPARE(VideoListToolbarData::mViewStateChangedModelReady, true);
    
    // show -flag is true, currentlist level == VideoCollectionCommon::ELevelDefaultColl
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelDefaultColl);
    emit testLayoutChangedSignal();    
    QCOMPARE(VideoListToolbarData::mViewStateChangedCount, 2);
    QCOMPARE(VideoListToolbarData::mViewStateChangedLevel, VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoListToolbarData::mViewStateChangedNoVideos, true);
    QCOMPARE(VideoListToolbarData::mViewStateChangedModelReady, true);
       
    // show -flag is false
    setRowCount(1);
    emit testLayoutChangedSignal();   
    QCOMPARE(VideoListToolbarData::mViewStateChangedCount, 3);
    QCOMPARE(VideoListToolbarData::mViewStateChangedLevel, VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoListToolbarData::mViewStateChangedNoVideos, false);
    QCOMPARE(VideoListToolbarData::mViewStateChangedModelReady, true);
    
    ///////
    // sub label
    
    // sub label cannot be found (cannot be verified, run for coverity)
    VideoCollectionUiLoaderData::mFindFailureNameList.append(DOCML_NAME_VC_HEADINGBANNER);
    emit testLayoutChangedSignal();
    VideoCollectionUiLoaderData::mFindFailureNameList.clear();
    
    HbGroupBox *subLabel =
            mUiLoader->findWidget<HbGroupBox>(
                DOCML_NAME_VC_HEADINGBANNER);
    
    // show -flag is false
    setRowCount(1);
    emit testLayoutChangedSignal();  
    QVERIFY(subLabel->isVisible());
    
    // show -flag is true, current list's level != ELevelVideos
    setRowCount(0);
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelAlbum);
    emit testLayoutChangedSignal();  
    QVERIFY(subLabel->isVisible());
    
    // show -flag is true, current list's level == ELevelVideos
    setRowCount(0);
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelVideos);
    emit testLayoutChangedSignal();  
    QVERIFY(!subLabel->isVisible());
}

// ---------------------------------------------------------------------------
// testTitleReadySlot
// ---------------------------------------------------------------------------
//
void TestListView::testTitleReadySlot()
{
    init(true);
    
    QVERIFY(connect(this, SIGNAL(testSignal(const QString &)), mTestView, SLOT(titleReadySlot(const QString &))));
    
    QString title("TEST");
    
    emit testSignal(title);
    
    QVERIFY(mTestView->mTitle == title);
    
    disconnect(this, SIGNAL(testSignal(const QString &)), mTestView, SLOT(titleReadySlot(const QString &)));
    cleanup();
}

// ---------------------------------------------------------------------------
// testDoDelayedSlot
// ---------------------------------------------------------------------------
//
void TestListView::testDoDelayedsSlot()
{
    init(true);
    
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestView, SLOT(doDelayedsSlot())));
    
    emit testSignal();
    
    QCOMPARE(VideoCollectionUiLoaderData::mLastLoadPhasedData, (int)VideoCollectionUiLoaderParam::LoadPhaseSecondary); 
    
    disconnect(this, SIGNAL(testSignal()), mTestView, SLOT(doDelayedsSlot()));
    cleanup();
}

// ---------------------------------------------------------------------------
// testAboutToChangeOrientationSlot
// ---------------------------------------------------------------------------
//
void TestListView::testAboutToChangeOrientationSlot()
{
    init(false);
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToChangeOrientationSlot())));

    emit testSignal();
    
    // nothing to verify
    
    disconnect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToChangeOrientationSlot()));
    cleanup();
}

// ---------------------------------------------------------------------------
// testOrientationChangedSlot
// ---------------------------------------------------------------------------
//
void TestListView::testOrientationChangedSlot()
{
    init(false);
    QVERIFY(connect(this, SIGNAL(testSignal(Qt::Orientation)), mTestView, 
            SLOT(orientationChangedSlot(Qt::Orientation))));

    HbView::mSetItemVisibleLast = false;
    
    emit testSignal(Qt::Vertical);
    QVERIFY(HbView::mSetItemVisibleLast == true);

    emit testSignal(Qt::Horizontal);
    QVERIFY(HbView::mSetItemVisibleLast == false);
    
    disconnect(this, SIGNAL(testSignal(Qt::Orientation)), mTestView, 
            SLOT(orientationChangedSlot(Qt::Orientation)));    
    cleanup();
}

// ---------------------------------------------------------------------------
// testToolbarActionsChanged
// ---------------------------------------------------------------------------
//
void TestListView::testToolbarActionsChanged()
{
    init();
    TMPXItemId id = TMPXItemId::InvalidId();
    mTestView->activateView(id);
    
    connect(this, SIGNAL(testSignal(QList<QAction*>)), mTestView, SLOT(toolbarActionsChanged(QList<QAction*>)));
    
    QList<QAction*> actions;
    actions.append(new QAction(mTestView));
    actions.append(new QAction(mTestView));
    
    emit testSignal(actions);
    QCOMPARE(mTestView->toolBar()->actions().count(), 2);
    QVERIFY(HbView::mSetItemVisibleLast);
    QCOMPARE(mTestView->toolBar()->actions(), actions);
    
    emit testSignal(QList<QAction*>());
    QCOMPARE(mTestView->toolBar()->actions().count(), 0);
    QVERIFY(HbView::mSetItemVisibleLast == false);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testToolbarExtensionChanged
// ---------------------------------------------------------------------------
//
void TestListView::testToolbarExtensionChanged()
{
    init();
    TMPXItemId id = TMPXItemId::InvalidId();
    mTestView->activateView(id);
    
    connect(this, SIGNAL(testSignal(HbToolBarExtension*)), mTestView, SLOT(toolbarExtensionChanged(HbToolBarExtension*)));
    
    // should not crash, no other verification possible.
    emit testSignal(0);
    
    HbToolBarExtension* extension = new HbToolBarExtension(mTestView);
    emit testSignal(extension);
    QCOMPARE(mTestView->toolBar()->mAddExtensionCallCount, 1);
    
    cleanup();
}

// End of file
