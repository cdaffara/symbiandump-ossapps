/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Recovery system class.
*
*/


// INCLUDE FILES
#include    "ctelerecoverysystem.h"
#include    <e32svr.h>
#include    <startupdomainpskeys.h>

#include    "phonelogger.h"
#include    "phoneui.pan"
#include    "cphonetimer.h"
#include    "ctelewaitingidle.h"


// CONSTANTS
const TInt KPhoneRecoveryItemsArrayGranularity = 8;
const TInt KPhoneRecoveryTickInterval = 50000; // 0.05 second
const TInt KPhoneRecoveryTicksForCritical = 2;
const TInt KPhoneRecoveryTicksForHigh = 3; 
const TInt KPhoneRecoveryTicksForStandard = 5;
const TInt KPhoneRecoveryTicksForLow = 23;
const TInt KPhoneRecoveryTicksForDontCare = 37;
const TInt KPhoneRecoveryTicksMaximum = 2048; //maximum about 1.5 minutes
const TInt KPhoneRecoveryDontStartTimer = 10000;
const TInt KPhoneRecoveryCounterStart = 0;
const TInt KPhoneRecoveryCounterPause = -1;
const TInt KPhoneRecoveryMainTimerPriority = CActive::EPriorityLow;
const TInt KPhoneRecoveryAllStepsTimerPriority = CActive::EPriorityLow + 1;

// Priority of active object; set it quite high.
const TInt KTelePubSubPriority = CActive::EPriorityUserInput;

// Flags for all priorities.
enum 
    {
    EPhoneFlagCritical = 1,
    EPhoneFlagHigh     = 2,
    EPhoneFlagStandard = 4,
    EPhoneFlagLow      = 8,
    EPhoneFlagDontCare = 16
    };
    
// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CTeleRecoverySystem::CTeleRecoverySystem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTeleRecoverySystem::CTeleRecoverySystem() 
    : CActive( KTelePubSubPriority )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::ConstructL()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::ConstructL ()" ); 
    iRequest = EPhoneNone;
    
    // Construct Recovery items array.
    iRecoveryItems = new ( ELeave ) CArrayFixFlat< TRecoveryItem >( 
        KPhoneRecoveryItemsArrayGranularity );

    // Construct CPhoneTimer.
    iTimer = CPhoneTimer::NewL( KPhoneRecoveryMainTimerPriority );    

    // Construct timer for recovery all steps.
    iAllStepsTimer = CPhoneTimer::NewL( KPhoneRecoveryAllStepsTimerPriority );
    
    iWaitingIdle = CTeleWaitingIdle::NewL( CActive::EPriorityLow, *this );
    iWaitingIdle->StartWaitingIdleL();
    
    __PHONELOG( 
        EBasic, 
        EPhoneUIUtils, 
        "CTeleRecoverySystem::ConstructL: timers" );

    User::LeaveIfError( 
        iSimStatusProperty.Attach( 
            KPSUidStartup, 
            KPSSimStatus ) );

    __PHONELOG( 
        EBasic, 
        EPhoneUIUtils, 
        "CTeleRecoverySystem::ConstructL: attach2" );

    // Check current SIM status.
    User::LeaveIfError( CheckSIMAvailable() );
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTeleRecoverySystem* CTeleRecoverySystem::NewL()
    {
    CTeleRecoverySystem* self = new( ELeave ) CTeleRecoverySystem;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// Destructor
CTeleRecoverySystem::~CTeleRecoverySystem()
    {
    // Stop listening for SIM changes.
    Cancel();    

    iSimStatusProperty.Close();
    iSimPresentProperty.Close();

    if ( iWaitingIdle )
        {
        delete iWaitingIdle;
        iWaitingIdle = NULL;  
        }
        
    delete iRecoveryItems;
    delete iTimer;
    delete iAllStepsTimer;
    }
    
// -----------------------------------------------------------------------------
// CTeleRecoverySystem::AddL
// -----------------------------------------------------------------------------
//
TPhoneRecoveryId CTeleRecoverySystem::AddL( TCallBack aCallBack, 
    TPhoneRecoveryPriority aPriority, TRecoveryState aState )
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::AddL()" ); 
    __ASSERT_DEBUG( !iIsCallBack, Panic( EPhoneUtilsNoCallBack ) );   

    CheckStateForPrecondition( aState );

    iIdCounter++;
    TInt count = iRecoveryItems->Count();
    TInt i = 0;
    for ( ; i < count; i++ )
        {
        if ( iRecoveryItems->At( i ).iPriority <= aPriority )
            {
            break;
            }
        }
    TRecoveryItem item;
    item.iCallBack = aCallBack;
    item.iId = iIdCounter;
    item.iPriority = aPriority;
    item.iState = aState;
    
    __PHONELOG1( 
        EBasic, 
        EPhoneUIUtils, 
        "CTeleRecoverySystem::AddL itemIdx= %d:", i );

    iRecoveryItems->InsertL( i, item );

    StartTimerIfRequired( aState );
    return iIdCounter;
    }
    
