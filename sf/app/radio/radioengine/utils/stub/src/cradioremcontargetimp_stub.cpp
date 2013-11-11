/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

// System includes
#include <RemConCallHandlingTarget.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>

// User includes
#include "cradioenginelogger.h"
#include "cradioremcontargetimp_stub.h"
#include "mradiocontroleventobserver.h"
#include "radioengineutils.h"

#define STUB  iRadioStubManager->iRemConTarget

// Constants
const TInt KVRVolumeTimerInitialDelay = 0; // Initial timer for headset volume up/down press event should expire immediately

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRemConTargetImp::CRadioRemConTargetImp()
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::ConstructL()
    {
    LOG_METHOD_AUTO;;
    // Open chunk for test configuration/control data
    TInt err = iRadioStubManagerChunk.OpenGlobal(
            KRadioStubManagerLocalChunkName,
            EFalse, // == Read | Write
            EOwnerThread );
    User::LeaveIfError( err );
    if ( sizeof(SRadioStubManager) > iRadioStubManagerChunk.MaxSize() )
        {
        User::Leave( KErrTooBig );
        }
    TUint8* basePtr = iRadioStubManagerChunk.Base();
    User::LeaveIfNull( basePtr );
    iRadioStubManager = (SRadioStubManager*)basePtr;
    if ( STUB.iLeaveNewL.iError )
        {
        User::Leave( STUB.iLeaveNewL.iError );
        }
    if ( STUB.iLeaveConstructL.iError )
        {
        User::Leave( STUB.iLeaveConstructL.iError );
        }
    STUB.iRemConCoreApiTargetObserver = this;
    STUB.iRemConCallHandlingTargetObserver = this;
    RadioEngineUtils::InitializeL();
    // Create interface selector.
        //iInterfaceSelector = CRemConInterfaceSelector::NewL();
    // Create a new CRemConCoreApiTarget, owned by the interface selector.
        //iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );
    // Create a new CRemConCallHandlingTarget, owned by the interface selector.
        //iCallTarget = CRemConCallHandlingTarget::NewL( *iInterfaceSelector, *this );
    // Start being a target.
        //iInterfaceSelector->OpenTargetL();
    // Create repeat timer.
    iRepeatTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRemConTargetImp* CRadioRemConTargetImp::NewL()
    {
    LOG_METHOD_AUTO;;
    CRadioRemConTargetImp* self = new ( ELeave ) CRadioRemConTargetImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRemConTargetImp::~CRadioRemConTargetImp()
    {
    LOG_METHOD_AUTO;;
    if ( iRepeatTimer )
        {
        iRepeatTimer->Cancel();
        }
    delete iRepeatTimer;
    //delete iInterfaceSelector; //deletes also iCallTarget and iCoreTarget
    //iCoreTarget = NULL;
    //iCallTarget = NULL;
    RadioEngineUtils::Release();
    iRadioStubManagerChunk.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::SetControlEventObserver( MRadioControlEventObserver* aControlEventObserver )
    {
    LOG_METHOD_AUTO;;
    iObserver = aControlEventObserver;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoCommand( TRemConCoreApiOperationId aOperationId,
                                         TRemConCoreApiButtonAction aButtonAct )
    {
    LOG_FORMAT( "aOperationId = %d, aButtonAct = %d", aOperationId, aButtonAct );
    LOG_METHOD_AUTO;
    if ( iObserver )
        {
        switch ( aOperationId )
            {
            case ERemConCoreApiChannelUp:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->ChannelUpL( ETrue ); iObserver->ChannelUpL( EFalse ))
                    }
                else if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    TRAP_IGNORE( iObserver->ChannelUpL( ETrue ))
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    TRAP_IGNORE( iObserver->ChannelUpL( EFalse ))
                    }
                else {}
                break;
                }
            case ERemConCoreApiChannelDown:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->ChannelDownL( ETrue ); iObserver->ChannelDownL( EFalse ))
                    }
                else if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    TRAP_IGNORE( iObserver->ChannelDownL( ETrue ))
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    TRAP_IGNORE( iObserver->ChannelDownL( EFalse ))
                    }
                else {}
                break;
                }
            case ERemConCoreApiVolumeUp:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->VolumeUpL( ETrue ); iObserver->VolumeUpL( EFalse ))
                    }
                else if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    iRepeatTimer->Cancel();
                    iRepeatId = ERemConCoreApiVolumeUp;
