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
* Description:   VideoListToolbar class source code
*
*/

// Version : %version: 1.1.3 %

// INCLUDE FILES
#include <qactiongroup.h>
#include <hbaction.h>
#include <hbtoolbarextension.h>
#include <vcxmyvideosdefs.h>

#include "videolisttoolbar.h"
#include "videoservices.h"
#include "videooperatorservice.h"
#include "videocollectioncenrepdefs.h"
#include "videocollectiontrace.h"
#include "videolistview.h"
#include "videocollectionuiloader.h"
#include "videolistwidget.h"
#include "videoproxymodelgeneric.h"
#include "videolistselectiondialog.h"

// Object names.
const char* const LIST_VIEW_OBJECT_NAME_TOOLBAR_EXTENSION = "vc::ListViewToolbarExtension";

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListToolbar::VideoListToolbar(VideoCollectionUiLoader* uiLoader, VideoListView* parent)
    : QObject(parent)
    , mCurrentLevel(VideoCollectionCommon::ELevelInvalid)
    , mUiUtils(VideoCollectionViewUtils::instance())
    , mUiLoader(uiLoader)
    , mListView(parent)
    , mVideoServices(0)
    , mToolbarViewsActionGroup(0)
    , mToolbarCollectionActionGroup(0)
    , mToolbarServiceExtension(0)
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListToolbar::~VideoListToolbar()
{
	FUNC_LOG;
    
	emit actionsChanged(mViewStateActions[VideoCollectionCommon::ELevelInvalid]);
	
    mToolbarActions.clear();

    delete mToolbarServiceExtension;
    mToolbarServiceExtension = 0;
    
    if(mVideoServices)
    {
    	mVideoServices->decreaseReferenceCount();
    	mVideoServices = 0;
    }
    
    QList<VideoOperatorService *>::const_iterator iter = mVideoOperatorServices.constBegin();
    while(iter != mVideoOperatorServices.constEnd())
    {
        delete *iter;
        iter++;
    }
    mVideoOperatorServices.clear();
}

// ---------------------------------------------------------------------------
// initialize()
// ---------------------------------------------------------------------------
//
void VideoListToolbar::initialize()
{
    if (mUiUtils.isService())
    {
        INFO("VideoListToolbar::initialize() initializing service.");
        if (!mVideoServices)
        {
            mVideoServices = VideoServices::instance();
        }
         
    }
    else if(mVideoServices)
    {
        mVideoServices->decreaseReferenceCount();
        mVideoServices = 0;
    }
    
    createToolbarActions();
}

// ---------------------------------------------------------------------------
// viewStateChanged()
// ---------------------------------------------------------------------------
//
void VideoListToolbar::viewStateChanged(
    VideoCollectionCommon::TCollectionLevels currentLevel, 
    bool noVideos, bool modelReady)
{
    if(!mToolbarViewsActionGroup || !mToolbarCollectionActionGroup)
    {
        // not initialized yet.
        return;
    }
    
    if(currentLevel != mCurrentLevel)
    {
        QList<QAction*> newActions = mViewStateActions[currentLevel];
        QList<QAction*> oldActions = mViewStateActions[mCurrentLevel];
        
        mCurrentLevel = currentLevel;
        
        if(newActions != oldActions)
        {
            emit actionsChanged(newActions);
            
            if(mToolbarServiceExtension &&
                (mCurrentLevel == VideoCollectionCommon::ELevelCategory ||
                 mCurrentLevel == VideoCollectionCommon::ELevelVideos))
            {
                emit toolbarExtensionChanged(mToolbarServiceExtension);
            }
            // note don't need to clear the toolbar extension actions, as the 
            // actionsChanged signal should clear all previous actions.
        }
        
        if(mCurrentLevel == VideoCollectionCommon::ELevelVideos &&
           !mToolbarActions[ETBActionAllVideos]->isChecked())
        {
            mToolbarActions[ETBActionAllVideos]->setChecked(true);
        }
        else if(mCurrentLevel == VideoCollectionCommon::ELevelCategory &&
           !mToolbarActions[ETBActionCollections]->isChecked())
        {
            mToolbarActions[ETBActionCollections]->setChecked(true);
        }
    }
    if(!mVideoServices)
    {
        if(modelReady)
        {
        
            mToolbarActions[ETBActionAddVideos]->setVisible(true);
            mToolbarActions[ETBActionRemoveVideos]->setVisible(!noVideos);

        }
        else
        {
            mToolbarActions[ETBActionAddVideos]->setVisible(false);
            mToolbarActions[ETBActionRemoveVideos]->setVisible(false);
        }
    }
}

