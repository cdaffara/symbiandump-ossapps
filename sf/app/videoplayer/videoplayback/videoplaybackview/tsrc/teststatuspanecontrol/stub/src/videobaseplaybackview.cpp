/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of Video playback view
*
*/

// Version : %version:   6 %



//  Include Files
#include <hbinstance.h>
#include <HbAction.h>
#include <HbMenu.h>

#include "../inc/videobaseplaybackview.h"


//  Member Functions

// -------------------------------------------------------------------------------------------------
// VideoBasePlaybackView::VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::VideoBasePlaybackView()
{
    mMenu = new HbMenu();
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::~VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::~VideoBasePlaybackView()
{
    if ( mMenu )
    {
        delete mMenu;
        mMenu = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleActivateView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleActivateView()
{
    mViewActive = true;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleDeactivateView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleDeactivateView()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleClosePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleClosePlaybackView()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::doClosePlayer
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::doClosePlayer()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::closePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::closePlaybackView()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::startClosingPlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::startClosingPlaybackView()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::setTitleBarFlags()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::setViewFlags( HbView::HbViewFlags flags )
{
    mFlag = flags;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::viewFlags()
// -------------------------------------------------------------------------------------------------
//
HbView::HbViewFlags VideoBasePlaybackView::viewFlags()
{
    return mFlag;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::setTitleBarVisible()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::setTitleBarVisible( bool visible )
{
    mTitleBarVisible = visible;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::setStatusBarVisible()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::setStatusBarVisible( bool visible )
{
    mStatusBarVisible = visible;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::setNavigationAction()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::setNavigationAction( HbAction *action )
{
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::menu()
// -------------------------------------------------------------------------------------------------
//
HbMenu* VideoBasePlaybackView::menu()
{
    return mMenu;
}

// EOF
