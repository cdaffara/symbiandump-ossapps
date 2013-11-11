/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Videolist content widget implementation
*
*/

#include "videocollectiontrace.h"
#include "videolistwidget.h"

#include <qcoreapplication.h>
#include <qtimer.h>
#include <hbscrollbar.h>
#include <xqserviceutil.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hblistviewitem.h>
#include <hbmessagebox.h>
#include <hbinputdialog.h>
#include <hbparameterlengthlimiter.h>
#include <vcxmyvideosdefs.h>

#include "videocollectionviewutils.h"
#include "videocollectionuiloader.h"
#include "videolistselectiondialog.h"
#include "videothumbnaildata.h"
#include "videoproxymodelgeneric.h"
#include "videocollectioncommon.h"
#include "mpxhbvideocommondefs.h"

// Object names.
const char* const LIST_WIDGET_OBJECT_NAME_CONTEXT_MENU             = "vc:ListWidgetContextMenu";
const char* const LIST_WIDGET_OBJECT_NAME_DELETE_VIDEO             = "vc:ListWidgetMessageBoxDeleteVideo";
const char* const LIST_WIDGET_OBJECT_NAME_RENAME_VIDEO             = "vc:ListWidgetInputDialogRenameVideo";
const char* const LIST_WIDGET_OBJECT_NAME_RENAME_ALBUM             = "vc:ListWidgetInputDialogRenameAlbum";
const char* const LIST_WIDGET_OBJECT_NAME_REMOVE_COLLECTION        = "vc:ListWidgetMessageBoxRemoveCollection";
const char* const LIST_WIDGET_OBJECT_NAME_NAV_KEY_BACK             = "vc:ListWidgetNavKeyBack";
const char* const LIST_WIDGET_OBJECT_NAME_NAV_KEY_QUIT             = "vc:ListWidgetNavKeyQuit";
const char* const LIST_WIDGET_OBJECT_NAME_ACTION_ATTACH            = "vc:ListWidgetActionAttach";
const char* const LIST_WIDGET_OBJECT_NAME_ACTION_PLAY              = "vc:ListWidgetActionPlay";
const char* const LIST_WIDGET_OBJECT_NAME_ACTION_DETAILS           = "vc:ListWidgetActionDetails";
const char* const LIST_WIDGET_OBJECT_NAME_ACTION_DELETE            = "vc:ListWidgetActionDelete";
const char* const LIST_WIDGET_OBJECT_NAME_ACTION_OPEN              = "vc:ListWidgetActionOpen";
const char* const LIST_WIDGET_OBJECT_NAME_ACTION_ADD_TO            = "vc:ListWidgetActionAddToCollection";
const char* const LIST_WIDGET_OBJECT_NAME_ACTION_REMOVE_FROM       = "vc:ListWidgetActionRemoveFromCollection";
const char* const LIST_WIDGET_OBJECT_NAME_ACTION_RENAME            = "vc:ListWidgetActionRename";
const char* const LIST_WIDGET_OBJECT_NAME_ACTION_REMOVE_COLLECTION = "vc:ListWidgetActionRemoveCollection";