//                  iRepeatTimer->Start( KVRVolumeTimerInitialDelay, KAknStandardKeyboardRepeatRate, TCallBack( RepeatTimerCallback, this ));
                    iRepeatTimer->Start( KVRVolumeTimerInitialDelay, 1000000, TCallBack( RepeatTimerCallback, this ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    iRepeatTimer->Cancel();
                    }
                else {}
                break;
                }
            case ERemConCoreApiVolumeDown:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->VolumeDownL( ETrue ); iObserver->VolumeDownL( EFalse ))
                    }
                else if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    iRepeatTimer->Cancel();
                    iRepeatId = ERemConCoreApiVolumeDown;
//                  iRepeatTimer->Start( KVRVolumeTimerInitialDelay, KAknStandardKeyboardRepeatRate, TCallBack( RepeatTimerCallback, this ));
                    iRepeatTimer->Start( KVRVolumeTimerInitialDelay, 1000000, TCallBack( RepeatTimerCallback, this ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    iRepeatTimer->Cancel();
                    }
                else {}
                break;
                }
            case ERemConCoreApiStop:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->StopL( ETrue ); iObserver->StopL( EFalse ))
                    }
                break;
                }
            case ERemConCoreApiBackward:
            case ERemConCoreApiRewind:
                {
                if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    TRAP_IGNORE( iObserver->RewindL( ETrue ))
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    TRAP_IGNORE( iObserver->RewindL( EFalse ))
                    }
                else if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->BackwardL( ETrue ); iObserver->BackwardL( EFalse ))
                    }
                else {}
                break;
                }
            case ERemConCoreApiForward:
            case ERemConCoreApiFastForward:
                {
                if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    TRAP_IGNORE( iObserver->FastForwardL( ETrue ))
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    TRAP_IGNORE( iObserver->FastForwardL( EFalse ))
                    }
                else if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->ForwardL( ETrue ); iObserver->ForwardL( EFalse ))
                    }
                else {}
                break;
                }
            case ERemConCoreApiPlay:
            case ERemConCoreApiPause:
            case ERemConCoreApiPausePlayFunction:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->PausePlayL( ETrue ); iObserver->PausePlayL( EFalse ))
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoPlay( TRemConCoreApiPlaybackSpeed /*aSpeed*/,
                                      TRemConCoreApiButtonAction aButtonAct )
    {
    LOG_METHOD_AUTO;
    if ( iObserver )
        {
        if ( aButtonAct == ERemConCoreApiButtonClick )
            {
            TRAP_IGNORE( iObserver->PlayL( ETrue ); iObserver->PlayL( EFalse ))
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoTuneFunction( TBool /*aTwoPart*/,
                                              TUint /*aMajorChannel*/,
                                              TUint /*aMinorChannel*/,
                                              TRemConCoreApiButtonAction /*aButtonAct*/)
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoSelectDiskFunction( TUint /*aDisk*/,
                                                    TRemConCoreApiButtonAction /*aButtonAct*/)
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoSelectAvInputFunction( TUint8 /*aAvInputSignalNumber*/,
                                                       TRemConCoreApiButtonAction /*aButtonAct*/)
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoSelectAudioInputFunction( TUint8 /*aAudioInputSignalNumber*/,
                                                          TRemConCoreApiButtonAction /*aButtonAct*/)
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CRadioRemConTargetImp::RepeatTimerCallback( TAny* aPtr )
    {
    LOG_METHOD_AUTO;

    CRadioRemConTargetImp* self = reinterpret_cast<CRadioRemConTargetImp*>( aPtr );

    if ( self )
        {
        if ( self->iObserver )
            {
            switch ( self->iRepeatId )
                {
                case ERemConCoreApiVolumeUp:
                    {
                    TRAP_IGNORE( self->iObserver->VolumeUpL( ETrue ); self->iObserver->VolumeUpL( EFalse ))
                    break;
                    }
                case ERemConCoreApiVolumeDown:
                    {
                    TRAP_IGNORE( self->iObserver->VolumeDownL( ETrue ); self->iObserver->VolumeDownL( EFalse ))
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::AnswerCall()
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::AnswerEndCall()
    {
    LOG_METHOD_AUTO;
    if ( iObserver )
        {
        TRAP_IGNORE( iObserver->AnswerEndCallL())
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::DialCall( const TDesC8& /*aTelNumber*/ )
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::EndCall()
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::GenerateDTMF( const TChar /*aChar*/ )
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::LastNumberRedial()
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MultipartyCalling( const TDesC8& /*aData*/ )
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::SpeedDial( const TInt /*aIndex*/ )
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::VoiceDial( const TBool /*aActivate*/ )
    {
    LOG_METHOD_AUTO;
    }
