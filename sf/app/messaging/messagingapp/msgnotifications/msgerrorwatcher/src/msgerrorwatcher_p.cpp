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
*     CMsgErrorWatcherPrivate implementation file
*
*/



// INCLUDE FILES
#include <e32base.h>

#include <msvapi.h>
#include <msvids.h>          //Msv index entry ID's
#include <msvuids.h>
#include <mmscmds.h>         //EMmsScheduledReceiveForced
#include <mmsconst.h>        //Notification folder
#include <SenduiMtmUids.h>   //MTM Uids
#include <mtclreg.h>         //ClientMtmRegistry
#include <watcher.h>
#include <mmserrors.h>
#include <mmsclient.h>
#include <mmssettings.h>



#include <xqconversions.h>
#include <hbglobal.h>
#include <HbTranslator>
#include <QString>
#include <in_iface.h>            // KErrIfAuthenticationFailure
#include <etelpckt.h>            // KErrGprsInsufficientResources, etc.
//#include <exterror.h>            // KErrGsmMMServiceOptionTemporaryOutOfOrder
#include <gsmerror.h>

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys

#include <messagingvariant.hrh>  // Variation

#include "msgerrorwatcher.h"
#include "msgerrorwatcher_p.h"
#include "msgerrorwatcher.hrh"
#include "msgerrorcommdbobserver.h"
#include "msgcenrepobserver.h"
// this should be handled later,
//#include "msgerrorconnectionobserver.h"
#include "msgerrordiskspaceobserver.h"
#include "msgerrorsmsdiskspaceobserver.h"
#include "msgerrorextsmsdiskspaceobserver.h"
#include "msgerrorroamingobserver.h"
#include "msgerrorstartupobserver.h"



#include "debugtraces.h"

//const TInt KConnectionRetries = 3;
//const TInt KIntMaxLength = 10; //2~32 =~ 4000000000
const TUint KInitialDelayBetweenSessionConnectRetries = 5000000; //five seconds
const TUint KMaxTimerRetries = 50;
//const TInt KDelayAfterDisconnect = 3000000; //3 seconds
//Total delay between first and last retry is
//( A * (B^2 + B) ) / 2
// - where A is KInitialDelayBetweenSessionConnectRetries
// -   and B is KMaxTimerRetries
//If A = 5 seconds and B = 50 times last retry is made
//after about 106 minutes from the first one.
//const TMsvId KWatcherInboxFolderId = KMsvGlobalInBoxIndexEntryIdValue;
const TMsvId KWatcherOutboxFolderId = KMsvGlobalOutBoxIndexEntryIdValue;
//const TUid KMessageEntryUid = { KUidMsvMessageEntryValue };


CMsgErrorWatcherPrivate::CMsgErrorWatcherPrivate(MsgErrorWatcher* msgErrorWatcher  ): 
CActive( EPriorityStandard ),
    q_ptr(msgErrorWatcher),
    iMmsServiceId( KMsvNullIndexEntryId ),
    iNotificationFolderId( KMsvNullIndexEntryId ),
    iCurrentEntryId ( KMsvNullIndexEntryId )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:CMsgErrorWatcherPrivate : Enter");
    CActiveScheduler::Add( this );
    TRAP_IGNORE(InitL())
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:CMsgErrorWatcherPrivate : Exit");
    
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::CMsgErrorWatcherPrivate
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::InitL()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:InitL : Enter");
    User::LeaveIfError( iTimer.CreateLocal() );
    User::LeaveIfError(iFs.Connect());

    TInt features = 0;   
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, features );
    delete repository;
    repository = NULL;
    if ( features & KMmsFeatureIdDelayDisconnectDialog )
        {
        iWatcherFlags |= EReceivingDisconnectDelay;
        }

    StartWatcherL();
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:InitL : Exit");
    }



    
// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::~CMsgErrorWatcherPrivate
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorWatcherPrivate::~CMsgErrorWatcherPrivate()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Enter");   
    StopWatcher();
    iTimer.Close();
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Exit");  
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::StartWatcherL
//
// Does the actual construction of the watcher.
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::StartWatcherL()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:StartMmsFetchL : Enter"); 
    iSession = CMsvSession::OpenSyncL( *this );
    // might be required in future for propogating the errors to the ui.
    iMmsReceiveErrorMessages = new ( ELeave ) CMsvEntrySelection;
    iMmsSendErrorMessages = new ( ELeave ) CMsvEntrySelection;
    
    // Get message server session
    iClientMtmRegistry = CClientMtmRegistry::NewL( *iSession );
    TRAPD( err,
        {
        iMmsClient = static_cast<CMmsClientMtm*>
            ( iClientMtmRegistry->NewMtmL( KUidMsgTypeMultimedia ) );
        } );
    if ( err == KErrNotFound )
        {
        QDEBUG_WRITE( "MmsClientMtm not found!" );
        // Make sure iMmsClient is NULL - just in case.
        delete iMmsClient;
        iMmsClient = NULL;
        }
    else
        {
        User::LeaveIfError( err );
        }

    if ( iMmsClient )
        {
        // We're interested in roaming events only if MMS is enabled.
        iRoamingObserver = CMsgErrorRoamingObserver::NewL( *this ); 
        QDEBUG_WRITE( "Created Roaming Observer!" );
        }


    iStartupObserver = CMsgErrorStartupObserver::NewL( *this );
    
    if(!( iWatcherFlags & EWatcherRunning))
        {
        //if not yet created
        iSmsDiskSpaceObserver = CMsgErrorSmsDiskSpaceObserver::NewL( *this );
        iSmsExtDiskSpaceObserver = CMsgErrorExtSmsDiskSpaceObserver::NewL( *this );
        QDEBUG_WRITE( "Created iSmsDiskSpaceObserver, iSmsExtDiskSpaceObserver!" );
        }

    iWatcherFlags |= EWatcherRunning;

    if ( GetMmsServiceL() )
        {
        CheckMmsReceivingModeL();
        }

    // Startup successful if we got this far. Reset retry counter.
    iTimerRetries = 0;
    
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:StartWatcherL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::StopWatcher
//
// Stops (and destructs) the watcher.
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::StopWatcher()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Enter");
    iWatcherFlags &= ~EWatcherRunning;
    iWatcherFlags &= ~ENoAPErrorPending;
    iMmsServiceId = KMsvNullIndexEntryId;
    iNotificationFolderId = KMsvNullIndexEntryId;
    Cancel();

    delete iCommDbObserver;
    iCommDbObserver = NULL;
    delete iCenRepObserver;
    iCenRepObserver = NULL;
//    delete iConnectionObserver;
//    iConnectionObserver = NULL;
    delete iDiskSpaceObserver;
    iDiskSpaceObserver = NULL;
    delete iSmsDiskSpaceObserver;
    iSmsDiskSpaceObserver = NULL;
    delete iSmsExtDiskSpaceObserver;
    iSmsExtDiskSpaceObserver = NULL;    
    delete iRoamingObserver;
    iRoamingObserver = NULL;
    delete iMmsClient;
    iMmsClient = NULL;
    delete iClientMtmRegistry;
    iClientMtmRegistry = NULL;
    delete iOperation;
    iOperation = NULL;
    delete iStartupObserver;
    iStartupObserver = NULL;
    delete iSession;
    iSession = NULL;
    delete iMmsReceiveErrorMessages;
    iMmsReceiveErrorMessages = NULL;
    delete iMmsSendErrorMessages;
    iMmsSendErrorMessages = NULL;

    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::StartRestartTimer
