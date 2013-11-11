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



// INCLUDE FILES
#include    <apparc.h>
#include    <msventry.h>
#include    <msvschedulepackage.h>
#include    <msvschedulesettings.h>
#include    <msvsenderroraction.h>
#include    <bautils.h>
#include    <e32math.h> // for notification generation
#include    <logcli.h>
#include    <logview.h>
#include    <flogger.h>
#include    <e32svr.h>
#include    <centralrepository.h>
#include    <utf.h>
#include    <cmsvmimeheaders.h>
#include    "LogsApiConsts.h"
#include    <logengdurations.h>

// MMS specific
#include    "mmsconst.h"
#include    "mmserrors.h"
#include    "mmsmmboxmessageheaders.h"
#include    "mmsservercommon.h"
#include    "mmscmds.h"
#include    "mmssettings.h"
#include    "mmsaccount.h"
#include    "mmsserver.h"
#include    "mmssendoperation.h"
#include    "mmsreceivemessage.h"
#include    "mmsforwardoperation.h"
#include    "mmsdeleteoperation.h"
#include    "mmsmmboxlist.h"
#include    "mmsdecode.h"
#include    "mmsencode.h"
#include    "mmsheaders.h"
#include    "mmsschedulesend.h"
#include    "mmsscheduledentry.h"
#include    "mmslog.h"
#include    "mmsgenutils.h"
#include    "mmsserverentry.h"
#include    "MmsEnginePrivateCRKeys.h"
#include    "mmsreadreport.h"

// LOCAL CONSTANTS AND MACROS
const TInt KMmsGarbageCollectionDelay = 30; // 30s delay
const TInt KMmsSanityInterval = 96;  // 96 hours, 4 days
const TInt KMmsScheduleAllowance = 10;
const TInt KMmsScheduleDelay = 5;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMmsServerMtm::CMmsServerMtm(
    CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvServerEntry* aInitialEntry )
    : CScheduleBaseServerMtm( aRegisteredMtmDll, aInitialEntry ),
    iNotification ( KMsvNullIndexEntryId ),
    iOOMState ( EFalse ),
    iDeliveryStatus (EFalse)
    {
    // Everything not mentioned gets initialized to NULL
    // Save our initial entry id
    // It is either default service or service specified in Entry Selection
    iServiceEntryId = aInitialEntry->Entry().Id();
    // We use the file session offered by initial entry.
    // Documentation says that it is expensive to open new file sessions
    // We offer the same session to all classes we create so that everybody
    // is using the same session.
    // In the final version file session is needed for attachment access
    // only, in the preliminary version we have a fake MMSC set up in a
    // directory on disk, and we need the file session to access that.
    iFs = aInitialEntry->FileSession();
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// Symbian OS default constructor can leave.
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::ConstructL()
    {

    CScheduleBaseServerMtm::ConstructL();
    iScheduleSend = CMmsScheduleSend::NewL( *iServerEntry );
    iMsvSelection = new( ELeave ) CMsvEntrySelection;
    iMmsSettings = CMmsSettings::NewL();
    // don't load settings yet in case someone else is trying
    // to change the settings.
    iMmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    iMessageDrive = EDriveC;
    // see if message server knows better
    iMessageDrive = MessageServer::CurrentDriveL( iFs );
    }

// ---------------------------------------------------------
// Factory function
// 
// ---------------------------------------------------------
//
EXPORT_C CMmsServerMtm* CMmsServerMtm::NewL(
    CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvServerEntry* aInitialEntry )
    {
    
    CleanupStack::PushL( aInitialEntry ); // Take ownership of aInitialEntry
    CMmsServerMtm* self = new( ELeave ) CMmsServerMtm(
        aRegisteredMtmDll, aInitialEntry );
    CleanupStack::Pop( aInitialEntry ); // Entry now safely stored in member

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// ---------------------------------------------------------
// Destructor
// 
// ---------------------------------------------------------
//
CMmsServerMtm::~CMmsServerMtm()
    {
    // We don't close the file session anymore, as we
    // obtained if from the initial entry, and it is not ours to close...
    Cancel(); // cancel anything that may be pending...
    if ( iRemoteParties )
        {
        iRemoteParties->Reset();
        }
    delete iEntryWrapper;
    delete iRemoteParties;
    delete iReadReport;
    delete iSendOperation;
    delete iReceiveMessage;
    delete iForwardOperation;
    delete iDeleteOperation;
    delete iUpdateMmboxList;
    delete iMsvSelection;
    delete iMmsSettings;
    delete iScheduleSend;
    delete iDecoder;
    delete iMmsHeaders;
    delete iEncodeBuffer;
    delete iMmsLog;
    delete iLogEvent;
    delete iLogViewEvent;
    delete iLogClient;
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("MMSServer destructor, done") );
#endif
    }


// ---------------------------------------------------------
// CMmsServerMtm::CopyToLocalL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::CopyToLocalL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {
    
    TRequestStatus* status = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete( status, KErrNotSupported );

    }

// ---------------------------------------------------------
// CMmsServerMtm::CopyFromLocalL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::CopyFromLocalL(
    const CMsvEntrySelection& aSelection,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {

    // test code: copy from local means send.
    // sent folder is handled separately
    TCommandParameters parameters;
    parameters.iInitialDelay = 0;
    TCommandParametersBuf paramPack( parameters );

    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(selection);
    if ( aSelection.Count() > 0 )
        {
        selection->AppendL( aSelection.Back( 0 ), aSelection.Count() );
        }
    StartCommandL( *selection, EMmsScheduledSend, paramPack, aStatus );
    CleanupStack::PopAndDestroy( selection );
  
    }

// ---------------------------------------------------------
// CMmsServerMtm::CopyWithinServiceL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::CopyWithinServiceL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {

    TRequestStatus* status = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete( status, KErrNotSupported );

    }

// ---------------------------------------------------------
// CMmsServerMtm::MoveToLocalL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::MoveToLocalL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {

    TRequestStatus* status = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete( status, KErrNotSupported );

    }

// ---------------------------------------------------------
// CMmsServerMtm::MoveFromLocalL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::MoveFromLocalL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {

    TRequestStatus* status = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete( status, KErrNotSupported );

    }

// ---------------------------------------------------------
// CMmsServerMtm::MoveWithinServiceL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::MoveWithinServiceL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {

    TRequestStatus* status = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete( status, KErrNotSupported );

    }

// ---------------------------------------------------------
// CMmsServerMtm::DeleteAllL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::DeleteAllL(
    const CMsvEntrySelection& aSelection,
    TRequestStatus& aStatus )
    {

    // this is implemented for scheduled send
    // It needs to change entry asynchronously,
    // and needs help on server mtm
    TInt error;
    // we are always called with a selection that has at least one member.
    // we cannot be called with an empty selection (because then the server
    // does not know whom to call)
    if ( aSelection.Count() == 0 )
        {
        User::Leave( KErrNotFound );
        }
    User::LeaveIfError( iServerEntry->SetEntry( aSelection[0] ) );
    User::LeaveIfError( iServerEntry->SetEntry( iServerEntry->Entry().Parent() ) );
    CMsvEntrySelection* sel = aSelection.CopyL();
    error = iServerEntry->DeleteEntries( *sel );
    if ( error == KErrNotFound )
        {
        error = KErrNone; // if not found, deleted already.
        }
    delete sel;

    TRequestStatus* status = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete( status, error );
    }

// ---------------------------------------------------------
// CMmsServerMtm::CreateL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::CreateL(
    TMsvEntry /*aNewEntry*/,
    TRequestStatus& aStatus )
    {

    TRequestStatus* status = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete( status, KErrNotSupported );

    }

// ---------------------------------------------------------
// CMmsServerMtm::ChangeL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::ChangeL(
    TMsvEntry aNewEntry,
    TRequestStatus& aStatus )
    {

    // this is implemented for scheduled send
    // It needs to change entry asynchronously,
    // and needs help on server mtm
    User::LeaveIfError( iServerEntry->SetEntry( aNewEntry.Id() ));
    User::LeaveIfError( iServerEntry->ChangeEntry( aNewEntry ) );
    TRequestStatus* status = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete( status, KErrNone );

    }

// ---------------------------------------------------------
// CMmsServerMtm::StartCommandL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::StartCommandL(
    CMsvEntrySelection& aSelection,
    TInt aCommand,
    const TDesC8& aParameter,
    TRequestStatus& aStatus )
    {
    
    TInt error = KErrNone;
    TMsvEntry entry; // This will be used a lot later to access the index data
    
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("MMSServer Start Command %d"), aCommand );
#endif // _NO_MMSS_LOGGING_
    // log the code for debugging
    LogCommandCode( aCommand );
    
    // The content of the parameter depends on command.
    // For EMmsDecodePushedMessage it will be TWatcherParametersBuf structure,
    // but the content is currently ignored because it has become impossible
    // to copy the data between processes in the protected environment.
    // For EMmsScheduledSend, EMmsScheduledReceive, and EMmsScheduledReceiveForced
    // it will be TCommandParametersBuf.
    // For EMmsGarbageCollection it will be TMMSGarbageCollectionParametersBuf
    // containing the reason for the garbage collection.
    // For others the parameter will be ignored.
    // The parameter should be unpackaged only when the contents are known.

    // The default service entry is always used.

    // Because of the restriction that only one mtm
    // per service can be open at any time, the scheduling calls
    // cheat and offer the stuff here using local service instead
    // of mms service. As we would like to load our settings
    // anyway, we try to find out the real service.

    if ( iServiceEntryId == KMsvLocalServiceIndexEntryId )
        {
        // we have been cheated
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- local service id") );
#endif
        // Get the actual service id from a message entry
        GetRealServiceId( aSelection );
        }
        
    // free whatever entry we are holding
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    //
    // Load the service settings.
    // In case call fails, loading will be retried
    //
    TInt loadServiceError = KErrNone;
    TRAP( loadServiceError, LoadSettingsL( aCommand ) );

#ifndef _NO_MMSS_LOGGING_
    if ( loadServiceError != KErrNone ) 
        {
        TMmsLogger::Log( _L("-ERROR loading settings: %d"), loadServiceError );
        }
    else
        {
        TMmsLogger::Log( _L("- settings loaded successfully") );        
        }
#endif

    iCurrentCommand = aCommand;
    if ( aCommand != EMmsRetryServiceLoading )
        {
        iCommand = aCommand;
        iParameter = aParameter;
        iRequestStatus = &aStatus;

        // We remove the service entry from the selection, as we don't need it anymore
        iMsvSelection->Reset();
        if ( aSelection.Count() > 0 )
            {
            iMsvSelection->AppendL( aSelection.Back( 0 ), aSelection.Count() );
            }
        }
    else
        {
        // iRequestStatus was set on an earlier round.
        // EMmsRetryServiceLoading is never the first command
        iCurrentCommand = iCommand; // orginal command was stored here
        }

    // If service loading has failed, we loop through RunL to retry.
    // Actually only EMmsScheduledReceive and EMmsScheduledReceiveForced
    // need this service.
    // Other callers can handle error situations gracefully.
    // We have saved all our parameters.
    // All we have to do now is to change iCurrent command and complete ourselves
    // In order to get to our RunL.
    // RunL will route us to DoRunL where we can continue

    if ( loadServiceError != KErrNone )
        {
        // Actually we should no longer get load service error as the settings
        // are now in central repository, no longer saved into the service entry
        HandleLoadServiceError( loadServiceError );
        // we cannot continue. HandleLoadServiceError has set completion status
        // as required.
        return;
        }

    // If we have loaded the service, we can discard the service entry
    // if it still is in our selection
    if ( iMsvSelection->Count() > 0 && iMsvSelection->At( 0 ) == iServiceEntryId )
        {
        iMsvSelection->Delete( 0 ); 
        }

#ifndef _NO_MMSS_LOGGING_
    // log the parent folder of the selection (needed for debugging)
    LogEntryParent();
#endif

    // we do not move the entries anywhere.
    // it is the client's responsibility to move them to the right place
    // we just make them visible because some applications left invisible
    // entries to outbox.
    if ( iCurrentCommand == EMmsSend ||
        iCurrentCommand == EMmsScheduledSend ||
        iCurrentCommand == EMmsDeleteSchedule )
        {
        RestoreVisibilityAndService();
        }

    //
    // Following switch handles all the different requests
    //
    switch( iCurrentCommand )
        {
        // scheduled operations can only use default service.
        case EMmsScheduledSend:
            if ( iMsvSelection->Count() > 0 )
                {
                iCommand = EMmsSend;
                // This will complete our caller.
                // If no error, task scheduler will complete the caller.
                // If error, the subroutine will complete
                error = ScheduleSelectionL();
#ifndef _NO_MMSS_LOGGING_
                if ( error != KErrNone )
                    {
                    TMmsLogger::Log( _L("MmsServer Schedule send status %d"), error );
                    }
#endif
                }
            else
                {
                // nothing to send - successfully sent nothing
                // ("You must have keen eyes to see nobody coming")
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, KErrNone );
                }
            break;
        //
        // Handle push message (i.e. notification or delivery report)
        //
        case EMmsDecodePushedMessage:
            {
            // 
            // First read pushed data from dummy entries stream store,
            // and then delete it.
            // If there is no entry, HandleDummyEntryL() leaves
            // 
            HandleDummyEntryL();
            //
            // Decode received databuffer into message
            //
            TInt err = KErrNone;
            TRAP( err, DecodePushedMessageL() );
            // DecodePushedMessageL might set iError, don't override it
            if ( iError == KErrNone )
                {
                iError = err;
                }

            delete iEncodeBuffer;
            iEncodeBuffer = NULL;
            // The resulting id is now in iNotification

            if ( iNotification != KMsvNullIndexEntryId )
                {
                iMsvSelection->AppendL( iNotification );
                if ( iServerEntry->SetEntry( iNotification ) == KErrNone )
                    {
                    entry = iServerEntry->Entry();
                    }
                // Unrecognized PDUs are handled as notifications.
                // we send a response back to MMSC
                if ( ( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageDeliveryInd ) ||
                    ( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageReadOrigInd ) )
                    {
                    // Delivery report or PDU read report
                    HandleDeliveryReportL();
                    }
                else
                    {
                    // Everything else. Handle as notification.
                    // If not a notification, send back response "unrecognized"
                    // Any PDU with wrong type will fall here besides the actual notifications.
                    HandleNotificationL();
                    }
                }
            else
                {
                // something has gone wrong...
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, iError );
                }
            break;
            }
        case EMmsScheduledReceiveForced:
            iCommand = EMmsReceiveForced;
            iMmsSettings->SetFetchOverride( ETrue );
            // if we do forced fetch, we clean up old schedules first
            if ( iMsvSelection->Count() > 0 )
                {
                CleanSchedulesL( *iMsvSelection );
                }
            // fall through on purpose
        case EMmsScheduledReceive:
            if ( iCurrentCommand != EMmsScheduledReceiveForced )
                {
                iCommand = EMmsReceive;
                }
            if ( iMsvSelection->Count() < 1 )
                {
                TRAP( iError, CreateNotificationsL() );
                }
            else // iMsvSelection->Count() > 0 
                {
                // notifications are not checked if the fetch is forced
                if ( iCurrentCommand == EMmsScheduledReceive )
                    {
                    CheckNotificationsL( *iMsvSelection );
                    }
                }

            if ( iMsvSelection->Count() > 0 )
                {
                // This will complete our caller.
                // If no error, task scheduler will complete the caller.
                // If error, the subroutine will complete
                error = ScheduleSelectionL();
#ifndef _NO_MMSS_LOGGING_
                if ( error != KErrNone )
                    {
                    TMmsLogger::Log( _L("MmsServer Schedule receive status %d"), error );
                    }
#endif
                }
            else
                {
                // Nothing to be done, complete.
                if ( iError != KErrNoMemory && iError != KErrDiskFull )
                    {
                    iError = KErrNone;
                    }
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, iError );
                }
            break;
        case EMmsSend:
            // send messages in current selection
            // we cannot do this, if we don't have settings.
            // Our access point is defined in settings.
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("Number of entries to send %d"), iMsvSelection->Count() );
#endif
            if ( iMsvSelection->Count() == 0 )
                {
                // nothing in the selection...
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, KErrNone );
                }
            else
                {
                iCommand = EMmsSend;
                SendToMmscL();
                }
            break;
        case EMmsReceive:
            // fetch message to inbox
            // This is a troublesome case.
            // we cannot fetch if we did not manage to load
            // our settings.
            // And we cannot leave, because schsend has an assert
            // that forbids rescheduling entries that are not children
            // of local service. And our notifications are children of
            // the MMS service itself.
            iCommand = EMmsReceive;
            FetchFromMmscL();
            break;
        case EMmsReceiveForced:
            iCommand = EMmsReceiveForced;
            iMmsSettings->SetFetchOverride( ETrue );
            FetchFromMmscL();
            break;
        case EMmsLogDeliveryReport:
            iCommand = EMmsLogDeliveryReport;
            // delivery reports should appear one by one for handling
            LogDeliveryReportL();
            break;
        case EMmsDeleteSchedule:
            iScheduleSend->DeleteScheduleL( aSelection );
            *iRequestStatus = KRequestPending;
            User::RequestComplete( iRequestStatus, KErrNone );
            break;
        case EMmsDeleteEntries:
            if ( iMsvSelection->Count() > 0 )
                {
                error = iServerEntry->SetEntry( iMsvSelection->At(0) );
                if ( error == KErrNone )
                    {
                    error = iServerEntry->SetEntry( iServerEntry->Entry().Parent() );
                    if ( error == KErrNone )
                        {
                        error = iServerEntry->DeleteEntries( *iMsvSelection );
                        }
                    }
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, error );
                }
            else
                {
                // if nothing to delete, then done already
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, KErrNone );
                }
            break;
        case EMmsGarbageCollection:
            TRAP(error, GarbageCollectionL());
            // This returns at least KMmsErrorOfflineMode
            *iRequestStatus = KRequestPending;
            User::RequestComplete( iRequestStatus, error );
            break;
        case EMmsMessageGeneration:
            iCommand = EMmsReceiveForced;
            iMmsSettings->SetLocalModeIn( KMmsMessageVariationDirectory() );
            iMmsSettings->SetFetchOverride( ETrue );
            // set local mode on the fly - not stored anywhere.
            iMmsSettings->SetLocalMode( ETrue );
            iMmsSettings->SetAcceptAnonymousMessages( ETrue ); // variated messages are anonymous
            FetchFromMmscL();
            break;
        case EMmsDeleteExpiredNotifications:
            // not implemented
            *iRequestStatus = KRequestPending;
            User::RequestComplete( iRequestStatus, KErrNotSupported );
            break;

        case EMmsScheduledForward:
            // Make sure there is something to schedule
            if ( iMsvSelection->Count() > 0 )
                {
                iCommand = EMmsForward;
                // ScheduleSelectionL completes the caller.
                // If no error, task scheduler will complete the caller.
                // If error, the subroutine will complete
                error = ScheduleSelectionL();
#ifndef _NO_MMSS_LOGGING_
                if ( error != KErrNone )
                    {
                    TMmsLogger::Log( _L("MmsServer EMmsScheduledForward status %d"), error );
                    }
#endif
                }
            else
                {
                // Nothing to send
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, KErrNone );
                }
            break;

        case EMmsForward:
            // Sends the current selection (containing forward requests)
            // we cannot do this, if we don't have settings.
            // Our access point is defined in settings.
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("Number of forward requests to send %d"), iMsvSelection->Count() );
#endif
            if ( iMsvSelection->Count() == 0 )
                {
                // Nothing in the selection
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, KErrNone );
                }
            else
                {
                iCommand = EMmsForward;
                SendForwardRequestsToMmscL();
                }
            break;

        //
        // Handles scheduling of notification deletion
        // 
        case EMmsScheduledNotificationDelete:
            // Make sure there is something to schedule
            if ( iMsvSelection->Count() > 0 )
                {
                iCommand = EMmsNotificationDelete;
                // ScheduleSelectionL completes the caller.
                // If no error, task scheduler will complete the caller.
                // If error, the subroutine will complete
                error = ScheduleSelectionL();
#ifndef _NO_MMSS_LOGGING_
                if ( error != KErrNone )
                    {
                    TMmsLogger::Log( _L("MmsServer EMmsScheduledNotificationDelete status %d"), error );
                    }
#endif
                }
            else
                {
                // Nothing to schedule
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, KErrNone );
                }
            break;

        //
        // Deletes selection of notifications
        //
        case EMmsNotificationDelete:
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("Number of notifications to delete %d"), iMsvSelection->Count() );
#endif
            if ( iMsvSelection->Count() == 0 )
                {
                // Nothing in the selection
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, KErrNone );
                }
            else
                {
                iCommand = EMmsNotificationDelete;

                // Dig out delete type
                TCommandParameters param;
                TPckgC<TCommandParameters> paramPack( param );
                paramPack.Set( iParameter );

                //
                // Create a CMmsDeleteOperation instance and start it
                // 
                delete iDeleteOperation;
                iDeleteOperation = NULL;
                iDeleteOperation = CMmsDeleteOperation::NewL( iFs, iMmsSettings  );
                iDeleteOperation->StartL(
                    (TMmsDeleteOperationType)paramPack().iError,
                    *iMsvSelection,
                    *iServerEntry,
                    iServiceEntryId,
                    iStatus );
                *iRequestStatus = KRequestPending;
                SetActive();        
                }
            break;
            // update mmbox list
        case EMmsUpdateMmboxList:
            iCommand = EMmsUpdateMmboxList;
            delete iUpdateMmboxList;
            iUpdateMmboxList = NULL;
            iUpdateMmboxList = CMmsMmboxList::NewL( iFs, iMmsSettings );
            iUpdateMmboxList->StartL(
                *iMsvSelection,
                *iServerEntry,
                iServiceEntryId, 
                iStatus );
            *iRequestStatus = KRequestPending;
            SetActive();
            break;
        case EMmsSendReadReport:
            SendReadReportL();
            break;
        case EMmsScheduledReadReport:
            if ( iMsvSelection->Count() > 0 )
                {
                iCommand = EMmsSendReadReport;
                // ScheduleSelectionL completes the caller.
                // If no error, task scheduler will complete the caller.
                // If error, the subroutine will complete
                error = ScheduleSelectionL();
#ifndef _NO_MMSS_LOGGING_
                if ( error != KErrNone )
                    {
                    TMmsLogger::Log( _L("MmsServer EMmsScheduledReadReport status %d"), error );
                    }
#endif
                }
            else
                {
                // Nothing to schedule
                *iRequestStatus = KRequestPending;
                User::RequestComplete( iRequestStatus, KErrNone );
                }
            break;
        default:
            *iRequestStatus = KRequestPending;
            User::RequestComplete( iRequestStatus, KErrNotSupported );
            break;
        }
    }

