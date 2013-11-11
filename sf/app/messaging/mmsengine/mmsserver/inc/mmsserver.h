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
*     Server Mtm
*
*/



#ifndef MMSSERVER_H
#define MMSSERVER_H

//  INCLUDES
#include <mtsr.h>
#include <msvipc.h>
#include <schedulebaseservermtm.h>
#include "mmsconst.h"

// CONSTANTS
// approximate amount of disk space needed for scheduling
// will be multiplied by the number of entries to be scheduled
const TInt KMmsTaskSpace = 80;
// c:\Private\1000484b\mmsvar
_LIT( KMmsMessageVariationDirectory, "\x43:\\Private\\1000484B\\mmsvar\\" ); 

// MACROS
// this definition allows logging of notifications
// and received messages.
//#define NOTIFICATION_LOGGING

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMmsServerEntry;
class CMmsSettings;
class CMmsSendOperation;
class CMmsReceiveMessage;
class CMmsForwardOperation;
class CMmsDeleteOperation;
class CMmsMmboxList;
class CMmsDecode;
class CMmsHeaders;
class CLogEvent;
class CMmsLog;
class RScheduler;
class CMmsReadReport;

// CLASS DECLARATION

/**
*  Server Mtm for multimedia messaging subsystem.
*  This component will take care of message sending and receiving
*/
class CMmsServerMtm : public CScheduleBaseServerMtm
    {
    public:  // Constructors and destructor
        // constructor is private.

        /**
        * Factory function.
        * The only function exported by this polymorphic interface dll.<br>
        * This function is called by message server when a client asks for
        *     some service from this mtm type
        * @param  aRegisteredMtmDll Reference to Mtm Dll registry class
        * @param  aInitialEntry Service entry. Can be the first entry in
        *     CMsvSelection array in the call from the client, or if
        *     there is no service entry as the first item of the selection,
        *     this will be the default service entry for this type of mtm.
        *     If no default entry has been specified, and the first entry
        *     in the selection is not a service entry, the creation of 
        *     server mtm will fail. 
        * @return Pointer to CMmsServerClass. 
        */
        IMPORT_C static CMmsServerMtm* NewL(
            CRegisteredMtmDll& aRegisteredMtmDll,
            CMsvServerEntry* aInitialEntry );

        /**
        * Destructor.
        */
        ~CMmsServerMtm();

    public: // New functions
        
    public: // Functions from base classes

        // Copy and move, these are actually designed for email.
        // They can be used for other purposes, if seem fit

        // Miscellaneous copy, move and other entry handling functions
        // are not supported in increment 1.

        /**
        * From CBaseServerMtm: Copy messages from remote service to
        *     local folders
        * @param aSelection list of TMsvIds of messages. The first is the service
        * @param aDestination TMsvId of the destination folder (for example inbox)
        * @param aStatus the status of the active object that is using this
        *     function as asynchronous service provider. This status will be
        *     set as completed when the server mtm active object is done.
        */
        void CopyToLocalL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm: Copy messages from local folder to
        *     remote service
        * @param aSelection list of TMsvIds of messages. The first is the service
        * @param aDestination TMsvId of the destination folder
        * @param aStatus the status of the active object that is using this
        *     function as asynchronous service provider. This status will be
        *     set as completed when the server mtm active object is done.
        */
        void CopyFromLocalL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm: Copy messages
        * @param aSelection list of TMsvIds of messages. The first is the service
        * @param aStatus the status of the active object that is using this
        *     function as asynchronous service provider. This status will be
        *     set as completed when the server mtm active object is done.
        */
        void CopyWithinServiceL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm: Move messages from remote service to
        *     local folders
        * @param aSelection list of TMsvIds of messages. The first is the service
        * @param aDestination TMsvId of the destination folder (for example inbox)
        * @param aStatus the status of the active object that is using this
        *     function as asynchronous service provider. This status will be
        *     set as completed when the server mtm active object is done.
        */
        void MoveToLocalL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm: Move messages from local folder to
        *     remote service
        * @param aSelection list of TMsvIds of messages. The first is the service
        * @param aDestination TMsvId of the destination folder
        * @param aStatus the status of the active object that is using this
        *     function as asynchronous service provider. This status will be
        *     set as completed when the server mtm active object is done.
        */
        void MoveFromLocalL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm: Move messages
        * @param aSelection list of TMsvIds of messages. The first is the service
        * @param aDestination TMsvId of the destination folder.
        * @param aStatus the status of the active object that is using this
        *     function as asynchronous service provider. This status will be
        *     set as completed when the server mtm active object is done.
        */
        void MoveWithinServiceL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );

        // Create, change, delete
        
        /**
        * From CBaseServerMtm: delete specified entries.
        * @param aSelection entries to be deleted
        * @param aStatus the status of the active object that is using this
        *     function as asynchronous service provider. This status will be
        *     set as completed when the server mtm active object is done.
        */
        void DeleteAllL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm: Create a new entry.
        * @param aNewEntry Basic settings for the new entry.
        * @param aStatus the status of the active object that is using this
        *     function as asynchronous service provider. This status will be
        *     set as completed when the server mtm active object is done.
        */
        void CreateL( TMsvEntry aNewEntry, TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm: Change an entry.
        * @param aNewEntry New settings for the entry.
        * @param aStatus the status of the active object that is using this
        *     function as asynchronous service provider. This status will be
        *     set as completed when the server mtm active object is done.
        */
        void ChangeL( TMsvEntry aNewEntry, TRequestStatus& aStatus );
        
        // command and progress

        /**
        * From CBaseServerMtm: Execute any MTM specific asynchronous command.
        * @param aSelection List of TMsvIds of messages. The first is the service
        * @param aCommand The command to be executed
        * @param aParameter Any special parameter the specified command needs
        * @param aStatus the status of the active object that is using this
        *     function as asynchronous service provider. This status will be
        *     set as completed when the server mtm active object is done.
        */
        void StartCommandL(
            CMsvEntrySelection& aSelection,
            TInt aCommand,
            const TDesC8& aParameter,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm: Before releasing the server mtm, the message
        *     server can query, if the mtm is expecting another command soon,
        *     so that unloading should be delayed.
        * @return ETrue if more commands are expected, EFalse otherwise
        */
        TBool CommandExpected();
        
        /**
        * From CBaseServerMtm: Accessor to progress of current operation.
        * @return progress value
        */
        const TDesC8& Progress();

        /**
        * From CScheduleBaseServerMtm: Load default resource file
        */
        void LoadResourceFileL();

        /**
        * From CScheduleBaseServerMtm: Put data into schedule package
        * @param aParameter
        * @param aPackage
        */
        void PopulateSchedulePackage(const TDesC8& aParameter, const TBool aMove, TMsvSchedulePackage& aPackage) const;
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
        /**
        * From CActive: Cancel current operation.
        */
        void DoCancel();

        /**
        * From CBaseServerMtm: Do work.
        */
        void DoRunL();

        /**
        * From CBaseServerMtm: Complete current operation.
        * @param aError Error code to be returned as final status
        */
        void DoComplete( TInt aError );

        /**
        * From CScheduleBaseServerMtm: Restore scheduling settings
        */
        void RestoreScheduleSettingsL(TBool aRestoreErrorsFromResource = EFalse, TInt aErrorsResourceId = 0 );

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        CMmsServerMtm(
            CRegisteredMtmDll& aRegisteredMtmDll,
            CMsvServerEntry* aInitialEntry );

        /**
        * construct function that may leave
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CMmsServerMtm( const CMmsServerMtm& );
        // Prohibit assigment operator
        CMmsServerMtm& operator= ( const CMmsServerMtm& );

        /**
        * Loads settings (also schedule settings).
        * Sets service entry.
        * @return error code
        */
        void LoadSettingsL( TInt aCommand );

        /**
        * Send current selection to MMSC using current service.
        */
        void SendToMmscL();

        /**
        * Send current selection to MMSC using current service and
        * CMmsForwardMessage operation class.
        */
        void SendForwardRequestsToMmscL();

        /**
        * Fetch messages from MMSC using current service.
        */
        void FetchFromMmscL();

        /**
        * Check which entries have failed an operation
        * and rescedule the failed ones.
        */
        void UpdateEntriesL();
        
        /**
        * Reschedule entries for which forward has failed
        * @param aPackage schedule package needed in rescheduling
        */
        void HandleFailedForwardsL( TMsvSchedulePackage& aPackage );
        
        /**
        * Clean schedule for successfully sent entries
        */
        void HandleSuccessfulSendsL();

        /**
        * Reschedule entries for which sending has failed
        * @param aPackage schedule package needed in rescheduling
        */
        void HandleFailedSendsL( TMsvSchedulePackage& aPackage );
        
        /**
        * Cleanup after message generation.
        */
        void CleanupAfterMessageGenerationL();
        
        /**
        * Handle entries for which receiving has failed
        * @param aPackage schedule package needed in rescheduling
        * @param aFatalError an error that must be returned to caller
        * @return
        * - ETrue if fetching must be restarted immediately
        * - EFalse if normal scheduling or failing is wanted
        */
        TBool HandleFailedReceivesL( TMsvSchedulePackage& aPackage, TInt& aFatalError );
        
        /**
        * Clean up successfully fetched entries
        */
        void HandleSuccessfulReceivesL();

        /**
        * Clean up bad notification entries
        */
        void HandleBadNotificationsL();

        /**
        * Reschedule unhandled delivery reports
        * @param aPackage schedule package needed in rescheduling
        */
        void UpdateDeliveryReportsL( TMsvSchedulePackage& aPackage );
        
        /**
        * Handke read reports whose sending has failed
        */
        void HandleFailedReadReports();


        /**
        * Make dates for scheduled entries identical.
        * This is needed at least in debug version, because
        * ScheduleL does want to assert that the dates of all
        * the entries are identical. <br>
        * We don't care and would send anything, but Symbian OS doesn't
        * allow us to.
        * @param aSelection the entries to be handled
        * @param aInterval the delay between NOW and the actual operation.
        *     Must be positive, and greater than KMmsDelayInSeconds,
        *     otherwise KMmsDelayInSeconds will be used, because 
        *     the task scheduler will not handle tasks that are overdue
        * @param aClearError specifies if the old error from the entry
        *     should be cleared. Clearing is needed to prevent
        *     MsgErrorWatcher from reacting to the same error twice
        */
        void MakeDatesIdenticalL( CMsvEntrySelection& aSelection,
            TTimeIntervalSeconds aInterval, TBool aClearError = EFalse );

        /**
        * Decode the data content of a pushed message.
        * The content should be either a MMS notification or a
        * delivery report.
        * If the content is a MMS notification, the message must
        * be scheduled for fetching.
        */
        void DecodePushedMessageL();

        /**
        * Schedule fetching of message referred to by this notification.
        * The decoded message is stored in iMsvSelection.
        */
        void HandleNotificationL();

        /**
        * Schedule a delivery report for logging
        * The decoded message is stored in iMsvSelection.
        */
        void HandleDeliveryReportL();

        /** 
        * Update the contents of message log from the delivery report.
        * delete the handled delivery report.
        */
        void LogDeliveryReportL();

        /**
        * Check if there are notifications that have identical URI.
        * If yes, this is a duplicate, and should be discarded
        * @param aParent folder where the notifications are stored
        * @param aNotification MMS headers for the new notification for
        *     identity check.
        * @return ETrue if the notification is a duplicate and should be discarded
        *     EFalse if a duplicate has not been found
        */
        TBool PruneDuplicateNotificationsL( TMsvId aParent, CMmsHeaders& aNotification );

        /**
        * Checks the fate of existing notifications.
        * Checks if the notifications have expired, and if 
        * they have, they are deleted.
        * Also checks, if the notifications have failed the
        * maximum number of times. The failed notifications
        * are removed.
        * @param aSelection reference to a TMSVId list. At return it
        *     contains any notifications that still need rescheduling.
        *     If the list is empty, notifications from mmsFolder are added
        *     and checked.
        */
        void CheckNotificationsL( CMsvEntrySelection& aSelection );

        /**
        * Fake function for testing purposes.
        * Creates notifications containing file paths.
        */
        void CreateNotificationsL();

        /**
        * Create notification into member buffer
        * Fake function for testing purposes.
        * @param aUrl address of the message (path & filename)
        * @param aSize size of the message (file)
        */
        void CreateNotificationL( TDesC8& aUrl, TInt aSize );

        /**
        * Do the actual garbage collection
        * This means: If there are entries in outbox, check if
        * they have been sent. If they have, move them to sent
        * folder or delete them.
        * If there are entries in outbox, that have not been sent,
        * schedule them for sending (unless already scheduled).
        * If there are pending delivery reports, log them.
        * If there are notifications, check if the correspoding
        * messages have been fetched already. If they have, delete
        * the notifications.
        * If the messages have not been fetched, schedule them for
        * fetching (unlessa already scheduled).
        */
        void GarbageCollectionL();

        /**
        * Delete schedule and reset retries for all entries in list
        * @param aSelection list of entries to be cleared.
        */
        void CleanSchedulesL( CMsvEntrySelection& aSelection );

        /**
        * Find the folder where MMS messages are to be stored
        */
        TMsvId FindMMSFolderL();

        /*
        * Check if disk space is below critical level for scheduling.
        * Checks both message drive and drive C:, as scheduling data goes
        * to C: drive regardless of message drive.
        * Params are as for function TMmsGenUtils::DiskSpaceBelowCriticalLevelL,
        * but this subroutine checks drive C: automatically in additio to
        * drive specified.
        * @param aFs File server session.
        * @param aBytesToWrite number of bytes the caller is about to add.
        *        The value 0 checks if the current
        *        space is already below the CL. 
        * @param aDrive number of the drive to be checked.
        * @return ETrue if storage space would go below CL after adding
        *         aBytesToWrite more data, EFalse otherwise.
        *         Leaves on error with one of system-wide error code.
        *         e.g. KErrNotReady if drive contains no media.
        */
        TBool DiskSpaceBelowCriticalForSchedulingL( RFs* aFs,
            TInt aBytesToWrite, TInt aMessageDrive);

        /*
        * Helper function to schedule a selection (send or receive)
        *     and put the selection into "failed" state if schdeduling fails.
        * iMsvSelection must contain some entries to be scheduled.
        * @return error code
        */
        TInt ScheduleSelectionL();

        /** 
        * HandleDummyEntryL reads notification data from entry's store,
        * and then deletes the entry.
        */
        void HandleDummyEntryL();

        /** 
        * HandleMMBoxFlagL sets the stored-in-mmbox flag in indexentry
        * primarily according to notification or secondarily (if notification
        * does not specify it) according to shared data.
        * @param aEntry reference to indexentry into which the change is done
        */
        void HandleMMBoxFlagL( TMsvEntry& aEntry );

        /** 
        * Garbage collection of notification entries in Outbox.
        * Currently notifications can be only forward requests
        */
        void GcOutBoxNotificationsL();

        /**
        * Garbage collection of messages in outbox.
        */        
        void GcOutBoxMessagesL();
        
        /**
        * Garbage collection of notifications in MMS private folder.
        */        
        void GcMmsFolderNotificationsL();
        
        /**
        * Garbage collection of notifications in inbox.
        */        
        void GcInboxNotifications();
        
        /** 
        * Garbage collection of notification entries in mmbox folder.
        * Since 2.8
        */
        void GcMmboxFolderNotifications();
        
        /**
        * Find duplicate notification
        * @param aParent entry id, under where the duplicate is searched
        * @param aHeaders the original mms headers, whose duplicate is searched
        * @param aDuplicate entry id of the found duplicate
        */
        TInt FindDuplicateNotificationL( TMsvId aParent, 
            CMmsHeaders& aHeaders, TMsvId& aDuplicate );
            
        /**
        * Send a read report to the originator of the MMS message
        */
        void SendReadReportL();
        
        /**
        * Log command code for debugging purposes
        * @param aCommand The command code MMS Server MTM was entered with
        */
        void LogCommandCode( TInt aCommand );
        
        /**
        * Get id of the actual MMS service.
        * This is needed when MMS Server MTM is originally called using local service id
        */
        void GetRealServiceId( CMsvEntrySelection& aSelection );
        
        /**
        * Terminate caller or loop to retry in case settings cannot be loaded
        */
        void HandleLoadServiceError( TInt aError );
        
        /**
        * Log the parent of the first entry in the list (for debugging)
        */
        void LogEntryParent();
        
        /**
        * Make sure outbox entries are visible and that the service id
        * is restored in case of schedule deletion command
        */
        void RestoreVisibilityAndService();
        
    public:     // Data
    
    protected:  // Data

    private:    // Data
        CMsvEntrySelection* iMsvSelection; // current selection
        TMsvId  iServiceEntryId;    // Current service id
        TMsvId  iNotification;      // Notification entry under construction
        TRequestStatus* iRequestStatus; // status of the calling program
        // This class may not leave in case of out of memory state
        // when sending or receiving a message
        TBool iOOMState; // out of memory error indicator.
        TBuf8<KMsvProgressBufferLength> iProgressBuffer;
        CMmsSettings* iMmsSettings; // settings corresponding to iServiceentryId
        CMmsHeaders* iMmsHeaders; // just needed to stay alive for a while
        TInt iCurrentMessage;
        
        // State machine classes for different operations
        CMmsSendOperation* iSendOperation;
        CMmsReceiveMessage* iReceiveMessage;
        CMmsForwardOperation* iForwardOperation;
        CMmsDeleteOperation* iDeleteOperation;
        CMmsMmboxList* iUpdateMmboxList;
        CMmsReadReport* iReadReport;
        
        CMmsDecode* iDecoder; // decoder for notifications and delivery reports
        TInt iCommand;  // command for scheduled operations (send or receive)
        TInt iCurrentCommand; // The asynchronous function that was sent to us
        TBufC8<KMaxParameterLength> iParameter;
        TInt iError; // for collecting errors from different operations...
        CBufFlat* iEncodeBuffer; // for decoding notifications and delivery reports
        CLogClient *iLogClient; // For updating log
        CLogViewEvent *iLogViewEvent; // log view
        CLogEvent*          iLogEvent;
        TLogString          iLogString;
        CMmsLog*            iMmsLog;
        CDesCArray*         iRemoteParties;   // Pure addresses to log
        TInt                iMessageDrive;    // messages are on C: drive by default,
                                              // may be moved to other drive
        CMmsServerEntry*    iEntryWrapper;
        TParse              iParse; // member to save stack space
        
        TBool iDeliveryStatus;  //Delivered = ETrue , Rejected = EFalse

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };


// panic function 
GLREF_C void gPanic( TMmsPanic aPanic );

#endif      // MMSSERVER_H
            
// End of File
