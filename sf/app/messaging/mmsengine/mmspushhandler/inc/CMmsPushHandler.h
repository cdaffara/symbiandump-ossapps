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
* Description:  CMmsPushHandler  declaration
*
*/




#ifndef __CMMSPUSHHANDLER_H
#define __CMMSPUSHHANDLER_H

//  INCLUDES
#include <msvapi.h>
#include <push/cpushhandlerbase.h>

#ifdef _DEBUG
#include <flogger.h>
#endif // _DEBUG

// CONSTANTS
//
const TInt KRequiredDiskSpace = 1024; // Used for checking the diskspace
const TInt KRetryCount = 5;           // Number of times to retry something (more specifically number of times RunL is entered)
const TInt KShortWait = 30*1000000;   // Timeout value to be used for short timeouts
const TInt KLongWait = 5*60*1000000;  // Timeout value to be used for long timeouts

// MACROS
// Related to logging
#ifdef _DEBUG
_LIT(KLogFile,"PushHandler.txt");
_LIT(KLogFolder,"mmss");

#define LOGTEXT(AAA)       RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend,AAA)
#define LOGTEXT2(AAA,BBB)  RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB)

#else // no _DEBUG defined

#define LOGTEXT(AAA)
#define LOGTEXT2(AAA,BBB)

#endif // _DEBUG

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  ECOM listener plugin for MMS engine.
*  Plugin is instantiated by the wap listener in the system watcher.
*  It delivers pushed delivery reports and message notifications to MMS Mtm.
*
*  @lib mmspushhandler.lib
*  @since 2.0
*/
class CMmsPushHandler : public CPushHandlerBase, public MMsvSessionObserver
    {
    public:  // Constructors and destructor

        /**
        * Destructor
        * @since 2.0
        */
        ~CMmsPushHandler();

    public: // New functions
        
        /**
        * Default 1st phase factory method.
        * @since 2.0
        * @return created instance of the CMmsPushHandler class
        */
        static CMmsPushHandler* NewL();
    
    public: // Functions from base classes

        /**
        * Method for handling a received message asynchronously.
        * @since 2.0
        * @param aPushMsg object representing the received push message
        * @param aStatus Status of the asynchronous call
        */
        void HandleMessageL(CPushMessage* aPushMsg, TRequestStatus& aStatus);

        /**
        * Method for handling a received message synchronously.
        * @since 2.0
        * @param aPushMsg object representing the received push message
        */
        void HandleMessageL(CPushMessage* aPushMsg);

        /**
        * Cancels the requested asynchronous message handling.
        * @since 2.0
        */
        void CancelHandleMessage();

        /**
        * Observer/callback method that gets called by Message Server.
        * Currently not used for anything.
        * @since 2.0
        * @param aEvent specifies the event that has occurred
        * @param -
        * @param -
        * @param -
        */
        void HandleSessionEventL(
            TMsvSessionEvent aEvent, 
            TAny* aArg1, 
            TAny* aArg2, 
            TAny* aArg3);

    protected:  // New functions
             
    protected:  // Functions from base classes
        
        /**
        * Performs operations needed for cancelling the message handling.
        * Called by the Active Scheduler after user calling Cancel().
        * @since 2.0
        */
        void DoCancel();

        /**
        * Performs the actual tasks related to message handling.
        * Called by the Active Scheduler.
        * @since 2.0
        */
        void RunL();

    private:    // New functions

        /**
        * Default constructor.
        * @since 2.0
        */
        CMmsPushHandler();

        /**
        * 2nd phase constructor.
        * @since 2.0
        */
        void ConstructL();

        /**
        * Performs basic sanity checks for the received message.
        * Only the body is checked since header is not of interest to us.
        * @since 2.0
        * @return standard error code
        */
        TInt PerformChecks();

        /**
        * Opens a CMsvSession to Server Mtm.
        * @since 2.0
        * @return error code (should be KErrNone)
        */
        void OpenSessionL();

        /**
        * Tries to find the MMS service resources from the Server Mtm.
        * @since 2.0
        */
        void FindServiceL();

        /**
        * Transfers a push message to Server Mtm.
        * @since 2.0
        */
        void TransferMessageL();

        /**
        * Finds/Creates a folder for MMS pushmessages to Message Store
        * @since 2.5
        * @return created entry's Id
        */
        TMsvId GetMMSFolderL();

        /**
        * Creates MMSNotifications folder into Message Store.
        * @since 2.0
        * @return created entry's Id
        */
        TMsvId CreateMMSFolderL();

        /**
        * Tries to find the correct MMSNotifications folder
        * @since 2.0
        * @return entryId
        *   if correct folder was found it is returned,
        *   if not found, KMsvNullIndexEntryId is returned.
        */
        TMsvId FindMMSFolderL();

        /**
        * Creates an entry for the pushed message if necessary and stores
        * the created Id to member variable.
        * @since 2.0
        * @param folder's entry Id into where the new message entry will be
        *  created.
        */
        TMsvId CreateEntryL( TMsvId aFolder );

        /**
        * Checks that there is enough diskspace
        * @since 2.5
        * @return KErrNone if there is enough diskspace
        *         KErrDiskFull if below critical level.
        *             NOTE: The class (this) has been activated already.
        */
        TInt CheckDiskSpaceL();        

    private:    // Functions from base classes
        /**
        * Reserved for future expansion.
        * @since 2.0
        */
        void CPushHandlerBase_Reserved1();      

        /**
        * Reserved for future expansion.
        * @since 2.0
        */
        void CPushHandlerBase_Reserved2();      

    public:     // Data
    
    protected:  // Data

    private:    // Data
        // Operations to indicate the current asynchronous operation
        typedef enum TMmsPushHandlerOperations
            {
            EInitial = 0,       // when entering RunL the first time
            EMsDriveChange,     // when changing MessageStore drive
            ETransferCommand,   // when TransferCommandL is called
            ETimer,             // when a sleep timer is running
            EDiskSpaceWait      // when waiting for disk space to come available
            } TState;
        
        // The state of the active object
        TState iState;
        // Entry Id of the MMS service 
        TMsvId iServiceId;
        // Object representing the session to Server Mtm
        CMsvSession* iMsvSession;
        // Object representing the request made asynchronously to Server Mtm
        CMsvOperation* iMsvOperation;
        // Pushed message
        CPushMessage* iPushMsg;
        // The body of the pushed message as HBufC8 descriptor
        HBufC8* iBody;
        // FileServer reference
        RFs iFs;
        // Timer for retries
        RTimer iTimer;
        // retry count
        TInt iRetryCount;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };

#endif // __CMMSPUSHHANDLER_H 
            
// End of File