// ---------------------------------------------------------
// CMmsServerMtm::CommandExpected
// 
// ---------------------------------------------------------
//
TBool CMmsServerMtm::CommandExpected()
    {
    // so far we don't expect anything

    return EFalse;

    }

// ---------------------------------------------------------
// CMmsServerMtm::Progress
// 
// ---------------------------------------------------------
//
const TDesC8& CMmsServerMtm::Progress()
    {
    // should load in latest progress, if something is going on

    return iProgressBuffer;

    }        

// ---------------------------------------------------------
// CMmsServerMtm::LoadResourceFile
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::LoadResourceFileL()
    {
    // THIS IS NO LONGER NEEDED, BECAUSE THERE IS NO RESOURCE FILE ANY MORE.
    }

// ---------------------------------------------------------
// CMmsServerMtm::PopulateSchedulePackage
// ---------------------------------------------------------
//
void CMmsServerMtm::PopulateSchedulePackage( const TDesC8& aParameter,
    const TBool /*aMove*/, TMsvSchedulePackage& aPackage ) const
    {
    aPackage.iParameter = aParameter;
    // We have a member telling what we are supposed to do.
    // We can schedule both sending and receiving.
    aPackage.iCommandId =  iCommand;
    }

// ---------------------------------------------------------
// CMmsServerMtm::RestoreScheduleSettingsL
// ---------------------------------------------------------
//
void CMmsServerMtm::RestoreScheduleSettingsL( 
    TBool /*aRestoreErrorsFromResource*/, 
    TInt /*aErrorsResourceId*/ )
    {
    // EMPTY IMPLEMENTATION
    }

// ---------------------------------------------------------
// CMmsServerMtm::DoCancel
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::DoCancel()
    {

    // first cancel whatever operation is active
    if ( iSendOperation )
        {
        iSendOperation->Cancel();
        }

    if ( iReceiveMessage )
        {
        iReceiveMessage->Cancel();
        }

    if ( iMmsLog )
        {
        iMmsLog->Cancel();
        }

    if( iDeleteOperation )
        {
        iDeleteOperation->Cancel();
        }

    if( iForwardOperation )
        {
        iForwardOperation->Cancel();
        }

    if ( iUpdateMmboxList )
        {
        iUpdateMmboxList->Cancel();
        }

    if ( iReadReport )
        {
        iReadReport->Cancel();
        }

    DoComplete( KErrCancel );

    }

// ---------------------------------------------------------
// CMmsServerMtm::DoRunL
// Active object completion
// Run is used in this object to clean up after operations have finished.
//
// ---------------------------------------------------------
//
void CMmsServerMtm::DoRunL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L(" MmsServer DoRunL status %d"), iStatus.Int() );
#endif

    if ( iCurrentCommand == EMmsRetryServiceLoading )
        {
        StartCommandL(
            *iMsvSelection,
            iCurrentCommand,
            iParameter,
            *iRequestStatus);
        return;
        }

    TInt error = KErrNone;

    if ( iOOMState ) // out of memory.
        {
        error = KErrNoMemory;
        }
        
    if ( iCurrentCommand == EMmsUpdateMmboxList )
        {
        error = iStatus.Int();
        }

    // When we come here, we must see, if everything
    // was sent - or received, or if some items need resceduling
    DoComplete( error );
        
    }

// ---------------------------------------------------------
// CMmsServerMtm::DoComplete
// 
// Active object complete
// ---------------------------------------------------------
//
void CMmsServerMtm::DoComplete( TInt aError )
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L(" MmsServer DoComplete with status %d"), aError );
#endif

    // free whatever entry we were holding
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    // iSendOperation and iReceiveMessage tell us if
    // we were sending or receiving. The one that is non-null
    // has done all the work

    // Now we must check if all went fine, or do some entries
    // need rescheduling.

    // In the case of sending iMsvSelection holds Ids of the
    // messages to be sent. In the case of receiving iMsvSelection
    // holds Ids of the notifications corresponding to messages
    // to be fetched.

    TRAPD( error, UpdateEntriesL() );

#ifdef __WINS__
    if ( iSendOperation )
        {
        User::InfoPrint(_L("MMS sending complete"));
        }
    if ( iReceiveMessage )
        {
        User::InfoPrint(_L("MMS receiving complete"));
        }
#endif

    // restore original entry
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    if ( iError == KErrNone )
        {
        // The error may be changed in UpdateEntriesL
        iError = aError;
        }
      
    if ( iError == KMmsErrorApplicationDiskFull )
        {
        // this was handled by restarting the fetch if possible
        iError = KErrNone;
        }
        
    if ( iError == KErrNone )
        {
        // Pass on the error from UpDateEntriesL if it is relevant
        // to the caller.
        // First all errors caused by simultaneous backup/restore
        // are passed on.
        // If the entries were successfully reschedules we should
        // have no error here.
        if ( error >= KMsvMediaUnavailable && error <= KMsvIndexRestore )
            {
            iError = error;
            }
        }
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("MmsServer returns error: %d to caller" ), iError );
#endif
    if ( !( IsActive() && iStatus.Int() == KRequestPending ) )
        {
        // If CMmsServer has become active again it means that it has restarted
        // the fetch after deleting some application messages
        // The restarted fetch will complete caller when finished
        
        // However, if any ongoing operation is cancelled prematurely,
        // we may still be in active state but our own status is "cancelled"
        
        User::RequestComplete( iRequestStatus, iError );
        }
    }

// ---------------------------------------------------------
// CMmsServerMtm::LoadSettingsL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::LoadSettingsL( TInt aCommand )
    {
    // Load settings
    iMmsSettings->LoadSettingsL();

    // Save service entry id
    iServiceEntryId = iMmsSettings->Service();
    
    // Make sure localmode related paths exist
    if( iMmsSettings->LocalMode() )
        {
        iFs.MkDirAll( iMmsSettings->LocalModeIn() );
        iFs.MkDirAll( iMmsSettings->LocalModeOut() );
        }

    // Load schedule settings    
    ((CMmsScheduleSend*)iScheduleSend)->LoadSettingsL( aCommand );
    }

// ---------------------------------------------------------
// CMmsServerMtm::SendToMmscL()
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::SendToMmscL()
    {
    // We call our new excellent state machine
    delete iSendOperation;
    iSendOperation = NULL;
    iSendOperation = CMmsSendOperation::NewL( iFs, iMmsSettings );
    iSendOperation->StartL( *iMsvSelection, *iServerEntry, 
        iServiceEntryId, iStatus );
    *iRequestStatus = KRequestPending;
    SetActive();
    }

// ---------------------------------------------------------
// CMmsServerMtm::SendForwardRequestsToMmscL()
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::SendForwardRequestsToMmscL()
    {
    //
    // Create a CMmsForwardOperation instance and start it
    // 
    delete iForwardOperation;
    iForwardOperation = NULL;
    iForwardOperation = CMmsForwardOperation::NewL( iFs, iMmsSettings );
    iForwardOperation->StartL( 
        *iMsvSelection, 
        *iServerEntry, 
        iServiceEntryId, 
        iStatus );
    *iRequestStatus = KRequestPending;
    SetActive();        
    }

// ---------------------------------------------------------
// CMmsServerMtm::FetchFromMmscL()
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::FetchFromMmscL()
    {
    // We must put notifications into the selection.
    // The notifications will be have the same format
    // as message entries, but they will contain only
    // MMS headers.

    // The test version will use only the URI (Content-Location)
    // from the notification.
    // The Content-Location will contain the path and filename - 
    // as the first approximation.

    // For test purposes we create a fake selection by scanning
    // the directory specified in the settings, and storing the
    // filenames as messages under current service.

    // If we have rescheduled entries, we don't create a new notification
    // list, as we have one already
    if ( iMsvSelection->Count() < 1 )
        {
        CreateNotificationsL();
        }

    // If anything was left, we fetch them
    if ( iMsvSelection->Count() > 0 )
        {
        // We call our new excellent state machine
        delete iReceiveMessage;
        iReceiveMessage = NULL;
        iReceiveMessage = CMmsReceiveMessage::NewL( iFs, iMmsSettings  );

        iReceiveMessage->StartL( *iMsvSelection, *iServerEntry,
            iServiceEntryId, iStatus );
        if ( iRequestStatus->Int() != KRequestPending )
            {
            *iRequestStatus = KRequestPending;
            }
        SetActive();    
        }
    else
        {
        // We say we are done without error, if we pruned everything.
        // The original notifications that caused the pruning should
        // still be hanging around.
        // There is a danger of fetching failing so often that
        // we cannot even manage to send a response to MMSC, and
        // it sends us a duplicate notification because it thinks
        // the original one has got lost.
        // We must carefully test the failure conditions and try to 
        // determine reasonable amount of retries that should be done
        // to avoid such situation.
        // The other possibility would be to always remove the old
        // notification if a new one arrives with identical TID and
        // Content location, but then we would be in danger of
        // deleting an entry that is currently being used to fetch
        // a message.
        // The actual fetching code in CMmsReceiveMessage class
        // tries to test that the notifications are accessible,
        // and it tries not to trap, if the notifications have
        // disappeared from its lists,
        if ( iError != KErrNoMemory &&
            iError != KErrDiskFull && iError != KMmsErrorApplicationDiskFull )
            {
            iError = KErrNone;
            }
        if ( iRequestStatus->Int() != KRequestPending )
            {
            *iRequestStatus = KRequestPending;
            }
        if ( iError == KMmsErrorApplicationDiskFull )
            {
            // DoComplete will complete caller
            iError = KErrNone; 
            }
        else
            {
            User::RequestComplete( iRequestStatus, iError );
            }
        }
    }

// ---------------------------------------------------------
// CMmsServerMtm::UpdateEntriesL()
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::UpdateEntriesL()
    {
    // if something goes fatally wrong, this error will be
    // returned to the caller in hope the caller may be able
    // to do something to fix the problem
    TInt fatalError = KErrNone; 
    TMsvEntry entry;
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("UpdateEntriesL" ));
#endif

    // Tell scheduler about entries that were successfully sent
    // or received

    // Rescedule failed entries
    // We have a member that tells which command to use
    // (Send or receive)

    // The central repository file contains a list of hopeless cases.
    // These are not rescheduled

    TMsvSchedulePackage* schedulePackage = new( ELeave ) TMsvSchedulePackage;
    CleanupStack::PushL( schedulePackage );
    PopulateSchedulePackage( iParameter, ETrue, *schedulePackage );

    // Failed forward entries
    if( iForwardOperation && iForwardOperation->Failed().Count() > 0 )
        {
        HandleFailedForwardsL( *schedulePackage );
        }

    // Successfully sent entries
    if( iSendOperation && iSendOperation->Sent().Count() > 0 )
        {
        HandleSuccessfulSendsL();
        }

    // Entries that failed to be sent
    if ( iSendOperation && iSendOperation->Failed().Count() > 0 )
        {
        HandleFailedSendsL( *schedulePackage );
        }

    // Message generation (branding messages)
    if ( iReceiveMessage && iCurrentCommand == EMmsMessageGeneration )
        {
        CleanupAfterMessageGenerationL();
        }

    // Mark entries that failed to be fetched
    // If there is some error connected to this, it must be returned to caller
    // as fetching is automatic.
    TBool restartFetch = EFalse;
    if ( iReceiveMessage && iReceiveMessage->Failed().Count() > 0 )
        {
        restartFetch = HandleFailedReceivesL( *schedulePackage, fatalError );
        }

    // Successfully received entries
    if ( iReceiveMessage && iReceiveMessage->Received().Count() > 0 )
        {
        HandleSuccessfulReceivesL();
        }

    // Remove bad notifications from task scheduler
    if ( iReceiveMessage && iReceiveMessage->BadNotifications().Count() > 0 )
        {
        HandleBadNotificationsL();
        }
        
    if ( restartFetch )
        {
        // We did find some failed notifications that were in inbox or MMBox folder
        // These are not normally rescheduled
        // But if the error was due to lack of disk space and we have been able to
        // free the disk space by deleting some older messages belonging to a 
        // lazy application, we can retry the fetch immediately
        
        // We must clear the receiver class to start with a clean slate.
        delete iReceiveMessage;
        iReceiveMessage = NULL;
        FetchFromMmscL();
        iError = KMmsErrorApplicationDiskFull;
        }

    // update delivery status counts and reschedule possible extra delivcery reports
    if ( iCommand == EMmsLogDeliveryReport )
        {
        UpdateDeliveryReportsL( *schedulePackage );
        }
     
    // handle failed read report entries    
    if ( iReadReport && iReadReport->Failed().Count() > 0 )
        {
        HandleFailedReadReports();
        }
        
    CleanupStack::PopAndDestroy( schedulePackage );

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("UpdateEntriesL done, error: %d" ), iError );
#endif
    if ( fatalError != KErrNone )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("UpdateEntriesL, fatal error: %d" ), fatalError );
