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
* Description:   Videolist view class source code
*
*/

// Version : %version: 113.1.15 %

// INCLUDE FILES
#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbgroupbox.h>
#include <hbparameterlengthlimiter.h>
#include <hbtoolbarextension.h>
#include <hblabel.h> 
#include <xqaiwdecl.h>
#include <vcxmyvideosdefs.h>

#include "videoservices.h"
#include "videocollectionviewutils.h"
#include "videolistwidget.h"
#include "videolistview.h"
#include "videolisttoolbar.h"
#include "videolistmenu.h"
#include "videocollectioncommon.h"
#include "videocollectionwrapper.h"
#include "videoproxymodelgeneric.h"
#include "videocollectionuiloader.h"
#include "videocollectiontrace.h"

// Object names.
const char* const LIST_VIEW_OBJECT_NAME_OPTIONS_MENU      = "vc::ListViewOptionsMenu";

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListView::VideoListView( VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent ) 
    : HbView(parent)
    , mUiUtils(VideoCollectionViewUtils::instance())
    , mWrapper(VideoCollectionWrapper::instance())
    , mUiLoader(uiLoader)
    , mToolbar(0)
    , mMenu(0)
    , mModelReady(false)
    , mViewReady(false)
    , mVideoServices(0)
    , mCurrentList(0)
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListView::~VideoListView()
{
	FUNC_LOG;
    
    toolBar()->clearActions();
    
    if(mVideoServices)
    {
    	mVideoServices->decreaseReferenceCount();
    	mVideoServices = 0;
    }
}

// ---------------------------------------------------------------------------
// initializeView()
// ---------------------------------------------------------------------------
//
int VideoListView::initializeView()
{
	FUNC_LOG;
	if(!mUiLoader)
	{
        cleanup();
		return -1;
	}
	int videoListPhase = VideoCollectionUiLoaderParam::LoadPhasePrimary;
	int collectionListPhase = VideoCollectionUiLoaderParam::LoadPhaseSecondary;
	int collectionContentListPhase = VideoCollectionUiLoaderParam::LoadPhaseSecondary;
	VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelVideos;
    if (mUiUtils.isService())
    {
        INFO("VideoListView::initializeView() initializing service.");
    	if (!mVideoServices)
        {
    	    mVideoServices = VideoServices::instance();
    	    connect(mVideoServices, SIGNAL(titleReady(const QString&)), 
    	            this, SLOT(titleReadySlot(const QString&)));
		}
    	 
	}
    else
    {
        if(mVideoServices)
        {
            disconnect(mVideoServices, SIGNAL(titleReady(const QString&)), 
                    this, SLOT(titleReadySlot(const QString&)));
            mVideoServices->decreaseReferenceCount();
            mVideoServices = 0;
        }
        VideoCollectionViewUtils::getActivityWidgetLevel(level);
        if(level == VideoCollectionCommon::ELevelCategory)
        {
            videoListPhase = VideoCollectionUiLoaderParam::LoadPhaseSecondary;
            collectionListPhase = VideoCollectionUiLoaderParam::LoadPhasePrimary;
            collectionContentListPhase = VideoCollectionUiLoaderParam::LoadPhaseSecondary;
        }
        else if(level == VideoCollectionCommon::ELevelDefaultColl ||
                level == VideoCollectionCommon::ELevelAlbum)
        {
            videoListPhase = VideoCollectionUiLoaderParam::LoadPhaseSecondary;
            collectionListPhase = VideoCollectionUiLoaderParam::LoadPhasePrimary;
            collectionContentListPhase = VideoCollectionUiLoaderParam::LoadPhasePrimary;
        }   
    }
    // start loading objects and widgets
    QList<VideoCollectionUiLoaderParam> params;
    
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_VIDEOLISTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        videoListPhase));
    
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_COLLECTIONWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        collectionListPhase));
    
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_COLLECTIONCONTENTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        collectionContentListPhase));
    
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_HEADINGBANNER,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_DIALOG,
        DOCML_VIDEOSELECTIONDIALOG_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_NO_CONTENT_LABEL,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    mUiLoader->addData(params,
        this,
        SLOT(objectReadySlot(QObject*, const QString&)));
    params.clear();
    
    if(!mMenu)
    {
        mMenu = new VideoListMenu(mUiLoader, this);
        int err = mMenu->initializeMenu();
        if(err)
        {
            cleanup();
            return -1;
        }
    }
    
    mUiLoader->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    
    // fetch current list right away for main views
    // for default and user defined collections, currentList 
    // will be setted during activation
    if(videoListPhase == VideoCollectionUiLoaderParam::LoadPhasePrimary)
    {
        mCurrentList = mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_VIDEOLISTWIDGET );
    }
    else 
    {
        mCurrentList = mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_COLLECTIONWIDGET );
    }

    return 0;
}

