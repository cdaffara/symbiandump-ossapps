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

// Version : %version: 2 %

// INCLUDE FILES
#include <qactiongroup.h>
#include <hbaction.h>

#include "videolisttoolbar.h"
#include "videolisttoolbardata.h"
#include "videolistview.h"
#include "videocollectionuiloader.h"
#include "videocollectioncommon.h"
#include "videocollectionviewutils.h"

int VideoListToolbarData::mViewStateChangedCount(0);
VideoCollectionCommon::TCollectionLevels VideoListToolbarData::mViewStateChangedLevel(
    VideoCollectionCommon::ELevelInvalid);
bool VideoListToolbarData::mViewStateChangedNoVideos(false);
bool VideoListToolbarData::mViewStateChangedModelReady(false);

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListToolbar::VideoListToolbar(VideoCollectionUiLoader* uiLoader, VideoListView* parent)
    : QObject(parent)
    , mUiUtils(VideoCollectionViewUtils::instance())
{
    Q_UNUSED(uiLoader);
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListToolbar::~VideoListToolbar()
{
}

// ---------------------------------------------------------------------------
// initialize()
// ---------------------------------------------------------------------------
//
void VideoListToolbar::initialize()
{
}

// ---------------------------------------------------------------------------
// viewStateChanged()
// ---------------------------------------------------------------------------
//
void VideoListToolbar::viewStateChanged(
    VideoCollectionCommon::TCollectionLevels currentLevel, 
    bool noVideos, bool modelReady)
{
    VideoListToolbarData::mViewStateChangedCount++;
    VideoListToolbarData::mViewStateChangedLevel = currentLevel;
    VideoListToolbarData::mViewStateChangedNoVideos = noVideos;
    VideoListToolbarData::mViewStateChangedModelReady = modelReady;
}

// ---------------------------------------------------------------------------
// createToolbarActions()
// Creates toolbar actions and toolbar icons
// ---------------------------------------------------------------------------
//
void VideoListToolbar::createToolbarActions()
{
}

// ---------------------------------------------------------------------------
// createOperatorServicesToolbar()
// ---------------------------------------------------------------------------
//
void VideoListToolbar::createOperatorServicesToolbarActions()
{
}

// ---------------------------------------------------------------------------
// loadOperatorService()
// ---------------------------------------------------------------------------
//
void VideoListToolbar::loadOperatorService(int titleKey, int iconKey, int uriKey, int uidKey)
{
    Q_UNUSED(titleKey);
    Q_UNUSED(iconKey);
    Q_UNUSED(uriKey);
    Q_UNUSED(uidKey);
}

// -------------------------------------------------------------------------------------------------
// addVideosToCollectionSlot()
// -------------------------------------------------------------------------------------------------
//
void VideoListToolbar::addVideosToCollectionSlot()
{
}

// -------------------------------------------------------------------------------------------------
// removeVideosFromCollectionSlot()
// -------------------------------------------------------------------------------------------------
//
void VideoListToolbar::removeVideosFromCollectionSlot()
{
}

// ---------------------------------------------------------------------------
// createAction()
// ---------------------------------------------------------------------------
//
HbAction* VideoListToolbar::createAction(QString icon,
        QActionGroup* actionGroup, const char *slot)
{
    Q_UNUSED(icon);
    Q_UNUSED(actionGroup);
    Q_UNUSED(slot);
    
    return 0;
}

// End of file
