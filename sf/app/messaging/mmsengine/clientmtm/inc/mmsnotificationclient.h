/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     A Client MTM to access MMS notifications in a manual fetch mode.
*     All this is needed to support new mtm type for Symbian OS messaging
*     UI components.
*
*/



#ifndef MMSNOTIFICATIONCLIENTMTM
#define MMSNOTIFICATIONCLIENTMTM

//  INCLUDES
#include <mmsclient.h>

// CONSTANTS

// MACROS

// DATA TYPES

typedef struct {
    TInt mmboxTotalInBytes;         // quota used in mmbox, in bytes
    TInt mmboxTotalInMessageCount;  // quota used in mmbox, number of messages
    TInt mmboxQuotaInBytes;         // quota defined for user's mmbox, in bytes
    TInt mmboxQuotaInMessageCount;  // quota defined for user's mmbox, number of messages 
    TTime date;                     // date and time when the mmbox is updated
    TInt error;                        // error about last mmbox update
    }TMmboxInfo;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Client Mtm for MMS Notifications
*
*/
class CMmsNotificationClientMtm : public CMmsClientMtm
    {
    public:  // Constructors and destructor

        
        IMPORT_C static CMmsNotificationClientMtm* NewL(
            CRegisteredMtmDll& aRegisteredMtmDll,
            CMsvSession& aSession );

        /**
        * Destructor.
        */
        ~CMmsNotificationClientMtm();        

    public: // New functions

        /**
        * Send current forward entry (has to be current context)
        * @param aCompletionStatus iStatus member of an active object.
        *     Will be set as completed when the request has finished.
        * @param aSendingTime time at which the message selection is to be sent
        *     given as local time. If aSending time is zero or in the past, the
        *     message is scheduled to be sent as soon as possible.
        *     Defaults to zero meaning immediate sending.
        * @return pointer to an operation active object.
        *     If successful, this is an asynchronously completing operation.
        *     In failure, a completed operation with status set to the relevant
        *       error code will be returned.
        *     In case the notification is already in use (there is
        *       an operation ongoing) a completed operation with status set KErrInUse
        *       is returned.
        *       If the entry is reserved, the progress of the completed operation
        *       will contain the id of the related entry that was reserved
        *     The returned completed operation also completes the status of the caller
        *       so there is no difference between the operation completing immediately
        *       with an error or starting and then completing with error.
        */
        virtual CMsvOperation* SendL( 
            TRequestStatus& aCompletionStatus,
            const TTime aSendingTime = TTime( 0 ) );

        /**
        * Returns possible extension text related to the notification
        * @return descriptor containing the text extension in the notification
        *     empty descriptor if extension does not exist
        */
        virtual const TPtrC GetExtendedText() const;

        /**
        * Lists all MMS Notifications, that have no active operation, 
        * for unfetched messages.<br>
        * Caller must delete the array when no longer needed.<br>
        * The notifications have similar format as messages, but they
        * contain only MMS headers, and no attachments.<br>
        * Individual fields can be queried the same way as for messages.
        * @return Array of entry IDs of MMS Notifications.
        */
        virtual CMsvEntrySelection* ListNotificationsL();

        /**
        * DeleteNotificationL deletes selected notifications.
        * @param aSelection contains list of notifications to be deleted
        * @param aDeleteType specifies which type of delete operation is done:
        *   -EMmsDeleteNotificationOnly: only notification entry from Inbox is deleted 
        *   -EMmsDeleteMMBoxOnly: only corresponding message from network is deleted
        *   -EMmsDeleteBoth: both are deleted
        * @param aCompletionStatus iStatus member of an active object.
        *     It will be set as completed when the request has finished.
        * @return pointer to an operation active object.
        *     If successful, this is an asynchronously completing operation.
        *     If failed, this is a completed operation, with status set to
        *     the relevant error code.
        */
        virtual CMsvOperation* DeleteNotificationL( 
            const CMsvEntrySelection& aSelection,
            TMmsDeleteOperationType aDeleteType,
            TRequestStatus& aCompletionStatus );

        /**
        * UnscheduledDeleteNotificationL
        * EXACTLY SAME AS DeleteNotificationL ABOVE, BUT:
        * This version performs unscheduled delete meaning that the returned
        * operation will not complete before the whole operation has been executed.
        * (Previous version completes as soon as MessageServer has scheduled
        * the request)
        */
        virtual CMsvOperation* UnscheduledDeleteNotificationL(
            const CMsvEntrySelection& aSelection,
            TMmsDeleteOperationType aDeleteType,
            TRequestStatus& aCompletionStatus );

        /**
        * DeleteAllNotificationsL deletes all the notifications found from Inbox
        * @param aDeleteType specifies which type of delete operation is done:
        *   -EMmsDeleteNotificationOnly: only notification entry from Inbox is deleted 
        *   -EMmsDeleteMMBoxOnly: only corresponding message from network is deleted
        *   -EMmsDeleteBoth: both are deleted
        * @param aCompletionStatus iStatus member of an active object.
        *     It will be set as completed when the request has finished.
        * @return pointer to an operation active object.
        *     If successful, this is an asynchronously completing operation.
        *     If failed, this is a completed operation, with status set to
        *     the relevant error code.
        */
        virtual CMsvOperation* DeleteAllNotificationsL( 
            TMmsDeleteOperationType aDeleteType,
            TRequestStatus& aCompletionStatus );

        /**
        * DeleteForwardEntryL deletes the given selection of forward entries.
        * Caller should make sure that all the entries in the selection are:
        * - either in Outbox or Sent Folder
        * - forward entry created earlier through this API
        * Entries that do not meet these requirements, are left untouched.
        * After the call, current context points to forward entry parent (outbox 
        *   or sent folder)
        * Note: that forward request entries should not be deleted directly e.g.
        *   with CMsvEntry, because of the possible locked notification in Inbox.
        *   This method handles this notification.
        * @param aSelection contains list of forward entries to be deleted
        */
        virtual void DeleteForwardEntryL( const CMsvEntrySelection& aSelection );

        /**
        * Fetch MMS messages referred by the notification selection 
        * from current MMSC to inbox.<br>
        * If aSelection is empty, the function leaves.
        *
        * If the aSelection contains only entries that are not notifications or
        * are notifications that are not allowed to start a new operation, 
        * The fetch is not started. The function leaves. 
        * 
        * Only real free notifications are fetched. 
        * No error code is returned, if some of the notifications are not fetched and there are 
        * notifications to be fetched.
        *
        * @param aSelection selection of notification entries.<br>
        * @param aCompletionStatus iStatus member of an active object.<br>
        *     It will be set as completed when the request has finished.
        * @return pointer to an operation active object.<br>
        *     If successful, this is an asynchronously completing operation.<br>
        *     If failed, this is a completed operation, with status set to
        *     the relevant error code.
        */
        virtual CMsvOperation* FetchMessagesL( 
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus );

        /**
        * MmBoxInfoL gives info about MMBox, if available.
        * @param aMmboxInfo struct about mmboxInfo
        * @return 
        *     ETrue, if info is available.
        *     EFalse, if info is not available.
        */

        virtual TBool MmboxInfoL( TMmboxInfo& aMmboxInfo );


        /**
        * UpdateMmBoxListL updates the list of notifications available
        * in the MMBox.
        * @param aCompletionStatus iStatus member of an active object.
        *     It will be set as completed when the request has finished.
        * @return pointer to an operation active object.
        *     If successful, this is an asynchronously completing operation.
        *     If failed, this is a completed operation, with status set to
        *     the relevant error code.
        */
        virtual CMsvOperation* UpdateMmBoxListL(
            TRequestStatus& aCompletionStatus );

        /**
        * Get the Mmbox folder
        * @return  
        *     Id of the mmbox folder
        */
        virtual TMsvId GetMmboxFolderL();

        /**
        * Get number of MMS Notifications for unfetched messages that 
        * have no active operation.<br>
        * If the receiving of Multimedia Messages has been turned off,
        * there may be a number of notifications waiting for processing.<br>
        * When the receiving of messages is turned on again, the UI may want
        * to check the number of pending notifications before giving the
        * "FetchAll" command.
        * @return Number of MMS notifications waiting for processing
        */
        virtual TInt NotificationCount();
        
        
        /**
        * Get the value of application id header if present in the notification.
        *
        * @since 3.2
        */
        virtual const TPtrC GetApplicationId() const;
        

    public: // Functions from base classes

        /**
        * Creates an entry representing forward request based on 
        * the notification being the current context.
        * @param aDestination refers to the folder where the entry is created
        * @param aPartlist NOT USED
        * @param aCompletionStatus reference to the status of an active object.
        *     This status will be set as completed when the operation completes.
        * @return Pointer to message server operation (active object).
        *     The progress information provides the id of the created message 
        *     when the message has been created in a 8-bit descriptor. 
        *     While the operation is in progress the package will contain a null 
        *     id (KMsvNullIndexEntryId). If there was an error while creating 
        *     the message, then the message will be deleted and the package will 
        *     contain null id.
        */
        virtual CMsvOperation* ForwardL( 
            TMsvId aDestination,
            TMsvPartList aPartList,
            TRequestStatus& aCompletionStatus );

        /**
        * Not supported. 
        * Implementation of the base class is overridden.
        */
        virtual CMsvOperation* SendL(
            CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus,
            TTime aSendingTime );

        /**
        * Fetch the multimedia message of the notification from current MMSC to inbox.<br>
        * @param aId Entry ID of a notification that refers to an
        *     unfetched message.
        * @param aCompletionStatus iStatus member of an active object.<br>
        *     It will be set as completed when the request has finished.
        * @return pointer to an operation active object.<br>
        *     If successful, this is an asynchronously completing operation.<br>
        *     If failed, this is a completed operation, with status set to
        *     the relevant error code.
        */
        virtual CMsvOperation* FetchMessageL(
            TMsvId aId,
            TRequestStatus& aCompletionStatus );
        
        /**
        * Fetch multimedia messages for all notifications that are free to be fetched
        * in Inbox from current MMSC to inbox.<br>
        * If the Inbox does not contain any notification, the function leaves.
        * @param aCompletionStatus iStatus member of an active object.<br>
        *     It will be set as completed when the request has finished.
        * @aparam aForced indicates if the messages should be fetched regardless
        *     or current mode settings.
        *     ETrue = user initiated fetch, use override
        *     EFalse = event triggered fetch, fetch only if settings allow.
        * @return pointer to an operation active object.<br>
        *     If successful, this is an asynchronously completing operation.<br>
        *     If failed, this is a completed operation, with status set to
        *     the relevant error code.
        */
        virtual CMsvOperation* FetchAllL( TRequestStatus& aCompletionStatus,
            TBool aForced = ETrue  );

        /**
        * QueryCapability 
        */
        virtual TInt QueryCapability( TUid aCapability, TInt& aResponse );
        
    protected:  // New functions
      

    protected:  // Functions from base classes
        

    private:

        /**
        * By default Symbian OS constructor is private.
        * @param aRegisteredMtmDll Reference to Mtm Dll registry class
        * @param aSession Reference to a Message Server session.
        */
        CMmsNotificationClientMtm(
            CRegisteredMtmDll& aRegisteredMtmDll,
            CMsvSession& aSession );  

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Creates an entry representing forward request based on 
        * the notification being the current context.
        * @param aDestination refers to the folder where the entry is created
        * @return entry id of the created forward request entry
        */
        TMsvId CreateForwardEntryL( const TMsvId aDestination );

        /**
        * ReserveNotificationOperationL
        * method first tests whether an operation is allowed or not.
        * If allowed, further operations are not possible
        * If not allowed, error is returned
        * @param aNotifId pointing to a notification entry
        * @param aOperation (fetching, forwarding or deleting)
        * @return Errorcode:
        *     KErrNone if operation was allowed and is now reserved for
        *     this application
        *     KErrInUse if notification already has an operation ongoing
        */
        TInt ReserveNotificationOperationL( const TMsvId aNotifIf, const TUint32 aOperation );
        
        /**
        * MarkFreeNotificationsReservedL
        * reserves notifications 
        * aSelection contains only reserved notifications, others are dropped out.
        * @param aSelection selection of notification entries
        * @param aOperation (fetching, forwarding or deleting)
        * @return 
        */

        void MarkFreeNotificationsReservedL( CMsvEntrySelection& aNotifications, const TUint32 aOperation );

        /**
        * check if the notification is free to start a new operation 
        */
        TBool FreeNotification( TMsvEntry& aEntry, const TUint32 aOperation );

        /**
        * Reserve the notification
        */
        void MarkNotificationOperationReserved( TMsvEntry& aEntry, const TUint32 aOperation );

        /**
        * Find duplicate notification
        * @param aParent entry id, under where the duplicate is searched
        * @param aHeaders the original mms headers, whose duplicate is searched
        * @param aDuplicate entry id of the found duplicate
        */
        void FindDuplicateNotificationL( TMsvId aParent, CMmsHeaders& aHeaders, 
                                                 TMsvId& aDuplicate);

    public:     // Data
    
    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

#endif // MMSNOTIFICATIONCLIENTMTM   
            
// End of File
