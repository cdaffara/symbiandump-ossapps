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
* Description:   tester for methods in VideoCollectionViewUtils
* 
*/

#include <qdebug.h>
#include <qapplication.h>
#include <qhash.h>
#include "xqsettingsmanagerstub.h"
#include <vcxmyvideosdefs.h>
#include "testvideocollectionviewutils.h"
#include "hblabel.h"
#include "hbaction.h"
#include "videocollectionwrapper.h"
#include "videoproxymodeldata.h"
#include "videoactivitystate.h"
#include "videocollectioncommon.h"
#include "centralrepository.h"
#include "hbmessageboxdata.h"
#include "hbnotificationdialog.h"
#include "hblistview.h"
#include <hbactivitymanager.h>
#include <hbapplication.h>
#include <xqsettingsmanagerstub.h>

#define private public
#include "videocollectionviewutils.h"
#undef private

// following consts are copied from videocollectionviewutils.cpp
const int KVideoSortingRoleKey(0x1);
const int KVideoSortingOrderKey(0x2);
const int KCollectionsSortingRoleKey(0x3);
const int KCollectionsSortingOrderKey(0x4);
static const QString KEY_WIDGET_LEVEL    = "_VideoActivity_widget_level_";

// id of the collection whose videolist is to be shown (int).
static const QString KEY_COLLECTION_ID   = "_VideoActivity_collection_id_";

// name of the collection whose videolist is to be shown (QString)
static const QString KEY_COLLECTION_NAME = "_VideoActivity_collection_name_";

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    //HbMainWindow window;
    //QApplication app(argc, argv);
    
    TestVideoVideoCollectionViewUtils tv;

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
        pass[2] = "c:\\data\\testvideocollectionviewutils.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
        
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::init()
{
    XQSettingsManager::mReadItemSucceedCounter = 99999;
}
 
// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::cleanup()
{
    XQSettingsManager::mWrittenValueHash.clear();
}

// ---------------------------------------------------------------------------
// testShowStatusMsgSlot
// ---------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testShowStatusMsgSlot()
{
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    QVariant additional;
    QList<QVariant> dataList;
    QString txt = "__test__";      
    HbNotificationDialog::mNotifDialogTitle = "";
    HbNotificationDialog::mTitleTextWRapping = Hb::TextNoWrap;
    HbNotificationDialog::mAttribute = Qt::WA_Disabled;
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;
    
    // status: VideoCollectionCommon::statusSingleDeleteFail
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;
    HbMessageBoxData::mInformationCallCount = 0;
    
    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;
    
    // status: VideoCollectionCommon::statusMultipleDeleteFail
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultipleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;

    // - invalid additional (no change, since additional not excepted)
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultipleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;
    
    // status: VideoCollectionCommon::statusMultipleRemoveFail (additional not needed)
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultiRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;
    
    // status: VideoCollectionCommon::statusVideosAddedToCollection
    // notification dialog shown
    dataList.clear();
    dataList.append(QVariant(1)); // count
    dataList.append(QVariant(txt)); // name
    additional = dataList;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() > 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextWordWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_DeleteOnClose);
    HbNotificationDialog::mNotifDialogTitle = "";
    HbNotificationDialog::mTitleTextWRapping = Hb::TextNoWrap;
    HbNotificationDialog::mAttribute = Qt::WA_Disabled;
        
    // - too few additional
    dataList.clear();
    dataList.append(QVariant(txt)); // name
    additional = dataList;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    
    // - empty message
    dataList.clear();
    dataList.append(QVariant(1)); // count
    dataList.append(QVariant("")); // name
    additional = dataList;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    
    // - with zero video count
    dataList.clear();
    dataList.append(QVariant(0)); // count
    dataList.append(QVariant(txt)); // name
    additional = dataList;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);    

    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    
    // status: VideoCollectionCommon::statusAllVideosAlreadyInCollection
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusAllVideosAlreadyInCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mShowCallCount = 0;
    HbMessageBoxData::mType = -1;
    
    // status: VideoCollectionCommon::statusDeleteInProgress
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusDeleteInProgress, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() > 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextWordWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_DeleteOnClose);
    HbNotificationDialog::mNotifDialogTitle = "";
    HbNotificationDialog::mTitleTextWRapping = Hb::TextNoWrap;
    HbNotificationDialog::mAttribute = Qt::WA_Disabled;

    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusDeleteInProgress, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    
    // status: VideoCollectionCommon::statusSingleRemoveFail
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mShowCallCount = 0;
    HbMessageBoxData::mType = -1;
    
    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    
    // status: statusMultiRemoveFail
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultiRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mShowCallCount = 0;
    HbMessageBoxData::mType = -1;
    
    // invalid additional (no change, since additional not excepted)
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultiRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mShowCallCount = 0;
    HbMessageBoxData::mType = -1;
    
    // invalid state
    additional = txt;
    testObject.showStatusMsgSlot(0, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
}   

