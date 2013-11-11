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
* Description:  Implementation of Video playback view
*
*/

// Version : %version:  8 %



//  Include Files
#include <aknappui.h>
#include <qgraphicssceneevent>
#include <hbinstance.h>
#include <mmf/common/mmferrors.h>

#include "../inc/videobaseplaybackview.h"


//  Member Functions

// -------------------------------------------------------------------------------------------------
// VideoBasePlaybackView::VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::VideoBasePlaybackView()
{
    mCurrentError = KErrNone;
    mViewActive = false;
    mSyncClose = false;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::~VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::~VideoBasePlaybackView()
{
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
// VideoBasePlaybackView::mousePress
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::mousePressEvent( QGraphicsSceneMouseEvent *event ) 
{
    Q_UNUSED( event );
}

// -------------------------------------------------------------------------------------------------
// VideoBasePlaybackView::itemChange
// -------------------------------------------------------------------------------------------------
//
QVariant VideoBasePlaybackView::itemChange( GraphicsItemChange change, const QVariant &value )
{
    return QGraphicsWidget::itemChange( change, value );
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::closePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::paint( QPainter *painter, 
                                     const QStyleOptionGraphicsItem *option, 
                                     QWidget *widget )
{
    Q_UNUSED( painter );
    Q_UNUSED( option );
    Q_UNUSED( widget );    
}


// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::event()
// -------------------------------------------------------------------------------------------------
//
bool VideoBasePlaybackView::event( QEvent *event )
{
    Q_UNUSED( event );
    bool consumed = false;

    return consumed;
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
//   VideoBasePlaybackView::issuePlayCommand
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::issuePlayCommand()
{
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleBufferingState
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleBufferingState()
{
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handlePluginError()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handlePluginError( int aError )
{
    mCurrentError = aError;
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
//   VideoBasePlaybackView::retrievePdlInformation
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::retrievePdlInformation()
{
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
//   VideoBasePlaybackView::getWindow()
// -------------------------------------------------------------------------------------------------
//
RWindow *VideoBasePlaybackView::getWindow()
{
    return static_cast<RWindow*>( hbInstance->allMainWindows()[0]->effectiveWinId()->DrawableWindow() );
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleClosePopupDialog()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleClosePopupDialog()
{
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleStoppedState()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleStoppedState()
{
    
}

// EOF