// Interval in ms to report the scroll position.
const int SCROLL_POSITION_TIMER_TIMEOUT = 100;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListWidget::VideoListWidget(VideoCollectionUiLoader *uiLoader, HbView *parent) :
HbListView(parent),
mModel(0),
mVideoServices(0),
mCurrentLevel(VideoCollectionCommon::ELevelInvalid),
mSignalsConnected(false),
mNavKeyAction(0),
mContextMenu(0),
mSelectionMode(HbAbstractItemView::NoSelection),
mScrollPositionTimer(0),
mUiLoader(uiLoader),
mService(VideoServices::ENoService)
{
	FUNC_LOG_ADDR(this);
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListWidget::~VideoListWidget()
{
	FUNC_LOG_ADDR(this);
    delete mScrollPositionTimer;
    mScrollPositionTimer = 0;
	mContextMenuActions.clear();
	disconnect();
    delete mContextMenu;
    mContextMenu = 0;
    delete mNavKeyAction;
    mNavKeyAction = 0;
    
    if(mVideoServices)
    {
        mVideoServices->decreaseReferenceCount();
        mVideoServices = 0;
    }
}

// ---------------------------------------------------------------------------
// initialize
// ---------------------------------------------------------------------------
//
int VideoListWidget::initialize(VideoProxyModelGeneric &model, 
                                bool isService,
                                VideoCollectionCommon::TCollectionLevels level)
{
	FUNC_LOG_ADDR(this);
    mModel = &model;    
	mCurrentLevel = level;

	if(isService)
	{
		if(!mVideoServices)
		{
		    mVideoServices = VideoServices::instance();
		}
    	if(XQServiceUtil::interfaceName().contains("IVideoFetch"))
    	{
    		mService = VideoServices::EUriFetcher;
    	}
    	else if (XQServiceUtil::interfaceName().contains("IVideoBrowse"))
    	{
    		mService = VideoServices::EBrowse;
    	}
	}
    else
    {
        if(mVideoServices)
        {
		    mVideoServices->decreaseReferenceCount();
            mVideoServices = 0;    
        }
    }

    // init list view
    VideoCollectionViewUtils::initListView(this);

	mScrollPositionTimer = new QTimer();
	mScrollPositionTimer->setSingleShot(true);

	if (mVideoServices)
	{
		connect(this, SIGNAL(fileUri(const QString&)), mVideoServices, SLOT(itemSelected(const QString&)));
	}

	setItemPixmapCacheEnabled(true);
	setModel(mModel);
	
    return 0;
}

// ---------------------------------------------------------------------------
// activate
// ---------------------------------------------------------------------------
//
int VideoListWidget::activate()
{
	FUNC_LOG_ADDR(this);
    return activate(mCurrentLevel);
}

// ---------------------------------------------------------------------------
// activate
// ---------------------------------------------------------------------------
//
int VideoListWidget::activate(VideoCollectionCommon::TCollectionLevels level)
{
	FUNC_LOG_ADDR(this);
	INFO_2("VideoListWidget::activate() [0x%x]: level: %d", this, level);
	
    if(!mModel || level == VideoCollectionCommon::ELevelInvalid)
    {
        return -1;
    }
    
	mCurrentLevel = level;
    setVisible(true);

    setNavigationAction();

    if (connectSignals() < 0)
    {
        ERROR_1(-1, "VideoListWidget::activate() [0x%x]: connecting signals failed.", this);
        return -1;
    }
    
    // Enable thumbnail background fetching.
    VideoThumbnailData::instance().enableBackgroundFetching(true);
    fetchThumbnailsForVisibleItems();
    
    // open model to the current level in case not in album or category
    if (level != VideoCollectionCommon::ELevelAlbum &&
        level != VideoCollectionCommon::ELevelDefaultColl)
    {
         mModel->open(level);
    }

    return 0;
}

// ---------------------------------------------------------------------------
// deactivate
// ---------------------------------------------------------------------------
//
void VideoListWidget::deactivate()
{
	FUNC_LOG_ADDR(this);
    if(mContextMenu) 
    {
        mContextMenu->hide();
    }

    setVisible(false);
    disConnectSignals();

    // Disable background thumbnail fetching.
    VideoThumbnailData::instance().enableBackgroundFetching(false);
}

// ---------------------------------------------------------------------------
// getLevel
// ---------------------------------------------------------------------------
//
VideoCollectionCommon::TCollectionLevels VideoListWidget::getLevel()
{
	FUNC_LOG_ADDR(this);
    return mCurrentLevel;
}

// ---------------------------------------------------------------------------
// connectSignals
// ---------------------------------------------------------------------------
//
int VideoListWidget::connectSignals()
{
	FUNC_LOG_ADDR(this);
	
    if (mSignalsConnected)
    {
        return 0;
    }
    
    if(!connect(this, SIGNAL(scrollingStarted()), this, SLOT(scrollingStartedSlot())) ||
       !connect(this, SIGNAL(scrollingEnded()), this, SLOT(scrollingEndedSlot())) ||
       !connect(this, SIGNAL(scrollPositionChanged(const QPointF &)), 
               this, SLOT(scrollPositionChangedSlot(const QPointF &))) ||
       !connect(mScrollPositionTimer, SIGNAL(timeout()), 
               this, SLOT(scrollPositionTimerSlot())) || 
       !connect(this, SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)), 
                this, SLOT(longPressedSlot(HbAbstractViewItem *, const QPointF &))) ||
       !connect(mModel, SIGNAL(modelSorted()), this, 
               SLOT(fetchThumbnailsForVisibleItems()), Qt::QueuedConnection))
    {
        return -1;
    }

    // handle navi key trigger -signal connection
    const char *slot = SLOT(quit()); 
    QObject *receiver = qApp;
    
    if(mVideoServices && mService == VideoServices::EBrowse)
    {
        receiver = mVideoServices;
        slot = SLOT(browsingEnded());
    }
    else 
    {        
        if(mCurrentLevel == VideoCollectionCommon::ELevelAlbum || 
           mCurrentLevel == VideoCollectionCommon::ELevelDefaultColl)
        {
            receiver = this;
            slot = SLOT(back());
        }
        else if(mVideoServices && mService == VideoServices::EUriFetcher)
        {
            receiver = this;
            slot = SLOT(endVideoFecthingSlot());            
        }
    }
    if(!connect(mNavKeyAction, SIGNAL(triggered()), receiver, slot))
    {
        return -1;
    }
    mSignalsConnected = true;
    
	return 0;
}

