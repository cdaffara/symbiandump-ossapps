/**
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
* Description:   tester for methods in VideoListWidget
*
*/

// Version : %version: 62 %

#include <qmap.h>
#include <vcxmyvideosdefs.h>
#include <mpxhbvideocommondefs.h>

#include "hbglobal.h"
#include "hblistview.h"
#include "hbview.h"
#include "videoproxymodelgeneric.h"
#include "hbscrollbar.h"
#include "hbmenu.h"
#include "hbmessagebox.h"
#include "hbinstance.h"
#include "hbmainwindow.h"
#include "hbstyleloader.h"
#include "hbinputdialog.h"
#include "videothumbnaildata.h"
#include "videocollectioncommon.h"
#include "videocollectionwrapper.h"
#include "videocollectionwrapperdata.h"
#include "hbmenudata.h"
#include "videothumbnailtestdata.h"
#include "videolistdatamodel.h"
#include "videolistdatamodeldata.h"
#include "videoproxymodeldata.h"
#include "videocollectionuiloader.h"
#include "videocollectionuiloaderdata.h"
#include "videolistselectiondialog.h"
#include "videolistselectiondialogdata.h"
#include "hbmessageboxdata.h"

#include "testlistwidget.h"

#define private public
#include "videolistwidget.h"
#include "videoservices.h"
#undef private

static const int CONTEXT_MENU_COUNT = 8;

/**
 * helper class to run protected member of the test object
 */
class ListWidgetTester : public VideoListWidget
{
public:
    /**
     * constructor
     */
    ListWidgetTester(VideoCollectionUiLoader* uiLoader, HbView *parent) :
            VideoListWidget(uiLoader, parent)
    {
        
    }
    
    /**
     * calls emitActivated
     */
    void callEmiteActivated(QModelIndex index)
    {
        VideoListWidget::emitActivated(index);
    }
    
    /**
     * calls longPressedSlot
     */
    void callLongPressedSlot(HbAbstractViewItem *item, const QPointF &point)
    {
        VideoListWidget::longPressedSlot(item, point);
    }
    
    void callRowsInserted(const QModelIndex &parent, int start, int end)
    {
        VideoListWidget::rowsInserted(parent, start, end);
    }

    
    void callRowsRemoved(const QModelIndex &parent, int start, int end)
    {
        VideoListWidget::rowsRemoved(parent, start, end);
    }    
};

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    TestListWidget tv;
    QApplication app(argc, argv);
    int res;
    if(argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\testlistwidget.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}

// ---------------------------------------------------------------------------
// setRowCount
// ---------------------------------------------------------------------------
//
void TestListWidget::setRowCount(int count,
    VideoProxyModelGeneric *proxyModel,
    VideoListDataModel *model)
{
    if (!model)
    {
        proxyModel = VideoCollectionWrapper::instance().getAllVideosModel();
        QVERIFY(proxyModel);
        
        VideoListDataModel *sourceModel = qobject_cast<VideoListDataModel*>(proxyModel->sourceModel());
        QVERIFY(sourceModel);
        
        VideoListDataModelData::setRowCount(*sourceModel, count);
        sourceModel->initialize();
    }
    else
    {
        VideoListDataModelData::setRowCount(*model, count);
        model->initialize();
    }
}

// ---------------------------------------------------------------------------
// initTestCase
// ---------------------------------------------------------------------------
//
void TestListWidget::initTestCase()
{
    qRegisterMetaType<QModelIndex>("QModelIndex");
    qRegisterMetaType<TMPXItemId>("TMPXItemId");
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestListWidget::init()
{
    mDummyMainWnd = new HbMainWindow;
    mTempView = new HbView;
    mTestUiLoader = new VideoCollectionUiLoader();
    hbInstance->mWindowses.append(mDummyMainWnd);
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestListWidget::cleanup()
{
    delete mTestWidget;
    mTestWidget = 0;
    
    delete mTempView;
    mTempView = 0;
    
    delete mDummyMainWnd;
    mDummyMainWnd = 0;
    
    hbInstance->mWindowses.clear();
    
    VideoCollectionWrapperData::reset();
    HbMenuData::reset();
    VideoThumbnailTestData::reset();
    VideoListDataModelData::reset();
    VideoProxyModelData::reset();
    HbMessageBoxData::reset();
}
 
// ---------------------------------------------------------------------------
// testInitialized
// ---------------------------------------------------------------------------
//
void TestListWidget::testInitialize()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    // succeed case ( new operator cannot be stubbed)
    QVERIFY(mTestWidget->initialize(*model) == 0);
	QVERIFY(mTestWidget->mModel == model);  
	QVERIFY(mTestWidget->mVideoServices == 0);
	
	// service initialization
	VideoServices *service = VideoServices::instance();
    QVERIFY(mTestWidget->initialize(*model, true) == 0);
    QVERIFY(mTestWidget->mModel == model);  
    QVERIFY(mTestWidget->mVideoServices == service);
    service->decreaseReferenceCount();
    service = 0;
	
}
 
// ---------------------------------------------------------------------------
// testActivate
// ---------------------------------------------------------------------------
//
void TestListWidget::testActivate()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    // no  model: fails
    QVERIFY(mTestWidget->activate() == -1);
    QCOMPARE(HbMenuData::mEnabledSetted, false);
    QCOMPARE(HbListView::mLatestVisibility, false);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 0);
    
    // model exist, no current view default level: fails
    QVERIFY(mTestWidget->initialize(*model) == 0);
    QVERIFY(mTestWidget->activate() == -1);
    QCOMPARE(HbMenuData::mEnabledSetted, false);
    QCOMPARE(HbListView::mLatestVisibility, false);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 0);
    
    HbMenuData::mEnabledSetted = true;
    HbListView::mLatestVisibility = false;
    HbListView::mLatestEnableValue = false;
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = 0;
    HbView *tmpView = new HbView();
    // model exists, current view exists, level neither ELevelAlbum nor ELevelDefaultColl 
    delete mTestWidget;
    mTestWidget = 0;
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
    model = wrapper.getAllVideosModel();
    
    QVERIFY(mTestWidget->initialize(*model, 0, VideoCollectionCommon::ELevelVideos) == 0);
    hbInstance->allMainWindows().value(0)->addView(tmpView);
    QVERIFY(mTestWidget->activate() == 0);
    QCOMPARE(HbMenuData::mEnabledSetted, true);
    QCOMPARE(HbListView::mLatestVisibility, true);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 1);
    QVERIFY(mTestWidget->mNavKeyAction);
    QVERIFY(HbAction::mNavAction == Hb::QuitAction);

    HbMenuData::mEnabledSetted = true;
    HbListView::mLatestVisibility = false;
    HbListView::mLatestEnableValue = false;
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = 0;
    
    // model exists, current view exists, level is ELevelAlbum
    delete mTestWidget;
    mTestWidget = 0;
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
    model = wrapper.getCollectionContentModel();
    tmpView->mNavigationAction = 0;
    QVERIFY(mTestWidget->initialize(*model) == 0);
    QVERIFY(mTestWidget->activate(VideoCollectionCommon::ELevelAlbum) == 0);
    QCOMPARE(HbMenuData::mEnabledSetted, true);
    QCOMPARE(HbListView::mLatestVisibility, true);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 1);
    QVERIFY(mTestWidget->mNavKeyAction);
    QVERIFY(HbAction::mNavAction == Hb::BackAction);
       
    HbMenuData::mEnabledSetted = true;
    HbListView::mLatestVisibility = false;
    HbListView::mLatestEnableValue = false;
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = 0;

    // activate:
    // -is service
    // -current service is browse
    VideoServices *videoServices = VideoServices::instance();    
    delete mTestWidget;
    mTestWidget = 0;
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
    videoServices->mCurrentService = VideoServices::EBrowse;
    QVERIFY(mTestWidget->initialize(*model, true) == 0);
    QVERIFY(mTestWidget->activate(VideoCollectionCommon::ELevelDefaultColl) == 0);
    QVERIFY(mTestWidget->mNavKeyAction);
    QVERIFY(HbAction::mNavAction == Hb::BackAction);    
    
    // activate:
    // -is service
    // -current service is fetch
    // -activating all videos
    delete mTestWidget;
    mTestWidget = 0;
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
    videoServices->mCurrentService = VideoServices::EUriFetcher;
    QVERIFY(mTestWidget->initialize(*model, true) == 0);
    QVERIFY(mTestWidget->activate(VideoCollectionCommon::ELevelVideos) == 0);
    QVERIFY(mTestWidget->mNavKeyAction);
    QVERIFY(HbAction::mNavAction == Hb::QuitAction);
    
    // activate:
    // -is service
    // -current service is fetch
    // -activating album
    delete mTestWidget;
    mTestWidget = 0;
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
    videoServices->mCurrentService = VideoServices::EUriFetcher;
    QVERIFY(mTestWidget->initialize(*model, true) == 0);
    QVERIFY(mTestWidget->activate(VideoCollectionCommon::ELevelAlbum) == 0);
    QVERIFY(mTestWidget->mNavKeyAction);
    QVERIFY(HbAction::mNavAction == Hb::BackAction);
    
    hbInstance->allMainWindows().value(0)->removeView(tmpView);
    delete tmpView;
    
    // final cleanup
    videoServices->mCurrentService = VideoServices::ENoService;
    videoServices->decreaseReferenceCount();
}
 
