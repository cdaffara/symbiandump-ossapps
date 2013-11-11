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
* Description:  Audio state class.
*
*/


#include <cvtengmodel.h>

#include "tlcvtaudiostate.h"
#include "lcvtutility.h"

// -----------------------------------------------------------------------------
// TLcVtAudioState::UpdateFlags
// -----------------------------------------------------------------------------
//
void TLcVtAudioState::UpdateFlags()
    {
    TInt avail( 0 );

    LcVtUtility::GetAudioRoutingAvailability( iModel.Audio(), avail );

    const TBool videoCallConnected ( iModel.Media().VideoCallConnected());
    //Before update AudioRouting, video call state must to be checked.
    //AudioRouting can not be changed if call is not in EConnected state.
    if ( videoCallConnected )
        {
        if ( avail & LcVtUtility::EDeactivateBtHandsfree )
            {
            SetFlag( ECanDeactivateBtHf );
            }
        if ( avail & LcVtUtility::EActivateBtHandsfree )
            {
            SetFlag( ECanActivateBtHf );
            }
        if ( avail & LcVtUtility::EDeactivateLoudspeaker )
            {
            SetFlag( ECanDeactivateLoudspeaker );
            }
        if ( avail & LcVtUtility::EActivateLoudspeaker )
            {
            SetFlag( ECanActivateLoudspeaker );
            }
        }

    LcVtUtility::GetOutgoingMediaState( iModel.Media(), avail );

    if ( avail & MVtEngMedia::EMediaAudio )
        {
        SetFlag( EIsAudio );
        }

    TInt status ( 0 );
    iModel.Media().GetMediaState( MVtEngMedia::EMediaOutgoing, status );
    if ( !(status & MVtEngMedia::EMediaAudioChannelOpen ) )
        {
        SetFlag( EIsAudioPermanentlyStopped );
        }

    }