#endif
        // we catch this
        User::Leave( fatalError );
        // return not needed as User::Leave returns
        }
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleFailedForwardsL( TMsvSchedulePackage& aPackage )
    {
    TInt count = 0;
    TMsvEntry entry;
    TInt error = KErrNone;

    count = iForwardOperation->Failed().Count();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- %d failed (not sent) forward entries"), count );
    TInt rescheduled = count; // rescheduled needed only for logging
#endif
    // Loop selection and make them reschedulable (readonly == false)
    while ( count-- )
        {
        if ( iServerEntry->SetEntry( iForwardOperation->Failed().At( count ) ) == KErrNone )
            {
            entry = iServerEntry->Entry();
            entry.SetReadOnly( EFalse );
            iServerEntry->ChangeEntry( entry ); // ignore error
            }
        }

    //
    //  Now reschedule.
    //
    iScheduleSend->ReScheduleL( iForwardOperation->Failed(), aPackage );

    // Mark entries that failed to be rescheduled
    count = iForwardOperation->Failed().Count();
    while ( count-- )
        {
        if ( iServerEntry->SetEntry( iForwardOperation->Failed().At( count ) ) == KErrNone )
            {
            entry = iServerEntry->Entry();
            if ( entry.SendingState() != KMsvSendStateResend )
                {
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- forward entry failed to reschedule, setting state to failed") );
#endif
                entry.SetSendingState( KMsvSendStateFailed );
                error = iServerEntry->ChangeEntry( entry ); // ignore error
#ifndef _NO_MMSS_LOGGING_
                if( error != KErrNone )
                    {
                    TMmsLogger::Log( _L("- ERROR: changing entry failed") );
                    }
#endif
                // Clear related notification from Inbox:
                // Get the related notification id
                CMsvStore* store = NULL;
                store = iServerEntry->EditStoreL();
                CleanupStack::PushL( store ); // ***
                iMmsHeaders->RestoreL( *store );
                CleanupStack::PopAndDestroy( store );
                TMsvId relatedEntryId = iMmsHeaders->RelatedEntry();
                iMmsHeaders->Reset(); // headers not needed any more

                if( relatedEntryId != KMsvNullIndexEntryId )
                    {
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- related notification-entry exists, clearing it") );
#endif
                    // Set context (iServerEntry and entry) to notification and clear it
                    error = iServerEntry->SetEntry( relatedEntryId );
#ifndef _NO_MMSS_LOGGING_
                    if( error != KErrNone )
                        {
                        TMmsLogger::Log( _L("- ERROR: Could not set entry") );
                        }
#endif
                    entry = iServerEntry->Entry();
                    entry.iMtmData2 &= ~KMmsNewOperationForbidden; // not forbidden
                    entry.iMtmData2 &= ~KMmsOperationOngoing;      // not ongoing
                    entry.iMtmData2 |= KMmsOperationFinished;      // finished
                    entry.iMtmData2 |= KMmsOperationResult;        // NOK
                    entry.SetReadOnly( ETrue );
                    error = iServerEntry->ChangeEntry( entry );
#ifndef _NO_MMSS_LOGGING_
                    if( error != KErrNone )
                        {
                        TMmsLogger::Log( _L("- ERROR: Could not change related entry") );
                        }
                    TMmsLogger::Log( _L("- Clear the related-entry link itself") );
#endif

                    // Clear related-id link from forward entry
                    if ( iMsvSelection->Count() > count )
                        {
                        error = iServerEntry->SetEntry( iMsvSelection->At( count ) );
                        }
                    else
                        {
                        // We should never get this,
                        // we check count only to keep CodeScanner happy
                        error = KErrNotFound; 
                        }
                    if ( error == KErrNone )
                        {
                        store = iServerEntry->EditStoreL();
                        CleanupStack::PushL( store ); // ***
                        iMmsHeaders->RestoreL( *store );
                        iMmsHeaders->SetRelatedEntry( KMsvNullIndexEntryId );
                        iMmsHeaders->StoreL( *store );
                        store->CommitL();
                        CleanupStack::PopAndDestroy( store );
                        iMmsHeaders->Reset(); // headers not needed any more
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- Related-entry and the link cleared") );
#endif
                        }
                    }

                // let go of the entry
                iServerEntry->SetEntry( KMsvNullIndexEntryId );
                }
            else
                {
                //
                // Delete successfully rescheduled entries
                //
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- notification is in resend, as it should") );
#endif
                iForwardOperation->Failed().Delete( count );
                }
            }
#ifndef _NO_MMSS_LOGGING_
        else
            {
            TMmsLogger::Log( _L("- ERROR: could not access entry %d"), count );
            }
#endif
        }

#ifndef _NO_MMSS_LOGGING_
    count = iForwardOperation->Failed().Count();
    rescheduled = rescheduled - count;
    if ( rescheduled > 0 )
        {
        TMmsLogger::Log( _L("- %d rescheduled forward entries"), rescheduled );
        }
    if ( count > 0 )
        {
        TMmsLogger::Log( _L("- %d not rescheduled forward entries"), count );
        }
#endif
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleSuccessfulSendsL()
    {
    TInt count = 0;
    TMsvEntry entry;
    
    // Delete schedule should not be needed in the case of successfully
    // sent entries.
    count = iSendOperation->Sent().Count();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- %d Sent entries"), count );
#endif
    iScheduleSend->DeleteScheduleL( iSendOperation->Sent() );
    while ( count-- )
        {
        if ( iServerEntry->SetEntry( iSendOperation->Sent().At( count ) ) == KErrNone )
            {
            entry = iServerEntry->Entry();
            entry.SetSendingState( KMsvSendStateSent );
            if ( entry.Parent() == KMsvSentEntryIdValue )
                {
                // if we have not managed to move this entry away from outbox,
                // it must not be set to read only state
                entry.SetReadOnly( ETrue );
                }
            // We don't want to leave here, we want to continue.
            // The next message may succeed.
            // We don't consider this fatal: If the message
            // has been successfully sent, it should already
            // be moved to sent folder.
            // If the user tries to send messages during backup/restore,
            // it is his own fault if the messages are sent more than once.
            iServerEntry->ChangeEntry( entry );
            }
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleFailedSendsL( TMsvSchedulePackage& aPackage )
    {
    TInt count = 0;
    TMsvEntry entry;
    
    count = iSendOperation->Failed().Count();
#ifndef _NO_MMSS_LOGGING_
    TInt rescheduled = count;
    TMmsLogger::Log( _L("- %d failed (not sent) entries"), count );
#endif
    while ( count-- )
        {
        if ( iServerEntry->SetEntry( iSendOperation->Failed().At( count ) ) == KErrNone )
            {
            entry = iServerEntry->Entry();
            // we set the entry into failed state after we have
            // checked if it still can be rescheduled.
            entry.SetReadOnly( EFalse );
            iServerEntry->ChangeEntry( entry ); // ignore error
            if ( entry.SendingState() == KMsvSendStateSuspended &&
                entry.iError != KMmsErrorOfflineMode )
                {
                // suspended by user, not offline mode
                // We remove this just in case.
                // If everything has gone well, the error is "KErrNotFound"
                // and the entry would not be rescheduled, but this is an
                // extra precaution.
                iSendOperation->Failed().Delete( count );    
                }
            }
        }
        
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    count = iSendOperation->Failed().Count();

    if ( count > 0 )
        {
        // Check needed to avoid a stupid ASSERT_DEBUG
        iScheduleSend->ReScheduleL( iSendOperation->Failed(), aPackage );
        }

    // Mark entries that failed to be rescheduled
    count = iSendOperation->Failed().Count();

    while ( count-- )
        {
        if ( iServerEntry->SetEntry( iSendOperation->Failed().At( count ) ) == KErrNone )
            {
            entry = iServerEntry->Entry();
            if ( entry.SendingState() != KMsvSendStateResend )
                {
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- setting state to failed") );
#endif
                entry.SetSendingState( KMsvSendStateFailed );
                iServerEntry->ChangeEntry( entry ); // ignore error
                // let go of the entry
                iServerEntry->SetEntry( KMsvNullIndexEntryId );
                }
            else
                {
                iSendOperation->Failed().Delete( count );
                }
            }
#ifndef _NO_MMSS_LOGGING_
        else
            {
            TMmsLogger::Log( _L("- could not access entry %d"), count );
            }
#endif
        }

    count = iSendOperation->Failed().Count();
#ifndef _NO_MMSS_LOGGING_
    rescheduled = rescheduled - count;
    if ( rescheduled > 0 )
        {
        TMmsLogger::Log( _L("- %d rescheduled for sending"), rescheduled );
        }
    if ( count > 0 )
        {
        TMmsLogger::Log( _L("- %d hopeless, not rescheduled for sending"), count );
        }
#endif

    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::CleanupAfterMessageGenerationL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Finished message generation") );
#endif
    // we try only once.
    // The whole selection must be in the same place.
    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    
    // everything must go.
    if ( iReceiveMessage->BadNotifications().Count() > 0 )
        {
        selection->AppendL( iReceiveMessage->BadNotifications().Back( 0 ),
            iReceiveMessage->BadNotifications().Count() );
        iReceiveMessage->BadNotifications().Reset();
        }
    
    if ( iReceiveMessage->Failed().Count() > 0 )
        {
        selection->AppendL( iReceiveMessage->Failed().Back( 0 ),
            iReceiveMessage->Failed().Count() );
        iReceiveMessage->Failed().Reset();
        }
        
    if ( iReceiveMessage->Received().Count() > 0 )
        {
        selection->AppendL( iReceiveMessage->Received().Back( 0 ),
            iReceiveMessage->Received().Count() );
        iReceiveMessage->Received().Reset();
        }
    
    // If the next loop fails, we don't care:
    // The files will be deleted ayway, and the notifications will be orphaned
    // If there is something weird going on during the boot,
    // the phone won't probably work after this anyway.
    if ( selection->Count() > 0 )
        {
        if ( iServerEntry->SetEntry( selection->At( 0 ) ) == KErrNone )
            {
            if ( iServerEntry->SetEntry( iServerEntry->Entry().Parent() ) == KErrNone )
                {
                iServerEntry->DeleteEntries( *selection );
                }
            }
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        }
    CleanupStack::PopAndDestroy( selection );

    // Then we delete the files in the directory
    // (if anything left)
    CFileMan* fileMan = CFileMan::NewL( iFs );
    CleanupStack::PushL( fileMan );
    // Best effort - ignore error
    fileMan->RmDir( KMmsMessageVariationDirectory );
    CleanupStack::PopAndDestroy( fileMan );
    
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CMmsServerMtm::HandleFailedReceivesL( TMsvSchedulePackage& aPackage, TInt& aFatalError )
    {
    TInt count = 0;
    TMsvEntry entry;
    TInt error = KErrNone;
    TBool restartFetch = EFalse;
    
    iMsvSelection->Reset();
    count = iReceiveMessage->Failed().Count();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- %d failed (not received) entries"), count );
#endif
    // Check if the current receive mode is manual
    TBool manual = EFalse;
    if( iReceiveMessage->InForeignNetwork() )
        {
        if( iMmsSettings->ReceivingModeForeign() == EMmsReceivingManual )
            {
            manual = ETrue;
            }
        }
    else
        {
        if( iMmsSettings->ReceivingModeHome() == EMmsReceivingManual )
            {
            manual = ETrue;
            }
        }

    TBool inInbox = EFalse;
    TBool inMmsFolder = EFalse;
    TBool inMmboxFolder = EFalse;

    while ( count-- )
        {
        error = iServerEntry->SetEntry( iReceiveMessage->Failed().At( count ) );
        if ( error == KErrNone )
            {
            entry = iServerEntry->Entry();

            if ( entry.iMtmData2 & KMmsDoNotMoveToInbox )
                {
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- do not move entry to inbox "));
#endif
                // We do not reschedule this. It will potentially cause problems at mode change
                // if it has not been handled by then.
                iReceiveMessage->Failed().Delete( count );
                // it is put into bad list and deleted later
                iReceiveMessage->BadNotifications().AppendL( entry.Id() );
                }
            else
                {
                if ( entry.Parent() == FindMMSFolderL() )
                    {
                    inMmsFolder = ETrue;
                    }
                else if ( entry.Parent() == KMsvGlobalInBoxIndexEntryIdValue )        
                    {
                    inInbox = ETrue;
                    // inbox entries will not be rescheduled
                    if ( entry.iError == KMmsErrorApplicationDiskFull )
                        {
                        // This is best effort only - error ignored
                        iMsvSelection->AppendL( iReceiveMessage->Failed().At( count ) );
                        restartFetch = ETrue;
                        }
                    iReceiveMessage->Failed().Delete( count );
                    }
                else
                    {
                    inMmboxFolder = ETrue;
                    if ( entry.iError == KMmsErrorApplicationDiskFull )
                        {
                        // This is best effort only - error ignored
                        iMsvSelection->AppendL( iReceiveMessage->Failed().At( count ) );
                        restartFetch = ETrue;
                        }
                    // mmbox entries will not be rescheduled
                    iReceiveMessage->Failed().Delete( count );
                    manual = EFalse; // manual mode must not affect the mmbox notification
                    }
                // We don't have a separate receiving state.
                // We just use sending state instead.
                // We are the only one that uses scheduled receiving.
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- marking sending state as failed "));
#endif
                entry.SetSendingState( KMsvSendStateFailed );

                if ( manual && inMmsFolder )
                    {
                    // Change the iMtm from KUidMsgTypeMultimedia to 
                    // KUidMsgMMSNotification   
                    entry.iMtm.iUid = KUidMsgMMSNotification.iUid; // this is a notification

                    entry.iMtmData2 &= ~KMmsOperationFinished; // clear flag just in case
                    entry.iMtmData2 &= ~KMmsOperationResult; // clear flag just in case
                    entry.iMtmData2 &= ~KMmsOperationOngoing; // Fetch operation is not active anymore
                    entry.iMtmData2 &= ~KMmsNewOperationForbidden; // New operation can be started
                    entry.iMtmData1 |= KMmsMessageMobileTerminated;
                    entry.SetReadOnly( ETrue );
                    entry.iError = KErrNone;
                    entry.SetSendingState( KMsvSendStateUnknown );                    
                    }
                else if ( inInbox  )
                    {
                    // Mark original notification
                    CMmsBaseOperation::MarkNotificationOperationFailed( entry ); 
                    if ( entry.iError != KMmsErrorApplicationDiskFull )
                        {
                        entry.SetReadOnly( ETrue );
                        }
                    }
                else if ( inMmboxFolder )
                    {
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- in mmbox folder"));
#endif
                    TRAP( error, CMmsBaseOperation::MarkDuplicateL(
                        CMmsBaseOperation::EMmsNotificationOperationFailed,
                        *iServerEntry ) );
                    error = KErrNone; // ignore error from trap    
                    entry = iServerEntry->Entry();
                    CMmsBaseOperation::MarkNotificationOperationFailed( entry );
                    if ( entry.iError != KMmsErrorApplicationDiskFull )
                        {
                        entry.SetReadOnly( ETrue );
                        }
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- marking original notif failed"));
#endif                    
                    }
                else
                    {
                    // keep LINT happy
                    }
                error = iServerEntry->ChangeEntry( entry );
                
                // move the entry from mms folder to the inbox in manual mode.
                if ( manual && inMmsFolder )
                    {
                    iError = iServerEntry->SetEntry( entry.Parent() );
                    if ( iError == KErrNone )
                        {
                        error = iServerEntry->MoveEntryWithinService(
                            entry.Id(), KMsvGlobalInBoxIndexEntryIdValue );
                        }
                    else
                        {
                        error = iError;
                        }
                    if ( error == KErrNone )
                        {
                        inInbox = ETrue;
                        // This should not be rescheduled as it has moved to inbox
                        iReceiveMessage->Failed().Delete( count );
                        }
                    }
                }
            if ( aFatalError == KErrNone )
                {
                // if we canot access the entry, tell the caller
                aFatalError = error;
                }
            // let go of the entry
            iServerEntry->SetEntry( KMsvNullIndexEntryId );
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("UpdateEntriesL: entry #%d, sending state = failed"), count );
            TMmsLogger::Log( _L("UpdateEntriesL: entry #%d, error = %d"), count, entry.iError );
            if ( error != KErrNone )
                {
                TMmsLogger::Log( _L("UpdateEntriesL: ChangeEntry failed, error %d"), error );
                }
#endif
            }
#ifndef _NO_MMSS_LOGGING_
        else
            {
            // Not found is not fatal. What is gone is gone.
            if ( error != KErrNotFound )
                {
                if ( aFatalError == KErrNone )
                    {
                    aFatalError = error;
                    }
                }
            TMmsLogger::Log( _L("UpdateEntriesL: could not access entry #%d"), count );
            }
#endif
        }

    // only if the notification is in mmsfolder, reschedule the notification
    // if the notification is in Inbox or in mmbox folder, do not rescedule.
    // We have removed each notification from failed list if it was originally
    // in inbox or in MMBox folder or if it was successfully moved to inbox
    if( iReceiveMessage->Failed().Count() > 0 )
        {
        iScheduleSend->ReScheduleL( iReceiveMessage->Failed(), aPackage );
        }
    // The messages that could not be rescheduled anymore, are either deleted, or moved
    // to Inbox to be handled manually
    count = iReceiveMessage->Failed().Count();
    // we do not delete hopeless entries:
    // There is a separate error watcher that decides
    // what to do with them.
    // For example: If there is no access point, receiving
    // fails. The error watcher gives notice to user, and
    // when the user has entered the access point, the 
    // fetch is restarted by the error watcher.
#ifndef _NO_MMSS_LOGGING_
    TInt hopeless = count;
#endif
    while ( count-- )
        {
        TMsvId failedEntry = iReceiveMessage->Failed().At( count );
        if ( iServerEntry->SetEntry( failedEntry ) == KErrNone )
            {
            entry = iServerEntry->Entry();
            // update retry count
            TRAP( error, 
                {
                CMsvStore* store = NULL;
                store = iServerEntry->ReadStoreL();
                CleanupStack::PushL( store );
                CMmsScheduledEntry* mmsScheduledEntry =
                    CMmsScheduledEntry::NewL( iServerEntry->Entry() );
                CleanupStack::PushL( mmsScheduledEntry );
                mmsScheduledEntry->RestoreL( *store );
                entry.iMtmData3 &= ~KMmsRetryCountMask;
                entry.iMtmData3 |= mmsScheduledEntry->MmsRecipient().Retries();
                iServerEntry->ChangeEntry( entry );
                CleanupStack::PopAndDestroy( mmsScheduledEntry );
                CleanupStack::PopAndDestroy( store );
                }
            );

            if ( entry.SendingState() == KMsvSendStateFailed ) 
                {
                // remove from list just to see what was left over
                iReceiveMessage->Failed().Delete( count );
                }
            }
        }
#ifndef _NO_MMSS_LOGGING_
    count = iReceiveMessage->Failed().Count();
    hopeless = hopeless - count;
    if ( count > 0 )
        {
        TMmsLogger::Log( _L("- %d rescheduled for receiving"), count );
        }
    if ( hopeless > 0 )
        {
        TMmsLogger::Log( _L("- %d hopeless, not rescheduled for receiving"), hopeless );
        }
#endif  
        
    return restartFetch;
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleSuccessfulReceivesL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- %d received entries"), iReceiveMessage->Received().Count() );
#endif
    TMsvId parent = KMsvNullIndexEntryId;
    // The whole selection must be in the same place.
    if ( iServerEntry->SetEntry( iReceiveMessage->Received().At( 0 ) ) == KErrNone  )
        {
        parent = iServerEntry->Entry().Parent();
        }

    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->AppendL( iReceiveMessage->Received().Back( 0 ),
        iReceiveMessage->Received().Count() );

    // if entry is in inbox or in MMBoxfolder,
    // check possible duplicate and mark it "fetched from server"
    if ( parent == KMsvGlobalInBoxIndexEntryIdValue || parent == iMmsSettings->MMBoxFolder() )
        {
        for ( TInt i = selection->Count(); i > 0; i-- )
            {
            if ( iServerEntry->SetEntry( selection->At( i - 1 )) == KErrNone )
                {
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- check possible duplicate" ));
#endif
                CMsvStore* store = iServerEntry->ReadStoreL();
                CleanupStack::PushL( store );
                iMmsHeaders->RestoreL( *store );
                CleanupStack::PopAndDestroy( store );
                store = NULL;
                TMsvId duplicate = KMsvNullIndexEntryId;

                if ( parent == KMsvGlobalInBoxIndexEntryIdValue )
                    {
                    TMsvId mmboxFolder = iMmsSettings->MMBoxFolder();
                    if ( mmboxFolder != KMsvNullIndexEntryId )
                        {
                        FindDuplicateNotificationL( mmboxFolder, *iMmsHeaders, duplicate );
                        }
                    }
                else // parent is mmbox folder
                    {
                    duplicate = iMmsHeaders->RelatedEntry();
                    }
                if ( duplicate != KMsvNullIndexEntryId )
                    {
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- duplicate found"));
#endif
                    if ( iServerEntry->SetEntry( duplicate ) == KErrNone )
                        {
                        // Mark duplicate
                        TMsvEntry dupEntry = iServerEntry->Entry();
                        CMmsBaseOperation::MarkNotificationDeletedFromMmbox( dupEntry );
                        iServerEntry->ChangeEntry( dupEntry );
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- duplicate marked as fetched from mmbox"));
#endif
                        }
                    } 
                }
            }
        }
    if ( parent != KMsvNullIndexEntryId && iServerEntry->SetEntry( parent ) == KErrNone )
        {
        iServerEntry->DeleteEntries( *selection );
        }
    CleanupStack::PopAndDestroy( selection );
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleBadNotificationsL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- %d bad notification entries"), iReceiveMessage->BadNotifications().Count() );
#endif
    // The whole selection must be in the same place.
    if ( iServerEntry->SetEntry( iReceiveMessage->BadNotifications().At( 0 ) ) == KErrNone )
        {
        if ( iServerEntry->SetEntry( iServerEntry->Entry().Parent() ) == KErrNone )
            {
            CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
            CleanupStack::PushL( selection );
            selection->AppendL( iReceiveMessage->BadNotifications().Back( 0 ),
                iReceiveMessage->BadNotifications().Count() );
            iServerEntry->DeleteEntries( *selection );
            CleanupStack::PopAndDestroy( selection );
            }
        }
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::UpdateDeliveryReportsL( TMsvSchedulePackage& aPackage )
    {
    TInt error = KErrNone;
    TMsvEntry entry;

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- logged delivery report" ));
#endif
    if ( iError == KErrNone )
        {
        // first entry in selection was handled
        CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
        CleanupStack::PushL( selection );
        selection->AppendL( iMsvSelection->At( 0 ), 1 );
        if ( selection->Count() > 0 )
            {
            error = iServerEntry->SetEntry( selection->At( 0 ) );
            }
        else
            {
            // this will never happen - we are just keeping codescanner happy
            error = KErrNotFound;
            }
        if ( error == KErrNone )
            {
            if ( iServerEntry->SetEntry( iServerEntry->Entry().Parent() ) == KErrNone )
                {
                iServerEntry->DeleteEntries( *selection );
                }
            }
        CleanupStack::PopAndDestroy( selection );
        iMsvSelection->Delete( 0 );
        }
    
    //Now let's set delivery report bits in MtmData
    TMsvId link = 0;
    if ( iMmsLog )
        {
        link = iMmsLog->GetLink();
        }
    if (link != 0)
        {
        error = iServerEntry->SetEntry(link);
        if ( error == KErrNone )
            {
            entry = iServerEntry->Entry();
            }
        }

    // Even if the link exists, the original message may have disappeared already
    // (if only 20 messages are saved in outbox, they may start disappearing quite fast)
    if ( link != 0 && error == KErrNone && ( entry.iMtmData2 & KMmsDeliveryStatusMask ) !=
        KMmsDeliveryStatusNotRequested )
        {
        TUint temp(0);
        TUint total(0);
        if (iDeliveryStatus) //successfully delivered
            {
            total = ( entry.iMtmData3 & KMmsSentItemTotalRecipientsMask ) >>
                KMmsSentItemTotalRecipientsShift;
            
            temp = ( entry.iMtmData3 & KMmsSentItemSuccessfullyDeliveredMask )
                >> KMmsSentItemSuccessfullyDeliveredShift;
            temp++;
            entry.iMtmData3 &= ~KMmsSentItemSuccessfullyDeliveredMask;
            entry.iMtmData3 |= temp << KMmsSentItemSuccessfullyDeliveredShift;
            
            // must make sure that if even one send has been failed the delivery status
            // is always failed
            if ( temp == total  && ( entry.iMtmData2 & KMmsDeliveryStatusMask )
                != KMmsDeliveryStatysFailed )
                {
                entry.iMtmData2 &= ~KMmsDeliveryStatusMask;
                entry.iMtmData2 |= KMmsDeliveryStatysDelivered;
                }
            else if (temp < total && ( entry.iMtmData2 & KMmsDeliveryStatusMask )
                != KMmsDeliveryStatysFailed  )
                {
                entry.iMtmData2 &= ~KMmsDeliveryStatusMask;
                entry.iMtmData2 |= KMmsDeliveryStatusPartial;
                }
            else
                {
                // keep LINT happy
                }

            }
        else 
            {
            temp = (entry.iMtmData3 & KMmsSentItemFailedDeliveryMask) >>
                KMmsSentItemFailedDeliveryShift;
            temp++;
            entry.iMtmData3 &= ~KMmsSentItemFailedDeliveryMask;
            entry.iMtmData3 |= temp << KMmsSentItemFailedDeliveryShift;
            entry.iMtmData2 &= ~KMmsDeliveryStatusMask;
            entry.iMtmData2 |= KMmsDeliveryStatysFailed;
                            
            
            }
        // If we successfully accessed the entry, iServerEntry must still be pointing to the link.
        iServerEntry->ChangeEntry(entry);    
        }
        
    iError = KErrNone; // we don't care about the error.
    // We should normally never be here, delivery reports come
    // one at a time...
    if ( iMsvSelection->Count() > 0 )
        {
        iScheduleSend->ReScheduleL( *iMsvSelection, aPackage );
        }
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleFailedReadReports()
    {
    TInt error = KErrNone;
    if ( iReadReport->Failed().Count() > 0 )
        {
        error = iServerEntry->SetEntry( iReadReport->Failed().At( 0 ) );
        }
    else
        {
        error = KErrNotFound;
        }
    if ( error == KErrNone )
        {
        error = iServerEntry->SetEntry( iServerEntry->Entry().Parent() );
        }
    if ( error == KErrNone )
        {
        TInt i;
        for ( i = iReadReport->Failed().Count() - 1; i >= 0; i-- )
            {
            iServerEntry->DeleteEntry( iReadReport->Failed().At( i ) );
            }
        }
    }
   
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::MakeDatesIdenticalL(
    CMsvEntrySelection& aSelection,
    TTimeIntervalSeconds aInterval,
    TBool aClearError /* = EFalse */ )
    {

    TInt count = aSelection.Count();
    TTime curTime;
    curTime.UniversalTime();
    if ( aInterval > TTimeIntervalSeconds( KMmsDelayInSeconds ) )
        {
        curTime += aInterval;
        }
    else
        {
        curTime += TTimeIntervalSeconds( KMmsDelayInSeconds );
        }

    while ( count-- )
        {
        if ( iServerEntry->SetEntry( aSelection.At( count ) ) == KErrNone )
            {
            TMsvEntry entry = iServerEntry->Entry();
            if ( entry.Id() != aSelection.At( count ) || entry.Id() == KMsvNullIndexEntryId )
                {
                // The entry is garbage
                aSelection.Delete( count, 1 );
                }
            else
                {
                entry.iDate = curTime;
                if ( aClearError )
                    {
                    entry.iError = KErrNone;
                    }
                iServerEntry->ChangeEntry( entry );
                }
            }
        }

    }

// ---------------------------------------------------------
// CMmsServerMtm::DecodePushedMessageL()
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::DecodePushedMessageL()
    {

    // Decode the pushed content to see, if it was
    // a notification or a delivery report

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("MmsServer decoding pushed message") );
#endif
    iNotification = KMsvNullIndexEntryId;
    iMmsHeaders->Reset();
    // the possible error from this is not important as the settings are
    // now stored in central repository
    iServerEntry->SetEntry( iServiceEntryId );

    // We decode first, and create the entry afterwards.
    if ( !iDecoder )
        {
        iDecoder = CMmsDecode::NewL( iFs );
        }

    if ( !iEntryWrapper )
        {
        iEntryWrapper = CMmsServerEntry::NewL(
            iFs,
            *iServerEntry,
            iServiceEntryId );
        }

    TRAP ( iError, iDecoder->DecodeHeadersL( *iEntryWrapper, *iMmsHeaders, *iEncodeBuffer ) );

    // If we could not allocate memory to decode, we must try again later.
    if ( iError == KErrNoMemory )
        {
        return;
        }

    if ( iError == KErrCorrupt ||
        iError == KErrTooBig )
        {
        // if the notification has illegal content,
        // it is just discarded, we must not tell the
        // mmswatcher to resend it.
        iError = KErrNone;
        return;
        }
    
    // Even if we encounter an error when decoding, we must save
    // the message for handling, because we must be able to send
    // "unrecognized" status back, if we get an unknown PDU
    // If we get a message with different major version number, it
    // may be so incompatible that we get an error while decoding.

    // If we get a notification, we make some sanity checks so that 
    // we can reject malicious notifications right away.

    // mark if this is a notification of a delivery report
    // send requests or retrieve confirmations are not pushed
    // They are equivalent to unrecognized type

    TUint32 messageType = 0;
    switch ( iMmsHeaders->MessageType() )
        {
        case KMmsMessageTypeMNotificationInd:
            messageType = KMmsMessageMNotificationInd;
            break;
        case KMmsMessageTypeDeliveryInd:
            messageType = KMmsMessageDeliveryInd;
            break;
        case KMmsMessageTypeReadOrigInd:
            messageType = KMmsMessageReadOrigInd;
            break;
        default:
            // unrecognized type.
            // We must send response to MMSC
            // This includes types that should never be pushed to us.
            messageType = KMmsMessageUnrecognized;
            break;
        }

    // If this is an extended notification, it may already contain the whole message
    TBool completeMessage = ( iMmsHeaders->MessageComplete() == KMmsExtendedMessageComplete );

    TBool passedChecks = ETrue; // we are optimistic

    // If we have a notification, we must do a couple of special tricks.

    if ( messageType == KMmsMessageMNotificationInd )
        {
        // Some handling has been moved here from CMmsDecode,
        // because it is better to have all notification logic in on place.

        // Expiry interval must be changed to absolute time,
        // otherwise it makes no sense.
        if ( iMmsHeaders->ExpiryDate() == 0 )
            {
            TTime time;
            // handle expiry in universal time in case user changes location
            time.UniversalTime();
            time += TTimeIntervalSeconds( iMmsHeaders->ExpiryInterval() );
            // we can't use "seconds from" as it only returns a
            // 32 bit signed integer. If fails in 2038.
            // "microseconds from" returns a 64 bit signed integer
            // expiry date in iMmsHeaders in in seconds from 1.1.1970.
            // interval must be changed back to seconds
            // This way the result may still be a 64bit integer ->
            // no overflow in 2038
            iMmsHeaders->SetExpiryDate(
                ( time.MicroSecondsFrom( TTime( KMmsYear1970String ) ).Int64() ) / KMmsMillion );
            }

        // Then we must check if the message type is acceptable.
        // Rejection based on message type overrides the status
        // set above. For example, if fetching is deferred, but
        // we get an advertisement, and we don't accept advertisements
        // we change the message status from deferred to rejected.
        
        switch ( iMmsHeaders->MessageClass() )
            {
            case EMmsClassPersonal:
                if ( iMmsSettings->AcceptPersonalMessages() == EFalse )
                    {
                    iMmsHeaders->SetStatus( KMmsMessageStatusRejected );
                    }
                break;
            case EMmsClassAdvertisement:
                if ( iMmsSettings->AcceptAdvertisementMessages() == EFalse ) 
                    {
                    iMmsHeaders->SetStatus( KMmsMessageStatusRejected );
                    }
                break;
            case EMmsClassInformational:
                if ( iMmsSettings->AcceptInformationalMessages() == EFalse )
                    {
                    iMmsHeaders->SetStatus( KMmsMessageStatusRejected );
                    }
                break;
            case EMmsClassAuto:
                // We accept automatic messages. The only automatic message we
                // know about is a text mode read report.
                // As we now handle read reports, we must accept automatic messages
                // in case some server has converted a read report into a text message
                // (possible if the server does not recognize the phone and know its
                // capabilities).
                break;
            default:
                // if we cannot determine the message type,
                // we reject it.
                // Message class header is mandatory in notification
                //iMmsHeaders->SetStatus( KMmsMessageStatusRejected );
                break;
            }
        // check if we accept anonymous messages
        if ( iMmsSettings->AcceptAnonymousMessages() == EFalse &&
            iMmsHeaders->Sender().Length() == 0 )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- Anonymous message rejected") );
#endif
            iMmsHeaders->SetStatus( KMmsMessageStatusRejected );
            }
            
        // If the encapsulation version is 1.3, the notification may already
        // contain the application id header.
        // If the application has not been registered, we reject the message
        // (at least until more complex support for the applicatiom message
        // handling has been implemented)
        if ( iMmsHeaders->ApplicId().Length() > 0 )
            {
            if ( !CMmsBaseOperation::RegisteredL( iMmsHeaders->ApplicId() ) )
                {
                iMmsHeaders->SetStatus( KMmsMessageStatusRejected );
                }
            }

        // if the extended notification contains the whole message
        // we must mark it as "retrieved"
        // If there is an extended message addressed to an application,
        // it is something that cannot be reasonably handled.
        // I hope that is an illegal case anyway.

        if ( completeMessage )
            {
            iMmsHeaders->SetStatus( 0 ); // not applicable
            }

        // legality checks:
        // TID is mandatory
        if ( iMmsHeaders->Tid().Length() == 0 )
            {
            passedChecks = EFalse;
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- invalid TID: length = %d"), iMmsHeaders->Tid().Length() );
#endif
            }
        // message class is mandatory
        if ( iMmsHeaders->MessageClass() == 0 )
            {
            passedChecks = EFalse;
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- message class not defined") );
#endif
            }

        // Messages with zero length must be accepted if message size may be
        // just the size of payload and subject (both may have zero length)
        // If headers are not included in calculation, zero length is acceptable here.

        // expiry is mandatory - but it cannot be 0 because we just set it.
        // If expiry is not given, the notification expires NOW
        
        // content location is mandatory
        if ( iMmsHeaders->ContentLocation().Length() == 0 )
            {
            passedChecks = EFalse;
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- no content location") );
#endif
            }
        }

    // if we have notification that does not fill our
    // criteria, we just throw it away. The purpose of this
    // is to discard possible malignant notifications that
    // would cause us to contact some unknown server and
    // do strange damage - or at least cause unncessary
    // network traffic.

    // If the message is complete, we keep it anyway

    if ( passedChecks == EFalse && !completeMessage )
        {
        iError = KErrNone;
        return;
        }

    TMsvEntry tEntry;
    tEntry.iType = KUidMsvMessageEntry;
    // This may be different for notifications in manual modes
    // This is ok, as all notifications are stored first to the internal MMS folder
    tEntry.iMtm = KUidMsgTypeMultimedia;

    // use the iRelatedId to bypass queue.
    tEntry.iServiceId = KMsvLocalServiceIndexEntryId;
    tEntry.iRelatedId = iServiceEntryId;
    tEntry.SetUnread( ETrue );
    tEntry.SetNew( ETrue );
    tEntry.SetVisible( EFalse );
    tEntry.SetComplete( EFalse );
    tEntry.SetInPreparation( ETrue );
    tEntry.SetReadOnly( EFalse );
    tEntry.iSize = iMmsHeaders->Size();
    
    //
    // Setting StoredInMMBox flag correctly
    //
    HandleMMBoxFlagL( tEntry );

    // Notifications are always originally children of MMS Folder
    // We have decoded our headers already.
    // We know if this is a notification or a delivery report,
    // and we can decide what to do.

    TMsvId parent = KMsvNullIndexEntryId;
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    TMsvId mmsFolder = FindMMSFolderL();
    if ( mmsFolder == KMsvNullIndexEntryId )
        {
        // If the id for our notification folder is 0, we are really in a mess
        // and cannot continue
        iError = KErrNotFound;
        return;
        }
    parent = mmsFolder;

    if ( messageType != KMmsMessageDeliveryInd &&
         messageType != KMmsMessageReadOrigInd &&
         messageType != KMmsMessageReadRecInd ) 
        { 
        // Check duplicates from parent folder, from Inbox and from mmbox folder 
        // no mater which receiving mode is on.
        TMsvId mmboxFolder = iMmsSettings->MMBoxFolder();
        if ( PruneDuplicateNotificationsL( parent, *iMmsHeaders ) ||
            PruneDuplicateNotificationsL( KMsvGlobalInBoxIndexEntryIdValue, *iMmsHeaders ) ||
            PruneDuplicateNotificationsL( mmboxFolder, *iMmsHeaders ))
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- duplicate - not stored") );
#endif
            iError = KErrNone;
            return;
            }
        }

    // If this is an extended notification that contains the whole message
    // the entry goes to inbox as a message, not a notification.
    // The fetching must not be scheduled in this case as we already have the
    // message.
    // However, if we are in home network, we must send acknowledge back to MMSC
    // Besides storing the notification as message entry, we must create another
    // entry to serve as base for sending the acknowledgement. Therefore we have
    // marked our status as "KMmsMessageStatusRetrieved"

    // this is the size of the attachment if we have an extended notification
    TInt attaSize = 0;

    if ( completeMessage )
        {
        parent = KMsvGlobalInBoxIndexEntryIdValue;
        tEntry.iMtm = KUidMsgTypeMultimedia;
        // indicate complete message
        iMmsHeaders->SetMessageType( KMmsMessageTypeMRetrieveConf );
        }

    // If we can't access the parent where the notification is to be stored,
    // we can do nothing
    iError = iServerEntry->SetEntry( parent );
    if ( iError != KErrNone )
        {
        return; // cannot continue
        }

    // Query about disk space. KMmsIndexEntryExtra is extra for TMsvEntry
    // Make one query, we assume no one takes away the disk space this fast.
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL(
        &iFs, iMmsHeaders->Size() + KMmsIndexEntryExtra + iMmsHeaders->Size(), iMessageDrive ) )
        {
        // we use standard error code here
        iError = KErrDiskFull;
        return; // cannot continue
        }

    iError = iServerEntry->CreateEntry( tEntry );

    if ( iError == KErrNone )
        {
        iError = iServerEntry->SetEntry( tEntry.Id() );
        iNotification = tEntry.Id();
        }

    CMsvStore* store = NULL;
    HBufC* buffer = NULL; // we need this to generate a description for the entry
    TBool attachmentAdded = EFalse; // to track if the extended notification text has been added to the message store or not.   

    if ( iError == KErrNone )
        {
        // if this is a whole message, create an attachment from the text
        if ( completeMessage && iNotification != KMsvNullIndexEntryId )
            {
            tEntry = iServerEntry->Entry(); // save the settings we have so far
            if ( iMmsHeaders->Subject().Length() > 0 )
                {
                tEntry.iDescription.Set( iMmsHeaders->Subject() );
                }
            else
                {
                // Save text as description if we have no subject
                TPtrC temp = iMmsHeaders->ExtendedNotification().Left( KMmsMaxDescription );
                buffer = HBufC::NewL( temp.Length() );
                // no need to put buffer onto cleanup stack - we don't leave before we are done
                buffer->Des().Copy( temp );
                TPtr pDescription = buffer->Des();
                TMmsGenUtils::ReplaceCRLFAndTrim( pDescription );
                tEntry.iDescription.Set( pDescription );
                }
            iServerEntry->ChangeEntry( tEntry );
            // If we have not allocated the buffer, it is NULL, and it is safe to delete.
            delete buffer;
            buffer = NULL;

            if ( iMmsHeaders->ExtendedNotification().Length() > 0 )
                {
                TMsvAttachmentId attachmentId = 0;
                CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
                CleanupStack::PushL( mimeHeaders );
                TPtrC8 temp;
                temp.Set( KMmsTextPlain );
                mimeHeaders->SetContentTypeL( temp.Left( temp.Find( KMmsSlash8 ) ) );
                mimeHeaders->SetContentSubTypeL( temp.Mid( temp.Find( KMmsSlash8 ) + 1 ) );
                mimeHeaders->SetMimeCharset( KMmsUtf8 ); // text saved as utf-8
                _LIT( KRelated, "att1.txt");
                mimeHeaders->SetSuggestedFilenameL( KRelated );
                // attaData must point to the text
                // buffer long enough for conversion
                const TInt KMmsConversionMultiplier = 5;
				// coverity[incorrect_multiplication][buffer_alloc]
                HBufC8* dataContent = HBufC8::NewL(
                    iMmsHeaders->ExtendedNotification().Length() * KMmsConversionMultiplier );
                CleanupStack::PushL( dataContent );
                TPtr8 attaData = dataContent->Des();
                CnvUtfConverter::ConvertFromUnicodeToUtf8(
                    attaData, iMmsHeaders->ExtendedNotification() );
                // set parent
                iError = iEntryWrapper->SetCurrentEntry( iNotification );
                
                if ( iError == KErrNone )
                    {
                    store = iEntryWrapper->EditStoreL();
                    CleanupStack::PushL( store );
                    TInt32 drmFlags = 0; //ignored
                    iError = iEntryWrapper->CreateFileAttachmentL(
                        *store,
                        KRelated,
                        attaData,
                        *mimeHeaders,
                        attachmentId,
                        attaSize,
                        drmFlags);
			        //If attachment is added to Message store successfully then attachmentAdded is set
			        if ( iError == KErrNone )
			            {
                        attachmentAdded = ETrue;
			            }    
                    store->CommitL();
                    CleanupStack::PopAndDestroy( store );
                    store = NULL;
                    }
                iMmsHeaders->SetExtendedNotificationL( TPtrC() );
                
                CleanupStack::PopAndDestroy( dataContent );
                CleanupStack::PopAndDestroy( mimeHeaders );
                }
            }
        if ( iServerEntry->SetEntry( iNotification ) == KErrNone &&
            iNotification != KMsvNullIndexEntryId )
            {
            store = iServerEntry->EditStoreL();
            CleanupStack::PushL(store);
            iMmsHeaders->StoreL(*store);
            store->CommitL();
            CleanupStack::PopAndDestroy( store );
            store = NULL;
            }
        }


    if ( ( iError == KErrNone || iMmsHeaders->MessageType() != 0 )
        && iNotification != KMsvNullIndexEntryId )
        {
        iError = iServerEntry->SetEntry( iNotification );
        if ( iError != KErrNone )
            {
            // If we have an error here, there is something
            // seriously wrong with the system
            if ( iServerEntry->SetEntry( parent ) == KErrNone )
                {
                iServerEntry->DeleteEntry( iNotification );
                }
            iNotification = KMsvNullIndexEntryId;
            iServerEntry->SetEntry( KMsvNullIndexEntryId );
            return; // cannot continue
            }
        }
    else
        {
        if ( iServerEntry->SetEntry( parent ) == KErrNone &&
            iNotification != KMsvNullIndexEntryId )
            {
            // We managed to create an entry, but not to decode
            // the buffer contents into the entry
            iServerEntry->DeleteEntry( iNotification );
            }
        iNotification = KMsvNullIndexEntryId;
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        return;
        }

    // finish the details, and the notification entry is ready to be used
    tEntry = iServerEntry->Entry();
    
    if ( messageType == KMmsMessageMNotificationInd )
        {
        // If the sender is a phone number, add alias.
        // We don't add alias for email addresses here, as the contact
        // database search for email addresses is very slow
        // if there are lots of contacts.
        buffer = HBufC::NewL( KMmsMaxDescription );
        CleanupStack::PushL( buffer );
        
        TPtr pBuffer = buffer->Des();

        if ( TMmsGenUtils::GenerateDetails( iMmsHeaders->Sender(),
            pBuffer, KMmsMaxDescription, iFs ) == KErrNone )
            {
            tEntry.iDetails.Set( pBuffer );
            }
        else
            {
            // We come here only if there was an fatal error in GenerateDetails.
            // Even if we don't find the alias, we have something in the string
            tEntry.iDetails.Set( iMmsHeaders->Sender() );
            }

        // set subject if available
        if ( iMmsHeaders->Subject().Length() > 0 )
            {
            tEntry.iDescription.Set( iMmsHeaders->Subject() );
            }
        CleanupStack::Pop( buffer );

        }
    else if ( messageType == KMmsMessageDeliveryInd &&
        iMmsHeaders->ToRecipients().MdcaCount() > 0 )
        {
        tEntry.iDetails.Set( iMmsHeaders->ToRecipients().MdcaPoint( 0 ) );
        }
    else
        {
        // keep LINT happy
        }

    tEntry.iMtmData1 &= ~KMmsMessageTypeMask;
    tEntry.iMtmData1 |= messageType;
    if ( iMmsHeaders->MessageClass() == EMmsClassAdvertisement )
        {
        tEntry.iMtmData1 |= KMmsMessageAdvertisement;
        }
    else if ( iMmsHeaders->MessageClass() == EMmsClassInformational )
        {
        tEntry.iMtmData1 |= KMmsMessageInformational;
        }
    else
        {
        // keep LINT happy
        }
        
    switch ( iMmsHeaders->MessagePriority() )
        {
        case KMmsPriorityNormal:
            tEntry.SetPriority( EMsvMediumPriority );
            break;
        case KMmsPriorityLow:
            tEntry.SetPriority( EMsvLowPriority );
            break;
        case KMmsPriorityHigh:
            tEntry.SetPriority( EMsvHighPriority );
            break;
        default:            
            // if not defined default is normal
            tEntry.SetPriority( EMsvMediumPriority );
            break;
        }
        
    tEntry.SetVisible( ETrue );
    tEntry.SetComplete( ETrue );
    tEntry.SetInPreparation( EFalse );
    tEntry.SetReadOnly( EFalse );
    tEntry.iDate.UniversalTime(); // This is arrival time
    tEntry.iSize = iMmsHeaders->Size() + attaSize; // add attachment data + mime headers

    // Set values to correspond to a new message if this was an extended notification
    // containing full text.

    if ( completeMessage )
        {
        tEntry.iMtmData1 &= ~KMmsMessageTypeMask;
        // We override message type.
        tEntry.iMtmData1 |= KMmsMessageMRetrieveConf | KMmsMessageMobileTerminated;
        tEntry.SetReadOnly( ETrue );
        tEntry.SetSendingState( KMsvSendStateSent );
        tEntry.iServiceId = iServiceEntryId;
        if ( attachmentAdded )
            {
        	tEntry.SetAttachment(ETrue);
            }
        }

    // we mark delivery reports as already sent so that
    // we get rid of them at next garbage collection, if the phone
    // boots or crashes before they are handled.
    // Delivery reports are lost if phone crashes.
    if ( messageType == KMmsMessageDeliveryInd ||
         messageType == KMmsMessageReadOrigInd )
        {
        tEntry.SetSendingState( KMsvSendStateSent );
        }

    iServerEntry->ChangeEntry( tEntry ); // ignore any error
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    delete buffer;
    buffer = NULL;

    // If the message was complete, we have put our notification to inbox, and it is ready to be used.
    // However, we still need an entry for sending back an acknowledgement to the server.
    // We must create a new new notification entry for that purpose.

    if ( completeMessage )
        {
        // don't schedule the entry that went into inbox
        iNotification = KMsvNullIndexEntryId;
        TInt error = KErrNone;
        tEntry.iType = KUidMsvMessageEntry; // This will never go to inbox
        tEntry.iMtm = KUidMsgTypeMultimedia;
        tEntry.iServiceId = iServiceEntryId;
        tEntry.iRelatedId = iServiceEntryId;
        tEntry.SetUnread( ETrue );
        tEntry.SetNew( ETrue );
        tEntry.SetVisible( EFalse );
        tEntry.SetComplete( EFalse );
        tEntry.SetInPreparation( ETrue );
        tEntry.SetReadOnly( EFalse );
        tEntry.iSize = iMmsHeaders->Size();
        tEntry.SetSendingState( KMsvSendStateUnknown );
        tEntry.iDescription.Set( iMmsHeaders->Subject() );
        tEntry.iMtmData1 &= ~KMmsMessageTypeMask;
        tEntry.iMtmData1 |= KMmsMessageMNotificationInd;
        // even if sending ack fails, this must not be moved to inbox
        // because the corresponding message already is there.
        tEntry.iMtmData2 |= KMmsDoNotMoveToInbox; 
        parent = mmsFolder;
        error = iServerEntry->SetEntry( parent );
        if ( error == KErrNone )
            {
            error = iServerEntry->CreateEntry( tEntry );
            }
        if ( error == KErrNone )
            {
            iError = iServerEntry->SetEntry( tEntry.Id() );
            if ( iError == KErrNone )
                {
                // Now we have a new entry that will be scheduled for sending the acknowledgement
                iNotification = tEntry.Id();
                iMmsHeaders->SetStatus( KMmsMessageStatusRetrieved );
                iMmsHeaders->SetMessageType( KMmsMessageTypeMNotificationInd );
                store = iServerEntry->EditStoreL();
                CleanupStack::PushL( store );
                iMmsHeaders->StoreL( *store );
                store->CommitL();
                CleanupStack::PopAndDestroy( store );
                store = NULL;
                tEntry.SetVisible( ETrue );
                tEntry.SetComplete( ETrue );
                tEntry.SetInPreparation( EFalse );
                iServerEntry->ChangeEntry( tEntry ); // ignore any error
                }
            }
        }

#ifndef _NO_MMSS_LOGGING_
    // log a little bit of something
    if ( messageType == KMmsMessageDeliveryInd )
        {
        TMmsLogger::Log( _L("- delivery report received") );
        }
    else if ( messageType == KMmsMessageMNotificationInd )
        {
        TMmsLogger::Log( _L("- notification received") );
        }
    else
        {
        TMmsLogger::Log( _L("- pushed message of type %d received"), messageType );
        }
#endif

    }

// ---------------------------------------------------------
// CMmsServerMtm::HandleNotificationL()
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleNotificationL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("MmsServer HandleNotification, %d items"), iMsvSelection->Count() );
#endif
    if ( iMsvSelection->Count() < 1 )
        {
        // nothing to do, give up
        *iRequestStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, iError );
        return;
        }

    // If both receiving modes in home and foreign network are "reject".
    // The notification is deleted.
    if( iMmsSettings->ReceivingModeHome() == EMmsReceivingReject &&
        iMmsSettings->ReceivingModeForeign() == EMmsReceivingReject )
        {
        // we play possum and delete the notification without
        // sending any response to MMSC.
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- playing possum, deleting notifications") );
#endif
        TInt count = iMsvSelection->Count();
        TMsvEntry entry;
        TMsvId parent = KMsvNullIndexEntryId;
        if ( iServerEntry->SetEntry( iMsvSelection->At( count - 1 ) ) == KErrNone ) 
            {
            entry = iServerEntry->Entry();
            parent = entry.Parent();
            }
        while ( count-- && parent != KMsvNullIndexEntryId )
            {
            if ( iServerEntry->SetEntry( iMsvSelection->At( count ) ) == KErrNone )
                {
                entry = iServerEntry->Entry();
                parent = entry.Parent();
                }
            
            if ( iServerEntry->SetEntry( parent ) == KErrNone )
                {
                iServerEntry->DeleteEntry( iMsvSelection->At( count ) );
                }
            }
        *iRequestStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, iError );
        return;
        }
        
    // When the notificatio arrived, it was checked, and if it was
    // a duplicate of an earlier one, it was not saved on disk.

    iCommand = EMmsReceive;
           
    // ScheduleL completes our caller
    // we want to get back to out RunL to check the error
    // Query about disk space.
    // Subroutine knows how much must be checked for task scheduler
    TInt error = KErrNone;
    if ( DiskSpaceBelowCriticalForSchedulingL( &iFs, 0, iMessageDrive ) )
        {
		// we use standard error code here
		#ifndef _NO_MMSS_LOGGING_
		TMmsLogger::Log( _L("HandleNotificationL.. Disk Full") );
		#endif
		error = KErrDiskFull;
		TInt count = iMsvSelection->Count();
		TMsvEntry entry;
		if ( iServerEntry->SetEntry( iMsvSelection->At( count - 1 ) ) == KErrNone ) 
		    {
			entry = iServerEntry->Entry();
			entry.iError = error;
			iServerEntry->ChangeEntry( entry );
			#ifndef _NO_MMSS_LOGGING_
			TMmsLogger::Log( _L("HandleNotificationL.. Setting ierror to Entry") );
			#endif
		    }
        
        }
    else
        {
        // We must set the caller's status to KRequest Pending because
        // CScheduleBaseServerMtm::ScheduleL does not do it.
        *iRequestStatus = KRequestPending;
        TRAP( error, ScheduleL( *iMsvSelection, EFalse, KNullDesC8, *iRequestStatus ) );
        // ScheduleL would complete our caller, but if it leaves,
        // we must complete. We don't want to leave...
        }
    if ( error != KErrNone )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("MmsServer HandleNotification status %d"), error );