// ---------------------------------------------------------------------------
// disConnectSignals
// ---------------------------------------------------------------------------
//
void VideoListWidget::disConnectSignals()
{
    FUNC_LOG_ADDR(this);
    
    // safe to disconnect these always
    disconnect(
        this, SIGNAL(scrollingStarted()),
        this, SLOT(scrollingStartedSlot()));
    disconnect(
        this, SIGNAL(scrollingEnded()),
        this, SLOT(scrollingEndedSlot()));
    disconnect(
        this, SIGNAL(scrollPositionChanged(const QPointF&)), 
        this, SLOT(scrollPositionChangedSlot(const QPointF&)));
    disconnect(this, SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)), 
             this, SLOT(longPressedSlot(HbAbstractViewItem *, const QPointF &)));
    disconnect(mModel, SIGNAL(modelSorted()), 
             this, SLOT(fetchThumbnailsForVisibleItems()));
    
    // check that scroll position timer is created
    if (mScrollPositionTimer)
    {
        disconnect(
            mScrollPositionTimer, SIGNAL(timeout()),
            this, SLOT(scrollPositionTimerSlot()));
    }
    if(mNavKeyAction)
    {
        mNavKeyAction->disconnect(SIGNAL(triggered()));
    }

	mSignalsConnected = false;
}

// ---------------------------------------------------------------------------
// setNavigationAction
// ---------------------------------------------------------------------------
//
void VideoListWidget::setNavigationAction()
{
    FUNC_LOG_ADDR(this);
    
    // Create navigation action if not already created
    if (!mNavKeyAction)
    {
        Hb::NavigationAction navAction = Hb::QuitNaviAction;
        QString objectName = LIST_WIDGET_OBJECT_NAME_NAV_KEY_QUIT;
        if(mCurrentLevel == VideoCollectionCommon::ELevelAlbum || 
           mCurrentLevel == VideoCollectionCommon::ELevelDefaultColl)
        {
            navAction = Hb::BackNaviAction; 
            objectName = LIST_WIDGET_OBJECT_NAME_NAV_KEY_BACK;
        }
        mNavKeyAction = new HbAction(navAction);
        mNavKeyAction->setObjectName(LIST_WIDGET_OBJECT_NAME_NAV_KEY_BACK);       
    }
        
    // Set navigation action only when widget is not in selection mode
    if (mSelectionMode == HbAbstractItemView::NoSelection)
    {
        HbView *currentView = hbInstance->allMainWindows().value(0)->currentView();
        if (currentView)
        {
            currentView->setNavigationAction(mNavKeyAction);
        }
    }
}

// ---------------------------------------------------------------------------
// deleteItemSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::deleteItemSlot()
{
	FUNC_LOG_ADDR(this);
    if(!mModel)
    {
        return;
    }
    
    QVariant variant;
    QModelIndex index = currentIndex();
    variant = mModel->data(index, VideoCollectionCommon::KeyTitle);

    if (variant.isValid())
    {
        QString text = HbParameterLengthLimiter(
                hbTrId("txt_videos_info_do_you_want_to_delete_1")).arg(variant.toString()); 
        
        HbMessageBox *messageBox = new HbMessageBox(text, HbMessageBox::MessageTypeQuestion);
        messageBox->setStandardButtons(HbMessageBox::Yes | HbMessageBox::No);
        messageBox->setAttribute(Qt::WA_DeleteOnClose);
        messageBox->setObjectName(LIST_WIDGET_OBJECT_NAME_DELETE_VIDEO);
        messageBox->open(this, SLOT(deleteItemDialogFinished(int)));
    }
}