//
// Start session reconnect timer.
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::StartRestartTimer()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Enter");
    if ( !IsActive() ) 
        {
        QDEBUG_WRITE( "Starting timer" );
        iStatus = KRequestPending;
        iRequestType = EMsgRequestStartingUp;
        iTimerRetries++;
        //The interval between retries comes longer every time
        iTimer.After( iStatus,
            iTimerRetries * KInitialDelayBetweenSessionConnectRetries );
        SetActive();
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::ResetWatcher
//
// Resets watcher.
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::ResetWatcher()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Enter");

    iWatcherFlags &= ~ENoAPErrorPending;
    
    //Drop all observers
    delete iCommDbObserver;
    iCommDbObserver = NULL;
    delete iCenRepObserver;
    iCenRepObserver = NULL;
//    delete iConnectionObserver;
//    iConnectionObserver = NULL;
    delete iDiskSpaceObserver;
    iDiskSpaceObserver = NULL;
	delete iSmsDiskSpaceObserver;
    iSmsDiskSpaceObserver = NULL;
    delete iSmsExtDiskSpaceObserver;
    iSmsExtDiskSpaceObserver = NULL;    
    //Reset disk space errors
    iDiskSpaceErrors = 0;
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::GetMmsServiceL
//
// Retrieves MMS service id from MsgStore
// ---------------------------------------------------------
//
TBool CMsgErrorWatcherPrivate::GetMmsServiceL()
    {
    if ( !iMmsClient )
        {
        return EFalse;
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Enter");
    if ( iMmsServiceId == KMsvNullIndexEntryId )
        {
        QDEBUG_WRITE( "Looking for MMS service" );
        iMmsClient->RestoreSettingsL();
        iMmsServiceId = iMmsClient->MmsSettings().Service();
        iNotificationFolderId = iMmsClient->MmsSettings().NotificationFolder();
        iMaxReceiveSize = iMmsClient->MmsSettings().MaximumReceiveSize();
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Exit");
    return ( iMmsServiceId != KMsvNullIndexEntryId );
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::StartMmsFetchL
//
// Initiates MMS fetch
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::StartMmsFetchL()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Enter");
    if ( !IsActive() && iMmsReceiveErrorMessages->Count() )
        {
        QDEBUG_WRITE_FORMAT("Fetching  message(s): ",
            iMmsReceiveErrorMessages->Count() );
        TCommandParameters parameters; // initialized to zero
        TCommandParametersBuf paramPack( parameters );

        //Add service entry as the first entry in the array
        iMmsReceiveErrorMessages->InsertL( 0, iMmsServiceId );

        // First remove existing schedules:
        QDEBUG_WRITE( "Deleting old schedules" );
        // Calling synchronous TransferCommandL method
        const TInt KBufSize = 256;
        TBuf8<KBufSize> dummy;
        iSession->TransferCommandL( *iMmsReceiveErrorMessages,
            EMmsDeleteSchedule,
            paramPack,
            dummy );
        QDEBUG_WRITE( "Old schedules deleted!" );

        // Then reschedule:
        iStatus = KRequestPending;
        iRequestType = EMsgRequestFetching;
        delete iOperation;
        iOperation = NULL;
        QDEBUG_WRITE( "Request fetch" );
        iOperation = iSession->TransferCommandL( *iMmsReceiveErrorMessages,
            EMmsScheduledReceive,
            paramPack,
            iStatus );
        SetActive();
        QDEBUG_WRITE( "Reset array" );
        iMmsReceiveErrorMessages->Reset();
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::StartMmsSendL
//
// 
// ---------------------------------------------------------
// TODO: to be handled late when connection errors are handled.
//void CMsgErrorWatcherPrivate::StartMmsSendL()
//    {
//   QDEBUG_WRITE("CMsgErrorWatcherPrivate:StartMmsSendL : Enter");
//    if ( !IsActive() && iMmsSendErrorMessages->Count() )
//        {
//        QDEBUG_WRITE_FORMAT("Sending message(s): "),
//            iMmsSendErrorMessages->Count() );
//        TCommandParameters parameters; // initialized to zero
//        TCommandParametersBuf paramPack( parameters );
//
//        //Add service entry as the first entry in the array
//        iMmsSendErrorMessages->InsertL( 0, iMmsServiceId );
//
//        iStatus = KRequestPending;
//        iRequestType = EMsgRequestSending;
//        delete iOperation;
//        iOperation = NULL;
//        QDEBUG_WRITE( "Request send" );
//        iOperation = iSession->TransferCommandL( *iMmsSendErrorMessages,
//            EMmsScheduledSend,
//            paramPack,
//            iStatus );
//        SetActive();
//        QDEBUG_WRITE( "Reset array" );
//        iMmsSendErrorMessages->Reset();
//        }
//     QDEBUG_WRITE("CMsgErrorWatcherPrivate:StartMmsSendL : Exit");
//    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::CheckMmsReceivingModeL
//
// Checks MMS receiving mode
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::CheckMmsReceivingModeL()
    {
    if ( !( iWatcherFlags & EStartupReady ) || !GetMmsServiceL() )
        {
        return;
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:CMsgErrorWatcherPrivate : Enter");
    TBool validAP = ValidateMmsServiceL(); //Refreshes MMS settings
    

    TInt fetchHome = iMmsClient->MmsSettings().ReceivingModeHome();
    TInt fetchRoam = iMmsClient->MmsSettings().ReceivingModeForeign();
    QDEBUG_WRITE_FORMAT("ReceivingModeHome: ", fetchHome );
    QDEBUG_WRITE_FORMAT("ReceivingModeForeign: ", fetchRoam );

    if ( validAP &&
        fetchRoam == EMmsReceivingReject &&
        fetchHome != EMmsReceivingReject )
        {
        QDEBUG_WRITE( "ShowNote flag enabled" );
        iWatcherFlags |= EShowRoamingNote;
        }
    else
        {
        //Reset roaming note flag
        QDEBUG_WRITE( "ShowNote flag disabled" );
        iWatcherFlags &= ~EShowRoamingNote;
        iWatcherFlags &= ~ERoamingNoteShown;
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::ValidateMmsServiceL
//
// Validates MMS service
// ---------------------------------------------------------
//
TBool CMsgErrorWatcherPrivate::ValidateMmsServiceL()
    {
//    if ( !GetMmsServiceL() )
//        {
//        return EFalse;
//        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:ValidateMmsServiceL : Enter");
    iMmsClient->RestoreSettingsL(); //Refreshes MMS settings
    QDEBUG_WRITE_FORMAT("ValidateService: ", iMmsClient->ValidateService( iMmsServiceId ) );

    TInt errorCode = iMmsClient->ValidateService( iMmsServiceId );
    QDEBUG_WRITE_FORMAT("ValidateService: errorcode ---  ", errorCode );
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:ValidateMmsServiceL : Exit");
    return ( errorCode == KErrNone );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleNoAPErrorL
//
// Handles "no access point defined" error
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleNoAPErrorL( TMsvEntry& aEntry )
    { 
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleNoAPErrorL : Enter");
    iMmsReceiveErrorMessages->AppendL( aEntry.Id() );

    iWatcherFlags |= ENoAPErrorPending;
    if ( !iCenRepObserver )
        {
        QDEBUG_WRITE( "Creating CenRep observer" );
        iCenRepObserver = CMsgCenRepObserver::NewL( *this );
        iCenRepObserver->SubscribeNotification();
        }
    //Let's reset the TMsvEntry::iError to get rig of excess warning note
    ResetErrorFieldL( aEntry );
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleNoAPErrorL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleInvalidAPErrorL
//
// Handles "invalid access point" error
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleInvalidAPErrorL( TMsvEntry& aEntry, TBool aStartObserver )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleInvalidAPErrorL : Enter");
    iMmsReceiveErrorMessages->AppendL( aEntry.Id() );
   
    q_ptr->ShowNote(EInvalidAccessPointNote);
    if ( aStartObserver && !iCommDbObserver )
        {
        QDEBUG_WRITE( "Creating CommDB observer" );
        iCommDbObserver = CMsgErrorCommDbObserver::NewL( *this );
        }
    if ( aStartObserver && !iCenRepObserver )
        {
        //We must also start cenrep observer
        QDEBUG_WRITE( "Creating CenRep observer" );
        iCenRepObserver = CMsgCenRepObserver::NewL( *this );
        iCenRepObserver->SubscribeNotification();
        }    
    
    //Let's reset the TMsvEntry::iError to get rig of excess warning note
    ResetErrorFieldL( aEntry ); 
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleInvalidAPErrorL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleConnectionErrorL
//
// Handles "connection reserved" error
// ---------------------------------------------------------
// TODO: to be handled later.
//void CMsgErrorWatcherPrivate::HandleConnectionErrorL( TMsvEntry& aEntry, TBool aReceive )
//    {
//        QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleConnectionErrorL : Enter");
//    if ( !iConnectionObserver )
//        {
//        QDEBUG_WRITE( "Creating connection observer" );
//        iConnectionObserver = CMsgErrorConnectionObserver::NewL( *this );
//        }
//    if ( iConnectionObserver->ConnectionsOpen() )
//        {
//        QDEBUG_WRITE( "Open connections detected" );
//        if ( aReceive )
//            {
//            iMmsReceiveErrorMessages->AppendL( aEntry.Id() );
//            }
//        else
//            {
//            iMmsSendErrorMessages->AppendL( aEntry.Id() );
//            }
//        //No matter if already started
//        iConnectionObserver->StartL();
//        }
//    else
//        {
//        QDEBUG_WRITE( "No open connections" );
//        delete iConnectionObserver;
//        iConnectionObserver = NULL;
//        }
//    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleConnectionErrorL : Exit");
//    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleDiskSpaceErrorL
//
// Handles "no disk space" error
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleDiskSpaceErrorL( TMsvEntry& aEntry )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleDiskSpaceErrorL : Enter");
    iMmsReceiveErrorMessages->AppendL( aEntry.Id() );

    iDiskSpaceErrors++;
    if ( iDiskSpaceErrors == 1 )
        {
        if ( !( iWatcherFlags & EStartupReady ) )
            {
            return;
            }
        QDEBUG_WRITE( "Show note" );
        q_ptr->ShowNote(EDiskLowNote1);
        }
    TUint triggerLevel = Max( KDefaultTriggerLevel, iMaxReceiveSize );
    //Activate DiskSpace observer
    if ( !iDiskSpaceObserver )
        {
       QDEBUG_WRITE( "Creating disk space observer" );
        iDiskSpaceObserver = CMsgErrorDiskSpaceObserver::NewL( *this, *iSession, iFs );
        }
       QDEBUG_WRITE_FORMAT("Limit set to: ",
        KCriticalLevel + triggerLevel + KTriggerMargin );
    iDiskSpaceObserver->SetLimitAndActivateL( KCriticalLevel +
        triggerLevel +
        KTriggerMargin );
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleDiskSpaceErrorL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleMemoryErrorL
//
// Handles "no memory" error
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleMemoryErrorL( TMsvEntry& aEntry )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleMemoryErrorL : Enter");
    iMmsReceiveErrorMessages->AppendL( aEntry.Id() );
    if ( !( iWatcherFlags & EStartupReady ) )
        {
        return;
        }
    q_ptr->ShowNote(EMemoryLowNote);
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleMemoryErrorL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleRoamingEventL
//
// Handles events from roaming observer
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleRoamingEventL( TBool aRoaming )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleRoamingEventL : Enter");
    QDEBUG_WRITE_FORMAT("aRoaming : ", aRoaming );
    if ( !iMmsClient )
        {
        // We should never get here if MMS Client MTM is not present
        // since roaming observer is not started in that case.
        // This return is here just in case...
        return;
        }

    TInt fetchHome = iMmsClient->MmsSettings().ReceivingModeHome();
    TInt fetchRoam =  iMmsClient->MmsSettings().ReceivingModeForeign();
    QDEBUG_WRITE_FORMAT("HandleRoamingEventL fetchHome : ", fetchHome );
    QDEBUG_WRITE_FORMAT("HandleRoamingEventL fetchRoam : ", fetchRoam );
    
    TBool fetchAll = EFalse;

    if ( aRoaming )
        {
        QDEBUG_WRITE( "we are in roaming." );
        //We are in roaming network
        if ( fetchRoam == EMmsReceivingAutomatic &&
            fetchHome != EMmsReceivingAutomatic )
            {
            fetchAll = ETrue;
            }
        if ( ( iWatcherFlags & EShowRoamingNote ) &&
            !( iWatcherFlags & ERoamingNoteShown ) )
            {
            //Show roaming note if requested
            if ( !( iWatcherFlags & EStartupReady ) )
                   {
                   return;
                   }
            q_ptr->ShowNote(ERoamingNote);
            iWatcherFlags |= ERoamingNoteShown;
            }
        }
    else
        {
        QDEBUG_WRITE( "we are in home network." );
        //We are in home network
        if ( fetchHome == EMmsReceivingAutomatic &&
            fetchRoam != EMmsReceivingAutomatic )
            {
            fetchAll = ETrue;
            }
        //Reset roaming note flag
        iWatcherFlags &= ~ERoamingNoteShown;
        }

    if ( fetchAll && !IsActive() )
        {
        QDEBUG_WRITE( "starting fetch all" );
        iStatus = KRequestPending;
        iRequestType = EMsgRequestFetchingAll;
        delete iOperation;
        iOperation = NULL;
        iOperation = iMmsClient->FetchAllL( iStatus, EFalse );
        SetActive();
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:~CMsgErrorWatcherPrivate : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleCommDbEventL
//
// Handles events from CommDB observer
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleCommDbEventL()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleCommDbEventL : Enter");
    if ( ValidateMmsServiceL() )
        {
        QDEBUG_WRITE( "starting fetch" );
        StartMmsFetchL();
        }
    else
        {
        //Wait for another event
        QDEBUG_WRITE( "restart CommDB observer" );
        iCommDbObserver->Restart();
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleCommDbEventL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleConnectionEvent
//
// Handles events from connection observer
// ---------------------------------------------------------
// TODO: handel later.
//void CMsgErrorWatcherPrivate::HandleConnectionEvent()
//    {
//    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleConnectionEvent : Enter");
//    if ( !IsActive() )
//        {
//        MEWLOGGER_WRITE( "HandleConnectionEvent, Starting delay timer" );
//        iStatus = KRequestPending;
//        iRequestType = EMsgRequestWaitingDisconnection;
//        iTimer.After( iStatus, KDelayAfterDisconnect );
//        SetActive();
//        }
//   QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleConnectionEvent : Exit");
//    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleDiskSpaceEventL
//
// Handles events from disk space observer
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleDiskSpaceEventL()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleDiskSpaceEventL : Enter");
    QDEBUG_WRITE( "starting fetch" );
    StartMmsFetchL();
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleDiskSpaceEventL : Exit");
    }


// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleDiskSpaceEvent2L
//
// Handles events from disk space observer
// -------k--------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleDiskSpaceEvent2L()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleDiskSpaceEvent2L : Enter");
    // erro note text was coming form avkon.
    QDEBUG_WRITE( "show note" );
    q_ptr->ShowNote(ESMSIncomingLowDiskSpace);
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleDiskSpaceEvent2L : Exit");
    
    }


// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleStartupReadyL
//
// Handles events from startup state observer (currently 
// CMsgSentItemsObserver)
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleStartupReadyL()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleStartupReadyL : Enter");
    QDEBUG_WRITE( "Startup ready!" );
    iWatcherFlags |= EStartupReady;
    CheckMmsReceivingModeL();
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleStartupReadyL : Exit");
    }


// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleCenRepNotificationL
//
// Handles events from Central Repository observer
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleCenRepNotificationL()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleCenRepNotificationL : Enter");
    if ( ValidateMmsServiceL() )
        {
        QDEBUG_WRITE( "starting fetch" );
        StartMmsFetchL();
        }
    else
        {
        //Wait for another event
        QDEBUG_WRITE( "restart CenRep observer" );
        iCenRepObserver->SubscribeNotification();
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleCenRepNotificationL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleSessionEventL
//
// Handles events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleSessionEventL( TMsvSessionEvent aEvent,
                                           TAny* aArg1,
                                           TAny* aArg2,
                                           TAny* aArg3 )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleSessionEventL : Enter");
    if ( ( aEvent == EMsvCloseSession ||
        aEvent == EMsvServerTerminated ||
        aEvent == EMsvMediaUnavailable ||
        aEvent == EMsvMediaChanged ) &&
        iWatcherFlags & EWatcherRunning )
        {
        QDEBUG_WRITE_FORMAT("StopWatcher event: ", aEvent );
        StopWatcher();
        //Start retry timer
        StartRestartTimer();
        return;
        }
    if ( aEvent == EMsvServerReady &&
        !( iWatcherFlags & EWatcherRunning ) )
        {
        TRAPD ( err, StartWatcherL() );
        if ( err ) //make sure watcher is not left in obscure state
            {
            StopWatcher();
            }
        return;
        }
    if ( aArg1 == 0 || aArg2 == 0 || !( iWatcherFlags & EWatcherRunning ) )
        {
        return;
        }
    // If for some reason MMS service is not yet found,
    // we try to find it now...
    GetMmsServiceL();

    CMsvEntrySelection* entries = static_cast<CMsvEntrySelection*>( aArg1 );
    TInt count = entries->Count();
    
    // Mark the _original_ folder as parent for "entries moved" events (in "aArg3").
    // For other events the parent is in "aArg2".
    TMsvId parentId = ( aEvent == EMsvEntriesMoved )
        ? *( static_cast<TMsvId*>( aArg3 ) )
        : *( static_cast<TMsvId*>( aArg2 ) );
    
    if ( count < 1 )
        {
        return;
        }
    if ( parentId == KMsvRootIndexEntryIdValue &&
        iMmsServiceId != KMsvNullIndexEntryId )
        {
        // We're not interested in these events if MMS Service is not present.
        HandleRootEventL( aEvent, entries );
        }
    else if ( parentId == KMsvLocalServiceIndexEntryIdValue )
        {
        HandleLocalServiceEventL( aEvent, entries );
        }
    else if ( parentId == KWatcherOutboxFolderId )
        {
        HandleOutboxEventL( aEvent, entries );
        }
    else if ( ( iMmsServiceId != KMsvNullIndexEntryId && parentId == iMmsServiceId ) ||
        ( iNotificationFolderId != KMsvNullIndexEntryId && parentId == iNotificationFolderId ) )
        {
        HandleMmsServiceEventL( aEvent, entries );
        }
    else
        {
        //do nothing
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleSessionEventL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleRootEventL
//
// Handles root events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleRootEventL( TMsvSessionEvent aEvent,
                                        CMsvEntrySelection* aEntries )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleRootEventL : Enter");
    QDEBUG_WRITE_FORMAT("aEvent : ",aEvent);
    switch ( aEvent )
        {
        case EMsvEntriesChanged:
            {
            TInt count = aEntries->Count();
            TInt i = 0;
            while ( i < count )
                {
                TMsvId dummy;
                TMsvEntry entry;
                TInt error = iSession->GetEntry(
                    aEntries->At( i ), dummy, entry );

                //We're only interested in MMS service
                if ( !error && 
                    iMmsServiceId != KMsvNullIndexEntryId &&
                    entry.Id() == iMmsServiceId )
                    {
                    QDEBUG_WRITE( "HandleRootEventL, MMS service changed" );
                    //Check whether the roaming setting has changed
                    CheckMmsReceivingModeL();

                    //We're waiting for the the user to change access points
                    //if iCommDbObserver exists
                    if ( iCommDbObserver && ValidateMmsServiceL() )
                        {
                        QDEBUG_WRITE( "HandleRootEventL, deleting CommDB observer" );
                        StartMmsFetchL();
                        }
                    }
                i++;
                }
            break;
            }
        default:
            break;
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleRootEventL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleLocalServiceEventL
//
// Handles local service events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleLocalServiceEventL( TMsvSessionEvent aEvent,
                                                CMsvEntrySelection* aEntries )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleLocalServiceEventL : Enter");
    if ( iNotificationFolderId == KMsvNullIndexEntryId &&
        aEvent == EMsvEntriesCreated )
        {
        TInt count = aEntries->Count();
        TInt i = 0;
        while ( i < count )
            {
            TMsvId dummy;
            TMsvEntry entry;
            TInt error = iSession->GetEntry(
                aEntries->At( i ), dummy, entry );
            if ( !error &&
                entry.iDetails.Compare( KMMSNotificationFolder ) == 0 )
                {
                iNotificationFolderId = aEntries->At( i );
               QDEBUG_WRITE_FORMAT("Notification folder created: ", iNotificationFolderId );
                }
            i++;
            }
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleLocalServiceEventL : Exit");
    }



// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleOutboxEventL
//
// Handles outbox events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleOutboxEventL( TMsvSessionEvent aEvent,
                                          CMsvEntrySelection* aEntries )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleOutboxEventL : Enter");
    TInt count = aEntries->Count();
    QDEBUG_WRITE_FORMAT("aEvent: ",aEvent);
    switch (aEvent)
        {
        case EMsvEntriesChanged:
               {
//               MEWLOGGER_WRITE("HandleOutboxEventL: case: EMsvEntriesChanged start");
//               TInt i = 0;
//               MEWLOGGER_WRITEF( _L("EMsvEntriesChanged count: %d"), count );
//               while ( i < count )
//                   {
//                   MEWLOGGER_WRITE( "EMsvEntriesChanged - inside while" );
//                   TMsvId dummy;
//                   TMsvEntry entry;
//                   TInt error = iSession->GetEntry(
//                       aEntries->At( i ), dummy, entry );
//                   TUid mtm = entry.iMtm;
//                   MEWLOGGER_WRITEF( _L("EMsvEntriesChanged - mtm ---  %d"), mtm );
//                   MEWLOGGER_WRITEF( _L("EMsvEntriesChanged - entry id ---  %d"), iMmsSendErrorMessages->Find( entry.Id() ));
//                   if ( mtm == KSenduiMtmMmsUid &&
//                       iMmsSendErrorMessages->Find( entry.Id() ) == KErrNotFound )
//                       {
//                       MEWLOGGER_WRITE( "EMsvEntriesChanged - inside if" );
//                       if ( // the first error is activated again to synchronize
//                           // with connection manager (MPM) 
//                           entry.iError == KErrPacketDataTsyMaxPdpContextsReached ||
//                           entry.iError == KErrUmtsMaxNumOfContextExceededByPhone ||
//                           entry.iError == KErrUmtsMaxNumOfContextExceededByNetwork ||
//                           // add the following error to the list to synchronize with 
//                           // connection manager (MPM)
//                           entry.iError == KErrGprsInsufficientResources )
//                           {
//                           MEWLOGGER_WRITE( "MMS send - connection active" );
//                           
//                           //Let's now save the id. This way we can reset the entrys
//                           //error field just before the fetch start. This prevents
//                           //duplicate error notes because this case branch is reached many times.
//                           iCurrentEntryId = entry.Id();
//                           
//                           HandleConnectionErrorL( entry, EFalse );
//                           }
//                       }
//                   i++;
//                   }
             break;      
             }
        case EMsvEntriesMoved: // Messages are moved _from_ this folder.
        case EMsvEntriesDeleted:
            {
            TInt i = 0;
            while ( i < count )
                {
                TInt selectionId = iMmsSendErrorMessages->Find( aEntries->At( i ) );
                if ( selectionId != KErrNotFound )
                    {
                    iMmsSendErrorMessages->Delete( selectionId );
                    }
                i++;
                }
            }
            break;
        default:
            break;
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleOutboxEventL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::HandleMmsServiceEventL
//
// Handles MMS service events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::HandleMmsServiceEventL( TMsvSessionEvent aEvent,
                                              CMsvEntrySelection* aEntries )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleMmsServiceEventL : Enter");
    TInt count = aEntries->Count();
    QDEBUG_WRITE_FORMAT("aEvent: ", aEvent );
    switch (aEvent)
        {
        case EMsvEntriesChanged:
            {
            TInt i = 0;
            while ( i < count )
                {
                TMsvId dummy;
                TMsvEntry entry;
                TInt error = iSession->GetEntry(
                    aEntries->At( i ), dummy, entry );

                if ( !error )
                    {
                    QDEBUG_WRITE_FORMAT("FetchState: ", entry.SendingState() );
                    QDEBUG_WRITE_FORMAT("Error: ", entry.iError );
                    QDEBUG_WRITE_FORMAT("Retries: ", entry.iMtmData3 );
                    QDEBUG_WRITE_FORMAT("Failed: ", (TInt)entry.Failed() );
                    QDEBUG_WRITE_FORMAT("ArrayId: ", iMmsReceiveErrorMessages->Find( entry.Id() ) );
                    }
                //Check that reception has failed and that the entry is not
                //already in iMmsReceiveErrorMessages
                if ( !error &&
                    iMmsReceiveErrorMessages->Find( entry.Id() ) == KErrNotFound )
                    {
                    TInt entryErr = entry.iError;
                    if ( entryErr == KErrGprsMissingorUnknownAPN )
                        {
                        // Map to "invalid ap" error.
                        entryErr = KMmsErrorAP1Invalid;
                        }
                    QDEBUG_WRITE_FORMAT("entryErr: ", entryErr );
                    switch ( entryErr )
                        {
                        case KErrDiskFull:
                            {
                            HandleDiskSpaceErrorL( entry );
                            }
                            break;
                        case KErrNoMemory:
                            {
                            HandleMemoryErrorL( entry );
                            }
                            break;
                          // TODO: to be handled later.
                         // the first error is activated again to synchronize
                         // with connection manager (MPM)
//                        case KErrPacketDataTsyMaxPdpContextsReached:
//                        case KErrUmtsMaxNumOfContextExceededByPhone:
//                        case KErrUmtsMaxNumOfContextExceededByNetwork:
//                        // add the following error to the list to synchronize with 
//                        // connection manager (MPM)
//                        case KErrGprsInsufficientResources:
//                            {
//                            //Let's now save the id. This way we can reset the entrys
//                            //error field just before the fetch start. This prevents
//                            //duplicate error notes because this case branch is reached many times.
//                            iCurrentEntryId = entry.Id();
//                            //Connection already active should be "detected"
//                            //only after third retry failure if "disconnect
//                            //delay" feature is activated.
//                            TInt retries = ( iWatcherFlags & EReceivingDisconnectDelay )
//                                ? KConnectionRetries
//                                : 0;
//                            if ( ( entry.iMtmData3 & KMmsRetryCountMask ) >= retries ) //lint !e574
//                                {
//                                MEWLOGGER_WRITE( "MMS fetch - connection active" );
//                                
//                                
//                                HandleConnectionErrorL( entry, ETrue );
//                                }
//                            }
//
//                            break;

                        case KMmsErrorNoWAPAccessPoint:
                            {
                            HandleNoAPErrorL( entry );
                            }
                            break;
                        case KMmsErrorAP1Invalid:
                        case KMmsErrorNoURI1:
                            {
                            HandleInvalidAPErrorL( entry, ETrue );
                            }
                            break;
                        case KErrIfAuthenticationFailure: //username/passwd
                            {
                            HandleInvalidAPErrorL( entry, EFalse );
                            }
                            break;
                        default:
                            //nothing to do
                            break;
                        }
                    }
                i++;
                }
            }
            break;
        case EMsvEntriesMoved: // Messages are moved _from_ this "folder".
        case EMsvEntriesDeleted:
            {
            TInt i = 0;
            TInt originalCount = iMmsReceiveErrorMessages->Count();
            while ( i < count )
                {
                TInt selectionId = iMmsReceiveErrorMessages->Find( aEntries->At( i ) );
                if ( selectionId != KErrNotFound )
                    {
                    iMmsReceiveErrorMessages->Delete( selectionId );
                    }
                i++;
                }
            if ( originalCount && !iMmsReceiveErrorMessages->Count() )
                {
                // array was emptied
                ResetWatcher();
                }
            }
            break;
        default:
            break;
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:HandleMmsServiceEventL : Enter");
    }


// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::ResetErrorFieldL
//
// Reset TMsvEntry::iError of the current notification
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::ResetErrorFieldL( )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:ResetErrorFieldL : Enter");
    //Makes sure the entry is set
    if( iCurrentEntryId != KMsvNullIndexEntryId )
        {
        CMsvEntry *cEntry( NULL );
        TRAPD( err, cEntry = iSession->GetEntryL( iCurrentEntryId ) );
        if ( err == KErrNotFound )
        	{
        	iCurrentEntryId = KMsvNullIndexEntryId;
        	return;
        	}
        CleanupStack::PushL( cEntry );
        TMsvEntry tEntry = cEntry->Entry();
        tEntry.iError = KErrNone;
        cEntry -> ChangeL( tEntry );
        CleanupStack::PopAndDestroy( cEntry );
        //This prevents getting here to often
        iCurrentEntryId = KMsvNullIndexEntryId;
        } 
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:ResetErrorFieldL : Exit");
    }
    
// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::ResetErrorFieldL
//
// Reset TMsvEntry::iError 
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::ResetErrorFieldL( TMsvEntry& aEntry )
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:ResetErrorFieldL() : Enter");
    CMsvEntry *cEntry = iSession->GetEntryL( aEntry.Id() );  
    CleanupStack::PushL( cEntry );
    aEntry.iError = KErrNone;
    cEntry -> ChangeL( aEntry );
    CleanupStack::PopAndDestroy( cEntry ); 
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:ResetErrorFieldL() : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::DoCancel()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:DoCancel : Enter");
    iTimer.Cancel();
    if ( iOperation )
        {
        iOperation->Cancel();
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:DoCancel : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorWatcherPrivate::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorWatcherPrivate::RunL()
    {
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:RunL : Enter");
    QDEBUG_WRITE_FORMAT("RunL, iStatus: ", iStatus.Int() );
    QDEBUG_WRITE_FORMAT("RunL, iRequestType: ", iRequestType );
    QDEBUG_WRITE_FORMAT("RunL, EMsgRequestSending - iMmsReceiveErrorMessages: ", iMmsReceiveErrorMessages->Count() );
    
    switch ( iRequestType )
        {
        case EMsgRequestStartingUp:
            {
            if ( !( iWatcherFlags & EWatcherRunning ) )
                {
                TRAPD ( err, StartWatcherL() );
                if ( err ) //make sure watcher is not left in obscure state
                    {
                    QDEBUG_WRITE_FORMAT("Leave from StartWatcherL: %d", err );
                    StopWatcher();
                    if ( iTimerRetries < KMaxTimerRetries )
                        {
                        StartRestartTimer();
                        }
                    //else give up
                    }
                }
            }
            break;
        case EMsgRequestSending:
            {
            if ( iMmsReceiveErrorMessages->Count() )
                {
                StartMmsFetchL();
                }
            else
                {
//                delete iConnectionObserver;
//                iConnectionObserver = NULL;
                ResetErrorFieldL();
                }
            break;
            }
        case EMsgRequestFetching:
            {
            ResetErrorFieldL();
            ResetWatcher();
            break;
            }
//       TODO: to be handled later.
//        case EMsgRequestWaitingDisconnection:
//            {
//            MEWLOGGER_WRITE( "RunL, Disconnect delay passed" );
//            MEWLOGGER_WRITEF( _L("RunL, EMsgRequestWaitingDisconnection - iMmsReceiveErrorMessages: %d"),  iMmsSendErrorMessages->Count() );
//            if ( iMmsSendErrorMessages->Count() )
//                {
//                StartMmsSendL();
//                }
//            else
//                {
//                StartMmsFetchL();
//                }
//            //ResetErrorFieldL();     
//            }
//            break;
        case EMsgRequestFetchingAll:
        default:
            break;
        }
    QDEBUG_WRITE("CMsgErrorWatcherPrivate:RunL : Exit");
    }


//  End of File  

