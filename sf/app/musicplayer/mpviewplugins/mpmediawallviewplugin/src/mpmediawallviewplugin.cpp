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
* Description: Plugin interface for Music Player media wall view.
*
*/

#include <xqplugin.h>

#include "mpmediawallviewplugin.h"
#include "mpmediawallview.h"
#include "mptrace.h"

/*!
    \class MpMediaWallViewPlugin
    \brief Plugin interface for Music Player media wall view.

    media wall view plugin provides interface to the media wall view
    beyond a plugin interface for application and view separation.
*/

/*!
    \fn void command( int command )

    This signal is emitted when the view issues a \a command to the
    application such as request to switch to a different view.
 */

/*!
 Constructs the media wall view plugin.
 */
MpMediaWallViewPlugin::MpMediaWallViewPlugin()
    : mView(0),
      mState(NullView)
{
    TX_LOG
}

/*!
 Destructs the media wall view plugin.
 */
MpMediaWallViewPlugin::~MpMediaWallViewPlugin()
{
    TX_LOG
}

/*!
 Creates the media wall view.
 */
void MpMediaWallViewPlugin::createView()
{
    TX_ENTRY
    if ( mState == NullView ) {
        mView = new MpMediaWallView();
        connect( mView, SIGNAL(command(int)), this, SIGNAL(command(int)) );
        mState = Created;
    }
    TX_EXIT
}

/*!
 Destroys the media wall view.
 */
void MpMediaWallViewPlugin::destroyView()
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
 Activates the media wall view. View initialization is done very first time.
 */
void MpMediaWallViewPlugin::activateView()
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
 Deactivates the media wall view.
 */
void MpMediaWallViewPlugin::deactivateView()
{
    TX_ENTRY
    if ( mState == Activated ) {
        mView->deactivateView();
        mState = Initialized;
    }
    TX_EXIT
}

/*!
 Returns pointer to QGraphicsWidget, which is the media wall view.
 The returned pointer is 0, if it is not created first.

 \sa createView()
 */
QGraphicsWidget* MpMediaWallViewPlugin::getView()
{
    TX_LOG
    return mView;
}

/*!
 Slot to be called when application orientation changes.

 \reimp
 */
void MpMediaWallViewPlugin::orientationChange( Qt::Orientation orientation )
{
    TX_LOG
    Q_UNUSED(orientation);
}

/*!
 Slot to handle back command from softkey.

 \reimp
 */
void MpMediaWallViewPlugin::back()
{
    //do nothing, softkey is handledinternally by the view.
    TX_LOG
}

XQ_EXPORT_PLUGIN2( mpmediawallviewplugin, MpMediaWallViewPlugin );