#endif
        *iRequestStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, error );
        }

    }

// ---------------------------------------------------------
// CMmsServerMtm::HandleDeliveryReportL()
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleDeliveryReportL()
    {

    if ( iMsvSelection->Count() < 1 )
        {
        // nothing to do, give up
        *iRequestStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, iError );
        return;
        }
    
    iCommand = EMmsLogDeliveryReport;
           
    // Query about disk space.
    // Subroutine knows how much must be checked for task scheduler
    TInt error = KErrNone;
    if ( DiskSpaceBelowCriticalForSchedulingL( &iFs, 0, iMessageDrive ) )
        {
        // we use standard error code here
        error = KErrDiskFull;
        }
    else
        {
        // ScheduleL completes our caller
        // We must set the caller's status to KRequest Pending because
        // CScheduleBaseServerMtm::ScheduleL does not do it.
        *iRequestStatus = KRequestPending;
        TRAP( error, ScheduleL( *iMsvSelection, EFalse, KNullDesC8, *iRequestStatus ) );
        // ScheduleL would complete our caller, but if it leaves,
        // we must complete. We don't want to leave...
        }
    if ( error != KErrNone )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("MmsServer HandleDeliveryReport status %d"), error );
#endif
        *iRequestStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, error );
        }
    }


