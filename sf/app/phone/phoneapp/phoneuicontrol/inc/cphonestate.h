/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*     A base class for an object implementing the state.
*
*/


#ifndef CPHONESTATE_H
#define CPHONESTATE_H

// INCLUDES
#include <w32std.h>
#include <eikenv.h>
#include <remconcoreapitargetobserver.h>
#include <MProfileEngine.h>
#include "mphonestate.h"
#include "mphoneviewcommandhandle.h"
#include "cphoneuicommandmanager.h"
#include "cphonenumberentrymanager.h"

// FORWARD DECLARATIONS
class MPhoneState;
class MPhoneStateMachine;
class TPhoneCmdParamCallHeaderData;
class MPhoneCustomization;

// CLASS DECLARATION

/**
*  An abstract class for an object implementing the state.
*
*/
class CPhoneState :
    public CBase,
    public MPhoneState
    {
    public:
        
        enum TNumberEntrySetRule
            {
            ESetNEVisibilityFalse,
            ECheckIfNEUsedBeforeSettingVisibilityFalse
            };
        /**
        * Constructor
        */
        IMPORT_C CPhoneState(
            MPhoneStateMachine* aStateMachine,
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aCustomization);

        /**
        * Creates UiCommandManager instance.
        */
        IMPORT_C void BaseConstructL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneState();

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        IMPORT_C virtual void HandlePhoneEngineMessageL(
            const TInt aMessage,
            TInt aCallId );

        /**
        * HandleError
        * Implements error handling framework
        * @param aErrorInfo: the error info
        */
        IMPORT_C virtual void HandleErrorL(
            const TPEErrorInfo& aErrorInfo );

        /**
        * Handle processed (short, long) key messages from the key event
        * handler
        * @param aMessage a key message
        * @param aCode key event code
        */
        IMPORT_C virtual void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );

        /**
        * Handles raw key events from the key event handler
        * @param aKeyEvent a key event
        * @param aEventCode key event code
        */
        IMPORT_C virtual void HandleKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aEventCode );

        /**
        * Handles startup of the phone application
        */
        IMPORT_C virtual void HandlePhoneStartupL();

        /**
        * This function is called when there is property value change.
        * @param aCategory Category of the property
        * @param aKey Property key that is changed
        * @param aValue New property value
        */
        IMPORT_C virtual void HandlePropertyChangedL(
            const TUid& aCategory,
            const TUint aKey,
            const TInt aValue );

        /**
        * Handles commands.
        * @param aCommand It is the code of the command to be handled.
        * @returns boolean value was the command handled by the state
        *          (ETrue) or not (EFalse)
        */
        IMPORT_C virtual TBool HandleCommandL( TInt aCommand );

        /**
        * Processes command.
        * @param aCommand It is the code of the command to be handled.
        * @returns boolean value was the command handled by the state
        *          (ETrue) or not (EFalse)
        */
        IMPORT_C virtual TBool ProcessCommandL( TInt aCommand );

        /**
        * Handles commands from the Remote Control framework.
        * @param aOperationId The operation ID of the command.
        * @param aButtonAct The button action associated with the command.
        * @return ETrue if the event was handled, EFalse otherwise.
        */
        IMPORT_C virtual TBool HandleRemConCommandL(
            TRemConCoreApiOperationId aOperationId,
            TRemConCoreApiButtonAction aButtonAct);

        /**
         * Plays DTMF tone for key event
         * */
        IMPORT_C void HandleDtmfKeyToneL( const TKeyEvent& aKeyEvent,
                TEventCode aEventCode );

        /**
        * Show global InfoNote
        * @param aResourceId resource id to be resolved
        * @param aNotificationDialog True if notification
        *        dialog should be used.
        */
        IMPORT_C void SendGlobalInfoNoteL( TInt aResourceId,
                TBool aNotificationDialog = EFalse );

        /**
        * Show global ErrorNote
        * @param aResourceId resource id to be resolved
        * @param aNotificationDialog True if notification
        *        dialog should be used.
        */
        IMPORT_C void SendGlobalErrorNoteL( TInt aResourceId,
                TBool aNotificationDialog = EFalse );

        /**
        * Fetches autolock information - is it set on or not
        * @return is autolock set on (ETrue) or off (EFalse)
        */
        IMPORT_C TBool IsAutoLockOn() const;

        /**
        * Return is sim state ok
        * @return ETrue if is sim ok, otherwise EFalse
        **/
        IMPORT_C TBool IsSimOk();
          
         /**
         * Getter for CEikonEnv to avoid use of static system calls
         * @return CEikonEnv handle
         */
         IMPORT_C CEikonEnv* EikonEnv() const;
         
         /**
         * Setter for CEikonEnv to avoid use of static system calls
         * @param CEikonEnv handle
         */
         IMPORT_C virtual void SetEikonEnv( CEikonEnv* aEnv );
         
         /**
         * Dial CS voice call
         */
         IMPORT_C void DialVoiceCallL();
         
         /**
         * Checks whether customized dialer view is active,
         * @return ETrue if customized dialer is active
         */
         IMPORT_C TBool IsCustomizedDialerVisibleL() const;
         
         /**
         * Handle state-specific behaviour when number entry is cleared
         */
         IMPORT_C virtual void HandleNumberEntryClearedL();
         
         /**
         * Sets up call header.(removes phone dialogs, 
         * sets dialer visibility according to aRule value and puts 
         * phone on top of everything). Orders UiCommand manager to
         * update CBS's etc.
         * @param aCallId - call id to set up
         * @param aRule - Number Entry visibility set option.
         */ 
          IMPORT_C void DisplayCallHeaderL( 
                  TInt aCallId,
                  TNumberEntrySetRule aRule );
    protected:

        /**
        * Handle EPEMessageAudioMuteChanged
        */
        IMPORT_C void HandleAudioMuteChangedL();

        /**
        * Handle EPEMessageAudioOutputChanged
        */
        IMPORT_C void HandleAudioOutputChangedL();

        /**
        * Handle SideVolumeKeyInc
        */
        IMPORT_C void IncreaseAudioVolumeL();

        /**
        * Handle SideVolumeKeyDec
        */
        IMPORT_C void DecreaseAudioVolumeL();

        /**
        * Dial CS multimedia call
        */
        IMPORT_C void DialMultimediaCallL();

        /**
        * Disconnect call
        */
        IMPORT_C TBool DisconnectCallL();

        /**
        * Set up the Idle screen in the background for the cases
        * where the phone is no longer the top application (eg. end
        * call when an external app is being displayed)
        */
        IMPORT_C void RemoveDialogsAndSendPhoneToBackgroundL();

        /**
        * Update Single Active Call
        * @param aCallid call id
        */
        IMPORT_C void UpdateSingleActiveCallL( TInt aCallId );

        /**
        * Update remote information data in the call header
        * @param aCallid call id
        */
        IMPORT_C void UpdateCallHeader( TInt aCallId );

        /**
        * Updates call header and ui commands.
        * @param aCallid call id
        */
        IMPORT_C void UpdateCallHeaderAndUiCommandsL( TInt aCallId );
       
        /**
        * Show note
        * @param aResourceId resource id to be resolved
        */
        IMPORT_C void ShowNoteL( TInt aResourceId );

        /**
        * Show query
        * @param aResourceId resource id to be resolved
        */
        IMPORT_C void ShowQueryL( TInt aResourceId );

        /**
        * Show text query
        * @param aDialogResourceId dialog's resource id
        * @param aDefaultCbaResourceId default CBA's resource id
        * @param aContentCbaResourceId content CBA's resource id
        * @param aDataText pointer to data text
        * @param aSendKeyEnabled send key enabled status for the query
        */
        IMPORT_C void ShowTextQueryL(
            TInt aDialogResourceId,
            TInt aDefaultCbaResourceId,
            TInt aContentCbaResourceId,
            TDes* aDataText,
            TBool aSendKeyEnabled = EFalse );

        /**
        * Displays the call termination note, if required
        */
        IMPORT_C void DisplayCallTerminationNoteL();

        /**
        * Set the handsfree mode
        * @param aHandsfreeMode ETrue to activate IHF, EFalse to deactivate
        */
        IMPORT_C void SetHandsfreeModeL( TBool aHandsfreeMode );

        /**
        * Set the BT handsfree mode
        * @param aHandsfreeMode ETrue to activate BT, EFalse to deactivate
        */
        IMPORT_C void SetBTHandsfreeModeL( TBool aHandsfreeMode );

         /**
        * Show global WarningNote
        * @param aResourceId resource id to be resolved
        * @param aNotificationDialog True if notification dialog
        *                            should be used.
        */
        IMPORT_C void SendGlobalWarningNoteL( TInt aResourceId,
                TBool aNotificationDialog = EFalse );

        /**
        * Checks is the call id a video call.
        * @param aCallId - call id to be checked
        * @return ETrue if video call id, otherwise EFalse
        */
        IMPORT_C TBool IsVideoCall( const TInt aCallId );

        /**
        * Return SimState.
        */
        IMPORT_C TPESimState SimState() const;

        /**
        * Start show security note
        * This should be called in startup and idle state
        * if sim state not ok
        */
        IMPORT_C void StartShowSecurityNoteL();

        /**
        * Check if the sim card state is EPESimNotPresent with
        * the security mode is enabled.
        * @return ETrue if the sim card state is EPESimNotPresent and
        * the security mode is enabled, otherwise EFalse
        **/
        IMPORT_C TBool IsSimStateNotPresentWithSecurityModeEnabled();

        /**
        * Handle audio output availability change
        */
        void HandleAudioAvailableOutputChangedL();

        /**
        * Shows 'Number busy' note.
        */
        IMPORT_C void ShowNumberBusyNoteL();

        /**
        * Informs view that UI is being updated (call bubble or number editor).
        * EndUiUpdate() must be called when update is done.
        */
        IMPORT_C void BeginUiUpdateLC();

        /**
        * Informs view that UI update is completed.
        */
        IMPORT_C void EndUiUpdate();

        /*
        * Checks if necessary to show call termination note
        *
        * @param aCallId call id
        * @return true if note should be shown
        */
        IMPORT_C virtual TBool CheckIfShowCallTerminationNote( );

        /*
        * Destroys touchpane buttons.
        */
        IMPORT_C void DeleteTouchPaneButtons();

        /**
        * Set default flags values.
        * No active call
        */
        IMPORT_C void SetDefaultFlagsL();

        /*
        * Sets ringtone playback.
        * @param aCallId call id
        */
        IMPORT_C void SetRingingTonePlaybackL( TInt aCallId );

        /**
        * A message handling function for message EPEMessageDisconnecting
        * @param aCallId: the call id of the call
        */
        IMPORT_C void HandleDisconnectingL( TInt aCallId );
        
        /**
        * Opens soft reject message editor.
        */
        IMPORT_C virtual void OpenSoftRejectMessageEditorL();
        
    protected:

        /**
        * Sets toolbar loudspeaker button enabled.
        */
        IMPORT_C void SetToolbarButtonLoudspeakerEnabled();
         
        /**
        * Sets state of TitleBar Back button 
        */
        IMPORT_C void SetBackButtonActive( TBool aActive );

         /**
         * Updates ui commands 
         */
         IMPORT_C void UpdateUiCommands();
         
    protected:

       /**
       * Checks if call is waiting, returns ETrue if call is waiting
       * otherwise EFalse.
       * @param aCallId, call id.
       */
       TBool IsWaitingCallL( const TInt aCallId );

       /**
       * Checks status of KFeatureIdSideVolumeKeys and KTelephonyLVFlagScrollVolumeKeys
       * keys. If KTelephonyLVFlagScrollVolumeKeys is false and KFeatureIdSideVolumeKeys is true
       * then side volume keys are only supported and method returns true, otherwise false.
       */
       TBool OnlySideVolumeKeySupported();

       /**
       * Checks if device has connected call ongoing.
       * List of connected call states: EPEStateConnected
       *                                EPEStateConnectedConference
       *                                EPEStateConnecting
       */
       TBool IsAnyConnectedCalls();

    private:

        /**
        * Handle EPEMessageChangedCallDuration
        * @param aCallId call id of the message
        */
        void HandleChangedCallDurationL( TInt aCallId );

        /**
        * Sends key down event to the phone engine
        * @param aKeyEvent a key event
        * @param aEventCode key event code
        */
        void SendKeyPressL(
            const TKeyEvent& aKeyEvent,
            TEventCode aEventCode );

        /**
        * Sends key events to the phone engine
        * @param aKeyEvent a key event
        * @param aEventCode key event code
        */
        void SendKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aEventCode );

        /**
        * Handles EPEMessageInitiatedEmergency call message from Phone Engine.
        * @param aCallId - Caller id.
        */
        void HandleInitiatedEmergencyCallL( TInt aCallId );

        /**
         *  Handles EPEMessageInitiatedEmergencyWhileActiveVideo message from
         * Phone Engine.
         */
        void HandleInitiatedEmergencyWhileActiveVideoL();

        /**
        * Handles network ciphering info changes
        */
        void HandleCallSecureStatusChangeL( TInt aCallId );

        /**
        * Shows WLAN MAC address note
        */
        void ShowWlanMacAddressL();

        /**
        * TCleanupOperation to call EndUiUpdate(), if leave occurs
        * after BeginUiUpdate().
        */
        static void UiUpdateCleanup(TAny* aThis );

        /**
        * Shows video call specific out of memory note.
        */
        void ShowVideoCallOutOfMemoryNoteL();

        /**
        * Returns sim security status:
        * Rejected or UnAccepted.
        */
        TInt SimSecurityStatus() const;

        /**
        * Show CUG in use note
        */
        void HandleCugInUseNoteL();

        /**
         * Handles changes in the SIM card state.
         */
        void HandleSimStateChangedL();

        /*
         * Checks if keyevent is from dtmf key and sends it to phone-engine
         */
        void SendDtmfKeyEventL( const TKeyEvent& aKeyEvent,
                TEventCode aEventCode  );

        /**
        * Loads data commonengine stringloader.
        * @param aData - The returned data including needed text.
        * @param aResourceId - Resource id for text.
        */
        void LoadResource( TDes& aData, const TInt aResource ) const;

    protected:  // Data

        /**
        * Reference to State machine
        */
        MPhoneStateMachine* iStateMachine;

        /**
        * Reference to the Phone UI View command handle
        */
        MPhoneViewCommandHandle* iViewCommandHandle;

    protected:

        /**
        * Customization of functionality according to e.g.
        * call type specific needs.
        * Not own.
        */
        MPhoneCustomization* iCustomization;

        // CBA key manager. Own.
        CPhoneUiCommandManager* iUiCommandManager;
        
        // Number entry manager
        CPhoneNumberEntryManager* iNumberEntryManager;


    private:

        // Previously handled SIM card state.
        TPESimState iPreviousSimState;

        // Internal variable for EikonEnv to avoid
        // use of static system calls
        CEikonEnv* iEnv; // Not owned
        };

#endif // CPHONESTATE_H

// End of File
