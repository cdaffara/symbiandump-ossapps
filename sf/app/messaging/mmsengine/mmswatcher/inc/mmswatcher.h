/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*     Mms Notification dispatcher
*
*/



#ifndef MMSWATCHER_H
#define MMSWATCHER_H

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>

#include "mmscenrepobserver.h"

// CONSTANTS

// MACROS

// DATA TYPES

enum TMmsWatcherStates
    {
    EInvalid = 0,
    EStartup,
    EMessageVariation,
    EGarbageCollection,
    EScheduling,
    EWaiting,
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CWatcherLog;
class CEnvironmentChangeNotifier;
#ifdef __WINS__
class CMmsPollingTimer;
#endif

// CLASS DECLARATION

// Small class for push entry
class CMmsPushEntry :public CBase
    {
    public:  // Constructors and destructor

        CMmsPushEntry();
        virtual ~CMmsPushEntry();
    
    public:
        HBufC8* iData;
        TInt    iStatus;
        CMsvOperation* iOperation;
    };

/**
* Mms Watcher performs all tasks that require waiting for some events
* @lib mmswatcher.lib
* @since 1.0
*/
class CMmsWatcher :
    public CActive,
    public MMsvSessionObserver,
    public MMmsCenRepNotifyHandler
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aFs file system handle
        * @param aLog watcher logger class reference (not used)
        */
        static CMmsWatcher* NewL(TAny* aWatcherParams);
        
        /**
        * Destructor.
        */
        virtual ~CMmsWatcher();

    public: // New functions
        
    public: // Functions from base classes

        /**
        * From MMmsCenRepNotifyHandler
        * HandleCenRepNotificationL is a callback function which is called when
        * a CenRepObserver receives an event.
        * There can be several observers running and they all call this callback
        * when they need to.
        * @param aRepositoryUid identifies the repository where the key under
        *   observation is located.
        * @param aKey identifies the particular key under observation.
        * @param aValue specifies the new changed value of the key.
        */    
        void HandleCenRepNotificationL(
            const TUid aRepositoryUid,
            const TInt32 aKey,
            const TInt aValue );

        /**
        * From MMsvSessionObserver needed to open message server session
        * NOTE that CMmsPollingTimer uses this same callback.
        * @param aEvent event code from message server,
        *   rest of parameters depend on event. See Symbian documentation.
        */
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny*, TAny*, TAny*);

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * constructor.
        * @param apriority actrive object priority.
        * @param aLog watcher logger class reference (not used).
        */
        CMmsWatcher(TInt aPriority,  CWatcherLog& aLog);

        /**
        * Perform one operation in state machine
        */
        void DoRunL();

        /**
        * From CActive: Active object completion
        */
        void RunL();

        /**
        * From CActive: Cancel current operation.
        */
        void DoCancel();

        /**
        * remove first entry from queuedMessages array
        */
        void Dequeue();

        /**
        * remove handled entry
        */
        void RemoveSent();

        /**
        * Send next waiting entry to Server mtm
        */
        void HandleNextInQueueL();

        /**
        * create notification
        * @param aUrl address of the message (path & filename)
        * @param aSize size of the message (file)
        * @return pointer to buffer containing the encoded notification
        */
        HBufC8* CreateNotificationL(TDesC8& aUrl, TInt aSize);

        /**
        * Initialization including garbage collection
        */
        void StartupL();

        /**
        * Garbage collection
        */
        void GarbageCollectionL();

        /**
        * Operator message variation
        */
        void MessageVariationL();

        /**
        * Create an entry into MMS folder
        * @param aFolder folder id
        * @return id of created entry. KMsvNullIndexEntryId if cannot be created
        */
        TMsvId CreateEntryL( TMsvId aFolder );

        /**
        * Open msv server session
        * Either the first time, or because it has been closed
        */
        void OpenSessionL();

        /**
        * Environment change functionS
        */
    	static TInt EnvironmentChanged(TAny* aThis);
        void HandleEnvironmentChange();

        /**
        * GetMessagingEntriesL reads service entryId and NotificationFolderId
        * from MmsSettings. If values are null, they are created.
        */
        void GetMessagingEntriesL( const TBool aMessageStoreHasChanged = EFalse );
       
#ifdef __WINS__
        /**
        * LocalModeL reads the localmode status from settings
        */
        TBool LocalModeL();

        /**
        * ReadLocalModeDirectoriesL is only for emulator use
        */
        void ReadLocalModeConfigData(); 
#endif

    public:     // Data
    
    protected:  // Data

    private:    // Data
        TMsvId                        iService;
        TMsvId                        iNotificationFolder;
        TInt                          iState;
        CWatcherLog&                  iLog;
        TInt                          iLastError;
        CMsvSession*                  iSession;
        CArrayPtrFlat<CMmsPushEntry>* iQueuedMessages;
        CMsvOperation*                iOperation;
        RTimer                        iTimer;
        RFs                           iFs;
        TInt                          iMessageDrive; // default is C:
        // Collection of event flags telling which event has triggered garbage
        // collection. More than one event may be specified at the same time.
        TUint32                       iEvents;
        // Copy of the events, must be saved in case MMS Server MTM is unreachable
        TUint32                       iOldEvents;
        // The moment when media became unavailable
        TTime                         iMediaUnavailableTime;
        // To keep track of MessageStore availability
        // (based on mediaAvailable/unavailable events)
        TBool                         iMediaAvailable;
        // Observer to watch environment time changes
        CEnvironmentChangeNotifier*   iNotifier;
        // Observer of offline mode changes
        CMmsCenRepObserver*           iOfflineObserver;
        // encapsulation version
        TInt16                        iMmsVersion;
#ifdef __WINS__
        // MMS Watcher might be used for polling localmode directory
        // (emulator only)
        TInt iPollingInterval;
        CMmsPollingTimer* iPollingTimer;
        // Specifies the folder from where incoming mms messages are looked for
        // Local mode works automatically in emulator only
        TFileName                     iLocalModeIn;
#endif

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

// panic function 
GLREF_C void gPanic( TMmsPanic aPanic );

#endif      // MMSWATCHER_H   
            
// End of File
