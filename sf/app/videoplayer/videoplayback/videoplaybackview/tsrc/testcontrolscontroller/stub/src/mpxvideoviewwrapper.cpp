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
* Description:   Implementation of Video wrapper
*
*/

// Version : %version:  5 %



//  Include Files

#include "mpxvideo_debug.h"
#include "mpxvideoviewwrapper.h"
#include "videobaseplaybackview.h"
#include "videoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"

//  Member Functions

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::CMPXVideoViewWrapper()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoViewWrapper::CMPXVideoViewWrapper( VideoBasePlaybackView* aView )
    : mView( aView )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::NewL()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoViewWrapper* CMPXVideoViewWrapper::NewL( VideoBasePlaybackView* aView )
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::CMPXVideoViewWrapper()"));
    CMPXVideoViewWrapper* self = new (ELeave) CMPXVideoViewWrapper( aView );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::ConstructL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::ConstructL()
{
    mMediaRequested = EFalse;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::~CMPXVideoViewWrapper()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoViewWrapper::~CMPXVideoViewWrapper()
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::~CMPXVideoViewWrapper"));
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::HandleCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandleCommandL( TInt aCommand )
{
    mCommandId = aCommand;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::SetPropertyL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::SetPropertyL( TMPXPlaybackProperty aProperty, TInt aValue )
{
    mProperty = aProperty;
    Q_UNUSED( aValue );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::UpdateVideoRect()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::UpdateVideoRect(
        TInt aX, TInt aY, TInt aWidth, TInt aHeight, TBool transitionEffect )
{
    Q_UNUSED( aX );
    Q_UNUSED( aY );
    Q_UNUSED( aWidth );
    Q_UNUSED( aHeight );
    Q_UNUSED( transitionEffect );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::IsResumingPlaybackAfterTermination()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoViewWrapper::IsResumingPlaybackAfterTermination()
{
    return false;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::VolumeSteps()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoViewWrapper::VolumeSteps()
{
    return mVolumeSteps;
}

// EOF
