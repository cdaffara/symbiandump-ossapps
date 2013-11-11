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
* Description:   tester for methods in VideoListToolbar
*
*/

// Version : %version: 3 %

#include <hbapplication.h>
#include <qlist.h>
#include <hbaction.h>
#include <videocollectioncommon.h>
#include <vcxmyvideosdefs.h>

#include "testlisttoolbar.h"
#include "videolistview.h"
#include "videocollectionuiloader.h"
#include "videocollectionuiloaderdata.h"
#include "videolistview.h"
#include "videolistviewdata.h"
#include "videooperatorservice.h"
#include "videooperatorservicedata.h"
#include "videoservices.h"
#include "videocollectionviewutils.h"
#include "videocollectionviewutilsdata.h"
#include "videoproxymodeldata.h"
#include "videolistdatamodeldata.h"
#include "videolistselectiondialog.h"
#include "videolistselectiondialogdata.h"
#include "videocollectionwrapper.h"

#define private public
#include "videolisttoolbar.h"
#undef private



// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);

    TestListToolbar tv;

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
        pass[2] = "c:\\data\\testlisttoolbar.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

    return res;
}

// ---------------------------------------------------------------------------
// setRowCount
// ---------------------------------------------------------------------------
//
void TestListToolbar::setRowCount(int count, VideoProxyModelGeneric *model)
{
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
// initTestCase
// ---------------------------------------------------------------------------
//
void TestListToolbar::initTestCase()
{
    mTestObject = 0;
    mStubListView = 0;
    mStubUiLoader = 0;
    mStubModel = 0;
}
 
// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestListToolbar::init()
{   
    if(!mStubUiLoader)
    {
        mStubUiLoader = new VideoCollectionUiLoader();
    }
    if(!mStubListView)
    {
       mStubListView = new VideoListView(mStubUiLoader);
    }
    if(!mTestObject)
    {
        mTestObject = new VideoListToolbar(mStubUiLoader, mStubListView);
    }
    if(!mStubModel)
    {
        mStubModel = VideoCollectionWrapper::instance().getAllVideosModel();
    }
    // by default, we run as stand alone
    VideoCollectionViewUtilsData::mIsServiceValue = false;
    VideoCollectionViewUtils::instance().setIsService();
}
 
// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestListToolbar::cleanup()
{
    delete mTestObject;
    mTestObject = 0;
    
    delete mStubListView;
    mStubListView = 0;
    
    delete mStubUiLoader;
    mStubUiLoader = 0;
    
    mStubModel = 0;
    
    mLastChangedActions.clear();
    
    mLastChangedExtension = 0;
}
 
// ---------------------------------------------------------------------------
// cleanupTestCase
// ---------------------------------------------------------------------------
//
void TestListToolbar::cleanupTestCase()
{
    delete mTestObject;
    mTestObject = 0;
    
    delete mStubListView;
    mStubListView = 0;
    
    delete mStubUiLoader;
    mStubUiLoader = 0;
    
    if( VideoListViewData::mCurrentListWidget)
    {
        delete VideoListViewData::mCurrentListWidget;
        VideoListViewData::mCurrentListWidget = 0;
    }
}

// ---------------------------------------------------------------------------
// testActionsChangedSlot
// ---------------------------------------------------------------------------
//
void TestListToolbar::testActionsChangedSlot(QList<QAction*> actions)
{
    mLastChangedActions.clear();
    mLastChangedActions = actions;
}

// ---------------------------------------------------------------------------
// testToolbarExtensionChangedSlot
// ---------------------------------------------------------------------------
//
void TestListToolbar::testToolbarExtensionChangedSlot(HbToolBarExtension* extension)
{
    mLastChangedExtension = 0;
    mLastChangedExtension = extension;
}

// ---------------------------------------------------------------------------
// testInitialize
// ---------------------------------------------------------------------------
//
void TestListToolbar::testInitialize()
{
    
    mTestObject->initialize();
    // not a service, no service extensions
    QVERIFY( mTestObject->mToolbarViewsActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarCollectionActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]);    
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);
    QVERIFY( !mTestObject->mVideoOperatorServices.count());
    
    cleanup();
    init();
    
    VideoOperatorServiceData::mTitles.append("TestTitle");
    VideoOperatorServiceData::mUris.append("TestURI");
    VideoOperatorServiceData::mIcons.append("TestIcon"); 
    // not a service, one service extension
    mTestObject->initialize();
    QVERIFY( mTestObject->mToolbarViewsActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarCollectionActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]);
    QVERIFY( !mTestObject->mToolbarServiceExtension);
    QVERIFY( mTestObject->mVideoOperatorServices.count() == 1);
    
    cleanup();
    init();
    
    VideoOperatorServiceData::mTitles.append("TestTitle");
    VideoOperatorServiceData::mTitles.append("TestTitle2");
    VideoOperatorServiceData::mUris.append("TestURI");
    VideoOperatorServiceData::mUris.append("TestURI2");
    VideoOperatorServiceData::mIcons.append("TestIcon"); 
    VideoOperatorServiceData::mIcons.append("TestIcon2"); 
    // not a service, multiple service extensions
    mTestObject->initialize();
    QVERIFY( mTestObject->mToolbarViewsActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarCollectionActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]);
    QVERIFY( mTestObject->mToolbarServiceExtension);
    QVERIFY( mTestObject->mVideoOperatorServices.count() == 2);
    
    // second call, no change
    mTestObject->initialize();
    QVERIFY( mTestObject->mToolbarViewsActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarCollectionActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]);
    QVERIFY( mTestObject->mToolbarServiceExtension);
    QVERIFY( mTestObject->mVideoOperatorServices.count() == 2);
    
    cleanup();
    init();
    VideoOperatorServiceData::mTitles.append("TestTitle");
    VideoOperatorServiceData::mUris.append("TestURI");
    VideoOperatorServiceData::mIcons.append("TestIcon"); 
    // for coverity: plain initialization, service for one service allready gotten
    VideoOperatorService *tmp = new VideoOperatorService();
    mTestObject->mVideoOperatorServices.append(tmp);
    mTestObject->initialize();
    QVERIFY( mTestObject->mToolbarViewsActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarCollectionActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]);
    // service toolbutton is not created
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]);
    QVERIFY( !mTestObject->mToolbarServiceExtension);
    QVERIFY( mTestObject->mVideoOperatorServices.count() == 1);   
    VideoOperatorServiceData::reset();
}