// ---------------------------------------------------------------------------
// titleReadySlot()
// ---------------------------------------------------------------------------
//
void VideoListView::titleReadySlot(const QString& title)
{
	FUNC_LOG;
	setTitle(title);
}

// ---------------------------------------------------------------------------
// activateView()
// ---------------------------------------------------------------------------
//
int VideoListView::activateView( TMPXItemId &itemId)
{
	FUNC_LOG;

    int err = -1;
           
    // activate collection to correct view
    if (itemId == TMPXItemId::InvalidId())
    {  
        
        VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelVideos;
        if(!mVideoServices)
        {
            VideoCollectionViewUtils::getActivityWidgetLevel(level);
        }
        if(level == VideoCollectionCommon::ELevelCategory ||
           level == VideoCollectionCommon::ELevelVideos)
        {
            err = activateMainView();
        }
        else if(level == VideoCollectionCommon::ELevelDefaultColl ||
                level == VideoCollectionCommon::ELevelAlbum)
        {
            // level is default or user defined collection
            // see if we have item id 
            VideoCollectionViewUtils::getCollectionActivityData(itemId, mCollectionName);     
        }
    }
    if(itemId != TMPXItemId::InvalidId())
    {
        err = activateCollectionContentView(itemId);
    }
    
    if(!err)
    {
       
       err = createToolbar();
    }
    
    // connect signals if everything went ok
    if (!err)
    {
        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
        mainWnd->setOrientation(Qt::Vertical, false);
        if (!connect(
                mainWnd, SIGNAL(aboutToChangeOrientation()),
                this, SLOT( aboutToChangeOrientationSlot())) ||
            !connect(
                mainWnd, SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChangedSlot(Qt::Orientation))) ||
            !connect(
                &mWrapper, SIGNAL(asyncStatus(int, QVariant&)),
                this, SLOT(handleAsyncStatusSlot(int, QVariant&))) ||
            !connect(
                mCurrentList->getModel()->sourceModel(), SIGNAL(modelChanged()),
                this, SLOT(layoutChangedSlot())) ||
            !connect(
                mCurrentList->getModel()->sourceModel(), SIGNAL(modelReady()),
                this, SLOT(modelReadySlot())) ||
            !connect(
                mCurrentList->getModel()->sourceModel(), SIGNAL(albumListReady()),
                this, SLOT(albumListReadySlot())))
        {
            ERROR(-1, "VideoListView::activateView() failed to connect signals.");
            // deactivate view so we get rid of dangling connections.
            deactivateView();
            err = -1;
        }
    }
    else
    {
        deactivateView();
    }
    
    return err;
}

// ---------------------------------------------------------------------------
// doDelayedsSlot
// ---------------------------------------------------------------------------
//
void VideoListView::doDelayedsSlot()
{
	FUNC_LOG;
    mUiLoader->loadPhase(VideoCollectionUiLoaderParam::LoadPhaseSecondary);
	if(mCurrentList)
	{
	    mCurrentList->doDelayedsSlot();
	}
}

// ---------------------------------------------------------------------------
// modelReadySlot
// ---------------------------------------------------------------------------
//
void VideoListView::modelReadySlot()
{
	FUNC_LOG;
	
	// check that current list is all videos or collection content.
	VideoCollectionCommon::TCollectionLevels level = mCurrentList->getLevel();
	if(level != VideoCollectionCommon::ELevelCategory)
	{
	    modelReady();
	}
}

// ---------------------------------------------------------------------------
// albumListReadySlot
// ---------------------------------------------------------------------------
//
void VideoListView::albumListReadySlot()
{
    FUNC_LOG;
    
    // check that current list is category list.
    VideoCollectionCommon::TCollectionLevels level = mCurrentList->getLevel();
    if(level == VideoCollectionCommon::ELevelCategory)
    {
        modelReady();
    }
}

// ---------------------------------------------------------------------------
// layoutChangedSlot
// ---------------------------------------------------------------------------
//
void VideoListView::layoutChangedSlot()
{
	FUNC_LOG;
	// Note that showHint should be executed before updateSubLabel as it
	// can modify the mModelReady flag.
    showHint();
    updateSubLabel();
}

