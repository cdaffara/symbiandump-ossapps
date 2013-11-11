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
* Description:  Media state class.
*
*/


#include <cvtlogger.h>
#include <mvtengmedia.h>
#include <mvtengcommandhandler.h>

#include "tlcvtmediastate.h"
#include "lcvtutility.h"

// -----------------------------------------------------------------------------
// TLcVtMediaState::UpdateFlags
// -----------------------------------------------------------------------------
//
void TLcVtMediaState::UpdateFlags()
    {
    __VTPRINTENTER( "MediaState.UpdateFlags" )
    TInt avail( 0 );

    LcVtUtility::GetOutgoingMediaState( iMedia, avail );

    if ( avail & MVtEngMedia::EMediaVideo )
        {
        SetFlag( EIsVideo );
        }

    if ( LcVtUtility::HasCameras( iMedia ) )
        {
        SetFlag( EHasCamera );
        }

    if ( LcVtUtility::IsFreezeSupported( iMedia ) )
        {
        SetFlag( EIsFreezeSupported );
        }

    if ( LcVtUtility::GetFreezeState( iMedia ) )
        {
        SetFlag( EIsFrozen );
        }

    MVtEngMedia::TShareObjectState shareObjectState;
    LcVtUtility::GetObjectSharingState( iMedia, shareObjectState );

    if ( shareObjectState == MVtEngMedia::ESharingImage )
        {
        SetFlag( EIsSharing );
        }
    else if ( ( shareObjectState == MVtEngMedia::EInitializingShareImage ) ||
         ( shareObjectState == MVtEngMedia::EInitializingShareVideoClip ) )
        {
        SetFlag( EIsInitializingShare );
        }
    else if ( shareObjectState == MVtEngMedia::ENotAbleToShare )
        {
        SetFlag( EIsNotAbleToShare );
        }
    // Camera configuration

    MVtEngMedia::TCameraId cameraId;
    TInt err = iMedia.GetCurrentCameraId( cameraId );

    // Don't care if camera is not ready, just what is selected camera
    if ( err == KErrNone || err == KErrNotReady )
        {
        SetFlag( ( cameraId == MVtEngMedia::EPrimaryCamera ) ?
            EIsPrimaryCameraInUse : EIsSecondaryCameraInUse );
        }

    TInt sourcesCaps( 0 );
    iMedia.GetSourcesCaps( sourcesCaps );
    if ( sourcesCaps & MVtEngMedia::ESourceCapsPrimaryCamera )
        {
        SetFlag( EHasPrimaryCamera );
        }
    if ( sourcesCaps & MVtEngMedia::ESourceCapsSecondaryCamera )
        {
        SetFlag( EHasSecondaryCamera );
        }
       
    TInt status ( 0 ); 
    iMedia.GetMediaState( MVtEngMedia::EMediaOutgoing, status );
    if ( !( status & MVtEngMedia::EMediaVideoChannelOpen ) )
        {        
        SetFlag( EIsVideoPermanentlyStopped );
        }      

    __VTPRINTEXITR( "MediaState.UpdateFlags bits = %b", Value() )

    // Video preferences
/*
    TInt caps( iCommandHandler.GetCommandCaps( KVtEngSetContrast ) );
    if ( caps & MVtEngCommandHandler::EAttribEnabled )
        {
        SetFlag( EIsContrastSupported );
        }

    caps = iCommandHandler.GetCommandCaps( KVtEngSetBrightness );
    if ( caps & MVtEngCommandHandler::EAttribEnabled )
        {
        SetFlag( EIsBrightnessSupported );
        }

    caps = iCommandHandler.GetCommandCaps( KVtEngSetWhiteBalance );
    if ( caps & MVtEngCommandHandler::EAttribEnabled )
        {
        SetFlag( EIsWhiteBalanceSupported );
        }

    caps = iCommandHandler.GetCommandCaps( KVtEngSetColorTone );
    if ( caps & MVtEngCommandHandler::EAttribEnabled )
        {
        SetFlag( EIsColorToneSupported );
        }

*/
    }
