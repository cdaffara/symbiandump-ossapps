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
* Description:  This module contains the implementation of CPEGsmAudioData class 
*                member functions
*
*/


// INCLUDE FILES
#include "cpeaudiodtmftoneplayer.h" 
#include "cpeaudiofactory.h"    
#include "cpeaudioinbandtoneplayer.h"   
#include "cpegsmaudiodata.h"    
#include <ccpdefs.h>    
#include <mpeclientinformation.h>   
#include <mpedatastore.h>   
#include <mpephonemodelinternal.h>  
#include <talogger.h>   

// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// None.

// MACROS
// None.

// LOCAL CONSTANTS AND MACROS
// None.

// MODULE DATA STRUCTURES
// None.

// LOCAL FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

// ==================== LOCAL FUNCTIONS ====================
// None.

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEGsmAudioData::CPEGsmAudioData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEGsmAudioData::CPEGsmAudioData
        ( 
        MPEPhoneModelInternal& aPhoneModel,
        RFs& aFsSession
        ) : CPEAudioData( aPhoneModel ),
            iFsSession( aFsSession )
    {
    }

// -----------------------------------------------------------------------------
// CPEGsmAudioData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEGsmAudioData::ConstructL( CPEAudioFactory& aAudioFactory )
    {    
    CPEAudioData::ConstructL( aAudioFactory );     
    
    iInbandTonePlayer = 
        CPEAudioInbandTonePlayer::NewL( *this, iFsSession, aAudioFactory );
    TInt volume = iPhoneModel.DataStore()->AudioVolume(); 
    iInbandTonePlayer->SetVolume( volume );
    
    iDtmfTonePlayer = CPEAudioDtmfTonePlayer::NewL();
    }