// ---------------------------------------------------------------------------
// deleteItemDialogFinished
// ---------------------------------------------------------------------------
//
void VideoListWidget::deleteItemDialogFinished(int action)
{
    if(action == HbMessageBox::Yes)
    {
        QModelIndex index = currentIndex();
        if(index.isValid())
        {
            QModelIndexList list;
            list.append(index);
            mModel->deleteItems(list);
        }
    }
}

// ---------------------------------------------------------------------------
// createContextMenu
// ---------------------------------------------------------------------------
//
void VideoListWidget::createContextMenu()
{
	FUNC_LOG_ADDR(this);
    if(mContextMenu)
    {
        return;
    }
    
    mContextMenu = new HbMenu();
    mContextMenu->setDismissPolicy(HbPopup::TapAnywhere);
    mContextMenu->setObjectName(LIST_WIDGET_OBJECT_NAME_CONTEXT_MENU);

    if (mService == VideoServices::EUriFetcher)
    {
        mContextMenuActions[EActionAttach] = 
                mContextMenu->addAction(hbTrId("txt_common_menu_select"), this, SLOT(openItemSlot()));
        mContextMenuActions[EActionAttach]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_ATTACH);
        
        mContextMenuActions[EActionOpen] = 
                                    mContextMenu->addAction(hbTrId("txt_common_menu_open"), this, SLOT(openItemSlot()));
        mContextMenuActions[EActionOpen]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_OPEN);
        
        mContextMenuActions[EActionPlay] = 
                mContextMenu->addAction(hbTrId("txt_videos_menu_play"), this, SLOT(playItemSlot()));
        mContextMenuActions[EActionPlay]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_PLAY);
        
        mContextMenuActions[EActionDetails] = 
                mContextMenu->addAction(hbTrId("txt_common_menu_details"), this, SLOT(openDetailsSlot()));
        mContextMenuActions[EActionDetails]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_DETAILS);
    }
    else if (mService == VideoServices::EBrowse)
    {
        mContextMenuActions[EActionPlay] = 
                mContextMenu->addAction(hbTrId("txt_videos_menu_play"), this, SLOT(playItemSlot()));
        mContextMenuActions[EActionPlay]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_PLAY);
        
        mContextMenuActions[EActionDetails] = 
                mContextMenu->addAction(hbTrId("txt_common_menu_details"), this, SLOT(openDetailsSlot()));
        mContextMenuActions[EActionDetails]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_DETAILS);
        
        mContextMenuActions[EActionDelete] = 
                mContextMenu->addAction(hbTrId("txt_common_menu_delete"), this, SLOT(deleteItemSlot()));
        mContextMenuActions[EActionDelete]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_DELETE);
    }
    else
    {
        mContextMenuActions[EActionPlay] = 
                mContextMenu->addAction(hbTrId("txt_videos_menu_play"), this, SLOT(playItemSlot()));
        mContextMenuActions[EActionPlay]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_PLAY);

        mContextMenuActions[EActionDetails] = 
                mContextMenu->addAction(hbTrId("txt_common_menu_details"), this, SLOT(openDetailsSlot()));
        mContextMenuActions[EActionDetails]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_DETAILS);
        
        mContextMenuActions[EActionOpen] = 
                            mContextMenu->addAction(hbTrId("txt_common_menu_open"), this, SLOT(openItemSlot()));
        mContextMenuActions[EActionOpen]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_OPEN);
        
        mContextMenuActions[EActionAddToCollection] = 
                mContextMenu->addAction(hbTrId("txt_videos_menu_add_to_collection"), this, SLOT(addToCollectionSlot()));
        mContextMenuActions[EActionAddToCollection]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_ADD_TO);
        
        mContextMenuActions[EActionRemove] = 
                mContextMenu->addAction(hbTrId("txt_videos_menu_remove_from_collection"), this, SLOT(removeFromCollectionSlot()));
        mContextMenuActions[EActionRemove]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_REMOVE_FROM);
        
        mContextMenuActions[EActionRename] = 
                mContextMenu->addAction(hbTrId("txt_common_menu_rename_item"), this, SLOT(renameSlot()));
        mContextMenuActions[EActionRename]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_RENAME);
        
        mContextMenuActions[EActionRemoveCollection] = 
                mContextMenu->addAction(hbTrId("txt_videos_menu_remove_collection"), this, SLOT(removeCollectionSlot()));
        mContextMenuActions[EActionRemoveCollection]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_REMOVE_COLLECTION);
        
        mContextMenuActions[EActionDelete] = 
                mContextMenu->addAction(hbTrId("txt_common_menu_delete"), this, SLOT(deleteItemSlot()));
        mContextMenuActions[EActionDelete]->setObjectName(LIST_WIDGET_OBJECT_NAME_ACTION_DELETE);
    }
}