// ---------------------------------------------------------------------------
// createToolbarActions()
// Creates toolbar actions and toolbar icons
// ---------------------------------------------------------------------------
//
void VideoListToolbar::createToolbarActions()
{
	FUNC_LOG;
    
	// Create actiongroup and add all actions to it. This ensures that only one is
    // active at certain moment.
    if(!mToolbarViewsActionGroup && !mToolbarCollectionActionGroup)
    {
    	mToolbarViewsActionGroup = new QActionGroup(this);
        mToolbarCollectionActionGroup = new QActionGroup(this);

        // create toolbar item actions

        // All Videos tab
        mToolbarActions[ETBActionAllVideos] = createAction("qtg_mono_video",
                mToolbarViewsActionGroup, SIGNAL(allVideosActionTriggered()));

        // Collections tab
        mToolbarActions[ETBActionCollections] = createAction("qtg_mono_video_collection",
                mToolbarViewsActionGroup, SIGNAL(collectionViewActionTriggered()));

        if (!mVideoServices)
        {
			// Create services button or toolbar extension depending how many operator 
            // services are configured.
            createOperatorServicesToolbarActions();
			
			// Add Videos tab
			mToolbarActions[ETBActionAddVideos] = 
			        createAction("qtg_mono_add_to_video_collection",
					mToolbarCollectionActionGroup, SLOT(addVideosToCollectionSlot()));

			// Remove Videos tab
			mToolbarActions[ETBActionRemoveVideos] = 
			        createAction("qtg_mono_remove_from_video_collection",
					mToolbarCollectionActionGroup, SLOT(removeVideosFromCollectionSlot()));
        }

        // Collection view actions are not checkable
        mToolbarActions[ETBActionAllVideos]->setCheckable(true);
        mToolbarActions[ETBActionCollections]->setCheckable(true);

        if(!mVideoServices && mToolbarActions[ETBActionServices])
        {
        	mToolbarActions[ETBActionServices]->setCheckable(false);
        }
        
        mViewStateActions[VideoCollectionCommon::ELevelCategory] = mToolbarViewsActionGroup->actions();
        mViewStateActions[VideoCollectionCommon::ELevelVideos] = mToolbarViewsActionGroup->actions();
        if(!mVideoServices)
        {
            mViewStateActions[VideoCollectionCommon::ELevelAlbum] = mToolbarCollectionActionGroup->actions();
        }
    }
}

// ---------------------------------------------------------------------------
// createOperatorServicesToolbar()
// ---------------------------------------------------------------------------
//
void VideoListToolbar::createOperatorServicesToolbarActions()
{
    FUNC_LOG;
    if(mVideoOperatorServices.count() > 0)
    {
        return;
    }
    
    // Load services.
    
    loadOperatorService(KVideoCollectionViewCenrepServiceItem1Title, KVideoCollectionViewCenrepServiceItem1ToolbarIconPath, 
            KVideoCollectionViewCenrepServiceItem1Url, KVideoCollectionViewCenrepServiceItem1Uid);
    
    loadOperatorService(KVideoCollectionViewCenrepServiceItem2Title, KVideoCollectionViewCenrepServiceItem2ToolbarIconPath, 
            KVideoCollectionViewCenrepServiceItem2Url, KVideoCollectionViewCenrepServiceItem2Uid);

    loadOperatorService(KVideoCollectionViewCenrepServiceItem3Title, KVideoCollectionViewCenrepServiceItem3ToolbarIconPath, 
            KVideoCollectionViewCenrepServiceItem3Url, KVideoCollectionViewCenrepServiceItem3Uid);

    loadOperatorService(KVideoCollectionViewCenrepServiceItem4Title, KVideoCollectionViewCenrepServiceItem4ToolbarIconPath, 
            KVideoCollectionViewCenrepServiceItem4Url, KVideoCollectionViewCenrepServiceItem4Uid);

    loadOperatorService(KVideoCollectionViewCenrepServiceItem5Title, KVideoCollectionViewCenrepServiceItem5ToolbarIconPath, 
            KVideoCollectionViewCenrepServiceItem5Url, KVideoCollectionViewCenrepServiceItem5Uid);

    loadOperatorService(KVideoCollectionViewCenrepServiceItem6Title, KVideoCollectionViewCenrepServiceItem6ToolbarIconPath, 
            KVideoCollectionViewCenrepServiceItem6Url, KVideoCollectionViewCenrepServiceItem6Uid);
    
    // Create toolbar extension when there's multiple services.
    if(mVideoOperatorServices.count() > 1 && !mToolbarServiceExtension)
    {
        mToolbarServiceExtension = new HbToolBarExtension();
        mToolbarServiceExtension->setObjectName(LIST_VIEW_OBJECT_NAME_TOOLBAR_EXTENSION);
        
        QList<VideoOperatorService *>::const_iterator iter = mVideoOperatorServices.constBegin();
        while(iter != mVideoOperatorServices.constEnd())
        {
            HbIcon icon((*iter)->iconResource());
            HbAction *action = mToolbarServiceExtension->addAction(icon, (*iter)->title(), 
                    (*iter), SLOT(launchService()));
            action->setObjectName((*iter)->title());
            iter++;
        }
    }
    
    // Add toolbar button when there's only one service.
    if(mVideoOperatorServices.count() == 1)
    {
        VideoOperatorService *service = mVideoOperatorServices[0];
        mToolbarActions[ETBActionServices] = createAction(service->iconResource(),
                mToolbarViewsActionGroup, 0 /*do not connect to any slot*/);
        connect(mToolbarActions[ETBActionServices], SIGNAL(triggered()), service, SLOT(launchService()));
    }
}