// ---------------------------------------------------------
// CMmsServerMtm::LogDeliveryReportL()
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::LogDeliveryReportL()
    {

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("MmsServer Logging delivery report") );
#endif
    // this subroutine does not do critical disk space level check
    // normally we just update an existing entry changing only status
    // which does not change disk space usage.
    // Only if our previous entry has been deleted, we add something.
    // We don't check that. The amount of disk space needed should
    // be less than 100 bytes anyway. Large messages are more critical
    // than small log entries.

    if ( iMsvSelection->Count() < 1 )
        {
        // nothing to do, give up
        // we complete our caller
        *iRequestStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, iError );
        return;
        }
    
    iCommand = EMmsLogDeliveryReport;
    TRequestStatus* status = &iStatus;
    TBool readReport = EFalse; // first guess - delivery report

    iError = iServerEntry->SetEntry( iMsvSelection->At( 0 ) );
    if ( iError == KErrNotFound )
        {
        // The entry we are supposed to handle has disappeared.
        // We complete the active object.
        // RunL will call UpdateEntriesL, and that function
        // will delete the entry from the list, and try the next
        // one, if it exists.
        // We must set the error to "none" to delete the entry
        // instead of retrying.
        iError = KErrNone;
        // get back to our own RunL, it will complete caller
        *iRequestStatus = KRequestPending;
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete( status, iError );
        return;
        }

    TMsvEntry entry;
    if ( iError == KErrNone )
        {
        entry = iServerEntry->Entry();
        }
    else
        {
        // cannot access entry.
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("MmsServer could not access delivery report") );
#endif
        // Retry later.
        // now our error is not KErrNone, so we will reschedule
        // in UpdateEntriesL
        *iRequestStatus = KRequestPending;
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete( status, iError );
        }
        
    if ( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageReadOrigInd )
        {
        // The remote party is in "From" field because that's the recipient
        // that is sending the read report
        readReport = ETrue;
        }
    else if ( ( entry.iMtmData1 & KMmsMessageTypeMask ) != KMmsMessageDeliveryInd )
        {
        iError = KErrNone;
        // get back to our own RunL
        *iRequestStatus = KRequestPending;
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete( status, iError );
        return;
        }
    
    if ( !iMmsLog )
        {
        iLogClient = CLogClient::NewL( iFs );
        iLogViewEvent = CLogViewEvent::NewL( *iLogClient );
        iMmsLog = CMmsLog::NewL( *iLogClient, *iLogViewEvent, iFs );
        }

    if ( !iLogEvent )
        {
        iLogEvent = CLogEvent::NewL();
        }

    if ( !iRemoteParties )
        {
        // We only handle one delivery report at a time
        iRemoteParties = new ( ELeave )CDesCArrayFlat( 1 );
        }
    else
        {
        iRemoteParties->Reset();
        }

    // save the items that are the same for all our events
    iLogEvent->SetEventType( KLogMmsEventTypeUid );
    iLogClient->GetString( iLogString, R_LOG_DIR_OUT );
    iLogEvent->SetDirection( iLogString );
    iLogEvent->SetDurationType( KLogDurationNone );
    // This should never stay
    iLogClient->GetString( iLogString, R_LOG_DEL_SENT );
    iLogEvent->SetStatus( iLogString );

    CMsvStore* store = NULL;

    TRAP( iError, store = iServerEntry->ReadStoreL(); )

    if ( iError == KErrNone )
        {
        CleanupStack::PushL( store );
        iMmsHeaders->RestoreL( *store );
        CleanupStack::PopAndDestroy( store );
        iLogEvent->SetDataL( iMmsHeaders->MessageId() );
        // Use delivery time from delivery report
        iLogEvent->SetTime( TTime( KMmsYear1970String ) +
            TTimeIntervalMicroSeconds( iMmsHeaders->Date() * KMmsMillion ) );
        
        switch ( iMmsHeaders->Status() )
            {
            case KMmsMessageStatusRetrieved:
            case KMmsMessageStatusForwarded: // forwarded is delivered from our point of view
                iDeliveryStatus = ETrue;
                iLogClient->GetString( iLogString, R_LOG_DEL_DONE );
                break;
            case KMmsMessageStatusExpired:
            case KMmsMessageStatusRejected:
            case KMmsMessageStatusUnreachable:
                iDeliveryStatus = EFalse;
                iLogClient->GetString( iLogString, R_LOG_DEL_FAILED );
                break;
            default:
                if ( !readReport )
                    {
                    // if status cannot be mapped, it is just "pending"
                    // KMmsMessageStatusDeferred, KMmsMessageStatusUnrecognized, and KMmsMessageStatusIndeterminate
                    // map to "pending" state
                    iLogClient->GetString( iLogString, R_LOG_DEL_PENDING );
                    }
                else
                    {
                    // read report can only have status "read" or 
                    // "deleted without being read"
                    // We need some string mapping for those...
                    if ( iMmsHeaders->ReadStatus() == KMmsReadStatusRead )
                        {
                        // read
                         iLogString.Copy( KLogsMsgReadText );
                        }
                    else
                        {
                        // deleted without being read
                        // This does not change "delivered" status
                        // But in case the delivery report has not arrived,
                        // This ensures that the information of delivery gets stored.
                        // If the user deleted the message without reading it,
                        // it must have been delivered first.
                        iLogClient->GetString( iLogString, R_LOG_DEL_DONE );
                        }
                    }
                break;
            }
            
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("MmsServer delivery status code %d"), iMmsHeaders->Status() );
        TMmsLogger::Log( _L(" - delivery status %S"), &iLogString );
        TMmsLogger::Log( _L(" - delivery datetime:") );
        CMmsBaseOperation::LogDateL( iLogEvent->Time() );
#endif
        iLogEvent->SetStatus( iLogString );
        if ( iMmsHeaders->ToRecipients().MdcaCount() == 0 &&
            iMmsHeaders->Sender().Length() == 0 )
            {
            // No recipient, cannot log
            // no use retrying either
            *iRequestStatus = KRequestPending;
            iStatus = KRequestPending;
            SetActive();
            User::RequestComplete( status, KErrNone );
            return;
            }
        if ( !readReport )
            {
            iRemoteParties->AppendL( TMmsGenUtils::PureAddress(
                iMmsHeaders->ToRecipients().MdcaPoint( 0 ) ) );
            }
        else
            {
            // If we have a read report, the remote party is the sender
            iRemoteParties->AppendL( TMmsGenUtils::PureAddress(
                iMmsHeaders->Sender() ) );
            }

// CMmsLog is responsible for setting our status to "KRequestPending"
// If the status or the entry is already "read" it must not be changed
// back to "delivered"
// CMmsLog must take care of that because it is the component that
// finds the corresponding entry from the log database
        iMmsLog->StartL( *iLogEvent, *iRemoteParties, iStatus);
        *iRequestStatus = KRequestPending;
        SetActive();
        }
    else
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("MmsServer could not access delivery report") );
#endif
        // Retry later.
        // now our error is not KErrNone, so we will reschedule
        // in UpdateEntriesL
        *iRequestStatus = KRequestPending;
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete( status, iError );
        }

    }
    
// ---------------------------------------------------------
// CMmsServerMtm::PruneDuplicateNotificationsL
// 
// ---------------------------------------------------------
//
TBool CMmsServerMtm::PruneDuplicateNotificationsL( TMsvId aParent, CMmsHeaders& aNotification )
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("MmsServer Pruning duplicate notifications") );
#endif
    TInt error = KErrNone;
    TBool pruned = EFalse;
    TMsvId mmsFolder = FindMMSFolderL();
    
    if ( aParent == KMsvNullIndexEntryId )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- no proper parent") );
#endif
        return EFalse;
        }

    error = iServerEntry->SetEntry( aParent );
    if ( error != KErrNone )
        {
        // cannot access parent, cannot prune.
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- cannot access notification parent, error %d "), error );
#endif
        return EFalse;
        }

    // show invisible entries
    TMsvSelectionOrdering ordering =
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue );
    iServerEntry->SetSort( ordering );

    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection; 
    CleanupStack::PushL( selection );
    if ( aParent == mmsFolder )
        {
        error = iServerEntry->GetChildrenWithType( KUidMsvMessageEntry, *selection );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- from MMS Folder") );
#endif
        }
    else
        {
        error = iServerEntry->GetChildrenWithMtm( KUidMsgMMSNotification, *selection );
#ifndef _NO_MMSS_LOGGING_
        if ( aParent == KMsvGlobalInBoxIndexEntryIdValue )
            {
            TMmsLogger::Log( _L("- from Inbox") );
            }
        else
            {
            TMmsLogger::Log( _L("- from mmbox folder") );
            }
#endif
        }

    TInt count = selection->Count();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- %d notifications on disk"), count );
#endif
    if ( error != KErrNone || count == 0 )
        {
        // cannot check or no old notifications found, anything goes
        CleanupStack::PopAndDestroy( selection ); // selection
        return EFalse;
        }

    TInt i;
    CMmsHeaders* notification = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    CleanupStack::PushL( notification );
    CMsvStore* store = NULL;
    for ( i = count; i > 0 && !pruned; i-- )
        {
        error = iServerEntry->SetEntry( selection->At( i - 1 ) );
        if ( error != KErrNone )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- cannot access notification") );
#endif
            // cannot handle this entry.
            continue;
            }
        if ( ( ( iServerEntry->Entry().iMtmData1 != 0 ) &&
            ( ( iServerEntry->Entry().iMtmData1 & KMmsMessageTypeMask ) !=
            KMmsMessageMNotificationInd ) ) ||
            ( iServerEntry->Entry().iMtmData2 & KMmsNotificationBinary ) ) 
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- not a notification") );
#endif
            continue;
            }

        // binary notifications (empty entries) were already handled, so we can delete the rest
        if ( iServerEntry->Entry().iMtmData1 == 0 )
            {
            // remove garbage.
            error = iServerEntry->SetEntry( aParent );
            if ( error == KErrNone )
                {
                // never mind the error - we are just doing our best
                iServerEntry->DeleteEntry( selection->At( i - 1 ) );
                }
            continue;
            }

        error = KErrNone;
        TRAP( error, 
            {
            store = iServerEntry->ReadStoreL();
            CleanupStack::PushL( store );
            notification->RestoreL( *store );
            CleanupStack::PopAndDestroy( store ); // store
            })
        store = NULL;

        if ( error != KErrNone )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- cannot access notification") );
#endif
            // cannot handle this entry.
            continue;
            }

        // content location is used to identify notifications
        // referring to the same message.
        // Content location is the only information given back to
        // MMSC when fetching a message, therefore it must be unique
        if ( notification->ContentLocation().Compare( aNotification.ContentLocation() ) == 0 )
            {
            // Identical. This probably means that we have not sent a response yet,
            // and MMSC has sent us a new notification.

#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- content locations match") );
#endif
            pruned = ETrue;
            }
        }

    CleanupStack::PopAndDestroy( notification );
    CleanupStack::PopAndDestroy( selection );
    error = iServerEntry->SetEntry( KMsvNullIndexEntryId );

    return pruned;
    }

// ---------------------------------------------------------
// CMmsServerMtm::CheckNotificationsL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::CheckNotificationsL( CMsvEntrySelection& aSelection )
    {
    TInt error = KErrNone;
    TMsvId parent = KMsvNullIndexEntryId;
    TMsvId mmsFolder = FindMMSFolderL();
    if ( mmsFolder == KMsvNullIndexEntryId )
        {
        // no folder...
        return;
        }
    
    TInt i;
    
    // Don't mix notifications from mms folder and inbox,
    // in debug mode Symbian scheduler panics.
    if ( aSelection.Count() > 0 )
        {
        i = aSelection.Count() - 1;
        while ( i >= 0 && parent == KMsvNullIndexEntryId )
            {
            error = iServerEntry->SetEntry( aSelection.At( 0 ) );
            if ( error == KErrNone )
                {
                parent = iServerEntry->Entry().Parent();
                }
            else if ( error == KErrNotFound )
                {
                // The entry has disappeared already.
                aSelection.Delete( i );
                }
            else
                {
                // keep LINT happy
                }
            i--;    
            }
        }
    
    if ( parent == KMsvNullIndexEntryId ) 
        {
        parent = mmsFolder;
        }

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CheckNotificationsL, got %d entries"), aSelection.Count() );
#endif

    error = iServerEntry->SetEntry( parent );
    User::LeaveIfError( error );
    
    // If no notifications input find notifications from mms folder
    if ( parent == mmsFolder && aSelection.Count() == 0 )
        {
        error = iServerEntry->GetChildrenWithType( KUidMsvMessageEntry, aSelection );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("Notifications found in mms folder %d "), aSelection.Count() );
#endif
        if ( error != KErrNone )
            {
            aSelection.Reset(); // make sure we return no garbage
            User::Leave( error );
            }
        }
    
    // notifications in inbox
    // If we have a selection, we don't generate new ones,
    // An empty selection only lists notifications from MMS folder, we cannot
    // touch inbox stuff otherwise.
    // MMS Client MTM lists notifications from inbox when mode is changed
    // from manual to automatic.
    
    if ( parent == KMsvGlobalInBoxIndexEntryId )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("Notifications found in inbox %d "), aSelection.Count() );