// -------------------------------------------------------------------------------------------------
// setContextMenu
// -------------------------------------------------------------------------------------------------
//
void VideoListWidget::setContextMenu()
{
	FUNC_LOG_ADDR(this);
    if(!mContextMenu)
    {
        createContextMenu();
    }
    
    if (!mContextMenu)
    {
        ERROR_1(-1, "VideoListWidget::setContextMenu() [0x%x]: failed to create context menu.", this);
        return;
    }
    
	int menuActionCount = 0;
    HbAction *action = 0;
    foreach(action, mContextMenuActions.values())
    {
        if(action)
        {
			++menuActionCount;
			action->setVisible(false);
        }
    }
    if(menuActionCount != mContextMenuActions.values().count() || mContextMenuActions.values().count() == 0)
    {
        ERROR_1(-1, "VideoListWidget::setContextMenu() [0x%x]: all actions have not been created.", this);
        delete mContextMenu;
        mContextMenu = 0;
        mContextMenuActions.clear();
    	return;
    }

    if (mVideoServices)
    {
        setServiceContextMenu();
        return;
    }

    if(mCurrentLevel == VideoCollectionCommon::ELevelVideos ||
       mCurrentLevel == VideoCollectionCommon::ELevelDefaultColl)
    {        
    	mContextMenuActions[EActionAddToCollection]->setVisible(true);
    	mContextMenuActions[EActionDelete]->setVisible(true);
    	mContextMenuActions[EActionPlay]->setVisible(true);
		mContextMenuActions[EActionDetails]->setVisible(true);
		mContextMenuActions[EActionRename]->setVisible(true);
    }
    else if(mCurrentLevel == VideoCollectionCommon::ELevelCategory) 
    {
        mContextMenuActions[EActionOpen]->setVisible(true);
        TMPXItemId mpxId = mModel->getMediaIdAtIndex(currentIndex());
		if(mpxId.iId2 == KVcxMvcMediaTypeAlbum)
		{
            mContextMenuActions[EActionRename]->setVisible(true);
            mContextMenuActions[EActionRemoveCollection]->setVisible(true);
		}
    }
    else if(mCurrentLevel == VideoCollectionCommon::ELevelAlbum)
    {
    	mContextMenuActions[EActionRemove]->setVisible(true);
        mContextMenuActions[EActionDelete]->setVisible(true);
    	mContextMenuActions[EActionPlay]->setVisible(true);
		mContextMenuActions[EActionDetails]->setVisible(true);
		mContextMenuActions[EActionRename]->setVisible(true);
    }
}

// ---------------------------------------------------------------------------
// setServiceContextMenu
// ---------------------------------------------------------------------------
//
void VideoListWidget::setServiceContextMenu()
{
    FUNC_LOG_ADDR(this);
    
    if(mCurrentLevel == VideoCollectionCommon::ELevelCategory) 
    {
        mContextMenuActions[EActionOpen]->setVisible(true);   
    } 
    else if(mCurrentLevel > VideoCollectionCommon::ELevelCategory)
    {        
        mContextMenuActions[EActionDetails]->setVisible(true);
        mContextMenuActions[EActionPlay]->setVisible(true);
        if(mService == VideoServices::EBrowse)
        {            
            mContextMenuActions[EActionDelete]->setVisible(true);
        }
        else if(mService == VideoServices::EUriFetcher)
        {
            mContextMenuActions[EActionAttach]->setVisible(true);
        }
    }
}

// ---------------------------------------------------------------------------
// getModel
// ---------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoListWidget::getModel()
{ 
	FUNC_LOG_ADDR(this);
    return mModel; 
}