// ---------------------------------------------------------------------------
// testDeactivate
// ---------------------------------------------------------------------------
//
void TestListWidget::testDeactivate()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    HbListView::mLatestVisibility = true;
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = 1;
    
    mTestWidget->activate();
    
    // no context menu and activated without model
    mTestWidget->deactivate();
    QCOMPARE(HbListView::mLatestVisibility, false);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 0);
    
    HbListView::mLatestVisibility = true;
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = 1;
    
    // context menu exists and activated with model
    mTestWidget->mContextMenu = new HbMenu;
    QVERIFY(mTestWidget->initialize(*model) == 0);
    mTestWidget->activate();
    mTestWidget->deactivate();
    QCOMPARE(HbListView::mLatestVisibility, false);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 0);
   
    // deactivate:
    // -is service
    // -current service is browse
    VideoServices *videoServices = VideoServices::instance();
    videoServices->mCurrentService = VideoServices::EBrowse;
    delete mTestWidget;
    mTestWidget = 0;
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
    QVERIFY(mTestWidget->initialize(*model, videoServices) == 0);
    QVERIFY(mTestWidget->activate(VideoCollectionCommon::ELevelDefaultColl) == 0);
    mTestWidget->deactivate();
    QCOMPARE(HbListView::mLatestVisibility, false);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 0);

    // final cleanup
    videoServices->mCurrentService = VideoServices::ENoService;
    videoServices->decreaseReferenceCount();
}

// ---------------------------------------------------------------------------
// testGetLevel
// ---------------------------------------------------------------------------
//
void TestListWidget::testGetLevel()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    mTestWidget->initialize(*model);
    
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    QCOMPARE(mTestWidget->getLevel(), VideoCollectionCommon::ELevelVideos);
}
   
// ---------------------------------------------------------------------------
// testGetModel
// ---------------------------------------------------------------------------
//
void TestListWidget::testGetModel()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    QVERIFY(mTestWidget->getModel() == 0);
    
    HbListView::mVScrollBarIsNull = false;
    HbListView::mReturnNullPrototype = false;
    mTestWidget->initialize(*model);
    
    QVERIFY(mTestWidget->getModel() == model);
}
 
