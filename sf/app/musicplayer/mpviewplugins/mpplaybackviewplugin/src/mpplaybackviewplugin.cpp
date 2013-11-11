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
* Description: Plugin interface for Music Player playback view.
*
*/

#include <xqplugin.h>

#include "mpplaybackviewplugin.h"
#include "mpplaybackview.h"
#include "mptrace.h"

/*!
    \class MpPlaybackViewPlugin
    \brief Plugin interface for Music Player playback view.

    playback view plugin provides interface to the playback view
    beyond a plugin interface for application and view separation.
*/

/*!
    \fn void command( int command )

    This signal is emitted when the view issues a \a command to the
    application such as request to switch to a different view.
 */

/*!
 Constructs the playback view plugin.
 */
MpPlaybackViewPlugin::MpPlaybackViewPlugin()
    : mView(0),
      mState(NullView)
{
    TX_LOG
}

/*!
 Destructs the playback view plugin.
 */
MpPlaybackViewPlugin::~MpPlaybackViewPlugin()
{
    TX_LOG
}

/*!
 Creates the playback view.
 */
void MpPlaybackViewPlugin::createView()
{
    TX_ENTRY
    if ( mState == NullView ) {
        mView = new MpPlaybackView();
        connect( mView, SIGNAL(command(int)), this, SIGNAL(command(int)) );
        mState = Created;
    }
    TX_EXIT
}

/*!
 Destroys the playback view.
 */
void MpPlaybackViewPlugin::destroyView()
{
    TX_ENTRY
    if ( mState != NullView ) {
        delete mView;
        mView = 0;
        mState = NullView;
    }
    TX_EXIT
}

/*!
 Activates the playback view. View initialization is done very first time.
 */
void MpPlaybackViewPlugin::activateView()
{
    TX_ENTRY_ARGS("mState=" << mState);
    switch ( mState ) {
    case Created:
        mView->initializeView();
        mView->activateView();
        mState = Activated;
        break;
    case Initialized:
        mView->activateView();
        mState = Activated;
        break;
    default:
        // Ignore
        break;
    }
    TX_EXIT
}

/*!
 Deactivates the playback view.
 */
void MpPlaybackViewPlugin::deactivateView()
{
    TX_ENTRY
    if ( mState == Activated ) {
        mView->deactivateView();
        mState = Initialized;
    }
    TX_EXIT
}

/*!
 Returns pointer to QGraphicsWidget, which is the playback view.
 The returned pointer is 0, if it is not created first.

 \sa createView()
 */
QGraphicsWidget* MpPlaybackViewPlugin::getView()
{
    TX_LOG
    return mView;
}

/*!
 Slot to be called when application orientation changes.

 \reimp
 */
void MpPlaybackViewPlugin::orientationChange( Qt::Orientation orientation )
{
    TX_LOG
    Q_UNUSED(orientation);
}

/*!
 Slot to handle back command from softkey.

 \reimp
 */
void MpPlaybackViewPlugin::back()
{
    //do nothing, softkey is handledinternally by the view.
    TX_LOG
}

XQ_EXPORT_PLUGIN2( mpplaybackviewplugin, MpPlaybackViewPlugin );