// ---------------------------------------------------------------------------
// emitActivated
// This slot is called by the fw when viewitem is activated
// ---------------------------------------------------------------------------
//
void VideoListWidget::emitActivated (const QModelIndex &modelIndex)
{
	FUNC_LOG_ADDR(this);
    // surprisingly interesting feature: after long press also single press
    // is executed. as a workaround the following hack check is needed.
    // otherwise after the context menu is shown also single press action
    // is executed.
    if (mContextMenu &&
        mContextMenu->isVisible())
    {
        // do not activate context menu if it is already visible
        return;
    }
    if(mSelectionMode != HbAbstractItemView::NoSelection)
    {
        // no custom functionality defined
        emit activated(modelIndex);
        return;
    }
    doEmitActivated(modelIndex);
}

// ---------------------------------------------------------------------------
// doEmitActivated
// ---------------------------------------------------------------------------
//
void VideoListWidget::doEmitActivated (const QModelIndex &index)
{
    if(mVideoServices &&
       mService == VideoServices::EUriFetcher &&
       mCurrentLevel != VideoCollectionCommon::ELevelCategory)
    {
        QVariant variant = mModel->data(index, VideoCollectionCommon::KeyFilePath);
        if ( variant.isValid()  )
        {
            QString itemPath = variant.value<QString>();
            emit(fileUri(itemPath));
        }
    }
    else
    {
        doActivateItem(index);
    }
}

// ---------------------------------------------------------------------------
// doActivateItem
// ---------------------------------------------------------------------------
//
void VideoListWidget::doActivateItem(const QModelIndex &index)
{
    FUNC_LOG_ADDR(this);
    if (!mModel || !index.isValid())
    {
        return;
    }
    
    if (mCurrentLevel == VideoCollectionCommon::ELevelCategory)
    {
        QVariant variant = mModel->data(index, VideoCollectionCommon::KeyTitle);
        if (variant.isValid())
        {
            // signal view that item has been activated
            emit(collectionOpened(true,
                variant.toString(),
                mModel->getMediaIdAtIndex(index)));                       
        }
        return;
    }
    else
    {
        if(mModel->getMediaIdAtIndex(index).iId2 == KVcxMvcMediaTypeVideo)
        {
            VideoThumbnailData::instance().freeThumbnailData();
        }
    	mModel->openItem(mModel->getMediaIdAtIndex(index));
    }
}

// ---------------------------------------------------------------------------
// setSelectionMode
// called by the fw when user long presses some item
// ---------------------------------------------------------------------------
//
void VideoListWidget::setSelectionMode(int mode)
{
	FUNC_LOG_ADDR(this);
    HbAbstractItemView::SelectionMode selMode = HbAbstractItemView::NoSelection;
    if(mode >= HbAbstractItemView::NoSelection && mode <= HbAbstractItemView::MultiSelection)
    {
        selMode = HbAbstractItemView::SelectionMode(mode);
    }

    HbListView::setSelectionMode(selMode);
    mSelectionMode = mode;
}

// ---------------------------------------------------------------------------
// longPressedSlot
// called by the fw when user long presses some item
// ---------------------------------------------------------------------------
//
void VideoListWidget::longPressedSlot(HbAbstractViewItem *item, const QPointF &point)
{
    FUNC_LOG_ADDR(this);
    if(mSelectionMode != HbAbstractItemView::NoSelection)
    {
        // do not activate context menu during selection mode
        return;
    }

    if(item)
    {
        QModelIndex index = item->modelIndex();
        if(mModel && index.isValid())
        {
            setContextMenu();
            // if menu not yet exists, it has been created
            // setup might fail causing menu to be removed
            if(mContextMenu)
            {
                mContextMenu->setPreferredPos(point);
                mContextMenu->show();
            }
        }
    }
}

// ---------------------------------------------------------------------------
// doDelayedsSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::doDelayedsSlot()
{
	FUNC_LOG_ADDR(this);
	if (!mContextMenu)
	{
		createContextMenu();
	}
}

// ---------------------------------------------------------------------------
// openItemSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::openItemSlot()
{
    FUNC_LOG_ADDR(this);
    doEmitActivated(currentIndex());
}

// ---------------------------------------------------------------------------
// playItemSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::playItemSlot()
{
    FUNC_LOG_ADDR(this);
    doActivateItem(currentIndex());
}

// ---------------------------------------------------------------------------
// openDetailsSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::openDetailsSlot()
{   
	FUNC_LOG_ADDR(this);
    if(mModel && mModel->fetchItemDetails(currentIndex()) == 0 ) 
    {
        emit command(MpxHbVideoCommon::ActivateVideoDetailsView);
    }
}