// -----------------------------------------------------------------------------
// CTeleRecoverySystem::Add
// -----------------------------------------------------------------------------
//
TPhoneRecoveryId CTeleRecoverySystem::Add( TCallBack aCallBack, 
    TPhoneRecoveryPriority aPriority, TRecoveryState aState )
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::AddL()" ); 
    TPhoneRecoveryId id = KNullId;
    TRAP_IGNORE( id = AddL( aCallBack, aPriority, aState ) );
    return id;
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::Remove
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::Remove( TPhoneRecoveryId aId )
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::Remove()" ); 
    __ASSERT_DEBUG( !iIsCallBack, Panic( EPhoneUtilsNoCallBack ) ); 

    TInt count = iRecoveryItems->Count();
    
    __PHONELOG1( 
        EBasic, 
        EPhoneUIUtils, 
        "CTeleRecoverySystem::count = %d:", count );
        
    for ( TInt i = 0; i < count; i++ )
        {
        if( iRecoveryItems->At( i ).iId == aId )
            {
            iRecoveryItems->Delete( i );
            __PHONELOG1( 
                EBasic, 
                EPhoneUIUtils, 
                "CTeleRecoverySystem::Delete itemIdx= %d:", i );
            return;
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CTeleRecoverySystem::SetState
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::SetState( 
    TPhoneRecoveryId aId, TRecoveryState aState )
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::SetState()" );
    CheckStateForPrecondition( aState );
    
    TInt count = iRecoveryItems->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TRecoveryItem& item = iRecoveryItems->At( i );
        if( item.iId == aId )
            {
            item.iState = aState;
            StartTimerIfRequired( aState ); 
            __PHONELOG1( 
                EBasic, 
                EPhoneUIUtils, 
                "CTeleRecoverySystem::SetState itemIdx= %d:", item.iId );      
            return;
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CTeleRecoverySystem::ResetPending
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::ResetPending()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::ResetPending()" );
    TBool startTimer = EFalse;
    TInt count = iRecoveryItems->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TRecoveryItem& item = iRecoveryItems->At( i );
        if( item.iState == EPhoneStatePending )
            {
            item.iState = EPhoneStateWaiting;
            startTimer = ETrue;
            }
        }

    if ( startTimer )
        {
        StartTimer();
        }
    }
    
// -----------------------------------------------------------------------------
// CTeleRecoverySystem::RecoverNow
// -----------------------------------------------------------------------------
//
TInt CTeleRecoverySystem::RecoverNow( TPhoneRecoveryId aId,
    TPhoneRecoveryPriority aPriority, TBool aAllSteps )
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::RecoverNow()" );
    TInt err = KErrNone;
    TInt pos = Find( aId );

    if ( pos != KErrNotFound )
        {
        TRecoveryItem& item = iRecoveryItems->At( pos );
        item.iPriority = aPriority;
        err = DoItemCallBack( item );
        if ( item.iState == EPhoneStateWaiting )
            {
            if ( aAllSteps )
                {
                item.iState = EPhoneStateStarting;
                }
                
            StartTimerIfRequired( item.iState );
            }
        }
    __PHONELOG1( 
        EBasic, 
        EPhoneUIUtils, 
        "CTeleRecoverySystem::RecoverNow error = %d:", err );   
    return err;
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::RecoverAllNow
//
// -----------------------------------------------------------------------------
//   
void CTeleRecoverySystem::RecoverAllNow()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::RecoverAllNow()" );
    StartTimerIfIdle();
    StartAllStepsTimer();
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::EnablePrecondition
//
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::EnablePrecondition()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::EnablePrecondition()" );
    // If precondition has already been satisfied, then
    // there should not be anything left to do.
    if ( iPrecondOk )
        {
        return;
        }

    // Store flag to indicate that precondition has been
    // satisfied.
    iPrecondOk = ETrue;

    // Go through all recovery items and update state so that
    // there won't be any items in EPhoneStatePrecond after the loop.
    // Timer must be started.
    TInt count = iRecoveryItems->Count();

    for ( TInt index = 0; index < count; index++ )
        {
        TRecoveryItem& item = iRecoveryItems->At( index );

        CheckStateForPrecondition( item.iState );
        }

    if ( IsIdle() )
        {
        iTimer->Cancel();
        StartTimer();       
        }
    }
    
