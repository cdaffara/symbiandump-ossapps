/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   An alarm UI class, CAlmAlarmControl.
*                This class takes care of displaying the alarm note and
*                handling user input.
*
*/



// INCLUDE FILES
#include "AlmAlarmControl.h"
#include "pim_trace.h"
#include "alarmutils.h"
// #include <secondarydisplay/alarmuiSecondaryDisplay.h>
#include <bautils.h>
#include <StringLoader.h>
#include <pathinfo.h>
#include <featmgr.h>
#include <aknnotewrappers.h>
#include <coreapplicationuisdomainpskeys.h>
// #include <alarmuidomainpskeys.h>
#include <data_caging_path_literals.hrh>

#include <AlmAlert.rsg>
#include "AlmAlert.hrh"
#include "AlmAlert.pan"

#include <aknnotewrappers.h>
#include <AknMediatorFacade.h>
#include "alarmalertwrapper.h" 

// #include <missedalarm.h>

_LIT( KResourceFile, "AlmAlert.rsc" );

const TInt KAlarmPriority( 3095 );  // Priority for global note queue

const TUint KAlarmAutoShutdown( 60000000 );  // 60 s
const TUint KShutdownTime( 1500000 ); // 1.5 s

// ==========================================================
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAlmAlarmControl::Panic(const TUint aReason) const
{
    TRACE_ENTRY_POINT;
    _LIT( KPanicCategory, "AlarmUI-AlmAlert.pan" );
    User::Panic( KPanicCategory, aReason );
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAlmAlarmControl::CAlmAlarmControl() :
    iGlobalNoteId( KErrNotFound ),
    iState( EStateIdle )
{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CAlmAlarmControl::~CAlmAlarmControl()
{
    TRACE_ENTRY_POINT;
    this->HideAlarm();

    delete iAlarmUtils;
    iAlarmUtils = NULL;

    iEikonEnv->EikAppUi()->RemoveFromStack( this );
    iEikonEnv->DeleteResourceFile( iResOffset );

    delete iPropertyHideAlarm;
    iPropertyHideAlarm = NULL;

    delete iPropertySWStateValue;
    iPropertySWStateValue = NULL;

    delete iPropertyStopAlarm;
    iPropertyStopAlarm = NULL;

    delete iPropertySnoozeAlarm;
    iPropertySnoozeAlarm = NULL;
    TRACE_EXIT_POINT;
}

// --------------------------------------------------------
//
// ---------------------------------------------------------
//
void CAlmAlarmControl::SetState(TInt aState)
{
    TRACE_ENTRY_POINT;
    const TInt oldState( iState );
    iState = aState;
    // handle state transitions
    HandleStateChange( oldState );
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// handle custom state transitions
// ---------------------------------------------------------
//
void CAlmAlarmControl::HandleStateChange(TInt aOldState)
{
    TRACE_ENTRY_POINT;

    // stop observers when exiting from EStateWaitingInput state
    if( aOldState == EStateWaitingInput
        && iState != EStateWaitingInput )
    {
        // stop the context/sensor observer.
        PIM_TRAPD_ASSERT( iAlarmUtils->StopCFObserverL(); )

        // stop waiting for "end call" command from accessories
        iAlarmUtils->StopAccessoryObserver();
    }

    switch( iState )
    {
        case EStateIdle:
        {
            iAlarmUtils->StopInactivityResetTimer();

            // was the device started for alarm?
            if( iAlarmUtils->IsDeviceInAlarmState() )
            {
                /**
                * Show the wakeup query if any of the alarms was stopped (by user
                * pressing the stop key) and there is no more alarms in the queue.
                */
                if( iAskWakeup && !iAlarmUtils->HasExpiredAlarmsWaiting() )
                {
                    SetState( EStateBeforeAskingWakeup );
                    iAlarmUtils->AsyncShowWakeupQuery();
                }
                else
                {
                    /**
                    * Prepare to shutdown if wakeup query is not needed.
                    * (also works as a fallback if we are waiting for more
                    * alarms to be notified (iAlarmUtils->HasExpiredAlarmsWaiting()==ETrue),
                    * but alarm server queue changes suddenly)
                    */
                    iAlarmUtils->StartShutdownTimer( KShutdownTime );
                }
            }
            else
            {
                /**
                * block all input for 0.5 seconds
                * (try to prevent accidental key presses from going through the key or security lock)
                */
                iAlarmUtils->StartKeyBlocker();

                // reset the key guard
                //iAlarmUtils->SetKeyGuard( ETrue );
            }

            break;
        }

        case EStateWaitingInput:
        {
            // make sure the screen saver, key or security lock, etc. won't active during the alarm
            iAlarmUtils->StartInactivityResetTimer();

            // start waiting for "end call" command from accessories
            iAlarmUtils->StartAccessoryObserver();

            // publish new alarm context value and wait for any actions
            PIM_TRAPD_ASSERT( iAlarmUtils->StartCFObserverL(); )
            break;
        }

        default:
            // fall through
            break;
    }
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CAlmAlarmControl::IsState(const TInt aState) const
{
    TRACE_ENTRY_POINT;
    TBool retVal(iState == aState);
    TRACE_EXIT_POINT;
    return retVal;
}

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CAlmAlarmControl::DoAutoSnooze()
    {
    TRACE_ENTRY_POINT;
    iAlarmUtils->CancelAutoSnooze();

    // Allow auto-snooze only while waiting user input
    if( IsState( EStateWaitingInput ) )
        {
        SetState( EStateAfterInput );       
        iAlarmUtils->TryToSnoozeActiveAlarm();
        DoCancelDialog();        
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Dismiss the alarm notification.
// ---------------------------------------------------------
//
void CAlmAlarmControl::DoCancelDialog()
{
    TRACE_ENTRY_POINT;
    /*if( iGlobalNoteId != KErrNotFound )
    {
        // iAlarmUtils->NotifierDialogController()->SetNoteObserver( this );
        // iAlarmUtils->NotifierDialogController()->CancelNote( iGlobalNoteId );
        iAlarmUtils->NotifierDialogController()->dismissAlarmAlert();
    }*/
    iAlarmUtils->NotifierDialogController()->dismissAlarmAlert();
    iAlarmUtils->StopAlarmSound();
    iAlarmUtils->CancelAutoSnooze();
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// Show the wake-up query
// ---------------------------------------------------------
//
void CAlmAlarmControl::AskWakeupPhoneL()
{
    TRACE_ENTRY_POINT;
    iAskWakeup = EFalse;
    ASSERT( IsState( EStateBeforeAskingWakeup ) );
    if( IsState( EStateBeforeAskingWakeup ) )
    {
        // HBufC* label = NULL;
        // TBuf<1> time;
        // iAlarmUtils->GetWakeupLabelL( label );
        // CleanupStack::PushL( label );

/*		if(!IsVisible())
		{
			MakeVisible(ETrue);
		}*/
        // iAlarmUtils->NotifierDialogController()->SetNoteObserver( this );
        // iGlobalNoteId = iAlarmUtils->NotifierDialogController()->DisplayAlarmL( EAskWakeUp, *label, time/*not used*/ );
        SAlarmInfo* alarmInfo = iAlarmUtils->GetAlarmInfo();
		iAlarmUtils->NotifierDialogController()->showAlarmAlert( alarmInfo );

        // CleanupStack::PopAndDestroy( label );

        // shutdown automatically if user doesn't react within one minute
        iAlarmUtils->StartShutdownTimer( KAlarmAutoShutdown );
    }
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// Handle alarm interrupt commands (power key)
// ---------------------------------------------------------
//
void CAlmAlarmControl::HandleInterruptAlarm(TInt aReason)
{
    TRACE_ENTRY_POINT;
    switch( aReason )
    {
        case EReasonKSysApHideAlarm:  // stop
        {
            switch( iState )
            {
                case EStateIdle:
                {
                    // ignore
                }
                break;

                case EStateWaitingShowRequest:  // stop -> EStateIdle
                case EStateWaitingDisplayRequest:  // stop -> hide -> EStateIdle
                case EStateWaitingInput:  // stop -> hide -> EStateIdle
                {
                    iAskWakeup = EFalse; // don't show the wakeup query!
                    SetState( EStateIdle );
                    // Stop alarm -> HideAlarm() -> CancelNote()
                    iAlarmUtils->DoStopAlarm();  // stop
                    DoCancelDialog();
                }
                break;

                case EStateBeforeAskingWakeup:  // hide and shutdown
                case EStateAskingWakeup:  // hide and shutdown
                {
                    iAskWakeup = EFalse;
                    iAlarmUtils->CancelAsynchRequest();
                    SetState( EStateIdle );
                    DoCancelDialog();
                }
                break;

                case EStateAfterInput:
                case EStateShowingSnoozeInfo:
                {
                    SetState( EStateIdle );
                }
                break;

                default:
                {
                    // panic - unknown state!
                    Panic( EAlarmUIUnknownState | EAlarmUIHandleInterrupt );
                }
                break;
            }
            break;
        }

        default:
        {
            // Unknown reason
            Panic( EAlarmUIUnknownReason | EAlarmUIHandleInterrupt );
        }
        break;
    }
    TRACE_EXIT_POINT;
}


// ---------------------------------------------------------
// Stores the missed alarm data in the repository  
// ---------------------------------------------------------
//
void CAlmAlarmControl::StoreMissedAlarmDataL()
    {
	//Get the Calendar instance values and         	  
	//Store it in the missed alarm repository
	/*
	RPointerArray<CMissedAlarm> missedAlarmList;
	TCalLocalUid localUid = iAlarmUtils->AlarmData().iLocalUid;
	TCalTime instanceTime = iAlarmUtils->AlarmData().iInstanceTime;
	TTime startTime = instanceTime.TimeLocalL();
	CMissedAlarm* missedAlarm = CMissedAlarm::NewL( localUid, startTime,
	                                                     iAlarmUtils->AlarmData().iCalFileName );
	CleanupStack::PushL( missedAlarm );
	missedAlarmList.AppendL(missedAlarm);

	CMissedAlarmStore* missedAlarmStore = CMissedAlarmStore::NewLC();
	missedAlarmStore->AddL(missedAlarmList);
		
	CleanupStack::PopAndDestroy( missedAlarmStore );
	CleanupStack::Pop( missedAlarm );
	missedAlarmList.ResetAndDestroy();
	*/
    }
// ===========================================================
// ================ INHERITED FUNCTIONS ======================

// ---------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------
//
void CAlmAlarmControl::ConstructL(CAknAlarmService* aSupervisor)
{
    TRACE_ENTRY_POINT;
  
  TFileName fileName;
	// Get the complate path of the DLL from where it is currently loaded
	Dll::FileName( fileName );
	
	TFileName resFile;
	
	// Append the Drive letters ex., Z: or C:
	resFile.Append(fileName.Mid(0,2));
	resFile.Append(KDC_RESOURCE_FILES_DIR);
    resFile.Append(KResourceFile);
    
    BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), resFile );
    iResOffset = iEikonEnv->AddResourceFileL( resFile );
    

    /*iEikonEnv->EikAppUi()->AddToStackL(
        this,
        ECoeStackPriorityAlert,
        ECoeStackFlagRefusesFocus );
*/
	// MakeVisible(EFalse);
	SetStopFromContext(EFalse);
    iAlarmUtils = CAlarmUtils::NewL( this, aSupervisor );

    // observe "hide alarm" commands (i.e. powerkey presses)
    iPropertyHideAlarm = CPropertyObserver::NewL( *this, KPSUidCoreApplicationUIs, KCoreAppUIsHideAlarm );

    // observe system state changes
    iPropertySWStateValue = CPropertyObserver::NewL( *this, KPSUidStartup, KPSGlobalSystemState );
    //For Plasma accessory handling
    // _LIT_SECURITY_POLICY_PASS( KGeneralReadPolicy );
	// _LIT_SECURITY_POLICY_C1( KProtectedWritePolicy, ECapabilityWriteDeviceData );

	/*
	RProperty::Define( KPSUidAlarmExtCntl, KAlarmStopKey, RProperty::EInt, KGeneralReadPolicy, KProtectedWritePolicy );
    RProperty::Define( KPSUidAlarmExtCntl, KAlarmSnoozeKey, RProperty::EInt, KGeneralReadPolicy, KProtectedWritePolicy );

    //observe Plasma accessory stop key changes
    iPropertyStopAlarm = CPropertyObserver::NewL(*this, KPSUidAlarmExtCntl, KAlarmStopKey);

    //observe Plasma accessory snooze key changes
    iPropertySnoozeAlarm = CPropertyObserver::NewL(*this, KPSUidAlarmExtCntl, KAlarmSnoozeKey);
	*/
    //observe SysAp backlight setting PS Key
	iPropertyBacklight = CPropertyObserver::NewL(*this, KPSUidCoreApplicationUIs, KLightsAlarmLightActive);
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MEikServAlarm (MAlarmObserver)
// ---------------------------------------------------------
//
void CAlmAlarmControl::Release()
{
    TRACE_ENTRY_POINT;
    delete this;
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MEikServAlarm
// NOTE!!! This function is also called from HandlePropertyChange()
// ---------------------------------------------------------
//
void CAlmAlarmControl::ShowAlarm()
{
    TRACE_ENTRY_POINT;
    switch( iState )
    {
        case EStateWaitingShowRequest:
        {
            /**
            * wait until device startup is ready,
            * snooze info note is not being displayed and
            * delay timer has finished.
            */
            if( iAlarmUtils->IsPhoneStartupReady() &&
                !iSnoozeInfoNoteActive &&
                !iAlarmUtils->AlarmDelayTimerActive() )
            {
                // State MUST be set before requesting the note from avkon!
                // DisplayAlarmL() will call DisplayDialogL() directly!
                SetState( EStateWaitingDisplayRequest );

                // Only call this within ShowAlarm to prevent updating data too early/late.
                // (e.g. updating snooze time before showing the snooze info note)
                TRAPD( err, iAlarmUtils->InitialiseAlarmDataL(); )

                // Get the alarm information
                SAlarmInfo* alarmInfo = iAlarmUtils->GetAlarmInfo();
                // Get the ringing type
                alarmInfo->iIsSilent = iAlarmUtils->IsRingingTypeSilent() || iAlarmUtils->IsOffTone();
                // Check if the alarm can be snoozed
                alarmInfo->iCanSnooze = iAlarmUtils->CanSnooze();
            
                // cleanup (release global data)
                iAlarmUtils->UninitialiseAlarmData();
                /*if(!IsVisible())
                {
                    MakeVisible(ETrue);
                }
                iAlarmUtils->NotifierDialogController()->SetNoteObserver( this );

                // setup CBA for the global note
                TUint cba(0);  // See AknDialogController.h
                // alarm type - no silence key for clock alarms, unknown alarms are handled as calendar alarms
                cba |= iAlarmUtils->IsClockAlarm() ? EClockAlarm | ENoSilence : ECalendarAlarm;
                // disable silence key when ringing type is set to "silent" or if alarm sounds is set off
                cba |= iAlarmUtils->IsRingingTypeSilent() || iAlarmUtils->IsOffTone() ? ENoSilence : 0;
                // disable silence and snooze key if alarm can't be snoozed anymore
                cba |= iAlarmUtils->CanSnooze() ? 0 : EHideSnooze | ENoSilence;
                // show "Open" MSK for calendar alarms if the security lock is not active
                cba |= iAlarmUtils->IsCalendarAlarm() && !iAlarmUtils->IsSecurityLockActive() ? EMskOpen : 0;

                // request alarm dialog
                TBuf<1> time;
                TRAP( err, iGlobalNoteId = iAlarmUtils->NotifierDialogController()->DisplayAlarmL( cba, *text, timenot used ) );
                delete text; // only delete if GetAlarmLabelL call was successfull
                ASSERT( !err ); */     
                iAlarmUtils->NotifierDialogController()->showAlarmAlert(alarmInfo);
                //iAlarmUtils->StartAutoSnoozeTimer();
                //iAlarmUtils->PlayAlarmSound();
                //SetState( EStateWaitingInput );

                if( err ) // failed to fetch alarm data or show the notification
                {
                    // this happens if user has deleted calendar entry with a snoozed alarm or removed alarm from a snoozed calendar entry.
                    // (or if the DisplayAlarmL call fails)
                    // -> stop and ignore the alarm
                    HandleInterruptAlarm( EReasonKSysApHideAlarm );
                }
                delete alarmInfo->iLocation;
                delete alarmInfo->iSubject;
            }
        }
        break;

        default:
        {
            // All other states are ignored
        }
    }
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MEikServAlarm (MAlarmObserver)
// !!! REMEMBER !!! - This function is only called AFTER
// the alarm is snoozed or stopped
// ---------------------------------------------------------
//
void CAlmAlarmControl::HideAlarm()
{
    TRACE_ENTRY_POINT;
    switch( iState )
    {
        case EStateIdle:
        case EStateBeforeAskingWakeup:
        case EStateAskingWakeup:
        case EStateShowingSnoozeInfo:
        {
            // fall through
        }
        break;

        case EStateWaitingShowRequest:
        case EStateWaitingDisplayRequest:
        case EStateWaitingInput:
        case EStateAfterInput:
        {
            SetState( EStateIdle );
            DoCancelDialog();
        }
        break;

        default:
        {
            // panic - unknown state!
            Panic( EAlarmUIUnknownState | EAlarmUIHideAlarm );
        }
    }
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MEikServAlarm (MAlarmObserver)
// ---------------------------------------------------------
//
TInt CAlmAlarmControl::CurrentServerState() const
{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iCurrentServerState;
}

// ---------------------------------------------------------
// From MEikServAlarm (MAlarmObserver)
// ---------------------------------------------------------
//
void CAlmAlarmControl::UpdateSoundPauseTimeInterval(TInt /*aMinutes*/)
{
    TRACE_ENTRY_POINT;
    // No implementation
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MEikServAlarm (MAlarmObserver)
// ---------------------------------------------------------
//
void CAlmAlarmControl::UpdateForAlarmServerState(TInt aNewAlarmServerState)
{
    TRACE_ENTRY_POINT;
    if( iCurrentServerState != aNewAlarmServerState )
    {
        iCurrentServerState = aNewAlarmServerState;
    }
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MEikServAlarm (MAlarmObserver)
// ---------------------------------------------------------
//
void CAlmAlarmControl::UpdateAlarmInfo(const TASShdAlarm& aAlarm, const TFullName& /*aOwner*/)
{
    TRACE_ENTRY_POINT;
    // cancel timers
    iAlarmUtils->CancelAutoSnooze();
    iAlarmUtils->CancelShutdown();
    iAlarmUtils->StartAlarmDelayTimer();

    // remember to store the alarm object
    iAlarmUtils->AlarmData().iAlarm = aAlarm;

    switch( iState )
    {
        case EStateIdle:
        case EStateAfterInput:
        case EStateShowingSnoozeInfo:
        {
            /**
            * ignore and snooze other than wakeup alarms when phone is not in "normal" state
            * (special handling - don't increment the snooze count)
            */
            if( !iAlarmUtils->IsDeviceInNormalState() &&
                !iAlarmUtils->IsWakeupAlarm() )
            {
                iAlarmUtils->DoSnooze();
            }
            else
            {
                SetState( EStateWaitingShowRequest );
            }
        }
        break;

        case EStateWaitingShowRequest:
        case EStateWaitingDisplayRequest:
        case EStateWaitingInput:
        {
            // Alarm server sent the alarm back to queue...just cancel the dialog and continue.
            SetState( EStateIdle );
            DoCancelDialog();
            SetState( EStateWaitingShowRequest );
        }
        break;

        case EStateBeforeAskingWakeup:
        case EStateAskingWakeup:
        {
            /**
            * Just snooze the alarm for 1 minute (without changing the counter) and
            * let the user (or shutdown timer) handle the wakeup dialog...
            */
            iAlarmUtils->AlarmData().iSnoozeTime = 1;
            iAlarmUtils->DoSnooze();
        }
        break;

        default:
        {
            // All other states are invalid!
            Panic( EAlarmUIInvalidState | EAlarmUIUpdateAlarmInfo );
        }
    }
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MEikServAlarm (MAlarmObserver)
// ---------------------------------------------------------
//
void CAlmAlarmControl::StartPlayAlarmL(const TDesC& /*aAlarmName*/)
{
    TRACE_ENTRY_POINT;
    // No implementation.
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MEikServAlarm (MAlarmObserver)
// ---------------------------------------------------------
//
void CAlmAlarmControl::StopPlayAlarm()
{
    TRACE_ENTRY_POINT;
    // No implementation.
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MPropertyChangeHandler
// ---------------------------------------------------------
//
void CAlmAlarmControl::HandlePropertyChange(const TUid aCategory, const TUint aKey, const TInt aValue)
{
    TRACE_ENTRY_POINT;
    // Notified when user presses the power key...
    if( aCategory == KPSUidCoreApplicationUIs && aKey == KCoreAppUIsHideAlarm && aValue == ECoreAppUIsHideAlarm )
    {
        /**
        * Does the device have a dedicated power key?
        */
        TBool noPowerKey( EFalse ); // is end key used as a power key?
        TRAPD( err, FeatureManager::InitializeLibL(); )
        ASSERT( !err );

        if( !err )
        {
            noPowerKey = FeatureManager::FeatureSupported( KFeatureIdNoPowerkey );
            FeatureManager::UnInitializeLib();
        }

        if( noPowerKey ) // end key as power key >> snooze
        {
            // cancel the alarm
            if( iGlobalNoteId != KErrNotFound )
            {
                // iAlarmUtils->NotifierDialogController()->SetNoteObserver( this );
                // iAlarmUtils->NotifierDialogController()->CancelNote( iGlobalNoteId );
                iAlarmUtils->NotifierDialogController()->dismissAlarmAlert();
            }
        }
        else // stop
        {
            HandleInterruptAlarm( EReasonKSysApHideAlarm );
        }
    }
    // phone state changed
    else if( aCategory == KPSUidStartup && aKey == KPSGlobalSystemState )
    {
        iAlarmUtils->SetDeviceState( static_cast<TPSGlobalSystemState>(aValue) );
        // This is just to make sure the ShowAlarm gets always called because
        // ShowAlarm() is ignored if phone is not in some of it's final states.
        if( IsState( EStateWaitingShowRequest ) )
        {
            ShowAlarm();
        }
    }

	/*
    //For plasma support
    else if( aCategory == KPSUidAlarmExtCntl && aKey == KAlarmStopKey && aValue == EAlarmUIStopAlarm )
    {
        //Set the P&S to an uninitialized value
    	TAlarmUIStopAlarm extStopAlarm = EAlarmUIStopAlarmUninitialized;
        PIM_ASSERT( RProperty::Set( KPSUidAlarmExtCntl, KAlarmStopKey, static_cast<TInt>(extStopAlarm)); )

    	ExternalStopAlarm();
    }

    else if( aCategory == KPSUidAlarmExtCntl && aKey == KAlarmSnoozeKey  && aValue == EAlarmUISnoozeAlarm)
    {
        //Set the P&S to an uninitialized value
    	TAlarmUISnoozeAlarm extSnoozeAlarm = EAlarmUISnoozeAlarmUninitialized;
    	PIM_ASSERT( RProperty::Set( KPSUidAlarmExtCntl, KAlarmSnoozeKey, static_cast<TInt>(extSnoozeAlarm)); )

    	ExternalSnoozeAlarm();
    }
	*/
	else if( aCategory == KPSUidCoreApplicationUIs && aKey == KLightsAlarmLightActive)
	{
		if(IsState(EStateWaitingInput) && aValue ==  ELightsBlinkingUninitialized )
		{
			iAlarmUtils->SetBackLight(ETrue);
		}
	}
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MNotifierDialogObserver
// ---------------------------------------------------------
//
void CAlmAlarmControl::NoteCompleted(TInt aNoteId, TInt aCommand)
{
    TRACE_ENTRY_POINT;
    if( aNoteId != iGlobalNoteId )  // Valid note?
    {
        TRACE_EXIT_POINT;
        return;
    }

    // cancel timers
    iAlarmUtils->CancelAutoSnooze();
    iAlarmUtils->CancelShutdown();
	
	// do not silence if EAlertOpen or EAknSoftkeyEmpty is received
	if ( EAlertOpen != aCommand && EAknSoftkeyEmpty != aCommand )
		{
		iAlarmUtils->StopAlarmSound();
		}
	if(IsVisible())
	{
		MakeVisible(EFalse);
	}

    switch( iState )
    {
        /**
        * EStateWaitingInput: normal case
        * EStateWaitingDisplayRequest:
        *     This happens only if the global note got handled before we got the DisplayDialogL call.
        *     (e.g. note is dismissed if the cover is closed at the same time when alarm expires)
        **/
        case EStateWaitingInput:
        case EStateWaitingDisplayRequest:
        {
            switch( aCommand )
            {
                case EAlertStop:
                {
                    iGlobalNoteId = KErrNotFound;  // Set note as discarded/used...
                    SetState( EStateAfterInput );
                    iAskWakeup = ETrue; // ask wakeup after all the alarms are handled
                    iAlarmUtils->DoStopAlarm();  // stop
                }
                break;

                case EAlertSnooze:
                {
                    iGlobalNoteId = KErrNotFound;  // Set note as discarded/used...
                    SetState( EStateAfterInput );

                    // show the snooze info note
                    iSnoozeInfoNoteActive = ETrue;
                    SetState( EStateShowingSnoozeInfo );
                    iAlarmUtils->AsyncShowSnoozeInfoNote();
                    iAlarmUtils->TryToSnoozeActiveAlarm();
                }
                break;

                case EAlertSilence:
                {
                    iAlarmUtils->StartAutoSnoozeTimer();  // restart auto-snooze timer
                }
                break;
                
                case EAlertOpen:
                {
                    if( iAlarmUtils->IsCalendarAlarm() && !iAlarmUtils->IsSecurityLockActive() )
	                    {
                        // Do not leave if calendar launch fails. Just continue as normally
                        iAlarmUtils->StopAlarmSound();
                        // PIM_TRAPD_ASSERT( iAlarmUtils->StartCalendarL(); )
                        iAlarmUtils->StartAutoSnoozeTimer();  // restart auto-snooze timer
	                    }
                }
                break;

                case EAknSoftkeyEmpty:
                    // Ignore if there is an empty softkey. No functionality to be completed.
                    break;


                default:
                {
                    // Unknown command!
                    Panic( EAlarmUIUnknownCommand | EAlarmUINoteCompleted );
                }
                break;
            }
        }
        break;

        case EStateAskingWakeup:
        {
            iGlobalNoteId = KErrNotFound; // Set note as discarded/used...

            switch( aCommand )
            {
                case EAknSoftkeyYes:
                {
                    SetState( EStateIdle );
                    /*
                    * Going to EStateIdle when phone was started for an alarm will
                    * always start the shutdown timer. Stop the timer before requesting
                    * device startup.
                    */
                    iAlarmUtils->CancelShutdown();

                    // boot up the device
                    iAlarmUtils->DeviceStartup();
                }
                break;

                case EAknSoftkeyNo:
                {
                    // shutdown
                    SetState( EStateIdle );
                }
                break;

                default:
                {
                    // Unknown command!
                    Panic( EAlarmUIUnknownCommand | EAlarmUINoteCompleted );
                }
                break;
            }
        }
        break;

        default:
        {
            // panic - invalid state!
            Panic( EAlarmUIInvalidState | EAlarmUINoteCompleted );
        }
    }
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// From MNotifierDialogObserver
// ---------------------------------------------------------
//
TBool CAlmAlarmControl::DisplayDialogL(TInt aPriority)
{
    TRACE_ENTRY_POINT;
    // Is it our responsibility
    if( aPriority != KAlarmPriority )
    {
        TRACE_EXIT_POINT;
        return EFalse;
    }

    switch( iState )
    {
        case EStateWaitingDisplayRequest:
        {
            iAlarmUtils->StartAutoSnoozeTimer();
            iAlarmUtils->PlayAlarmSound();
            //iAlarmUtils->SetKeyGuard( EFalse );
            SetState( EStateWaitingInput );
            iAlarmUtils->StartKeyBlocker();  // block all input for 0.5 seconds
        }
        break;

        case EStateBeforeAskingWakeup:
        {
            SetState( EStateAskingWakeup );
        }
        break;

        default:
        {
            // panic - invalid state!
            Panic( EAlarmUIInvalidState | EAlarmUIDisplayDialog );
        }
    }
    TRACE_EXIT_POINT;
    return EFalse;
}

// ---------------------------------------------------------
// From MNotifierDialogObserver
// ---------------------------------------------------------
//
TBool CAlmAlarmControl::CancelDialog(TInt aPriority)
{
    TRACE_ENTRY_POINT;
    // Is it our responsibility
    if( aPriority != KAlarmPriority )
    {
        TRACE_EXIT_POINT;
        return EFalse;  // -> ignore
    }

    iGlobalNoteId = KErrNotFound;

    /**
    * Reset back to EStateIdle and snooze the alarm if active.
    */
    switch( iState )
    {
        case EStateIdle:
        case EStateShowingSnoozeInfo:
        {
            // fall through
        }
        break;

        case EStateWaitingShowRequest:
        case EStateWaitingDisplayRequest:
        case EStateWaitingInput:
        {
            SetState( EStateIdle );
            DoCancelDialog();
            iAlarmUtils->TryToSnoozeActiveAlarm();
        }
        break;

        case EStateBeforeAskingWakeup:
        case EStateAskingWakeup:
        {
            // cancel and shutdown
            iAlarmUtils->CancelAsynchRequest();
            SetState( EStateIdle );
            DoCancelDialog(); // make sure the dialog gets cancelled
        }
        break;

        case EStateAfterInput:
        {
            SetState( EStateIdle );
        }
        break;

        default:
        {
            // panic - invalid state!
            Panic( EAlarmUIUnknownState | EAlarmUICancelDialog );
        }
    }

    TRACE_EXIT_POINT;
    return EFalse;
}

// ---------------------------------------------------------
// From CCoeControl
// ---------------------------------------------------------
//
TKeyResponse CAlmAlarmControl::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TRACE_ENTRY_POINT;
    // Consume all events except softkeys + power and end call key
    // For keycodes see AVKON documentation: Series_60_AVKON_Key_Codes_Modifications_UI_B.doc

    // consume all key presses while key block timer is active
    if( iAlarmUtils->KeyBlockActive() )
    {
        TRACE_EXIT_POINT;
        return EKeyWasConsumed;
    }

    // don't consume any keys while in idle state or showing snooze info note
    if( IsState( EStateIdle ) || iSnoozeInfoNoteActive )
    {
        TRACE_EXIT_POINT;
        return EKeyWasNotConsumed;
    }

    if( (IsState( EStateWaitingInput ) ||
         IsState( EStateAskingWakeup ) )
        && aType == EEventKey )
    {
        switch( aKeyEvent.iCode )
        {
            case EKeyCBA1:     // (scan code EStdKeyDevice0)
            case EKeyCBA2:     // (scan code EStdKeyDevice1)
            case EKeyPowerOff: // (scan code EStdKeyDevice2)
            {
                // no need to let the event through if we don't have an active alarm note
                if( iGlobalNoteId != KErrNotFound )
                {
                    TRACE_EXIT_POINT;
                    return EKeyWasNotConsumed;
                }
                else
                {
                    TRACE_EXIT_POINT;
                    return EKeyWasConsumed;
                }
            }

            // MSK support
            case EKeyOK: // (scan code EStdKeyDevice3)
            {
                // Allow MSK for the "wakeup" query.
                if( IsState( EStateAskingWakeup ) )
                {
                    TRACE_EXIT_POINT;
                    return EKeyWasNotConsumed;
                }
                else
                {
                    if( IsState( EStateWaitingInput ) && 
                        iAlarmUtils->IsCalendarAlarm() && 
                        !iAlarmUtils->IsSecurityLockActive() )
                    {
                        TRACE_EXIT_POINT;
                        return EKeyWasNotConsumed;
                    }

                    TRACE_EXIT_POINT;
                    return EKeyWasConsumed;
                }
            }

            case EKeyPhoneEnd: // (scan code EStdKeyNo)
            {
                // Normally pressing End Call key generates CancelDialog callback, but
                // when we have an active call we don't get the CancelDialog...
                // iAlarmUtils->NotifierDialogController()->CancelNote( iGlobalNoteId );  // Cancel alarm....will snooze...
                TRACE_EXIT_POINT;
                return EKeyWasNotConsumed;
            }

            case EKeyVolumeUp:   // (scan code EStdKeyIncVolume)
            case EKeyVolumeDown: // (scan code EStdKeyDecVolume)
            {
                ExternalSnoozeAlarm();  // snooze the alarm
                TRACE_EXIT_POINT;
                return EKeyWasConsumed;
            }

            // OK/selection key, number keys, etc..
            default:
            {
                // let the user "silence" the calendar alarm (=stop flashing & vibra) when
                // using "silent" or "beep once" ringing type.
                // !!! calendar alarm uses "silence" key if ringing type is not "silent" or "beep once"
                if( !iAlarmUtils->IsClockAlarm() && iAlarmUtils->IsRingingTypeSilent() )
                {
                    iAlarmUtils->StopAlarmSound();
                }

                // Eat all other keys...
                // To enable keys (numbers, etc.) remember to re-evaluate
                // all the switch...case Panics...
                TRACE_EXIT_POINT;
                return EKeyWasConsumed;
            }
        }
    }

    // consume all other key presses if phone startup is ready
    if( iAlarmUtils->IsPhoneStartupReady() )
    {
        TRACE_EXIT_POINT;
        return EKeyWasConsumed;
    }
    else // still booting (alarm might be waiting to show the notification while phone is still in PIN query)
    {
        TRACE_EXIT_POINT;
        return EKeyWasNotConsumed;
    }
}

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CAlmAlarmControl::ShowSnoozeInfoNoteL()
{
    TRACE_ENTRY_POINT;
/*
    //Changes for MPIN-73VCR2
   HBufC* stringHolder = NULL;
   CAknInformationNote* note = new (ELeave) CAknInformationNote();
   TAlarmSecondaryDisplayNote alarmSecDispNote = EAlarmNoNote;

   if( iAlarmUtils->AlarmData().iSnoozeTime == 1 )
    {
        stringHolder = StringLoader::LoadLC( R_EIKALARM_NOTE_SNOOZE_ONE );
	      alarmSecDispNote = EAlarmSingleMinuteSnooze;
    }
   else
    {
        stringHolder = StringLoader::LoadLC( R_EIKALARM_NOTE_SNOOZE, iAlarmUtils->AlarmData().iSnoozeTime );
        alarmSecDispNote = EAlarmMutlipleMinuteSnooze;
    }

   CleanupStack::PushL( note );
   note->PublishDialogL(alarmSecDispNote,KAlarmUINoteCategory);
   CleanupStack::Pop();

   TPtr bufPtr = stringHolder->Des();
   AknTextUtils::DisplayTextLanguageSpecificNumberConversion( bufPtr );

		//Commit data for buffer
		CAknMediatorFacade* covercl1 = AknMediatorFacade(note);
		if (covercl1) // returns null if __COVER_DISPLAY is not defined
	    {
		    covercl1->BufStream().WriteInt32L( iAlarmUtils->AlarmData().iSnoozeTime );// for coverui localisation info
		    covercl1->BufStream().CommitL(); // no more data to send so commit buf
	    }
	  note->ExecuteLD( *stringHolder );

		CleanupStack::PopAndDestroy(); //stringHolder

    // End of changes for MPIN-73VCR2

    iSnoozeInfoNoteActive = EFalse;

    // new alarm expired during the snooze info note --> show it
    if( IsState( EStateWaitingShowRequest ) )
    {
        ShowAlarm();
    }
    else
    {
        SetState( EStateIdle );
    }
	*/
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// Stops an active alarm.
// Has the same effect as pressing the power key (i.e. if
// the device was set off the startup query won't be shown)
// ---------------------------------------------------------
//
void CAlmAlarmControl::ExternalStopAlarm()
{
    TRACE_ENTRY_POINT;
    /**
    * External commands should be handled only if we are
    * really expecting some input from the user...
    */
    if( IsState( EStateWaitingInput ) )
    {
        iAlarmUtils->CancelAutoSnooze();
        iAskWakeup = EFalse; // don't show the wakeup query!
        SetState( EStateAfterInput );
        iAlarmUtils->DoStopAlarm();  // stop
        DoCancelDialog();
    }
    iAlarmUtils->SetCalendarAlarmViewer(EFalse);
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// Snoozes an active alarm.
// ---------------------------------------------------------
//
void CAlmAlarmControl::ExternalSnoozeAlarm()
{
    TRACE_ENTRY_POINT;
    /**
    * External commands should be handled only if we are
    * really expecting some input from the user...
    */
    if( IsState( EStateWaitingInput ) )
    {
        iAlarmUtils->CancelAutoSnooze();
        SetState( EStateAfterInput );

        if( iAlarmUtils->CanSnooze() )
        {
            // show the snooze info note
            iSnoozeInfoNoteActive = ETrue;
            SetState( EStateShowingSnoozeInfo );
            iAlarmUtils->AsyncShowSnoozeInfoNote();
        }
        // snooze (or stop, if max snooze count passed) the alarm and cancel the dialog
        iAlarmUtils->TryToSnoozeActiveAlarm();
        DoCancelDialog();
    }
    iAlarmUtils->SetCalendarAlarmViewer(EFalse);
    TRACE_EXIT_POINT;
}

/**
* Return if alarm can be snoozed.
* @since 5.0
**/
TBool CAlmAlarmControl::CanSnooze()
 	{
 		return iAlarmUtils->CanSnooze();
 	}

//---------------------------------------------------------
// CAlmAlarmControl::HandleAlmInfoCRChangeL
// Handles Missed Alarm Store notifications
//---------------------------------------------------------
//
void CAlmAlarmControl::HandleAlmInfoCRChangeL(TUint32 /*aCount*/)
    {
    TRACE_ENTRY_POINT; 

    TRACE_EXIT_POINT;
    }

//---------------------------------------------------------
// CAlmAlarmControl::SetStopFromContext
// Sets if Stop to be sent to calendar
//---------------------------------------------------------
 void CAlmAlarmControl::SetStopFromContext(TBool aStopFromContext)
 {
 	iStopFromContextFw = aStopFromContext;
 }

//---------------------------------------------------------
// CAlmAlarmControl::IsStopFromContext
// Gets if Stop/StopAndExit to be sent to calendar
//---------------------------------------------------------
//
 TBool CAlmAlarmControl::IsStopFromContext()
 {
 	return iStopFromContextFw;
 }
 
 void CAlmAlarmControl::alertCompleted(AlarmCommand command)
 {
     //iAlarmUtils->CancelShutdown();
     iAlarmUtils->StopAlarmSound();

     switch( iState )
         {
         /**
          * EStateWaitingInput: normal case
          * EStateWaitingDisplayRequest:
          *     This happens only if the global note got handled before we got the DisplayDialogL call.
          *     (e.g. note is dismissed if the cover is closed at the same time when alarm expires)
          **/
         case EStateWaitingInput:
         case EStateWaitingDisplayRequest:
             {
             switch( command )
                 {
                 case AlarmStop:
                     {
                     SetState( EStateAfterInput );
					 // cancel timers
					 iAlarmUtils->CancelAutoSnooze();
                     iAskWakeup = ETrue; // ask wakeup after all the alarms are handled
                     iAlarmUtils->DoStopAlarm();  // stop
                     break;
                     }
                 case AlarmSnooze:
                     {
                     // cancel timers
					 iAlarmUtils->CancelAutoSnooze();
					 SetState( EStateAfterInput );

                     SetState( EStateShowingSnoozeInfo );
                     iAlarmUtils->TryToSnoozeActiveAlarm();
                     if( IsState( EStateWaitingShowRequest ) )
                         {
                         ShowAlarm();
                         }
                     else
                         {
                         SetState( EStateIdle );
                         }
                     }
                 }
             }
         break;
         }  
     TRACE_EXIT_POINT;
 }
 
 void CAlmAlarmControl::alertDisplayed(AlarmCommand /*command*/)
 {
     TRACE_ENTRY_POINT;
     switch( iState )
     {
         case EStateWaitingDisplayRequest:
         {
             iAlarmUtils->StartAutoSnoozeTimer();
             iAlarmUtils->PlayAlarmSound();
             //iAlarmUtils->SetKeyGuard( EFalse );
             SetState( EStateWaitingInput );
             iAlarmUtils->StartKeyBlocker();  // block all input for 0.5 seconds
         }
         break;

         case EStateBeforeAskingWakeup:
         {
             SetState( EStateAskingWakeup );
         }
         break;

         default:
         {
             // panic - invalid state!
             Panic( EAlarmUIInvalidState | EAlarmUIDisplayDialog );
         }
     }
 }
 
 void CAlmAlarmControl::alertCancelled(AlarmCommand /*command*/)
 {
     iAlarmUtils->StopAlarmSound();
     iAlarmUtils->CancelAutoSnooze();
 }

// End of File