// ---------------------------------------------------------------------------
// renameSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::renameSlot()
{
	FUNC_LOG_ADDR(this);
    if(!mModel)
    {
        return;
    }
    
    QModelIndex index = currentIndex();
    QVariant variant = mModel->data(index, VideoCollectionCommon::KeyTitle);
    
    if(variant.isValid())
    {
        QString label;
        const char* objectName = 0;
        QString currentName = variant.toString();
        
        TMPXItemId mpxId = mModel->getMediaIdAtIndex(index);
        if(mpxId.iId2 == KVcxMvcMediaTypeAlbum)
        {
            label = hbTrId("txt_videos_title_enter_name");
            objectName = LIST_WIDGET_OBJECT_NAME_RENAME_ALBUM;
        }
        else if(mpxId.iId2 == KVcxMvcMediaTypeVideo)
        {
            label = hbTrId("txt_videos_dialog_video_name");
            objectName = LIST_WIDGET_OBJECT_NAME_RENAME_VIDEO;
        }
        
        if(!label.isEmpty())
        {
            HbInputDialog *dialog = new HbInputDialog();
            dialog->setAttribute(Qt::WA_DeleteOnClose);
            dialog->setObjectName(objectName);
            dialog->setPromptText(label);
            dialog->setValue(currentName);
            dialog->open(this, SLOT(renameDialogFinished(HbAction *)));
        }   
    }
}

// -------------------------------------------------------------------------------------------------
// renameDialogFinished
// -------------------------------------------------------------------------------------------------
//
void VideoListWidget::renameDialogFinished(HbAction *action)
{
    Q_UNUSED(action);

    HbInputDialog *dialog = static_cast<HbInputDialog*>(sender());
    if(dialog->actions().first() != action)
    {
        return;
    }
    QModelIndex index = currentIndex();
    TMPXItemId itemId = mModel->getMediaIdAtIndex(index);
    
    QVariant newNameVariant = dialog->value();
    QVariant oldNameVariant = mModel->data(index, VideoCollectionCommon::KeyTitle);
    if(!newNameVariant.isValid() || !oldNameVariant.isValid())
    {
        // invalid data at index
        return;
    }
    
    QString newName = newNameVariant.toString().trimmed();
    QString oldName = oldNameVariant.toString();
    if(!newName.length() || newName == oldName)
    {
        // no new name provided or name has not changed
        return;
    }
    if(itemId.iId2 == KVcxMvcMediaTypeAlbum)
    {     
        // for album, we need to make sure name is unique
        newName = mModel->resolveAlbumName(newName);
    }
    mModel->renameItem(itemId, newName);
}

// ---------------------------------------------------------------------------
// addToCollectionSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::addToCollectionSlot()
{
	FUNC_LOG_ADDR(this);
    VideoListSelectionDialog *dialog =
       mUiLoader->findWidget<VideoListSelectionDialog>(
           DOCML_NAME_DIALOG);
    if (!dialog || !mModel)
    {
        ERROR_1(-1, "VideoListWidget::addToCollectionSlot() [0x%x]: failed to get selection dialog or model is null.", this);
        return;
    }
    TMPXItemId itemId = mModel->getMediaIdAtIndex(currentIndex());
    if(itemId != TMPXItemId::InvalidId())
    {
        dialog->setupContent(VideoListSelectionDialog::ESelectCollection, itemId);
        dialog->exec();
    }
}

// ---------------------------------------------------------------------------
// removeFromCollectionSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::removeFromCollectionSlot()
{
	FUNC_LOG_ADDR(this);
    if(!mModel)
    {
        return;
    }
    TMPXItemId collectionId = mModel->getOpenItem();
    TMPXItemId itemId = mModel->getMediaIdAtIndex(currentIndex());
    if(collectionId != TMPXItemId::InvalidId() && 
       itemId != TMPXItemId::InvalidId())
    {
        QList<TMPXItemId> ids;
        ids.append(itemId);
        mModel->removeItemsFromAlbum(collectionId, ids);
    }
}