// ---------------------------------------------------------------------------
// loadOperatorService()
// ---------------------------------------------------------------------------
//
void VideoListToolbar::loadOperatorService(int titleKey, int iconKey, int uriKey, int uidKey)
{
    FUNC_LOG;
    VideoOperatorService *service = new VideoOperatorService();
    if(service->load(titleKey, iconKey, uriKey, uidKey))
    {
        mVideoOperatorServices.append(service);
    }
    else
    {
        // Load failed, delete service data.
        delete service;
    }
}

// -------------------------------------------------------------------------------------------------
// addVideosToCollectionSlot()
// -------------------------------------------------------------------------------------------------
//
void VideoListToolbar::addVideosToCollectionSlot()
{
    FUNC_LOG;
    
    VideoListWidget* currentList = mListView->getCurrentList();
    
    if(!currentList || !currentList->getModel())
    {
        return;
    }

    VideoListSelectionDialog *dialog =
        mUiLoader->findWidget<VideoListSelectionDialog>(
            DOCML_NAME_DIALOG);
    if (!dialog)
    {
        // fatal: no selection dialog
        return;
    }
    
    // do not proceed in case it already have same amount
    // of videos than all videos view.
    VideoListWidget *allVideos = mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_VIDEOLISTWIDGET);
    if(allVideos && allVideos->getModel())
    {
        int count = allVideos->getModel()->rowCount();
        if(count == currentList->getModel()->rowCount())
        {
            if(count)
            {
                QVariant emptyAdditional;
                mUiUtils.showStatusMsgSlot(
                        VideoCollectionCommon::statusAllVideosAlreadyInCollection,
                        emptyAdditional);
            }
            return;
        }  
    }
    
    TMPXItemId collectionId = currentList->getModel()->getOpenItem();
    dialog->setupContent(VideoListSelectionDialog::EAddToCollection, collectionId);
    dialog->exec();
}

// -------------------------------------------------------------------------------------------------
// removeVideosFromCollectionSlot()
// -------------------------------------------------------------------------------------------------
//
void VideoListToolbar::removeVideosFromCollectionSlot()
{
    FUNC_LOG;

    VideoListWidget* currentList = mListView->getCurrentList();
    
    if(!currentList || !currentList->getModel())
    {
        return;
    }
    
    // not allowed if for some reason current widget 
    // is all videos or collection or there are no items
    if(currentList->getLevel() != VideoCollectionCommon::ELevelAlbum ||
       !currentList->getModel()->rowCount())
    {
        return;
    }

    VideoListSelectionDialog *dialog =
            mUiLoader->findWidget<VideoListSelectionDialog>(
                        DOCML_NAME_DIALOG);
    if (!dialog)
    {
        ERROR(-1, "VideoListView::removeVideosFromCollectionSlot() failed to load selection dialog.");
        return;
    }
    
    TMPXItemId collectionId = currentList->getModel()->getOpenItem();
    if(collectionId != TMPXItemId::InvalidId() && collectionId.iId2 == KVcxMvcMediaTypeAlbum)
    {
        dialog->setupContent(VideoListSelectionDialog::ERemoveFromCollection, collectionId);
        dialog->exec();
    }
}

// ---------------------------------------------------------------------------
// createAction()
// ---------------------------------------------------------------------------
//
HbAction* VideoListToolbar::createAction(QString icon,
        QActionGroup* actionGroup, const char *slot)
{
	FUNC_LOG;
    HbAction* action = new HbAction(actionGroup);

    HbIcon hbIcon(icon);
    action->setIcon(hbIcon);

    if(slot)
    {
        if(!connect(action, SIGNAL(triggered()), this, slot)) {
            // actiongroup deletion deletes this also.
            // return 0 tells that there was a problem in creation to caller.
            delete action;
            return 0;
        }
    }
    
    return action;
}

// End of file