// ---------------------------------------------------------------------------
// InitializeService
// ---------------------------------------------------------------------------
//   
void TestListToolbar::testInitializeService()
{
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    // service object does not exists yet
    mTestObject->initialize();
    QVERIFY( mTestObject->mToolbarViewsActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarCollectionActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]);    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);
    QVERIFY( !mTestObject->mVideoOperatorServices.count());
    
    // service object exist
    cleanup();
    init();
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    
    mTestObject->mVideoServices = VideoServices::instance();
    mTestObject->initialize();
    QVERIFY( mTestObject->mToolbarViewsActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarCollectionActionGroup != 0 );
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]);    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);
    QVERIFY( !mTestObject->mVideoOperatorServices.count());
    
    VideoCollectionViewUtilsData::mIsServiceValue = false;
    VideoCollectionViewUtils::instance().setIsService();
}

// ---------------------------------------------------------------------------
// testViewStateChanged
// ---------------------------------------------------------------------------
//   
void TestListToolbar::testViewStateChanged()
{
    connect(mTestObject, SIGNAL(actionsChanged(QList<QAction*>)), this, SLOT(testActionsChangedSlot(QList<QAction*>)));
    connect(mTestObject, SIGNAL(toolbarExtensionChanged(HbToolBarExtension*)), this, SLOT(testToolbarExtensionChangedSlot(HbToolBarExtension*)));
    
    // not initialized
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelVideos, false, true);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]);    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]);
        
    mTestObject->initialize();
    
    // not a service, level is ELevelCategory  
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelCategory, false, true);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY(mLastChangedActions.count() == 2);

    // not a service, level is ELevelDefaultColl
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelDefaultColl, false, true);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( !mLastChangedActions.count());
    
    // not a service, level is ELevelVideos
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelVideos, false, true);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY(mLastChangedActions.count() == 2);
    
    // not a service, level is ELevelAlbum, model ready 
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelAlbum, false, true);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]->isVisible());
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]->isVisible());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( mLastChangedActions.count() == 2);
    
    // change level, otherwise signals are not emitted
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelVideos, false, true);
    
    // not a service, level is ELevelAlbum, model not ready 
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelAlbum, true, false);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]->isVisible());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]->isVisible());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( mLastChangedActions.count() == 2);
    
    // not a service, level is not anything above
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelInvalid, false, true);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( !mLastChangedActions.count());
    
    disconnect(mTestObject, SIGNAL(actionsChanged(QList<QAction*>)), this, SLOT(testActionsChangedSlot(QList<QAction*>)));
    disconnect(mTestObject, SIGNAL(toolbarExtensionChanged(HbToolBarExtension*)), this, SLOT(testToolbarExtensionChangedSlot(HbToolBarExtension*)));
    
}

