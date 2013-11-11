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
* Description: video collection view plugin's ui utils class
*
*/

// Version : %version: 52 %

// INCLUDE FILES
#include <hbglobal.h>
#include <hblistview.h>
#include <hbscrollbar.h>
#include <hblistviewitem.h>
#include <hbdialog.h>
#include <hbmessagebox.h>
#include <hbnotificationdialog.h>
#include <hbparameterlengthlimiter.h>
#include <xqsettingsmanager.h>
#include <xqserviceutil.h>
#include <vcxmyvideosdefs.h>

#include "videocollectioncommon.h"
#include "videocollectionviewutils.h"
#include "videoproxymodelgeneric.h"
#include "videoactivitystate.h"
#include "videocollectioncenrepdefs.h"
#include "videocollectiontrace.h"

// Object names.
const char* const VIEW_UTILS_OBJECT_NAME_STATUS_MSG          = "vc:ViewUtilsStatusMessage";
const char* const VIEW_UTILS_OBJECT_NAME_MESSAGE_BOX_WARNING = "vc:ViewUtilsMessageBoxWarning";

const int KAddToCollectionDataCount(2);
const int KAddToCollectionCountIndex(0);
const int KAddToCollectionNameIndex(1);

// type of collectionview plugin's widget level(int): 
// all videos, collections or collection video list
static const QString KEY_WIDGET_LEVEL    = "_VideoActivity_widget_level_";

// id of the collection whose videolist is to be shown (int).
static const QString KEY_COLLECTION_ID   = "_VideoActivity_collection_id_";

// name of the collection whose videolist is to be shown (QString)
static const QString KEY_COLLECTION_NAME = "_VideoActivity_collection_name_";


// ---------------------------------------------------------------------------
// instance
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils& VideoCollectionViewUtils::instance()
{
    FUNC_LOG;
    static VideoCollectionViewUtils _popupInstance;
    return _popupInstance;
}

// ---------------------------------------------------------------------------
// VideoCollectionViewUtils
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils::VideoCollectionViewUtils()
    : mIsService(false)
    , mVideosSortRole(-1)
    , mCollectionsSortRole(-1)
    , mVideosSortOrder(Qt::AscendingOrder)
    , mCollectionsSortOrder(Qt::AscendingOrder)
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// ~VideoCollectionViewUtils
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils::~VideoCollectionViewUtils()
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// setIsService
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::setIsService()
{
    FUNC_LOG;

    mIsService = XQServiceUtil::isService();
    
    INFO_1("VideoCollectionViewUtils::setService() saving: is service: %d", mIsService);
}

// ---------------------------------------------------------------------------
// isService
// ---------------------------------------------------------------------------
//
bool VideoCollectionViewUtils::isService()
{
    FUNC_LOG;

    return mIsService;
}

// ---------------------------------------------------------------------------
// saveSortingValues
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::saveSortingValues(int role, Qt::SortOrder order, VideoCollectionCommon::TCollectionLevels target)
{
	FUNC_LOG;
	INFO_3("VideoCollectionViewUtils::saveSortingValues() saving: role: %d, order: %d, target: %d", role, order, target);

    int status = -1;
    if (target != VideoCollectionCommon::ELevelInvalid)
	{
		int *rolePtr            = &mVideosSortRole;
		Qt::SortOrder *orderPtr = &mVideosSortOrder;

		int roleKey(KVideoCollectionViewCenrepVideoSortingRoleKey);
		int orderKey(KVideoCollectionViewCenrepVideoSortingOrderKey);

		if(target == VideoCollectionCommon::ELevelCategory)
	    {
	    	rolePtr  = &mCollectionsSortRole;
	    	orderPtr = &mCollectionsSortOrder;

	    	roleKey  = KVideoCollectionViewCenrepCollectionsSortingRoleKey;
			orderKey = KVideoCollectionViewCenrepCollectionsSortingOrderKey;
	    }
		XQSettingsManager mgr;
		XQCentralRepositorySettingsKey crRoleKey(KVideoCollectionViewCenrepUid, roleKey);
		XQCentralRepositorySettingsKey crOrderKey(KVideoCollectionViewCenrepUid, orderKey);
		QVariant value;
		value = role;
		if(!mgr.writeItemValue(crRoleKey, value))
		{
		    return -1;
		}
		value = order;
		if(!mgr.writeItemValue(crOrderKey, value))
        {
            return -1;
        }
	    *rolePtr = role;
	    *orderPtr = order;
	    status = 0;
	}
    return status;
}

