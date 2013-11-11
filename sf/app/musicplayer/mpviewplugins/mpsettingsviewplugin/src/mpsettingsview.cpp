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
* Description: Music Player Settings view.
*
*/

#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbapplication.h>

#include "mpsettingsview.h"
#include "mpsettingsaudioeffectswidget.h"
#include "mpcommondefs.h"
#include "mptrace.h"

/*!
    \class MpSettingsView
    \brief Music Player Settings view.

    Music player audio settings and audio effects are grouped under this view
*/

/*!
    \fn void command( int command )

    This signal is emitted when the view issues a \a command to the
    application such as request to switch to a different view.
 */

/*!
 Constructs the settings view.
 */
MpSettingsView::MpSettingsView()
    : mWindow(0),
      mSoftKeyBack(0),
      mAudioEffectsWidget(0)
{
    TX_LOG
}

/*!
 Destructs the settings view.
 */
MpSettingsView::~MpSettingsView()
{
    TX_ENTRY
    delete mSoftKeyBack;
    TX_EXIT
}

/*!
 Initializes the settings view.
 */
void MpSettingsView::initializeView()
{
    TX_ENTRY

    mWindow = mainWindow();
    mSoftKeyBack = new HbAction(Hb::BackNaviAction, this);
    mAudioEffectsWidget = new MpSettingsAudioEffectsWidget(this);
    mAudioEffectsWidget->initialize();
    setWidget( mAudioEffectsWidget );

    connect( mSoftKeyBack,
                SIGNAL(triggered()),
                this,
                SLOT(back()) );

    connect( mSoftKeyBack,
                SIGNAL(triggered()),
                mAudioEffectsWidget,
                SLOT(persistBalance()) );
    TX_EXIT
}

/*!
 Activates the settings view.
 */
void MpSettingsView::activateView()
{
    TX_ENTRY
    setNavigationAction( mSoftKeyBack );
    TX_EXIT
}

/*!
 Deactivates the settings view.
 */
void MpSettingsView::deactivateView()
{
    TX_ENTRY
    setNavigationAction( 0 );
    TX_EXIT
}

/*!
 Slot to handle back command from softkey.
 */
void MpSettingsView::back()
{
    TX_ENTRY
    emit command( MpCommon::ActivatePlaybackView );
    TX_EXIT
}