// ---------------------------------------------------------------------------
// testViewStateChangedService
// ---------------------------------------------------------------------------
//
void TestListToolbar::testViewStateChangedService()
{
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    connect(mTestObject, SIGNAL(actionsChanged(QList<QAction*>)), this, SLOT(testActionsChangedSlot(QList<QAction*>)));
    connect(mTestObject, SIGNAL(toolbarExtensionChanged(HbToolBarExtension*)), this, SLOT(testToolbarExtensionChangedSlot(HbToolBarExtension*)));
    
    mTestObject->initialize();
    
    // service, level is ELevelCategory  
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelCategory, false, true);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY(mLastChangedActions.count() == 2);
    
    // service, level is ELevelDefaultColl
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelDefaultColl, false, true);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( !mLastChangedActions.count());
    
    // service, level is ELevelVideos
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelVideos, false, true);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY(mLastChangedActions.count() == 2);
  
    // service, level is ELevelAlbum model ready
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelAlbum, false, true);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( !mLastChangedActions.count());
    
    // change level, otherwise not all signal are emitted
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelVideos, false, true);
    
    // service, level is ELevelAlbum model not ready
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelAlbum, false, false);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( !mLastChangedActions.count());

    disconnect(mTestObject, SIGNAL(actionsChanged(QList<QAction*>)), this, SLOT(testActionsChangedSlot(QList<QAction*>)));
    disconnect(mTestObject, SIGNAL(toolbarExtensionChanged(HbToolBarExtension*)), this, SLOT(testToolbarExtensionChangedSlot(HbToolBarExtension*)));
}