// ---------------------------------------------------------------------------
// loadSortingValues
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::loadSortingValues(int &role, Qt::SortOrder &order, VideoCollectionCommon::TCollectionLevels target)
{
	FUNC_LOG;

    if ((target > VideoCollectionCommon::ELevelCategory) &&
		(mVideosSortRole != -1))
    {
    	role = mVideosSortRole;
    	order = mVideosSortOrder;
    }
    else if ((target == VideoCollectionCommon::ELevelCategory) &&
			 (mCollectionsSortRole != -1))
    {
    	role = mCollectionsSortRole;
    	order = mCollectionsSortOrder;
    }
    else if (target != VideoCollectionCommon::ELevelInvalid)
    {
        int roleKey(KVideoCollectionViewCenrepVideoSortingRoleKey);
        int orderKey(KVideoCollectionViewCenrepVideoSortingOrderKey);
    	
        int *rolePtr             = &mVideosSortRole;
    	Qt::SortOrder *orderPtr  = &mVideosSortOrder;
    	    	
    	if(target == VideoCollectionCommon::ELevelCategory)
        {
    		roleKey  = KVideoCollectionViewCenrepCollectionsSortingRoleKey;
    		orderKey = KVideoCollectionViewCenrepCollectionsSortingOrderKey;
        	rolePtr  = &mCollectionsSortRole;
        	orderPtr = &mCollectionsSortOrder;
        }

    	*rolePtr       = getCenRepIntValue(roleKey);
    	int orderValue = getCenRepIntValue(orderKey);

    	if(*rolePtr < 0 || (orderValue < Qt::AscendingOrder || orderValue > Qt::DescendingOrder))
    	{
    	    *rolePtr  = -1;
    	    return -1;
    	}
    	orderValue == Qt::AscendingOrder ? 
    	        *orderPtr = Qt::AscendingOrder : *orderPtr = Qt::DescendingOrder;
    	        
		role  = *rolePtr;
		order = *orderPtr;
    }
    INFO_3("VideoCollectionViewUtils::loadSortingValues() loaded: role: %d, order: %d, target: %d", role, order, target);
    return 0;
}

// ---------------------------------------------------------------------------
// getCenrepStringValue
// ---------------------------------------------------------------------------
//
QString VideoCollectionViewUtils::getCenRepStringValue(int key)
{
    FUNC_LOG;
    XQCentralRepositorySettingsKey crKey(KVideoCollectionViewCenrepUid, key);
    XQSettingsManager mgr;
    QVariant value = mgr.readItemValue(crKey, XQSettingsManager::TypeString);
    return value.toString();
}

// ---------------------------------------------------------------------------
// getCenRepIntValue
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::getCenRepIntValue(int key)
{
    FUNC_LOG;
    XQCentralRepositorySettingsKey crKey(KVideoCollectionViewCenrepUid, key);
    XQSettingsManager mgr;
    QVariant value = mgr.readItemValue(crKey, XQSettingsManager::TypeInt);
    int uid = -1;
    if(value.isValid())
    {
        uid = value.toInt();
    }
    return uid;
}

