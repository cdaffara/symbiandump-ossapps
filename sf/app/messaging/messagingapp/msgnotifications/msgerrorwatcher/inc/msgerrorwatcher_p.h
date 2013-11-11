/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     CMsgErrorWatcher declaration file
*
*/



#ifndef MSGERRORWATCHERPRIVATE_H
#define MSGERRORWATCHERPRIVATE_H

// INCLUDES
#include <f32file.h>
#include <msvstd.h>
#include <msvapi.h>
#include <systemwarninglevels.hrh>    // Critical level
#include <mmsconst.h>          // KMmsDiskSafetyMargin

#include "MsgErrorWatcher.hrh"

const TUint KCriticalDiskLevel = KDRIVECCRITICALTHRESHOLD;
const TUint KCriticalLevel = KCriticalDiskLevel + KMmsDiskSafetyMargin;
const TUint32 KDefaultTriggerLevel = 300 * 1024; // 100 kilobytes
const TUint KTriggerMargin = 20 * 1024; // 20 kilobytes

// FUNCTION PROTOTYPES
GLREF_C void Panic( TInt aPanic ); 

// FORWARD DECLARATIONS
class CWatcherLog;
class CMmsClientMtm;
class CClientMtmRegistry;
class MsgErrorWatcher;
class CMsgErrorCommDbObserver;
class CMsgErrorDiskSpaceObserver;
class CMsgErrorSmsDiskSpaceObserver;
class CMsgErrorExtSmsDiskSpaceObserver;
class CMsgErrorRoamingObserver;
class CMsgErrorStartupObserver;
class CMsgErrorDisconnectDlg;
class CMsgCenRepObserver;


// CLASS DECLARATION

