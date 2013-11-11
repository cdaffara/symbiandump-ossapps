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

#include "videolistmenu.h"
#include "videolistview.h"
#include "videocollectionuiloader.h"
#include "videocollectionviewutils.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListMenu::VideoListMenu(VideoCollectionUiLoader *uiLoader, VideoListView* parent) 
    : QObject(parent)
    , mUiUtils(VideoCollectionViewUtils::instance())
{
    Q_UNUSED(uiLoader);
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListMenu::~VideoListMenu()
{
}

// ---------------------------------------------------------------------------
// initializeView()
// ---------------------------------------------------------------------------
//
int VideoListMenu::initializeMenu()
{
    return 0;
}

// ---------------------------------------------------------------------------
// showAction()
// ---------------------------------------------------------------------------
//
void VideoListMenu::showAction(bool show, const QString &name)
{
    Q_UNUSED(show);
    Q_UNUSED(name);
}

// ---------------------------------------------------------------------------
// startSorting()
// ---------------------------------------------------------------------------
//
void VideoListMenu::startSorting()
{
}

// ---------------------------------------------------------------------------
// doSorting()
// ---------------------------------------------------------------------------
//
void VideoListMenu::doSorting(int role)
{
    Q_UNUSED(role);
}

// -------------------------------------------------------------------------------------------------
// deleteItemsSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::deleteItemsSlot()
{
}

// -------------------------------------------------------------------------------------------------
// createCollectionSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::createCollectionSlot()
{
}

// -------------------------------------------------------------------------------------------------
// addVideosToCollectionSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::addVideosToCollectionSlot()
{
}

// -------------------------------------------------------------------------------------------------
// aboutToShowMainMenuSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::aboutToShowMainMenuSlot()
{
}

// -------------------------------------------------------------------------------------------------
// prepareBrowseServiceMenu
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::prepareBrowseServiceMenu()
{
}

// -------------------------------------------------------------------------------------------------
// objectReadySlot
// -------------------------------------------------------------------------------------------------
//
void VideoListMenu::objectReadySlot(QObject *object, const QString &name)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
}

// End of file
