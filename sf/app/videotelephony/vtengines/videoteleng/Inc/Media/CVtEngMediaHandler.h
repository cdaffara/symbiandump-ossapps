/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video session interface.
*
*/



#ifndef CVTENGMEDIAHANDLER_H
#define CVTENGMEDIAHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <mvtprotocolhandler.h>
#include "MVtEngMedia.h"
#include "vtengcommands.h"
#include "CVtEngLocalVideo.h"
#include "MVtEngSettingObserver.h"

// FORWARD DECLARATIONS
class CVtEngLocalVideo;
class CVtEngRemoteVideo;
class MVtEngFrameObserver;
class CVtEngOperation;
class CVtEngExtensionBase;

// CLASS DECLARATION

/**
*  Media control.
*
*  @lib videoteleng.lib
*  @since Series 60 2.6
*/
NONSHARABLE_CLASS( CVtEngMediaHandler ) :
    public CBase,
    public MVtEngMedia,
    public MVtEngSettingObserver,
    public MVtProtocolHandler
    {
    public: // Constructor and destructor.

        /**
        * Symbian two-phase constructor.
        * @return media handler instance
        */
        static CVtEngMediaHandler* NewL();

        /**
        * Destructor.
        */
        ~CVtEngMediaHandler();

    public: // From MVtEngMedia.

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetMediaState(
            const TMediaDirection aMediaDirection,
            TInt& aActiveMedia ) const;

        /**
        * @see MVtEngMedia
        */
        virtual TBool RenderingStarted(
            const TMediaDirection
            aDirection ) const;

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetSource( TMediaSource& aSource ) const;

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetSourcesCaps( TInt& aCaps ) const;

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetCurrentCameraId( TCameraId& aId ) const;

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetCameraInfo(
            const TCameraId aId,
            TCameraInfo& aInfo ) const;

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetCurrentZoomStep( TInt& aCurrentStep );

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetMaxZoomStep( TInt& aMaxZoomStep );

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetMajorZoomStepCount( TInt& aCount );

        /**
        * @see MVtEngMedia
        */
        virtual void GetMajorZoomStep( TInt aIndex, TInt& aStep );

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetFreezeSupported( TBool& aFreezeSupported ) const;

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetFreezeState( TBool& aFrozen ) const;

        /**
        * @see MVtEngMedia
        */
        virtual TInt GetDsaState(
            const TMediaDirection aMediaDirection,
            TBool& aActive ) const;

        /**
        * @see MVtEngMedia
        */
        virtual TBool VideoCallConnected () const;
        
        /**
        * @see MVtEngMedia
        */
        virtual void GetVideoQuality( TVideoQuality& aVideoQuality ) const;

        /**
        * @see MVtEngMedia
        */
        virtual void GetObjectSharingState(
            TShareObjectState& aObjectSharingState ) const;

        /**
        * @ see MVtEngMedia
        */
        virtual void GetCameraOrientations(
            TCameraOrientation& aPrimaryCameraOrientation,
            TCameraOrientation& aSecondaryCameraOrientation ) const;

public: // New functions.

        /**
        * Tells if the Protocol initialization has completed.
        * @return ETrue if initialized
        */
        TBool ProtoInitialized() const;

        /**
        * Performs an operation in the handler.
        * @param operation to be performed on the handler
        */
        virtual void HandleL( CVtEngOperation& aOperation );

        /**
        * Returns if command can be performed by media handler.
        * @return KErrNone if a command can be perfomed
        *         KErrNotReady if not in appropriate state
        *         KErrNotSupported if command is not for media handler
        */
        TInt ValidateCommand( const TVtEngCommandId aCommand ) const;

        /**
        * Returns ETrue if video channel is closed.
        */
        TBool IsVideoChannelClosed() const;
        
        /**
         * Request update last remote video frame through MVtEngFrameObserver::vtSetFrame
         */
        void RequestLastRemoteFrame();        

    public: // from MVtProtocolHandler

        /**
        * Handles callback from protocol.
        * @param aResponse response
        */
        virtual void HandleSessionCommandEventL(const TVtCommandResponse& aResponse);

    public: // From MVtProtocolHandler.

        /**
        * Handles callback from protocol audiocontroller.
        * @param aId which command
        * @param aCmd which audio command
        * @param aContextData any additional data
        * @param aStatus how did it end up with
        */
        virtual void HandleAudioOutputControlCommandComplete(TInt aId,
            TVtAudioOutputControlCommand aCmd,
            TAny *aContextData,
            TInt aStatus);

    public: // From MVtProtocolHandler.

        /**
        * Handles event from protocol.
        * @param aEvent event
        */
        virtual void HandleSessionInformationalEventL(
            const TVtIndicationEvent& aEvent);

    public: // from MVtProtocolHandler

        /**
        * Handles error event from protocol.
        * @param aEvent error event
        */
        virtual void HandleSessionErrorEventL(const TVtErrorEvent& aEvent);

    public: // from MVtEngSettingObserver

        /**
        * Handles shared data setting change.
        * @param aId setting id
        * @param aValue setting value
        */
        virtual void HandleSettingChangedL(
            CVtEngSettings::TSettingId aId,
            const TDesC& aValue );

    public: // From MVtProtocolHandler.

        /**
        * @see MVtProtocolHandler.
        */
        void HandleVideoEncoderCommandCompletedL(
            const TVtCommandResponse& aResponse );

        /**
        * @see MVtProtocolHandler.
        */
        void HandleVideoEncoderInformationalEventL(
            const TVtIndicationEvent& aEvent);

    public: // From MVtProtocolHandler

        /**
        * @see MVtProtocolHandler.
        * @since 3.1
        */
        void HandleH324MConfigCommandCompletedL( const TVtCommandResponse& aResponse );

        /**
        * @see MVtProtocolHandler.
        * @since 3.1
        */
        void HandleH324MConfigInformationalEventL(
            const TVtIndicationEvent& aEvent );

    public: // New functions.

        /**
        * Initializes media handler.
        * @param aStatus signaling handle
        */
        void InitializeL( TRequestStatus& aStatus );

        /**
        * Cancels media handler initialization.
        */
        void CancelInitialize();

        /**
        * Initializes default blank provider.
        * @param aStatus signaling handle
        */
        void InitializeProviderL( TRequestStatus& aStatus );

        /**
        * Cancel Image sharing initialization.         
        */        
        void CancelInitializeShareImage();
        
        /**
        * Sets protocol interface.
        * aProto2Way protocol interface
        */
        void SetSessionCommand( MVtSessionCommand* aProto2Way );

        /**
        * Unitializes media handler.
        * @param aStatus request semaphore to complete
        * when uninitialization is complete
        */
        void Uninitialize( TRequestStatus& aStatus );

        /**
        * Goes to next step of uninitialization process, it can
        * be one of following
        * - Cancel all Protocol commands
        * - Protocol disconnect
        * - Protocol reset
        * - Switch blank provider.
        */
        void UninitializeNextStep();

        /**
        * Sets selected camera ID.
        * @param aId camera ID (primary or secondary)
        * @param aUpdateRequired if ETrue switch could not be performed
        *        Switch will be done when engine has been initialized.
        */
        void SetSelectedCameraId(
            const TCameraId aId,
            TBool aUpdateRequired = EFalse );

        /**
        * Handles source initialization or switch.
        * @param aResult Symbian OS error code
        */
        void HandleLocalVideoOperationCompleteL( const TInt aResult );

        /**
        * SDK init info.
        * @return Protocol initialization info
        */
        TVtInitInfo& SdkInitInfo();

        /**
        * Returns protocol state maintained by VTEngine, not by Protocol.
        * @return state
        */
        MVtProtocolCommand::TVtProtocolState ProtoState();

        /**
        * Sets up video connection to remote terminal.
        * @return Symbian OS error code
        */
        TInt Connect();

        /**
        * Disconnects session.
        * @return Symbian OS error code
        */
        TInt Disconnect();

        /**
        * Adds video data source to protocol engine.
		* @param Video channel logical channel ID number.
        * @return Symbian OS error code
        */
        TInt AddVideoSource( TUint aChannelId );

        /**
        * Adds audio data source to protocol engine.
		* @param Audio channel logical channel ID number.
        * @return Symbian OS error code
        */
        TInt AddAudioSource( TUint aChannelId );

        /**
        * Adds video sink.
		* @param Video channel logical channel ID number.
        * @return Symbian OS error code
        */
        TInt AddVideoSink( TUint aChannelId );

        /**
        * Adds audio sink.
		* @param Audio channel logical channel ID number.
        * @return Symbian OS error code
        */
        TInt AddAudioSink( TUint aChannelId );

        /**
        * Sets the output volume for Protocol.
        * @param aVolume the level of the volume
        * @return Symbian OS error code
        */
        TInt SetVolume( const TInt aVolume,
            CVtEngOperation* aOperation = NULL );

        /**
        * Enables video when primary camera becomes available.
        * @param aEnable do we enable or disable
        * @return Symbian OS error code
        */
        TInt AutoEnableVideo( TBool aEnable = ETrue );

        /**
        * Deletes protocol instance and associated Protocol interfaces.
        */
        void FinalizeUninitialization();

        /**
        * Creates extension according to API Uid.
        * @since S60 v3.2
        * @param aUid extension API Uid.
        * @return Extension pointer
        * @exeption Leaves if creation fails
        *
        */
        CVtEngExtensionBase* CreateExtensionsL( const TUid& aUid );
        
        /**
        * Checks if there is already an operation pending.
        * @return KVtEngCommandNone if no operation is pending or Command identifier
        */
        TVtEngCommandId GetPendingCommand();

    private: // new functions

        /**
        * Sends a sync message to Protocol after provider switch.
        */
        void RequestFrame( );

        /**
        * Sets video quality
        */
        void SetVideoQualityL(
            const TVideoQuality aVideoQuality,
            const TBool aSetPeerVideoQuality );

        /**
        * Sets peer video quality
        */
        void SetPeerVideoQuality( const TVideoQuality aVideoQuality );

        /**
        * Check's flag's validity. If not valid, panic's in debug mode.
        * @param aFlag flag to check.
        */
        void AssertFlag( const TInt aFlag ) const;

        /**
        * Gets the version information from SysUtil
        * and sets it into Protocol.
        */
        void SetVersionInfoL();

        /**
        * Sets supported resolutions to Protocol.
        */
        void SetSupportedResolutions();

		/**
        * Sets used call setup enhancement parameters.
        */
		void SetFastCsupOptions();

        /**
        * Does Protocol reset.
        */
        void ResetProtoL();

        /**
        * Does the actual disconnecting with Protocol.
        */
        void DisconnectProtoL();

        /**
        * Sets the defaults for volumes.
        */
        void InitializeVolume();

        /**
        * Checks if a flag in iProtoInitFlags is set.
        * @return ETrue if flag is set
        */
        TBool IsFlag( const TInt aFlag ) const;

        /**
        * Sets a flag in iProtoInitFlags.
        * @param aFlag flag to set
        */
        inline void SetFlag( const TInt aFlag );

        /**
        * Clears a flag in iProtoInitFlags.
        * @param aFlag flag to clear
        */
        inline void ClearFlag( const TInt aFlag );

        /**
        * Requests the volume controller.
        * @return ETrue if retrieved ok
        */
        TBool RequestAudioControllerL();

        /**
        * Spatial trade off indication handling method. Indication is
        * sent by the peer every time when its spatial trade off is updated.
        * @param aLogicalChannelId Indication's logical channel Id.
        * @param aTradeOff Indication trade off value.
        */
        void HandleSpatialTradeoffIndication(
            const TUint16 aLogicalChannelId, const TUint8 aTradeOff );

        /**
        * Spatial trade off command handling method. Spatial trade off
        * command is sent by the peer whenever peer wants to adjust our
        * video quality.
        * @param aLogicalChannelId Command's logical channel Id.
        * @param aTradeOff New trade off value.
        */
        void HandleSpatialTradeoffCommandL(
            const TUint16 aLogicalChannelId, const TUint8 aTradeOff );

    private: // constructor and destructor

        /**
        * C++ default constructor.
        */
        CVtEngMediaHandler();

        /**
        * Symbian constructor that may leave.
        */
        void ConstructL();

    private:
        /**
        * Operation values
        */
        enum TOperation
            {
            /**
            * No-op.
            * 0
            */
            ENone,

            /**
            * Add video source to protocol.
            * 1
            */
            EAddVideoSource,

            /**
            * Add video source to protocol.
            * 2
            */
            EAddVideoSink,

            /**
            * Add audio source to protocol.
            * 3
            */
            EAddAudioSource,

            /**
            * Add audio source to protocol.
            * 4
            */
            EAddAudioSink,

            /**
            * Connect protocol.
            * 5
            */
            EConnectProto,

            /**
            * Mute audio.
            * 6
            */
            EMuteAudio,

            /**
            * Resume audio sending.
            * 7
            */
            EUnmuteAudio,

            /**
            * Enable video for peer.
            * 8
            */
            EEnableVideo,

            /**
            * Disable video for peer.
            * 9
            */
            EDisableVideo,

            /**
            * Cancel ongoing request to Protocol
            * as part of uninitiazation process.
            * 10
            */
            EUninitializeCancelAllProto,

            /**
            * Disconnect protocol.
            * 11
            */
            EDisconnectProto,

            /**
            * Destructs local/remote video and VS instances.
            * 12
            */
            EDestructInternals,

            /**
            * Terminates H.324 session.
            * 13
            */
            ETerminateSession,

            /**
            * Removing sinks and sources.
            * 14
            */
            ERemovingSinksAndSources,

            /**
            * Request 324 control interface from protocol.
            * 15
            */
            EGet324CtrlInterface,

            /**
            * Set volume.
            * 16
            */
            ESetAudioVolume,

            /**
            * Gets the maximum volume for Protocol.
            * 17
            */
            EGetAudioGetMaxVolume,

            /**
            * Reset protocol.
            * 18
            */
            EDestructProtoPhaseDisconnect,

            /**
            * Destruct switch blank.
            * 19
            */
            EDestructSwitchBlank,

            /**
            * Reset protocol.
            * 20
            */
            EDestructProtoPhaseReset,

            /**
            * Check Protocol state.
            * 21
            */
            ECheckProtoState,

            /**
            * Enables video internally (not originated by UI).
            * 22
            */
            EEnableVideoInternal,

            /**
            * Disables video internally (not originated by UI).
            * 23
            */
            EDisableVideoInternal,

            /**
            * Request H263Enc interface from protocol.
            * 24
            */
            EGetH263EncInterface,

            /**
            * Requests an intra frame.
            * 25
            */
            ESendIntraframe,

            /**
            * Set supported resolutions.
            * 26
            */
            ESetSupportedResolutions,

            /**
            * Set video quality (by changing framerate)
            * 27
            */
            ESetVideoQuality,

			/**
            * Remove H324 interface
            * 28
            */
			ERemove324CtrlInterface,

			/**
            * Remove video encoder interface
            * 29
            */
			ERemoveH263EncInterface,

			/**
            * Sends Video Temporal-spatial trade off command
            * 30
            */
			ESendVTSTO,

			/**
            * Sets fast call setup options
            * 31
            */
			ESetFastCsupOptions,

            /**
            * Sets vendor id
            * 32
            */
			ESetVendorId,

            /**
            * Set interval when I-frames are sent
            * 33
            */
			ESetIFrameInterval
            };

        /**
        * Operation and related command id.
        */
        struct TCmdOpPair
            {
            // Operation
            TOperation   iOp;
            // Command id returned by proto2way asynch request
            TInt iProtoCmdId;
            };

        /**
        * Enables/Disables video sending.
        * @param aEnable ETrue if video is to be enabled
        * @param aOp operation to be added
        * NOTE. Valid only for enabling video, not when disabled.
        * @see void AutoEnableVideo
        * @return started operation or ENone
        */
        TOperation EnableVideoL(
            const TBool aEnable,
            const TOperation aOp = EEnableVideo );

        /**
        * Calls Protocol pausing and acts according to the result.
        * @return operation id for performed operation
        */
        TOperation DoPauseVideo();

        /**
        * Continues to next step of uninitialization.
        * @param aOp operation to perform
        * @return command id
        */
        void ContinueUninitializeL( const TOperation aOp );

        /**
        * Cancels all the running commands within Protocol.
        * @param operation to be returned
        *       EUninitializeCancelAllProto if cancellation is ongoing
        *       (either because was already or just started)
        *       ENone if cancellation is not ongoing
        * @return error code
        *        KErrNotReady if engine not in suitable state
        *        KErrAlreadyExists if cancel was already pending
        *        KErrNone if cancellation request succeeded
        *        other Symbian OS error code if cancellation fails
        */
        TInt CancelAllProtoCommands( TOperation &aOp );

        /**
        * Stops view finder and remote video rendering.
        */
        void StopRendering();

        /**
        * Updates volume to Protocol (if possible).
        */
        void UpdateVolume();

        /**
        * Initiates protocol engine reset.
        */
        void ResetProto2WayL();

        /**
        * Removes operation from pending operations array.
        * @param aOp TOperation
        * @return ETrue if operation was found (and hence removed)
        */
        template<class T>
        TBool RemoveOperation( const T& aOp );

        /**
        * Adds operation pending for callback.
        * @param aOp operation value
        * @param aCmdId protocol commd id or 0 if not Protocol cmd
        * @param aResetExisting if ETrue removes existing operations
        *        from array
        * @return index of the operation in array (i.e. >= 0 )
        */
        TInt AddOperation(
            const TOperation aOp,
            TInt aCmdId,	
            const TBool aResetExisting = EFalse );

        /**
        * Checks if an operation is pending.
        * @param aOp operation
        * @return index if operation is pending or KErrNotFound
        */
        TInt IsPending( const TOperation aOp ) const;

        /**
        * Checks if an operation is pending.
        * @param aCmdId command id
        * @return index if operation is pending or KErrNotFound
        */
        TInt IsPending( const TInt aCmdId ) const;

        /**
        * Completes asynchronous operation.
        * @param aResult
        * @return KErrNotFound if no operation else KErrNone
        */
        TInt CompleteOp( const TInt aResult );

        /**
        * Completes asynchronous operation asynchronously.
        * @param aResult
        * @return KErrNotFound if no operation else KErrNone
        */
        void AsyncCompleteOp( const TInt aResult = KErrNone );

        /**
        * Checks if specific operation matches to response from protocol.
        * @param aProtoCmdId command id returned by protocol
        * @param aOperation operation to search for in pending commands
        * @param aIndex index in iPendingOps array if found
        * @return ETrue if there's corrensponding entry in pending ops
        */
        TBool MatchResponseToPendingOps(
            const TInt aProtoCmdId,
            const TOperation aOperation,
            TInt* aIndex = NULL );

        /**
        * Starts "self-destruction" timer.
        */
        void StartDisconnectTimerL();

        /**
        * Assigns new pending operation. Only one can be pending at a time
        * so if there already is pending operation it is completed with error.
        * @param aOperation operation to assign as pending
        * @param aReplaceError error code to complete penging operation if
        *        one exists. If KErrNone is passed, replacement is not done.
        * @return KErrNone if assignment done successfully i.e. there was
                  not pending operation that was completed.
        *         KErrCancel if some other operation was pending and was
        *                     completed with aReplaceError. Operation is
        *                     still assigned successfully.
        *         KErrGeneral if operation was not assigned.
        */
        TInt AssignPendingOp(
            CVtEngOperation& aOperation,
            TInt aReplaceError = KErrCancel );

        /**
        * Callback function that completes ongoing operation.
        * @param aAny pointer to an instance of the class.
        * @return always KErrNone.
        */
        static TInt DoCompleteOp( TAny* aAny );

        /**
        * Implementation of self-destruction timer activity.
        * Causes a panic.
        * @param aAny pointer to an instance of the class
        *   Not used.
        * @return KErrNone;
        */
        static TInt TimerCallback( TAny* aAny );

        /**
        * Creates and enques async callback for uninitialization.
        * @see iNextStepCallback
        */
        void NextUninitStepCallbackL();

        /**
        * Callback method for async uninitialization step.
        * @seeiNextStepCallback
        * @return KErrNone;
        */
        static TInt UninitCallback( TAny* aAny );

		/**
        * Query 2-way's additional interfaces, encoder and H324
        * @return KErrNone;
        */
        void QueryProtoInterfacesL();

		/**
        * Release 2-way's additional interfaces, encoder and H324
        * @return KErrNone;
        */
        void ReleaseProtoInterfacesL();

        /**
        * Check for pending Protocol extension interface commands.
        * @return ETrue if one or more extension command is pending
        */
        TBool ExtensionCommandsPending() const;

        /**
        * Checks is it possible to add sink / source from session 
        * state point of view.
        * @return ETrue if adding is allowed.
        */
        TBool MediaAddingValidity() const;
        
        /**
        * Provider initialization state
        */
        enum TInitState
            {
            /**
            * Initialization not ongoing.
            */
            EInitStateNone,

            /**
            * Initialize still image.
            */
            EStateInitStillImage,

            /**
            * Initilize default blank provider.
            */
            EStateInitProvider,

            /**
            * Provider is set to none.
            */
            EStateWaitingProviderChangeForUninit,

            /**
            * Initialization has been cancelled.
            */
            EStateInitCanceled
            };

        /**
        * State flags.
        */
        enum
            {
            // Protocol state flags
            // Sink/source added
            EProtoAudioSourceAdded   = 0x000001,
            EProtoVideoSourceAdded   = 0x000002,
            EProtoAudioSinkAdded     = 0x000004,
            EProtoVideoSinkAdded     = 0x000008,
            // Track paused
            EProtoAudioTrackPaused   = 0x000040,
            EProtoVideoTrackPaused   = 0x000080,

            // Pause/resume related flags.
            ETwoStageOperation    = 0x000100,
            ESourceUpdateNeeded   = 0x000200,
            EProtoPauseVideoRequested= 0x000400,
            EProtoPauseVideoBlankProv= 0x000800,
            EProtoContVideoBlankProv = 0x001000,
            EProtoContVideoBlankInit = 0x002000,

            // Must send intra
            EProtoSendIntraFrame     = 0x004000,

            // 324 interface acquired
            EProto324IFAcquired      = 0x008000,

            // Cancel all command is called on Protocol
            // or there's nothing to cancel
            EProtoCancelProtoCmdsOk     = 0x010000,

            // NON Protocol related flags

            EVideoQualitySetByPeer= 0x020000,

            // Outgoing video channel closed (after being open)
            // Used for preventing video pause/resume when
            // the channel is not open
            EOutVideoChannelClosed = 0x040000,

            // Outgoing audio channel closed (after being open)
            // Used for preventing audio pause/resume
            // the channel is not open
            EOutAudioChannelClosed = 0x080000
            };


        /**
        * Defines actions that can be taken when sharing started
        * and/or stopped
        */
        enum TVideoEngAction
            {
            EVideoActionNone,
            EVideoActionEnable,
            EVideoActionDisable
            };

        /**
        * Helper class for handling video quality manipulation.
        */
        class TVtEngVideoQuality
            {

        public:

            // Forward for inner class
            class TVQSParams;

        public:

            /**
            * Constructor.
            */
            TVtEngVideoQuality();

            /**
            * Destructor.
            */
            ~TVtEngVideoQuality();

            /**
            * Sets new local video quality setting.
            * @param aId Operation id.
            * @param aSetPeer ETrue if peer video quality needs to be set,
            * EFalse otherwise.
            */
            void SettingVideoQualityL( TInt aId, const TVideoQuality aValue,
                TBool aSetPeer );

            /**
            * Get current local video quality setting.
            * @return Current local video quality setting.
            */
                TVideoQuality VideoQuality() const;

            /**
            * Setting video quality succeeded.
            * @return ETrue if record with given id was found, EFalse other
            * wise.
            */
            TBool SettingSucceeded( TInt aId, TVQSParams& aParams );

            /**
            * Setting video quality failed.
            */
            void SettingFailed( TInt aId );

            /**
            * Called when video quality indication is received from peer.
            */
            void UpdateVideoQuality( const TVideoQuality aVideoQuality );

            /**
            * Converts given trade off value to video quality enum.
            */
            TVideoQuality FromTradeOff( const TInt aTradeOff ) const;

            /**
            * Converts given video quality enum to trade off value.
            */
            TInt ToFrameRate( const TVideoQuality aVideoQuality ) const;

            /**
            * Converts given video quality value to frame rate.
            */
            TUint8 ToTradeOff( const TVideoQuality aVideoQuality ) const;

            /**
            * Finds id of setting param and returns index to it.
            */
            TInt FindVQSParam( TInt aId ) const;

        public:

            /**
            * Video quality parameters.
            */
            struct TVQSParams
                {

            public:

                // Default constructor
                TVQSParams();

                // Constructor with initializing parameters
                TVQSParams( TInt aId, const TVideoQuality aValue,
                    TBool aSetPeer );

            public: // data

                // Operation id
                TInt iId;

                // VideoQuality value to be set
                TVideoQuality iValue;

                // Defines whether peer video quality needs to be set or not
                TBool iSetPeer;

                };

        private:

            // Current local video quality
            TVideoQuality iCurrent;

            // Qued params
            RArray< TVQSParams > iParamList;
            };

		// Class for handling different kind of H.245 logical
		// channels and their state in different MONA call setup
		// scenarios
		class TLogicalChannel
			{
		public:

			// H.245 logical channel ID to be established or
			// already active
			TUint iLogicalChannelId;

			// H.245 logical channel ID that is currently closing.
			TUint iLogicalChannelIdClosing;

			// Variable to tell is same type (video or audio, incoming or outgoing)
			// channel already started to close but not closed yet. If closing is pending
			// same type channel cannot be opened until closing has finished or sink/source
			// addition has failed for some reason.
			TBool iSameTypeClosingPending;

			// Variable to tell is same type (video or audio, incoming or outgoing)
			// channel ready to be opened.
			TBool iSameTypeChannelReadyToOpen;

			};

    private:    // Data members.

        // Protocol engine pointer.
        MVtSessionCommand* iSessionCommand;

        // Local video configuration.
        CVtEngLocalVideo*   iLocalVideo;

        // Remote video configuration.
        CVtEngRemoteVideo*  iRemoteVideo;

        // Pending internal operations.
        TInt               iCmdId;
        CArrayFixFlat<TCmdOpPair>* iPendingOps;

        // Video Source initialization state.
        TInitState          iVSInitState;

        // Flags used to determine if audio and video sink/source
        // is added to protocol and media state.
        TInt                iProtoStateFlags;

        // protocol state.
        MVtProtocolCommand::TVtProtocolState        iProtoState;

        // Initialization flag.
        TBool               iInitialized;

        // Remote media state.
        TInt                iRemoteMediaState;

        // Pending operation from client.
        CVtEngOperation*    iPendingOp;

        // Protocol initialization information.
        TVtInitInfo     iSdkInitInfo;


        // Request status for (un)initialization.
        TRequestStatus*     iRequestStatus;

        // Audio controller handle.
        MVtAudioConfigCommand* iAudioCtrl;

        // Timer to detect failure in disconnection.
        CPeriodic* iTimer;

        // Protocol max volume.
        TInt iProtoMaxVolume;

        // Selected camera is not necessary activated also.
        TCameraId           iSelectedCameraId;

        // ETrue when it is allowed to update volume.
        TBool iUpdateVolumeAllowed;

        // ETrue when it is needed to update volume.
        TBool iUpdateVolumeRequired;

        // Owned asynch callback.
        CIdle* iAsyncCallback;

        // Pending volume operation.
        CVtEngOperation*    iPendingVolumeOp;

        // Used for various connection related operations.
        TInt iConnectionStates;

        // Used while pausing and resuming video.
        CVtEngLocalVideo::TVtEngProviderType iTempProviderInfo;

        // Used for setting the vendor info.
        MVtH324ConfigCommand* iH324Config;


        // Used H263 interface.
        MVtVideoConfigCommand* iH263Encoder;

        // Video quality
        TVtEngVideoQuality iVideoQuality;

        // Sharing object while video is disable will set this flag
        TVideoEngAction iVideoAction;

		// Logical channel structure for outgoing video channel(s).
		TLogicalChannel iVideoOutgoingLogicalChannel;

		// Logical channel structure for outgoing audio channel(s).
		TLogicalChannel iAudioOutgoingLogicalChannel;

		// Logical channel structure for incoming video channel(s).
		TLogicalChannel iVideoIncomingLogicalChannel;

		// Logical channel structure for incoming audio channel(s).
		TLogicalChannel iAudioIncomingLogicalChannel;

		/* For proceeding to next uninit step asynchronously.
		 * Needed because Proto extension interface cannot be released within
		 * callback 
		 */
		 CAsyncCallBack* iNextStepCallback;


        /* Do not complete operation on pause command. This flag
         * is used when image sharing is stopped and video was disabled
         * when sharing was initially started to prevent black frame sending
         * during blank switch.
         */
        TBool iPauseNoCompleteOp;

        /**
        * Result for asynchronous operation completion.
        */
        TInt iAsyncCompleteOpResult;
    };

#endif      // CVTENGMEDIAHANDLER_H

// End of File
