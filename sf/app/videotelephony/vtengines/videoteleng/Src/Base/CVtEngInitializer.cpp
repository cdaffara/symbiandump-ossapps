/*
* Copyright (c) 2004 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Implementation class for initializing the engine gradually.
*                Active object initializes one "subsystem" on each
*                round. In case of failure timer is started and
*                retrial is done after time-out. Uninitialization
*                is done is one shot.




*
*/


// INCLUDE FILES
#include    "CVtEngInitializer.h"
#include    "CVtEngStateManager.h"
#include    "CVtEngEventManager.h"
#include    "VtEngUtils.h"
#include    "CVtEngHandlerContainer.h"
#include    "CVtEngOperation.h"
#include    "VtEngPanic.h"
#include    "VtEngConstants.h"
#include    "VtEngDefs.hrh"
#include    <cvtlogger.h>
#include    <mvtprotocolhandler.h>
#include    <telcommsinfopskeys.h>
#include    <videotelephonyvariant.hrh>
#include    "VtEngUtils.h"
#include    "CVtEngSettings.h"

// CONSTANTS
const TInt KVtEngInitializerPriority = CActive::EPriorityStandard;
const TInt KVtEngInitializerRetrials = 3;
const TInt KVtEngInitializerRetrialTimeout = 500000; // 0.5 seconds
const TInt KVtEngInitializerTimerPriority = CActive::EPriorityUserInput;

_LIT( KVtEngCommDiagPortName, "PVDIAGPORT" );
_LIT( KVtEngCommTestPortName, "COMM::0" );

// ============================ MEMBER FUNCTIONS ===============================