// ---------------------------------------------------------------------------
// deactivateView()
// ---------------------------------------------------------------------------
//
void VideoListView::deactivateView()
{
	FUNC_LOG;
    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
    
    mainWnd->unsetOrientation();
    
    disconnect(mainWnd, SIGNAL(aboutToChangeOrientation()),
               this, SLOT(aboutToChangeOrientationSlot()));

    disconnect(mainWnd, SIGNAL(orientationChanged(Qt::Orientation)),
               this, SLOT(orientationChangedSlot(Qt::Orientation)));

    disconnect(&mWrapper, SIGNAL(asyncStatus(int, QVariant&)),
               this, SLOT(handleAsyncStatusSlot(int, QVariant&)));
    
    HbMenu *menu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
    if(menu)
    {
        menu->setObjectName(LIST_VIEW_OBJECT_NAME_OPTIONS_MENU);
        menu->hide();
    }

    if(mCurrentList && mCurrentList->getModel() && mCurrentList->getModel()->sourceModel())
    {
        disconnect(mCurrentList->getModel()->sourceModel(),
                SIGNAL(modelChanged()),
                this, SLOT(layoutChangedSlot()));
        disconnect(mCurrentList->getModel()->sourceModel(),
                SIGNAL(modelReady()),
                this, SLOT(modelReadySlot()));

        showHint(false);
        if(!mVideoServices)
        {
            VideoCollectionCommon::TCollectionLevels level = mCurrentList->getLevel();
            VideoCollectionViewUtils::setWidgetActivityLevel(level);
        }
        mCurrentList->deactivate();
    }
}

// ---------------------------------------------------------------------------
// back()
// ---------------------------------------------------------------------------
//
void VideoListView::back()
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// getCurrentList()
// ---------------------------------------------------------------------------
//
VideoListWidget* VideoListView::getCurrentList()
{
    FUNC_LOG;
    
    return mCurrentList;
}

// ---------------------------------------------------------------------------
// modelReady()
// ---------------------------------------------------------------------------
//
void VideoListView::modelReady()
{
    FUNC_LOG;

    // if mViewReady is false, then it means that this is the first time 
    // modelReady or albumListReady signal fires. Signaling that view is ready.
    if(!mViewReady)
    {
        mViewReady = true;
        emit viewReady();
    }

    mModelReady = true;
    
    // since the reset signal arrives after
    // layout changed, need to make sure that
    // view is updated in case needed
    layoutChangedSlot();
}

// ---------------------------------------------------------------------------
// cleanup()
// ---------------------------------------------------------------------------
//
void VideoListView::cleanup()
{
	FUNC_LOG;
	
	delete mToolbar;
	mToolbar = 0;
	
    mCurrentList = 0;    
}

// ---------------------------------------------------------------------------
// createToolbar()
// Creates toolbar
// ---------------------------------------------------------------------------
//
int VideoListView::createToolbar()
{
	FUNC_LOG;
    
	// creates VideoListToolbar and HbToolbar
    if(!mToolbar)
    {
    	mToolbar = new VideoListToolbar(mUiLoader, this);

        HbToolBar *bar = toolBar(); // First call to toolBar() creates the object, so on failure it could return 0.
        
        if(!bar || !mToolbar ||
           !connect(
                mToolbar, SIGNAL(actionsChanged(QList<QAction*>)), 
                this, SLOT(toolbarActionsChanged(QList<QAction*>))) ||
           !connect(
                mToolbar, SIGNAL(toolbarExtensionChanged(HbToolBarExtension*)), 
                this, SLOT(toolbarExtensionChanged(HbToolBarExtension*))) ||
           !connect(
                mToolbar, SIGNAL(allVideosActionTriggered()), 
                this, SLOT(openAllVideosViewSlot())) ||
           !connect(
                mToolbar, SIGNAL(collectionViewActionTriggered()),
                this, SLOT(openCollectionViewSlot())))
        {
            ERROR(-1, "VideoListView::createToolbar() failed to create all the toolbar.");
            delete mToolbar;
            mToolbar = 0;
            return -1;
        }
        
        mToolbar->initialize();
        mToolbar->viewStateChanged(mCurrentList->getLevel(), true, false);
    }

    return 0;
}