// ---------------------------------------------------------------------------
// testEmitActivated
// ---------------------------------------------------------------------------
//
void TestListWidget::testEmitActivated()
{
    VideoServices *videoServices = VideoServices::instance();
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    QSignalSpy spysignal(mTestWidget, SIGNAL(collectionOpened(bool, const QString&, const TMPXItemId&)));
    QSignalSpy spysignalFileUri(mTestWidget, SIGNAL(fileUri(const QString&)));
    QSignalSpy spysignalActivated(mTestWidget, SIGNAL(activated(const QModelIndex&)));
    
    mTestWidget->initialize(*model);
    mTestWidget->activate();
    
    VideoProxyModelData::mLastItemId = TMPXItemId::InvalidId();
    TMPXItemId savedId = TMPXItemId(1,1);
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(savedId);
    QVariant data = QString("test");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    setRowCount(2);
    VideoProxyModelData::mItemIds.append(TMPXItemId(0,0));
    
    QModelIndex fetchIndex = model->index(0, 0, QModelIndex());
    
    // selection mode == HbAbstractItemView::MultiSelection
    mTestWidget->setSelectionMode(HbAbstractItemView::MultiSelection);
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(spysignalFileUri.count() == 0);
    QVERIFY(spysignalActivated.count() == 1);
    QVERIFY(VideoProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    spysignal.clear();
    spysignalFileUri.clear();
    spysignalActivated.clear();
    
    // modelIndex is not valid
    mTestWidget->setSelectionMode(HbAbstractItemView::NoSelection);
    fetchIndex = QModelIndex();
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(spysignalFileUri.count() == 0);
    QVERIFY(spysignalActivated.count() == 0);
    QVERIFY(VideoProxyModelData::mLastItemId == TMPXItemId::InvalidId());
   
    // current level is ELevelCategory
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    // --> variant is not valid 
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, QVariant());
    fetchIndex = model->index(0, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(spysignalFileUri.count() == 0);
    QVERIFY(spysignalActivated.count() == 0);
    QVERIFY(VideoProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    

    // --> variant is valid, collectionOpened -signal should be emitted
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    fetchIndex = model->index(1, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignalFileUri.count() == 0);
    QVERIFY(spysignalActivated.count() == 0);
    QVERIFY(VideoProxyModelData::mLastItemId == TMPXItemId::InvalidId());
  
    spysignal.clear();
    spysignalFileUri.clear();
    spysignalActivated.clear();
    
    // current level is not ELevelCategory
    // mVideoServices exist, variant gotten is invalid
    mTestWidget->mService = VideoServices::EUriFetcher;
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, QVariant());
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    mTestWidget->mVideoServices = videoServices;
    fetchIndex = model->index(5, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(spysignalFileUri.count() == 0);
    QVERIFY(spysignalActivated.count() == 0);
    QVERIFY(VideoProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    
    // current level is not ELevelCategory
    // mVideoServices, variant gotten is valid
    mTestWidget->mService = VideoServices::EUriFetcher;
    VideoListDataModelData::setData(VideoCollectionCommon::KeyFilePath, data);
    fetchIndex = model->index(0, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(spysignalFileUri.count() == 1);
    QVERIFY(spysignalActivated.count() == 0);
    QVERIFY(VideoProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    
    spysignal.clear();
    spysignalFileUri.clear();
    spysignalActivated.clear();
    
    // current level is not ELevelCategory
    // mVideoServices does not exists
    mTestWidget->mVideoServices = 0;
    fetchIndex = model->index(0, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(spysignalFileUri.count() == 0);
    QVERIFY(spysignalActivated.count() == 0);
    QVERIFY(VideoProxyModelData::mLastItemId == savedId);
    
    // context menu is visible
    if (!mTestWidget->mContextMenu)
    {
        mTestWidget->mContextMenu = new HbMenu;
    }
    mTestWidget->mContextMenu->show();
    VideoProxyModelData::mLastItemId = TMPXItemId::InvalidId();
    fetchIndex = model->index(0, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(spysignalFileUri.count() == 0);
    QVERIFY(spysignalActivated.count() == 0);
    QVERIFY(VideoProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    
    // final cleanup
    videoServices->decreaseReferenceCount();
}

// ---------------------------------------------------------------------------
// testLongPressedSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testLongPressedSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    QVariant data = QString("test");
    // correct data to index 0
    VideoListDataModelData::setData(Qt::DisplayRole, data);
    setRowCount(2);
    
    QPointF point(1,1);
    mTestWidget->initialize(*model);
    
    HbAbstractViewItem *item = new HbAbstractViewItem();
    item->mModelIndex = mTestWidget->mModel->index(0, 0, QModelIndex());
    mTestWidget->setSelectionMode(HbAbstractItemView::MultiSelection);
    
    // mDetailsReady is false
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(HbMenuData::mExecPoint != point);
    
    // multiselection is on
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(HbMenuData::mExecPoint != point);
    
    // selection mode is custom
    mTestWidget->setSelectionMode(-1);
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(HbMenuData::mExecPoint != point);
        
    // item is null
    mTestWidget->setSelectionMode(HbAbstractItemView::NoSelection);
    HbListView::mCurrentIndex = QModelIndex();    
    mTestWidget->callLongPressedSlot(0, point);
    QVERIFY(HbMenuData::mExecPoint != point);
    
    // item has invalid index
    item->mModelIndex = QModelIndex();
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(HbMenuData::mExecPoint != point);
    
    // item at position is not null, returns current index is valid
    item->mModelIndex = model->index(0, 0, QModelIndex());
    mTestWidget->mItem->mModelIndex = HbListView::mCurrentIndex ;
    
    // model is null
    VideoProxyModelGeneric *tmp = mTestWidget->mModel;
    mTestWidget->mModel = 0;
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(HbMenuData::mExecPoint != point);
    mTestWidget->mModel = tmp;
    
    VideoServices *videoServices = VideoServices::instance();
    
    // gotten id != KVcxMvcMediaTypeVideo, service exists true and id != KVcxMvcMediaTypeAlbum
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(TMPXItemId(1,1));
    mTestWidget->mVideoServices = videoServices;
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(HbMenuData::mExecPoint == point);
    
    // gotten id != KVcxMvcMediaTypeVideo, service does not exists and id !=  KVcxMvcMediaTypeAlbum
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(TMPXItemId(1,1));
    mTestWidget->mVideoServices = 0;
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(HbMenuData::mExecPoint == point);
    
    // gotten id != KVcxMvcMediaTypeVideo, service service does not exists and id ==  KVcxMvcMediaTypeAlbum
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(TMPXItemId(1,2));
    mTestWidget->mVideoServices = 0;
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(HbMenuData::mExecPoint == point);
    HbMenuData::mExecPoint = QPointF();
    
    // gotten id == KVcxMvcMediaTypeVideo
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(TMPXItemId(1,02));
    mTestWidget->mVideoServices = 0;
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(HbMenuData::mExecPoint == point);
    HbMenuData::mExecPoint = QPointF();
    
    // no context menu
    // (context menu setup fails, due invalid amount of correct actions)
    QMap<VideoListWidget::TContextActionIds, HbAction*>::iterator iter = 
            mTestWidget->mContextMenuActions.begin();
    iter++;
    HbAction *nullAction = 0;
    iter.value() = nullAction;
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(HbMenuData::mExecPoint != point);

    // long press gesture:
    // -is service
    // -current service is browse
    HbMenuData::mExecPoint = QPointF();
    videoServices->mCurrentService = VideoServices::EBrowse;
    delete mTestWidget;
    mTestWidget = 0;
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
    QVERIFY(mTestWidget->initialize(*model, true) == 0);
    QVERIFY(mTestWidget->activate(VideoCollectionCommon::ELevelDefaultColl) == 0);
    mTestWidget->mItem->mModelIndex = model->index(0, 0, QModelIndex());
    mTestWidget->callLongPressedSlot(item, point);
    QCOMPARE(mTestWidget->mContextMenuActions.count(), 3);
    QVERIFY(mTestWidget->mContextMenuActions[VideoListWidget::EActionPlay]->isVisible());
    QVERIFY(mTestWidget->mContextMenuActions[VideoListWidget::EActionDelete]->isVisible());
    QVERIFY(mTestWidget->mContextMenuActions[VideoListWidget::EActionDetails]->isVisible());
    
    // long press gesture:
    // - is service
    // - current service is EUriFetcher
    // - category view activated
    HbMenuData::mExecPoint = QPointF();
    videoServices = VideoServices::instance();
    videoServices->mCurrentService = VideoServices::EUriFetcher;
    delete mTestWidget;
    mTestWidget = 0;
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
    QVERIFY(mTestWidget->initialize(*model, true) == 0);
    QVERIFY(mTestWidget->activate(VideoCollectionCommon::ELevelCategory) == 0);
    mTestWidget->mItem->mModelIndex = model->index(0, 0, QModelIndex());
    mTestWidget->callLongPressedSlot(item, point);
    QCOMPARE(mTestWidget->mContextMenuActions.count(), 4);
    QVERIFY(mTestWidget->mContextMenuActions[VideoListWidget::EActionOpen]->isVisible()); 
    QVERIFY(!mTestWidget->mContextMenuActions[VideoListWidget::EActionAttach]->isVisible());    
    QVERIFY(!mTestWidget->mContextMenuActions[VideoListWidget::EActionPlay]->isVisible());    
    QVERIFY(!mTestWidget->mContextMenuActions[VideoListWidget::EActionDetails]->isVisible());
    
    // long press gesture:
    // - is service
    // - current service is EUriFetcher
    // - user defined collection activated 
    HbMenuData::mExecPoint = QPointF();
    videoServices = VideoServices::instance();
    videoServices->mCurrentService = VideoServices::EUriFetcher;
    delete mTestWidget;
    mTestWidget = 0;
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
    QVERIFY(mTestWidget->initialize(*model, true) == 0);
    QVERIFY(mTestWidget->activate(VideoCollectionCommon::ELevelAlbum) == 0);
    mTestWidget->mItem->mModelIndex = model->index(0, 0, QModelIndex());
    mTestWidget->callLongPressedSlot(item, point);
    QCOMPARE(mTestWidget->mContextMenuActions.count(), 4);
    QVERIFY(!mTestWidget->mContextMenuActions[VideoListWidget::EActionOpen]->isVisible()); 
    QVERIFY(mTestWidget->mContextMenuActions[VideoListWidget::EActionAttach]->isVisible());    
    QVERIFY(mTestWidget->mContextMenuActions[VideoListWidget::EActionPlay]->isVisible());    
    QVERIFY(mTestWidget->mContextMenuActions[VideoListWidget::EActionDetails]->isVisible());
    
    // long press gesture:
    // - is service
    // - current service is EUriFetcher
    // - default collection activated 
    HbMenuData::mExecPoint = QPointF();
    videoServices = VideoServices::instance();
    videoServices->mCurrentService = VideoServices::EUriFetcher;
    delete mTestWidget;
    mTestWidget = 0;
    mTestWidget = new ListWidgetTester(mTestUiLoader, mTempView);
    QVERIFY(mTestWidget->initialize(*model, true) == 0);
    QVERIFY(mTestWidget->activate(VideoCollectionCommon::ELevelDefaultColl) == 0);
    mTestWidget->mItem->mModelIndex = model->index(0, 0, QModelIndex());
    mTestWidget->callLongPressedSlot(item, point);
    QCOMPARE(mTestWidget->mContextMenuActions.count(), 4);
    QVERIFY(!mTestWidget->mContextMenuActions[VideoListWidget::EActionOpen]->isVisible()); 
    QVERIFY(mTestWidget->mContextMenuActions[VideoListWidget::EActionAttach]->isVisible());    
    QVERIFY(mTestWidget->mContextMenuActions[VideoListWidget::EActionPlay]->isVisible());    
    QVERIFY(mTestWidget->mContextMenuActions[VideoListWidget::EActionDetails]->isVisible());
    
    // final cleanup
    videoServices->decreaseReferenceCount();
    delete item;
}

// ---------------------------------------------------------------------------
// testSetContextMenu
// ---------------------------------------------------------------------------
//
void TestListWidget::testSetContextMenu()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();
    VideoListDataModel *sourceModel = qobject_cast<VideoListDataModel*>(model->sourceModel());
    VideoProxyModelData::mItemIds.clear();
    mTestWidget->initialize(*model);
    
    setRowCount(1);
    HbListView::mCurrentIndex = model->index(0, 0, QModelIndex());
    mTestWidget->mItem->mModelIndex = HbListView::mCurrentIndex ;
     
    TMPXItemId itemId;
    QPointF point(1,1);
    itemId.iId2 = 0;
    VideoProxyModelData::mItemIds.append(itemId);

    HbAbstractViewItem *item = new HbAbstractViewItem();
    item->mModelIndex = model->index(0, 0, QModelIndex());
    
    // no context menu
    delete mTestWidget->mContextMenu;
    mTestWidget->mContextMenu = 0;   
    
    // mCurrentLevel == ELevelVideos
    int visibleCount = 0;
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    mTestWidget->callLongPressedSlot(item, point);
    
    QMap<VideoListWidget::TContextActionIds, HbAction*>::iterator iter = mTestWidget->mContextMenuActions.begin();    
    QVERIFY(iter != mTestWidget->mContextMenuActions.end());
    
    while(iter != mTestWidget->mContextMenuActions.end())
    {
        if(iter.value()->isVisible())
        {
            visibleCount++;   
        }
        ++iter;
    }
    QVERIFY(visibleCount == 5);
    
    // invalid amount of actions -> invalid items gets removed
    HbAction *nullAction = 0;
    iter = mTestWidget->mContextMenuActions.begin();
    iter++;
    iter.value() = nullAction;
    mTestWidget->callLongPressedSlot(item, point);
    QVERIFY(!mTestWidget->mContextMenu);
    QVERIFY(mTestWidget->mContextMenuActions.count() == 0);            
    
    // mCurrentLevel == ELevelCategory
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    // service does not exist
    mTestWidget->mVideoServices = 0;
    mTestWidget->callLongPressedSlot(item, point);
    iter = mTestWidget->mContextMenuActions.begin();
    QVERIFY(iter != mTestWidget->mContextMenuActions.end());
    visibleCount = 0;
    while(iter != mTestWidget->mContextMenuActions.end())
    {
        if(iter.value()->isVisible())
        {
            visibleCount++;   
         }
        ++iter;
    }
    QVERIFY(visibleCount == 1);
    
    VideoServices *videoServices = VideoServices::instance();
    // service exists, mpxId.iId2 != KVcxMvcMediaTypeAlbum
    mTestWidget->mVideoServices = videoServices;
    mTestWidget->callLongPressedSlot(item, point);
    iter = mTestWidget->mContextMenuActions.begin();
    QVERIFY(iter != mTestWidget->mContextMenuActions.end());
    visibleCount = 0;
    while(iter != mTestWidget->mContextMenuActions.end())
    {
        if(iter.value()->isVisible())
        {
            visibleCount++;   
        }
        ++iter;
    }
    QVERIFY(visibleCount == 1);
    
    // service does not exists , mpxId.iId2 == KVcxMvcMediaTypeAlbum
    VideoProxyModelData::mItemIds.append(TMPXItemId(1,KVcxMvcMediaTypeAlbum));
    HbListView::mCurrentIndex = model->index(1, 0, QModelIndex());
    item->mModelIndex = model->index(1, 0, QModelIndex());
    mTestWidget->mVideoServices = 0;
    mTestWidget->callLongPressedSlot(item, point);
    iter = mTestWidget->mContextMenuActions.begin();
    QVERIFY(iter != mTestWidget->mContextMenuActions.end());
    visibleCount = 0;
    while(iter != mTestWidget->mContextMenuActions.end())
    {
        if(iter.value()->isVisible())
        {
            visibleCount++;   
        }
        ++iter;
    }
    QVERIFY(visibleCount == 3);
    HbListView::mCurrentIndex = model->index(0, 0, QModelIndex());
    
    // mCurrentLevel == ELevelAlbum
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
    // service does not exists
    mTestWidget->mVideoServices = 0;
    mTestWidget->callLongPressedSlot(item, point);
    iter = mTestWidget->mContextMenuActions.begin();
    QVERIFY(iter != mTestWidget->mContextMenuActions.end());
    visibleCount = 0;
    while(iter != mTestWidget->mContextMenuActions.end())
    {
        if(iter.value()->isVisible())
        {
            visibleCount++;   
        }
        ++iter;
    }
    QVERIFY(visibleCount == 5);    
    
    // service does exists
    // object needs to be resetted for the service use
    cleanup();
    init();
    setRowCount(1);
    model = wrapper.getAllVideosModel();
    mTestWidget->mVideoServices = videoServices;
    videoServices->mCurrentService = VideoServices::EUriFetcher;
    mTestWidget->initialize(*model, videoServices);
    HbListView::mCurrentIndex = model->index(0, 0, QModelIndex());
    mTestWidget->mItem->mModelIndex = HbListView::mCurrentIndex ;
    
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
    VideoProxyModelData::mItemIds.append(itemId);

    mTestWidget->callLongPressedSlot(item, point);
    iter = mTestWidget->mContextMenuActions.begin();
    QVERIFY(iter != mTestWidget->mContextMenuActions.end());
    visibleCount = 0;
    while(iter != mTestWidget->mContextMenuActions.end())
    {
        if(iter.value()->isVisible())
        {
            visibleCount++;   
        }
        ++iter;
    }
    QVERIFY(visibleCount == 3);    
      
    //invalid level
    mTestWidget->mCurrentLevel = (VideoCollectionCommon::TCollectionLevels)0;
    mTestWidget->callLongPressedSlot(item, point);
    iter = mTestWidget->mContextMenuActions.begin();
    QVERIFY(iter != mTestWidget->mContextMenuActions.end());
    visibleCount = 0;
    while(iter != mTestWidget->mContextMenuActions.end())
    {
        if(iter.value()->isVisible())
        {
            visibleCount++;   
        }
        ++iter;
    }
    QVERIFY(visibleCount == 0);
    videoServices->decreaseReferenceCount();
}

void TestListWidget::testDoDelayedsSlot()
{
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(doDelayedsSlot()));
    
    //dodelayed calls create context menu, which is already tested at
    // testSetContextMenu -method, these tests are just for coverity's sake
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();
    mTestWidget->initialize(*model);
    
    delete mTestWidget->mContextMenu;
    mTestWidget->mContextMenu = 0;   
    int visibleCount = 0;
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    
    // no context menu
    emit testSignal();
    
    QVERIFY(mTestWidget->mContextMenuActions.count() == 8);
   
    // context menu exists
    emit testSignal();
   
    QVERIFY(mTestWidget->mContextMenuActions.count() == 8);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(scrollingStartedSlot()));
}

// ---------------------------------------------------------------------------
// testDeleteItemSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testDeleteItemSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    HbMessageBoxData::mLatestTxt = "";
    VideoProxyModelData::mLastIndex = QModelIndex();
    mTestWidget->initialize(*model);    
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(deleteItemSlot()));    
    
    HbMessageBox *box = new HbMessageBox(); 
    
    // no model
    VideoListDataModelData::mDataAccessCount = 0;
    VideoProxyModelGeneric *tmp = mTestWidget->mModel;
    mTestWidget->mModel = 0;
    emit testSignal();
    QVERIFY(VideoListDataModelData::dataAccessCount() == 0);
    QVERIFY(HbMessageBoxData::mLatestTxt.isEmpty());
    QVERIFY(!VideoProxyModelData::mLastIndex.isValid());
    setRowCount(1);
    mTestWidget->mModel = tmp;
    
    // data fetched from item is invalid
    VideoListDataModelData::mDataAccessCount = 0;
    setRowCount(1);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex()); 
    emit testSignal();
    QVERIFY(VideoListDataModelData::dataAccessCount() == 1);
    QVERIFY(HbMessageBoxData::mLatestTxt.isEmpty());
    QVERIFY(!VideoProxyModelData::mLastIndex.isValid());
    setRowCount(0);
    
    // data is valid
    VideoListDataModelData::mDataAccessCount = 0;
    setRowCount(1);
    QVariant data = QString("test");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    
    // messagebox question returns false
    HbMessageBoxData::mQuestionReturnValue = false;
    emit testSignal();
    box->emitDialogFinished(mTestWidget, SLOT(deleteItemDialogFinished(int)), HbMessageBox::No);
    QVERIFY(VideoListDataModelData::dataAccessCount() == 1);
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    QVERIFY(!VideoProxyModelData::mLastIndex.isValid());
    HbMessageBoxData::mLatestTxt = "";
    setRowCount(1);
    data = QString("test");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    
    // messagebox question returns true
    VideoListDataModelData::mDataAccessCount = 0;
    VideoProxyModelData::mDeleteItemsFails = false;
    HbMessageBoxData::mQuestionReturnValue = true;
    emit testSignal();
    box->emitDialogFinished(mTestWidget, SLOT(deleteItemDialogFinished(int)), HbMessageBox::Yes);
    QVERIFY(VideoListDataModelData::dataAccessCount() == 1);
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    QVERIFY(VideoProxyModelData::mLastIndex.row() == 0);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(deleteItemSlot()));
}
 
