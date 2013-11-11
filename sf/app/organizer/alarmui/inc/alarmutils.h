/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef ALARMUTILS_H
#define ALARMUTILS_H


// INCLUDES
#include "AlmAlertVariant.hrh"

#include <e32base.h>
#include <asshdalarm.h>
#include <asclisession.h>
#include <TProfileToneSettings.h>
#include <apgcli.h>
#include <startupdomainpskeys.h>
#include <starterclient.h>
#include <calentry.h>
#include <caltime.h>
#include <calprogresscallback.h>

#include "alarmcommon.h"

// FORWARD DECLARATIONS

class CCalSession;
class CCalEntryView;
class CAlmAlarmControl;
class CAlmAlarmPlayer;
class CAlmAlarmInfo;
class CAlarmRemConEventsHandler;
#ifdef RD_ALMALERT__SENSOR_SUPPORT
class CAlarmContextFwSupport;
#endif // RD_ALMALERT__SENSOR_SUPPORT
class MProfileEngine;
class CNotifierDialogController;
class CAknAlarmService;
class AlarmAlert;

// CLASS DECLARATION

/**
*  ?description
*
*  @lib AknAlarmService.lib
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CAlarmUtils ) : public CBase, public MCalProgressCallBack
    {
public: // data types
    /**
    * Alarm types.
    */
    enum TAlarmType
        {
        EAlarmTypeClock = 0,
        EAlarmTypeCalendar,
        EAlarmTypeTodo,
        EAlarmTypeOther
        };


    struct TAlarmData
        {
        // Alarm snooze time.
        TInt iSnoozeTime;

        // Currently active alarm object on the alarm server queue.
        TASShdAlarm iAlarm;

        // Name of the alarm tone file.
        TFileName iAlarmTone;

        // Name of the default alarm tone file.
        TFileName iDefaultAlarmTone;

        // Volume setting.
        TInt iVolume;

        // Repeate value for the player.
        TInt iRepeatValue;

        // Volume ramp time value for the player.
        TInt iVolumeRampTime;

        // Ringing type.
        TProfileRingingType iRingType;

        // alarm type.
        TAlarmType iAlarmType;

        // instance time (only for calendar alarms)
        TCalTime iInstanceTime;

        // local uid (only for calendar alarms)
        TCalLocalUid iLocalUid;
		
		TBufC<256> iCalFileName;

        };

public: // Constructors and destructor
    // Constructor
    static CAlarmUtils* NewL( CAlmAlarmControl* aAlarmControl,
                              CAknAlarmService* aSupervisor );

    // Destructor.
    ~CAlarmUtils();

private:
    // C++ default constructor.
    CAlarmUtils( CAlmAlarmControl* aAlarmControl,
                  CAknAlarmService* aSupervisor );

    // Symbian 2nd phase constructor
    void ConstructL();

