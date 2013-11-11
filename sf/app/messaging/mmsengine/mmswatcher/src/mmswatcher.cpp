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
*     Mms event watcher
*
*/



// INCLUDE FILES
#include    <msvapi.h>
#include    <msvids.h>
#include    <msvuids.h>
#include    <e32math.h>
#include    <apparc.h>
#include    <mtclreg.h>
#include    <bacntf.h>
#include    <centralrepository.h>
#include    <CoreApplicationUIsSDKCRKeys.h>
#include    <implementationproxy.h>

#include    "mmsconst.h"
#include    "mmsservercommon.h"
#include    "mmscmds.h"
#include    "mmswatcher.h"
#include    "watcher.h"
#include    "mmsclient.h"
#include    "mmserrors.h"
#include    "mmssettings.h" // for message variation directory
#include    "MmsEnginePrivateCRKeys.h"
#include    "mmscenrepobserver.h"
#include    "mmspollingtimer.h"
#include    "mmswatcherdebuglogging.h"
#include    "mmssettings.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KPauseTime = 10 * KMmsMillion; // 10 s
const TInt KMmsMessageGranularity = 5;
const TInt KMmsStartDelay = 2 * KMmsMillion; // 2 s
const TInt KMmsMediaAvailableDelay = 3 * KMmsMillion; // 3 s
const TInt KMmsMessageVariationDelay = 5 * KMmsMillion; // 5 s
const TInt KMmsDiskFullPause = 300 * KMmsMillion; // 5 min
#ifdef __WINS__
const TInt KMmsDefaultPollingInterval = 3 * KMmsMillion; 
const TInt KMmsTidBuffer = 18;
const TInt KMmsRowBufferLength = 128;
#endif

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

//
// MmsWatcher is an ECOM plugin
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x10005948, CMmsWatcher::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

// ================= MEMBER FUNCTIONS =======================

// Helper class constructor and destructor
CMmsPushEntry::CMmsPushEntry():
    iData( NULL ),
    iStatus( 0 ),
    iOperation( 0 )
    {
    }

CMmsPushEntry::~CMmsPushEntry()
    {
    delete iData;
    delete iOperation;
    }

// ---------------------------------------------------------
// CMmsWatcher::CMmsWatcher
// ---------------------------------------------------------
//
CMmsWatcher::CMmsWatcher(
    TInt aPriority,
    CWatcherLog& aLog)
    : CActive( aPriority ),
    iService( KMsvNullIndexEntryId ),
    iNotificationFolder( KMsvNullIndexEntryId ),
    iState( 0 ),
    iLog( aLog ),
    iLastError( KErrNone ),
    iSession( NULL ),
    iQueuedMessages( NULL ),
    iOperation( NULL )
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CMmsWatcher::ConstructL
// ---------------------------------------------------------
//
void CMmsWatcher::ConstructL()
    {
    LOG(_L("ConstructL"));

    //
    // ConstructL basically creates some member objects and 
    // starts a couple of observers
    //
    
    iEvents = KMmsReasonBoot;
    iQueuedMessages = new(ELeave) CArrayPtrFlat<CMmsPushEntry>( KMmsMessageGranularity );
    iState = EStartup;
    iMediaAvailable = ETrue;
    iMmsVersion = KMmsDefaultVersion;

    User::LeaveIfError( iTimer.CreateLocal() );
    User::LeaveIfError( iFs.Connect() );

    // Offline state observer
    iOfflineObserver = CMmsCenRepObserver::NewL();
    iOfflineObserver->SubscribeNotification(
        KCRUidCoreApplicationUIs,
        KCoreAppUIsNetworkConnectionAllowed,
        this );

#ifdef __WINS__ // Support for localmode in WINS emulator
    iLocalModeIn = KMmsDefaultLocalModeDir;
    iPollingInterval = 0; 
    ReadLocalModeConfigData();
#endif

    // CEnvironmentChangeNotifier detects changes in system time.
    // This is needed to run garbage collection if the user moves
    // system time too much towards future.
    // Task scheduler can handle short changes, but if the change is a large one,
    // task scheduler does not reschedule the entry, and we must do it ourselves.
  	iNotifier = CEnvironmentChangeNotifier::NewL(
  	    CActive::EPriorityLow, 
  	    TCallBack( EnvironmentChanged, this ) );
	iNotifier->Start();

    //
    // NOTE: message server session is created just before used (in garbage 
    // collection). This ensures that MessageServer has properly booted.
    //

    // A little delay before any real actions in order to ensure everything
    // else is properly up and running.
    // iTimer cannot be active because it was just created
    iTimer.After( iStatus, KMmsStartDelay ); // 2 s - even this may be to much
    SetActive();
#ifndef _NO_MMSS_LOGGING_
    if ( IsActive() )
        {
        LOG(_L("MMS watcher is now active" ));
        }
    else
        {
        LOG(_L("MMS watcher is NOT active" ));
        }
#endif
    LOG(_L("End of ConstructL"));
    }