// ---------------------------------------------------------------------------
// testViewStateChangedWithServiceExt
// ---------------------------------------------------------------------------
//
void TestListToolbar::testViewStateChangedWithServiceExt()
{
    connect(mTestObject, SIGNAL(actionsChanged(QList<QAction*>)), this, SLOT(testActionsChangedSlot(QList<QAction*>)));
    connect(mTestObject, SIGNAL(toolbarExtensionChanged(HbToolBarExtension*)), this, SLOT(testToolbarExtensionChangedSlot(HbToolBarExtension*)));
    
    VideoOperatorServiceData::mTitles.append("TestTitle");
    VideoOperatorServiceData::mTitles.append("TestTitle2");
    VideoOperatorServiceData::mUris.append("TestURI");
    VideoOperatorServiceData::mUris.append("TestURI2");
    VideoOperatorServiceData::mIcons.append("TestIcon"); 
    VideoOperatorServiceData::mIcons.append("TestIcon2"); 
    
    mTestObject->initialize();
    
    // not a service, level is ELevelCategory  
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelCategory, false, true);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( mLastChangedExtension);
    QVERIFY(mLastChangedActions.count() == 2);
    
    mLastChangedExtension = 0;
        
    // not a service, level is ELevelDefaultColl
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelDefaultColl, false, true);
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( !mLastChangedActions.count());
    
    // not a service, level is ELevelVideos
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelVideos, false, true);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( mLastChangedExtension);
    QVERIFY(mLastChangedActions.count() == 2);
      
    mLastChangedExtension = 0; 
    // not a service, level is ELevelAlbum, model ready 
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelAlbum, false, true);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]->isVisible());
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]->isVisible());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( mLastChangedActions.count() == 2);
    
    // need to change level to make sure signals are emitted correctly
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelVideos, false, true);
    mLastChangedExtension = 0;
    
    // not a service, level is ELevelAlbum, model not ready 
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelAlbum, true, false);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionAddVideos]->isVisible());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionRemoveVideos]->isVisible());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( mLastChangedActions.count() == 2);
    
    // not a service, level is not anything above
    mTestObject->viewStateChanged(VideoCollectionCommon::ELevelInvalid, false, true);
    QVERIFY( mTestObject->mToolbarActions[VideoListToolbar::ETBActionAllVideos]->isChecked());    
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionCollections]->isChecked());
    QVERIFY( !mTestObject->mToolbarActions[VideoListToolbar::ETBActionServices]);    
    QVERIFY( !mLastChangedExtension);
    QVERIFY( !mLastChangedActions.count());
    
    disconnect(mTestObject, SIGNAL(actionsChanged(QList<QAction*>)), this, SLOT(testActionsChangedSlot(QList<QAction*>)));
    disconnect(mTestObject, SIGNAL(toolbarExtensionChanged(HbToolBarExtension*)), this, SLOT(testToolbarExtensionChangedSlot(HbToolBarExtension*)));
}

// ---------------------------------------------------------------------------
// testAddVideosToCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListToolbar::testAddVideosToCollectionSlot()
{

    TMPXItemId invalidId = TMPXItemId::InvalidId();
    VideoProxyModelData::mOpenedItemId = TMPXItemId(0, 2);
    
    mStubModel =  VideoCollectionWrapper::instance().getCollectionContentModel();
    
    // no list widget
    mTestObject->addVideosToCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);
    
    VideoListViewData::mCurrentListWidget = new VideoListWidget(mStubUiLoader);
    
    // no model in widget
    mTestObject->addVideosToCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);
    
    // model exist. 
    VideoListViewData::mCurrentListWidget->initialize(*mStubModel, false, VideoCollectionCommon::ELevelAlbum);
    
    // Selection dialog widget loading fails.
    VideoCollectionUiLoaderData::mFindFailure = true;
    mTestObject->addVideosToCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);    
    VideoCollectionUiLoaderData::mFindFailure = false;
    
    // cannot load DOCML_NAME_VC_VIDEOLISTWIDGET
    VideoCollectionUiLoaderData::mFindFailureNameList.append(DOCML_NAME_VC_VIDEOLISTWIDGET);
    mTestObject->addVideosToCollectionSlot();
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionLaunchCount == 1);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == VideoListSelectionDialog::EAddToCollection);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoProxyModelData::mOpenedItemId);
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);  
    VideoListSelectionDialogData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoCollectionUiLoaderData::reset();
    
    VideoListWidget *allVideos = mStubUiLoader->findWidget<VideoListWidget>(
                        DOCML_NAME_VC_VIDEOLISTWIDGET);
    // loads DOCML_NAME_VC_VIDEOLISTWIDGET but is has no model
    VideoProxyModelGeneric *tmp = allVideos->getModel();
    VideoProxyModelGeneric *nullModel = 0;
    allVideos->initialize(*nullModel, false, VideoCollectionCommon::ELevelVideos);
    VideoCollectionUiLoaderData::mFindFailureNameList.removeAll(DOCML_NAME_VC_VIDEOLISTWIDGET);
    mTestObject->addVideosToCollectionSlot();
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionLaunchCount == 1);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == VideoListSelectionDialog::EAddToCollection);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoProxyModelData::mOpenedItemId);
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);  
    VideoListSelectionDialogData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoCollectionUiLoaderData::reset();   
    allVideos->initialize(*tmp, false, VideoCollectionCommon::ELevelVideos);
    
    // loads DOCML_NAME_VC_VIDEOLISTWIDGET, it has model, both have zero videos
    mTestObject->addVideosToCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount );
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);    

    // loads DOCML_NAME_VC_VIDEOLISTWIDGET, it has model, both have same amount, more that zero videos
    setRowCount(3, tmp);
    setRowCount(3, mStubModel);
    mTestObject->addVideosToCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount );
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusAllVideosAlreadyInCollection); 
    VideoListSelectionDialogData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoCollectionUiLoaderData::reset(); 
    
   
    // loads DOCML_NAME_VC_VIDEOLISTWIDGET, it has model, different amount, more that zero videos    
    // clear tmp model's source model to get row count zero
    VideoListDataModel *tmpSourceModel =
                qobject_cast<VideoListDataModel*>(
                        tmp->sourceModel());
    tmp->setSourceModel(0);
    mTestObject->addVideosToCollectionSlot();
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionLaunchCount == 1);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == VideoListSelectionDialog::EAddToCollection);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoProxyModelData::mOpenedItemId);
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);
    // need to restore source model. Otherwise allvideos -proxymodel used in other cases does not work correctly 
    tmp->setSourceModel(tmpSourceModel);

}
  