CVtEngInitializer* CVtEngInitializer::NewL( CVtEngHandlerContainer& aHandlers )
    {
    __VTPRINTENTER( "Intlzr.NewL" )
    CVtEngInitializer* self = new ( ELeave ) CVtEngInitializer( aHandlers );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    __VTPRINTEXIT( "Intlzr.NewL" )
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::CVtEngInitializer
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngInitializer::CVtEngInitializer(
            CVtEngHandlerContainer& aHandlers ) :
            CActive( KVtEngInitializerPriority ),
            iHandlers( aHandlers ),
            iStep( EInitNone ),
            iDataportHandled( 0 ),
			iOwnershipInMH( EFalse )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::ConstructL
// Second phase constructor.
//
// -----------------------------------------------------------------------------
//
void CVtEngInitializer::ConstructL()
    {
    iTimer = CPeriodic::NewL( KVtEngInitializerTimerPriority );
    
    }

// Destructor
CVtEngInitializer::~CVtEngInitializer()
    {
    __VTPRINTENTER( "Intlzr.~" )
    delete iTimer;
    Cancel();
    
    if ( iSessionCommand && !iOwnershipInMH )
        {
        VTProtocolFactory::DeleteSessionCommand( iSessionCommand );
        iSessionCommand = NULL;
        }
    __VTPRINTEXIT( "Intlzr.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::InitializeL
// Start initialization process.
//
// -----------------------------------------------------------------------------
//
void CVtEngInitializer::InitializeL( CVtEngOperation& aCallback )
    {
    __VTPRINTENTER( "Intlzr.InitializeL" )
    __VTPRINT( DEBUG_GEN, "Intlzr.Init")
    __ASSERT_ALWAYS( iStep != EInitComplete,
        Panic( EVtEngPanicInvalidInitializationEntry ) );
    iCallback = &aCallback;
    if ( iStep == EInitComplete )
        {
        __VTPRINTEXIT( "Intlzr.InitializeL" )
        return;
        }
    ContinueOrRetry( EContinue );
    __VTPRINTEXIT( "Intlzr.InitializeL" )
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::CancelInit
// Cancels already started ini.
//
// -----------------------------------------------------------------------------
//
TBool CVtEngInitializer::CancelInit()
    {
    __VTPRINTENTER( "Intlzr.CancelInit" )
    TBool cancelSignalled = EFalse;
    __VTPRINT2( DEBUG_GEN, "Intlzr.CancelInit %d", iStep )
    if ( IsActive() )
        {
        switch ( iStep )
            {
            case EInitMedia: // media->InitializeL pending
            case EInitProto: // media->InitializeProviderL pending
                {
                if ( ! ( iDataportHandled & EDataportReceived )  )
                    {
                    __VTPRINT( DEBUG_GEN, "Intlzr.CancelInit complete" )
                    if ( iStatus == KRequestPending )
	                    {
                        // Actual protocol init is not done yet.
                        TRequestStatus* status = &iStatus;
                        __VTPRINT( DEBUG_GEN, "Intlzr.CancelInit complete1" )
                        User::RequestComplete( status, KErrNone );
                        __VTPRINT( DEBUG_GEN, "Intlzr.CancelInit complete2" )
                        }
                    iDataportHandled |= EDataportReceived;
                    __VTPRINT( DEBUG_GEN, "Intlzr.CancelInit complete" )
                    }
                __VTPRINT( DEBUG_GEN, "Intlzr.CancelInit MH cancel" )
                CVtEngMediaHandler* media =
                    static_cast<CVtEngMediaHandler*>( &iHandlers.Media() );
                media->CancelInitialize();
                iStep = EResetMediaPhase2;
                }
                break;
            case EResetMedia:
                break;
            default:
                cancelSignalled = ETrue;
                Cancel();
            }
        }
    __VTPRINTEXITR( "Intlzr.CancelInit %d", cancelSignalled )
    return cancelSignalled;
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::Uninitialize
// Start un-initialization process.
//
// -----------------------------------------------------------------------------
//
void CVtEngInitializer::Uninitialize( CVtEngOperation& aCallback )
    {
    __VTPRINTENTER( "Intlzr.Uninitialize" )
    iCallback = &aCallback;
    ContinueOrRetry( EReset );
    __VTPRINTEXIT( "Intlzr.Uninitialize" )
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::GetSessionCommand
// Transfer ownership of Protocol and continue initialization.
//
// -----------------------------------------------------------------------------
//
MVtSessionCommand* CVtEngInitializer::GetSessionCommand()
    {
    __VTPRINTENTER( "Intlzr.GetSessionCommand" )
	 __VTPRINT2( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.GetSessionCommand iSessionCommand: $%x", reinterpret_cast< TUint >( iSessionCommand ) )
    MVtSessionCommand* sessionCommand = iSessionCommand;

	// Set ownership to media handler since media handler is requesting so.
	iOwnershipInMH = ETrue;
	if( iStep >= EInitDataportCompleted )
		{
		// Ownership can be totally given to MH after dataport is loaned.
		__VTPRINT( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.GetSessionCommand iSessionCommand ownership to MH GetSessionCommand")
		
		iSessionCommand = NULL;
		iStep = EInitComplete;
		ContinueOrRetry( EContinue );		
		}
    if ( iStep != EResetMedia )
        {
        __VTPRINT( DEBUG_GEN, "Intlzr.GetSessionCommand=>ContinueOrRetry( EContinue )")
        ContinueOrRetry( EContinue );
        }
    else
        {
        // Reset requested. ContinueOrRetry with EReset was called
        // while protocol Init was ongoing. Now continue
        __VTPRINT( DEBUG_GEN, "Intlzr.GetSessionCommand=>ContinueOrRetry( EReset )")
        ContinueOrRetry( EReset );
        }
    __VTPRINTEXIT( "Intlzr.GetSessionCommand" )
    return sessionCommand;
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::Initialized
// Returns if engine is initialized.
//
// -----------------------------------------------------------------------------
//
TBool CVtEngInitializer::Initialized() const
    {
    return ( iStep == EInitComplete );
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::RunL
// Perform initialization step
//
// -----------------------------------------------------------------------------
//
void CVtEngInitializer::RunL()
    {
    __VTPRINTENTER( "Intlzr.RunL" )
    __VTPRINT3( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.Run step=%d err=%d",
        iStep, iStatus.Int() )
    if ( iStatus != KErrNone )
        {
        Notify( iStatus.Int() );
        __VTPRINTEXIT( "Intlzr.RunL" )
        return;
        }
    CVtEngMediaHandler* media =
        static_cast<CVtEngMediaHandler*>( &iHandlers.Media() );

    switch ( iStep )
        {
        case EInitNone:
            return;
        case EInitMedia:
            // Initialize video source
            iStatus = KRequestPending;
            media->InitializeL( iStatus );
            SetActive();
            iStep++; // EInitProto
            __VTPRINTEXIT( "Intlzr.RunL" )
            return;
        case EInitProto:
            // Initialize Protocol => create terminal only once.
            __VTPRINT( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.Run create Protocol")
            if ( !iSessionCommand )
                {
                __VTPRINT( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.Run DOCREATE Protocol" )
                TVt3G324MSupported protocolSupported;
                iSessionCommand = VTProtocolFactory::CreateSessionCommandL(
                    media, ETrue, protocolSupported );
                    
                if (protocolSupported == EVt3G324MMissing)
                    {
                    __VTPRINT( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.Run init 3G324M stack is missing")    
                    Notify(KErrNotReady);
                    return;   
                    }

				// Initialize Protocol immediately after it is created.
				if( iSessionCommand )
					{
					__VTPRINT( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.Run InitL protocol")
					iSessionCommand->InitProtocolL( media->SdkInitInfo() );
					}
                }
            if ( iSessionCommand )
                {
                const TDesC* dataportName = NULL;
                __VTPRINT( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.Run init protocol")
                if(iCallback->Command() == KVtEngInitializeEngineTest )
                    {
                    __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                        "Intlzr.Run init with COMM::0")
                    CVtEngUtility::Settings().SetConnectReady();    
                    iComms = VTProtocolFactory::CreateCommServerL(
                        KVtEngCommTestPortName );
                    }
                else if( iCallback->Command() == KVtEngInitializeEngineDiag )
                    {
                    __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                        "Intlzr.Run init with PVDIAGPORT")
                    CVtEngUtility::Settings().SetConnectReady();    
                    iComms = VTProtocolFactory::CreateCommServerL(
                    KVtEngCommDiagPortName  );
                    }
                else
                    { // [MediatorChange]: 
                    __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                        "Intlzr.Run init with DATAPORT" )
                    dataportName = 
                        CVtEngUtility::Settings().DataportInfo();
                    if ( !dataportName ) 
                        {
                        // not available yet, start waiting,
                        // HandleSettingChangedL is called when dataport
                        // becomes available
                        __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                                                "Intlzr.Run DATAPORT is not ready yet" )
                        
                        CVtEngUtility::Settings().NotifyChangeL( 
                        CVtEngSettings::EDataportInfo, *this );
                        iStatus = KRequestPending;
                        SetActive();                        
                        __VTPRINTEXIT( "Intlzr.RunL" )
                        return;
                        }
                    else
                        {
                        //RDebug::Print( _L("DataportName is:%S"), dataportName );
                        TBool bitReversed( BitReversalUsed() );
                                        
                        iComms = VTProtocolFactory::CreateCommServerL(
                           *dataportName, bitReversed );
                        // not interested of DP info any more
                        CVtEngUtility::Settings().CancelNotifyChange( 
                            CVtEngSettings::EDataportInfo, *this );
                        }
                    }
                    
                // Initialization is now completed
					iStep = EInitDataportCompleted;

					// Dataport is now loaned
					CVtEngUtility::Settings().SetDataportLoaned( ETrue );

					// Update states since dataport is now loaned
					CVtEngStateManager* states = CVtEngUtility::StateManager();
					states->Update();

					// No need for ownership anymore, media handler owns the pointer and is 
					// responsible of deletion.
					if( iOwnershipInMH )
						{
						__VTPRINT( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.Run iSessionCommand ownership to MH")						
						iSessionCommand = NULL; 
						iStep = EInitComplete;
						ContinueOrRetry( EContinue );						
						}
                    
                    __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                        "Intlzr.Run init protocol port operation completed")
                // Media handler calls GetSessionCommand when InitL
                // is complete and initialization continues.
                __VTPRINTEXIT( "Intlzr.RunL" )
                return;
                }
            break;
        case EInitSession:
            {
            CVtEngSessionHandler* session =
                static_cast<CVtEngSessionHandler*>( &iHandlers.Session() );
            session->InitializeL();
            __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                    "Intlzr.RunL StartMediatorListenerL" )
            CVtEngUtility::EngineUtils()->StartMediatorListenerL();
            }
            break;
        case EResetMedia:
            iHandlers.Uninitialize();
            iStatus = KRequestPending;
            media->Uninitialize( iStatus );
            SetActive();
            iStep++; // EResetMediaPhase2
            __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                "Intlzr.RunL step reset media started" )
            __VTPRINTEXIT( "Intlzr.RunL" )
            return;
        case EResetMediaPhase2:
            if ( iComms )
                {
                __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                    "Intlzr.RunL VTENgine deletes COMM server" )
                CVtEngMediaHandler* media =
                static_cast<CVtEngMediaHandler*>( &iHandlers.Media() );
                if ( media->ProtoInitialized() )
                    {
                    TRAPD( err, VTProtocolFactory::DeleteCommServerL( iComms ) );
                    __VTPRINT2( DEBUG_GEN | DEBUG_DETAIL,
                        "Intlzr.RunL VTENgine deletes COMM server err=%d", err )
                    __ASSERT_ALWAYS( err == KErrNone,
                        Panic( EVtEngPanicCommsDestructOnInvalidState ) );
                    iComms = NULL;
                    }
                else if ( !media->ProtoInitialized() && iSessionCommand )
                    {
                    __VTPRINTEXIT( "Intlzr.RunL" )
                    return;
                    }
                }
            break;
        case EInitComplete:
            break;
        default:
            Panic( EVtEngPanicInvalidInitializationState );
            break;
        }
    ContinueOrRetry( EContinue );
    __VTPRINTEXIT( "Intlzr.RunL" )
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::DoCancel
// Cancels initialization
//
// -----------------------------------------------------------------------------
//
void CVtEngInitializer::DoCancel()
    {
    __VTPRINTENTER( "Intlzr.DoCancel" )    
    if ( iStatus == KRequestPending )
        {
        __VTPRINT2( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.DoCancel step=%d", iStep )
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrCancel );
        }
    iStep = EInitNone;
    iCallback = NULL;
    __VTPRINTEXIT( "Intlzr.DoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::RunError
// Handler error on initialization.
//
// -----------------------------------------------------------------------------
//
TInt CVtEngInitializer::RunError( TInt aError )
    {
    __VTPRINTENTER( "Intlzr.RunError" )
    __VTPRINT2( DEBUG_GEN, "Intlzr.RunError %d", aError )
    if ( iRetrials )
        {
        __VTPRINT2( DEBUG_GEN | DEBUG_DETAIL, "Intlzr.retrials %d", iRetrials )
        TCallBack callback( CVtEngInitializer::HandleTimer, this );
        iTimer->Start(
            KVtEngInitializerRetrialTimeout,
            KVtEngInitializerRetrialTimeout,
            callback );
        }
    else
        {
        Notify( aError );
        }
    __VTPRINTEXIT( "Intlzr.RunError" )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::HandleTimer
// Handler for retrial timer.
//
// -----------------------------------------------------------------------------
//
TInt CVtEngInitializer::HandleTimer( TAny* aAny )
    {
    CVtEngInitializer* handler =
        reinterpret_cast<CVtEngInitializer*>( aAny );
    handler->DoHandleTimer();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::DoHandleTimer
// Handler for retrial timer.
//
// -----------------------------------------------------------------------------
//
void CVtEngInitializer::DoHandleTimer()
    {
    iTimer->Cancel();
    ContinueOrRetry( ERetry );
    }

TBool CVtEngInitializer::ActivateAndSignal( TInt aSignalValue )
    {
    __VTPRINTENTER( "Intlzr.ActivateAndSignal" )
    TRequestStatus* status = &iStatus;
    TBool signaled = EFalse;
    if ( iStatus.Int() != KRequestPending && !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete( status, aSignalValue );
        signaled = ETrue;
        }                
    __VTPRINTEXITR( "Intlzr.ActivateAndSignal signaled=%d", signaled )
    return signaled;
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::CreateDtmfHandlerL
// Creates handler for DTMF sending.
//
// -----------------------------------------------------------------------------
//
void CVtEngInitializer::CreateDtmfHandlerL( MVtH324ConfigCommand* aH324Config )
    {
    iHandlers.CreateDtmfHandlerL( aH324Config );
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::ContinueOrRetry
// Proceed to next step, retry on failure or notify observer.
//
// -----------------------------------------------------------------------------
//
void CVtEngInitializer::ContinueOrRetry( const TProcess aOperation )
    {
      __VTPRINTENTER( "Intlzr.ContinueOrRetry" )
    __VTPRINT2( DEBUG_GEN, "Intlzr.CoR op=%d", aOperation )
    if ( aOperation == EContinue )
        {
        const TBool resetComplete( iStep == EResetMediaPhase2 );
        // check if last step complete and notify observer
        if ( iStep == EInitComplete || resetComplete )
            {
            if ( resetComplete && 
                // resetComplete is true also when clearing state is entered
                // while waiting for dataport. In that case Final Uninit must
                // not be done because MediaHandler's state is not ready for
                // it => need to wait for KVtEngResetEngine from UI
                iCallback->Command() == KVtEngResetEngine )
                {
                CVtEngMediaHandler* media =
                    static_cast<CVtEngMediaHandler*>( &iHandlers.Media() );
                media->FinalizeUninitialization();
                iStep = EInitNone;
                }
            Notify( KErrNone );
            __VTPRINT2( DEBUG_GEN, "Intlzr.CoR op=EContinue, branch=%d", 1 )
            return;
            }
		else if( iStep == EInitProto )
			{
			__VTPRINT2( DEBUG_GEN, "Intlzr.CoR op=EContinue, branch=%d", 2 )
			return;
			}
        __VTPRINT2( DEBUG_GEN, "Intlzr.CoR op=EContinue, branch=%d", 3 )
        iStep++;
        iRetrials = KVtEngInitializerRetrials;
        }
    else if ( aOperation == EReset )
        {
        __VTPRINT( DEBUG_GEN, "Intlzr.CoR op=EReset")
        iRetrials = KVtEngInitializerRetrials;
        const TInt step( iStep );
        if ( step != EInitComplete && IsActive() )
            {
            // Reset requested while init ongoing but
            // Protocol InitL not yet issued
            if ( CancelInit() == EFalse )
                { // AO still active, must return here because end of this method 
                __VTPRINT( DEBUG_GEN, " Initializer CoR 2")
                __VTPRINTEXIT( "Intlzr.ContinueOrRetry" )
                return;
                }
            }
        iStep = EResetMedia;
        if ( ( step == EInitProto ||
               step == EInitSession ) && !IsActive() )
            {
            // Protocol InitL is pending. Wait until it completes.
            // GetSessionCommand is called and there we this is
            // again called.
            __VTPRINT( DEBUG_GEN, " Initializer reset while InitL")
            __VTPRINTEXIT( "Intlzr.ContinueOrRetry" )
            return;
            }
        }
    else
        {
        iRetrials--;
        }
    ActivateAndSignal( KErrNone );    
    __VTPRINTEXIT( "Intlzr.ContinueOrRetry" )
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::Notify
// Notifies observer on completion or error.
//
// -----------------------------------------------------------------------------
//
void CVtEngInitializer::Notify( const TInt aResult )
    {
    __VTPRINTENTER( "Intlzr.Notify" )
    if ( iStep == EInitComplete )
        {
        CVtEngStateManager* states = CVtEngUtility::StateManager();
        states->Update();
        }

    if ( iCallback )
        {
        if ( aResult != KErrCancel )
            { // UI is not interested on cancel completion
            iCallback->HandleOpComplete( aResult );
            }
        iCallback = NULL;
        __VTPRINT2( DEBUG_GEN, "Intlzr.Notify res=%d", aResult )
        }
    __VTPRINTEXIT( "Intlzr.Notify" )
    }


// -----------------------------------------------------------------------------
// CVtEngInitializer::BitReversalUsed
// Checks if bit reversal should be used from locally variated information.
// -----------------------------------------------------------------------------
//
TBool CVtEngInitializer::BitReversalUsed()
    {
    __VTPRINT( DEBUG_GEN, "Intlzr.BitReversalUsed" )
    return CVtEngUtility::Settings().CheckBits(
        KVTLVFlagEnableBitReversal );
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::HandleSettingChangedL
// Notification of received dataport name.
// -----------------------------------------------------------------------------
//
void CVtEngInitializer::HandleSettingChangedL( 
    CVtEngSettings::TSettingId aId, 
    const TDesC& /*aValue*/ )
    {
    __VTPRINTENTER( "Intlzr.HandleSettingChangedL" )
    if ( !( iDataportHandled & EDataportReceived ) && 
            aId == CVtEngSettings::EDataportInfo && 
            iStatus == KRequestPending )
     
        {        
        iDataportHandled |= EDataportReceived;
        //dataport will be fetched in runl, otherwise, cancelinit can not work
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }    
    __VTPRINTEXIT( "Intlzr.HandleSettingChangedL" )
    }      

// -----------------------------------------------------------------------------
// CVtEngInitializer::GetVtComms
// Return pointer to communication port.
// -----------------------------------------------------------------------------
//
MCommServer* CVtEngInitializer::GetVtComms()
	{
	return iComms;
	}

//  End of File