// ---------------------------------------------------------
// CMmsWatcher::NewL
// ---------------------------------------------------------
//
CMmsWatcher* CMmsWatcher::NewL( TAny* aWatcherParams )
    {
    TWatcherParams* params = reinterpret_cast<TWatcherParams*>( aWatcherParams );
	CMmsWatcher* self = new (ELeave) CMmsWatcher( EPriorityStandard, params->iLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// CMmsWatcher::~CMmsWatcher
// ---------------------------------------------------------
//
CMmsWatcher::~CMmsWatcher()
    {
    //
    // Watchers should be running all the time
    // 
    LOG(_L("Destructor called"));

    Cancel();
    delete iOperation; // unfinished garbage collection...
    if ( iQueuedMessages )
        {
        iQueuedMessages->ResetAndDestroy();
        }
    delete iQueuedMessages;
    iTimer.Close();
    iFs.Close();
    delete iSession;
    delete iNotifier;
    delete iOfflineObserver;
#ifdef __WINS__ // Support for localmode polling in WINS emulator
    if( iPollingTimer )
        {
        delete iPollingTimer;
        }
#endif
    }

// ---------------------------------------------------------
// CMmsWatcher::HandleCenRepNotificationL
// ---------------------------------------------------------
//
void CMmsWatcher::HandleCenRepNotificationL(
    const TUid /*aRepositoryUid*/, // not needed currently because only one observer
    const TInt32 /*aKey*/,         // not needed currently because only one observer
    const TInt aValue )
    {
    // Offline state has changed.
    // If switch has been from offline to online -> invoke GarbageCollection
    LOG2(_L("Offline state: %d"), aValue );

    if( aValue != ECoreAppUIsNetworkConnectionAllowed )
        {
        // Now in offline -> nothing to do
        return;
        }
    else // back in online
        {
        iEvents |= KMmsReasonNetworkAllowed;
        TRequestStatus* status = &iStatus;
        if( iState != EStartup && iState != EMessageVariation )
            {
            iState = EGarbageCollection;
            }
        if( !IsActive() )
            {
            iStatus = KRequestPending;
            SetActive();
            User::RequestComplete( status, KErrNone );
            }
        }
    }


// ---------------------------------------------------------
// CMmsWatcher::HandleSessionEventL
// ---------------------------------------------------------
//
void CMmsWatcher::HandleSessionEventL(
    TMsvSessionEvent aEvent,
    TAny* aArg1,
    TAny* aArg2,
    TAny* /*aArg3*/)
    {
    // CMmsSession requires as a parameter a class that implements
    // session observer interface (this function). Therefore this
    // function must always exist, though it may return immediately
    // without doing anything.

    // This function is needed to watch events related to moving
    // the message store in systems where the message store can
    // be moved to a memory card.

    // For test purposes, we use this as a test callback.
    // This function is called every time an entry changes.

    // we check what happened, and if an entry appears in
    // sent folder, we know that it has been sent to MMSC, and
    // we try to fetch it back.

    if ( aEvent == EMsvGeneralError )
        {
        return;
        }

#ifdef __WINS__
    TMsvId parentId = KMsvNullIndexEntryId;
    if( aArg2 )
        {
        parentId = *(TMsvId*)aArg2;
        }

    CMsvEntrySelection* entries = NULL;
    if( aArg1 )
        {
        entries = STATIC_CAST(CMsvEntrySelection*, aArg1);
        }
#endif

    TRequestStatus* status = &iStatus;

#ifdef __WINS__
    TInt oldState = iState;
#endif

    switch (aEvent)
        {
        case EMsvCloseSession:
            LOG(_L("Session event EMsvCloseSession"));
            // fall through on purpose
        case EMsvServerTerminated:
            LOG(_L("Session event EMsvServerTerminated"));
            if ( iSession )
                {
                delete iSession;
                iSession = NULL;
                // if we have deleted the session, we get no more
                // events. We don't know when to restart.
                }
            break;
        case EMsvMediaChanged:
            {
            // MessageStore drive has changed.
            LOG(_L("Session event EMsvMediaChanged"));
            LOG3(_L("- from drive %d to drive %d"), *(TInt*) aArg1, *(TInt*) aArg2 );
            iMediaAvailable = ETrue;
            iMessageDrive = *(TInt*) aArg2;
            if( *(TInt*) aArg1 != *(TInt*) aArg2 )
                {
                iEvents |= KMmsReasonMessageStoreChanged;
                }
            // CenRep values for all entries must be updated
            // The ids of the MMS folders must be synchronized even if the drive has not changed
            // as we get this event when backup/restore happens, and in some case the restored
            // folder ids may be different from those in central repository (if restore is done
            // from a different phone or after phone has been flashed)
            GetMessagingEntriesL( ETrue );
            if( iEvents != 0 )
                {
                if ( iState != EStartup && iState != EMessageVariation )
                    {
                    iState = EGarbageCollection;
                    }
                if ( !IsActive() )
                    {
                    iStatus = KRequestPending;
                    SetActive();
                    User::RequestComplete( status, KErrNone );
                    }
                else
                    {
                    // if timer is running, cancel it because it is obvious
                    // that we have waited long enough and can start garbage
                    // collection now.
                    // Cancelling timer should bring us back to RunL()
                    LOG(_L("- already active, cancelling timer to speed up the process"));
                    iTimer.Cancel();
                    }
                }
            break;
            }
        case EMsvMediaUnavailable:
            {
            LOG(_L("Session event EMsvMediaUnavailable"));
            LOG2(_L("- Drive %d no longer available"), *(TInt*) aArg1 );
            iMediaAvailable = EFalse;
            if ( *(TInt*) aArg1 == iMessageDrive )
                {
                iMediaUnavailableTime.UniversalTime();
                }
            break;
            }
        case EMsvMediaAvailable:
            {
            LOG(_L("Session event EMsvMediaAvailable"));
            LOG2(_L("- Drive %d again available"), *(TInt*) aArg1 );
            // MessageStore has become available again. This could be due to
            // insertion of MMC card or after backup/restore operation 
            // has finished
            iMediaAvailable = ETrue;
            if ( !(iEvents & KMmsReasonMessageStoreChanged) )
                {
                iEvents |= KMmsReasonHotswap;
                iEvents |= KMmsReasonBackupEnded;
                }
            if( iState != EStartup && iState != EMessageVariation )
                {
                iState = EGarbageCollection;
                }
            if( !IsActive() )
                {
                // Delay here to prevent EMsvMediaAvailable from triggering
                // Garbage collection when message store is moved to another drive.
                // When message store is moved, the store is copied first.
                // After the copy is complete, we get EMsvMediaAvailable event,
                // but we must not start garbage collection on the original drive
                // because the message store is about to be changed.
                //
                // 3 s delay - we are cautious
                // if we are not active, timer cannot be running - no need to cancel
                iTimer.After( iStatus, KMmsMediaAvailableDelay ); 
                SetActive();
                }
            break;
            }
        case EMsvMediaIncorrect:
            {
            LOG(_L("Session event EMsvMediaIncorrect"));
            LOG2(_L("- Incorrect disk in drive %d"), *(TInt*) aArg1 );
            break;
            }
        case EMsvServerReady:
            {
            LOG(_L("Session event EMsvServerReady"));
            break;
            }
#ifdef __WINS__
        case EMsvEntriesCreated:
        case EMsvEntriesMoved:
            {
            // 
            // These events are relevant in Localmode only
            // 
            if ( parentId != KMsvSentEntryId )
                {
                // only sent items are interesting
                return;
                }
            
            // Get localmode setting from CenRep
            TBool localMode = EFalse;
            TInt retval = KErrNone;
            TRAP( retval, 
                {
                localMode = LocalModeL();
                });
            #ifndef _NO_MMSS_LOGGING_
            if( retval != KErrNone )
                {
                LOG(_L("ERROR connecting CenRep, defaulting to normal mode"));
                }
            #endif
            if( localMode == EFalse )
                {
                // Only localmode case is interesting
                return;
                }
            
            // Now invoke the server mtm to fetch it.
            // We queue the entry, set ourselves complete and
            // Let the RunL initiate the fetching.
            // We don't do anything unless the entry was MMS entry
            // Then get a list of files in MMSC directory, and
            // create notifications for them.
            // This will probably produce extra notifications,
            // but it is good to test that notifications about
            // unexisting messages or duplicates won't mess 
            // up the system.
            // Messages should be fetched one at a time, so that
            // there should not be too many conflicts.
            TInt notifications = 0;
            TInt size = 1;
            TEntry entry;
            TInt err = KErrNone;
            CDir* fileList = NULL;
            err = iFs.Entry( iLocalModeIn, entry );
            if ( err == KErrNone )
                {
                err = iFs.SetSessionPath( iLocalModeIn );
                TFindFile finder( iFs );
                _LIT( KWild, "*.mms" );
                if ( err == KErrNone )
                    {
                    err = finder.FindWildByPath( KWild, NULL, fileList );
                    }
                if ( err == KErrNone )
                    {
                    notifications = fileList->Count();
                    }
                TInt i;
                for (i = 0; i < notifications; i++ )
                    {
                    // generate notification from filename and file size
                    TParse fullEntry;
                    fullEntry.Set( ( ( *fileList )[i] ).iName, &iLocalModeIn, NULL );
                    TBuf8<KMaxFileName> buffer;
                    buffer.Copy( fullEntry.FullName() );
                    RFile file;
                    err = file.Open(iFs, fullEntry.FullName(), EFileShareAny );
                    if (err == KErrNone )
                        {
                        err = file.Size(size);
                        }
                    file.Close();
                    HBufC8 * notif = CreateNotificationL(buffer, size);
                    CleanupStack::PushL( notif );
                    TBool found = EFalse;
                    TInt i = 0;
                    while ( found == EFalse && i < iQueuedMessages->Count() ) 
                        {
                        if ( iQueuedMessages->At(i)->iData->Compare(*notif) == 0 )
                            {
                            found = ETrue;
                            }
                        i++;
                        }
                    if (! found )
                        {
                        CMmsPushEntry* newEntry = new( ELeave )CMmsPushEntry;
                        newEntry->iData = notif;
                        newEntry->iStatus = EWaiting;
                        newEntry->iOperation = NULL;
                        CleanupStack::Pop( notif );
                        CleanupStack::PushL( newEntry );
                        iQueuedMessages->AppendL( newEntry );
                        // notif has gone to our member...
                        CleanupStack::Pop( newEntry );
                        }
                    else
                        {
                        // get rid of notif
                        CleanupStack::PopAndDestroy( notif );
                        }
                    iState = EScheduling;
                    }
                }

            if ( oldState == EWaiting && notifications > 0)
                {
                // if we are not currently sending something to server,
                // we must declare ourselves complete in order to get back to
                // RunL.
                // Actually we should be active already, but 
                // we say so just in case (to prevent stray signal)
                if ( !IsActive() )
                    {
                    iStatus = KRequestPending;
                    SetActive();
                    User::RequestComplete( status, KErrNone );
                    }
                }
            break;
            }
#endif // __WINS__

        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMmsWatcher::EnvironmentChanged
// ---------------------------------------------------------
//
TInt CMmsWatcher::EnvironmentChanged(TAny* aThis )
	{
	CMmsWatcher* self = reinterpret_cast<CMmsWatcher*>(aThis);
	self->HandleEnvironmentChange();
	return KErrNone;
	}

// ---------------------------------------------------------
// CMmsWatcher::HandleEnvironmentChange
// ---------------------------------------------------------
//
void CMmsWatcher::HandleEnvironmentChange()
    {

  	TInt changes = iNotifier->Change();
	if ( changes & EChangesSystemTime )
        {
#ifndef _NO_MMSS_LOGGING_
        LOG(_L("System time Change"));
        TTime time;
		time.UniversalTime();
		TDateTime due(time.DateTime());
		LOG7( _L("- system time is now: [%02d/%02d/%d] @ %02d:%02d:%02d"),
		    due.Day(), (TInt)due.Month() + 1, due.Year(), due.Hour(), due.Minute(), due.Second());
#endif
        iEvents |= KMmsReasonEnvironmentTimeChanged;
        TRequestStatus* status = &iStatus;
        if ( iState != EStartup && iState != EMessageVariation )
            {
            iState = EGarbageCollection;
            }

        if ( !IsActive() )
            {
            iStatus = KRequestPending;
            SetActive();
            User::RequestComplete( status, KErrNone );
            }
        }
    }

// ---------------------------------------------------------
// CMmsWatcher::Dequeue
// ---------------------------------------------------------
//
void CMmsWatcher::Dequeue()
    {
    delete iQueuedMessages->At( 0 );
    iQueuedMessages->Delete( 0 );
    iQueuedMessages->Compress();
    }

// ---------------------------------------------------------
// CMmsWatcher::RemoveSent
// ---------------------------------------------------------
//
void CMmsWatcher::RemoveSent()
    {
    LOG(_L("RemoveSent called"));
    while (iQueuedMessages->Count() > 0 &&
        iQueuedMessages->At(0)->iStatus != EWaiting )
        {
        // done, delete
        Dequeue();
        }
    }

// ---------------------------------------------------------
// CMmsWatcher::HandleNextInQueueL
// ---------------------------------------------------------
//
void CMmsWatcher::HandleNextInQueueL()
    {
    // access to first entry in queue need not be protected by
    // semaphore:
    // This function is called only after we have decided
    // that there is at least one entry.
    // Entries are removed only when we get to the RunL
    // the next time (we are still in DoRunL now).
    // Adding new entries to the end of the array does not hurt us.
    // Removing and adding entries must be mutally exclusive,
    // as entries are added to the end and removed from the beginning.

    LOG(_L("HandleNextInQueueL"));
    
    if( !iSession )
        {
        // if we have lost our session, we must try to get a new one
        OpenSessionL();
        }
        
    // If there was no service entry when we were started, we must
    // check if one has appeared now. Otherwise message server does
    // not know where to send the request

    if( iService == KMsvNullIndexEntryId || 
        iNotificationFolder == KMsvNullIndexEntryId )
        {
        GetMessagingEntriesL();
        }

    if( iService == KMsvNullIndexEntryId || 
        iNotificationFolder == KMsvNullIndexEntryId )
        {
        // Something really weird is going on.
        User::Leave( KErrNotFound );
        }

    TMsvId entryId = CreateEntryL( iNotificationFolder );

    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    // Now we have an entry that says: local service, MMS MTM
    if ( entryId != KMsvNullIndexEntryId )
        {
        selection->AppendL( entryId );
        }
    else
        {
        selection->AppendL( iService );
        }
  
    iQueuedMessages->At(0)->iStatus = EScheduling;
    TWatcherParameters parameters; // initialized to zero
    TWatcherParametersBuf paramPack( parameters );

    iQueuedMessages->At(0)->iOperation = iSession->TransferCommandL(
        *selection, EMmsDecodePushedMessage, paramPack, iStatus );

    CleanupStack::PopAndDestroy( selection );
    SetActive();
    }
    
// ---------------------------------------------------------
// CMmsWatcher::RunL()
// ---------------------------------------------------------
//
void CMmsWatcher::RunL()
    {
    LOG2(_L("RunL (iStatus = %d)"), iStatus.Int());

    iLastError = iStatus.Int();
    if ( iLastError == KErrCancel )
        {
        // cancel occurs only if a new event cancels timer
        iLastError = KErrNone;
        }

    // when we get here after doing garbage collection,
    // the operation can go
    if( iOperation )
        {
        if( iOperation->iStatus == KRequestPending )
            {
            LOG(_L("- operation still pending"));
            // 5 second delay needed for message variation
            iTimer.After( iStatus, KMmsMessageVariationDelay );
            SetActive();
            return;
            }
        else
            {
            LOG(_L("- operation completed"));
            if( iLastError == KErrNone )
                {
                iLastError = iOperation->iStatus.Int();
                }
            delete iOperation;
            iOperation = NULL;
            }
        }

    // Now we check if Garbage collection was successful.
    // If not, old events must be retried

    TBool onlineEvent = iEvents & KMmsReasonNetworkAllowed;

    if ( iLastError != KErrNone )
        {
        // We combine all flags in case we are getting loads of events
        // faster than we can handle them.
        iEvents |= iOldEvents;
        }

    // clear these now to prevent endless loop.
    iOldEvents = 0;

    // If we have got error in the range KMsvMediaUnavailable - KMsvIndexRestore
    // we must wait until we get media available event, otherwise we are in trouble.
    if ( iLastError <= (TInt) KMsvMediaUnavailable &&
        iLastError >= (TInt) KMsvIndexRestore  && !iMediaAvailable )
        {
        // media is not available, no use to retry until we get media available event.
        // If media is available, it is worth retrying in 10 s.
        return;
        }

    if ( iLastError == KMmsErrorOfflineMode && !onlineEvent )
        {
        // if we are not yet online, wait for event before retrying
        return;
        }

    TRAPD( error, DoRunL() );
    if( error )
        {
        LOG2(_L("DoRunL error %d"), error );
        // An error occurred - try again after a while
        TInt pause = KPauseTime; // default 10 s
        if ( error == KErrDiskFull )
            {
            pause = KMmsDiskFullPause; // 5 min
            }
        LOG2(_L("- Retry after %d seconds"), pause/KMmsMillion );
        iTimer.Cancel();
        iTimer.After(iStatus, pause);
        if ( !IsActive() )
            {
            SetActive();
            }
        } // error

    iLastError = error;

    // Handle next in queue sets us active again, if there
    // are more pushed messages to be sent to Server MTM.
    // If there is nothing to send, we are not active.
    // In that case the callback must call HandleNextInQueueL()
    // to start the active loop.
    }


// ---------------------------------------------------------
// CMmsWatcher::DoRunL()
// ---------------------------------------------------------
//
void CMmsWatcher::DoRunL()
    {
    LOG2(_L("DoRunL, state == %d"), iState);
    switch ( iState )
        {
        case EStartup:
            // Make sure no error in status
            StartupL();
            iState = EMessageVariation;
            break;
        case EMessageVariation:
            {
            // Message variation is tried every time because watchers are no started
            // so late that first boot indicator is always off.
            // Precreated messages are in private directory and cannot be accessed
            // by outsiders except by those that have ALL FILES capability
            MessageVariationL();                
            iState = EGarbageCollection;
#ifdef __WINS__
            // Support for localmode polling in WINS emulator is started here if needed
            if( iPollingInterval >= KMmsDefaultPollingInterval ) // ReadLocalModeConfigDataL might have changed it
                {
                iPollingTimer = CMmsPollingTimer::NewL();
                iPollingTimer->Start( this, iPollingInterval );
                }
#endif
            break;
            }
        case EGarbageCollection:
            GarbageCollectionL();
            iState = EScheduling;
            break;
        case EScheduling:
            {
            TInt count = 0;
            if ( iLastError != KErrNone )
                {
                LOG2(_L("iLastError %d"), iLastError );
                // I assume only the first one has been tried and failed.
                // However, we retry all.
                count = iQueuedMessages->Count();
                TInt i;
                for ( i = 0; i < count; ++i )
                    {
                    iQueuedMessages->At( i )->iStatus = EWaiting;
                    }
                // Leave here, and RunL will retry after a pause
                User::Leave( iLastError );
                }

            // Now we must check if there is anything to send to 
            // MMS server mtm
            if (iQueuedMessages->Count() > 0)
                {
                RemoveSent();
                }
            count = iQueuedMessages->Count();
            // If there is anything left, queue the next one
            if ( count > 0 )
                {
                HandleNextInQueueL();
                }
            else if ( iEvents != 0 )
                {
                iState = EGarbageCollection;
                iStatus = KRequestPending;
                TRequestStatus* status = &iStatus;
                SetActive();
                User::RequestComplete( status, KErrNone );
                }
            else
                {
                // If there is nothing left, continue waiting
                iState = EWaiting;
                }
            break;
            }
        case EWaiting:
            iStatus = KErrNone;
            break;
        default:
            __ASSERT_DEBUG(EFalse, gPanic(EMmsUnknownState));
            break;
        }
    }

// ---------------------------------------------------------
// CMmsWatcher::DoCancel
// ---------------------------------------------------------
//
void CMmsWatcher::DoCancel()
    {
    LOG(_L("DoCancel"));

    iTimer.Cancel();
    if ( iOperation )
        {
        iOperation->Cancel();
        }
    // only one operation at a time is active
    if ( iQueuedMessages->Count() > 0 && 
        iQueuedMessages->At(0)->iOperation )
        {
        iQueuedMessages->At(0)->iOperation->Cancel();
        }

    iState = EInvalid;
    }

// ---------------------------------------------------------
// CMmsWatcher::StartupL()
// ---------------------------------------------------------
//
void CMmsWatcher::StartupL()
    {
    LOG(_L("StartupL"));
    TInt error = KErrNone;
    
    iMessageDrive = EDriveC; // default
    // Open session to MessageServer
    TRAP( error, OpenSessionL() );
    // We ignore the error. If the session could not be opened now,
    // it will be retried later
    error = KErrNone;
    
    // Get serviceId and NotificationFolderId from MMS settings
    TRAP( error, GetMessagingEntriesL() );
    if( error )
        {
        LOG(_L("- ERROR reading settings"));
        }
    
    // Complete in order to get back to RunL
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    SetActive();
    User::RequestComplete( status, KErrNone );
    LOG(_L("- End of StartupL"));
    }

// ---------------------------------------------------------
// CMmsWatcher::GarbageCollectionL()
// ---------------------------------------------------------
//
void CMmsWatcher::GarbageCollectionL()
    {
    LOG(_L("GarbageCollectionL"));

    // Open session in case the message server has died
    // but we have received an event from elsewhere    
    if( !iSession )
        {
        // if we have lost our session, we must try to get a new one
        OpenSessionL();
        }

    if ( iService != KMsvNullIndexEntryId )
        {
        // if there is no service, there is no garbage either...
        CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
        CleanupStack::PushL( selection );
        selection->AppendL( iService );
        // Start Garbage collection
        TMMSGarbageCollectionParameters parameters; // initialized to zero
        parameters.iReasonFlags = iEvents;
        parameters.iMediaUnavailableTime = iMediaUnavailableTime;
        TMMSGarbageCollectionParametersBuf paramPack( parameters );
        LOG(_L("Invoking garbage collection"));
        iOperation = iSession->TransferCommandL(
            *selection, EMmsGarbageCollection, paramPack, iStatus );
        CleanupStack::PopAndDestroy( selection );
        // These events were handled.
        iOldEvents = iEvents; 
        iEvents = 0;
        iMediaUnavailableTime = 0;
        SetActive();
        }
    LOG(_L("- End of GarbageCollectionL"));
    }

// ---------------------------------------------------------
// CMmsWatcher::MessageVariationL()
// ---------------------------------------------------------
//
void CMmsWatcher::MessageVariationL()
    {
    LOG(_L("MessageVariationL"));
    if( !iSession )
        {
        // if we have lost our session, we must try to get a new one
        OpenSessionL();
        }

    // If there is no service, it must be created first
    if( iService == KMsvNullIndexEntryId )
        {
        LOG(_L("no service - must create one"));
        GetMessagingEntriesL();
        }
    CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->AppendL( iService );
    LOG(_L("Invoking message variation"));
    iOperation = iSession->TransferCommandL(
        *selection, EMmsMessageGeneration, TPtrC8(), iStatus );
    CleanupStack::PopAndDestroy( selection );
    SetActive();

    LOG(_L("End of MessageVariationL"));
    }

// ---------------------------------------------------------
// CMmsWatcher::CreateEntryL()
// ---------------------------------------------------------
//
TMsvId CMmsWatcher::CreateEntryL( TMsvId aFolder )
    {
    TMsvId entryId = KMsvNullIndexEntryId;

    if ( aFolder == KMsvNullIndexEntryId )
        {
        // no folder no entry
        return entryId;
        }
    
    if( !iSession )
        {
        // if we have lost our session, we must try to get a new one
        OpenSessionL();
        }
    CMsvEntry* cEntry = iSession->GetEntryL( aFolder );
    CleanupStack::PushL( cEntry ); // ***

    TMsvEntry entry;
    entry.iType = KUidMsvMessageEntry;
    entry.iMtm = KUidMsgTypeMultimedia;
    entry.SetVisible( ETrue );
    // If we want to put data here, InPreparation must be set to false first
    entry.SetInPreparation( EFalse );
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    entry.iRelatedId = iService;
    entry.iMtmData2 = KMmsNotificationBinary;
    cEntry->CreateL( entry );
    entryId = entry.Id();

    //
    // Stream 
    // 1) length of the data as 32 bit integer
    // 2) pushed message data
    // into created entry's stream  
    //
    cEntry->SetEntryL( entryId );
    CMsvStore* store = cEntry->EditStoreL();
    CleanupStack::PushL( store );   // ***
    RMsvWriteStream outs;
    outs.AssignLC( *store, KUidBinaryNotificationStream ); // ***
    outs.WriteUint32L( iQueuedMessages->At(0)->iData->Size() );
    outs.WriteL( *iQueuedMessages->At(0)->iData );
    outs.CommitL();
    outs.Close();
    store->CommitL();
    CleanupStack::PopAndDestroy( &outs ); // close outs
    CleanupStack::PopAndDestroy( store );
    CleanupStack::PopAndDestroy( cEntry );

    return entryId;
    }

// ---------------------------------------------------------
// CMmsWatcher::OpenSessionL()
// ---------------------------------------------------------
//
void CMmsWatcher::OpenSessionL()
    {
    LOG(_L("Opening session"));
    if ( !iSession )
        {
        // leaves if session cannot be opened
        iSession = CMsvSession::OpenSyncL( *this );
        }
        
    TRAPD( error2, iMessageDrive = iSession->CurrentDriveL() );
    if( error2 != KErrNone )
        {
        // Using default
        iMessageDrive = EDriveC;
        }
    }

// ---------------------------------------------------------
// CMmsWatcher::CreateNotificationL()
// ---------------------------------------------------------
//
#ifdef __WINS__
HBufC8* CMmsWatcher::CreateNotificationL(TDesC8& aUrl, TInt aSize)
    {
    // for test purposes aUrl will contain the filename.
    TInt position = 0;
    CBufFlat* encodeBuffer = CBufFlat::NewL( 0x400 );
    CleanupStack::PushL( encodeBuffer ); // ***
    encodeBuffer->ResizeL( 0 );
    encodeBuffer->ResizeL( 0x400 );

    // encode message type
    encodeBuffer->Write( position, &KMmsAssignedMessageType, 1 );
    position++;
    encodeBuffer->Write( position, &KMmsMessageTypeMNotificationInd, 1 );
    position++;

    // encode tid
    encodeBuffer->Write( position, &KMmsAssignedTID, 1 );
    position++;

    // generate random TID
    TPtrC8 cid;
    TBufC8<KMmsTidBuffer> target;
    TTime now;
    now.UniversalTime();
    TInt random = 0;

    // we don't generate a true random TID: We generate the
    // TID from the URL so that if we generate a notification
    // twice from the same file, we get the same TID and the
    // same URL. This way we can test the pruning function in
    // server MTM

    TInt i;
    for ( i = 0; i < aUrl.Length(); ++i )
        {
        random += aUrl[ i ];
        }

    target.Des().Num( random );
    cid.Set( target.Des() );
    
    TInt length = cid.Length();
    encodeBuffer->Write( position, cid, length );
    position += length;
    encodeBuffer->Write( position, &KMmsNull, 1 );
    position++;

    encodeBuffer->Write( position, &KMmsAssignedMmsVersion, 1 );
    position++;
    TUint8 version = iMmsVersion | 0x80;
    encodeBuffer->Write( position, &version, 1 );
    position++;

    // from is optional - we don't care

    // message class
    encodeBuffer->Write( position, &KMmsAssignedMessageClass, 1 );
    position++;
    encodeBuffer->Write( position, &KMmsMessageClassPersonal, 1 );
    position++;

    // message size
    encodeBuffer->Write( position, &KMmsAssignedMessageSize, 1 );
    position++;

    TUint8 byte;
    const TInt KConst4 = 4;
    const TInt KConst8 = 8;
    TUint8 array[KConst4];

    if ( aSize <= 0x7f )
        {
        byte = ( TInt8 ) aSize;
        byte |= 0x80;
        encodeBuffer->Write( position, &byte, 1);
        position++;
        }
    else
        {

        length = KConst4;

        TUint temp = aSize;
        byte = TInt8( ( temp >> 24 ) & 0xFF );

        while ( byte == 0 && length > 0 )
            {
            length--;
            temp = temp << KConst8;
            byte = TInt8( ( temp >> 24 ) & 0xFF );
            }

        for ( i = 0; i < length; i++ )
            {
            array[i] = TInt8( ( temp >> ( KConst8 * (3 - i) ) ) & 0xFF );
            }

        // write short length
        encodeBuffer->Write( position, &length, 1 );
        position++;

        // write as many bytes as were non-zero
        // There will be always at least one byte in the array
        // because aSize > 127 if we are here
        encodeBuffer->Write( position, &array[0], length );
        position+= length;
        }

    // expiry

    const TUint KTenHours = 10 * 60 * 60; // 10 hours
    TUint temp = KTenHours;
    encodeBuffer->Write( position, &KMmsAssignedExpiry, 1 );
    position++;

    // calculate value length

    length = KConst4;
    byte = TInt8( ( temp >> 24 ) & 0xFF );

    while ( byte == 0 && length > 0 )
       {
       length--;
       temp = temp << KConst8;
       byte = TInt8( ( temp >> 24 ) & 0xFF );
       }

    // now add short length and absolute/relative token

    length += 2;

    encodeBuffer->Write( position, &length, 1 );
    position++;

    encodeBuffer->Write( position, &KMmsRelativeToken, 1 );
    position++;

    length -= 2; // actual integer length 

    for (i = 0; i < length; ++i)
        {
        array[i] = TInt8( ( temp >> ( KConst8 * (3 - i) ) ) & 0xFF );
        }

    // write short length
    encodeBuffer->Write( position, &length, 1 );
    position++;

    // write as many bytes as were non-zero
    encodeBuffer->Write( position, &array[0], length );
    position+= length;

    // And now the salt of the notification:
    // the actual location of the message.

    encodeBuffer->Write( position, &KMmsAssignedContentLocation, 1 );
    position++;

    // Check if we need a Quote (This does not mean the quoted string.)
    if ( aUrl[0] >= 0x80 )
        {
        encodeBuffer->Write( position, &KMmsQuote, 1 );
        position++;
        }

    length = aUrl.Length();
    encodeBuffer->Write( position, aUrl, length );
    position += length;

    encodeBuffer->Write( position, &KMmsNull, 1 );
    position++;

    // DONE.

    TPtrC8 ptr;
    encodeBuffer->ResizeL( position );
    ptr.Set( encodeBuffer->Ptr(0).Left( position ) );
    HBufC8* result = ptr.AllocL();
    CleanupStack::PopAndDestroy( encodeBuffer );

    return result;
    }
#else
HBufC8* CMmsWatcher::CreateNotificationL(TDesC8& /*aUrl*/, TInt /*aSize*/)
    {
    User::Leave( KErrNotSupported );
    return NULL; // this is actually unnecessary as we always leave.
    }
#endif    

// ---------------------------------------------------------
// CMmsWatcher::GetMessagingEntriesL
// ---------------------------------------------------------
//
void CMmsWatcher::GetMessagingEntriesL( const TBool aMessageStoreHasChanged )
    {
    LOG(_L("ReadMessagingEntriesL"));
    // Connect CenRep
    CMmsSettings* settings = CMmsSettings::NewL();
    CleanupStack::PushL( settings ); // ***
    settings->LoadSettingsL();
    iMmsVersion = settings->MmsVersion();
    
    if( aMessageStoreHasChanged == EFalse )
        {
        // Read entries from CenRep
        iService = settings->Service();
        iNotificationFolder = settings->NotificationFolder();        
        }
    else 
        {
        // MessageStore has changed 
        // -> CenRep values are false and have to be updated
        iService = KMsvNullIndexEntryId;
        iNotificationFolder = KMsvNullIndexEntryId;
        }
    
    // If entries are null, create them to MessageStore
    if( iSession &&
        ( iService == KMsvNullIndexEntryId || 
        iNotificationFolder == KMsvNullIndexEntryId ) )
        {
        LOG(_L("- No service exist or message store changed"));
        LOG(_L("- Must update service id or create new"));
        settings->CreateNewServiceL( *iSession );
        iService = settings->Service();
        iNotificationFolder = settings->NotificationFolder();

        // Retest
        if( iService == KMsvNullIndexEntryId || 
        iNotificationFolder == KMsvNullIndexEntryId )
            {
            LOG(_L("- ERROR: Still no service or notification folder"));
            }
        settings->SaveSettingsL();
        }
    
    // Cleanup
    CleanupStack::PopAndDestroy( settings );
    }

//
// Following methods are only in WINS compilations
// for testing purposes
//
#ifdef __WINS__
// ---------------------------------------------------------
// CMmsWatcher::LocalModeL
// ---------------------------------------------------------
//
TBool CMmsWatcher::LocalModeL()
    {
    CRepository* repository = CRepository::NewL( KUidMmsServerMtm );
    CleanupStack::PushL( repository );
    TInt temp = 0;
    TInt retval = repository->Get( KMmsEngineLocalMode, temp );
    CleanupStack::PopAndDestroy( repository );
    if( retval == KErrNone )
        {
        return (TBool)temp;
        }
    else
        {
        LOG(_L("- ERROR reading localmode key from CenRep, assuming normal mode"));
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CMmsWatcher::ReadLocalModeConfigData
// ---------------------------------------------------------
//
void CMmsWatcher::ReadLocalModeConfigData()
    {
    RFileReadStream reader;
    TInt err = reader.Open( iFs, KMmsLocalModeConfigFile, EFileShareReadersOnly );
    if( err != KErrNone )
        {
        reader.Close();
        return;
        }

    TChar delim = 0x000A;
    TBuf<KMmsRowBufferLength> rowBuffer;
    FOREVER
        {
        TRAP( err, reader.ReadL( rowBuffer, delim ) );
        if( err == KErrEof )
            {
            reader.Close();
            return;
            }
        TInt length = rowBuffer.Length();
        if( length > 2 )
            {
            // Check for comment line
            if( rowBuffer[0] == 0x0023 ) // 0x23 == '#'
                {
                continue;
                }
            // Check for start of file (BOM)
            if( rowBuffer[0] == 0xFEFF )
                {
                rowBuffer.Delete( 0, 1 );
                length = rowBuffer.Length();
                }
            // Drop CR+LF from the end of line
            rowBuffer.Delete( length - 2, 2 );
  
            TInt separatorPosition = 0;
            _LIT( KSeparator, "=" );
            separatorPosition = rowBuffer.Find( KSeparator );
            if( separatorPosition > 0 )
                {
                if( rowBuffer.Left( separatorPosition ).CompareF( KMmsLocalmodeInDirectory ) == 0 )
                    {
                    iLocalModeIn = rowBuffer.Mid( separatorPosition+1 );
                    }
                if( rowBuffer.Left( separatorPosition ).CompareF(
                    KMmsLocalmodePollingInterval ) == 0 )
                    {
                    TLex16 lex;
                    lex.Assign( rowBuffer.Mid( separatorPosition+1 ) );
                    lex.Val( iPollingInterval );
                    iPollingInterval *= KMmsMillion;
                    }
                }
            }
        }
    }

#endif // __WINS__ emulator polling support


// ================= OTHER EXPORTED FUNCTIONS ==============

//
// ---------------------------------------------------------
// Panic implements
// panic, for debug version only
//
GLDEF_C void gPanic(
    TMmsPanic aPanic ) // error number enumerations
    {
    _LIT( KMmsPanic,"MMS" );
    User::Panic( KMmsPanic, aPanic );
    }

//  End of File  