// ---------------------------------------------------------------------------
// initListView
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::initListView(HbListView *view)
{
	FUNC_LOG;
    if (view)
    {
        HbListViewItem *prototype = view->listItemPrototype();
        if(prototype)
        {
            //Use image layout in prototype
            prototype->setGraphicsSize(HbListViewItem::WideThumbnail);
        }
        view->setItemRecycling(true);
        view->setClampingStyle(HbScrollArea::BounceBackClamping);
        view->setScrollingStyle(HbScrollArea::PanWithFollowOn);
        view->setFrictionEnabled(true);
        view->setUniformItemSizes(true);  
        view->setSelectionMode(HbAbstractItemView::NoSelection);
        
        //Use scrollbar
        HbScrollBar *scrollBar = view->verticalScrollBar();
        if (scrollBar)
        {
            scrollBar->setInteractive(true);
        }
    }
}

// ---------------------------------------------------------------------------
// sortModel
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::sortModel(
    VideoProxyModelGeneric *model,
    bool async,
    VideoCollectionCommon::TCollectionLevels target)
{
	FUNC_LOG;
    if (model)
    {
        // setup sorting order for model
        int sortRole(VideoCollectionCommon::KeyDateTime);

        // default for categories
        if(target == VideoCollectionCommon::ELevelCategory)
        {
            sortRole = VideoCollectionCommon::KeyTitle;
        }

        Qt::SortOrder sortOrder(Qt::AscendingOrder);

        // return value ignored, as in case of error the sortRole and sortOrder variables
        // stay at their predefined values, and in error cases those are the sorting values
        // that are used.
        VideoCollectionViewUtils &self = VideoCollectionViewUtils::instance();
        self.loadSortingValues(sortRole, sortOrder, target);

        model->doSorting(sortRole, sortOrder, async);
    }
}

// ---------------------------------------------------------------------------
// setWidgetActivityLevel
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::setWidgetActivityLevel(
        const VideoCollectionCommon::TCollectionLevels &level)
{
    FUNC_LOG; 
    QVariant data = int(level);
    VideoActivityState::instance().setActivityData(data, KEY_WIDGET_LEVEL);

    if(level != VideoCollectionCommon::ELevelDefaultColl && 
       level != VideoCollectionCommon::ELevelAlbum)
    {
        // no need to save collection related data, clear them
        data.clear();
        VideoActivityState::instance().setActivityData(data, KEY_COLLECTION_ID);    
        VideoActivityState::instance().setActivityData(data, KEY_COLLECTION_NAME);   
    }
}

// ---------------------------------------------------------------------------
// getActivityWidgetLevel
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::getActivityWidgetLevel(VideoCollectionCommon::TCollectionLevels &level)
{
    FUNC_LOG;
    // default value is all videos list level
    level = VideoCollectionCommon::ELevelVideos;
    QVariant data = VideoActivityState::instance().getActivityData(KEY_WIDGET_LEVEL);
    
    int value = data.toInt();
    if(value == VideoCollectionCommon::ELevelCategory)
    {
        level = VideoCollectionCommon::ELevelCategory;
    }
    else if(value == VideoCollectionCommon::ELevelDefaultColl)
    {
        level = VideoCollectionCommon::ELevelDefaultColl;
    }
    else if(value == VideoCollectionCommon::ELevelAlbum)
    {
        level = VideoCollectionCommon::ELevelAlbum;
    }
}
 
// ---------------------------------------------------------------------------
// setCollectionActivityData
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::setCollectionActivityData(const TMPXItemId &id, const QString &name)
{
    FUNC_LOG;
    QVariant data = int(id.iId1);
    VideoActivityState::instance().setActivityData(data, KEY_COLLECTION_ID);
    data.clear();
    data = name;
    VideoActivityState::instance().setActivityData(data, KEY_COLLECTION_NAME);
}