// -----------------------------------------------------------------------------
// CPEGsmAudioData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEGsmAudioData* CPEGsmAudioData::NewL
        ( 
        MPEPhoneModelInternal& aPhoneModel,
        RFs& aFsSession
        )
    {
    CPEGsmAudioData* self = new ( ELeave ) 
        CPEGsmAudioData ( aPhoneModel, aFsSession );
    CleanupStack::PushL( self );
    
    CPEAudioFactory* factory = CPEAudioFactory::NewLC();
    self->ConstructL(*factory);
    CleanupStack::PopAndDestroy( factory );
    
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CPEGsmAudioData::NewL
// Two-phased constructor for testing purposes.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEGsmAudioData* CPEGsmAudioData::NewL
        ( 
        MPEPhoneModelInternal& aPhoneModel,
        RFs& aFsSession,
        CPEAudioFactory& aAudioFactory  
        )
    {
    CPEGsmAudioData* self = new ( ELeave ) 
        CPEGsmAudioData ( aPhoneModel, aFsSession );
    CleanupStack::PushL( self );
    
    self->ConstructL(aAudioFactory);    
    CleanupStack::Pop( self );
    return self;
    }        
    
// Destructor
EXPORT_C CPEGsmAudioData::~CPEGsmAudioData
        (
        // None.
        )
    {
    delete iDtmfTonePlayer;
    delete iInbandTonePlayer;
    }

// -----------------------------------------------------------------------------
// CPEGsmAudioData::PlayDtmfTone
// Starts to play the given tone from dtmf player. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEGsmAudioData::PlayDtmfTone
        ( 
        const TChar& aTone //DTMF tone to play
        ) const
    {
    TInt volume;  //DTMF tone volume
    volume = iPhoneModel.DataStore()->KeypadVolume();

    TEFLOGSTRING3( KTAREQIN, 
        "AUD CPEGsmAudioData::PlayDtmfTone, aTone = %s, volume = %d", 
        &aTone, 
        volume );

    iDtmfTonePlayer->SetVolume( volume );
    iDtmfTonePlayer->PlayDtmfTone( aTone );
    }

// -----------------------------------------------------------------------------
// CPEGsmAudioData::StopDtmfTonePlay
// Stops the playing of dtmf tone. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEGsmAudioData::StopDtmfTonePlay
        (
        )
    {
    TEFLOGSTRING( KTAREQIN, "AUD CPEGsmAudioData::StopDtmfTonePlay" );
    iDtmfTonePlayer->Cancel();
    }

// -----------------------------------------------------------------------------
// CPEGsmAudioData::StopInbandTonePlay
// Stops the playing of inband tone. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEGsmAudioData::StopInbandTonePlay
        ( 
        )
    {
    TEFLOGSTRING( KTAREQIN, "AUD CPEGsmAudioData::StopInbandTonePlay" );
    iInbandTonePlayer->Cancel();
    }

// -----------------------------------------------------------------------------
// CPEGsmAudioData::PlayInbandTone
// Starts to play the given tone from inband player.
// -----------------------------------------------------------------------------
//

EXPORT_C void CPEGsmAudioData::PlayInbandTone()
    {
    TCCPTone tone = iPhoneModel.DataStore()->InbandTone();

    TEFLOGSTRING2( KTAREQIN, 
        "AUD CPEGsmAudioData::PlayInbandTone, tone = %d", 
        tone );
    iInbandTonePlayer->PlayInbandTone( tone );
    }

// -----------------------------------------------------------------------------
// CPEGsmAudioData::HandleDtmfEvent
// From MCCEDtmfObserver.
// All the occurred DTMF events are notified through this interface.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEGsmAudioData::HandleDTMFEvent( const MCCEDtmfObserver::TCCEDtmfEvent aEvent, 
                                                const TInt aError, 
                                                const TChar aTone )
    {
    TEFLOGSTRING3( 
        KTAMESIN, 
        "AUD CPEGsmAudioData::HandleDtmfEvent: aEvent %d, aError %d", 
        aEvent, 
        aError );
        
    if( aError == KErrNone )
        {
        switch( aEvent )
            {                
            case ECCEDtmfManualStart:
            case ECCEDtmfSequenceStart:
                if ( DtmfLocalPlayAllowed() )
                    {
                    TEFLOGSTRING( KTAINT, "AUD CPEGsmAudioData::HandleDTMFEvent: PlayDtmfTone" );
                    PlayDtmfTone( aTone );
                    }
                else
                    {
                    TEFLOGSTRING( KTAERROR, "AUD CPEGSMAUDIODATA::HANDLEDTMFEVENT ! LOCAL PLAY NOT ALLOWED" );
                    }

                break;
                
            case ECCEDtmfManualStop:
            case ECCEDtmfSequenceStop:
            case ECCEDtmfManualAbort:
            case ECCEDtmfSequenceAbort:
            case ECCEDtmfStopInDtmfString:
            case ECCEDtmfStringSendingCompleted:
            default:
                TEFLOGSTRING( KTAINT, "AUD CPEGsmAudioData::HandleDTMFEvent: StopDtmfTonePlay" );
                //Stop tone playing
                StopDtmfTonePlay();
                break;
            }
        }
    else
        {
        TEFLOGSTRING( KTAINT, "AUD CPEGsmAudioData::HandleDTMFEvent: Error case" );
        //Stop tone playing
        StopDtmfTonePlay();
        }
    }

// -----------------------------------------------------------------------------
// CPEGsmAudioData::DtmfLocalPlayAllowed
// Checks if a DTMF tone may be played locally
// -----------------------------------------------------------------------------
//
TBool CPEGsmAudioData::DtmfLocalPlayAllowed() const
    {
    TBool localPlayAllowed( ETrue );
    
    // Check if dtmf string sending was requested by user
    if ( iPhoneModel.DataStore()->DtmfString() == KNullDesC )
        {
        // No dtmf string was defined - assuming dtmf string sending was initiated
        // by third party. Proceed to check if a connected SAT originated call exists
        for( TInt callId = 0; callId < KPEMaximumNumberOfVoiceCalls; callId++ )
            {
            if( iPhoneModel.DataStore()->CallOrigin( callId ) == EPECallOriginSAT    
                && iPhoneModel.DataStore()->CallState( callId ) == EPEStateConnected )
                {
                // SAT call is in connected state - prevent dtmf local play
                localPlayAllowed = EFalse;
                break;
                }
            }
        }

    return localPlayAllowed;
    }

// -----------------------------------------------------------------------------
// CPEGsmAudioData::DoHandleVolumeChange
// -----------------------------------------------------------------------------
//
void CPEGsmAudioData::DoHandleVolumeChange( TInt aVolume, TBool aSendMsg)

    {
    iInbandTonePlayer->SetVolume( aVolume );
    CPEAudioData::DoHandleVolumeChange(aVolume , aSendMsg);
    }

// ================= OTHER EXPORTED FUNCTIONS ===============================
// None

// End of File