#endif
        TInt j;
        for ( j = aSelection.Count() - 1; j >= 0; j-- )
            {
            error = iServerEntry->SetEntry( aSelection.At( j ) );
            TMsvEntry entry;
            if ( error == KErrNone )
                {
                entry = iServerEntry->Entry();
                }

            // Drop notification, if operationForbidden flag is on OR
            // if messageexpired flag is on OR
            // if notification is not stored in mmbox and forward operation has been succussfull OR
            // if notification is deleted succuessfully from mmbox OR 
            // if notification is fetched successfully from mmbox.
            // Or if notification cannot be accessed (used by someone else)
            if ( error != KErrNone ||
                entry.iMtmData2 & KMmsNewOperationForbidden ||
                entry.iMtmData2 & KMmsMessageExpired ||
                ( entry.iMtmData2 & KMmsOperationFinished &&
                !( entry.iMtmData2 & KMmsOperationResult) &&
                !(entry.iMtmData2 & KMmsStoredInMMBox) ) )
                {
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L(" Dropped " ));
                aSelection.Delete( j );
#endif
                }
            else
                {
                // mark this that others can't start do an operation
                entry.iMtmData2 |= KMmsNewOperationForbidden; 
            
                // readonly is needed to set EFalse for scheduling
                entry.SetReadOnly( EFalse ); 
                iServerEntry->ChangeEntry( entry );
                // We keep this entry in our selection
                }
            }
        aSelection.Compress();
        }
        
    error = iServerEntry->SetEntry( parent );

    // Check the list and see what we got.
    // The list may contain both notifications and delivery reports
    
    TInt count = aSelection.Count();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Found %d entries altogether"), count );
#endif
    TMsvEntry tEntry;
    CMsvEntrySelection* thisNotification = NULL;
    TTime now;
    TTime entryTime;
    
    for ( i = count; i > 0; i-- )
        {
        error = iServerEntry->SetEntry( aSelection.At( i - 1 ) );
        if ( error == KErrNoMemory )
            {
            User::Leave( error );
            }
        else if ( error != KErrNone )
            {
            continue;
            }
        else
            {
            // keep LINT happy
            }
        tEntry = iServerEntry->Entry();
        parent = tEntry.Parent();
        // We keep only notifications - the rest are removed from the list.
        // If we are doing garbage collection, all non-notifications are
        // deleted from the disk (because they will never expire).
        // See if a notification or dummy entry
        if ( iServerEntry->Entry().iMtmData2 & KMmsNotificationBinary &&
            iCurrentCommand == EMmsGarbageCollection )
            {
            // get rid of these as these are empty
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- dummy entry - deleting"));
#endif
            error = iServerEntry->SetEntry( parent );
            if ( error == KErrNone )
                {
                iServerEntry->DeleteEntry( aSelection.At( i - 1 ) );
                }
            aSelection.Delete( i - 1 );
            }
        else if ( ( tEntry.iMtmData1 & KMmsMessageTypeMask ) != KMmsMessageMNotificationInd )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- not a notification"));
#endif
            if ( iCurrentCommand == EMmsGarbageCollection )
                {
                // If we are doing garbage collection, these are deleted, too
                // If any delivery reports or other non-notification stuff
                // is around at garbage collection time, it has become garbage.
                
                // Garbage collection is called with MMS Service, not with
                // local service, so it means that garbage collection cannot
                // be called when some other operation is ongoing.
                // It should not be possible to delete an entry that is being
                // handled already because access to MMS Server MTM is sequential
                
                // However, a delivery report or read report might just have been
                // scheduled for handling. Check that they won't get deleted too soon
                
                thisNotification = new( ELeave ) CMsvEntrySelection;
                CleanupStack::PushL( thisNotification );
                thisNotification->AppendL( aSelection.At( i - 1 ) );
                iScheduleSend->CheckScheduleL( *thisNotification );
                CleanupStack::PopAndDestroy( thisNotification );
                thisNotification = NULL;

                TBool alreadyScheduled = EFalse;
                
                if ( iServerEntry->Entry().Scheduled() )
                    {
                    // Already scheduled - check, if schedule is valid
                    // Leave the schedule, if it is in the future - but not too much
                    now.UniversalTime();
                    now += TTimeIntervalSeconds( KMmsScheduleDelay );
                    entryTime = iServerEntry->Entry().iDate;
                    if ( ( ( entryTime - TTimeIntervalHours( KMmsSanityInterval ) ) <= now ) &&
                        ( entryTime > now ) )
                        {
                        // scheduled in the future, we don't touch it
                        alreadyScheduled = ETrue;
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- already scheduled"));
#endif
                        }
                    }
                
                error = iServerEntry->SetEntry( parent );
                if ( error == KErrNone && !alreadyScheduled )
                    {
                    // we delete extra entries - but if they seem to have
                    // legal schedule we leave them, the scheduling will
                    // handle them in due time
                    iServerEntry->DeleteEntry( aSelection.At( i - 1 ) );
                    }
                }
            // We always clear the non-notifications from our list
            // Either they were already legally scheduled and were left alone
            // or they were gagbage and were deleted
            aSelection.Delete( i - 1 );
            }
        else
            {
            // Check if scheduled

            thisNotification = new( ELeave ) CMsvEntrySelection;
            CleanupStack::PushL( thisNotification );
            thisNotification->AppendL( aSelection.At( i - 1 ) );
            if ( iCommand != EMmsReceiveForced )
                {
                iScheduleSend->CheckScheduleL( *thisNotification );
                }
            else
                // if we do forced fetch, we clean up old schedules first
                {
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- cleaning schedules"));
#endif
                CleanSchedulesL( *thisNotification );
                tEntry = iServerEntry->Entry();
                }

            if ( iServerEntry->Entry().Scheduled() )
                {
                // Already scheduled - check, if schedule is valid
                // Reschedule this, if it was scheduled in the past
                // The entry is left into the selection list.
                // If the list is rescheduled, this entry will
                // be rescheduled to a later time.
                // Leave the schedule, if it is in the future - but not too much
                now.UniversalTime();
                now += TTimeIntervalSeconds( KMmsScheduleDelay );
                entryTime = iServerEntry->Entry().iDate;
                if ( ( ( entryTime - TTimeIntervalHours( KMmsSanityInterval ) ) <= now ) &&
                                ( entryTime > now ) )
                    {
                    // scheduled in the future, we don't touch it
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- already scheduled"));
#endif
                    aSelection.Delete( i - 1 );
                    }
                }
            else
                {
                // Check expiration
                CMsvStore * store = NULL;
                store = iServerEntry->ReadStoreL();
                CleanupStack::PushL( store );
                iMmsHeaders->RestoreL( *store );
                CleanupStack::PopAndDestroy( store );
                TTime now;
                now.UniversalTime();
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("MMSserver checking notifications") );
                TMmsLogger::Log( _L("MMS terminal universal datetime: ") );
                CMmsBaseOperation::LogDateL( now );
                TMmsLogger::Log( _L("MMS message expiry datetime:") );
                CMmsBaseOperation::LogNetworkFormatDateL( iMmsHeaders->ExpiryDate() );
#endif
                if ( ( ( iMmsHeaders->ExpiryDate() + iMmsSettings->ExpiryOvershoot() ) *
                    KMmsMillion ) < now.MicroSecondsFrom( TTime( KMmsYear1970String ) ).Int64() )
                    {
                    // expired
                    // remove schedule, 
                    iScheduleSend->DeleteScheduleL( *thisNotification );

                    // if the entry is in inbox or in mmbox folder, let it go                   
                    if ( parent == KMsvGlobalInBoxIndexEntryIdValue  ||
                         parent == iMmsSettings->MMBoxFolder() )
                        {
                        tEntry.SetReadOnly( ETrue );
                        // not forbidden for a new operation
                        tEntry.iMtmData2 &= ~KMmsNewOperationForbidden;
                        iServerEntry->ChangeEntry( tEntry );

                        }
                    else // delete the whole entry if it is in mms folder
                        {
                        error = iServerEntry->SetEntry( parent );
                        if ( error == KErrNone )
                            {
                            iServerEntry->DeleteEntry( aSelection.At( i - 1 ) );
#ifndef _NO_MMSS_LOGGING_
                            TMmsLogger::Log( _L("- whole entry deleted"));
#endif
                            }
                        }
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- already expired"));
#endif
                    aSelection.Delete( i - 1 );
                    }
                }
            CleanupStack::PopAndDestroy( thisNotification );
            thisNotification = NULL;
            }
        }
    aSelection.Compress();
    
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CheckNotificationsL, %d entries left"), aSelection.Count() );
#endif

    }

// ---------------------------------------------------------
// CMmsServerMtm::CreateNotificationsL()
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::CreateNotificationsL()
    {
    // This function is for testing and message variation

    TInt err = KErrNone;
    TInt i;
    TInt count = 0;
    TUint size = 0;
    // Include old notifications into the selection
    // Except when doing message generation at boot time -
    // Then only new notifications are handled
    if ( iCurrentCommand != EMmsMessageGeneration )
        {
        CheckNotificationsL( *iMsvSelection );
        }
    count = iMsvSelection->Count();

    // If there are notifications to be handled, we don't
    // create new ones, because new notification creation
    // should be used for testing only
    if ( !iMmsSettings->LocalMode() && count > 0 )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CreateNotificationsL - Global mode") );
        TMmsLogger::Log( _L("- %d old notifications, not generating new ones"), count );
#endif
        return;
        }
        
    RFs fs; // We need a separate session to be able to set the session path
    err = fs.Connect();
    if ( err != KErrNone )
        {
        return;
        }
    CleanupClosePushL( fs );

    HBufC8* bufferPointer = HBufC8::NewL( KMaxFileName );
    CleanupStack::PushL( bufferPointer );
    TPtr8 buffer = bufferPointer->Des();
// coverity[assign_zero]
    CDir* fileList = NULL;

    // create notifications for local messages:
    if ( iMmsSettings->LocalMode() )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CreateNotificationsL - Local mode"));
#endif
        TEntry* entry = new( ELeave ) TEntry; // allocated from heap to save stack space
        CleanupStack::PushL( entry );
        HBufC* filename = HBufC::NewL( iMmsSettings->LocalModeIn().Length() );
        CleanupStack::PushL( filename );
        TPtr fileNamePtr = filename->Des();
        fileNamePtr.Copy( iMmsSettings->LocalModeIn() );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("local mode dir %S "), &fileNamePtr );
#endif
        err = fs.SetSessionPath( fileNamePtr );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("set session path returned: %d"), err );
        HBufC* temp = HBufC::NewL( KMaxPath );
        CleanupStack::PushL( temp );
        TPtr tempPtr = temp->Des();
        fs.SessionPath( tempPtr );
        TMmsLogger::Log( _L("Session path: %S"), &tempPtr );
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
#endif
        err = fs.Entry( fileNamePtr, *entry );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("get path properties returned: %d"), err );
#endif
        if ( err == KErrNone )
            {
            TFindFile* finder = new( ELeave ) TFindFile( fs ); // allocated from heap to save stack space
            CleanupStack::PushL( finder );
            _LIT( KWild, "*" );

            err = finder->FindWildByPath( KWild, &fileNamePtr, fileList );
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("find files returned: %d"), err );
#endif
            CleanupStack::PopAndDestroy( finder );
            
            if ( fileList )
                {
                CleanupStack::PushL( fileList );
                }

            if ( err == KErrNone )
                {
                count = fileList->Count();
                }
            else
                {
                count = 0;
                }
            }

#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("found %d files in %S"), count, &fileNamePtr );
#endif
        for (i = 0; i < count; i++ )
            {
			// coverity[var_deref_model]
            iParse.Set( ( ( *fileList )[i] ).iName, &fileNamePtr, NULL );
            buffer.Copy( iParse.FullName() );

            size = fs.Entry( iParse.FullName(), *entry );
            size = entry->iSize;

            iNotification = KMsvNullIndexEntryId;

#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("notification size %d"), size );
#endif
            if ( size > 0 )
                {
                if ( !iEncodeBuffer )
                    {
                    iEncodeBuffer = CBufFlat::NewL( 0x400 );
                    }
                else
                    {
                    iEncodeBuffer->Reset();
                    }

                CreateNotificationL( buffer, size );
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("notification created") );
#endif

                DecodePushedMessageL();
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("notification decoded") );
#endif
                delete iEncodeBuffer;
                iEncodeBuffer = NULL;
                }
            else
                {
                fs.Delete( iParse.FullName() );
                }
        
            if ( iNotification != KMsvNullIndexEntryId )
                {
                iMsvSelection->AppendL( iNotification );
                }

            }
        if ( fileList )
            {
            CleanupStack::PopAndDestroy( fileList );
            }
            
        CleanupStack::PopAndDestroy( filename );
        CleanupStack::PopAndDestroy( entry );
        }

    CleanupStack::PopAndDestroy( bufferPointer );
    CleanupStack::PopAndDestroy( &fs ); // close fs
    iServerEntry->SetEntry( iServiceEntryId );

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CreateNotificationsL - %d notifications"), iMsvSelection->Count() );
#endif
    }

// ---------------------------------------------------------
// CMmsServerMtm::CreateNotificationL()
//
// ---------------------------------------------------------
//
void CMmsServerMtm::CreateNotificationL( TDesC8& aUrl, TInt aSize )
    {
    // for test purposes aUrl will contain the filename.

    // Reset sets the default encapsulation version
    // The default version has been set from MmsSettings in NewL
    iMmsHeaders->Reset();

    // construct the notification into iMmsHeaders, and call encode

    iMmsHeaders->SetMessageType( KMmsMessageTypeMNotificationInd );

    TTime currentTime;
    currentTime.UniversalTime();
    currentTime.Int64();

    TBufC8<KMMSMAXTIDLENGTH> tid;
    tid.Des().NumUC( currentTime.Int64(), EHex );
    iMmsHeaders->SetTidL( tid );

    iMmsHeaders->SetMessageClass( EMmsClassPersonal );
    iMmsHeaders->SetMessageSize( aSize );
    const TInt KTenHours = 10 * 60 * 60; // 10 hours relative expiry
    iMmsHeaders->SetExpiryInterval( KTenHours );
    iMmsHeaders->SetContentLocationL( aUrl );
 
    CMmsEncode* encoder = CMmsEncode::NewL( iFs );
    CleanupStack::PushL( encoder );
    encoder->EncodeHeadersL( *iMmsHeaders, *iEncodeBuffer );
    CleanupStack::PopAndDestroy( encoder ); // encoder
    
    }

// ---------------------------------------------------------
// CMmsServerMtm::GarbageCollectionL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::GarbageCollectionL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("MMSServer doing garbage collection") );
#endif

    // are we in offline mode...
    // We use CMmsSendOperation to allow change in the name of the
    // base opeation class. The function is in the base class anyway.
    if ( !CMmsSendOperation::NetworkOperationsAllowed() )
        {
        // not allowed to send or receive anything
        User::Leave( KMmsErrorOfflineMode );
        }
        
    // MMS watcher sends us reason codes with garbage collection parameters.
    // paramPack().iReasonFlags will contain the reason flags.
    // paramPack().iMediaUnavailableTime tells when the memory card was removed
    TMMSGarbageCollectionParameters param;
    TPckgC<TMMSGarbageCollectionParameters> paramPack( param );
    paramPack.Set( iParameter );
    
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- reason code:") );
    if ( paramPack().iReasonFlags & KMmsReasonBoot )
        {
        TMmsLogger::Log( _L(" -- Boot") );
        }
    if ( paramPack().iReasonFlags & KMmsReasonMessageStoreChanged )
        {
        TMmsLogger::Log( _L(" -- MessageStoreChanged") );
        }
    if ( paramPack().iReasonFlags & KMmsReasonNetworkAllowed )
        {
        TMmsLogger::Log( _L(" -- Network operations allowed") );
        }
    if ( paramPack().iReasonFlags & KMmsReasonBackupEnded )
        {
        TMmsLogger::Log( _L(" -- Backup ended") );
        }
    if ( paramPack().iReasonFlags & KMmsReasonHotswap )
        {
        TMmsLogger::Log( _L(" -- Hotswap") );
        }
    if ( paramPack().iReasonFlags & KMmsReasonEnvironmentTimeChanged )
        {
        TMmsLogger::Log( _L(" -- Environment time change") );
        }
#endif

    //
    // Forward entries left in outbox
    //
    GcOutBoxNotificationsL();
    
    //
    // Message entries left in outbox
    //
    GcOutBoxMessagesL();
    
    //
    // Notifications in MMSFolder (automatic fetch)
    //
    GcMmsFolderNotificationsL();
    
    //
    // Notifications from inbox
    //
    GcInboxNotifications();

    //
    // Notifications in mmbox folder    
    //
    GcMmboxFolderNotifications();
        
    }

// ---------------------------------------------------------
// CMmsServerMtm::CleanSchedulesL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::CleanSchedulesL( CMsvEntrySelection& aSelection )
    {
    // delete old schedules and reset scheduled entry.
    // we trap each entry separately, so that we can clean as
    // many as possible even if there are errors

    TMsvId oldEntry = iServerEntry->Entry().Id();
    TInt i;
    CMsvEntrySelection* thisNotification = new( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( thisNotification );
    TInt error = KErrNone;

    for ( i = 0; i < aSelection.Count(); i++ )
        {
        TRAP ( error,
            {
            error = iServerEntry->SetEntry( aSelection.At( i ) );
            if ( error == KErrNone )
                {
                CMmsScheduledEntry* mmsScheduledEntry =
                    CMmsScheduledEntry::NewL( iServerEntry->Entry() );
                CleanupStack::PushL( mmsScheduledEntry );
                thisNotification->Reset();
                thisNotification->AppendL( aSelection.At( i ) );
                iScheduleSend->DeleteScheduleL( *thisNotification );

                CMsvStore* store = NULL;
                store = iServerEntry->EditStoreL();
                CleanupStack::PushL( store );
                // We clean up the old scheduled entry data
                // by storing a new clean scheduled entry
                mmsScheduledEntry->StoreL( *store );
                store->CommitL();

                CleanupStack::PopAndDestroy( store );
                CleanupStack::PopAndDestroy( mmsScheduledEntry );
                }
            }
            );
        }
    CleanupStack::PopAndDestroy( thisNotification );
    // restore the entry we were pointing at
    // if this fails, something is seriously wrong. We did not delete anything.
    iServerEntry->SetEntry( oldEntry );

    }

// ---------------------------------------------------------
// CMmsServerMtm::FindMMSFolderL
// 
// ---------------------------------------------------------
//
TMsvId CMmsServerMtm::FindMMSFolderL()
    {
    // Actually the value in iMmsSettings should be correct
    // but we try to really search for the folder in case
    // the message store has been moved or something
    TMsvId mmsFolderId = iMmsSettings->NotificationFolder();
    TInt error;
    
    // get a new entry, don't mess up with the original entry.
    CMsvServerEntry* workingEntry = NULL;
    TRAP( error, workingEntry = iServerEntry->NewEntryL( KMsvLocalServiceIndexEntryId ) );
    CleanupStack::PushL( workingEntry );
    
    if ( error == KErrNoMemory )
        {
        CleanupStack::PopAndDestroy( workingEntry );
        User::Leave( error );
        }

    if ( error != KErrNone )
        {
        // no can do
        CleanupStack::PopAndDestroy( workingEntry );
        return mmsFolderId;
        }

    // Get List of services
    // show invisible entries
    TMsvSelectionOrdering ordering =
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByIdReverse, ETrue );
    workingEntry->SetSort( ordering );
    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
    error = workingEntry->GetChildrenWithType( KUidMsvFolderEntry, *selection );
    if ( error != KErrNone )
        {
        // no can do
        delete selection;
        if ( error == KErrNoMemory )
            {
            CleanupStack::PopAndDestroy( workingEntry );
            User::Leave( error );
            }
        else
            {
            CleanupStack::PopAndDestroy( workingEntry );
            return mmsFolderId;
            }
        }
    CleanupStack::PushL( selection );

    // Now we should have a list of all local folders.
    // prune away the standard folders.
    // They should be at the end of the list

    TInt count = selection->Count();
    TInt i;

    for ( i = count - 1; i >= 0; i-- )
        {
        if ( selection->At( i ) <= KMsvDeletedEntryFolderEntryId )
            {
            // Anything below this should not be ours
            selection->Delete( i );
            }
        }

    // anything left...
    count = selection->Count();

    for ( i = 0; i < count && mmsFolderId == KMsvNullIndexEntryId ; i++ )
        {
        error = workingEntry->SetEntry( selection->At( i ) );
        if ( error == KErrNoMemory )
            {
            CleanupStack::PopAndDestroy( selection );
            CleanupStack::PopAndDestroy( workingEntry );
            User::Leave( error );
            }
        // must be exact match
        if ( error == KErrNone &&
            workingEntry->Entry().iDetails.Compare( KMMSNotificationFolder ) == 0 )
            {
            mmsFolderId = selection->At( i );
            }
        }
    CleanupStack::PopAndDestroy( selection );
    CleanupStack::PopAndDestroy( workingEntry );

    return mmsFolderId;
    }

