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
* Description:   mms operation common base class
*
*/




#ifndef MMSBASEOPERATION_H
#define MMSBASEOPERATION_H

//  INCLUDES
#include <mentact.h> // base class CMsgActive
#include <es_sock.h> // for RConnection & RSocketServ
#include <msvstd.h>
#include <babackup.h>
#include <logwrap.h>
#include <badesca.h>

// CONSTANTS
// used for recipient arrays, just increment for expanding the array.
const TInt KMmsArrayAllocationNumber = 6;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMmsSettings;
class RFs;
class CMsvServerEntry;
class CMmsConnectionInitiator;
class CMmsServerEntry;
class CMmsSession;
class CMmsHeaders;
class CMmsEncode;
class CMmsDecode;
class CMmsScheduledEntry;
class CLogClient;
class CLogViewEvent;
class CLogEvent;
class CMmsLog;
class CMmsPhoneClient;

// CLASS DECLARATION

/**
*  Base class for all MMS operations.
*  State machine that contains basic structure for all MMS operations
*  (send, receive, forward, etc.)
*
*  @lib mmssrv.lib
*  @since 2.5
*/
class CMmsBaseOperation : public CMsgActive, public MBackupOperationObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aFs open file system handle
        * @param aMmsSettings pointer to Mms Settings class, values already loaded
        */
        static CMmsBaseOperation* NewL( RFs& aFs, CMmsSettings* aMmsSettings );
        
        /**
        * Destructor.
        */
        virtual ~CMmsBaseOperation();

    public: // New functions
        
        /** 
        * Check if network operations are allowed
        * @return ETrue if network operations are allowed
        *         EFalse if network operations are forbidden
        */
        static TBool NetworkOperationsAllowed();
        
        /** 
        * Allocate 64 bit TID
        * @return TID based on current time
        */
        static TInt64 AllocateTID();
        
        /**
        * Mark failed operation to notification entry 
        * @param reference to the entry where the flags are to be set
        */
        static void MarkNotificationOperationFailed( TMsvEntry& aEntry );

        /**
        * Mark notification as deleted successfully from mmbox 
        * @param reference to the entry where the flags are to be set
        */
        static void MarkNotificationDeletedFromMmbox( TMsvEntry& aEntry );

        /**
        * If a notification is in MMBox folder
        * The corresponding duplicate in Inbox has to be marked and freed.
        * iServerEntry must point to the original notification
        * @param aState tells how the entry should be marked
        *     possible values:
        *     EMmsNotificationOperationFailed - the operation has failed
        *     EMmsDeletedFromMmbox - the message was deleted from MMBox
        * @param reference to a server entry pointing to the original notification
        * Function leaves if the duplicate cannot be accessed (has disappeared already)
        */
        static void MarkDuplicateL( TInt aState, CMsvServerEntry& aServerEntry );
        
        /**
        * Reserve the notification
        * @param reference to the entry where the flags are to be set
        * @param the operation that is to be ongoing
        */
        static void MarkNotificationOperationReserved( TMsvEntry& aEntry, const TUint32 aOperation );

        /**
        * Log date given as seconds from 1.1.1970.
        * @param aDateInSeconds date to be logged
        */        
        static void LogNetworkFormatDateL( const TInt64& aDateInSeconds );
        
        /**
        * Log date given as TTime.
        * @param aDate date to be logged
        */        
        static void LogDateL( const TTime& aDate );

        /**
        * Check if the application is registered
        * @param aApplicationId application id to check
        * @return ETrue if registered, EFalse if not registered
        */
        static TBool RegisteredL( const TDesC& aApplicationId );

        /**
        * Start the state machine.
        * @param aSelection list of message entry ids to be handled.
        * @param aServerEntry CMsvServerEntry pointer from CMmsServer
        * @param aSettings MMSC settings (entry points)
        * @param aService current MMS service id
        * @param aStatus TRequestStatus of the calling active object
        */
        void StartL(
            CMsvEntrySelection& aSelection,
            CMsvServerEntry& aServerEntry,
            TMsvId aService,
            TRequestStatus& aStatus );
            
        /**
        * Check if the message contains any attachment types that must not be sent.
        * @since 3.0
        * @param aServerEntry CMsvServerEntry reference that points to the message
        *     to be checked.
        * @param aFs share protected file session
        * @return KErrNone if the message is clear to send
        * if the message has an attachment the type of which is on the
        *             closed content list
        *         EFalse if the message contains no forbidden content types.
        */
        static TInt CheckClosedContentL( CMsvServerEntry& aServerEntry, RFs& aFs );

    public: // Functions from base classes

        /**
        * From MBackupOperationObserver
        * Called when a backup or restore operation either starts or ends.
    	* @param aBackupOperationAttributes Operation attributes
        */
	    void HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes);

    protected:  // New functions
        
        /**
        * Derived classes may call common constructor
        * @param aMmsSettings pointer to Mms Settings class, values already loaded
        */
        virtual void ConstructL( CMmsSettings* aMmsSettings );

        /**
        * Select next state for state machine
        */
        virtual void SelectNextState();

        /**
        * Switch states
        */
        virtual void ChangeStateL();

        /**
        * Fall through.
        * Base operation for all states that are no-op in some derived classes.
        * This function just completes itself.
        * This function should not be overridden, but should be used by all
        * derived classes when no-op state is required.
        */
        void FallThrough();

        // The following functions are default implementations for all states.
        // The function names end to L to allow derived classes to 
        // override that default functions with leaving functions.
        // All default implementations do not leave.

        /**
        * Initialize common variables.
        * @param aSelection list of message entry ids to be handled.
        * @param aServerEntry CMsvServerEntry pointer from CMmsServer
        * @param aSettings MMSC settings (entry points)
        * @param aService current MMS service id
        */
        virtual void InitializeL(
            CMsvEntrySelection& aSelection,
            CMsvServerEntry& aServerEntry,
            TMsvId aService );

        /**
        * Encode the PDU to be sent to MMSC.
        */
        virtual void EncodePDUL();

        /**
        * Make a network connection to gateway before attempting a transaction.
        * This would normally connect to GPRS or CDMA or WBCDMA access point
        */
        virtual void ConnectToIAPL();

        /**
        * Initialize protocol session.
        */
        virtual void InitializeSessionL();

        /**
        * Submit a transaction (default is POST transaction).
        */
        virtual void SubmitTransactionL();

        /**
        * Create new entry to receive data from MMSC.
        */
        virtual void CreateEntryL();

        /**
        * Decode response PDU from MMSC.
        */
        virtual void DecodeResponseL();

        /**
        * Store response data from MMSC (if not stored during decoding).
        * This function would store e.g. message id, error code from MMSC
        * and response text.
        */
        virtual void StoreResponseL();

        /**
        * Encode acknowledgement message for MMSC (for received message).
        */
        virtual void EncodeAckL();

        /**
        * Send acknowledgement message for MMSC (for received message).
        */
        virtual void SendAckL();

        /**
        * Update the trigger entry status.
        */
        virtual void UpdateEntryStatusL();

        /**
        * Add MMS event to log.
        */
        virtual void LogL();

        /**
        * Move trigger entry after handling.
        * (Move sent item to sent folder, delete notification, etc.)
        */
        virtual void MoveEntryL();

        /**
        * Disconnect and do any other finalizing steps.
        */
        virtual void FinalizeL();

        /**
        * Wait for backup end.
        */
        virtual void WaitForBackupEnd();

        // Utility functions that may be used by all derived classes

       /**
        * Set sending state of scheduled entries
        * @param aSelection entries to handle
        */
        virtual void SetSendMask( const CMsvEntrySelection& aSelection );

        /**
        * Reset sending state of scheduled entries
        * @param aSelection entries to handle
        * @param aError error code
        */
        virtual void UnSetSendMask( CMsvEntrySelection& aSelection, TInt aError);

        /**
        * Reset sending state of scheduled entry
        * @param aId entry id
        * @param aError error code
        */
        virtual void UnSetSendMask( const TMsvId aId, TInt aError );

        /**
        * Save error status
        * @param aError error code
        * @param aScheduledEntry entry to be updated
        */
        virtual void UpdateRecipient(
            TInt aError,
            CMmsScheduledEntry& aScheduledEntry );

        /**
        * Map the response status from MMSC to iError
        * @param aResponse structure where the response code from MMSC is stored
        */
        virtual void MapResponseStatus( CMmsHeaders& aResponse );

        /**
        * Initialize members needed for logging.
        * Called only when logging needed
        * (send message, possibly receive message).
        */
        virtual void InitializeLoggingL();
        
        /**
        * Find duplicate notification
        * @param aParent entry id, under where the duplicate is searched
        * @param aHeaders the original mms headers, whose duplicate is searched
        * @param aDuplicate entry id of the found duplicate
        */
        virtual TInt FindDuplicateNotificationL( TMsvId aParent, CMmsHeaders& aHeaders, TMsvId& aDuplicate );

        /**
        * check if the notification is free to start a new operation 
        */
        virtual TBool FreeNotification( TMsvEntry aEntry, const TUint32 aOperation );
        
        /**
        * Put recipient information and description to log event.
        * The caller may add more information afterwards.
        * @param aMmsHeaders reference to headers where the recipient information can be found
        * @param aEntry reference to entry that contains id and description
        */
        virtual void CommonLogEventInitializationL( CMmsHeaders& aMmsHeaders, TMsvEntry& aEntry );
        
