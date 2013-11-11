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
* Description: Plugin interface for Music Player settings view.
*
*/

#include <xqplugin.h>

#include "mpsettingsviewplugin.h"
#include "mpsettingsview.h"
#include "mptrace.h"

/*!
    \class MpSettingsViewPlugin
    \brief Plugin interface for Music Player settings view.

    settings view plugin provides interface to the settings view
    beyond a plugin interface for application and view separation.
*/

/*!
    \fn void command( int command )

    This signal is emitted when the view issues a \a command to the
    application such as request to switch to a different view.
 */

/*!
 Constructs the settings view plugin.
 */
MpSettingsViewPlugin::MpSettingsViewPlugin()
    : mView(0),
      mState(NullView)
{
    TX_LOG
}

/*!
 Destructs the settings view plugin.
 */
MpSettingsViewPlugin::~MpSettingsViewPlugin()
{
    TX_LOG
}

/*!
 Creates the settings view.
 */
void MpSettingsViewPlugin::createView()
{
    TX_ENTRY
    if ( mState == NullView ) {
        mView = new MpSettingsView();
        connect( mView, SIGNAL(command(int)), this, SIGNAL(command(int)) );
        mState = Created;
    }
    TX_EXIT
}

/*!
 Destroys the settings view.
 */
void MpSettingsViewPlugin::destroyView()
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
 Activates the settings view. View initialization is done very first time.
 */
void MpSettingsViewPlugin::activateView()
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
 Deactivates the settings view.
 */
void MpSettingsViewPlugin::deactivateView()
{
    TX_ENTRY
    if ( mState == Activated ) {
        mView->deactivateView();
        mState = Initialized;
    }
    TX_EXIT
}

/*!
 Returns pointer to QGraphicsWidget, which is the settings view.
 The returned pointer is 0, if it is not created first.

 \sa createView()
 */
QGraphicsWidget* MpSettingsViewPlugin::getView()
{
    TX_LOG
    return mView;
}

/*!
 Slot to be called when application orientation changes.

 \reimp
 */
void MpSettingsViewPlugin::orientationChange( Qt::Orientation orientation )
{
    TX_LOG
    Q_UNUSED(orientation);
}

/*!
 Slot to handle back command from softkey.

 \reimp
 */
void MpSettingsViewPlugin::back()
{
    //do nothing, softkey is handledinternally by the view.
    TX_LOG
}

XQ_EXPORT_PLUGIN2( mpsettingsviewplugin, MpSettingsViewPlugin );