// ---------------------------------------------------------------------------
// showHint
// ---------------------------------------------------------------------------
//
void VideoListView::showHint(bool show)
{
	FUNC_LOG;
    if(!mCurrentList)
    {
        return;
    }

    VideoProxyModelGeneric *model = mCurrentList->getModel();
    
    if(!model || (!mModelReady && model->rowCount() == 0))
    {
        return;
    }
    
    mModelReady = true;
    
    // decide if no content label needs to be shown or not.
    show = show && model->rowCount() == 0;

    // set visibility for the label.
    HbLabel *noContentLabel =
        mUiLoader->findWidget<HbLabel>(
            DOCML_NAME_NO_CONTENT_LABEL);
    if (noContentLabel)
    {
        noContentLabel->setVisible(show);
    }

    mToolbar->viewStateChanged(mCurrentList->getLevel(), show, mModelReady);
    
    // prepare sublabel
    HbGroupBox *subLabel =
        mUiLoader->findWidget<HbGroupBox>(
            DOCML_NAME_VC_HEADINGBANNER);
    if (subLabel)
    {
        if (show &&
            mCurrentList->getLevel() == VideoCollectionCommon::ELevelVideos)
        {
            subLabel->hide();
        }
        else
        {
            subLabel->show();
        }
    }
}

// ---------------------------------------------------------------------------
// updateSubLabel
// ---------------------------------------------------------------------------
//
void VideoListView::updateSubLabel()
{
	FUNC_LOG;
    VideoProxyModelGeneric *model = 0;
    if(mCurrentList)
    {
        model = mCurrentList->getModel(); 
    }
    
    if (model && mModelReady)
    {
        int itemCount = model->rowCount();
        
        HbGroupBox *subLabel = 
            mUiLoader->findWidget<HbGroupBox>(
                DOCML_NAME_VC_HEADINGBANNER);

        if(subLabel && mCurrentList)
        {
			if (mCurrentList->getLevel() == VideoCollectionCommon::ELevelVideos)
			{
			    // no need to update sublabel if there are no items in videolist.
			    // no content label is shown instead.
			    if (itemCount)
			    {
	                subLabel->setHeading(hbTrId("txt_videos_subtitle_all_videos_l1", itemCount));
			    }
			}
			else if (mCurrentList->getLevel() == VideoCollectionCommon::ELevelCategory)
			{
				subLabel->setHeading(hbTrId("txt_videos_subtitle_collections_l1", itemCount));
			}
			else
			{
				QString text = HbParameterLengthLimiter(hbTrId("txt_videos_subtitle_1_l2")).arg(mCollectionName).arg(itemCount);
				subLabel->setHeading(text);
			}
        }
    }
}

