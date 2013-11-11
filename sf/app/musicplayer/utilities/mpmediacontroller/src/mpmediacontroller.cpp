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
* Description: Music Player Media Controller.
*
*/


#include "mpmediacontroller.h"
#include "mpmediakeyhandler.h"
#include "mpvolumeslider.h"
#include "mptrace.h"


/*!
 Constructs the MpMediaController.
 */
MpMediaController::MpMediaController( QObject *parent )
    : QObject( parent ),
      mMpMediaKeyHandler( 0 ),
      mMpVolumeSlider( 0 )
{
    TX_ENTRY
    mMpMediaKeyHandler = new MpMediaKeyHandler();
    mMpVolumeSlider = new MpVolumeSlider();
    TX_EXIT
}

/*!
 Destructs MpMediaController.
 */
MpMediaController::~MpMediaController()
{
    delete mMpMediaKeyHandler;
    delete mMpVolumeSlider;
}