public:
    /**
    * Plays the alarm sound.
    * @since S60 3.2
    */
    void PlayAlarmSound();

    /**
    * Stops the alarm sound.
    * @since S60 3.2
    */
    void StopAlarmSound();

    /**
    * Activates and deactivates keyguard.
    * @since S60 3.2
    * @param aActive ETrue, to activate keyguard.
    */
    void SetKeyGuard(TBool aActive);

    /**
    * Checks whether current alarm is a clock alarm.
    * @since S60 3.2
    * @return True, if alarm is a clock alarm
    */
    TBool IsClockAlarm() const;

    /**
    * Check whether current alarm is a calendar alarm.
    * @since S60 3.2
    * @return True, if alarm is a calendar alarm
    */
    TBool IsCalendarAlarm() const;

    /**
    * Checks the current ringing type from Profiles.
    * @since S60 3.2
    * @return True, if current ringing type is set to silent
    */
    TBool IsRingingTypeSilent() const;

    /**
    * Check if using the "Off" tone. (i.e. no_sound.wav or KNullDesC.)
    * @since S60 3.2
    * @return ETrue, if iAlarmName is an "off" tone.
    */
    TBool IsOffTone() const;

    /**
    * Check if the alarm sound file is a KRngMimeType file.
    * @since S60 3.2
    * @return ETrue, if the tone is a KRngMimeType file.
    */
    TBool IsRngTone();

    /**
    * @since S60 3.2
    * @return The maximum value of profile volume.
    */
    TInt MaxProfileVolume() const;

    /**
    * @since S60 3.2
    * @return Reference to the iAlarmData struct.
    **/
    CAlarmUtils::TAlarmData& AlarmData();

    /**
    * Initialise the TAlarmData struct and 
    * get alarm informations string(s).
    * @since S60 3.2
    */
    void InitialiseAlarmDataL();

    /**
    * Cleanup.
    * @since S60 3.2
    */
    void UninitialiseAlarmData();

    /**
    * Checks whether active alarm can be snoozed or not.
    * @since S60 3.2
    * @return True, if the alarm can be snoozed
    */
    TBool CanSnooze() const;

    /**
    * Check whether we are in restore mode or not.
    * @since S60 3.2
    * @return ETrue if phone is in restore mode and EFalse if not
    **/
    TBool IsPhoneInRestoreMode() const;

    /**
    * Gets text and time values for an alarm note.
    * @since S60 3.2
    * @param aText Buffer to receive the text.
    **/
    void GetAlarmLabelL(HBufC*& aText);

    /**
    * Gets the label for wakeup note.
    * @since S60 3.2
    * @param aLabel Buffer to receive the text.
    **/
    void GetWakeupLabelL(HBufC*& aLabel);

    /**
    * ?description
    * @since S60 3.2
    * @return Pointer to CNotifierDialogController.
    */
    // CNotifierDialogController* NotifierDialogController();
    AlarmAlert *NotifierDialogController();

    /**
    * Stop the active alarm and reset the snooze count.
    * @since S60 3.2
    **/
    void DoStopAlarm();

    /**
    * Access to iHasExpiredAlarmsWaiting.
    * @since S60 3.2
    * @return ETrue if there are alarms waiting to be shown.
    **/
    TBool HasExpiredAlarmsWaiting() const;

    /**
    * Snooze the active alarm.
    * @since S60 3.2
    **/
    void DoSnooze();

    /**
    * Checks if this alarm can be shown in "alarm" or in "charging" state.
    * @since S60 3.2
    * @return ETrue, if alarm can be shown.
    */
    TBool IsWakeupAlarm() const;

    /**
    * Checks if the device is in "alarm" or in "charging" state.
    * @since S60 3.2
    * @return ETrue, if device is in "alarm" or in "charging" state.
    */
    TBool IsDeviceInAlarmState() const;

    /**
    * Checks if the device is in any of the normal states.
    * @since S60 3.2
    * @return ETrue, if device is in any of the normal states.
    */
    TBool IsDeviceInNormalState() const;

    /**
    * Check that phone is in correct state (startup finished).
    * @since S60 3.2
    * @return ETrue, if startup state is one of the final states.
    */
    TBool IsPhoneStartupReady() const;

    /**
    * Store current device state.
    * @since S60 3.2
    * @param aDeviveState Current device state.
    */
    void SetDeviceState(TPSGlobalSystemState aDeviceState);
    
    /**
    * Boot up the device.
    * @since S60 3.2
    **/
    void DeviceStartup();

    /**
    * Starts shutdown timer.
    * @since S60 3.2
    */
    void StartShutdownTimer(TUint aTimeout);

    /**
    * Cancel the shutdown timer.
    * @since S60 3.2
    */
    void CancelShutdown();

    /**
    * Starts auto-snooze timer.
    * @since S60 3.2
    */
    void StartAutoSnoozeTimer();

    /**
    * Cancel the auto snooze timer.
    * @since S60 3.2
    */
    void CancelAutoSnooze();

    /**
    * Start observing accessory events.
    * @since S60 3.2
    */
    void StartAccessoryObserver();

    /**
    * Stop observing accessory events.
    * @since S60 3.2
    */
    void StopAccessoryObserver();

    /**
    * Start observing context framework/sensors.
    * @since S60 3.2
    */
    void StartCFObserverL();

    /**
    * Stop observing context framework/sensors.
    * @since S60 3.2
    */
    void StopCFObserverL();

    /**
    * Try to snoozes the active alarm. 
    * Stop if max snooze count reached.
    * @since S60 3.2
    */
    void TryToSnoozeActiveAlarm();

    /**
    * Start the key block timer.
    * @since S60 3.2
    */
    void StartKeyBlocker();

    /**
    * Is the kay block timer active.
    * @since S60 3.2
    */
    TBool KeyBlockActive();

    /**
    * Show the snooze info note.
    * @since S60 3.2
    */
    void AsyncShowSnoozeInfoNote();

    /**
    * Show the wakeup query.
    * @since S60 3.2
    */
    void AsyncShowWakeupQuery();

    /**
    * Cancel iAsyncCallBack.
    * @since S60 3.2
    */
    void CancelAsynchRequest();

    /**
    * Start alarm delay timer.
    * @since S60 3.2
    */
    void StartAlarmDelayTimer();

    /**
    * Is delay timer active.
    * @since S60 3.2
    * @return ETrue, if delay timer is active.
    */
    TBool AlarmDelayTimerActive();

    /**
    * Reset system inactivity timers periodically.
    * @since S60 3.2
    */
    void StartInactivityResetTimer();

    /**
    * Stop the inactivity reset timer.
    * @since S60 3.2
    */
    void StopInactivityResetTimer();

    /**
    * Fetch local uid and instance time for the alarmed calendar entry.
    * @since S60 3.2
    */
    void GetCalendarEntryInfoL();

    /**
    * Gets the calendar entry we are currently alarming.
    * @since 3.2
    */
    void FetchEntryL(CCalEntry*& aEntry);

 	/**
    * From MCalProgressCallBack.Notifies the progress percentage of  completion of 
    * calendar entry view creation
    * @since 5.0
    */

	void Progress(TInt aPercentageCompleted);

 	/**
    * From MCalProgressCallBack . Called  on completion of calendar entry view creation
    * @since 5.0
    */

	void Completed(TInt aError);

	/**
    * From MCalProgressCallBack . Called  on compltetion of calendar entry view creation
    * @since 5.0
    */
	TBool NotifyProgress();

    /**
    * Start calendar viewer in alarm mode.
    * @since S60 5.0
    */
    void StartCalendarL();

    /**
    * Check the security lock status.
    * @since S60 5.0
    * @return ETrue if security lock is active.
    */
    TBool IsSecurityLockActive();

    /**
    * Activate and deactivate backlight.
    * @since S60 3.2
    * @param aActive ETrue, to set background light blinking.
    */
    void SetBackLight(TBool aActive);
    
    /**
    * Set the flag for calendar viewer is open.
	* @param aCalendaralarmViewer ETrue, for calendar viewer.
    */    
    void SetCalendarAlarmViewer(TBool aCalendarAlarmViewer);
    
    /**
    * Get the status whether, calendar viewer is open or not
	* @return ETrue if viewer is open.
    */
    TBool IsCalendarAlarmViewer();
    
    /**
     * @brief Gets the alarm information for the alarm that is
     * about to expire
     * @return The alarm information 
     */
    SAlarmInfo* GetAlarmInfo();

