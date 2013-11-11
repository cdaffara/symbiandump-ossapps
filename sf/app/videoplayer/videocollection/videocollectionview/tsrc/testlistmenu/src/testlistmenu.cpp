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

// Version : %version: 1 %


#include <qdebug.h>
#include <xqserviceutil.h>
#include <QtTest/QtTest>
#include "hbapplication.h"
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
#include <videocollectioncommon.h>
#include <qhash.h>
#include <qmap.h>

#include "testlistmenu.h"

#define private public
#include "videolistmenu.h"
#include "videolistwidget.h"
#include "videolistview.h"
#include "videoproxymodelgeneric.h"
#undef private

#include "videolistwidgetdata.h"
#include "videocollectionuiloaderdata.h"
#include "videoproxymodeldata.h"
#include "videolistselectiondialog.h"
#include "videolistselectiondialogdata.h"
#include "videocollectionviewutils.h"
#include "videocollectionviewutilsdata.h"
#include "videocollectioncommon.h"
#include "videocollectionuiloader.h"
#include "videolistdatamodel.h"
#include "videolistdatamodeldata.h"
#include "videocollectiontrace.h"

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int	main(int argc, char	*argv[])
{
	HbApplication app(argc,	argv);
	HbMainWindow window;
	hbInstance->mWindowses.append(&window);

	TestListMenu tv;

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
    	pass[2]	= "c:\\data\\testlistmenu.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

	return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestListMenu::init()
{
    mUiLoader = new VideoCollectionUiLoader();
    mUiLoader->reset();

    bool ok(false);
    mUiLoader->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    if (!ok)
    {
        return;
    }

    mListView = mUiLoader->findWidget<VideoListView>( DOCML_NAME_VIEW );
    QVERIFY(mListView);
    
    mDataModel = new VideoListDataModel();
    
    mModel = new VideoProxyModelGeneric();
    mModel->setSourceModel(mDataModel);
    
    mListWidget = new VideoListWidget(mUiLoader, 0);
    mListWidget->initialize(*mModel, false, VideoCollectionCommon::ELevelVideos);
    
    mListView->mCurrentList = mListWidget;

    mTestObject = new VideoListMenu(mUiLoader, mListView);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestListMenu::cleanup()
{
    delete mUiLoader;
    mUiLoader = 0;
    
    delete mListWidget;
    mListWidget = 0;
    
    delete mModel;
    mModel = 0;
    
    mListView = 0;

    disconnect();
    
    VideoListWidgetData::reset();
    VideoProxyModelData::reset();
    VideoCollectionViewUtilsData::reset();
    HbDocumentLoader::cleanup();
}

// ---------------------------------------------------------------------------
// testInitializeMenu
// ---------------------------------------------------------------------------
//
void TestListMenu::testInitializeMenu()
{
    // Good case
    init();
    QVERIFY(mTestObject->initializeMenu() == 0);
    QVERIFY(VideoCollectionUiLoaderData::mAddDataCallCount == 1);
    cleanup();

    // Test null ui loader.
    mTestObject = new VideoListMenu(0, 0);
    QVERIFY(mTestObject->initializeMenu() == -1);
    QVERIFY(VideoCollectionUiLoaderData::mAddDataCallCount == 0);
    cleanup();
    
    // Is service
    init();
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    QVERIFY(mTestObject->initializeMenu() == 0);
    QVERIFY(VideoCollectionUiLoaderData::mAddDataCallCount == 1);
    // Call again, now mVideoServices is set already.
    QVERIFY(mTestObject->initializeMenu() == 0);
    QVERIFY(VideoCollectionUiLoaderData::mAddDataCallCount == 2);
    cleanup();
}

// ---------------------------------------------------------------------------
// testInitializeMenu
// ---------------------------------------------------------------------------
//
void TestListMenu::testStartSorting()
{
    HbAction *action(0);
    HbMenu *sortMenu(0);
    HbMenu *optionsMenu(0);
    HbAction *sortMenuAction = new HbAction("test");
    
    init();
    
    // No current list.
    connect(this, SIGNAL(testSignal()), mTestObject, SLOT(startSorting()));
    mListView->mCurrentList = 0;
    emit testSignal();
    QCOMPARE(VideoProxyModelData::mDoSortingCallCount, 0);
    mListView->mCurrentList = mListWidget;
    
    // Action is not sort menu
    action = mUiLoader->findObject<HbAction>(DOCML_NAME_DELETE_MULTIPLE);
    optionsMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
    sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    QVERIFY(action != 0);
    optionsMenu->setActiveAction(action);
    sortMenuAction->setMenu(sortMenu);
    HbMenuData::mMenuAction = sortMenuAction;
    emit testSignal();
    QCOMPARE(VideoProxyModelData::mDoSortingCallCount, 0);

    // Sort menu has no active action.
    action = mUiLoader->findObject<HbAction>(DOCML_NAME_DELETE_MULTIPLE);
    optionsMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
    sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    QVERIFY(action != 0);
    optionsMenu->setActiveAction(action);
    sortMenuAction->setMenu(sortMenu);
    HbMenuData::mMenuAction = action;
    sortMenu->setActiveAction(0);
    emit testSignal();
    QCOMPARE(VideoProxyModelData::mDoSortingCallCount, 0);

    // Good cases
    
    cleanup();
    
    TMPXItemId invalidId = TMPXItemId::InvalidId();
    
    init();
    mListView->activateView(invalidId);
    sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    sortMenuAction->setMenu(sortMenu);
    HbMenuData::mMenuAction = sortMenuAction;
    VideoProxyModelGeneric* model = mListView->mCurrentList->getModel();
    QVERIFY(action != 0);
    mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(sortMenuAction);
    HbAction *sortAction = mUiLoader->findObject<HbAction>(DOCML_NAME_SORT_BY_NAME);
    QVERIFY(sortAction != 0);
    sortMenu->setActiveAction(sortAction);
    model->setSortRole(1);
    connect(this, SIGNAL(testObjectReadySignal(QObject*, const QString)), mTestObject, SLOT(objectReadySlot(QObject*, const QString)));
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_DATE), DOCML_NAME_SORT_BY_DATE);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_NAME), DOCML_NAME_SORT_BY_NAME);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS), DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_SIZE), DOCML_NAME_SORT_BY_SIZE);
    disconnect(this, SIGNAL(testObjectReadySignal(QObject*, const QString)), mTestObject, SLOT(objectReadySlot(QObject*, const QString)));

    connect(this, SIGNAL(testSignal()), mTestObject, SLOT(startSorting()));
    
    // No model.
    mListView->mCurrentList->mModel = 0;
    emit testSignal();
    QCOMPARE(VideoProxyModelData::mDoSortingCallCount, 0);
    
    // First ok case. 
    mListView->mCurrentList->mModel = mModel;
    emit testSignal();
    
    // since current list is videolist, sorting count is 2 because both all videos and collection content are sorted
    QCOMPARE(VideoProxyModelData::mDoSortingCallCount, 2);
    QCOMPARE(VideoProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(VideoProxyModelData::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoProxyModelData::mSortAsync);

    VideoProxyModelData::mDoSortingCallCount = 0;
    // emit test signal again, with same parameters. Sorting should be switched to
    // descending order.
    emit testSignal();
    // since current list is videolist, sorting count is 2 because both all videos and collection content are sorted
    QCOMPARE(VideoProxyModelData::mDoSortingCallCount, 2);
    QCOMPARE(VideoProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(VideoProxyModelData::mSortOrder, Qt::DescendingOrder);
    QVERIFY(VideoProxyModelData::mSortAsync);

    VideoProxyModelData::mDoSortingCallCount = 0;
    // on third emit, sorting should be switched back to ascending
    emit testSignal();
    // since current list is videolist, sorting count is 2 because both all videos and collection content are sorted
    QCOMPARE(VideoProxyModelData::mDoSortingCallCount, 2);
    QCOMPARE(VideoProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(VideoProxyModelData::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoProxyModelData::mSortAsync);

    // test that after changing the sort role, the order is also switched to ascending.
    model->doSorting(model->sortRole(), Qt::DescendingOrder);
    sortAction = mUiLoader->findObject<HbAction>(DOCML_NAME_SORT_BY_DATE);
    QVERIFY(sortAction != 0);
    sortMenu->setActiveAction(sortAction);
    VideoProxyModelData::mDoSortingCallCount = 0;
    emit testSignal();
    // since current list is videolist, sorting count is 2 because both all videos and collection content are sorted
    QCOMPARE(VideoProxyModelData::mDoSortingCallCount, 2);
    QCOMPARE(VideoProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(VideoProxyModelData::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoProxyModelData::mSortAsync);
    cleanup();
    
    // start sorting slot:
    // -is service
    init();
    connect(this, SIGNAL(testSignal()), mTestObject, SLOT(startSorting()));
    sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    sortMenuAction->setMenu(sortMenu);
    HbMenuData::mMenuAction = sortMenuAction;
    mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(sortMenuAction);
    sortAction = mUiLoader->findObject<HbAction>(DOCML_NAME_SORT_BY_NAME);
    QVERIFY(sortAction != 0);
    sortMenu->setActiveAction(sortAction);
    mTestObject->mVideoServices = VideoServices::instance();
    int sortRole = VideoCollectionViewUtilsData::mVideoSortRole;
    emit testSignal();
    QCOMPARE(sortRole, VideoCollectionViewUtilsData::mVideoSortRole);
    cleanup();    
}

/**
 * Tests testDeleteItemsSlot
 */
void TestListMenu::testDeleteItemsSlot()
{
    init();
    connect( this, SIGNAL(testSignal()), mTestObject, SLOT(deleteItemsSlot()) );
    
    // dialog loading fails
    VideoCollectionUiLoaderData::mFailDialogLoad = true;
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 0);

    // selection dialog exists
    VideoCollectionUiLoaderData::mFailDialogLoad = false;
    VideoProxyModelData::mOpenedItemId = TMPXItemId(2, 3);
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 1);
    QCOMPARE(VideoListSelectionDialogData::mSelectionType, (int)VideoListSelectionDialog::EDeleteVideos);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoProxyModelData::mOpenedItemId);
    
    // test second successful activation.
    VideoProxyModelData::mOpenedItemId = TMPXItemId(3, 2);
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 2);
    QCOMPARE(VideoListSelectionDialogData::mSelectionType, (int)VideoListSelectionDialog::EDeleteVideos);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoProxyModelData::mOpenedItemId);

    cleanup();
}