// -----------------------------------------------------------------------------
// testSaveSortingValues
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testSaveSortingValues()
{
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    int sortRole = VideoCollectionCommon::KeyDateTime;
    Qt::SortOrder sortOrder = Qt::AscendingOrder;
    VideoCollectionCommon::TCollectionLevels target = VideoCollectionCommon::ELevelInvalid;
    XQSettingsManager::mWrittenValueHash.clear();
    
    // target invalid
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(XQSettingsManager::mWrittenValueHash.count() == 0);
    
    // target == VideoCollectionCommon::ELevelCategory
    target = VideoCollectionCommon::ELevelCategory;    
    
    // first writing fails 
    XQSettingsManager::mWriteItemSucceedCounter = 0;
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(XQSettingsManager::mWrittenValueHash.count() == 0);

    // second writing fails (writing of sortrole has succeed)
    XQSettingsManager::mWriteItemSucceedCounter = 1;
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(XQSettingsManager::mWrittenValueHash.count() == 1);
    QVERIFY(XQSettingsManager::mWrittenValueHash.value(KCollectionsSortingRoleKey).toInt() == sortRole);
    
    XQSettingsManager::mWrittenValueHash.clear();

    // succeed
    XQSettingsManager::mWriteItemSucceedCounter = 2;
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(XQSettingsManager::mWrittenValueHash.count() == 2);
    QVERIFY(XQSettingsManager::mWrittenValueHash.value(KCollectionsSortingRoleKey).toInt() == sortRole);
    QVERIFY(XQSettingsManager::mWrittenValueHash.value(KVideoSortingOrderKey).toInt() == Qt::AscendingOrder);
    
    // target != VideoCollectionCommon::ELevelCategory
    XQSettingsManager::mWrittenValueHash.clear();
    target = VideoCollectionCommon::ELevelVideos;    
    
    // writing fails
    XQSettingsManager::mWriteItemSucceedCounter = 0;
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(XQSettingsManager::mWrittenValueHash.count() == 0);
    
    // second writing fails (writing of sortrole has succeed)
    XQSettingsManager::mWriteItemSucceedCounter = 1;
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(XQSettingsManager::mWrittenValueHash.count() == 1);
    QVERIFY(XQSettingsManager::mWrittenValueHash.value(KVideoSortingRoleKey).toInt() == sortRole);

    XQSettingsManager::mWrittenValueHash.clear();
    // succeed
    XQSettingsManager::mWriteItemSucceedCounter = 2;
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(XQSettingsManager::mWrittenValueHash.count() == 2);
    QVERIFY(XQSettingsManager::mWrittenValueHash.value(KVideoSortingRoleKey).toInt() == sortRole);
    QVERIFY(XQSettingsManager::mWrittenValueHash.value(KVideoSortingOrderKey).toInt() == Qt::AscendingOrder);
}
 