// -----------------------------------------------------------------------------
// CTeleRecoverySystem::Resume
// 
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::Resume()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::Resume()" );
    if ( iTickCounter == KPhoneRecoveryCounterPause )
        {
        StartTimer();
        }
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::RunL
// 
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::RunL()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::RunL()" );
    if ( iStatus != KErrNone )
        {
        // If SIM is already available, just listen for SIM status.
        if ( iSimAvailable == ESimUsable )
            {            
            iRequest = EPhoneSIMStatus;
            }
        else
            {
            iRequest = EPhoneSIMPresent;
            }

        __PHONELOG1( 
            EBasic, 
            EPhoneUIUtils, 
            "CTeleRecoverySystem::RunL NewRq = %d ", (TInt) iRequest ); 

        IssueRequest();
        return;
        }
        
    // If SIM present availability is still scrutinized, check status.
    if ( iRequest == EPhoneSIMPresent )
        {
        if ( iSimPresentProperty.Handle() )
            {            
            iSimPresentProperty.Get( iSimAvailable );
            }            

        if ( iSimAvailable == ESimUsable )
            {
            iRequest = EPhoneSIMStatus;
            
            __PHONELOG1( 
                EBasic, 
                EPhoneUIUtils, 
                "CTeleRecoverySystem::RunL NewRq = %d ", (TInt) iRequest ); 
            }
        else
            {
            // SIM is not yet available, start listening again.
            IssueRequest();
            return;
            }
        }

    // If SIM is available and SIM status is ok and precondition has not been
    // set, set precondition.
    if ( IsSIMOk() && !iPrecondSimOk )
        {
        EnablePrecondSimOk();
        }
    
    // Start listening again.
    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::DoCancel()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::DoCancel()");

    if ( iSimStatusProperty.Handle() )
        {        
        iSimStatusProperty.Cancel();
        }

    if ( iSimPresentProperty.Handle() )
        {        
        iSimPresentProperty.Cancel();
        }                
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::EnablePrecondSimOk
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::EnablePrecondSimOk()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::EnablePreSIM()");
    
    if ( iPrecondSimOk )
        {
        return;
        }
    iPrecondSimOk = ETrue;

    // Go through all recovery items and update state so that
    // there won't be any items in EPhoneStatePrecondSim after the loop.
    // Timer must be started.
    TInt count = iRecoveryItems->Count();

    for ( TInt index = 0; index < count; index++ )
        {
        TRecoveryItem& item = iRecoveryItems->At( index );

        CheckStateForPrecondition( item.iState );
        }

    iTimer->Cancel();
    StartTimer();
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::DoItemCallBack
// -----------------------------------------------------------------------------
//
TInt CTeleRecoverySystem::DoItemCallBack( TRecoveryItem& aItem )
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::DoItemCallBack()");
    
#ifdef _DEBUG
    iIsCallBack = ETrue;
#endif // _DEBUG

    TInt err = KErrNone;    
    TRAPD( leaveErr, err = aItem.iCallBack.CallBack() );

    __PHONELOG1( 
        EBasic, 
        EPhoneUIUtils, 
        "CTeleRecoverySystem::DoItemCallBack Err = %d ", leaveErr );
    
#ifdef _DEBUG
    iIsCallBack = EFalse;
