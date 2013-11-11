/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Seek or trickplay
 *
*/

// Version : %version: 10 %


// INCLUDE FILES
#include <mpxplaybackpluginobserver.h>

#include "mpxvideoseeker.h"
#include "mpxvideoplaybackcontroller.h"
#include "mpxvideo_debug.h"

// CONSTANTS

#if TRICK_PLAY
const TInt KDefaultPlaySpeed(100);        // 1x
const TInt KDefaultTrickPlaySpeed(400);   // 4x
#else
const TInt KTrickPlayTimeout(2000000);    // 2 sec
const TInt KSpeedInterval(125000);        // 125 msec
const TInt KDurationA(90000000);          // 90 sec
const TInt KDurationB(180000000);         // 180 sec
const TInt64 KSpeedAIncrements(2000000);  // 2 sec
const TInt64 KSpeedBIncrements(4000000);  // 4 sec
const TInt64 KSpeedOffset(3000000);       // 3 sec
#endif

// ============================ MEMBER FUNCTIONS ===================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::CMPXVideoSeeker
// C++ default constructor can NOT contain any code, that might leave.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoSeeker::CMPXVideoSeeker( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
    : iVideoPlaybackCtlr( aVideoPlaybackCtlr )
{
    MPX_DEBUG(_L("CMPXVideoSeeker::CMPXVideoSeeker()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::ConstructL
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoSeeker::ConstructL()
{
    MPX_DEBUG(_L("CMPXVideoSeeker::ConstructL()"));

    iTrickPlayTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iSeekTimer = CPeriodic::NewL( CActive::EPriorityStandard );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::NewL
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoSeeker* CMPXVideoSeeker::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_DEBUG(_L("CMPXVideoSeeker::NewL()"));

    CMPXVideoSeeker* self = new( ELeave ) CMPXVideoSeeker( aVideoPlaybackCtlr );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::~CMPXVideoSeeker
// Destructor
// -------------------------------------------------------------------------------------------------
//
CMPXVideoSeeker::~CMPXVideoSeeker()
{
    MPX_DEBUG(_L("CMPXVideoSeeker::~CMPXVideoSeeker()"));

    if ( iTrickPlayTimer )
    {
        iTrickPlayTimer->Cancel();
        delete iTrickPlayTimer;
    }

    if ( iSeekTimer )
    {
        iSeekTimer->Cancel();
        delete iSeekTimer;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::StartSeeking
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoSeeker::StartSeekingL( TBool aForward,
                                     TMPXVideoPlaybackState aState,
                                     TInt64 aPosition,
                                     TInt64 aDuration )
{
    MPX_DEBUG(_L("CMPXVideoSeeker::StartSeekingL() forward = (%d)"), aForward );

    TInt err = KErrNone;
    iPreviousState = aState;
    iForward = aForward;
    iPosition = aPosition;
    iDuration = aDuration;

#if TRICK_PLAY
    if ( iPreviousState == EMPXVideoPaused )
    {
        iVideoPlaybackCtlr->iPlayer->Play();
    }

    TInt trickplaySpeed = KDefaultTrickPlaySpeed;

    if ( ! iForward )
    {
        trickplaySpeed *= -1;
    }

    //
    // Start trick play with 4x speed as default
    //
    MPX_TRAP( iVideoPlaybackCtlr->iPlayer->SetPlayVelocityL( trickplaySpeed ) );

#else
    if ( iPreviousState == EMPXVideoPlaying )
    {
        iVideoPlaybackCtlr->iPlayer->PauseL();
    }

    iIncrements = KSpeedAIncrements / 2;
    TInt interval = KSpeedInterval * 2;

    if ( !iForward )
    {
        iIncrements *= -1;
    }

    if ( ! iSeekTimer->IsActive() )
    {
        iSeekTimer->Start( 0,
                           interval,
                           TCallBack( CMPXVideoSeeker::UpdatePosition, this ) );
    }
#endif

    if ( err == KErrNone )
    {
        if ( ! iTrickPlayTimer->IsActive() )
        {
            iTrickPlayTimer->Start( KTrickPlayTimeout,
                                    0,
                                    TCallBack( CMPXVideoSeeker::StopTrickPlay, this ) );
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::StopSeekingL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoSeeker::StopSeekingL()
{
    MPX_DEBUG(_L("CMPXVideoSeeker::StopSeekingL()"));

    TBool seekTimerActive = EFalse;

    if ( iTrickPlayTimer->IsActive() )
    {
        iTrickPlayTimer->Cancel();
    }

    if ( iSeekTimer->IsActive() )
    {
        seekTimerActive = ETrue;
        iSeekTimer->Cancel();
    }

    //
    // If it reaches the end of a clip, issue stop
    //
    if ( iDuration <= iPosition )
    {
        iVideoPlaybackCtlr->iState->HandleEndOfClip();
    }
    else
    {
        if ( seekTimerActive )
        {
            iVideoPlaybackCtlr->iPlayer->SetPositionL( iPosition );

            if ( iPreviousState == EMPXVideoPlaying )
            {
                iVideoPlaybackCtlr->DoHandleCommandL( EPbCmdPlay );
            }
            else if (iPreviousState == EMPXVideoPaused )
            {
                iVideoPlaybackCtlr->DoHandleCommandL( EPbCmdPause );
            }
            else
            {
                iVideoPlaybackCtlr->ChangeState( iPreviousState );
            }
        }

#if TRICK_PLAY
        else
        {
            //
            // Start trick play with default speed
            //
            MPX_TRAP( iVideoPlaybackCtlr->iPlayer->SetPlayVelocityL( KDefaultPlaySpeed ) );

            if ( iPreviousState == EMPXVideoPaused )
            {
                iVideoPlaybackCtlr->DoHandleCommandL( EPbCmdPause );
            }
            else if ( iPreviousState == EMPXVideoPlaying )
            {
                iVideoPlaybackCtlr->DoHandleCommandL( EPbCmdPlay );
            }
        }
#endif
    }

}

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::StopTrickPlay
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoSeeker::StopTrickPlay( TAny* aPtr )
{
    MPX_DEBUG(_L("CMPXVideoSeeker::StopTrickPlay()"));

	static_cast<CMPXVideoSeeker*>(aPtr)->DoStopTrickPlay();

	return KErrNone;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::DoStopTrickPlay
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoSeeker::DoStopTrickPlay()
{
    MPX_DEBUG(_L("CMPXVideoSeeker::DoStopTrickPlay()"));

    TInt err = KErrNone;

    //
    // Stop trick play
    //
    if ( iTrickPlayTimer->IsActive() )
    {
        iTrickPlayTimer->Cancel();
    }

#if TRICK_PLAY
    MPX_TRAP( err,
              iVideoPlaybackCtlr->iPlayer->SetPlayVelocityL( KDefaultPlaySpeed );
              iVideoPlaybackCtlr->iPlayer->PauseL() );
#endif

    if ( err == KErrNone )
    {
        ConvertToSeeking();
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::ConvertToSeeking
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoSeeker::ConvertToSeeking()
{
    MPX_DEBUG(_L("CMPXVideoSeeker::ConvertToSeeking()"));

    //
    // Start seeking (increments in every 125 msec)
    // - ~ 90sec      : Increment is 2 sec
    // - 91sec~180sec : Increment is 4 sec
    // - 181sec~      : Increment depends on the duration
    //                  ( 0.5 % * duration ) + 3 sec
    //
    if ( iDuration < KDurationA )
    {
        iIncrements = KSpeedAIncrements;
    }
    else
    {
        if ( iDuration < KDurationB )
        {
            iIncrements = KSpeedBIncrements;
        }
        else
        {
            iIncrements = (TInt)( iDuration / 200 ) + KSpeedOffset;
        }
    }

    if ( !iForward )
    {
        iIncrements *= -1;
    }

    if ( iSeekTimer->IsActive() )
    {
        iSeekTimer->Cancel();
    }

    iSeekTimer->Start( 0, KSpeedInterval, TCallBack( CMPXVideoSeeker::UpdatePosition, this ) );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::UpdatePosition
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoSeeker::UpdatePosition( TAny* aPtr )
{
    MPX_DEBUG(_L("CMPXVideoSeeker::UpdatePosition()"));

    static_cast<CMPXVideoSeeker*>(aPtr)->DoUpdatePosition();
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoSeeker::DoUpdatePosition
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoSeeker::DoUpdatePosition()
{
    MPX_DEBUG(_L("CMPXVideoSeeker::DoUpdatePosition()"));

    iPosition += iIncrements;

    if ( iPosition > iDuration )
    {
        iPosition = iDuration;
    }
    else if ( iPosition < 0 )
    {
        iPosition = 0;
    }

    iVideoPlaybackCtlr->iPlaybackMode->UpdateSeekPosition( iPosition );
}

//  End of File