// -----------------------------------------------------------------------------
// testLoadSortingValues
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testLoadSortingValues()
{

    XQSettingsManager::mWrittenValueHash.insert(KVideoSortingRoleKey, VideoCollectionCommon::KeyDateTime);
    XQSettingsManager::mWrittenValueHash.insert(KVideoSortingOrderKey, Qt::DescendingOrder);
    XQSettingsManager::mWrittenValueHash.insert(KCollectionsSortingRoleKey, VideoCollectionCommon::KeyTitle);
    XQSettingsManager::mWrittenValueHash.insert(KCollectionsSortingOrderKey, Qt::DescendingOrder);
    
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    
    int sortRole(-1);
    Qt::SortOrder sortOrder(Qt::AscendingOrder);   
    VideoCollectionCommon::TCollectionLevels target = VideoCollectionCommon::ELevelInvalid;
    
    testObject.mVideosSortRole = -1;
    testObject.mVideosSortOrder = Qt::AscendingOrder;
    testObject.mCollectionsSortRole = -1;
    testObject.mCollectionsSortOrder = Qt::AscendingOrder;
    
    // target is invalid
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    
    // target is VideoCollectionCommon::ELevelCategory
    target = VideoCollectionCommon::ELevelCategory;
    
    // first read fails
    XQSettingsManager::mReadItemSucceedCounter = 0;
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);

    
    // second read fails
    XQSettingsManager::mReadItemSucceedCounter = 1;
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    
    
    // succeeds
    XQSettingsManager::mReadItemSucceedCounter = 2;
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(sortOrder == Qt::DescendingOrder);
    sortRole = -1;
    sortOrder = Qt::AscendingOrder;

    // another call used previously saved values
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(sortOrder == Qt::DescendingOrder);
    sortRole = -1;
    sortOrder = Qt::AscendingOrder;
    
    // target is different than VideoCollectionCommon::ELevelCategory
    target = VideoCollectionCommon::ELevelVideos;
       
    // first read fails
    XQSettingsManager::mReadItemSucceedCounter = 0;
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    
    // second read fails
    XQSettingsManager::mReadItemSucceedCounter = 1;
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    
    
    // succeeds
    XQSettingsManager::mReadItemSucceedCounter = 2;
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(testObject.mVideosSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(testObject.mVideosSortOrder == Qt::DescendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(sortOrder == Qt::DescendingOrder);
    
    sortRole = -1;
    sortOrder = Qt::AscendingOrder;
    
    // another call used previously saved values
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(testObject.mVideosSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(testObject.mVideosSortOrder == Qt::DescendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(sortOrder == Qt::DescendingOrder);

}

// -----------------------------------------------------------------------------
// testGetCenRepStringValue
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testGetCenRepStringValue()
{  
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());

    // no data
    XQSettingsManager::mWrittenValueHash.clear();
    QVERIFY(testObject.getCenRepStringValue(0) == "");
    
    // Ok
    XQSettingsManager::mWrittenValueHash.insert(0, QVariant("test"));
    QVERIFY(testObject.getCenRepStringValue(0) == "test");
}

// -----------------------------------------------------------------------------
// testGetCenRepIntValue
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testGetCenRepIntValue()
{   
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());

    // no data
    XQSettingsManager::mWrittenValueHash.clear();
    QVERIFY(testObject.getCenRepIntValue(0) == -1);
    
    // Ok
    XQSettingsManager::mWrittenValueHash.insert(0, QVariant(13));
    QVERIFY(testObject.getCenRepIntValue(0) == 13);
}

// -----------------------------------------------------------------------------
// testInitListView
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testInitListView()
{    
    // empty view
    VideoCollectionViewUtils::initListView(0);
    QVERIFY(HbListView::mLatestrecycling == false);
    QVERIFY(HbListView::mLatestClamping == HbScrollArea::InvalidClamping);
    QVERIFY(HbListView::mLatestScrolling == HbScrollArea::InvalidScrolling);
    QVERIFY(HbListView::mLatestFrictionEnabled == false);
    QVERIFY(HbListView::mLatestUniformItemSizes == false);
    QVERIFY(HbListView::mSelectionMode == HbAbstractItemView::InvalidSelection);
    
    HbListView view;
    VideoCollectionViewUtils::initListView(&view);
    QVERIFY(HbListView::mLatestrecycling == true);
    QVERIFY(HbListView::mLatestClamping == HbScrollArea::BounceBackClamping);
    QVERIFY(HbListView::mLatestScrolling == HbScrollArea::PanWithFollowOn);
    QVERIFY(HbListView::mLatestFrictionEnabled == true);
    QVERIFY(HbListView::mLatestUniformItemSizes == true);
    QVERIFY(HbListView::mSelectionMode == HbAbstractItemView::NoSelection);
    
    HbListView::mLatestrecycling = false;
    HbListView::mLatestClamping = HbScrollArea::InvalidClamping;
    HbListView::mLatestScrolling = HbScrollArea::InvalidScrolling;
    HbListView::mLatestFrictionEnabled = false;
    HbListView::mLatestUniformItemSizes = false;
    HbListView::mSelectionMode = HbAbstractItemView::InvalidSelection;
    
    // no prototype from view (for coverity)
    HbListView::mReturnNullPrototype = true;
    VideoCollectionViewUtils::initListView(&view);
    QVERIFY(HbListView::mLatestrecycling == true);
    QVERIFY(HbListView::mLatestClamping == HbScrollArea::BounceBackClamping);
    QVERIFY(HbListView::mLatestScrolling == HbScrollArea::PanWithFollowOn);
    QVERIFY(HbListView::mLatestFrictionEnabled == true);
    QVERIFY(HbListView::mLatestUniformItemSizes == true);
    QVERIFY(HbListView::mSelectionMode == HbAbstractItemView::NoSelection);
    
    HbListView::mLatestrecycling = false;
    HbListView::mLatestClamping = HbScrollArea::InvalidClamping;
    HbListView::mLatestScrolling = HbScrollArea::InvalidScrolling;
    HbListView::mLatestFrictionEnabled = false;
    HbListView::mLatestUniformItemSizes = false;
    HbListView::mSelectionMode = HbAbstractItemView::InvalidSelection;
    HbListView::mReturnNullPrototype = false;
    
    // no vertical scrollbar from view (for coverity)
    HbListView::mVScrollBarIsNull = true;
    VideoCollectionViewUtils::initListView(&view);
    QVERIFY(HbListView::mLatestrecycling == true);
    QVERIFY(HbListView::mLatestClamping == HbScrollArea::BounceBackClamping);
    QVERIFY(HbListView::mLatestScrolling == HbScrollArea::PanWithFollowOn);
    QVERIFY(HbListView::mLatestFrictionEnabled == true);
    QVERIFY(HbListView::mLatestUniformItemSizes == true);
    QVERIFY(HbListView::mSelectionMode == HbAbstractItemView::NoSelection);

}

// -----------------------------------------------------------------------------
// testSortModel
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testSortModel()
{

    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    testObject.mVideosSortRole = -1;
    testObject.mVideosSortOrder = Qt::AscendingOrder;
    testObject.mCollectionsSortRole = -1;
    testObject.mCollectionsSortOrder = Qt::AscendingOrder;
    
    XQSettingsManager::mWrittenValueHash.insert(KVideoSortingRoleKey, VideoCollectionCommon::KeyDateTime);
    XQSettingsManager::mWrittenValueHash.insert(KVideoSortingOrderKey, Qt::DescendingOrder);
    XQSettingsManager::mWrittenValueHash.insert(KCollectionsSortingRoleKey, VideoCollectionCommon::KeyTitle);
    XQSettingsManager::mWrittenValueHash.insert(KCollectionsSortingOrderKey, Qt::DescendingOrder);
    
    VideoCollectionCommon::TCollectionLevels target = VideoCollectionCommon::ELevelInvalid;
    
    // null model
    VideoCollectionViewUtils::sortModel(0, false, target);
    
    VideoProxyModelGeneric *model = 
        VideoCollectionWrapper::instance().getGenericModel();
    
    // target == invalid -> default values used as sort -call 
    VideoCollectionViewUtils::sortModel(model, false, target);
    
    QVERIFY(VideoProxyModelData::mDoSortingCallCount == 1);
    QVERIFY(VideoProxyModelData::mSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(VideoProxyModelData::mSortOrder == Qt::AscendingOrder);
    QVERIFY(VideoProxyModelData::mSortAsync == false); 
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    
    VideoProxyModelData::mDoSortingCallCount = 0;
    VideoProxyModelData::mSortRole = -1;
    VideoProxyModelData::mSortOrder = Qt::AscendingOrder;
    VideoProxyModelData::mSortAsync = true;
    
    // target == VideoCollectionCommon::ELevelVideos
    target = VideoCollectionCommon::ELevelVideos;
    VideoCollectionViewUtils::sortModel(model, false, target);
    
    QVERIFY(VideoProxyModelData::mDoSortingCallCount == 1);
    QVERIFY(VideoProxyModelData::mSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(VideoProxyModelData::mSortOrder == Qt::DescendingOrder);
    QVERIFY(VideoProxyModelData::mSortAsync == false); 
    QVERIFY(testObject.mVideosSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(testObject.mVideosSortOrder == Qt::DescendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    
    
    VideoProxyModelData::mDoSortingCallCount = 0;
    VideoProxyModelData::mSortRole = -1;
    VideoProxyModelData::mSortOrder = Qt::AscendingOrder;
    VideoProxyModelData::mSortAsync = true;
       
    // target == VideoCollectionCommon::ELevelCategory
    target = VideoCollectionCommon::ELevelCategory;
    VideoCollectionViewUtils::sortModel(model, false, target);
    
    QVERIFY(VideoProxyModelData::mDoSortingCallCount == 1);
    QVERIFY(VideoProxyModelData::mSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(VideoProxyModelData::mSortOrder == Qt::DescendingOrder);
    QVERIFY(VideoProxyModelData::mSortAsync == false); 
    QVERIFY(testObject.mVideosSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(testObject.mVideosSortOrder == Qt::DescendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
}

// -----------------------------------------------------------------------------
// testSetAndGetWidgetLevel
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testSetAndGetWidgetLevel()
{
    static const char* ACTIVITY_VIDEOS_MAINVIEW = "VideosMainView";
    
    VideoCollectionCommon::TCollectionLevels level;
    
    // Invalid level
    level = VideoCollectionCommon::ELevelInvalid;
    VideoCollectionViewUtils::setWidgetActivityLevel(level);
    VideoCollectionViewUtils::getActivityWidgetLevel( level);
    QVERIFY(level == VideoCollectionCommon::ELevelVideos);
    
    // Category level.
    level = VideoCollectionCommon::ELevelCategory;
    VideoCollectionViewUtils::setWidgetActivityLevel(level);
    VideoCollectionViewUtils::getActivityWidgetLevel( level);
    QVERIFY(level == VideoCollectionCommon::ELevelCategory);
    
    // make sure unneeded data is cleared
    TMPXItemId id = TMPXItemId(1,1);
    QString name = "name";
    VideoCollectionViewUtils::setCollectionActivityData(id, name);
    level = VideoCollectionCommon::ELevelVideos;
    VideoCollectionViewUtils::setWidgetActivityLevel(level);

    QVERIFY(level == VideoCollectionCommon::ELevelVideos);
    QVariant data = VideoActivityState::instance().getActivityData(KEY_COLLECTION_ID);
    QVERIFY(data.toInt() == 0);
    data = VideoActivityState::instance().getActivityData(KEY_COLLECTION_NAME);
    QVERIFY(data.toString().isEmpty());
    
    id = TMPXItemId(1,1);
    name = "name";
    VideoCollectionViewUtils::setCollectionActivityData(id, name);
    level = VideoCollectionCommon::ELevelAlbum;
    VideoCollectionViewUtils::setWidgetActivityLevel(level);

    QVERIFY(level == VideoCollectionCommon::ELevelAlbum);
    data = VideoActivityState::instance().getActivityData(KEY_COLLECTION_ID);
    QVERIFY(data.toInt() == 1);
    data = VideoActivityState::instance().getActivityData(KEY_COLLECTION_NAME);
    QVERIFY(data.toString() == "name");

}

void TestVideoVideoCollectionViewUtils::testSetAndGetCollectionActivityData()
{
    TMPXItemId id = TMPXItemId(1,1);
    QString name = "name";   
    
    // no default collection nor user defined collection
    VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelVideos;
    VideoCollectionViewUtils::setWidgetActivityLevel(level); 
    VideoCollectionViewUtils::setCollectionActivityData(id, name);
    VideoCollectionViewUtils::getCollectionActivityData(id, name);
    QVERIFY(id == TMPXItemId::InvalidId());
    QVERIFY(name.isEmpty());
    
    // default collection
    level = VideoCollectionCommon::ELevelDefaultColl;
    id = TMPXItemId(1,1);
    name = "name"; 
    VideoCollectionViewUtils::setWidgetActivityLevel(level); 
    VideoCollectionViewUtils::setCollectionActivityData(id, name);
    VideoCollectionViewUtils::getCollectionActivityData(id, name);
   
    QVERIFY(id == TMPXItemId(1, KVcxMvcMediaTypeCategory));
    QVERIFY(name == "name");
   
    // user defined collection
    level = VideoCollectionCommon::ELevelAlbum;
    id = TMPXItemId(1,1);
    name = "name"; 
    VideoCollectionViewUtils::setWidgetActivityLevel(level); 
    VideoCollectionViewUtils::setCollectionActivityData(id, name);
    VideoCollectionViewUtils::getCollectionActivityData(id, name);
    
    QVERIFY(id == TMPXItemId(1, KVcxMvcMediaTypeAlbum));
    QVERIFY(name == "name");
   
}

// End of file