#endif // _DEBUG

    if( leaveErr != KErrNone )
        {
        err = leaveErr;
        }

    if ( err > KErrNone )
        {
        aItem.iState = EPhoneStateWaiting;
        err = KErrNone;
        }
    else
        {
        switch ( err )
            {
            case KErrNone:
                aItem.iState = EPhoneStateIdle;
                break;
            case KErrNoMemory:
            case KErrInUse:
            case KErrServerBusy:
            case KErrNotReady:
            case KErrLocked:
            case KErrTimedOut:
            case KErrDied:
            case KErrServerTerminated:
                aItem.iState = EPhoneStateWaiting;
                break;
            default:
                aItem.iState = EPhoneStatePending;
                break;
            }
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::HandleTimer
//
// Callback function.
// -----------------------------------------------------------------------------
//
TInt CTeleRecoverySystem::HandleTimer( TAny* aAny )
    {
    return static_cast< CTeleRecoverySystem* >( aAny )->DoHandleTimer();
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::DoHandleTimer
// -----------------------------------------------------------------------------
//
TInt CTeleRecoverySystem::DoHandleTimer()
    {
    // It will be sum of flags in which there are items in priority.
    TInt found = 0;

    // It contains flags which will be run at the current time.
    const TInt run = DetermineRun();
    
    // Go through all items and for each item:
    //  If item is in waiting state,
    //      update variable found and
    //      update priority and
    //      perform action if required.

    const TInt count = iRecoveryItems->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TRecoveryItem& item = iRecoveryItems->At( i );
        if( item.iState == EPhoneStateWaiting )
            {
            TBool doCallBack = EFalse;

            if ( item.iPriority >= EPhonePriorityCritical )
                {
                found |= EPhoneFlagCritical;
                doCallBack = ( run & EPhoneFlagCritical );
                }
            else if ( item.iPriority > EPhonePriorityHigh )
                {
                // For priority Critical.
                item.iPriority--;
                found |= EPhoneFlagHigh;
                doCallBack = ( run & EPhoneFlagHigh );
                }
            else if ( item.iPriority > EPhonePriorityStandard )
                {
                // For priority High.
                item.iPriority--;
                found |= EPhoneFlagStandard;
                doCallBack = ( run & EPhoneFlagHigh );
                }
            else if ( item.iPriority > EPhonePriorityLow )
                {
                // For priority Standard.
                item.iPriority--;
                found |= EPhoneFlagLow;
                doCallBack = ( run & EPhoneFlagStandard );
                }
            else if ( item.iPriority > EPhonePriorityDontCare )
                {
                // For priority Low.
                item.iPriority--;
                found |= EPhoneFlagDontCare;
                doCallBack = ( run & EPhoneFlagLow );
                }
            else 
                {
                // For priority DontCare.
                found |= EPhoneFlagDontCare;
                doCallBack = ( run & EPhoneFlagDontCare );
                }

            if ( doCallBack )
                {
                DoItemCallBack( item );
                }
            }
        }

    // Start timer for the next time.
    StartNext( found );       
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::HandleAllStepsTimer
//
// Callback function.
// -----------------------------------------------------------------------------
//
TInt CTeleRecoverySystem::HandleAllStepsTimer( TAny* aAny )
    {
    return 
        static_cast< CTeleRecoverySystem* >( aAny )->DoHandleAllStepsTimer();
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::DoHandleAllStepsTimer
// -----------------------------------------------------------------------------
//
TInt CTeleRecoverySystem::DoHandleAllStepsTimer()
    {
    TInt count = iRecoveryItems->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TRecoveryItem& item = iRecoveryItems->At( i );
        if( item.iState == EPhoneStateStarting )
            {
            // Perform recovery step.
            //
            TInt err = DoItemCallBack( item );

            if ( item.iState == EPhoneStateWaiting )
                {
                // If step went ok, then we can perform next step soon.
                // If step didn't succeed, then there is no point to 
                // start again immediately.
                if ( err == KErrNone )
                    {
                    item.iState = EPhoneStateStarting;
                    }
                
                // And start timer.
                StartTimerIfRequired( item.iState );
                }
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::CheckStateForPrecondition
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::CheckStateForPrecondition( TRecoveryState& aState )
    {
    if ( iPrecondOk && aState == EPhoneStatePrecond )
        {
        aState = EPhoneStateWaiting;
        }

    if ( iPrecondSimOk && aState == EPhoneStatePrecondSim )
        {
        aState = EPhoneStateWaiting;
        }
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::StartTimerIfRequired
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::StartTimerIfRequired( TRecoveryState aNewState )
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::StartTimerIfRequired()");
    
    if ( IsIdle() )
        {
        if ( aNewState == EPhoneStateWaiting )
            {
            StartTimerIfIdle();
            }
        else if ( aNewState == EPhoneStateStarting )
            {
            StartAllStepsTimer();
            }       
        }
    }
    
// -----------------------------------------------------------------------------
// CTeleRecoverySystem::IsIdle()
// -----------------------------------------------------------------------------
//    
TBool CTeleRecoverySystem::IsIdle()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::IsIdle()");
    TBool retVal(EFalse);
    TInt error(0);
    TInt state(0);
    
    error = iIdleActive.Get(
        KPSUidStartup, 
        KPSIdlePhase1Ok, 
        state );
    
    if ( ( state == EIdlePhase1Ok ) &&
        ( error == KErrNone ) )
        {
        if ( iWaitingIdle )
            {
            delete iWaitingIdle;
            iWaitingIdle = NULL;  
            }
        retVal = ETrue;
        }

    return retVal;      
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::StartTimerIfIdle
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::StartTimerIfIdle()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::StartTimerIfIdle()");

    if ( !iTimer->IsActive() )
        {
        iTimer->After( KPhoneRecoveryTickInterval,
            TCallBack( HandleTimer, this ) );
        iTickCounter++;
        }
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::StartTimer
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::StartTimer()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::StartTimer()");
   
    if ( !iTimer->IsActive() )
        {
        iTickCounter = KPhoneRecoveryCounterStart;
        iTimer->After( KPhoneRecoveryTickInterval, 
            TCallBack( HandleTimer, this ) );
        }       
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::StartAllStepsTimer
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::StartAllStepsTimer()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::StartAllStepsTimer()");
    if ( !iAllStepsTimer->IsActive() )
        {
        iAllStepsTimer->After( 1, TCallBack( HandleAllStepsTimer, this ) );
        }
  }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::StartNext
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::StartNext( TInt aFound )
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::StartNext()");
    // Calculate time of next attempt.
    
    // This is performed as follows:
    //  For each priority:
    //      If there was item in waiting state, 
    //      then calculate next attempt time for that.
    //  Use the minimum attempt time to determine next attempt.
    
    TInt nextTick = KPhoneRecoveryDontStartTimer; 
    if ( aFound & EPhoneFlagCritical )
        {
        nextTick = Min( nextTick, NextTick( KPhoneRecoveryTicksForCritical ) );
        }
    if ( aFound & EPhoneFlagHigh )
        {
        nextTick = Min( nextTick, NextTick( KPhoneRecoveryTicksForHigh ) );
        }
    if ( aFound & EPhoneFlagStandard )
        {
        nextTick = Min( nextTick, NextTick( KPhoneRecoveryTicksForStandard ) );
        }
    if ( aFound & EPhoneFlagLow )
        {
        nextTick = Min( nextTick, NextTick( KPhoneRecoveryTicksForLow ) );
        }
    if ( aFound & EPhoneFlagDontCare )
        {
        nextTick = Min( nextTick, NextTick( KPhoneRecoveryTicksForDontCare ) );
        }
    
    if( nextTick < KPhoneRecoveryDontStartTimer )
        {
        if ( nextTick <= KPhoneRecoveryTicksMaximum )
            {
            iTimer->After( KPhoneRecoveryTickInterval*( nextTick-iTickCounter ), 
                TCallBack( HandleTimer, this ) );
            iTickCounter = nextTick;
            }
        else 
            {
            //pause the recover to save memory
            iTickCounter = KPhoneRecoveryCounterPause;
            }
        }
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::DetermineRun
// -----------------------------------------------------------------------------
//
TInt CTeleRecoverySystem::DetermineRun() const
    {
    TInt run = 0;

    // Check if critical tasks ought to be run.
    if ( !( iTickCounter % KPhoneRecoveryTicksForCritical ) )
        {
        run |= EPhoneFlagCritical;
        }
    
    // Check if high tasks ought to be run.
    if ( !( iTickCounter % KPhoneRecoveryTicksForHigh ) )
        {
        run |= EPhoneFlagHigh;
        }
    
    // Check if standard tasks ought to be run.
    if ( !( iTickCounter % KPhoneRecoveryTicksForStandard ) )
        {
        run |= EPhoneFlagStandard;
        }
    
    // Check if low tasks ought to be run.
    if ( !( iTickCounter % KPhoneRecoveryTicksForLow ) )
        {
        run |= EPhoneFlagLow;
        }
    
    // Check if 'dont care' tasks ought to be run.
    if ( !( iTickCounter % KPhoneRecoveryTicksForDontCare ) )
        {
        run |= EPhoneFlagDontCare;
        }

    if ( !run )
        {
        run |= EPhoneFlagCritical;
        }

    return run;
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::NextTick
// -----------------------------------------------------------------------------
//
inline TInt CTeleRecoverySystem::NextTick( TInt aTicks ) const
    {
    // Find smallest such value k that satisfies the following conditions:
    //  1. It must be larger than iTickCounter
    //  2. It must be multiple of aTicks.

    return ( iTickCounter / aTicks + 1 ) * aTicks; 
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::Find
// -----------------------------------------------------------------------------
//
TInt CTeleRecoverySystem::Find( TPhoneRecoveryId aId ) const
    {
    const TInt count = iRecoveryItems->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        const TRecoveryItem& item = iRecoveryItems->At( i );
        if ( item.iId == aId )
            {
            return i;
            }
        }

    return KErrNotFound;
    }
    
// -----------------------------------------------------------------------------
// CTeleRecoverySystem::IssueRequest
// 
// -----------------------------------------------------------------------------
//
void CTeleRecoverySystem::IssueRequest()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::IssueRequest()");
    if ( !IsActive() )
        {
        if ( iRequest == EPhoneSIMPresent )
            {                        
            iSimPresentProperty.Subscribe( iStatus );

            __PHONELOG( 
                EBasic, 
                EPhoneUIUtils, 
                "CTeleRecoverySystem::DoItemCallBack IssueReq.2");
            }
        else
            {
            __PHONELOG( 
                EBasic, 
                EPhoneUIUtils, 
                "CTeleRecoverySystem::DoItemCallBack IssueReq.3");
                
            __ASSERT_DEBUG( iRequest == EPhoneSIMStatus, Panic( EPhoneUtilsNoCallBack ) );

            iSimStatusProperty.Subscribe( iStatus );
            }

        SetActive();
        }
    }
    
    
// -----------------------------------------------------------------------------
// CTeleRecoverySystem::IsSIMOk
// 
// -----------------------------------------------------------------------------
//
TBool CTeleRecoverySystem::IsSIMOk()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::IsSIMOk()");
    
    TInt value = KErrNotFound;
    TBool simOk = EFalse;

    if ( iSimAvailable != ESimUsable )
        {
        __PHONELOG( 
                EBasic, 
                EPhoneUIUtils, 
                "CTeleRecoverySystem::IsSIMOk Ret 0");
        // SIM is not yet available.
        return EFalse;
        }

    // Get current status.
    if ( iSimStatusProperty.Handle() )
        {        
        iSimStatusProperty.Get( value );
        }
        
    simOk = ( value == ESimUsable ); 

    __PHONELOG1( 
            EBasic, 
            EPhoneUIUtils, 
            "CTeleRecoverySystem::IsSIMOk Ret: %d ",  (TInt) simOk);
    
    return simOk;
    }

// -----------------------------------------------------------------------------
// CTeleRecoverySystem::CheckSIMAvailable
// 
// -----------------------------------------------------------------------------
//
TInt CTeleRecoverySystem::CheckSIMAvailable()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CTeleRecoverySystem::CheckSIMAvailable()");

    // Check current SIM present state.
    if ( iSimStatusProperty.Handle() )
        {
        iSimStatusProperty.Get( iSimAvailable );

        __PHONELOG1( 
                EBasic, 
                EPhoneUIUtils, 
                "CTeleRecoverySystem::CheckSIMAvailable %d ",  iSimAvailable );

        if ( iSimAvailable != ESimUsable )

            {
            // Since SIM is not yet present, wait for availability.
            iRequest = EPhoneSIMStatus;
            }
        else
            {
            // SIM is now available, start listening for SIM status.
            iRequest = EPhoneSIMStatus;

            __PHONELOG1( 
                    EBasic, 
                    EPhoneUIUtils, 
                    "CTeleRecoverySystem::CheckSIMAvailable new rq %d ",  (TInt) iRequest );

            
            if ( iSimStatusProperty.Handle() )
                {        
                TInt value = ESimStatusUninitialized;
                iSimStatusProperty.Get( value );
                iPrecondSimOk = ( value == ESimUsable );
                }
            else
                {
                return KErrBadHandle;
                }            
            }      
        IssueRequest();
        }
    else
        {
        return KErrBadHandle;
        }    
    return KErrNone;
    }
//  End of File