private:

    /**
    * Read alarm sound filenames from CenRep.
    * @since S60 3.2
    */
    void GetAlarmSoundFilenames();

    /**
    * Read active profile settings from the profile engine.
    * @since S60 3.2
    */
    void GetProfileSettings();

    /**
    * Read the user snooze time setting for the active alarm.
    * @since S60 3.2
    */
    void GetSnoozeTimeSetting();

    /**
    * Check if the alarm server has more expired alarms to ready to be shown.
    * @since S60 3.2
    * @return ETrue, if any alarms in EAlarmStateWaitingToNotify state.
    */
    TBool CheckHasExpiredAlarmsWaiting();

    /**
    * Update alarm snooze counter.
    * @since S60 3.2
    */
    void UpdateAlarmSnoozeCount();

    /**
    * request device shutdown.
    * @since S60 3.2
    */
    void DeviceShutdown();

    /**
    * Connect to starter client.
    * @since S60 3.2
    * @return ETrue, if connection was successfull.
    **/
    TBool StarterConnect();

    /**
    * Callback function for the shutdown timer.
    * @since S60 3.2
    * @param aPtr  Reference to parent object
    * @return 0
    */
    static TInt ShutdownCallBack(TAny* aPtr);

    /**
    * Callback function for the auto snooze timer.
    * @since S60 3.2
    * @param aPtr  Reference to parent object
    * @return 0
    */
    static TInt AutoSnoozeCallBack(TAny* aPtr);

    /**
    * Callback function for the inactivity reset timer.
    * @since S60 3.2
    * @param aPtr  Not used.
    * @return 1
    */
    static TInt InactivityResetTimerCallBack(TAny* aPtr);

    /**
    * Call CAlmAlarmControl::DoAutoSnooze to handle the auto snooze.
    * @since S60 3.2
    */
    void AutoSnooze();

    /**
    * Stop the key block timer.
    * @since S60 3.2
    */
    void StopKeyBlocker() const;

    /**
    * Callback for the key block timer.
    * @since S60 3.2
    * @param aPtr  Pointer to iKeyBlockTimer
    * @return 0
    */
    static TInt KeyBlockCallBack(TAny* aPtr);

    /**
    * Callback for the auto snooze timer.
    * @since S60 3.2
    * @param aPtr  Pointer to iAlarmControl
    * @return 0
    */
    static TInt SnoozeInfoCallBack(TAny* aPtr);

    /**
    * Request wakeup dialog. Shutdown on error.
    * @since S60 3.2
    */
    void DoAskWakeup();

    /**
    * ?description
    * @since S60 3.2
    * @param aPtr  Pointer to this class.
    * @return 0
    */
    static TInt WakeupCallBack(TAny* aPtr);

    /**
    * Callback for alarm delay timer.
    * @since S60 3.2
    */
    static TInt DelayTimerCallBack(TAny* aPtr);

 /**
    * Sets the Calendar view creation status.
    * @since S60 5.0  
    */

	void SetCalendarViewStatus(TInt);

    /**
    * Gets the Calendar view creation status.
    * @since S60 5.0
    */

   TInt CalendarViewStatus();