// ---------------------------------------------------------------------------
// testRenameSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testRenameSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();
    
    HbInputDialog *dialog = new HbInputDialog();
    
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(renameSlot()));
    
    VideoProxyModelData::reset();
    VideoProxyModelData::mLastIndex = QModelIndex();
    mTestWidget->initialize(*model);
    
    // Good case
    VideoProxyModelData::mLastItemNameInRename = "";
    HbInputDialog::mValueReturnValue = "renamedVideo";
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mOpenCallCount = 0;
    setRowCount(1);
    VideoProxyModelData::mItemIds.append(TMPXItemId(0, KVcxMvcMediaTypeAlbum));
    QVariant data = QString("albumName");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    emit testSignal();
    dialog->emitDialogFinished(mTestWidget, SLOT(renameDialogFinished(HbAction *)), 0);
    
    QCOMPARE(HbInputDialog::mOpenCallCount, 1);
    QVERIFY(HbInputDialog::mValueCallCount == 1);
           
    // New name is same as previous
    HbInputDialog::mValueReturnValue = "albumName";
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mOpenCallCount = 0;
    VideoProxyModelData::mLastItemNameInRename = "";
    data = QString("albumName");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    emit testSignal();
    dialog->emitDialogFinished(mTestWidget, SLOT(renameDialogFinished(HbAction *)), 0);
    QVERIFY(VideoProxyModelData::mLastItemNameInRename == "");
    QCOMPARE(HbInputDialog::mOpenCallCount, 1);
    QVERIFY(HbInputDialog::mValueCallCount == 1);
    
    // Dialog canceled 
    HbInputDialog::mValueReturnValue = "";
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mOpenCallCount = 0;
    VideoProxyModelData::mLastItemNameInRename = "";
    data = QString("albumName");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    emit testSignal();
    dialog->emitDialogFinished(mTestWidget, SLOT(renameDialogFinished(HbAction *)), 1);
    QVERIFY(VideoProxyModelData::mLastItemNameInRename == "");
    QCOMPARE(HbInputDialog::mOpenCallCount, 1);
    QVERIFY(HbInputDialog::mValueCallCount == 0);
    
    // New name is empty.
    VideoProxyModelData::mLastItemNameInRename = "";
    HbInputDialog::mValueReturnValue = "";
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mOpenCallCount = 0;
    data = QString("albumName");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    emit testSignal();
    dialog->emitDialogFinished(mTestWidget, SLOT(renameDialogFinished(HbAction *)), 0);
    QVERIFY(VideoProxyModelData::mLastItemNameInRename == "");
    QCOMPARE(HbInputDialog::mOpenCallCount, 1);
    QVERIFY(HbInputDialog::mValueCallCount == 1);
    
    // Item is video
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(TMPXItemId(0, KVcxMvcMediaTypeVideo));
    VideoProxyModelData::mLastItemNameInRename = "";
    HbInputDialog::mValueReturnValue = "renamedVideo";
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mOpenCallCount = 0;
    data = QString("oldVideoNama");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    emit testSignal();
    dialog->emitDialogFinished(mTestWidget, SLOT(renameDialogFinished(HbAction *)), 0);
    QCOMPARE(HbInputDialog::mOpenCallCount, 1);
    QVERIFY(HbInputDialog::mValueCallCount == 1);
    QVERIFY(VideoProxyModelData::mLastItemNameInRename == "renamedVideo");

    // Item is album
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(TMPXItemId(0, KVcxMvcMediaTypeAlbum));
    VideoProxyModelData::mLastItemNameInRename = "";
    HbInputDialog::mValueReturnValue = "renamedAlbum";
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mOpenCallCount = 0;
    data = QString("oldAlbumName");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    emit testSignal();
    dialog->emitDialogFinished(mTestWidget, SLOT(renameDialogFinished(HbAction *)), 0);
    QCOMPARE(HbInputDialog::mOpenCallCount, 1);
    QVERIFY(HbInputDialog::mValueCallCount == 1);
    QVERIFY(VideoProxyModelData::mLastItemNameInRename == "renamedAlbum");
    
    // Item is category
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(TMPXItemId(0, KVcxMvcMediaTypeCategory));
    VideoProxyModelData::mLastItemNameInRename = "";
    HbInputDialog::mValueReturnValue = "notallowed";
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mOpenCallCount = 0;
    data = QString("oldCategoryName");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    emit testSignal();
    QCOMPARE(HbInputDialog::mOpenCallCount, 0);
    QVERIFY(VideoProxyModelData::mLastItemNameInRename == "");    
    
    // No model
    VideoProxyModelData::mLastItemNameInRename = "";
    HbInputDialog::mValueReturnValue = "";
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mOpenCallCount = 0;
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(TMPXItemId(0, KVcxMvcMediaTypeAlbum));
    data = QString("albumName");
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    VideoProxyModelGeneric *tmp = mTestWidget->mModel;
    mTestWidget->mModel = 0;    
    emit testSignal();
    mTestWidget->mModel = tmp;    
    QVERIFY(VideoProxyModelData::mLastItemNameInRename == "");
    QCOMPARE(HbInputDialog::mOpenCallCount, 0);
    
    // Variant data is invalid
    VideoProxyModelData::mLastItemNameInRename = "";
    HbInputDialog::mValueReturnValue = "renamedVideo";
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mOpenCallCount = 0;
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(TMPXItemId(0, KVcxMvcMediaTypeAlbum));
    data = QVariant();
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    emit testSignal();
    QVERIFY(VideoProxyModelData::mLastItemNameInRename == "");
    QCOMPARE(HbInputDialog::mOpenCallCount, 0);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(renameSlot()));
}
  
