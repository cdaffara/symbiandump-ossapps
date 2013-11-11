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
* Description:
*
*/

// INCLUDE FILES
#include <telephonyvariant.hrh>
#include "cphoneringingtonecontroller.h"
#include "phonelogger.h"
#include "cphoneringingtone.h"
#include "tphonecmdparamringtone.h"
#include "cphonecenrepproxy.h"
#include "cphoneringingtoneplayer.h"
#include "cphonettsplayer.h"


bool m_EPhoneViewMuteRingToneOnAnswer_called;
bool m_EPhoneViewStopRingTone_called;
bool m_EPhoneViewMuteRingTone_called;
bool m_EPhoneViewPlayRingTone_called;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneRingingToneController* CPhoneRingingToneController::NewL()
    {
    CPhoneRingingToneController* self = 
        new ( ELeave ) CPhoneRingingToneController();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::CPhoneRingingToneController
// -----------------------------------------------------------------------------
//
CPhoneRingingToneController::CPhoneRingingToneController()
    {
    m_EPhoneViewMuteRingToneOnAnswer_called = false;
    m_EPhoneViewStopRingTone_called = false;
    m_EPhoneViewMuteRingTone_called = false;
    m_EPhoneViewPlayRingTone_called = false;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::ConstructL
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::ConstructL()
    {


    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::~CPhoneRingingToneController
// -----------------------------------------------------------------------------
//
CPhoneRingingToneController::~CPhoneRingingToneController()
    {

    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::PlayRingToneL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneRingingToneController::PlayRingToneL( 
                                         TPhoneCommandParam* aCommandParam )
    {

    if ( aCommandParam->ParamId() == TPhoneCommandParam::EPhoneParamIdRingTone )
        {
        m_EPhoneViewPlayRingTone_called = true;
        }
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::PlayAudioRingTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::PlayAudioRingTone( 
    TInt aVolume, 
    TProfileRingingType aRingingType )
    {

    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::MuteRingingToneOnAnswer
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneRingingToneController::MuteRingingToneOnAnswer()
    {
    m_EPhoneViewMuteRingToneOnAnswer_called = true;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::MuteRingingTone
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneRingingToneController::MuteRingingTone()
    {
    m_EPhoneViewMuteRingTone_called = true;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::StopPlaying
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneRingingToneController::StopPlaying()
    {
    m_EPhoneViewStopRingTone_called = true;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::DoMuteRingingTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::DoMuteRingingTone()
    {

    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::ConvertVideoRingingType
// -----------------------------------------------------------------------------
//
MPhoneVideoPlayer::TPlayMode CPhoneRingingToneController::ConvertVideoRingingType(
    TProfileRingingType aRingingType )
    {
    MPhoneVideoPlayer::TPlayMode playMode;
        
    
    return playMode;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::SetVideoPlayer
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::SetVideoPlayer( 
    MPhoneVideoPlayer* aVideoPlayer )
    {
    iVideoPlayer = aVideoPlayer;    
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::PlayVideoRingingTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::PlayVideoRingingTone( 
    const CPhoneRingingTone& aRingingTone, 
    TInt aVolume, 
    TProfileRingingType aRingingType,
    TBool aPersonalTone )
    {
   
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::HandleVideoPlayerError
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::HandleVideoPlayerError( 
    TPhoneVideoPlayerErrorEvent /*aEvent*/,
    TInt aError )
    {
 
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::HandleVideoPlayerInitComplete
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::HandleVideoPlayerInitComplete()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::HandleVideoPlayerInitComplete()" );
    
    // cancel guarding timer
    //iTimer->Cancel();
    
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::HandleVideoPlayerPlayingComplete
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::HandleVideoPlayerPlayingComplete()
    {
    //__LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::HandleVideoPlayerPlayingComplete()" );
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::PlayDefaultVideoAsync
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::PlayDefaultVideoAsync()
    {

    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::ExtendedSecurity
// -----------------------------------------------------------------------------
//    
TBool CPhoneRingingToneController::ExtendedSecurity() const
    {
    //__LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::ExtendedSecurity()" );
    return EFalse;
   
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::CoverUIPlayVideoRingingToneL
// -----------------------------------------------------------------------------
//   
TBool CPhoneRingingToneController::CoverUIPlayVideoRingingToneL( 
    const CPhoneRingingTone& aRingingTone )
    {
    //__LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::CoverUIPlayVideoRingingToneL()" );

    TBool showVideo( EFalse );
    /*
    RBuf8 data;
    CleanupClosePushL( data );
    data.CreateL( aRingingTone.FileName().Length() );
    data.Copy( aRingingTone.FileName() );
    TInt err = iMediatorSender->IssueCommand( 
    	EPhoneCmdCoverUiShowMultimediaRingingTone, 
	    data );
    if ( err == KErrNone )
        {
        showVideo = ETrue;
        iActiveCoverUICommand = ETrue;
        }
    CleanupStack::PopAndDestroy(); //data 
    */
    return showVideo;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::ShowMultimediaRingingToneResponseL
// -----------------------------------------------------------------------------
//   
void CPhoneRingingToneController::ShowMultimediaRingingToneResponseL( 
	TInt aStatus )
	{
    //__LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::ShowMultimediaRingingToneResponseL()" );

	}


    
//  End of File  
