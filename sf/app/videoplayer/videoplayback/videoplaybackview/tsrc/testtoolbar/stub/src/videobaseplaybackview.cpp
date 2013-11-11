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

// Version : %version:   3 %



//  Include Files

#include <hbinstance.h>
#include <coecntrl.h>

#include "hbtoolbar.h"
#include "videobaseplaybackview.h"

//  Member Functions

// -------------------------------------------------------------------------------------------------
// VideoBasePlaybackView::VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::VideoBasePlaybackView()
{
     mToolBar = new HbToolBar();
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::~VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::~VideoBasePlaybackView()
{
    if ( mToolBar )
    {
         delete mToolBar;
         mToolBar = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::toolBar()
// -------------------------------------------------------------------------------------------------
//
HbToolBar* VideoBasePlaybackView::toolBar()
{
    return mToolBar;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::hideItems()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::hideItems( int item )
{
    Q_UNUSED( item );
    mToolBar->setVisible( false );
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::hideItems()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::showItems( int item )
{
    Q_UNUSED( item );
    mToolBar->setVisible( true );
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::getWindow()
// -------------------------------------------------------------------------------------------------
//
RWindow *VideoBasePlaybackView::getWindow()
{
    return static_cast<RWindow*>( hbInstance->allMainWindows()[0]->effectiveWinId()->DrawableWindow() );
}

// EOF