// ---------------------------------------------------------------------------
// testOpenItemSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testOpenItemSlot()
{
    // testEmitActivated has already tested the doEmitActivated functionality.
    // Just verify here, that the openItemSlot calls doEmitActivated.
    
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();
    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(openItemSlot()));
    setRowCount(1);
    TMPXItemId savedId = TMPXItemId(1,1);
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(savedId);
    HbListView::mCurrentIndex = model->index(0, 0, QModelIndex());
    
    // Making the context menu visible ensures that the doEmitActivated is called
    // instead of just emitActivated, as emitActivated does nothing if context menu
    // is already visible.
    if (!mTestWidget->mContextMenu)
    {
        mTestWidget->mContextMenu = new HbMenu;
    }
    mTestWidget->mContextMenu->show();
    
    emit testSignal();
    
    QVERIFY(VideoProxyModelData::mLastItemId == savedId);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(openItemSlot()));    
}

// ---------------------------------------------------------------------------
// testPlayItemSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testPlayItemSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();
    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(playItemSlot()));
    setRowCount(1);
    TMPXItemId savedId = TMPXItemId(1,1);
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(savedId);
    HbListView::mCurrentIndex = model->index(0, 0, QModelIndex());
    
    emit testSignal();
    
    QVERIFY(VideoProxyModelData::mLastItemId == savedId);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(playItemSlot()));    
}