// ---------------------------------------------------------------------------
// testRemoveVideosFromCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListToolbar::testRemoveVideosFromCollectionSlot()
{
    if( VideoListViewData::mCurrentListWidget)
    {
        delete VideoListViewData::mCurrentListWidget;
        VideoListViewData::mCurrentListWidget = 0;
    }
    // no current list
    mTestObject->removeVideosFromCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount );
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    VideoListViewData::mCurrentListWidget = new VideoListWidget(mStubUiLoader);
        
    // no model in widget
    mTestObject->removeVideosFromCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    
    // has model, level different than ELevelAlbum
    VideoListViewData::mCurrentListWidget->initialize(*mStubModel, false, VideoCollectionCommon::ELevelVideos); 
    mTestObject->removeVideosFromCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    
    // has model, level is ELEvelAlbum, rowcount == 0
    setRowCount(0, mStubModel);
    VideoListViewData::mCurrentListWidget->initialize(*mStubModel, false, VideoCollectionCommon::ELevelAlbum);
    mTestObject->removeVideosFromCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    
    // has model, level is ELevelAlbum, rowcount > 0, dialog loading fails
    setRowCount(5, mStubModel);
    VideoCollectionUiLoaderData::mFindFailure = true;
    mTestObject->removeVideosFromCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    VideoCollectionUiLoaderData::mFindFailure = false;
    
    // has model, level is ELevelAlbum, rowcount > 0, dialog loading succeeds,
    // opened item is TMPXItemId::InvalidId()
    VideoProxyModelData::mOpenedItemId = TMPXItemId::InvalidId();
    mTestObject->removeVideosFromCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    
    // has model, level is ELevelAlbum, rowcount > 0, dialog loading succeeds,
    // opened items iId2 != KVcxMvcMediaTypeAlbum
    VideoProxyModelData::mOpenedItemId = TMPXItemId(1,KVcxMvcMediaTypeCategory);
    mTestObject->removeVideosFromCollectionSlot();
    QVERIFY(!VideoListSelectionDialogData::mMultiSelectionLaunchCount);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    
    // succeed case
    VideoProxyModelData::mOpenedItemId = TMPXItemId(1,KVcxMvcMediaTypeAlbum);
    mTestObject->removeVideosFromCollectionSlot();
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionLaunchCount == 1);
    QVERIFY(VideoListSelectionDialogData::mSelectionType == VideoListSelectionDialog::ERemoveFromCollection);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoProxyModelData::mOpenedItemId );
    
}
