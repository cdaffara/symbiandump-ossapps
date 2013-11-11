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
* Description:  Implementation of VideoPlaybackControlPolicy
*
*/

// Version : %version: 3 %



// INCLUDE FILES

#include "mpxvideo_debug.h"
#include "videoplaybackcontrolpolicy.h"
#include "videoplaybackviewfiledetails.h"

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlPolicy::VideoPlaybackControlPolicy()
// C++ default constructor can NOT contain any code, that might leave.
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlPolicy::VideoPlaybackControlPolicy()
{
    MPX_DEBUG(_L("VideoPlaybackControlPolicy::VideoPlaybackControlPolicy()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlPolicy::~VideoPlaybackControlPolicy()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlPolicy::~VideoPlaybackControlPolicy()
{
    MPX_DEBUG(_L("VideoPlaybackControlPolicy::~VideoPlaybackControlPolicy()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlPolicy::setControlProperties()
// -------------------------------------------------------------------------------------------------
//
void 
VideoPlaybackControlPolicy::setControlProperties( TVideoPlaybackControls controlIndex,
                                                  TUint& properties,
                                                  VideoPlaybackViewFileDetails *details,
                                                  TPlaybackViewMode viewMode )
{
    MPX_DEBUG(_L("VideoPlaybackControlPolicy::setControlProperties()"));

    properties = 0;

    switch ( controlIndex )
    {
        case EBufferingAnimation:
        {
            properties = EShownWhenInitializing |
                         EShownWhenBuffering;
            break;
        }
        case EStatusPane:
        {
            if ( details->mPlaybackMode != EMPXVideoLocal ||
            	   viewMode == EAudioOnlyView ||
            	   viewMode == EDetailsView )
            {                
                properties = EAllProperties;
            }
            else
            {
                properties = EShownWhenPlaying |
                             EShownWhenPaused |
                             EShownWhenSeeking;
            }
            break;
        }
        case EControlBar:
        {
            //
            // We don't delete ButtonBar and Progress Bar from the controlsList 
            // to update information all the time though it's not visible or activated
            // So just hide these if it is details view
            //
            if ( viewMode == EDetailsView )
            {
                properties = 0;
            }
            else if ( viewMode == EAudioOnlyView )
            {
                properties = EAllProperties;
            }
            else
            {
                properties = EShownWhenPlaying |
                             EShownWhenPaused |
                             EShownWhenSeeking;
            }
            break;
        }
        case EFileDetailsWidget:
        case EIndicatorBitmap:
        case EDetailsViewPlaybackWindow:
        {
            //
            // Add all these controls when it is details view or audio only view
            // Once it's added to controlsList, these are visible always
            //
            properties = EAllProperties;
            break;
        }
    }
}

// End of File
