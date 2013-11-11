/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     State machine for message receiving
*
*/



#ifndef MMSRECEIVEMESSAGE_H
#define MMSRECEIVEMESSAGE_H

//  INCLUDES
#include <mentact.h>
#include <mtsr.h>
#include <s32mem.h>
#include <es_sock.h>
#include <etel.h>
#include <etelmm.h>

//#include "mmsoperation.h"
#include    "mmsbaseoperation.h"

// CONSTANTS
enum TMmsSettingsFetchMode
    {
    // Modes that send acknowledgements to MMSC

    // Fetch everything that fits criteria, discard rest
    // (too big messages, advertisements etc.)
    // Fetching is tried until message expires.
    // Retry interval may be lengthened in some cases.
    EMmsMinimumFetchingMode = 1,
    EMmsFetchingOn = EMmsMinimumFetchingMode,

    // Send a reject response to everything
    EMmsFetchingOff = EMmsFetchingOn + 1,

    // Send a deferred response to everything until the
    // user switches fetching mode on again.
    // At that point fetch everything that has not expired
    EMmsFetchingDeferred = EMmsFetchingOff + 1,

    // change this is modes added
    EMmsMaximumFetchingMode = EMmsFetchingDeferred
    };

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMmsHeaders;
class CMmsDecode;
class CMmsEncode;
class CMmsPhoneClient;

// CLASS DECLARATION

/**
*  State machine for handling message fetching.
*  Calls CMmsTransaction and CMmsDecode to do the dirty work
*/
//class CMmsReceiveMessage :public CMsgActive
class CMmsReceiveMessage :public CMmsBaseOperation
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor is private
        */

        /**
        * Two-phased constructor.
        */
        static CMmsReceiveMessage* NewL( RFs& aFs, CMmsSettings* aMmsSettings  );
        
        /**
        * Destructor.
        */
        virtual ~CMmsReceiveMessage();

    public: // New functions
        
        /**
        * Start the state machine.
        * @param aSelection list of notification ids referring to
        *     messages to be fetched. <br>
        *     If the code decides that a message will not be fetched,
        *     it will send a "Reject" or "Deferred" response to MMSC
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
        * Get list of successfully received messages.
        * Actually this is a list of notifications corresponding to successful message
        */
        inline CMsvEntrySelection& Received() const;

        /**
        * Get list of failed messages
        */
        inline CMsvEntrySelection& Failed() const;

        /**
        * Get list of bad notifications
        * These should be deleted. The scheduling information should
        * be removed first. After the scheduling has been finalized,
        * this function may disappear, if it turns out that we can
        * just delete these entries, and the schedules disappear 
        * automatically.
        */
        inline CMsvEntrySelection& BadNotifications() const;

        // Tells if we in home network or in foreign.
        inline TBool InForeignNetwork() const;

    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * Default constructor
        */
        CMmsReceiveMessage( RFs& aFs );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CMmsSettings* aMmsSettings );

        // By default, prohibit copy constructor
        CMmsReceiveMessage( const CMmsReceiveMessage& );
        // Prohibit assignment operator
        CMmsReceiveMessage& operator= ( const CMmsReceiveMessage& );

        /**
        * From CMsgActive: Complete current operation.
        * Do whatever cleanup is possible. (Delete incomplete entry etc.)
        * @param aError Error code received by RunL
        */
        void DoComplete(TInt& aStatus);

        /**
        * Decode one message
        */
        void DecodeResponseL();

        /**
        * Connect to Internet Access Point before connecting to WAP Gateway
        */
        void ConnectToIAPL();

        /**
        * Connect to MMSC
        */
        void InitializeSessionL();

        /**
        * Receive one message from MMSC
        */
        void SubmitTransactionL();
        
        /**
        * Check if we connection was successful for the next transaction.
        * @return
        * - ETrue if successfully connected and can continue
        * - EFalse if not connected (should terminate state machine)
        */
        TBool IsConnected();
        
        /**
        * Load the notification entry and clear old status if needed
        * @param aEntry Index entry of the notification (filled with data)
        * @return
        * - ETrue if processing can continue with this notification
        * - EFalse if processing should end.
        * If processing is discontinued, caller must return.
        * Depending on error the state machine goes to next state (if set active)
        * or terminates (not set active)
        */
        TBool LoadNotificationL( TMsvEntry& aEntry );
        
        /**
        * Check if the message has expired or if it can be fetched
        * In manual mode the fetch will continue even if the message
        * has expired. It is up to the user if he wants to try to fetch
        * expired messages.
        * @return
        * - ETrue continue operation with this notification
        * - EFalse message has expired and no response will be sent
        *   The function has changed state and the caller should just return.
        */
        TBool CheckExpirationL();
        
        /**
        * Checks notification sanity.
        * If MMS major version number or message type is incorrect,
        * notification status will be set to "Unrecognized"
        * @return
        * - ETrue notification is insane
        * - EFalse notification is sane, and further checks can be made
        */
        TBool IsNotificationInsaneL();
        
        /**
        * Check is the message is not fetched
        * Response should be sent anyway´
        * @param aEntry notification index entry
        * @return
        * - ETrue if the message will be fetched
        * - EFalse if the message is fetched, only response sent
        *   The function will change state as needed, caller must return
        */
        TBool WantToFetchThisL( TMsvEntry& aEntry );

        /**
        * Create entry into inbox to receive the new message.
        * Called in chunked mode.
        * Does not complete the caller.
        */
        void DoCreateEntryL();

        /**
        * Finalize the new entry
        */
        void StoreResponseL();

        /**
        * Send notify response or acknowledge indication
        */
        void SendAckL();

        /**
        * Check if we sent the response successfully.
        * If not successful, we may need to requeue the notification.
        * Move entry to inbox if needed.
        */
        void UpdateEntryStatusL();

        /**
        * Encode a notify response
        */
        void EncodeNotifyResponseL();

        /**
        * Encode Acknowledge Indication
        */
        void EncodeAcknowledgeIndicationL();