// ---------------------------------------------------------------------------
// activateMainView()
// ---------------------------------------------------------------------------
//
int VideoListView::activateMainView()
{
    FUNC_LOG;

    if(!mCurrentList)
    {
        return -1;
    }

    int result = mCurrentList->activate();
    if(result < 0)
    {
        ERROR(result, "VideoListView::activateVideosView() failed to activate.");
        return -1;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// activateCollectionContentView()
// ---------------------------------------------------------------------------
//
int VideoListView::activateCollectionContentView(const TMPXItemId &itemId)
{
    FUNC_LOG;

    // resolve collection name if possible
    if(itemId.iId2  == KVcxMvcMediaTypeCategory && mCollectionName.isEmpty())
    {
        if (itemId.iId1 == KVcxMvcCategoryIdDownloads)
        {
            mCollectionName = hbTrId("txt_videos_dblist_downloaded");
        }
        else if(itemId.iId1 == KVcxMvcCategoryIdCaptured) 
        {
            mCollectionName = hbTrId("txt_videos_dblist_captured");
        }
        else
        {
            // only downloads and captured are supported in default collections
            ERROR(-1, "VideoListView::activateVideosView() invalid defauld collection.");
            return -1;
        }
    }
    // at this point activation will fail if there's no collection name available
    if(mCollectionName.isEmpty())
    {
        ERROR(-1, "VideoListView::activateVideosView() no collection name, cannot proceed.");
        return -1;
    }
    
    // if current list at this point is already collection content, 
    // no need to init again, just activate
    if(mCurrentList && 
       (mCurrentList->getLevel() == VideoCollectionCommon::ELevelDefaultColl ||
        mCurrentList->getLevel() == VideoCollectionCommon::ELevelAlbum))
    {
        
        return mCurrentList->activate();
    }
    // no currentlist, or currentlist differs, create and initialize all over
    collectionOpenedSlot(true, mCollectionName, itemId);
    
    if((!mCurrentList || !mCurrentList->getModel()) ||
       (mCurrentList->getLevel() != VideoCollectionCommon::ELevelDefaultColl && 
        mCurrentList->getLevel() != VideoCollectionCommon::ELevelAlbum ))
    {
        ERROR(-1, "VideoListView::activateVideosView() failed to init and activate collection.");
        return -1;
    }

    // if we're servicing, need to fetch sorting role from client
    // in normal use, sorting has set already at uiloader
   
    if (mVideoServices)
    {       
        // TODO: service sorting roles needs to be defined somewhere
        int sortRole = mVideoServices->sortRole();
        if(sortRole == XQService::SortTitle)
        {
            sortRole = VideoCollectionCommon::KeyTitle;
        }
        else if(sortRole == XQService::SortSize)
        {
            sortRole = VideoCollectionCommon::KeySizeValue;
        } 
        else
        {
            // default 
            sortRole = VideoCollectionCommon::KeyDateTime;
        }
        mCurrentList->getModel()->doSorting(sortRole, Qt::AscendingOrder);
    }
    
    return 0;
}

// ---------------------------------------------------------------------------
// openAllVideosViewSlot()
// ---------------------------------------------------------------------------
//
void VideoListView::openAllVideosViewSlot()
{
	FUNC_LOG;
    VideoListWidget *videoListWidget =
        mUiLoader->findWidget<VideoListWidget>(
            DOCML_NAME_VC_VIDEOLISTWIDGET);
    
    if (mCurrentList &&
        videoListWidget &&
        mCurrentList != videoListWidget)
    {
        // deactivate old list
        mCurrentList->deactivate();
        
        // activate all videos list
        mCurrentList = videoListWidget;
        mCurrentList->activate(VideoCollectionCommon::ELevelVideos);
        
        // update the sublabel, as in most cases the data is already up to date.
        updateSubLabel();
    }
}

// ---------------------------------------------------------------------------
// openCollectionViewSlot()
// ---------------------------------------------------------------------------
//
void VideoListView::openCollectionViewSlot()
{
	FUNC_LOG;
    VideoListWidget *collectionWidget =
        mUiLoader->findWidget<VideoListWidget>(
            DOCML_NAME_VC_COLLECTIONWIDGET);
    
    if (mCurrentList &&
        mCurrentList != collectionWidget)
    {
        // deactivate all videos widget
        mCurrentList->deactivate();
        
        // activate video collection widget
        mCurrentList = collectionWidget;
        mCurrentList->activate(VideoCollectionCommon::ELevelCategory);        

        // the collection view is not empty, so we should hide the hint in advance.
        showHint(false);
        
        // also update the sublabel immediatelly, as the data is up to date almost always.
        updateSubLabel();
    }
}

// -------------------------------------------------------------------------------------------------
// aboutToChangeOrientationSlot()
// hide all items in the window during orientation change
// -------------------------------------------------------------------------------------------------
//
void VideoListView::aboutToChangeOrientationSlot()
{
	FUNC_LOG;
    // this method is required for changing from to mediawall
}

// -------------------------------------------------------------------------------------------------
// orientationChangedSlot()
// orientation changed, items can be put back to visible after view has handled the change
// -------------------------------------------------------------------------------------------------
//
void VideoListView::orientationChangedSlot( Qt::Orientation orientation )
{
	FUNC_LOG;
    // this method is required for changing from to mediawall
    
    // in landscape we need to hide the title and the toolbar. (also required for the mediawall?)
    // TODO: how about the feature where the toolbar comes visible if screen is tapped?
    this->setItemVisible(Hb::AllItems, orientation == Qt::Vertical);
}

// -------------------------------------------------------------------------------------------------
// handleAsyncStatusSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::handleAsyncStatusSlot(int statusCode, QVariant &additional)
{
	FUNC_LOG;
    // show msg from status
    mUiUtils.showStatusMsgSlot(statusCode, additional);
}

// -------------------------------------------------------------------------------------------------
// collectionOpenedSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::collectionOpenedSlot(bool openingCollection,
    const QString& collection,
    const TMPXItemId &collectionId)
{
	FUNC_LOG;
 
    // update / clear collection name
	mCollectionName = collection;

	if(openingCollection)
    {
	    VideoListWidget *collectionContentWidget =
	                   mUiLoader->findWidget<VideoListWidget>(
	                       DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
        // open album view, mCurrentList might be NULL at this point        
        if (!collectionContentWidget || mCurrentList == collectionContentWidget)
        {
            // collection widget cannot be loaded or 
            // currentlist is already collection content -list            
            return;
        }
                
        // get level from the item to be opened only default 
        // or user defined collections can be activated here
        VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelInvalid;
        if(collectionId.iId2 == KVcxMvcMediaTypeCategory)
        {
            level = VideoCollectionCommon::ELevelDefaultColl; 
        }
        else if(collectionId.iId2 == KVcxMvcMediaTypeAlbum)
        {
            level = VideoCollectionCommon::ELevelAlbum; 
        }
        else 
        {
            return;
        }
                
        VideoProxyModelGeneric *model = collectionContentWidget->getModel();
        if(!model)
        {
            // no model for content widget, cannot activate
            return;
        }
        
        // disable collection content animations during widget change        
        HbAbstractItemView::ItemAnimations animationState = collectionContentWidget->enabledAnimations();
        collectionContentWidget->setEnabledAnimations(HbAbstractItemView::None);
        
        // Start fetching content before changing list widget
        model->openItem(collectionId);
        
        // deactivat current widget.
        if(mCurrentList)
        {
            mCurrentList->deactivate();
        }
        
        // activate video collection content widget and set it as current list.
        mCurrentList = collectionContentWidget;
        mCurrentList->activate(level);

        updateSubLabel();
        
        mModelReady = model->rowCount() > 0;

        if(mToolbar)
        {
            // if toolbar not yet created, it means that we're activating
            // for the startup, toolbar will be updated during modelReady()
            mToolbar->viewStateChanged(level, false, mModelReady);
        }
        
        // restore animations for collection content widget
        collectionContentWidget->setEnabledAnimations(animationState);
    }
    else
    {
        // open collection view
        openCollectionViewSlot();
        
        mToolbar->viewStateChanged(VideoCollectionCommon::ELevelCategory, false, mModelReady);
    }	
	if(!mVideoServices)
	{
	    // save / clear collection related activity data
	    VideoCollectionViewUtils::setCollectionActivityData(collectionId, collection);
	}
}

// -------------------------------------------------------------------------------------------------
// objectReadySlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::objectReadySlot(QObject *object, const QString &name)
{
	FUNC_LOG;
    if (name.compare(DOCML_NAME_VC_VIDEOLISTWIDGET) == 0)
    {
        connect(object, SIGNAL(command(int)), this, SIGNAL(command(int)));
        if(mCurrentList != object)
        {
            // this widget not yet activated so it's has been created on the second phase
            // safe to call doDelayed right away
            qobject_cast<VideoListWidget*>(object)->doDelayedsSlot();
        }
    }
    else if (name.compare(DOCML_NAME_VC_COLLECTIONWIDGET) == 0)
    {
        connect(
            object, SIGNAL(collectionOpened(bool, const QString&, const TMPXItemId&)),
            this, SLOT(collectionOpenedSlot(bool, const QString&, const TMPXItemId&)));
        if(mCurrentList != object)
        {
            // this widget not yet activated so it's has been created on the second phase
            // safe to call doDelayed right away
            qobject_cast<VideoListWidget*>(object)->doDelayedsSlot();
        }
    }
    else if (name.compare(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET) == 0)
    {
        connect(object, SIGNAL(command(int)), this, SIGNAL(command(int)));
        connect(
            object, SIGNAL(collectionOpened(bool, const QString&, const TMPXItemId&)),
            this, SLOT(collectionOpenedSlot(bool, const QString&, const TMPXItemId&)));
        if(mCurrentList != object)
        {
            // this widget not yet activated so it's has been created on the second phase
            // safe to call doDelayed right away
            qobject_cast<VideoListWidget*>(object)->doDelayedsSlot();
        }        
    }
}

// -------------------------------------------------------------------------------------------------
// toolbarActionsChanged
// -------------------------------------------------------------------------------------------------
//
void VideoListView::toolbarActionsChanged(QList<QAction*> newActions)
{
    HbToolBar* bar = toolBar();
    
    bar->clearActions();
    
    if(newActions.count() > 0)
    {
        bar->addActions(newActions);
        setItemVisible(Hb::ToolBarItem, true);
    }
    else
    {
        setItemVisible(Hb::ToolBarItem, false);
    }
}

// -------------------------------------------------------------------------------------------------
// toolbarExtensionChanged
// -------------------------------------------------------------------------------------------------
//
void VideoListView::toolbarExtensionChanged(HbToolBarExtension* newExtension)
{
    if(newExtension)
    {
        HbAction *action = toolBar()->addExtension(newExtension);
        HbIcon icon("qtg_mono_video_services");
        action->setIcon(icon);
    }
}

// End of file