// ---------------------------------------------------------------------------
// removeCollectionSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::removeCollectionSlot()
{
	FUNC_LOG_ADDR(this);
    if(!mModel)
    {
        return;
    }
    
    QVariant variant;
    QModelIndex index = currentIndex();
    variant = mModel->data(index, VideoCollectionCommon::KeyTitle);

    if (variant.isValid())
    {
        QString text = HbParameterLengthLimiter(
                hbTrId("txt_videos_info_do_you_want_to_remove_collection")).arg(variant.toString()); 
        
        HbMessageBox *messageBox = new HbMessageBox(text, HbMessageBox::MessageTypeQuestion);
        messageBox->setStandardButtons(HbMessageBox::Yes | HbMessageBox::No);
        messageBox->setAttribute(Qt::WA_DeleteOnClose);
        messageBox->setObjectName(LIST_WIDGET_OBJECT_NAME_REMOVE_COLLECTION);
        messageBox->open(this, SLOT(removeCollectionDialogFinished(int)));
    }
}

// ---------------------------------------------------------------------------
// removeCollectionDialogFinished
// ---------------------------------------------------------------------------
//
void VideoListWidget::removeCollectionDialogFinished(int action)
{
    if(action == HbMessageBox::Yes)
    {
        QModelIndex index = currentIndex();
        if(index.isValid())
        {
            QModelIndexList list;
            list.append(index);
            mModel->removeAlbums(list);
        }
    }
}

// ---------------------------------------------------------------------------
// back
// ---------------------------------------------------------------------------
//
void VideoListWidget::back()
{
	FUNC_LOG_ADDR(this);

    if(mModel)
    {
    	// Empty the proxy model causing the items to be removed from list widget.
        mModel->setAlbumInUse(TMPXItemId::InvalidId());
		emit collectionOpened(false, QString(), TMPXItemId::InvalidId());
    }
}

// ---------------------------------------------------------------------------
// endVideoFecthingSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::endVideoFecthingSlot()
{
    if(mVideoServices && mService == VideoServices::EUriFetcher)
    {        
        QString empty = "";
        emit fileUri(empty);
    }
}

// ---------------------------------------------------------------------------
// scrollingStartedSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollingStartedSlot()
{
	FUNC_LOG_ADDR(this);
    VideoThumbnailData::instance().enableThumbnailCreation(false);
}

// ---------------------------------------------------------------------------
// scrollingEndedSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollingEndedSlot()
{
	FUNC_LOG_ADDR(this);
    if(mScrollPositionTimer)
    {
        mScrollPositionTimer->stop();
    }
    VideoThumbnailData::instance().enableThumbnailCreation(true);
    fetchThumbnailsForVisibleItems();
}

// ---------------------------------------------------------------------------
// scrollPositionChangedSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollPositionChangedSlot(const QPointF &newPosition)
{
    Q_UNUSED(newPosition);
	if(mScrollPositionTimer && !mScrollPositionTimer->isActive())
	    mScrollPositionTimer->start(SCROLL_POSITION_TIMER_TIMEOUT);
}

// ---------------------------------------------------------------------------
// scrollPositionTimerSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollPositionTimerSlot()
{
	FUNC_LOG_ADDR(this);
    fetchThumbnailsForVisibleItems();
}

// ---------------------------------------------------------------------------
// fetchThumbnailsForVisibleItems
// ---------------------------------------------------------------------------
//
void VideoListWidget::fetchThumbnailsForVisibleItems()
{
	FUNC_LOG_ADDR(this);
	
	// No need to do anything if thumbnail fetching is not enabled.
	if(!VideoThumbnailData::instance().backgroundFetchingEnabled())
	{
	    return;
	}
	
    const QList<HbAbstractViewItem *> itemsVisible = visibleItems();
    
    if(itemsVisible.count() > 0)
    {
        int row = itemsVisible.value(0)->modelIndex().row();
        VideoThumbnailData::instance().startBackgroundFetching(mModel, row);
    }
    else
    {
        // Nothing visible yet, start from first index.
        VideoThumbnailData::instance().startBackgroundFetching(mModel, 0);
    }
}

// ---------------------------------------------------------------------------
// rowsInserted
// ---------------------------------------------------------------------------
//
void VideoListWidget::rowsInserted(const QModelIndex &parent, int start, int end)
{
    FUNC_LOG_ADDR(this);
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    fetchThumbnailsForVisibleItems();
    HbListView::rowsInserted(parent, start, end);
}

// ---------------------------------------------------------------------------
// rowsRemoved
// ---------------------------------------------------------------------------
//
void VideoListWidget::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    FUNC_LOG_ADDR(this);
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    fetchThumbnailsForVisibleItems();
    HbListView::rowsRemoved(parent, start, end);
}

// end of file
