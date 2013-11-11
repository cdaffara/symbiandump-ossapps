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

// Version : %version: 1 %

// INCLUDE FILES

#include <hbmenu.h>

#include "videolistmenu.h"
#include "videolistselectiondialog.h"
#include "videocollectionviewutils.h"
#include "videolistwidget.h"
#include "videolistview.h"
#include "videocollectioncommon.h"
#include "videoproxymodelgeneric.h"
#include "videocollectionuiloader.h"
#include "videocollectiontrace.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListMenu::VideoListMenu(VideoCollectionUiLoader *uiLoader, VideoListView* parent) 
    : QObject(parent)
    , mUiUtils(VideoCollectionViewUtils::instance())
    , mUiLoader(uiLoader)
    , mListView(parent)
    , mVideoServices(0)
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListMenu::~VideoListMenu()
{
	FUNC_LOG;
    
    mSortingRoles.clear();
    
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
int VideoListMenu::initializeMenu()
{
	FUNC_LOG;
	if(!mUiLoader)
	{
		return -1;
	}
	
    if (mUiUtils.isService())
    {
        INFO("VideoListMenu::initializeMenu() initializing service.");
        if (!mVideoServices)
        {
            mVideoServices = VideoServices::instance();
        }
         
    }
	
    // start loading objects and widgets
    QList<VideoCollectionUiLoaderParam> params;
    
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_OPTIONS_MENU,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_ADD_TO_COLLECTION,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_CREATE_COLLECTION,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_DELETE_MULTIPLE,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_MENU,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_DATE,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_NAME,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_SIZE,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    
    mUiLoader->addData(params,
        this,
        SLOT(objectReadySlot(QObject*, const QString&)));
    
    params.clear();
    
    return 0;
}

// ---------------------------------------------------------------------------
// showAction()
// ---------------------------------------------------------------------------
//
void VideoListMenu::showAction(bool show, const QString &name)
{
	FUNC_LOG;
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

    // hide or show action
    if (action)
    {
        action->setVisible(show);
    }
}

// ---------------------------------------------------------------------------
// startSorting()
// ---------------------------------------------------------------------------
//
void VideoListMenu::startSorting()
{
	FUNC_LOG;
	
    HbMenu *optionsMenu =
        mUiLoader->findWidget<HbMenu>(
            DOCML_NAME_OPTIONS_MENU);
    
    VideoListWidget* currentList = mListView->getCurrentList();
    
    if (optionsMenu && currentList)
    {
        // get sorting role from active action
        HbAction* action = optionsMenu->activeAction();
        HbMenu* sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
        if(action == sortMenu->menuAction()) // make sure that active action is the sort menu. 
        {
            HbAction* action = sortMenu->activeAction();
            if(action)
            {
                doSorting(mSortingRoles[action]);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// doSorting()
// ---------------------------------------------------------------------------
//
void VideoListMenu::doSorting(int role)
{
	FUNC_LOG;
	
    VideoListWidget* currentList = mListView->getCurrentList();
    
	if(!currentList || !currentList->getModel())
	{
	    // no list or model, cannot sort
	    return;
	}
	// sort model
	Qt::SortOrder order(Qt::AscendingOrder);
	VideoProxyModelGeneric *model = currentList->getModel();
	if(model->sortRole() == role && model->sortOrder() == Qt::AscendingOrder)
	{
		order = Qt::DescendingOrder;
	}
	model->doSorting(role, order);
	
	// for video related sorting, all videos list and collection content
	// list, sorting orders are same all the time
	VideoListWidget *anotherVideosList = 0;
	VideoCollectionCommon::TCollectionLevels level = currentList->getLevel();
	if (level == VideoCollectionCommon::ELevelDefaultColl ||
	    level == VideoCollectionCommon::ELevelAlbum)
	{
	    anotherVideosList = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
	}
	else if(level == VideoCollectionCommon::ELevelVideos)
	{
	    anotherVideosList = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
	}
    if(anotherVideosList && anotherVideosList->getModel())
    {
        anotherVideosList->getModel()->doSorting(role, order);
    }

    // save sorting values only if the application is not started as a service
	if (!mVideoServices)
	{
	    // save sorting values
	    mUiUtils.saveSortingValues(role, order, currentList->getLevel());
	}
}

// -------------------------------------------------------------------------------------------------
// deleteItemsSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::deleteItemsSlot()
{
	FUNC_LOG;

    VideoListSelectionDialog *dialog =
        mUiLoader->findWidget<VideoListSelectionDialog>(
            DOCML_NAME_DIALOG);
    
    if (dialog)
    {
        TMPXItemId collectionId = mListView->getCurrentList()->getModel()->getOpenItem();
        dialog->setupContent(VideoListSelectionDialog::EDeleteVideos, collectionId); 
        dialog->exec();
    }
}

// -------------------------------------------------------------------------------------------------
// createCollectionSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::createCollectionSlot()
{
	FUNC_LOG;
     
    VideoListSelectionDialog *dialog =
        mUiLoader->findWidget<VideoListSelectionDialog>(
            DOCML_NAME_DIALOG);
    
    if (!dialog)
    {
        // fatal: no selection dialog
        return;
    }
    
    dialog->setupContent(VideoListSelectionDialog::ECreateCollection, TMPXItemId::InvalidId());
    dialog->exec();
}

// -------------------------------------------------------------------------------------------------
// addVideosToCollectionSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::addVideosToCollectionSlot()
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

    TMPXItemId collectionId = currentList->getModel()->getOpenItem();
    dialog->setupContent(VideoListSelectionDialog::EAddToCollection, collectionId);
    dialog->exec();
}

// -------------------------------------------------------------------------------------------------
// aboutToShowMainMenuSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::aboutToShowMainMenuSlot()
{
    if (mVideoServices &&
        mVideoServices->currentService() == VideoServices::EBrowse)
    {
        prepareBrowseServiceMenu();
        return;
    }
    
    VideoListWidget* currentList = mListView->getCurrentList();
    
	if (!currentList)
	{
		return;
	}
	
	// hide all actions by default
    showAction(false, DOCML_NAME_ADD_TO_COLLECTION);
    showAction(false, DOCML_NAME_CREATE_COLLECTION);
    showAction(false, DOCML_NAME_DELETE_MULTIPLE);
    showAction(false, DOCML_NAME_SORT_BY_DATE);
    showAction(false, DOCML_NAME_SORT_BY_NAME);
    showAction(false, DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    showAction(false, DOCML_NAME_SORT_BY_SIZE);
    showAction(false, DOCML_NAME_SORT_MENU);

    // Create collection action is shown even when there's no videos. 
    if(currentList->getLevel() == VideoCollectionCommon::ELevelCategory && !mVideoServices)
    {
        showAction(true, DOCML_NAME_CREATE_COLLECTION);
    }
    
    //  No other actions shown if there's no videos.
    VideoProxyModelGeneric *model = currentList->getModel();
    if (!model || !model->rowCount())
    {
        return;
    }
    
    // get current sorting values
    int role;
    Qt::SortOrder order;
    model->getSorting(role, order);

    if(currentList->getLevel() == VideoCollectionCommon::ELevelVideos)
    {
        showAction(true, DOCML_NAME_SORT_MENU);
        showAction(true, DOCML_NAME_SORT_BY_DATE);
        showAction(true, DOCML_NAME_SORT_BY_NAME);
        showAction(true, DOCML_NAME_SORT_BY_SIZE);

        HbAction* action = mSortingRoles.key(role);
		if (action)
		{
			action->setChecked(true);
		}

        if (!mVideoServices)
        {
            showAction(true, DOCML_NAME_ADD_TO_COLLECTION);
            showAction(true, DOCML_NAME_DELETE_MULTIPLE);
        }
    }
    else if(currentList->getLevel() == VideoCollectionCommon::ELevelCategory)
    {
        showAction(true, DOCML_NAME_SORT_MENU);
        showAction(true, DOCML_NAME_SORT_BY_NAME);
        showAction(true, DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    	
		HbAction* action = mSortingRoles.key(role);
		if (action)
		{
			action->setChecked(true);
		}
    }
    else if(currentList->getLevel() == VideoCollectionCommon::ELevelDefaultColl ||
            currentList->getLevel() == VideoCollectionCommon::ELevelAlbum)
    {
        showAction(true, DOCML_NAME_SORT_MENU);
        showAction(true, DOCML_NAME_SORT_BY_DATE);
        showAction(true, DOCML_NAME_SORT_BY_NAME);
        showAction(true, DOCML_NAME_SORT_BY_SIZE);

        HbAction* action = mSortingRoles.key(role);
		if (action)
		{
			action->setChecked(true);
		}

        if (!mVideoServices)
        {
            showAction(true, DOCML_NAME_DELETE_MULTIPLE);
        }
    }
}

// -------------------------------------------------------------------------------------------------
// prepareBrowseServiceMenu
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::prepareBrowseServiceMenu()
{
    VideoListWidget* currentList = mListView->getCurrentList();
    
    if (!currentList)
    {
        return;
    }
    
    // hide all actions by default
    showAction(false, DOCML_NAME_ADD_TO_COLLECTION);
    showAction(false, DOCML_NAME_CREATE_COLLECTION);
    showAction(false, DOCML_NAME_DELETE_MULTIPLE);
    showAction(false, DOCML_NAME_SORT_BY_DATE);
    showAction(false, DOCML_NAME_SORT_BY_NAME);
    showAction(false, DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    showAction(false, DOCML_NAME_SORT_BY_SIZE);
    showAction(false, DOCML_NAME_SORT_MENU);
    
    VideoProxyModelGeneric *model = currentList->getModel();
    if (!model || !model->rowCount())
    {
        return;
    }
    
    // show delete action
    showAction(true, DOCML_NAME_DELETE_MULTIPLE);
    
    // show sort actions
    showAction(true, DOCML_NAME_SORT_MENU);
    showAction(true, DOCML_NAME_SORT_BY_DATE);
    showAction(true, DOCML_NAME_SORT_BY_NAME);
    showAction(true, DOCML_NAME_SORT_BY_SIZE);
    
    // set current sort action selected
    int role;
    Qt::SortOrder order;
    model->getSorting(role, order);
    HbAction* action = mSortingRoles.key(role);
    if (action)
    {
        action->setChecked(true);
    }
}

// -------------------------------------------------------------------------------------------------
// objectReadySlot
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::objectReadySlot(QObject *object, const QString &name)
{
	FUNC_LOG;
    
	if (name.compare(DOCML_NAME_OPTIONS_MENU) == 0)
    {
        connect(
            object, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMainMenuSlot()));
    }
    else if (name.compare(DOCML_NAME_SORT_BY_DATE) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
        	connect(action, SIGNAL(triggered()), this, SLOT(startSorting()));
            mSortingRoles[action] = VideoCollectionCommon::KeyDateTime;
        }
    }
    else if (name.compare(DOCML_NAME_SORT_BY_NAME) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
        	connect(action, SIGNAL(triggered()), this, SLOT(startSorting()));
            mSortingRoles[action] = VideoCollectionCommon::KeyTitle;
        }
    }
    else if (name.compare(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
            connect(action, SIGNAL(triggered()), this, SLOT(startSorting()));
            mSortingRoles[action] = VideoCollectionCommon::KeyNumberOfItems;
        }
    }
    else if (name.compare(DOCML_NAME_SORT_BY_SIZE) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
        	connect(action, SIGNAL(triggered()), this, SLOT(startSorting()));
            mSortingRoles[action] = VideoCollectionCommon::KeySizeValue;
        }
    }
    else if (name.compare(DOCML_NAME_ADD_TO_COLLECTION) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
            connect(action, SIGNAL(triggered()), this, SLOT(addVideosToCollectionSlot()));
        }
    }
    else if (name.compare(DOCML_NAME_CREATE_COLLECTION) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
            connect(action, SIGNAL(triggered()), this, SLOT(createCollectionSlot()));
        }
    }
    else if (name.compare(DOCML_NAME_DELETE_MULTIPLE) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
            connect(action, SIGNAL(triggered()), this, SLOT(deleteItemsSlot()));
        }
    }
}

// End of file
