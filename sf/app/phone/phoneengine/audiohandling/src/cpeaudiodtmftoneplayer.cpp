/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPEAudioDtmfTonePlayer class 
*                member functions
*
*/


// INCLUDE FILES
#include    "cpeaudiodtmftoneplayer.h"
#include    "pepanic.pan"
#include    <utility.h>
#include    <mdaaudiotoneplayer.h>
#include    <talogger.h>
#include    <AudioPreference.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
const TInt KDtmfPauseAfterTone = 45000;
const TInt KDtmfToneZero = 0;
const TInt KBufSize = 10;
_LIT( KPhoneTone_Asterisk, "*+w" );

const TInt KMaxVolumeLevel = 10;

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None. 

// ==================== LOCAL FUNCTIONS =====================================
// None

// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CPEAudioDtmfTonePlayer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEAudioDtmfTonePlayer* CPEAudioDtmfTonePlayer::NewL()
    {
    CPEAudioDtmfTonePlayer* self = 
        new ( ELeave ) CPEAudioDtmfTonePlayer;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

//Destructor
CPEAudioDtmfTonePlayer::~CPEAudioDtmfTonePlayer()
    {
    TEFLOGSTRING( KTAOBJECT, "AUD CPEAudioDtmfTonePlayer::~CPEAudioDtmfTonePlayer");
    Cancel();
    delete iDTMFPlayer;
    }

// -----------------------------------------------------------------------------
// CPEAudioDtmfTonePlayer::CPEAudioDtmfTonePlayer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEAudioDtmfTonePlayer::CPEAudioDtmfTonePlayer()
    {
    TEFLOGSTRING( KTAOBJECT, "AUD CPEAudioDtmfTonePlayer::CPEAudioDtmfTonePlayer");
    }

// -----------------------------------------------------------------------------
// CPEAudioDtmfTonePlayer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "AUD CPEAudioDtmfTonePlayer::ConstructL start");
    TMMFPrioritySettings dtmfClickPlayerPrioritySettings;  
    iDTMFPlayer = CMMFDevSound::NewL();
    
    iDTMFPlayer->InitializeL( *this, EMMFStateTonePlaying );
    
    TEFLOGSTRING( KTAOBJECT, "AUD CPEAudioDtmfTonePlayer::ConstructL completed");
    }

// -----------------------------------------------------------------------------
// CPEAudioDtmfTonePlayer::SetVolume
// Sets the volume level that is used when the dtmf tone is played.
// -----------------------------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::SetVolume( 
        TInt aVolume  //Volume to be set
        )
    {
    TInt vol = ConvertVolume( aVolume );
        
    if ( vol != iVolume )
        {
        iVolume = vol;
        iDTMFPlayer->SetVolume( iVolume );
        TEFLOGSTRING2( KTAINT, "AUD CPEAudioDtmfTonePlayer::SetVolume, iVolume = %d", 
            iVolume );
        }
    }

// -----------------------------------------------------------------------------
// CPEAudioDtmfTonePlayer::PlayDtmfTone
// Searches the given DTMF tone from iToneArray and calls PlayCurrentTone -member function.
// -----------------------------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::PlayDtmfTone( 
        TChar aTone )
    {
    TEFLOGSTRING( KTAINT, "AUD CPEAudioDtmfTonePlayer::PlayDtmfTone");
    Cancel();
    Normalize( aTone ); 

    TBuf<KBufSize> key; 
    key.Append( aTone );
    TEFLOGSTRING( KTAREQOUT, "AUD CPEAudioDtmfTonePlayer::PlayDtmfTone > CMMFDevSound::PlayDTMFStringL");
    TRAP_IGNORE( iDTMFPlayer->PlayDTMFStringL( key ) ); 
    
    }

// -----------------------------------------------------------------------------
// CPEAudioDtmfTonePlayer::Cancel
// Cancels the currently playing tone.
// -----------------------------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::Cancel()
    {
    TEFLOGSTRING( KTAREQEND, "AUD CPEAudioDtmfTonePlayer::Cancel");
    iDTMFPlayer->Stop();
    }

// -----------------------------------------------------------------------------
// CPEAudioDtmfTonePlayer::Normalize
// Lowers case for chars and if its '*','+', or 'w' sets it to asterisk char ('*').
// -----------------------------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::Normalize( 
        TChar& aTone )
    {
    TEFLOGSTRING( KTAINT, "AUD CPEAudioDtmfTonePlayer::Normalize");
    aTone.LowerCase();

    TPtrC ast( KPhoneTone_Asterisk );

    if ( ast.Locate( aTone ) != KErrNotFound )
        {
        aTone = ast[ 0 ];
        TEFLOGSTRING( KTAINT, "AUD CPEAudioDtmfTonePlayer::Normalize");
        }
    }
    