// ---------------------------------------------------------------------------
// testAddToCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testAddToCollectionSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    VideoListSelectionDialogData::mSelectionType = -1;
    VideoListSelectionDialogData::mSettedMpxId = TMPXItemId::InvalidId();
    
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(addToCollectionSlot()));
    setRowCount(1);
    TMPXItemId savedId = TMPXItemId(1,1);
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(savedId);
    HbListView::mCurrentIndex = model->index(0, 0, QModelIndex());
    
    // no model
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1 );
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());

    mTestWidget->initialize(*model);
    // dialog finding fails
    VideoCollectionUiLoaderData::mFindFailureNameList.append(DOCML_NAME_DIALOG);
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1 );
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    VideoCollectionUiLoaderData::mFindFailureNameList.clear();
    
    // invalid id at current index
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(TMPXItemId::InvalidId());
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1 );
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    
    // valid id at current index
    VideoProxyModelData::mItemIds.clear();
    VideoProxyModelData::mItemIds.append(savedId);
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == VideoListSelectionDialog::ESelectCollection );
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == savedId);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(addToCollectionSlot()));
        
}
 
// ---------------------------------------------------------------------------
// testRemoveFromCollectionSlot
// ---------------------------------------------------------------------------
// 
void TestListWidget::testRemoveFromCollectionSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    VideoProxyModelData::mRemoveItemsFromAlbumReturnValue = 0;
    VideoProxyModelData::mLastItemId = TMPXItemId::InvalidId();
    VideoProxyModelData::mItemIds.clear();
    
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(removeFromCollectionSlot()));
    setRowCount(2);
    TMPXItemId savedId = TMPXItemId(1,1);
    VideoProxyModelData::mItemIds.clear();
    // invalid id at index 0
    VideoProxyModelData::mItemIds.append(TMPXItemId::InvalidId());
    VideoProxyModelData::mItemIds.append(savedId);
    HbListView::mCurrentIndex = model->index(1, 0, QModelIndex());
    
    // no model
    emit testSignal();
    QVERIFY(VideoProxyModelData::mLastItemId  == TMPXItemId::InvalidId());
    
    mTestWidget->initialize(*model);
    VideoProxyModelData::mOpenedItemId = TMPXItemId::InvalidId();
    
    // collection id is invalid
    emit testSignal();
    QVERIFY(VideoProxyModelData::mLastItemId  == TMPXItemId::InvalidId());
    VideoProxyModelData::mOpenedItemId = TMPXItemId(1,2);
    
    // media at current index is invalid
    HbListView::mCurrentIndex = model->index(0, 0, QModelIndex());
    emit testSignal();
    QVERIFY(VideoProxyModelData::mLastItemId  == TMPXItemId::InvalidId());
            
    // all is ok
    HbListView::mCurrentIndex = model->index(1, 0, QModelIndex());
    emit testSignal();
    
    QVERIFY(VideoProxyModelData::mLastItemId  == TMPXItemId(1,2));
    // ids are to be saved at VideoProxyModelData::mItemIds 
    QVERIFY(VideoProxyModelData::mItemIds.at(0) == savedId);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(removeFromCollectionSlot())); 
}
 
