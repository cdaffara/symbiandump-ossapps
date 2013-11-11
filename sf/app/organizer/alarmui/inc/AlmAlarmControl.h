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



#ifndef ALMALARMCONTROL_H
#define ALMALARMCONTROL_H


// INCLUDES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <uikon/eiknotifyalert.h>
#endif
#include "pim_trace.h"
#include "PropertyObserver.h"
#include "AlmAlertVariant.hrh"
#include "alarmalertobserver.h"

#include <akndialogcontroller.h>
#include <AlarmObserver.h>

// #include <missedalarmstore.h>

// FORWARD DECLARATIONS
class CAknAlarmService;
class CAlarmUtils;
class AlarmAlertWidget;

// CLASS DECLARATION

/**
*  Alarm UI
*  This class handles displaying the alarm note and playing alarm sound
*
*  @lib AknAlarmService.lib
*  @since 1.0
*/
NONSHARABLE_CLASS( CAlmAlarmControl ) : public CCoeControl,
                                        public MEikServAlarm,
                                        public MPropertyChangeHandler,
                                        public MNotifierDialogObserver,
                                        public AlarmAlertObserver
{
 public: // Constructors and destructor
    /**
     * C++ default constructor.
     */
    CAlmAlarmControl();

    /**
     * Destructor.
     */
    ~CAlmAlarmControl();

    void ConstructL(CAknAlarmService* aSupervisor);

 public:  // New functions
    /**
    * Stops an active alarm.
    * Has the same effect as pressing the power key.
    * I.e. if the device was set off the startup query won't be shown.
    * @since S60 3.2
    **/
    void ExternalStopAlarm();

    /**
    * Snoozes an active alarm.
    * @since S60 3.2
    **/
    void ExternalSnoozeAlarm();

    /**
    * Handles auto-snooze operation.
    * @since S60 3.2
    */
    void DoAutoSnooze();

    /**
    * Query user whether power should be switched on.
    * @since S60 3.2
    */
    void AskWakeupPhoneL();

    /**
    * Show the snooze information note.
    * @since 3.1
    **/
    void ShowSnoozeInfoNoteL();

	/**
    * Return if alarm can be snoozed.
    * @since 5.0
    **/
    TBool CanSnooze();
 private:  // New functions
    /**
     * Alarm UI Panic.
     * Only called after an irreversible error.
     * @param aReason One of the panic reasons defined in TAlarmUIPanicReason (combined with TAlarmUIFunctionInfo).
     **/
    void Panic(const TUint aReason) const;

    /**
     * Set internal state.
     * @since 2.0
     * @param aState New internal state
     */
    void SetState(TInt aState);

    /**
     * Handle custom state transitions.
     * @since 3.2
     */
    void HandleStateChange(TInt aOldState);

    /**
     * Get internal state.
     * @since 2.0
     * @param aState State to compare.
     * @return ETrue if aState matches the current internal state
     */
    TBool IsState(const TInt aState) const;

    /**
     * Cancel active dialog.
     * @since 1.0
     */
    void DoCancelDialog();

    /**
     * Handle interrupt request's.
     * @since 2.0
     * @param aReason One of the alarm interrupt reasons
     */
    void HandleInterruptAlarm(TInt aReason);

    /**
     * Stores the missed alarm data in the repository 
     * 
     */
    void StoreMissedAlarmDataL();

public:  // from MEikServAlarm
    /**
     * From MEikServAlarm.
     * @since 1.0
     */
    void Release();

    /**
     * From MEikServAlarm.
     * @since 1.0
     */
    void ShowAlarm();

    /**
     * From MEikServAlarm.
     * @since 1.0
     */
    void HideAlarm();

    /**
     * From MEikServAlarm.
     * @since 1.0
     * Returns state of the alarm server.
     */
    TInt CurrentServerState() const;

    /**
     * From MEikServAlarm.
     * No implementation.
     * @since 1.0
     */
    void UpdateSoundPauseTimeInterval(TInt aMinutes);

    /**
     * From MEikServAlarm.
     * Updates the state of the alarm server.
     * @since 1.0
     * @param aNewAlarmServerState New state of the alarm server
     */
    void UpdateForAlarmServerState(TInt aNewAlarmServerState);

    /**
     * From MEikServAlarm.
     * Updates active alarm information.
     * @since 1.0
     * @param aAlarm Alarm info
     * @param aOwner Owner of the alarm. Ignored.
     */
    void UpdateAlarmInfo(const TASShdAlarm& aAlarm, const TFullName& aOwner);

    /**
     * From MEikServAlarm.
     * No implementation.
     * @since 1.0
     * @param aAlarmName Ignored
     */
    void StartPlayAlarmL(const TDesC& aAlarmName);

    /**
     * From MEikServAlarm.
     * No implementation.
     * @since 1.0
     */
    void StopPlayAlarm();
    
     /**
     * Handles the Missed Alarms Notifications from cenrep
     *
     * @since S60 5.0
     * @param aCount gets the missed alarm count
     */
    void HandleAlmInfoCRChangeL(TUint32 aCount);
    
     /**
     * Sets if StopAndExit to be sent to CFW/calendar
     * @since S60 5.0
     * @param aStopFromContext sets the missed alarm flag
     */
    void SetStopFromContext(TBool aStopFromContext);
    
    /**
     * Gets if StopAndExit to be sent to CFW/calendar
     * @since S60 5.0
     * @param None
     */
    TBool IsStopFromContext();
    
public: // From AlarmAlertObserver
    
    void alertCompleted(AlarmCommand command);
    void alertDisplayed(AlarmCommand command);
    void alertCancelled(AlarmCommand command);

 private:  // From MPropertyChangeHandler
    /**
     * This is a callback function which is called when a property is changed.
     *
     * @param aCategory  UID of the category.
     * @param aKey       changed key
     * @param aValue     The new value.  See enumerations in PS/CR header files.
     *
     * Note! references aKey and aValue are only valid while executing
     * HandlePropertyChange(). After that the data in which they refer can change.
     **/
    void HandlePropertyChange(const TUid aCategory, const TUint aKey, const TInt aValue);

 private:  // from MNotifierDialogObserver
     /**
      * From MNotifierDialogObserver.
      * No implementation.
      * @since 1.0
      * @param aNoteId   Ignored
      * @param aCommand  Ignored
      */
     void NoteCompleted(TInt aNoteId, TInt aCommand);

     /**
      * From MNotifierDialogObserver
      * @since 1.0
      * @param aPriority
      * @return  ETrue, if we can process this request (based on the priority).
      */
     TBool DisplayDialogL(TInt aPriority);

     /**
      * From MNotifierDialogObserver
      * @since 1.0
      * @param aPriority
      * @return  ETrue, if we can process this request (based on the priority).
      */
     TBool CancelDialog(TInt aPriority);

 private:  // from CCoeControl
    /**
     * From CCoeControl.
     * Handles key events.
     * @since 1.0
     * @param aKeyEvent Key event information
     * @param aType Key event type
     * @return EKeyWasConsumed or EKeyWasNotConsumed.
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

 private:  // data
    /**
    * To observe "hide alarm" command (powerkey press) from SysAp.
    * Own.
    */
    CPropertyObserver* iPropertyHideAlarm;

    /**
    * To observe system state changes.
    * Own.
    */
    CPropertyObserver* iPropertySWStateValue;

    /**
    * To observe alarm ui stop command.
    * Own.
    */
    CPropertyObserver* iPropertyStopAlarm;

    /**
    * To observe alarm ui snooze command.
    * Own.
    */
    CPropertyObserver* iPropertySnoozeAlarm;

    /**
    * To observe the backlight setting PS Key.
    * Own.
    */
	CPropertyObserver* iPropertyBacklight;
    // resource file
    TInt iResOffset;

    // Id of an active global note. KErrNotFound when not active.
    TInt iGlobalNoteId;

    // Current state of the alarm server. Not used in AlarmUI.
    TInt iCurrentServerState;

    TInt iState;  // Holds the current internal state
    enum  // AlmAlert internal states
    {
        EStateIdle = 1,               // no pending actions
        EStateWaitingShowRequest,     // alarm info accuired...waiting for request to show the alarm
        EStateWaitingDisplayRequest,  // alarm ready and pushed to global note queue...waiting for request to display the dialog
        EStateWaitingInput,           // waiting for user input...auto-snooze timer running
        EStateBeforeAskingWakeup,     // used to allow allow AskWakeupPhoneL() to cancel the dialog...see CancelDialog()...
        EStateAskingWakeup,           // only for clock alarms...when phone is off or in charging state...after user selects stop...asking to turn the phone on
        EStateAfterInput,             // Alarm already handled...Waiting for HideAlarm() or UpdateAlarmInfo()
        EStateShowingSnoozeInfo       // Showing (or about to show) the snooze info note.  This state maybe overridden with EStateWaitingShowRequest by an expiring alarm during the info note.  The new alarm is shown as soon as the info note has been handled.
    };

    enum  // Alarm interrupt reasons
    {
        EReasonKSysApHideAlarm = 1  // System request (power button)
    };

    // Set to ETrue only if device is in "alarm" or "charging" mode and an alarm was stopped.
    TBool iAskWakeup;

    // Only used to delay showing any alarms while showing the snooze info note.
    TBool iSnoozeInfoNoteActive;

    /**
    * An alarm utility class.
    * Own.
    */
    CAlarmUtils* iAlarmUtils;

    // To send Stop/StopAndExit to CFW to exit the opened calendar view.
    TBool iStopFromContextFw;
};

#endif  // ALMALARMCONTROL_H


// End of File