private:  // data
    /**
    * Pointer to the alarm control.
    * Not own.
    */
    CAlmAlarmControl* iAlarmControl;

    /**
    * Pointer to our service class to send stop and snooze commands.
    * Not own.
    */
    CAknAlarmService* iSupervisor;

    /**
    * Pointer to global note API.
    * Not own.
    */
    // CNotifierDialogController* iNotifierDialogController;
    AlarmAlert* iAlarmAlert;

    /**
    * Plays alarm tones according to user settings.
    * Own.
    */
    CAlmAlarmPlayer* iAlarmPlayer;

    /**
    * Pointer to accessory observer.
    * Own.
    */
    CAlarmRemConEventsHandler* iRemConHandler;

    /**
    * Pointer to context framework observer.
    * Own.
    */
    #ifdef RD_ALMALERT__SENSOR_SUPPORT
    CAlarmContextFwSupport* iCFSupport;
    #endif // RD_ALMALERT__SENSOR_SUPPORT

    /**
    * Class to fetch the text for alarm UI.
    * Own.
    */
    CAlmAlarmInfo* iAlarmInfo;

    /**
    * Alarm server client session.
    */
    RASCliSession iAlarmServer;

    /**
    * Starter client session.
    */
    RStarterSession iStarter;   

    /**
    * For tone MIME -type.
    */
    RApaLsSession iApaSession;

    /**
    * Access to active profile settings.
    * Own.
    */
    MProfileEngine* iProfileEng;

    /**
    * Current device state.
    */
    TPSGlobalSystemState iCurrentDeviceState;   

    /**
    * Timer to allow actions to complete before device is shutdown.
    * Own.
    */
    CPeriodic* iShutdownTimer;

    /**
    * Timer for auto-snooze.
    * Own,
    */
    CPeriodic* iAutoSnoozeTimer;

    /**
    * Timer to ignore all input for 0.5 sec.
    * Own.
    */
    CPeriodic* iKeyBlockTimer;

    /**
    * Timer to delay the alarm. Prevents alarm from quickly 
    * flashing on the screen on system time change.
    * Own.
    */
    CPeriodic* iAlarmDelayTimer;

    /**
    * Timer to keep screen saver, autolock, etc. from 
    * activating during an alarm.
    * Own.
    */
    CPeriodic* iInactivityResetTimer;

    /**
    * Asynchronous callback to display the wakeup query and snooze info notes.
    * (to ease up state handling and to resolve problems when requesting 
    * a new note from inside a NoteComplete call...)
    * Own.
    */
    CAsyncCallBack* iAsyncCallBack;

    /**
    * Alarm data.
    */
    CAlarmUtils::TAlarmData iAlarmData;

    /**
    * Flag to check whether it's OK to show the wakeup query.
    */
    TBool iHasExpiredAlarmsWaiting;

    /**
    * Flag to prevent unnecessary setting of background lights.
    */
    TBool iLightsBlinking;

    /**
    * Flag to prevent unnecessary setting of keyguard.
    */
    TBool iKeyGuardActivated;

  /**
    * Pointer to calendar session.
    * Own. Remember to release after use!
    */

	CCalSession* 		iCalSession;

  /**
    * Pointer to calendar entry view.
    * Own. Remember to release after use!
    */

	CCalEntryView* 		iCalEntryView;

    /**
    * Pointer to CActiveSchedulerWait object which is  entry view.
    * Own. Remember to release after use!
    */
   CActiveSchedulerWait* iEntryAsyncWait;
	
    /**
    * Pointer to CActiveSchedulerWait object to wait for completion of current view creation request.
    * Own. Remember to release after use!
    */

   CActiveSchedulerWait* iQueueManagerAsw;
  
  /**
    * Calendar entry view creation status.
    */
enum TCalViewCreateStatus
	{
	ECalViewCreateError = -1,
	ECalViewCreateNone = 0,
	ECalViewCreateReqPending,
	ECalViewCreateCompleted
  	};
  
   TInt iCalEntryViewStatus; 
   
   /**
   * Calendar Alarm Viewer flag
   */
   TBool iCalendarAlarmViewer; 
    };

#endif  // ALARMUTILS_H


// End of File