// ---------------------------------------------------------
// CMmsServerMtm::DiskSpaceBelowCriticalForScheduling
// 
// ---------------------------------------------------------
//
TBool CMmsServerMtm::DiskSpaceBelowCriticalForSchedulingL( RFs* aFs,
            TInt aBytesToWrite, TInt aMessageDrive)
    {
    TBool belowCritical = EFalse; // optimistic
    // The amount of memory needed depends on the number of messages to handle
    belowCritical = TMmsGenUtils::DiskSpaceBelowCriticalLevelL(
        aFs, KMmsTaskSpace * iMsvSelection->Count(), EDriveC );
    if ( !belowCritical )
        {
        belowCritical = TMmsGenUtils::DiskSpaceBelowCriticalLevelL(
            aFs, aBytesToWrite, aMessageDrive );
        }
    return belowCritical;
    }

// ---------------------------------------------------------
// CMmsServerMtm::ScheduleSelectionL
// 
// ---------------------------------------------------------
//
TInt CMmsServerMtm::ScheduleSelectionL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsServerMtm::ScheduleSelectionL") );
#endif
    
    TInt error = KErrNone;
    TInt i;
    if ( iMsvSelection->Count() > 0 )
        {
        TCommandParameters param;
        TPckgC<TCommandParameters> paramPack( param );
        paramPack.Set( iParameter );

        MakeDatesIdenticalL( *iMsvSelection, paramPack().iInitialDelay, ETrue );
        // Query about disk space.
        // Subroutine knows how much must be checked for task scheduler
        if ( DiskSpaceBelowCriticalForSchedulingL( &iFs, 0, iMessageDrive ) )
            {
            // we use standard error code here
            error = KErrDiskFull;
            }
        else
            {
            // We must set the caller's status to KRequest Pending because
            // CScheduleBaseServerMtm::ScheduleL does not do it.
            *iRequestStatus = KRequestPending;
            TRAP( error, ScheduleL( *iMsvSelection, EFalse, iParameter, *iRequestStatus ) );
            // ScheduleL would complete our caller, but if it leaves,
            // we must complete. We don't want to leave without cleaning up first.
    #ifndef _NO_MMSS_LOGGING_
            if ( error != KErrNone )
                {
                TMmsLogger::Log( _L("- ScheduleL left with error %d"), error );
                }
    #endif
            }
        if ( error != KErrNone )
            {
            // Put the entries into failed state because we could not schedule them.
            for ( i = 0; i < iMsvSelection->Count(); i++ )
                {
                if ( iServerEntry->SetEntry( iMsvSelection->At( i ) ) == KErrNone )
                    {
                    TMsvEntry entry = iServerEntry->Entry();
                    TMsvId parent = entry.Parent();

                    TMsvId mmboxFolder = iMmsSettings->MMBoxFolder();
                    
                    if ( entry.iMtm == KUidMsgMMSNotification && 
                        ( parent == KMsvGlobalInBoxIndexEntryIdValue ||
                        parent == mmboxFolder ) )
                        {
                        // if the notification is in mmbox folder and duplicate exists, 
                        // the duplicate has to be marked too.
                        if ( parent == mmboxFolder )
                            {
    #ifndef _NO_MMSS_LOGGING_
                            TMmsLogger::Log( _L("- parent is mmbox folder") );
    #endif
                            TRAP_IGNORE( CMmsBaseOperation::MarkDuplicateL(
                                CMmsBaseOperation::EMmsNotificationOperationFailed,
                                *iServerEntry ) );
                            }

                        // Mark original notification
                        entry = iServerEntry->Entry();
                        CMmsBaseOperation::MarkNotificationOperationFailed( entry );
                        entry.SetSendingState( KMsvSendStateFailed );
                        entry.iError = error;
                        entry.SetReadOnly( ETrue );
    #ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- marked original notification as failed") );
    #endif
                        }
                    iServerEntry->ChangeEntry( entry );
                    }
                }
            *iRequestStatus = KRequestPending;
            User::RequestComplete( iRequestStatus, error );
            }
        }
    return error;
    }

// ---------------------------------------------------------
// CMmsServerMtm::HandleDummyEntryL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleDummyEntryL()
    {
    //
    // Get access to the streamstore of the entry
    //
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("HandleDummyEntryL()") );
#endif
    if ( iMsvSelection->Count() == 0 )
        {
        User::Leave( KErrNotFound );
        }
        
    User::LeaveIfError( iServerEntry->SetEntry( iMsvSelection->At( 0 ) ) );
    CMsvStore* store = iServerEntry->ReadStoreL();
    CleanupStack::PushL( store ); // ***
    RMsvReadStream ins;
    ins.OpenLC( *store, KUidBinaryNotificationStream ); // ***

    //
    // Read first 32 bits into integer. It will tell the length of the data
    //
    TInt datalength = ins.ReadUint32L();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L(" - Streamed %d bytes from dummy-entry's store"), datalength );
#endif

    //
    // Reserve correct size buffer
    //
    if ( !iEncodeBuffer )
        {
        iEncodeBuffer = CBufFlat::NewL( datalength );
        iEncodeBuffer->ResizeL( datalength );
        }
    else
        {
        // Throw away old stuff and resize
        iEncodeBuffer->Reset();
        iEncodeBuffer->ResizeL( datalength );
        }
    TPtr8 pBuf = iEncodeBuffer->Ptr( 0 );

    //
    // Read the data into buffer
    //
    ins.ReadL( pBuf );
    CleanupStack::PopAndDestroy( &ins ); // close in
    CleanupStack::PopAndDestroy( store );

    //
    // Clean up: the dummy entry is deleted
    //
    if ( iServerEntry->SetEntry( iServerEntry->Entry().Parent() ) == KErrNone )
        {
        iServerEntry->DeleteEntry( iMsvSelection->At( 0 ) );
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    iMsvSelection->Reset();
    }

// ---------------------------------------------------------
// CMmsServerMtm::HandleMMBoxFlagL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleMMBoxFlagL( TMsvEntry& aEntry )
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("HandleMMBoxFlagL()") );
#endif
    // Is there an mmbox field in the notification PDU...
    if( iMmsHeaders->MMBoxMessageHeadersL().MmsStored() == 0 )
        {
        TInt value = 0;
        TInt error = KErrNone;
        CRepository* repository = NULL;
        TRAPD( error2,        
            // Notification does not contain explicit field, so checking cenrep
            repository = CRepository::NewL( KUidMmsServerMtm ) ); // ***
        if ( error2 == KErrNone )
            {
            CleanupStack::PushL( repository );
            #ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- repository created") );
            #endif
            error = repository->Get( KMmsEnginePseudoMMBox, value );
            if( error == KErrNone && value == 1 )
                {
                // Assuming MMBox storage in this case
                aEntry.iMtmData2 |= KMmsStoredInMMBox;
                #ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- Setting MMBox flag based on cenrep data!") );
                #endif
                }
            else
                {
                #ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- Key not found from cenrep data, or value != 1") );
                #endif
                }
            CleanupStack::PopAndDestroy( repository );
            }
        }
    else if( iMmsHeaders->MMBoxMessageHeadersL().MmsStored() == KMmsYes )
        {
        // Notification specifies the flag as 'yes'
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Notification contains stored in mmbox flag!") );
        #endif
        aEntry.iMtmData2 |= KMmsStoredInMMBox;
        }
    else
        {
        // Keep LINT happy
        }
    }

// ---------------------------------------------------------
// CMmsServerMtm::GcOutBoxMessagesL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::GcOutBoxMessagesL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsServerMtm::GcOutBoxMessagesL") );
#endif
    //
    // MMS watcher sends us reason codes with garbage collection parameters.
    // paramPack().iReasonFlags will contain the reason flags.
    // paramPack().iMediaUnavailableTime tells when the memory card was removed
    //
    TMMSGarbageCollectionParameters param;
    TPckgC<TMMSGarbageCollectionParameters> paramPack( param );
    paramPack.Set( iParameter );
    
    TMsvEntry entry;
    
    iMsvSelection->Reset();
    TInt err = iServerEntry->SetEntry( KMsvGlobalOutBoxIndexEntryId );
    // Get entries of type MMS Message
    if ( err == KErrNone )
        {
        err = iServerEntry->GetChildrenWithMtm( KUidMsgTypeMultimedia, *iMsvSelection );
        }
    TInt count = iMsvSelection->Count();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- found %d message entries from outbox"), count );
#endif

    if ( err == KErrNone && count > 0 )
        {
        // iSendOperation should not be around in this context.
        // To avoid possible memory leaks, we clean it away anyway
        delete iSendOperation; 
        iSendOperation = NULL;
        iSendOperation = CMmsSendOperation::NewL( iFs, iMmsSettings );
                
        iSendOperation->Failed().AppendL( iMsvSelection->Back( 0 ), count );
        iSendOperation->Sent().SetReserveL( count );

        CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
        CleanupStack::PushL( selection );
        while ( count-- )
            {
            // The detailed handling depends both on the state of the entry
            // and the event that triggered the garbage collection.
            // Some states are handled differently depending on the event,
            // some states are always hanldled the same way.
            if ( iServerEntry->SetEntry( iMsvSelection->At( count ) ) == KErrNone )
                {
                entry = iServerEntry->Entry();
                selection->Reset();
                selection->AppendL( iMsvSelection->At( count ) );
                switch ( entry.SendingState() )
                    {
                    case KMsvSendStateSent:
                        {
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- already sent"));
#endif
                        // Sent entries are always moved to sent folder (or deleted)
                        // regardless of trigger event
                        // A sent message should not be in outbox. It should have already been
                        // moved to sent folder. However, as it is not moved, we must do it.
                        if ( iServerEntry->SetEntry( KMsvGlobalOutBoxIndexEntryId ) == KErrNone )
                            {
                            if ( iMmsSettings->MoveToSent() )
                                {
                                // Move entry from Outbox to Sent Folder
                                // count has originally been set to iMsvSelection->Count().
                                iServerEntry->MoveEntryWithinService(
                                    iMsvSelection->At( count ), KMsvSentEntryIdValue );
                                // This will not fail
                                if ( iSendOperation->Failed().Count() > count )
                                    {
                                    iSendOperation->Sent().AppendL(
                                        iSendOperation->Failed().At( count ) );
                                    }
                                }
                            else    
                                {
                                // Move entry to Message Heaven
                                iServerEntry->DeleteEntry( iMsvSelection->At( count ) );
                                }
                            }
                        iSendOperation->Failed().Delete( count );
                        break;
                        }
                    case KMsvSendStateWaiting:
                    case KMsvSendStateSending:
#ifndef _NO_MMSS_LOGGING_
                        if ( entry.SendingState() == KMsvSendStateWaiting )
                            {
                            TMmsLogger::Log( _L("- waiting"));
                            }
                        else
                            {
                            TMmsLogger::Log( _L("- sending"));
                            }
#endif
                        if ( paramPack().iReasonFlags &
                            ( KMmsReasonHotswap | KMmsReasonBackupEnded ) )
                            {
                            // reschedule if last time accessed earlier that media unavailable time stamp
                            if ( iServerEntry->Entry().iDate > paramPack().iMediaUnavailableTime )
                                {
                                // Access time is after media unavailable,
                                // It means that something has been done to this entry already.
                                // Better leave it as is.
#ifndef _NO_MMSS_LOGGING_
                                TMmsLogger::Log( _L(" -- accessed after media unavailable - leave as is"));
#endif
                                iSendOperation->Failed().Delete( count );
                                }
                            }
                        else
                            {
                            iSendOperation->Failed().Delete( count );
                            }
                        break;
                    case KMsvSendStateUponRequest:
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- upon request"));
#endif
                        // Entries in "upon request" state are not rescheduled
                        iSendOperation->Failed().Delete( count );
                        break;
                    case KMsvSendStateSuspended:
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- suspended"));
#endif
                        // entries in "upon request" or "suspended" state are not rescheduled
                        // unless we have switched from offline mode back to online mode
                        if ( !( ( paramPack().iReasonFlags & KMmsReasonNetworkAllowed ) &&
                            ( entry.SendingState() == KMmsOffLineState ) &&
                            ( entry.iError == KMmsErrorOfflineMode ) ) )
                            {
                            iSendOperation->Failed().Delete( count );
                            }
                        break;
                    case KMsvSendStateScheduled:
                    case KMsvSendStateResend:
                        {
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- already scheduled"));
#endif
                        // These are either rescheduled or scheduled for the first time
                        // reschedule if the schedule is in the past
                        // If the scedule is in the past, CheckSchedule moves it forward.
                        // However, if it is too much in the past, it is moved forward by a year.
                        // We must do some sanity chaecking about the amount of change
                        
                        // we set flag to indicate that our shedule had passed
                        TInt oldSchedule = EFalse;
                        TTime now;
                        now.UniversalTime();
                        
                        if ( iServerEntry->Entry().iDate < now )
                            {
                            oldSchedule = ETrue;
                            }
                        
                        iScheduleSend->CheckScheduleL( *selection );
                        if ( iServerEntry->Entry().Scheduled() )
                            {
                            // already scheduled - check, if schedule is valid
                            TTime scheduleTime = iServerEntry->Entry().iDate;
                            now += TTimeIntervalSeconds( 1 );
#ifndef _NO_MMSS_LOGGING_
                            TMmsLogger::Log( _L("MMS terminal universal datetime: ") );
                            CMmsBaseOperation::LogDateL( now );
                            TMmsLogger::Log( _L("Scheduled datetime:") );
                            CMmsBaseOperation::LogDateL( scheduleTime );
#endif
                            // leave the schedule, if it is in the future - but not too much
                            TTimeIntervalMinutes allowance = 
                                TTimeIntervalMinutes( KMmsScheduleAllowance );
                            if ( ( ( ( scheduleTime - allowance ) <= now ) && ( scheduleTime > now ) )
                                || !oldSchedule )
                                {
                                // scheduled in the future, we don't touch it
                                // or the schedule has been originally set into the future,
                                // and we don't want to change it
                                iSendOperation->Failed().Delete( count );
                                }
#ifndef _NO_MMSS_LOGGING_
                            else
                                {
                                TMmsLogger::Log( _L("- bad schedule"));
                                }
#endif
                            }
                        break;
                        }
                    case KMsvSendStateNotApplicable:
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- not applicable"));
#endif
                        break;
                    case KMsvSendStateUnknown:
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- unknown"));
#endif
                        break;
                    case KMsvSendStateFailed:
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- failed"));
#endif
                        break;
                    default:
                        // all entries that are left in the array, are rescheduled in the end
                        break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( selection );
        iCommand = EMmsSend;
        // whatever was left gets rescheduled
        MakeDatesIdenticalL( iSendOperation->Failed(), KMmsGarbageCollectionDelay );
        TRAP( err, UpdateEntriesL() );
        }
    }

// ---------------------------------------------------------
// CMmsServerMtm::GcMmsFolderNotificationsL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::GcMmsFolderNotificationsL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsServerMtm::GcMmsFolderNotificationsL") );
#endif
    //
    // MMS watcher sends us reason codes with garbage collection parameters.
    // paramPack().iReasonFlags will contain the reason flags.
    // paramPack().iMediaUnavailableTime tells when the memory card was removed
    //
    TMMSGarbageCollectionParameters param;
    TPckgC<TMMSGarbageCollectionParameters> paramPack( param );
    paramPack.Set( iParameter );
    
    TMsvEntry entry;
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    TMsvId parent = FindMMSFolderL();

    TInt err = iServerEntry->SetEntry( parent );
    iMsvSelection->Reset(); 
    // All entries in MMSFolder have the same Uid as messages
    // There may be notifications, delivery reports and read reports.
    // If the whole notification folder has disappeared we cannot have messages either...
    if ( err == KErrNone && parent != KMsvNullIndexEntryId )
        {
        err = iServerEntry->GetChildrenWithType( KUidMsvMessageEntry, *iMsvSelection );
        }
    TInt count = iMsvSelection->Count();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L(" - found %d notification entries"), iMsvSelection->Count() );
#endif

    if ( err == KErrNone && count > 0 )
        {
        // iReceiveMessage should not be around in this context.
        // To avoid possible memory leaks, we clean it away anyway
        delete iReceiveMessage; 
        iReceiveMessage = NULL;
        iReceiveMessage = CMmsReceiveMessage::NewL( iFs, iMmsSettings  );
                
        iReceiveMessage->Failed().AppendL( iMsvSelection->Back( 0 ), count );
        iReceiveMessage->Received().SetReserveL( count );

        while ( count-- )
            {
            if ( iServerEntry->SetEntry( iMsvSelection->At( count ) ) == KErrNone )
                {
                entry = iServerEntry->Entry();
                if ( entry.SendingState() == KMsvSendStateSent )
                    {
                    // successful entry, delete it
                    // This will not fail
                    // Read reports are never marked as "sent" so they should not
                    // disturb the logic here
                    if ( iReceiveMessage->Failed().Count() > count )
                        {
                        iReceiveMessage->Received().AppendL(
                            iReceiveMessage->Failed().At( count ) );
                        iReceiveMessage->Failed().Delete( count );
                        }
                    }
                else if ( ( ( entry.SendingState() == KMsvSendStateWaiting ) || 
                    ( entry.SendingState() == KMsvSendStateSending ) ) )
                    {
                    if ( paramPack().iReasonFlags & ( KMmsReasonHotswap | KMmsReasonBackupEnded ) )
                        {
                        // These will be rescheduled only if they were accessed the last time
                        // before the media unavailable event.
                        // Otherwise they might be a new operation.
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- hotswap or backup trigger"));
#endif
                        if ( entry.iDate > paramPack().iMediaUnavailableTime )
                            {
                            iReceiveMessage->Failed().Delete( count );
                            }
                        }
                    }
                else if ( entry.iError == KErrNone && entry.iMtmData2 & KMmsNotifyResponseSent )
                    {
                    // If this is legally deferred, it will not be rescheduled
                    CMsvStore* store = iServerEntry->ReadStoreL();
                    CleanupStack::PushL( store );
                    iMmsHeaders->RestoreL( *store );
                    CleanupStack::PopAndDestroy( store );
                    if ( iMmsHeaders->Status() == KMmsMessageStatusDeferred )
                        {
                        // this has been deferred legally - do not reschedule
                        iReceiveMessage->Failed().Delete( count );
                        }
                    }
                else
                    {
                    // Keep LINT happy
                    }
                }
            // Here could be a branch that prevents automatic sending of
            // messages that have been suspended by user. However, user can suspend
            // fetching only in manual mode, and then the notifications are in inbox
            }
        // Check notifications will remove expired notifications
        if ( iReceiveMessage->Failed().Count() > 0 )
            {
            // only check if there was anything left
            CheckNotificationsL( iReceiveMessage->Failed() );
            }
        iCommand = EMmsReceive;
        MakeDatesIdenticalL( iReceiveMessage->Failed(), KMmsGarbageCollectionDelay );
        TRAP( err, UpdateEntriesL() );
        err = KErrNone; // we don't care about the error, we just do our best
        }
    }
    
// ---------------------------------------------------------
// CMmsServerMtm::GcMmsFolderNotificationsL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::GcInboxNotifications()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsServerMtm::GcInboxNotifications") );
#endif
    //
    // MMS watcher sends us reason codes with garbage collection parameters.
    // paramPack().iReasonFlags will contain the reason flags.
    // paramPack().iMediaUnavailableTime tells when the memory card was removed
    //
    TMMSGarbageCollectionParameters param;
    TPckgC<TMMSGarbageCollectionParameters> paramPack( param );
    paramPack.Set( iParameter );
    
    TMsvEntry entry;
    
    TInt err = iServerEntry->SetEntry( KMsvGlobalInBoxIndexEntryId );
    iMsvSelection->Reset(); 
    if ( err == KErrNone )
        {
        err = iServerEntry->GetChildrenWithMtm( KUidMsgMMSNotification, *iMsvSelection );
        }
    TInt count = iMsvSelection->Count();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L(" - found %d notifications in inbox"), iMsvSelection->Count() );