// start of ROAMING CHECK handling
        /**
        * Start the roaming check, will complete asynchronously
        */
        virtual void RoamingCheck();
        
        /**
        * Check result of roaming state query
        */
        virtual void GetRoamingState();
// end of ROAMING CHECK handling


        

    protected:  // Functions from base classes
        
        /**
        * C++ default constructor.
        */
        CMmsBaseOperation( RFs& aFs );

        /**
        * From CMsgActive: Active object completion
        * We must override this, because we may have a selection of
        * entries to handle. If one entry fails, we must be able to
        * continue to the next one ( depending on the error ). At least
        * we must be able to check what to do next. Do some cleanup, and 
        * maybe disconnect.
        */
        virtual void RunL();

        /**
        * From CMsgActive: State machine state operation
        */
        virtual void DoRunL();

        /**
        * From CMsgActive: Active object cancellation
        */
        virtual void DoCancel();

        /**
        * From CMsgActive: Complete current operation.
        * Do whatever cleanup is possible. (Delete incomplete entry etc.)
        * @param aError Error code received by RunL
        */
        virtual void DoComplete( TInt& aError );

    private:

        /**
        * Check connection state after connecting to IAP
        */
        void CheckConnectionState();


    public:     // Data
        // states for the state machine
        enum TMmsOperationStates
            {
            EMmsOperationIdle,
            EMmsOperationCheckingRoaming,
            EMmsOperationEncodingPDU,
            EMmsOperationConnectingToIAP,
            EMmsOperationInitializingSession,
            EMmsOperationSubmittingTransaction,
            EMmsOperationCreatingEntry,
            EMmsOperationDecodingResponse,
            EMmsOperationStoringResponseData,
            EMmsOperationEncodingAck,
            EMmsOperationSendingAck,
            EMmsOperationUpdatingEntryStatus,
            EMmsOperationLogging,
            EMmsOperationMovingEntry,
            EMmsOperationFinalizing,
            EMmsOperationWaitingForBackupEnd
            };
            
        enum TMmsMarkDuplicateState
            {
            EMmsNotificationOperationFailed,
            EMmsDeletedFromMmbox
            };
    
    protected:  // Data
        RFs&                iFs;    // file system
        TInt                iError;
        TInt                iState;
        TMsvId              iService;
        // These three pointers come from the caller, they must not be deleted
        // by this class or any derived class.
        CMsvServerEntry*    iServerEntry;
        CMsvEntrySelection* iSelection; // Array of messages to be handled
        CMmsSettings*       iMmsSettings;

        RSocketServ         iSocketServ;
        RConnection         iConnection;
        CMmsSession*        iMmsSession; 

        // state we must retry in case of backup/restore error
        TInt                iCriticalState;
        // is there an open connection to MMSC
        TBool               iConnected;
        TInt                iCurrentMessageNo;
        // access point array
        CArrayFixFlat<TUint32>* iIAPArray;
        // The number of access point used (in case there are more than one)
        TInt                iConnectionIndex; 
        // Home page URI corresponding to the connection made
        HBufC*              iUri;
        CMmsConnectionInitiator* iConnectionInitiator;
        CMsvEntrySelection* iFailed;          // failed messages
        CMsvEntrySelection* iSuccessful;      // successfully handled messages
        CBufFlat*           iEncodeBuffer;
        CMmsServerEntry*    iEntryWrapper; // entry wrapper for encode and decode
        CMmsHeaders*        iResponse; // the response message from MMSC is decoded here
        // When sending messages encode the outgoing message
        // When receiving messages, encode the response to MMSC
        CMmsEncode*         iEncoder;
        // When sending messages, decode send confirmation from MMSC
        // When receiving messages, decode the incoming message
        CMmsDecode*         iDecoder;
        TBool               iBackupStart;
        TBool               iBackupEnd;
        RTimer              iTimer;     // timer for backup operations - we don't wait forever
        TBool               iHaveTimer;  // have we created the local timer
        // we wait only once - otherwise we might run into an endless loop
        TBool               iDoNotWaitForBackupEnd;
        TBool               iMustWait;   // small delay
        CLogClient*         iLogClient; // For updating log
        CLogViewEvent*      iLogViewEvent; // log view
        CLogEvent*          iLogEvent;
        TLogString          iLogString;
        CMmsLog*            iMmsLog;
        CDesCArray*         iRemoteParties;  // Pure addresses to log
        TInt                iNumberOfRemoteParties;
// start of ROAMING CHECK handling
// Roaming check variables included in base operation
// Only those operations that actually need roaming chech implement the actual handling
// Base class will default to "home" without actually checking anything
        TInt                iRegistrationStatus; // 0 = home network
        CMmsPhoneClient*    iPhone;
// end of ROAMING CHECK handling


        
    private:    // Data
         
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // MMSBASEOPERATION
            
// End of File