// ---------------------------------------------------------------------------
// getCollectionActivityData
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::getCollectionActivityData(TMPXItemId &id, QString &name)
{
    FUNC_LOG;
    id = TMPXItemId::InvalidId();
    name = "";
    VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelVideos;
    getActivityWidgetLevel(level); 
    
    if(level == VideoCollectionCommon::ELevelDefaultColl)
    {
        id.iId2 = KVcxMvcMediaTypeCategory; 
    }
    else if(level == VideoCollectionCommon::ELevelAlbum)
    {
        id.iId2 = KVcxMvcMediaTypeAlbum;
    }
    if(id.iId2 != KMaxTUint32)
    {
        QVariant data = VideoActivityState::instance().getActivityData(KEY_COLLECTION_ID);
        id.iId1 = data.toInt();
        data = VideoActivityState::instance().getActivityData(KEY_COLLECTION_NAME);
        name = data.toString();
    }
}

// ---------------------------------------------------------------------------
// showStatusMsgSlot
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::showStatusMsgSlot(int statusCode, QVariant &additional)
{
	FUNC_LOG;
    QString msg("");
    QString format("");
    bool error(true);
    switch(statusCode)
    {
        case VideoCollectionCommon::statusSingleDeleteFail:
            format = hbTrId("txt_videos_info_unable_to_delete_1_it_is_current");
            if(additional.isValid())
            {
                msg = HbParameterLengthLimiter(format).arg(additional.toString());
            }
        break;
        case VideoCollectionCommon::statusMultipleDeleteFail:
            msg = hbTrId("txt_videos_info_unable_to_delete_some_videos_which");
        break;
        case VideoCollectionCommon::statusSingleRemoveFail:
            format = hbTrId("txt_videos_info_unable_to_remove_collection_1");
            if(additional.isValid())
            {
                msg = format.arg(additional.toString());
            }
        break;
        case VideoCollectionCommon::statusMultiRemoveFail:
            msg = hbTrId("txt_videos_info_unable_to_remove_some_collections");
        break;
        case VideoCollectionCommon::statusVideosAddedToCollection:            
            // videos added to collection - status should containg both count and collection name
            if(additional.isValid() && additional.toList().count() == KAddToCollectionDataCount)
            {
                int count = additional.toList().at(KAddToCollectionCountIndex).toInt();
                QString name = additional.toList().at(KAddToCollectionNameIndex).toString();
                if(count > 0 && name.length())
                {
                    const char* locId = count == 1 ? "txt_videos_dpopinfo_video_added_to_1" :
                        "txt_videos_dpopinfo_videos_added_to_1";
                    
                    msg = hbTrId(locId).arg(name);
                }
            }
            error = false;
        break;
        case VideoCollectionCommon::statusAllVideosAlreadyInCollection:
            msg = hbTrId("txt_videos_info_all_videos_already_added_to_this_c");
        break;
        case VideoCollectionCommon::statusDeleteInProgress:
            if(additional.isValid())
            {
                msg = hbTrId("txt_videos_dpopinfo_ln_videos_are_being_deleted", additional.toInt());
            }
            error = false;
        break;
        default: // no msg to show
        return;
    }

    if(msg.count() > 0)
    {
        HbDialog *note = 0;
        if(error)
        {
            note = new HbMessageBox(msg, HbMessageBox::MessageTypeWarning); 
            qobject_cast<HbMessageBox*>(note)->setStandardButtons( HbMessageBox::NoButton );
            note->setObjectName(VIEW_UTILS_OBJECT_NAME_MESSAGE_BOX_WARNING);
        }
        else
        {
            note = new HbNotificationDialog();

            // only title can be two rows for HbNotificationDialog
            qobject_cast<HbNotificationDialog*>(note)->setTitleTextWrapping(Hb::TextWordWrap);
            qobject_cast<HbNotificationDialog*>(note)->setTitle(msg);
            note->setObjectName(VIEW_UTILS_OBJECT_NAME_STATUS_MSG);
        }
        note->setAttribute(Qt::WA_DeleteOnClose);
        note->setDismissPolicy(HbPopup::TapAnywhere);
        note->setTimeout(HbPopup::StandardTimeout);
        note->show();
    }
}

// End of file