#endif
    TInt i;
    if ( err == KErrNone && count > 0 )
        {
        for ( i = 0; i < count; i++ )
            {
            if ( iServerEntry->SetEntry( iMsvSelection->At( i ) ) == KErrNone )
                {
                entry = iServerEntry->Entry();
                
                //
                // If booting and notif is not allowed to start a new operation -> something wrong
                //
                if( ( paramPack().iReasonFlags & ( KMmsReasonBoot ) ) &&
                    ( entry.iMtmData2 & KMmsNewOperationForbidden ) )
                    {
                    entry.iError = KMmsGeneralError;
                    CMmsBaseOperation::MarkNotificationOperationFailed( entry );
                    entry.SetReadOnly( ETrue );
                    entry.iMtmData2 &= ~KMmsOperationIdentifier;
                    iServerEntry->ChangeEntry( entry );                 
                    }
                }
                        
            }
        }
    }
    
// ---------------------------------------------------------
// CMmsServerMtm::GcOutBoxNotificationsL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::GcOutBoxNotificationsL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsServerMtm::GcOutBoxNotificationsL") );
#endif
    //
    // MMS watcher sends us reason codes with garbage collection parameters.
    // paramPack().iReasonFlags will contain the reason flags.
    // paramPack().iMediaUnavailableTime tells when the memory card was removed
    //
    TMMSGarbageCollectionParameters param;
    TPckgC<TMMSGarbageCollectionParameters> paramPack( param );
    paramPack.Set( iParameter );

    //
    // Get selection of notifications from Outbox
    //
    TInt err = iServerEntry->SetEntry( KMsvGlobalOutBoxIndexEntryId );
    iMsvSelection->Reset();
    if ( err == KErrNone )
        {
        err = iServerEntry->GetChildrenWithMtm( KUidMsgMMSNotification, *iMsvSelection );
        }
    TInt count = iMsvSelection->Count();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- found %d notification entries from Outbox"), count );
#endif
    if( err != KErrNone || count <= 0 )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- no entries to clean up"), count );
#endif
        return;
        }

    //
    // Creating iForwardOperation  that will handle resends
    //
    delete iForwardOperation;
    iForwardOperation = NULL;
    iForwardOperation = CMmsForwardOperation::NewL( iFs, iMmsSettings );                
    iForwardOperation->Failed().AppendL( iMsvSelection->Back( 0 ), count );

    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    //
    // Loop through the found notifications (forward entries)
    //
    while( count-- )
        {
        //
        // The detailed handling depends both on the state of the entry
        // and the event that triggered the garbage collection.
        //
        TMsvEntry tEntry;
        err = iServerEntry->SetEntry( iMsvSelection->At( count ) );
        if( err != KErrNone )
            {
            // If entry not accessible, start a new round
            continue;
            }
        tEntry = iServerEntry->Entry();
        selection->Reset();
        selection->AppendL( iMsvSelection->At( count ) );

        //
        // Switch through based on notification entry's state
        //
        switch ( tEntry.SendingState() )
            {
            case KMsvSendStateSuspended:
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- KMsvSendStateSuspended"));
#endif
                if( tEntry.iError == KMmsErrorOfflineMode )
                    {
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- entry error == offline"));
#endif
                    if( paramPack().iReasonFlags & KMmsReasonNetworkAllowed )
                        {
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- GCreason == back-from-offline: rescheduling entry"));
#endif
                        // Reschedule entry
                        // i.e. nothing done here
                        }
                    else // GC reason is not "back from offline"
                        {
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- GCreason != back-from-offline: leaving entry suspended"));
#endif
                        // Leave entry suspended (applies to both entry types)
                        // i.e. take entryId out from to-be-scheduled list
                        iForwardOperation->Failed().Delete( count );
                        }
                    }
                else // entry.iError not equal to KMmsErrorOfflineMode
                    {
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- entry error != offline, set forward entry as Failed") );
#endif
                    // Set forward entry's send-state to failed
                    tEntry.SetFailed( ETrue );

                    // Clear related notification from Inbox:
                    // Get the related notification id
                    CMsvStore* store = NULL;
                    store = iServerEntry->EditStoreL();
                    CleanupStack::PushL( store ); // ***
                    iMmsHeaders->RestoreL( *store );
                    CleanupStack::PopAndDestroy( store );
                    TMsvId relatedEntryId = iMmsHeaders->RelatedEntry();
                    iMmsHeaders->Reset(); // headers not needed any more

                    if( relatedEntryId != KMsvNullIndexEntryId )
                        {
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- related notification-entry exists, clearing it") );
#endif
                        // Set context (iServerEntry and entry) to notification and clear it
                        err = iServerEntry->SetEntry( relatedEntryId );
#ifndef _NO_MMSS_LOGGING_
                        if( err != KErrNone )
                            {
                            TMmsLogger::Log( _L("- ERROR: Could not set entry") );
                            }
#endif
                        if ( err == KErrNone )
                            {
                            tEntry = iServerEntry->Entry();
                            tEntry.iMtmData2 &= ~KMmsNewOperationForbidden; // not forbidden
                            tEntry.iMtmData2 &= ~KMmsOperationOngoing;      // not ongoing
                            tEntry.iMtmData2 |= KMmsOperationFinished;      // finished
                            tEntry.iMtmData2 |= KMmsOperationResult;        // NOK
                            tEntry.SetReadOnly( ETrue );
                            err = iServerEntry->ChangeEntry( tEntry );
                            }
#ifndef _NO_MMSS_LOGGING_
                        if( err != KErrNone )
                            {
                            TMmsLogger::Log( _L("- ERROR: Could not change related entry") );
                            }
                        TMmsLogger::Log( _L("- Clear the related-entry link itself") );
#endif

                        // Clear related-id link from forward entry
                        err = iServerEntry->SetEntry( iMsvSelection->At( count ) );
                        if ( err == KErrNone )
                            {
                            store = iServerEntry->EditStoreL();
                            CleanupStack::PushL( store ); // ***
                            iMmsHeaders->RestoreL( *store );
                            iMmsHeaders->SetRelatedEntry( KMsvNullIndexEntryId );
                            iMmsHeaders->StoreL( *store );
                            store->CommitL();
                            CleanupStack::PopAndDestroy( store );
                            }
                        iMmsHeaders->Reset(); // headers not needed any more
#ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L("- Related-entry and the link cleared") );
#endif
                        }

                    // Clean up. 
                    // iServerEntry will be needed and set next time
                    // at the start of this loop.
                    iServerEntry->SetEntry( KMsvNullIndexEntryId );

                    // Leave entry suspended (applies to both entry types)
                    // i.e. take entryId out from to-be-scheduled list
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- not scheduling entry") );
#endif
                    iForwardOperation->Failed().Delete( count );
                    }
                break;
            default:
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- Forward entry's sendState == %d, not scheduling"), tEntry.SendingState() );
#endif
                    iForwardOperation->Failed().Delete( count );
                break;
            }
        } // while loop

    CleanupStack::PopAndDestroy( selection );
    iCommand = EMmsForward;
    // whatever was left gets rescheduled
    MakeDatesIdenticalL( iForwardOperation->Failed(), KMmsGarbageCollectionDelay );
    TRAP( err, UpdateEntriesL() );
#ifndef _NO_MMSS_LOGGING_
    if( err != KErrNone )
        {
        TMmsLogger::Log( _L("- UpdateEntriesL failed with code %d"), err );
        }
#endif
    }

// ---------------------------------------------------------
// CMmsServerMtm::GcMmboxFolderNotifications
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::GcMmboxFolderNotifications()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsServerMtm::GcMmboxFolderNotifications") );
#endif

    TMsvId mmboxFolder = iMmsSettings->MMBoxFolder();
    // if mmbox folder is not found, no need to check notifications from there. 
    if ( mmboxFolder == KMsvNullIndexEntryId )
        {
        return;
        }

    TInt error = iServerEntry->SetEntry( mmboxFolder );
    iMsvSelection->Reset(); 
    if ( error == KErrNone )
        {
        error = iServerEntry->GetChildrenWithMtm( KUidMsgMMSNotification, *iMsvSelection );
        }
    TInt count = iMsvSelection->Count();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L(" - mmbox folder contains %d notifications"), count );
#endif
    if ( error == KErrNone && count > 0 )
        {

        for ( TInt i=0; i < count; i++ )
            {
            if ( iServerEntry->SetEntry( iMsvSelection->At( i ) ) == KErrNone )
                {
                TMsvEntry entry = iServerEntry->Entry();

                // If forbidden flag is on, mark as failed. 
                if( entry.iMtmData2 & KMmsNewOperationForbidden )
                    {             
                    entry.iError = KMmsGeneralError;
                    entry.SetSendingState( KMsvSendStateSuspended );
                    CMmsBaseOperation::MarkNotificationOperationFailed( entry );
                    entry.SetReadOnly( ETrue );
                    entry.iMtmData2 &= ~KMmsOperationIdentifier;
                    iServerEntry->ChangeEntry( entry );
                    }           
                }
            }
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    }

// ---------------------------------------------------------
// CMmsServerMtm::FindDuplicateNotificationL
// 
// ---------------------------------------------------------
//
TInt CMmsServerMtm::FindDuplicateNotificationL(
    TMsvId aParent, CMmsHeaders& aHeaders, TMsvId& aDuplicate )
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsServerMtm::FindDuplicateNotificationL") );
#endif
    
    aDuplicate = KMsvNullIndexEntryId;
 
    if ( aParent == KMsvNullIndexEntryId )
        {
        return KErrNotSupported;
        }

    TInt error = iServerEntry->SetEntry( aParent );
    if ( error != KErrNone )
        {
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        return error;
        }

    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection; 
    CleanupStack::PushL( selection );

    error = iServerEntry->GetChildrenWithMtm( KUidMsgMMSNotification, *selection );

    TInt count = selection->Count();
    if ( count == 0 )
        {
        error = KErrNotSupported;
        }

    if ( error != KErrNone  )
        {
        CleanupStack::PopAndDestroy( selection );
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        return error;
        }

    CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    CleanupStack::PushL( mmsHeaders );
     
    for ( TInt i = count; i > 0 && ( aDuplicate == KMsvNullIndexEntryId ); i-- )
        {
        error = iServerEntry->SetEntry( selection->At( i - 1 ) );
        if ( error == KErrNone )
            {            
            CMsvStore* store = iServerEntry->ReadStoreL();
            CleanupStack::PushL( store );
            mmsHeaders->RestoreL( *store );
            CleanupStack::PopAndDestroy( store );

            // content location must match 
            if ( mmsHeaders->ContentLocation().Compare( aHeaders.ContentLocation() ) == 0 )
                {
                // Identical. This probably means that we have not sent a response yet,
                // and MMSC has sent us a new notification.

#ifndef _NO_MMSS_LOGGING_
               TMmsLogger::Log( _L("- content locations match") );
#endif
                TMsvEntry entry = iServerEntry->Entry();
                aDuplicate = entry.Id();
                }

            }
        
        }

    CleanupStack::PopAndDestroy( mmsHeaders );
    CleanupStack::PopAndDestroy( selection );
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    return error;
    }    
    
// ---------------------------------------------------------
// CMmsServerMtm::SendReadReportL
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::SendReadReportL()
    {
    if ( iMsvSelection->Count() > 0 )
        {
        delete iReadReport;
        iReadReport = NULL;
        iReadReport = CMmsReadReport::NewL( iFs, iMmsSettings );

        iReadReport->StartL( *iMsvSelection, *iServerEntry,
            iServiceEntryId, iStatus );
        *iRequestStatus = KRequestPending;
        SetActive();    
        }
    else
        {
        *iRequestStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, KErrNotFound );
        }
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::LogCommandCode( TInt aCommand )
    {
#ifndef _NO_MMSS_LOGGING_
    switch ( aCommand )
        {
        case EMmsSend:
            TMmsLogger::Log( _L(" - Send") );
            break;
        case EMmsReceive:
            TMmsLogger::Log( _L(" - Receive") );
            break;
        case EMmsScheduledSend:
            TMmsLogger::Log( _L(" - Scheduled send") );
            break;
        case EMmsScheduledReceive:
            TMmsLogger::Log( _L(" - Scheduled fetch") );
            break;
        case EMmsDeleteSchedule:
            TMmsLogger::Log( _L(" - Delete schedule") );
            break;
        case EMmsDecodePushedMessage:
            TMmsLogger::Log( _L(" - Decode pushed message") );
            break;
        case EMmsLogDeliveryReport:
            TMmsLogger::Log( _L(" - Log delivery report") );
            break;
        case EMmsDeleteEntries:
            TMmsLogger::Log( _L(" - Delete entries") );
            break;
        case EMmsReceiveForced:
            TMmsLogger::Log( _L(" - Receive forced") );
            break;
        case EMmsScheduledReceiveForced:
            TMmsLogger::Log( _L(" - Scheduled receive forced") );
            break;
        case EMmsGarbageCollection:
            TMmsLogger::Log( _L(" - Garbage collection") );
            break;
        case EMmsDeleteExpiredNotifications:
            TMmsLogger::Log( _L(" - Delete expired notifications") );
            break;
        case EMmsRetryServiceLoading:
            TMmsLogger::Log( _L(" - Retry because service loading failed!") );
            break;
        case EMmsMessageGeneration:
            TMmsLogger::Log( _L(" - Message generation") );
            break;
        case EMmsForward:
            TMmsLogger::Log( _L(" - EMmsForward") );
            break;
        case EMmsScheduledForward:
            TMmsLogger::Log( _L(" - EMmsScheduledForward") );
            break;
        case EMmsNotificationDelete:
            TMmsLogger::Log( _L(" - EMmsNotificationDelete") );
            break;
        case EMmsScheduledNotificationDelete:
            TMmsLogger::Log( _L(" - EMmsScheduledNotificationDelete") );
            break;
        case EMmsUpdateMmboxList:
            TMmsLogger::Log( _L(" - EMmsUpdateMmboxList") );
            break;
        case EMmsSendReadReport:
            TMmsLogger::Log( _L(" - EMmsSendReadReport") );
            break;
        case EMmsScheduledReadReport:
            TMmsLogger::Log( _L(" - EMmsScheduledReadReport") );
            break;
        default:
            TMmsLogger::Log( _L(" - Unknown command") );
            break;
        }
    TMemoryInfoV1Buf memory;
    UserHal::MemoryInfo( memory );
    TInt available = memory().iFreeRamInBytes;
    TMmsLogger::Log(_L("Free memory %d"), available );
#endif // _NO_MMSS_LOGGING_
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::GetRealServiceId( CMsvEntrySelection& aSelection )
    {
    TInt error = KErrNone;
    TMsvId messageEntryId = KMsvNullIndexEntryId;
    // the function cannot be called without a selection so the selection
    // array always has at least one item.
    if ( aSelection.Count() > 0 && aSelection.At( 0 ) != KMsvLocalServiceIndexEntryId )
        {
        messageEntryId = aSelection.At( 0 );
        }
    else if ( aSelection.Count() > 1 )
        {
        messageEntryId = aSelection.At( 1 );
        }
    else
        {
        // We never get here. This is just a safety valve.
        error = KErrNotFound;
        }

    // if only a service entry in selection list,
    // nothing can be done. If we have only service entry,
    // and that is not ours, we won't get here anyway.

    if ( messageEntryId != KMsvNullIndexEntryId )
        {
        error = iServerEntry->SetEntry( messageEntryId );
        }
    if ( error == KErrNone )
        {
        if ( iServerEntry->Entry().iServiceId == KMsvLocalServiceIndexEntryId &&
            iServerEntry->Entry().iRelatedId != KMsvNullIndexEntryId )
            // This is our actual service.
            {
            iServiceEntryId = iServerEntry->Entry().iRelatedId;
            }
        }
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::HandleLoadServiceError( TInt aError )
    {
    if ( iCommand == EMmsDecodePushedMessage )
        {
        // We must delete the dummy entry given to us,
        // otherwise nobody will delete it.

        if ( iMsvSelection->Count() > 0 && iMsvSelection->At( 0 ) == iServiceEntryId )
            {
            iMsvSelection->Delete( 0 ); 
            }
        if ( iMsvSelection->Count() > 0 )
            {
            if ( iServerEntry->SetEntry( iMsvSelection->At( 0 ) ) == KErrNone )
                {
                if ( iServerEntry->SetEntry( iServerEntry->Entry().Parent() ) == KErrNone )
                    {
                    iServerEntry->DeleteEntries( *iMsvSelection );
                    }
                }
            }
        iServerEntry->SetEntry( KMsvNullIndexEntryId );

        // If we are trying to decode a notification from WAP stack
        // return error to caller, not to ourselves
        *iRequestStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, aError );
        return;
        }
        
    if ( aError == KErrNotFound || aError == KErrNoMemory )
        {
        // This is a hopeless case.
        // It could lead to endless loop.
        *iRequestStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, aError );
        return;
        }

    // We only have to help client mtm when it is trying to start automatic
    // fetch after changing fetch mode from "deferred" to "on"
    if ( iCurrentCommand == EMmsScheduledReceive ||
        iCurrentCommand == EMmsScheduledReceiveForced )
        {
        iCurrentCommand = EMmsRetryServiceLoading;
        TRequestStatus* status = &iStatus;
        // caller should be in pending state, too.
        *iRequestStatus = KRequestPending;
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete( status, aError );
        return;
        }
    else
        {
        *iRequestStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, aError );
        return;
        }
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::LogEntryParent()
    {
#ifndef _NO_MMSS_LOGGING_
    TMsvId msgEntryId = KMsvNullIndexEntryId;
    TInt error = KErrNone;
    if ( iMsvSelection->Count() > 0 )
        {
        msgEntryId = iMsvSelection->At( 0 );
        error = iServerEntry->SetEntry( msgEntryId );
        if ( error == KErrNone )
            {
            if ( iServerEntry->Entry().Parent() == KMsvGlobalOutBoxIndexEntryId )
                {
                TMmsLogger::Log( _L("- EntryParent: Outbox") );
                }
            else
                {
                TMmsLogger::Log( _L("- EntryParent: 0x%08X"), iServerEntry->Entry().Parent() );
                }
            }
        }
    // free whatever entry we are holding
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
#endif
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsServerMtm::RestoreVisibilityAndService()
    {
    TInt error = KErrNone;
    TInt i = 0;
    TMsvEntry entry;

    // Make sure that the entries are visible.
    // If they are "in preparation" that's the caller's problem

    for ( i = 0; i < iMsvSelection->Count(); i++ )
        {
        error = iServerEntry->SetEntry( iMsvSelection->At( 0 ) );
        if ( error == KErrNone )
            {
            entry = iServerEntry->Entry();
            if ( entry.Visible() == EFalse && iCurrentCommand != EMmsDeleteSchedule )
                {
                entry.SetVisible( ETrue );
                // if this fails we cannot help...
                iServerEntry->ChangeEntry( entry );
                }
            if ( iCurrentCommand == EMmsDeleteSchedule &&
                entry.iServiceId == KMsvLocalServiceIndexEntryId &&
                entry.iRelatedId != KMsvNullIndexEntryId)
                {
                // restore the correct service id
                entry.iServiceId = entry.iRelatedId;
                entry.iRelatedId = KMsvNullIndexEntryId;
                // if this fails we cannot help...
                iServerEntry->ChangeEntry( entry );
                }
            }
        }

    // free the entry we are holding
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    }
          
// ================= OTHER EXPORTED FUNCTIONS ==============

#ifndef _NO_MMSS_LOGGING_
const TInt KLogBufferLength = 256;
_LIT(KLogDir, "mmss");
_LIT(KLogFile, "mmss.txt");

void TMmsLogger::Log(TRefByValue<const TDesC> aFmt,...)
    {
    VA_LIST list;
    VA_START(list, aFmt);

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList(aFmt, list);

    // Write to log file
    RFileLogger::Write(KLogDir, KLogFile, EFileLoggingModeAppend, buf);
    }
#endif

//
// ---------------------------------------------------------
// gPanic implements
// panic function, should be used by debug version only
//
GLDEF_C void gPanic(
    TMmsPanic aPanic ) // error number enumerations
    {
    _LIT( KMmsPanic,"MMS" );
    User::Panic( KMmsPanic, aPanic );
    }

//  End of File  