// ---------------------------------------------------------
// CPEAudioDtmfTonePlayer::InitializeComplete
// ---------------------------------------------------------
//
 void CPEAudioDtmfTonePlayer::InitializeComplete(  
    TInt aError )
    {
    TEFLOGSTRING( KTAREQEND, "AUD CPEAudioDtmfTonePlayer::InitializeComplete");
    __ASSERT_DEBUG( aError == KErrNone , Panic( EPEPanicAudioInitializeError ) );
      
    if( aError == KErrNone)
        {
        TMMFPrioritySettings dtmfClickPlayerSettings;

        dtmfClickPlayerSettings.iPref = 
                  ( TMdaPriorityPreference ) KAudioPrefKeyDownDTMF;
        dtmfClickPlayerSettings.iPriority = KAudioPriorityDtmfKeyPress; 
        dtmfClickPlayerSettings.iState = EMMFStateTonePlaying; 
      
        iDTMFPlayer->SetPrioritySettings( dtmfClickPlayerSettings );         
      
        TTimeIntervalMicroSeconds32 length( KMdaInfiniteDurationDTMFToneOnLength );
        TTimeIntervalMicroSeconds32 pause( KDtmfPauseAfterTone );
        TTimeIntervalMicroSeconds32 zero( KDtmfToneZero );
      
        iDTMFPlayer->SetDTMFLengths( length, pause, zero  );

        iDTMFPlayer->SetToneRepeats( 
                            KMdaAudioToneRepeatForever, 
                            TTimeIntervalMicroSeconds( KDtmfToneZero ) );              
        // Must initialize volume to dtmf player. Otherwise it will be
        // 5 by default in audio SW.
        iDTMFPlayer->SetVolume( iVolume );
        }


    }

// ---------------------------------------------------------
// CPEAudioDtmfTonePlayer::ToneFinished
// ---------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::ToneFinished( 
    TInt /*aError*/ )
    {
    TEFLOGSTRING( KTAREQEND, "AUD CPEAudioDtmfTonePlayer::ToneFinished");
    }

// ---------------------------------------------------------
// CPEAudioDtmfTonePlayer::BufferToBeFilled
// ---------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::BufferToBeFilled( CMMFBuffer* /*aBuffer*/ )
    {
    TEFLOGSTRING( KTAREQEND, "AUD CPEAudioDtmfTonePlayer::BufferToBeFilled");
    }

// ---------------------------------------------------------
// CPEAudioDtmfTonePlayer::PlayError
// ---------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::PlayError( TInt /*aError*/ )
    {
    TEFLOGSTRING( KTAREQEND, "AUD CPEAudioDtmfTonePlayer::PlayError");
    }

// ---------------------------------------------------------
// CPEAudioDtmfTonePlayer::BufferToBeEmptied
// ---------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::BufferToBeEmptied( CMMFBuffer* /*aBuffer*/ )
    {
    TEFLOGSTRING( KTAREQEND, "AUD CPEAudioDtmfTonePlayer::BufferToBeEmptied");
    }

// ---------------------------------------------------------
// CPEAudioDtmfTonePlayer::RecordError
// ---------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::RecordError( TInt /*aError*/ )
    {
    TEFLOGSTRING( KTAREQEND, "AUD CPEAudioDtmfTonePlayer::RecordError");
    }

// ---------------------------------------------------------
// CPEAudioDtmfTonePlayer::ConvertError
// ---------------------------------------------------------
//
void CPEAudioDtmfTonePlayer::ConvertError( TInt /*aError*/ )  
    {
    TEFLOGSTRING( KTAREQEND, "AUD CPEAudioDtmfTonePlayer::ConvertError");
    }
    
// ---------------------------------------------------------
// CPEAudioDtmfTonePlayer::DeviceMessage
// ---------------------------------------------------------
//  
void CPEAudioDtmfTonePlayer::DeviceMessage( TUid /*aMessageType*/, 
                                         const TDesC8& /*aMsg*/ )
    {
    TEFLOGSTRING( KTAREQEND, "AUD CPEAudioDtmfTonePlayer::DeviceMessage");
    }

// -----------------------------------------------------------------------------
// CPEAudioDtmfTonePlayer::ConvertVolume
// -----------------------------------------------------------------------------
//  
TInt CPEAudioDtmfTonePlayer::ConvertVolume( TInt aVolume )
    { 
    TEFLOGSTRING( KTAREQEND, "AUD CPEAudioDtmfTonePlayer::ConvertVolume");
    TInt result = iDTMFPlayer->MaxVolume() * aVolume / KMaxVolumeLevel;    
    
    TEFLOGSTRING3( KTAREQEND, 
            "AUD CPEAudioDtmfTonePlayer::ConvertVolume, result = %d, maxVol=%d", 
            result, iDTMFPlayer->MaxVolume() );
    return result;
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
// None.

//  End of File  