/**
* CMsgErrorWatcherPrivate
*/
class CMsgErrorWatcherPrivate :public CActive, public MMsvSessionObserver
    {
    public:  // Constructors and destructor        
      /**
        * Default constructor.
        */
    CMsgErrorWatcherPrivate(MsgErrorWatcher* msgErrorWatcher);

       /**
        * Destructor
        */
       virtual ~CMsgErrorWatcherPrivate();

    public: // New functions

        /**
        * Callback function for roaming observer.
        *
        * @param aRoaming ETrue if roaming, EFalse otherwise
        */
        void HandleRoamingEventL( TBool aRoaming );

        /**
        * Callback function for CommDB observer.
        */
        void HandleCommDbEventL();

        /**
        * Callback function for connection observer.
        */
       // void HandleConnectionEvent();

        /**
        * Callback function for disk space observer.
        */
        void HandleDiskSpaceEventL();
        void HandleDiskSpaceEvent2L();
        
        /**
        * Callback function for startup state observer (currently
        * sent items observer).
        */
       void HandleStartupReadyL();
        
        /**
        * HandleCenRepNotificationL
        * Handles events from central repository observer
        */
        void HandleCenRepNotificationL();
                
        
    public: // Functions from base classes

        /**
        * From MMsvSessionObserver
        */
        void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

    protected: // Functions from base classes

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

    private:

        
        /**
         * Initializes the MsgStoreHandler class.     
         */
        void InitL();

        /**
        * Start watcher.
        */
        void StartWatcherL();

        /**
        * Stop watcher.
        */
        void StopWatcher();

        /**
        * Start restart timer. Timer is used when trying to reconnect
        * to message server after watcher has been stopped due
        * to certain message server events.
        */
        void StartRestartTimer();

        /**
        * Kills all observers and resets counters.
        */
        void ResetWatcher();

        /**
        * Checks whether MMS Service is already found. If not
        * gets a pointer to MMS service from MsgStore and if
        * found updates the context of MmsClientMtm.
        *
        * @return ETrue if MMS service was found, EFalse otherwise
        */
        TBool GetMmsServiceL();

        /**
        * Initiates MMS fetch by sending a request to MMS engine.
        */
        void StartMmsFetchL();

        /**
        * Initiates MMS send by sending a request to MMS engine.
        */
        void StartMmsSendL();

        /**
        * Checks whether MMS receiving mode is "Enabled in home network"
        * and "On". If so, activates roaming observer.
        */
        void CheckMmsReceivingModeL();

        /**
        * A wrapper for CMmsClient::ValidateService().
        *
        * @return ETrue if service is valid, EFalse otherwise.
        */
        TBool ValidateMmsServiceL();

        /**
        * Called when MMS reception fails due to insufficient
        * disk space.
        *
        * @param aEntry The failed entry
        */
        void HandleDiskSpaceErrorL( TMsvEntry& aEntry );

        /**
        * Called when MMS reception fails due to insufficient
        * memory.
        *
        * @param aEntry The failed entry
        */
        void HandleMemoryErrorL( TMsvEntry& aEntry );

        /**
        * Called when MMS reception fails because there is
        * another data connection open.
        *
        * @param aEntry The failed entry
        * @param aReceive, ETrue for incoming messages, EFalse for outgoing
        */
        //void HandleConnectionErrorL( TMsvEntry& aEntry, TBool aReceive );

        /**
        * Called when MMS reception fails because there is no
        * access point defined.
        *
        * @param aEntry The failed entry
        */
        void HandleNoAPErrorL( TMsvEntry& aEntry );

        /**
        * Called when MMS reception fails because the current
        * access point is invalid.
        *
        * @param aEntry The failed entry
        */
        void HandleInvalidAPErrorL( TMsvEntry& aEntry, TBool aStartObserver );

        /**
        * Subfunction of HandleSessionEventL
        * Handles root events from MsgServer observer
        *
        * @param aEvent     Message server event
        * @param aEntries   Affected entries
        */
        void HandleRootEventL(
            TMsvSessionEvent aEvent,
            CMsvEntrySelection* aEntries );

        /**
        * Subfunction of HandleSessionEventL
        * Handles local service events from MsgServer observer
        *
        * @param aEvent     Message server event
        * @param aEntries   Affected entries
        */
        void HandleLocalServiceEventL(
            TMsvSessionEvent aEvent,
            CMsvEntrySelection* aEntries );

        /**
        * Subfunction of HandleSessionEventL
        * Handles inbox events from MsgServer observer
        *
        * @param aEvent     Message server event
        * @param aEntries   Affected entries
        */
        void HandleInboxEventL(
            TMsvSessionEvent aEvent,
            CMsvEntrySelection* aEntries );

        /**
        * Subfunction of HandleSessionEventL
        * Handles outbox events from MsgServer observer
        *
        * @param aEvent     Message server event
        * @param aEntries   Affected entries
        */
        void HandleOutboxEventL(
            TMsvSessionEvent aEvent,
            CMsvEntrySelection* aEntries );

        /**
        * Subfunction of HandleSessionEventL
        * Handles MMS service events from MsgServer observer
        *
        * @param aEvent     Message server event
        * @param aEntries   Affected entries
        */
        void HandleMmsServiceEventL(
            TMsvSessionEvent aEvent,
            CMsvEntrySelection* aEntries );


        /**
        * Resets TMsvEntry::iError of an entry saved in iCurrentEntry
        */
        void ResetErrorFieldL( );
        
        /**
        * Resets TMsvEntry::iError of an entry given as parameter
        */
        void ResetErrorFieldL( TMsvEntry& aEntry );
         
    private:    // Data

        enum TMsgErrorWatcherFlags
            {
            EStartupReady               = 0x0001,
            EWatcherRunning             = 0x0002,
            ENoAPErrorPending           = 0x0004,
            EShowRoamingNote            = 0x0008,
            ERoamingNoteShown           = 0x0010,
            EReceivingDisconnectDelay   = 0x0020
            };

        enum TMsgRequestTypes
            {
            EMsgRequestNone = 0,
            EMsgRequestStartingUp,
            EMsgRequestFetching,
            EMsgRequestFetchingAll,
            EMsgRequestSending,
            EMsgRequestWaitingErrorNote,
            EMsgRequestWaitingDisconnection
            };

        /**
         * Pointer to MsgErrorWatcher
         * Doesn't own
         */
        MsgErrorWatcher* q_ptr;

        /**
         * MMSservice id
         */
        TMsvId iMmsServiceId;
        
        /**
         * Notification folder id
         */
        TMsvId iNotificationFolderId;
        
        /**
         * Msv session
         * Own
         */
        CMsvSession* iSession;
        
        /**
         * Msv operation
         * Own
         */
        CMsvOperation* iOperation;
        
        /**
         * Mtmregitsry client
         * Own
         */
        CClientMtmRegistry* iClientMtmRegistry;
        
        /**
         * Mmsclient
         * Own
         */
        CMmsClientMtm* iMmsClient;
        
        /**
         * Pointer to mms receive error messages
         * Own
         */
        CMsvEntrySelection* iMmsReceiveErrorMessages;
        
        /**
         * Pointer to mms receive error messages
         * Own
         */
        CMsvEntrySelection* iMmsSendErrorMessages;

        /**
         * Current entry id
         */
        TMsvId iCurrentEntryId;

        /**
         * Pointers to error observers
         * CommDb observer
         * Own
         */
        CMsgErrorCommDbObserver* iCommDbObserver;
        
        /**
         * Pointer to diskspace observer
         * Own
         */
        CMsgErrorDiskSpaceObserver* iDiskSpaceObserver;
        /**
         * Pointer to sms diskspace observer
         * Own
         */
        CMsgErrorSmsDiskSpaceObserver* iSmsDiskSpaceObserver;
        /**
         * Pointer to extension sms diskspace observer
         * Own
         */
        CMsgErrorExtSmsDiskSpaceObserver* iSmsExtDiskSpaceObserver;
        /**
         * Pointer to roaming observer
         * Own
         */
        CMsgErrorRoamingObserver* iRoamingObserver;
        /**
         * Pointer to startup observer
         * Own
         */
        CMsgErrorStartupObserver* iStartupObserver;
        /**
         * Pointer to cenerep observer
         * Own
         */
        CMsgCenRepObserver* iCenRepObserver;
       
        /**
         * File session
         */
        RFs iFs;
        
        /**
         * Timer
         */
        RTimer iTimer;

        TMsgRequestTypes iRequestType;
        TUint iWatcherFlags;
        TUint32 iMaxReceiveSize;
        TUint iDiskSpaceErrors;
        TUint iTimerRetries;
    };

#endif      // MSGERRORWATCHERPRIVATE_H
            
// End of File