// start of ROAMING CHECK handling
        /**
        * Start the roaming check, will complete asynchronously
        */
        void RoamingCheck();
        
        /**
        * Check result of roaming state query
        */
        void GetRoamingState();
// end of ROAMING CHECK handling

        /**
        * Handle messages that are type multipart alternative.
        * If text/plain part is found, it is retained, others are deleted.
        */
        void HandleMultipartAlternativeL();

        /**
        * Check the fate of the notification.
        * As Receive message has no PDU to send, we check the fate of the
        * notification in this state. If the state indicates that the 
        * entry must not be fetched, we clean up this entry and loop
        * to the next.
        */
        void EncodePDUL();
        
        /**
        * Gets a folder entry id with 'aFolderName' name under a parent folder. 
        * @param aParentFolder a folder, under where the folder is searched
        * @param aFolderName the name of the searched folder entry
        * @param aFolderId the entry id of the searched folder 
        * @return  KErrNone if successful
        */
        TInt FolderEntryL( TMsvId aParent, const TDesC& aFolderName, TMsvId& aFolderId );
        
        /**
        * Creates a folder entry id with 'aFolderName' name under a parent folder.
        * If the folder already exists, its folder id is returned. 
        * @param aParentFolder a folder, under where the folder will be created
        * @param aFolderName the name of the folder entry to be created
        * @param aFolderId the entry id of the created folder 
        * @return  KErrNone if successful
        */
        TInt CreateFolderEntryL( TMsvId aParentFolder, const TDesC& aFolderName, TMsvId& aFolderId );
        
        /**
        * Find duplicate notification
        * @param aParent entry id, under where the duplicate is searched
        * @param aHeaders the original mms headers, whose duplicate is searched
        * @param aDuplicate entry id of the found duplicate
        */
        TInt FindDuplicateNotificationL( TMsvId aParent, 
            CMmsHeaders& aHeaders, TMsvId& aDuplicate );
            
        /**
        * Get the message from a file in local mode
        */
        void LocalModeFetchL();
        
        /**
        * Dump the incoming message into a file if there has been an error.
        * Dumping can be done only when logging is enabled.
        */
        void DumpIfNeededL();
        
        /**
        * Close the message file used in local mode
        * aDeleteFile 
        */
        void CloseLocalFileL( TBool aDeleteFile );
        
        /**
        * If the message contains an application id which is registered, move the
        * message to the corresponding folder. The corresponding folder is under 
        * application folder. There may be several folders under application folder. 
        * @param aEntry index entry of the message to be handled
        * @return status code that tells if the message was intended
        *     for an unregistered applicatio or successfully moved to an
        *     application folder.
        *     Return value 0 denotes normal message.
        */
        TInt MoveToApplicationFolderIfNeededL( TMsvEntry& aEntry );
            
        /**
        * Skip inaccessible notification.
        * The notification remains in failed list.
        * The state machine winds back to start
        */
        void SkipEntryL();
        
        /**
        * Entry has been handled. Loop to next entry or finalize the whole loop
        */
        void LoopToNextEntryL();
        
        /**
        * The state of duplicate notification is cleared in case of fatal failure.
        *     iServerEntry must point to the notification in question.
        * 
        */
        void ClearDuplicateEntryOperationL();
        
        /**
        * Marks info inton notification and clear duplicate
        * @param aApplicationMessageStatus status that tells if the message
        *    was addressed to an application.
        */
        void ClearOperationL( TInt aApplicationMessageStatus );
        
        /**
        * Copy missing info from notification to message.
        * If we have received a message that contains only status code
        * we copy fields like sender and subject from notification
        * to the message to retain some relevant information.
        */
        void CopyDataFromNotificationToMessageL();
        
        /**
        * Copy status code and application id from message to notification.
        * If we get a message with transient error, we copy the status data
        * to the notification before retry (to inform user about the cause
        * of the problem).
        * We also copy the application id in case the notification does not
        * already contain it. This may inform the user why the message went
        * to some application instead of appearing in the inbox (manual mode)
        */
        void CopyDataFromMessageToNotificationL();
        
        /**
        * Map the error status received from MMSC into an internal error code
        * @param aErrorStatus status from MMS headers
        * @return ETrue if the error is permanent, EFalse if the error is transient
        */
        TBool MapErrorStatus( const TInt32 aErrorStatus );
        
        /**
        * Set the bits to index entry to complete a received message
        * @param aEntry the index entry of the received message
        */
        void SetIndexEntryBitsForReceivedMessage( TMsvEntry& aEntry );
        
        /**
        * Try to free space by deleting older messages for the current application.
        * If memory can be freed iError is changed from KErrNoDisk to
        * KMmsErrorApplicationDiskFull. The operation will be rescheduled in
        * automatic mode. In manual mode the user must restart the fetch.
        */
        void DeleteApplicationMessagesL();
        

    public:     // Data
    
    protected:  // Data

    private:    // Data
        RFile iFile; // local mode message

        CMmsHeaders*        iMmsHeaders;      // Headers from incoming message
        CMmsHeaders*        iNotification;    // Headers forming the notification
        TBool               iAlreadyDeferred; // deferred response has already been sent
        CMsvEntrySelection* iBad;             // bad notifications

        TMsvId              iEntryUnderConstruction;
        TMsvId              iNotificationParent;

        // Buffer for the incoming message.
        TInt                iMessageDrive; // messages are on C: drive by default,
        TBool               iFileOpen;     // if file is not open, do not close

        TMmsReceivingMode   iReceivingMode; // set to correct mode depending on network
        // tells if an application id has been registered or not
        TBool               iRegistered;
                
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#include "mmsreceivemessage.inl"

#endif      // MMSRECEIVEMESSAGE_H   
            
// End of File