// ---------------------------------------------------------------------------
// testRemoveCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testRemoveCollectionSlot()
{
    VideoProxyModelData::mRemoveAlbumsFails = false;
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();
    
    HbMessageBoxData::mQuestionReturnValue = true;
    HbMessageBoxData::mLatestTxt = "";
    VideoProxyModelData::mLastIndex = QModelIndex();
    
    setRowCount(1);
    HbListView::mCurrentIndex = model->index(0, 0, QModelIndex());
    
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(removeCollectionSlot()));
    HbMessageBox *box = new HbMessageBox();
    
    setRowCount(2);
    TMPXItemId savedId = TMPXItemId(1,1);
    VideoProxyModelData::mItemIds.clear();
    // invalid id at index 0
    VideoProxyModelData::mItemIds.append(TMPXItemId::InvalidId());
    VideoProxyModelData::mItemIds.append(savedId);
    HbListView::mCurrentIndex = model->index(1, 0, QModelIndex());
    
    // no model
    emit testSignal();
    QVERIFY(!VideoProxyModelData::mLastIndex.isValid());
    QVERIFY(HbMessageBoxData::mLatestTxt.isEmpty());
    
    mTestWidget->initialize(*model);
    
    // invalid data    
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, QVariant());
    emit testSignal();
    QVERIFY(!VideoProxyModelData::mLastIndex.isValid());
    QVERIFY(HbMessageBoxData::mLatestTxt.isEmpty());
    
    // valid data 
    VideoListDataModelData::setData(VideoCollectionCommon::KeyTitle, "test");
    emit testSignal();
    box->emitDialogFinished(mTestWidget, SLOT(removeCollectionDialogFinished(int)), HbMessageBox::Yes);
    QVERIFY(VideoProxyModelData::mLastIndex.isValid());
    QVERIFY(VideoProxyModelData::mLastIndex.row() == 1);
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    
    // msg box return false (for coverity)
    VideoProxyModelData::mLastIndex = QModelIndex();
    HbMessageBoxData::mLatestTxt = "";
    HbMessageBoxData::mQuestionReturnValue = false;
    emit testSignal();
    box->emitDialogFinished(mTestWidget, SLOT(removeCollectionDialogFinished(int)), HbMessageBox::No);
    QVERIFY(!VideoProxyModelData::mLastIndex.isValid());
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(removeCollectionSlot()));
}