// ---------------------------------------------------------------------------
// testCreateCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListMenu::testCreateCollectionSlot()
{
    init();
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestObject, SLOT(createCollectionSlot())));
  
    // dialog finding fails
    VideoCollectionUiLoaderData::mFailDialogLoad = true;
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 0);
    QCOMPARE(VideoListSelectionDialogData::mSelectionType, -1);
    // dialog finding succeeds
    VideoCollectionUiLoaderData::mFailDialogLoad = false;
    emit testSignal();    
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QCOMPARE(VideoListSelectionDialogData::mSelectionType, (int)VideoListSelectionDialog::ECreateCollection);
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 1);
 
    disconnect(this, SIGNAL(testSignal()), mTestObject, SLOT(createCollectionSlot()));
    cleanup();
}

// ---------------------------------------------------------------------------
// testAddVideosToCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListMenu::testAddVideosToCollectionSlot()
{
    init();
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestObject, SLOT(addVideosToCollectionSlot())));

    // No current list.
    mListView->mCurrentList = 0;
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 0);
    mListView->mCurrentList = mListWidget;
    
    // Current list has no model.
    mListView->mCurrentList->mModel = 0;
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 0);
    mListView->mCurrentList->mModel = mModel;
    
    // dialog finding fails
    VideoCollectionUiLoaderData::mFailDialogLoad = true;
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 0);
    QCOMPARE(VideoListSelectionDialogData::mSelectionType, -1);

    // dialog finding succeeds
    VideoCollectionUiLoaderData::mFailDialogLoad = false;
    emit testSignal();    
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QCOMPARE(VideoListSelectionDialogData::mSelectionType, (int)VideoListSelectionDialog::EAddToCollection);
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 1);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testAboutToShowMainMenuSlot
// ---------------------------------------------------------------------------
//
void TestListMenu::testAboutToShowMainMenuSlot()
{
    init();
    
    VideoServices *videoServices = VideoServices::instance();

    // Setup menu objects.
    connect(this, SIGNAL(testObjectReadySignal(QObject*, const QString)), mTestObject, SLOT(objectReadySlot(QObject*, const QString)));
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_OPTIONS_MENU), DOCML_NAME_OPTIONS_MENU);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_DATE), DOCML_NAME_SORT_BY_DATE);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_NAME), DOCML_NAME_SORT_BY_NAME);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS), DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_ADD_TO_COLLECTION), DOCML_NAME_ADD_TO_COLLECTION);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_CREATE_COLLECTION), DOCML_NAME_CREATE_COLLECTION);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_DELETE_MULTIPLE), DOCML_NAME_DELETE_MULTIPLE);
    disconnect(this, SIGNAL(testObjectReadySignal(QObject*, const QString)), mTestObject, SLOT(objectReadySlot(QObject*, const QString)));
    
    // No current list.
    setVisibilityForMenuActions(false);
    VideoListDataModelData::setRowCount(*mDataModel, 5);
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestObject, SLOT(aboutToShowMainMenuSlot())));
    mListView->mCurrentList = 0;
    emit testSignal();
    int visible = visibleMenuActions();
    QCOMPARE(visible, 0);
    
    // No model, current list is category
    mListView->mCurrentList = mListWidget;
    mListWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    mListWidget->mModel = 0;
    setVisibilityForMenuActions(true);
    VideoListDataModelData::setRowCount(*mDataModel, 0);
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 1);

    // No model, current list is not category
    mListWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    mListWidget->mModel = 0;
    setVisibilityForMenuActions(true);
    VideoListDataModelData::setRowCount(*mDataModel, 0);
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 0);
    
    // Empty model.
    mListView->mCurrentList->mModel = mModel;
    setVisibilityForMenuActions(true);
    VideoListDataModelData::setRowCount(*mDataModel, 0);
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 0);
    
    VideoProxyModelData::mSortRole = VideoCollectionCommon::KeyDateTime;
    VideoProxyModelData::mSortOrder = Qt::AscendingOrder; 

    // Level videos
    VideoListDataModelData::setRowCount(*mDataModel, 10);
    mListWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->setChecked(false);
    mTestObject->mVideoServices = 0;
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 5);
    QVERIFY(mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->isChecked());

    // Level videos and is service.
    VideoListDataModelData::setRowCount(*mDataModel, 10);
    mListWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->setChecked(false);
    mTestObject->mVideoServices = videoServices;
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 3);
    QVERIFY(mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->isChecked());

    // Level category
    VideoListDataModelData::setRowCount(*mDataModel, 10);
    mListWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->setChecked(false);
    mTestObject->mVideoServices = 0;
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 3);
    QVERIFY(mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->isChecked());

    // Level ELevelDefaultColl
    VideoListDataModelData::setRowCount(*mDataModel, 10);
    mListWidget->mCurrentLevel = VideoCollectionCommon::ELevelDefaultColl;
    mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->setChecked(false);
    mTestObject->mVideoServices = 0;
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 4);
    QVERIFY(mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->isChecked());

    // Level ELevelDefaultColl and is service
    VideoListDataModelData::setRowCount(*mDataModel, 10);
    mListWidget->mCurrentLevel = VideoCollectionCommon::ELevelDefaultColl;
    mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->setChecked(false);
    mTestObject->mVideoServices = videoServices;
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 3);
    QVERIFY(mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->isChecked());    

    VideoProxyModelData::mSortRole = VideoCollectionCommon::KeyDateTime;
    VideoProxyModelData::mSortOrder = Qt::AscendingOrder; 
    // Level ELevelAlbum
    VideoListDataModelData::setRowCount(*mDataModel, 10);
    mListWidget->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
    mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->setChecked(false);
    mTestObject->mVideoServices = 0;
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 4);
    QVERIFY(mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->isChecked());
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testPrepareBrowseServiceMenu
// ---------------------------------------------------------------------------
//
void TestListMenu::testPrepareBrowseServiceMenu()
{
    init();
    
    VideoServices *videoServices = VideoServices::instance();
    videoServices->mCurrentService = VideoServices::EBrowse;
    mTestObject->mVideoServices = videoServices;

    // Setup menu objects.
    connect(this, SIGNAL(testObjectReadySignal(QObject*, const QString)), 
            mTestObject, SLOT(objectReadySlot(QObject*, const QString)));
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_OPTIONS_MENU), DOCML_NAME_OPTIONS_MENU);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_DATE), DOCML_NAME_SORT_BY_DATE);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_NAME), DOCML_NAME_SORT_BY_NAME);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS), DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_ADD_TO_COLLECTION), DOCML_NAME_ADD_TO_COLLECTION);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_CREATE_COLLECTION), DOCML_NAME_CREATE_COLLECTION);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_DELETE_MULTIPLE), DOCML_NAME_DELETE_MULTIPLE);
    
    disconnect(this, SIGNAL(testObjectReadySignal(QObject*, const QString)), mTestObject, SLOT(objectReadySlot(QObject*, const QString)));
    
    // No current list.
    setVisibilityForMenuActions(false);
    VideoListDataModelData::setRowCount(*mDataModel, 5);
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestObject, SLOT(aboutToShowMainMenuSlot())));
    mListView->mCurrentList = 0;
    emit testSignal();
    int visible = visibleMenuActions();
    QCOMPARE(visible, 0);
    
    // No model
    mListView->mCurrentList = mListWidget;
    mListWidget->mModel = 0;
    setVisibilityForMenuActions(true);
    VideoListDataModelData::setRowCount(*mDataModel, 0);
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 0);

    // Empty model.
    mListView->mCurrentList->mModel = mModel;
    setVisibilityForMenuActions(true);
    VideoListDataModelData::setRowCount(*mDataModel, 0);
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 0);
    
    VideoProxyModelData::mSortRole = VideoCollectionCommon::KeyDateTime;
    VideoProxyModelData::mSortOrder = Qt::AscendingOrder; 
    
    // Good case.
    mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->setChecked(false);
    setVisibilityForMenuActions(true);
    VideoListDataModelData::setRowCount(*mDataModel, 10);
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 4);
    QVERIFY(mTestObject->mSortingRoles.key(VideoCollectionCommon::KeyDateTime)->isChecked());
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testObjectReadySlot
// ---------------------------------------------------------------------------
//
void TestListMenu::testObjectReadySlot()
{
    init();
    
    // Ok cases have been tested earlier.
    connect(this, SIGNAL(testObjectReadySignal(QObject*, const QString)), 
            mTestObject, SLOT(objectReadySlot(QObject*, const QString)));
    
    // Null objects, qobject cast fails and sorting roles array should be empty.
    emit testObjectReadySignal(0, "notknown");
    emit testObjectReadySignal(0, DOCML_NAME_SORT_BY_DATE);
    emit testObjectReadySignal(0, DOCML_NAME_SORT_BY_NAME);
    emit testObjectReadySignal(0, DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    emit testObjectReadySignal(0, DOCML_NAME_SORT_BY_SIZE);
    emit testObjectReadySignal(0, DOCML_NAME_ADD_TO_COLLECTION);
    emit testObjectReadySignal(0, DOCML_NAME_CREATE_COLLECTION);
    emit testObjectReadySignal(0, DOCML_NAME_DELETE_MULTIPLE);
    QCOMPARE(mTestObject->mSortingRoles.count(), 0);
    
    disconnect(this, SIGNAL(testObjectReadySignal(QObject*, const QString)), mTestObject, SLOT(objectReadySlot(QObject*, const QString)));
    
    cleanup();
}

// ---------------------------------------------------------------------------
// visibleMenuActions
// ---------------------------------------------------------------------------
//
int TestListMenu::visibleMenuActions()
{
    int visibleActions(0);
    VideoCollectionUiLoader::ActionIds key;
    HbAction *action(0);
    foreach (key, VideoCollectionUiLoaderData::mMenuActions.keys())
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
// setVisibilityForMenuActions
// ---------------------------------------------------------------------------
//
void TestListMenu::setVisibilityForMenuActions(bool visible)
{
    VideoCollectionUiLoader::ActionIds key;
    HbAction *action(0);
    foreach (key, VideoCollectionUiLoaderData::mMenuActions.keys())
    {
        action = dynamic_cast<HbAction*>(VideoCollectionUiLoaderData::mMenuActions[key]);
        if (action)
        {
            action->setVisible(visible);
        }
    }
}

// End of file.
