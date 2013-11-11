/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPEAudioInbandTonePlayer class 
*                member functions
*
*/

 
//  INCLUDE FILES
#include    "cpeaudiodata.h" // CPEAudioData header
#include    "cpeaudioinbandtoneplayer.h"
#include    "cpeaudiofactory.h"
#include    "cpeaudiotoneutility.h"
#include    <AudioPreference.h>
#include    <barsc.h> 
#include    <barsread.h>
#include    <data_caging_path_literals.hrh> 
#include    <defaultbeep.rsg> 
#include    <featdiscovery.h>
#include    <pepanic.pan>
#include    <talogger.h>    


// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
const TInt KPhoneInbandVolumeMax = 10;
const TInt KPhoneInbandVolumeMin = 0;
const TInt KPhoneInbandToneZero = 0;
const TInt KPhoneInbandToneNoRepeat = 0;
const TInt KPhoneInbandToneRepeatOnce = 1;

// Resourcefile destination.
_LIT( KSystemDefaultbeepRscDrive, "Z:" );
_LIT( KSystemDefaultbeepRscFile, "DEFAULTBEEP.rSC" );

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None


// ==================== LOCAL FUNCTIONS ====================
// None.

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEAudioInbandTonePlayer* CPEAudioInbandTonePlayer::NewL
    (
    CPEAudioData& aOwner,     // ref. to class owner
    RFs& aFsSession,
    CPEAudioFactory& aAudioFactory
    )
    {
    CPEAudioInbandTonePlayer* self = 
        new ( ELeave ) CPEAudioInbandTonePlayer( aOwner, aFsSession );
    
    CleanupStack::PushL( self );
    self->ConstructL( aAudioFactory );
    CleanupStack::Pop( self );

    return self;
    }

//Destructor
CPEAudioInbandTonePlayer::~CPEAudioInbandTonePlayer()
    {
    delete iPlayBeepSequence;
    delete iPlayNoSoundSequence;
    delete iPlayDataSequence;
    delete iResourceCallWaitingSeq;
    delete iResourceRingGoingSeq;
    delete iResourceRadioPathSeq;
    delete iResourceSpecialSeq;
    delete iResourceCongestionSeq;
    delete iResourceReorderSeq;
    delete iResourceBusySeq;
    delete iCurrent;
    }

// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::CPEAudioInbandTonePlayer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEAudioInbandTonePlayer::CPEAudioInbandTonePlayer
    (
    CPEAudioData& aOwner,     // ref. to class owner
    RFs& aFsSession
    ):iOwner( aOwner ),
      iFsSession( aFsSession )
    {
    }

// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEAudioInbandTonePlayer::ConstructL(
    CPEAudioFactory& aFactory )
    {
    TEFLOGSTRING( KTAOBJECT, "AUD CPEAudioInbandTonePlayer::ConstructL()" );
    iCurrent = aFactory.CreateAudioToneUtilityL( *this );


    //Get the inband tone sequences from resource and and store to member variables
    RResourceFile resourceFile;

    TFileName fileName( KSystemDefaultbeepRscDrive );
    fileName.Append( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KSystemDefaultbeepRscFile );

    resourceFile.OpenL( iFsSession, fileName );
    CleanupClosePushL( resourceFile );

    HBufC8* resourceBusy = resourceFile.AllocReadL( R_NET_BUSY );
    HBufC8* resourceReorder = resourceFile.AllocReadL( R_NET_REORDER );
    HBufC8* resourceCongestion = resourceFile.AllocReadL( R_NET_CONGESTION );
    HBufC8* resourceSpecial = resourceFile.AllocReadL( R_NET_SPECIAL_INFORMATION );
    HBufC8* resourceRadioPath = resourceFile.AllocReadL( R_NET_RADIO_NOT_AVAILABLE );
    HBufC8* resourceRingGoing = resourceFile.AllocReadL( R_NET_RING_GOING );
    
    
    HBufC8* resourceCallWaiting;

    if ( CFeatureDiscovery::IsFeatureSupportedL( KFeatureIdFfAlternativeCallWaitingTone ) )
        {
        resourceCallWaiting = resourceFile.AllocReadL( R_NET_CALL_WAITING_ALTERNATIVE_TONE );
        }
    else
        {
        resourceCallWaiting = resourceFile.AllocReadL( R_NET_CALL_WAITING );
        }  
    
    
    HBufC8* dataCallTone = resourceFile.AllocReadL( R_DATA_CALL_TONE );
    HBufC8* noSoundSequence = resourceFile.AllocReadL( R_NO_SOUND_SEQUENCE );
    HBufC8* beepSequence = resourceFile.AllocReadL( R_BEEP_SEQUENCE );
    
    CleanupStack::PopAndDestroy( &resourceFile );

    TResourceReader reader;
    TInt i(0);
    TInt length(0);

    //SubscriberBusy
    reader.SetBuffer( resourceBusy );
    length = reader.ReadInt16();
    iResourceBusySeq = HBufC8::NewL( length );
    //Tptr modifies member variables (HBufC8) length so it includes the memory edited by Append()
    TPtr8 appendBusy(iResourceBusySeq->Des());

    for( i = 0 ; i < length ; i++ )
        {
        appendBusy.Append(reader.ReadUint16());
        }
    
    //Reorder
    reader.SetBuffer( resourceReorder );
    length = reader.ReadInt16();
    iResourceReorderSeq = HBufC8::NewL( length );
    //Tptr modifies member variables (HBufC8) length so it includes the memory edited by Append()
    TPtr8 appendReorder(iResourceReorderSeq->Des());

    for( i = 0 ; i < length ; i++ )
        {
        appendReorder.Append(reader.ReadUint16());
        }

    //Congestion
    reader.SetBuffer( resourceCongestion );
    length = reader.ReadInt16();
    iResourceCongestionSeq = HBufC8::NewL( length );
    //Tptr modifies member variables (HBufC8) length so it includes the memory edited by Append()
    TPtr8 appendCongestion(iResourceCongestionSeq->Des());

    for( i = 0 ; i < length ; i++ )
        {
        appendCongestion.Append(reader.ReadUint16());
        }
    
    //Special Information
    reader.SetBuffer( resourceSpecial );
    length = reader.ReadInt16();
    iResourceSpecialSeq = HBufC8::NewL( length );
    //Tptr modifies member variables (HBufC8) length so it includes the memory edited by Append()
    TPtr8 appendSpecial(iResourceSpecialSeq->Des());

    for( i = 0 ; i < length ; i++ )
        {
        appendSpecial.Append(reader.ReadUint16());
        }
    
    //RadioPathNotAvailable
    reader.SetBuffer( resourceRadioPath );
    length = reader.ReadInt16();
    iResourceRadioPathSeq = HBufC8::NewL( length );
    //Tptr modifies member variables (HBufC8) length so it includes the memory edited by Append()
    TPtr8 appendRadio(iResourceRadioPathSeq->Des());

    for( i = 0 ; i < length ; i++ )
        {
        appendRadio.Append(reader.ReadUint16());
        }
    
    //RemoteAlertingTone
    reader.SetBuffer( resourceRingGoing );
    length = reader.ReadInt16();
    iResourceRingGoingSeq = HBufC8::NewL( length );
    //Tptr modifies member variables (HBufC8) length so it includes the memory edited by Append()
    TPtr8 appendRingGoing(iResourceRingGoingSeq->Des());

    for( i = 0 ; i < length ; i++ )
        {
        appendRingGoing.Append(reader.ReadUint16());
        }
    //CallWaitingTone
    reader.SetBuffer( resourceCallWaiting );
    length = reader.ReadInt16();
    iResourceCallWaitingSeq = HBufC8::NewL( length );
    //Tptr modifies member variables (HBufC8) length so it includes the memory edited by Append()
    TPtr8 appendCallWaiting(iResourceCallWaitingSeq->Des());

    for( i = 0 ; i < length ; i++ )
        {
        appendCallWaiting.Append(reader.ReadUint16());
        }

    //DataCalTone
    reader.SetBuffer( dataCallTone );
    length = reader.ReadInt16();
    iPlayDataSequence = HBufC8::NewL( length );
    //Tptr modifies member variables (HBufC8) length so it includes the memory edited by Append()
    TPtr8 appendDataCallTone( iPlayDataSequence->Des() );

    for( i = 0 ; i < length ; i++ )
        {
        appendDataCallTone.Append(reader.ReadUint16());
        }

    //NoSoundSequence
    reader.SetBuffer( noSoundSequence );
    length = reader.ReadInt16();
    iPlayNoSoundSequence = HBufC8::NewL( length );
    //Tptr modifies member variables (HBufC8) length so it includes the memory edited by Append()
    TPtr8 appendNoSoundSequence( iPlayNoSoundSequence->Des() );

    for( i = 0 ; i < length ; i++ )
        {
        appendNoSoundSequence.Append(reader.ReadUint16());
        }

    //BeepSequence
    reader.SetBuffer( beepSequence );
    length = reader.ReadInt16();
    iPlayBeepSequence = HBufC8::NewL( length );
    //Tptr modifies member variables (HBufC8) length so it includes the memory edited by Append()
    TPtr8 appendBeepSequence( iPlayBeepSequence->Des() );

    for( i = 0 ; i < length ; i++ )
        {
        appendBeepSequence.Append(reader.ReadUint16());
        }

    delete dataCallTone;
    delete noSoundSequence;
    delete beepSequence;
    delete resourceBusy;
    delete resourceReorder;
    delete resourceCongestion;
    delete resourceSpecial;
    delete resourceRadioPath;
    delete resourceRingGoing;
    delete resourceCallWaiting;

    }

// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::PlayInbandTone
// Searches the given Inband tone from iToneArray and calls PlayCurrentTone -member function.
// -----------------------------------------------------------------------------
//
void CPEAudioInbandTonePlayer::PlayInbandTone( 
        TCCPTone aTone )
    {
    //Stop playing if there is something playing
    Cancel();

    UpdateTonePlayerVolume();
    
    iToneName = aTone;

    TPtr8 resourceBusySeq = iResourceBusySeq -> Des();
    TPtr8 resourceRadioPathSeq = iResourceRadioPathSeq -> Des();
    TPtr8 resourceCongestionSeq = iResourceCongestionSeq -> Des();
    TPtr8 resourceSpecialSeq = iResourceSpecialSeq -> Des();
    TPtr8 resourceReorderSeq = iResourceReorderSeq->Des();
    TPtr8 resourceRingGoingSeq = iResourceRingGoingSeq -> Des();
    TPtr8 resourceCallWaitingSeq = iResourceCallWaitingSeq -> Des();

    TEFLOGSTRING( KTAREQOUT, 
        "AUD CPEAudioInbandTonePlayer::PlayInbandTone: CMdaAudioToneUtility->PrepareToPlayDesSequence()" );
    switch( aTone )
        {
        case ECCPToneUserBusy:
            TEFLOGSTRING( KTAINT, "AUD CPEAudioInbandTonePlayer::PlayInbandTone: iResourceBusySeq" );
            iCurrent->PrepareToPlayDesSequence( resourceBusySeq );
            break;
        case ECCPToneRadioPathNotAvailable:
            TEFLOGSTRING( KTAINT, "AUD CPEAudioInbandTonePlayer::PlayInbandTone: iResourceRadioPathSeq" );
            iCurrent->PrepareToPlayDesSequence( resourceRadioPathSeq );
            break;
        case ECCPToneCongestion:
            TEFLOGSTRING( KTAINT, "AUD CPEAudioInbandTonePlayer::PlayInbandTone: iResourceCongestionSeq" );
            iCurrent->PrepareToPlayDesSequence( resourceCongestionSeq );
            break;
        case ECCPToneSpecialInformation:
            TEFLOGSTRING( KTAINT, "AUD CPEAudioInbandTonePlayer::PlayInbandTone: iResourceSpecialSeq" );
            iCurrent->PrepareToPlayDesSequence( resourceSpecialSeq );
            break;
        case ECCPReorder:
            TEFLOGSTRING( KTAINT, "AUD CPEAudioInbandTonePlayer::PlayInbandTone: iResourceReorderSeq" );
            iCurrent->PrepareToPlayDesSequence( resourceReorderSeq );
            break;
        case ECCPRemoteAlerting:
            TEFLOGSTRING( KTAINT, "AUD CPEAudioInbandTonePlayer::PlayInbandTone: iResourceRingGoingSeq" );
            iCurrent->PrepareToPlayDesSequence( resourceRingGoingSeq );
            break;
        case ECCPCallWaiting:
            TEFLOGSTRING( KTAINT, "AUD CPEAudioInbandTonePlayer::PlayInbandTone: iResourceCallWaitingSeq" );
            iCurrent->PrepareToPlayDesSequence( resourceCallWaitingSeq );
            break;
        case ECCPNoSoundSequence:
            TEFLOGSTRING( KTAINT, "AUD CPEAudioInbandTonePlayer::PlayInbandTone: called ECCPNoSoundSequence " );
            break;
        default:
            TEFLOGSTRING2( KTAINT, "AUD CPEAudioInbandTonePlayer::PlayInbandTone: No InbandTone defined to play with: %d", aTone );
            __ASSERT_DEBUG( EFalse, Panic( EPEPanicNotCompleted ) );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::Cancel
// Cancels the currently playing tone.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEAudioInbandTonePlayer::Cancel()
    {
    TEFLOGSTRING( KTAINT, "AUD CPEAudioInbandTonePlayer::Cancel()");
    if ( iCurrent )
        {
        if ( iCurrent->State() == EMdaAudioToneUtilityNotReady )
            {
            // Prepare is called, but toneplayer's state is not yet prepare,
            // then cancel to prepare is needed.
            TEFLOGSTRING( KTAREQOUT, 
                "AUD CPEAudioInbandTonePlayer::Cancel: CMdaAudioToneUtility->CancelPrepare");
            iCurrent->CancelPrepare();
            }
        else
            {
            TEFLOGSTRING( KTAREQOUT, 
                "AUD CPEAudioInbandTonePlayer::Cancel: CMdaAudioToneUtility->CancelPlay");
            iCurrent->CancelPlay();
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::PlayCurrentTone
// Cancels the currently playing tone.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEAudioInbandTonePlayer::PlayCurrentTone()
    {
    if ( iCurrent )
        {
        if ( iCurrent->State() == EMdaAudioToneUtilityPrepared )
            {

            UpdateTonePlayerVolume();

            TEFLOGSTRING( KTAREQOUT, 
                "AUD CPEAudioInbandTonePlayer::PlayCurrentTone(): CMdaAudioToneUtility->SetPriority()");

            switch( iToneName )
                {
                case ECCPToneUserBusy:
                     iCurrent->SetRepeats( 
                        KMdaAudioToneRepeatForever, 
                        TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    iCurrent->SetPriority(
                        KAudioPriorityNetMsg,
                        static_cast < TMdaPriorityPreference > ( KAudioPrefBusy ) );
                    break;
                case ECCPToneRadioPathNotAvailable:
                    iCurrent->SetRepeats( 
                        KPhoneInbandToneRepeatOnce, 
                        TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    iCurrent->SetPriority(
                        KAudioPriorityNetMsg,
                        static_cast < TMdaPriorityPreference > ( KAudioPrefRadioNotAvailable ) );
                    break;
                case ECCPToneCongestion:
                    iCurrent->SetRepeats( 
                        KMdaAudioToneRepeatForever, 
                        TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    iCurrent->SetPriority(
                        KAudioPriorityNetMsg,
                        static_cast < TMdaPriorityPreference > ( KAudioPrefCongestion ) );
                    break;
                case ECCPToneSpecialInformation:
                    iCurrent->SetRepeats( 
                        KMdaAudioToneRepeatForever, 
                        TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    iCurrent->SetPriority(
                        KAudioPriorityNetMsg,
                        static_cast < TMdaPriorityPreference > ( KAudioPrefSpecialInformation ) );
                    break;
                case ECCPReorder:
                    iCurrent->SetRepeats( 
                        KMdaAudioToneRepeatForever, 
                        TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    iCurrent->SetPriority(
                        KAudioPriorityNetMsg,
                        static_cast < TMdaPriorityPreference > ( KAudioPrefReorder ) );
                    break;
                case ECCPRemoteAlerting:
                    iCurrent->SetRepeats( 
                        KMdaAudioToneRepeatForever, 
                        TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    iCurrent->SetPriority(
                        KAudioPriorityNetMsg,
                        static_cast < TMdaPriorityPreference > ( KAudioPrefRingGoing ) );
                    break;
                case ECCPCallWaiting:
                    iCurrent->SetRepeats( 
                        KPhoneInbandToneNoRepeat, 
                        TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    iCurrent->SetPriority(
                        KAudioPriorityNetMsg,
                        static_cast < TMdaPriorityPreference > ( KAudioPrefCallWaiting ));
                    break;
                case ECCPDataCallTone: //EProfileRingingTypeRinging, EProfileRingingTypeAscending, EProfileRingingTypeRingingOnce
                    iCurrent->SetRepeats( 
                        KMdaAudioToneRepeatForever, 
                        TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    iCurrent->SetPriority(
                        KAudioPriorityPhoneCall,
                        static_cast < TMdaPriorityPreference > ( KAudioPrefIncomingDataCall ) );
                    
                    if ( iRingingType == EProfileRingingTypeAscending )
                        {
                        iCurrent->SetVolumeRamp( 
                            TTimeIntervalMicroSeconds( KPERingingToneRampDuration ) );
                        }
                    else
                        {
                        //EProfileRingingTypeRinging, EProfileRingingTypeRingingOnce
                        iCurrent->SetVolumeRamp( 
                            TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                        }
                    if ( iRingingType == EProfileRingingTypeRingingOnce )
                        {
                        iCurrent->SetRepeats( 
                            KPhoneInbandToneNoRepeat, 
                            TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                        }
                    break;
                case ECCPNoSoundSequence:
                case ECCPBeepSequence:
                    iCurrent->SetRepeats( 
                        KMdaAudioToneRepeatForever, 
                        TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    iCurrent->SetPriority(
                        KAudioPriorityPhoneCall,
                        static_cast < TMdaPriorityPreference > ( KAudioPrefIncomingCall ) );
                    iCurrent->SetRepeats( 
                        KPhoneInbandToneNoRepeat, 
                        TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    iCurrent->SetVolumeRamp( TTimeIntervalMicroSeconds( KPhoneInbandToneZero ) );
                    break;
                default:
                    TEFLOGSTRING2( KTAREQOUT, 
                        "AUD CPEAudioInbandTonePlayer::PlayCurrentTone: No InbandTone defined to set priority with %d", 
                        iToneName);
                    break;
                }

            #ifdef __WINS__
                    TEFLOGSTRING( KTAMESINT, 
                        "AUD CPEAudioInbandTonePlayer::PlayCurrentTone: Ignored call to CMdaAudioToneUtility->Play(), because can't play in WINS");
            #else
                    //Play the tone
                    TEFLOGSTRING( KTAREQOUT, 
                        "AUD CPEAudioInbandTonePlayer::PlayCurrentTone: CMdaAudioToneUtility->Play() called");
                    iCurrent->Play();
            #endif
            } // State()
        } //iCurrent
    }

// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::MatoPrepareComplete
// Updates flag values and plays current inbandtone.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEAudioInbandTonePlayer::MatoPrepareComplete( TInt aError )
    {

    if( aError == KErrNone )
        {
        PlayCurrentTone();
        }
    else
        {
        TEFLOGSTRING2( KTAREQIN, 
            "AUD CPEAudioInbandTonePlayer::MatoPrepareComplete: aError = %d", 
            aError );
        }

    
    }


// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::SetVolume
// -----------------------------------------------------------------------------
//
void CPEAudioInbandTonePlayer::SetVolume( TInt aVolume )
    {
    TInt safeVolume = 
        Max( Min( aVolume, KPhoneInbandVolumeMax ), KPhoneInbandVolumeMin );
    
    TEFLOGSTRING2( KTAREQIN, 
        "AUD CPEAudioInbandTonePlayer::SetVolume: VOLUME = %d", safeVolume );
    
    if ( iCurrent && EMdaAudioToneUtilityNotReady != iCurrent->State() )
        {
        TInt mediaServerVolume = CalculateMediaServerVolume( safeVolume );
        TEFLOGSTRING2( KTAREQIN, 
            "AUD CPEAudioInbandTonePlayer::SetVolume: MEDIA SERVER VOLUME = %d", 
            mediaServerVolume );
        iCurrent->SetVolume( mediaServerVolume );
        }
    }


// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::MatoPlayComplete
// -
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEAudioInbandTonePlayer::MatoPlayComplete( TInt aError )
    {
 
    if( aError != KErrNone )
        {
        TEFLOGSTRING2( KTAREQIN, 
            "CPEAudioInbandTonePlayer::MatoPlayComplete: aError = %d",
            aError );
        }
    
    }

// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::CalculateMediaServerVolume
// -----------------------------------------------------------------------------
//
TInt CPEAudioInbandTonePlayer::CalculateMediaServerVolume( TInt aVolume ) const
    {
    // Our volume level scale is 0...10, media server's can be anything. 
    // This scales the volume to correct level.
    return ( iCurrent->MaxVolume() * aVolume ) / 
        ( KPhoneInbandVolumeMax - KPhoneInbandVolumeMin );
    }


// -----------------------------------------------------------------------------
// CPEAudioInbandTonePlayer::UpdateTonePlayerVolume
// -----------------------------------------------------------------------------
//
void CPEAudioInbandTonePlayer::UpdateTonePlayerVolume()
    {
    TInt volume( KPhoneInbandVolumeMin );
    iOwner.GetAudioVolumeSync( volume );
    SetVolume( volume );
    }

// End of File