// ---------------------------------------------------------------------------
// testOpenDetailsSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testOpenDetailsSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    QSignalSpy spysignal(mTestWidget, SIGNAL(command(int)));
    HbMessageBoxData::mLatestTxt = "";
    VideoProxyModelData::mLastIndex = QModelIndex();
    setRowCount(0);
    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(openDetailsSlot())); 

    // no model
    VideoProxyModelGeneric *tmp = mTestWidget->mModel;
    mTestWidget->mModel = 0;
    emit testSignal();
    QVERIFY(spysignal.count() == 0);
    QVERIFY(!VideoProxyModelData::mLastIndex.isValid());
    mTestWidget->mModel = tmp;
    
    // detail fetch fails
    setRowCount(1);
    QVariant data = QString("test");
    VideoListDataModelData::setData(Qt::DisplayRole, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    VideoProxyModelData::mDetailsReturnValue = -1;
    
    emit testSignal();
    QVERIFY(spysignal.count() == 0);
    QVERIFY(VideoProxyModelData::mLastIndex.row() == 0);
    
    // detail fetch succeeds
    VideoProxyModelData::mDetailsReturnValue = 0;
    VideoProxyModelData::mLastIndex = QModelIndex();
    emit testSignal();
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.at(0).at(0).isValid());
    QVERIFY(spysignal.at(0).at(0).toInt() == MpxHbVideoCommon::ActivateVideoDetailsView);
    QVERIFY(VideoProxyModelData::mLastIndex.row() == 0);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(openDetailsSlot())); 
}

// ---------------------------------------------------------------------------
// testBack
// ---------------------------------------------------------------------------
//
void TestListWidget::testBack()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    QSignalSpy spysignal(mTestWidget, SIGNAL(collectionOpened(bool, const QString&, const TMPXItemId&)));
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(back()));
    // no model
    emit testSignal();        
    QVERIFY(spysignal.count() == 0);
        
    // model exist
    mTestWidget->initialize(*model);   
    emit testSignal();    
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).isValid());
    QVERIFY(spysignal.value(0).at(0).toBool() == false);   
    QVERIFY(spysignal.value(0).at(1).isValid());
    QVERIFY(spysignal.value(0).at(1).toString().isEmpty());   
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(back()));
}

// ---------------------------------------------------------------------------
// testScrollingStartedSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testScrollingStartedSlot()
{
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(scrollingStartedSlot()));
    
    emit testSignal();
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(scrollingStartedSlot()));
}

// ---------------------------------------------------------------------------
// testScrollingEndedSlot
// ---------------------------------------------------------------------------
// 
void TestListWidget::testScrollingEndedSlot()
{
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = true;
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(scrollingEndedSlot()));
    
    // no visible items
    HbListView::mVisibleItems.clear();
    emit testSignal();
    QVERIFY(VideoThumbnailTestData::mStartBackgroundFetchingCallCount == 1);
    QVERIFY(VideoThumbnailTestData::mFetchIndex == 0);
    QVERIFY(!mTestWidget->mScrollPositionTimer->isActive());
    
    // setup few "visible" items and make sure item count match at thumbnail data
    int count = 10;
    setRowCount(count);
    HbAbstractViewItem *item = 0;
    for(int i = 0; i < count; ++i)
    {
        item = new HbAbstractViewItem();
        item->mModelIndex = model->index(i, 0, QModelIndex());
        HbListView::mVisibleItems.append(item);
    }
    
    // Test
    emit testSignal();
    QVERIFY(VideoThumbnailTestData::mStartBackgroundFetchingCallCount == 2);
    QVERIFY(!mTestWidget->mScrollPositionTimer->isActive());

    // Test again when timer is null.
    VideoThumbnailTestData::mStartBackgroundFetchingCallCount = 0;
    QTimer *backup = mTestWidget->mScrollPositionTimer;
    mTestWidget->mScrollPositionTimer = 0;
    emit testSignal();
    QVERIFY(VideoThumbnailTestData::mStartBackgroundFetchingCallCount == 1);
    mTestWidget->mScrollPositionTimer = backup;
    backup = 0;
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(scrollingEndedSlot()));
}

// ---------------------------------------------------------------------------
// testScrollPositionChangedSlot
// ---------------------------------------------------------------------------
// 
void TestListWidget::testScrollPositionChangedSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    mTestWidget->initialize(*model);
    const QPointF point;
    
    connect(this, SIGNAL(testSignal(const QPointF&)), mTestWidget, SLOT(scrollPositionChangedSlot(const QPointF&)));
    
    // Test
    emit testSignal(point);
    
    // Test again when timer is running.
    mTestWidget->mScrollPositionTimer->start(1000*30);
    emit testSignal(point);
    
    // Test when timer is null.
    QTimer *backup = mTestWidget->mScrollPositionTimer;
    mTestWidget->mScrollPositionTimer = 0;
    emit testSignal(point);
    mTestWidget->mScrollPositionTimer = backup;
    backup = 0;
    
    disconnect(this, SIGNAL(testSignal(const QPointF&)), mTestWidget, SLOT(scrollPositionChangedSlot(const QPointF&)));
}

void TestListWidget::testScrollPositionTimerSlot()
{
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(scrollPositionTimerSlot()));

    VideoThumbnailTestData::mStartBackgroundFetchingCallCount = 0;
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = true;
    emit testSignal();
    QCOMPARE(VideoThumbnailTestData::mStartBackgroundFetchingCallCount, 1);
    
    VideoThumbnailTestData::mStartBackgroundFetchingCallCount = 0;
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = false;
    emit testSignal();
    QCOMPARE(VideoThumbnailTestData::mStartBackgroundFetchingCallCount, 0);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(scrollPositionTimerSlot()));
}

void TestListWidget::testRowsInsertedSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    mTestWidget->initialize(*model);
    
    // setup few "visible" items to trigger the fetch
    int count = 10;
    setRowCount(count);
    HbAbstractViewItem *item = 0;
    for(int i = 0; i < count; ++i)
    {
        item = new HbAbstractViewItem();
        item->mModelIndex = model->index(i, 0, QModelIndex());
        HbListView::mVisibleItems.append(item);
    }
    
    // Test
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = true;
    QModelIndex parent;
    mTestWidget->callRowsInserted(parent, 0, 0);
    QVERIFY(VideoThumbnailTestData::mStartBackgroundFetchingCallCount == 1);
    QVERIFY(!mTestWidget->mScrollPositionTimer->isActive());
}

void TestListWidget::testRowsRemovedSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getAllVideosModel();

    mTestWidget->initialize(*model);
    
    // setup few "visible" items to trigger the fetch
    int count = 10;
    setRowCount(count);
    HbAbstractViewItem *item = 0;
    for(int i = 0; i < count; ++i)
    {
        item = new HbAbstractViewItem();
        item->mModelIndex = model->index(i, 0, QModelIndex());
        HbListView::mVisibleItems.append(item);
    }
    
    // Test
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = true;
    QModelIndex parent;
    mTestWidget->callRowsRemoved(parent, 0, 0);
    QVERIFY(VideoThumbnailTestData::mStartBackgroundFetchingCallCount == 1);
    QVERIFY(!mTestWidget->mScrollPositionTimer->isActive());
}

// end of file
