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
* Description: mmsclient implementation
*
*/



// INCLUDE FILES
#include    <txtrich.h>
#include    <msvids.h>
#include    <badesca.h>
#include    <msvstore.h>
#include    <mtmuids.h> 
#include    <mtclbase.h>
#include    <mtmdef.h>
#include    <logcli.h>
#include    <msvftext.h>       // CMsvFindText 
#include    <cmsvmimeheaders.h>
#include    <mmsvattachmentmanager.h>
#include    <mmsvattachmentmanagersync.h>
#include    <charconv.h>
#include    <msgtextutils.h>

// mms headers
#include    "mmsheaders.h"
#include    "mmssettings.h"
#include    "mmsclient.h"
#include    "mmsmessageoperation.h"
#include    "mmsgenutils.h"
#include    "mmscmds.h"
#include    "mmsattachmenthandler.h"
#include    "mmssendingchain.h"
#include    "mmsattachmentwaiter.h"
#include    "mmssendmessageoperation.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
#ifdef _DEBUG
#undef _NO_MMSC_LOGGING_
#endif

_LIT( K1970, "19700000:000000.000000" );    // 1-Jan 1970 0:00:00

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KMmsAttributeArrayGranularity = 8;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================

// Factory function.
EXPORT_C CMmsClientMtm* CMmsClientMtm::NewL(
    CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvSession& aSession )
    {
    CMmsClientMtm* self=new( ELeave ) CMmsClientMtm(
        aRegisteredMtmDll, aSession );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Constructor
// Notice that CMmsNotificationClientMtm constructor calls this.
CMmsClientMtm::CMmsClientMtm(
    CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvSession& aSession )
    : CBaseMtm( aRegisteredMtmDll, aSession ),
    iMmsSettings ( NULL ),
    iMmsHeaders ( NULL ),
    iServiceId ( KMsvNullIndexEntryId ),
    iFetchAll( EFalse ),
    iFetchOverride( ETrue ),
    iOwnSession( aSession ),
    iAttaWaiter( NULL )
    {
    }

    
// Destructor
CMmsClientMtm::~CMmsClientMtm()
    {
    delete iAttaWaiter;
    // We created the settings, it is ours to delete
    delete iMmsSettings;
    delete iMmsHeaders;
    if ( iAttributes != 0 )
        {
        iAttributes->Reset();
        }
    delete iAttributes;
    }

// ---------------------------------------------------------
// CMmsClientMtm::CreateNewEntryL
// ---------------------------------------------------------
//
CMsvOperation* CMmsClientMtm::CreateNewEntryL(
            TMsvId aDestination, 
            TRequestStatus& aCompletionStatus)
    {

    return CMmsMessageOperation::CreateNewL(
            aCompletionStatus,
            Session(),
            aDestination,
            iServiceId );
    }

// DEPRECATED - TO BE REMOVED IN THE FUTURE
// ---------------------------------------------------------
// CMmsClientMtm::CreateServiceL
// ---------------------------------------------------------
//
void CMmsClientMtm::CreateServiceL()
    {
    TMsvId service = KMsvNullIndexEntryId;
    // load settings in order to be sure the old ones are deleted
    iMmsSettings->LoadSettingsL();
    // creates new service entry + new notification and mmbox folder entries
    iMmsSettings->CreateNewServiceL( Session() );
    // save the new entry ids into cenrep immediatedly
    iMmsSettings->SaveSettingsL();
    // Get the base values to detect changes
    // This function should not be idly called, so this should be safe.
    iHomeMode = iMmsSettings->ReceivingModeHome();
    iRoamingMode = iMmsSettings->ReceivingModeForeign();
    iAccessPointCount = iMmsSettings->AccessPointCount();
    // service id must be returned
    service = iMmsSettings->Service();
    
    if ( service == KMsvNullIndexEntryId )
        {
        // No new entry created
        User::Leave( KErrNotSupported );
        }
    SwitchCurrentEntryL( service );
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::MmsSettings()
// ---------------------------------------------------------
//
const CMmsSettings& CMmsClientMtm::MmsSettings()
    {
    return *iMmsSettings;
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::SetSettingsL
// ---------------------------------------------------------
//
void CMmsClientMtm::SetSettingsL( const CMmsSettings& aSettings )
    {
    // copy caller's settings to our member
    iMmsSettings->CopyL( aSettings );
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::StoreSettingsL()
// ---------------------------------------------------------
//
void CMmsClientMtm::StoreSettingsL()
    {
    // Check that sufficient disk space available
    
    if ( iAccessPointCount < iMmsSettings->AccessPointCount() )
        {
        // Disk space checked only if the file size increases.
        // And the file size increases only when access points are added.
        if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
            &Session().FileSession(), 
            KMmsAccessPointDiskSpaceNeed,
            EDriveC ) )
            {
            // we use standard error code here
            User::Leave( KErrDiskFull );
            }
        }
    iMmsSettings->SaveSettingsL();

    // if one of the receiving modes have changed to automatic,
    // start fetching deferred messages 
    TInt homeMode = iMmsSettings->ReceivingModeHome();
    TInt roamMode = iMmsSettings->ReceivingModeForeign();
    
    if ( ( homeMode != iHomeMode && homeMode == EMmsReceivingAutomatic ) ||
        ( roamMode != iRoamingMode && roamMode == EMmsReceivingAutomatic ))
        {
        // fetch all, but only if fetching mode allows it.
        iFetchAll = ETrue;
        iFetchOverride = EFalse;
        }
    else
        {
        iFetchAll = EFalse;
        iFetchOverride = ETrue;
        }

    // We don't leave the settings store function just because the automatic
    // fetch operation leaves. Some entry may be locked and cannot be fetched
    // or some other problem may prevent the fetch.
    // Settings are stored anyway.
    TInt error = KErrNone;
    TRAP( error,
        if ( iFetchAll )
            {
            // Implicit fetch of messages, wrap asynch method to make it synch.
            CMsvOperation* myOperation;
            myOperation = NULL;        
            CMsvOperationActiveSchedulerWait* wait = 
               CMsvOperationActiveSchedulerWait::NewLC();
            // FetchAllL only schedules notifications in mms folder
            myOperation = FetchAllL( wait->iStatus, iFetchOverride );
            // temporary.
            CleanupStack::PushL( myOperation );
            wait->Start();
            CleanupStack::PopAndDestroy( myOperation );
            CleanupStack::PopAndDestroy( wait );
            // Then the notifications in inbox
            wait = CMsvOperationActiveSchedulerWait::NewLC();
            wait->iStatus = KRequestPending;
            // FetchAllFromInboxL only schedules notifications in inbox
            myOperation = FetchAllFromInboxL( wait->iStatus, iFetchOverride );
            CleanupStack::PushL( myOperation );
            wait->Start();

            CleanupStack::PopAndDestroy( myOperation );
            CleanupStack::PopAndDestroy( wait );
            iFetchAll = EFalse; // don't do this next time unless needed...
            }
        );
       
    // Reset the remembered values
    iHomeMode = iMmsSettings->ReceivingModeHome();
    iRoamingMode = iMmsSettings->ReceivingModeForeign();
    iAccessPointCount = iMmsSettings->AccessPointCount();
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::RestoreSettingsL()
// ---------------------------------------------------------
//
void CMmsClientMtm::RestoreSettingsL()
    {
    // The settings may have been changed without us knowing it.
    // We do not reset saved mode values here, because we try to
    // detect if receiving mode has change since we last checked
    // in StoreSettingsL() function.
    iMmsSettings->LoadSettingsL();
    }   


// DEPRECATED - TO BE REMOVED IN THE FUTURE
// ---------------------------------------------------------
// CMmsClientMtm::RestoreFactorySettingsL
// ---------------------------------------------------------
//
void CMmsClientMtm::RestoreFactorySettingsL( TMmsFactorySettingsLevel aLevel )
    {
    iMmsSettings->LoadSettingsL();
    iMmsSettings->RestoreFactorySettingsL( Session(), aLevel );
    // StoreSettingsL triggers the fetching of messages if the receiving mode
    // has changed in a way that requires it.
    StoreSettingsL();
    }


//----------------------------------------------------------
// MMS HEADER HANDLING METHODS
//---------------------------------------------------------- 

// ---------------------------------------------------------
// CMmsClientMtm::SetSenderL
// ---------------------------------------------------------
//
void CMmsClientMtm::SetSenderL( const TDesC& aAlias )
    {
    iMmsHeaders->SetSenderL( aAlias );
    }

// ---------------------------------------------------------
// CMmsClientMtm::Sender
// ---------------------------------------------------------
//
const TPtrC CMmsClientMtm::Sender( ) const
    {
    return iMmsHeaders->Sender();
    }

// ---------------------------------------------------------
// CMmsClientMtm::MessageClass
// ---------------------------------------------------------
//
void CMmsClientMtm::SetMessageClass( TMmsMessageClass aMessageClass )
    {
    iMmsHeaders->SetMessageClass( aMessageClass );
    }

// ---------------------------------------------------------
// CMmsClientMtm::MessageClass
// ---------------------------------------------------------
//
TInt CMmsClientMtm::MessageClass() const
    {
    return iMmsHeaders->MessageClass();
    }

// ---------------------------------------------------------
// CMmsClientMtm::SetMessagePriority
// ---------------------------------------------------------
//
void CMmsClientMtm::SetMessagePriority( TMmsMessagePriority aPriority )
    {
    iMmsHeaders->SetMessagePriority(aPriority);
    }

// ---------------------------------------------------------
// CMmsClientMtm::MessagePriority
// ---------------------------------------------------------
//
TInt CMmsClientMtm::MessagePriority() const
    {
    return iMmsHeaders->MessagePriority();
    }

// ---------------------------------------------------------
// CMmsClientMtm::SetSenderVisibility
// ---------------------------------------------------------
//
void CMmsClientMtm::SetSenderVisibility( TMmsMessageSenderVisibility aVisibility )
    {
    iMmsHeaders->SetSenderVisibility(aVisibility);
    }

// ---------------------------------------------------------
// CMmsClientMtm::SenderVisibility
// ---------------------------------------------------------
//
TInt CMmsClientMtm::SenderVisibility() const
    {
    return iMmsHeaders->SenderVisibility();
    }

// ---------------------------------------------------------
// CMmsClientMtm::SetDeliveryReport
// ---------------------------------------------------------
//
void CMmsClientMtm::SetDeliveryReport( TMmsMessageDeliveryReport aVisibility )
    {
    iMmsHeaders->SetDeliveryReport(aVisibility);
    }

// ---------------------------------------------------------
// CMmsClientMtm::DeliveryReport
// ---------------------------------------------------------
//
TInt CMmsClientMtm::DeliveryReport() const
    {
    return iMmsHeaders->DeliveryReport();
    }

// ---------------------------------------------------------
// CMmsClientMtm::SetReadReply
// ---------------------------------------------------------
//
void CMmsClientMtm::SetReadReply( TMmsMessageReadReply aRequest )
    {
    iMmsHeaders->SetReadReply(aRequest);
    }

// ---------------------------------------------------------
// CMmsClientMtm::ReadReply
// ---------------------------------------------------------
//
TInt CMmsClientMtm::ReadReply() const
    {
    return iMmsHeaders->ReadReply();
    }

// ---------------------------------------------------------
// CMmsClientMtm::SetMessageRootL
// ---------------------------------------------------------
//

void CMmsClientMtm::SetMessageRootL( const TMsvAttachmentId aId )
    {
    // We cannot check if the attachent id is legal because Symbian
    // message server panics if the store is already open.
    // We must just trust the caller.
    iMmsHeaders->SetMessageRoot( aId );
    }

// ---------------------------------------------------------
// CMmsClientMtm::MessageRootAttachment
// ---------------------------------------------------------
//
TMsvAttachmentId CMmsClientMtm::MessageRootAttachment() const
    {
    // This function only returns what has been set earlier by the caller.
    // As the caller can manipulate the attachments without calling
    // MMS Client MTM, we cannot keep track of the presence of the root
    // attachment. It is up to the user to handle the case where the root
    // has been deleted.
    return iMmsHeaders->MessageRoot();
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::SendingDate
// ---------------------------------------------------------
//
TTime CMmsClientMtm::SendingDate() const
    {

    TInt64 inSeconds = iMmsHeaders->Date();

    TTime y1970( K1970 );

    // 1970 presented as microseconds after January 1st, 0 AD Gregorian.
    TInt64 ms1970 = y1970.Int64();

    // If not defined in message headers return 0
    if ( inSeconds == 0 )
        {
        return TTime( 0 );
        }

    return TTime( ms1970 + ( inSeconds * KMmsMillion ) );

    }

// ---------------------------------------------------------
// CMmsClientMtm::SetMaximumImage
// ---------------------------------------------------------
//
void CMmsClientMtm::SetMaximumImage( TInt aHigh, TInt aWidth )
    {
    iMmsHeaders->SetMaximumImage( aHigh, aWidth );
    }

// ---------------------------------------------------------
// CMmsClientMtm::GetMaximumImage
// ---------------------------------------------------------
//
void CMmsClientMtm::GetMaximumImage( TInt& aHeight, TInt& aWidth ) const
    {
    iMmsHeaders->GetMaximumImage( aHeight, aWidth );
    }

// ---------------------------------------------------------
// CMmsClientMtm::SetExpiryInterval
// ---------------------------------------------------------
//       
void CMmsClientMtm::SetExpiryInterval( 
    TTimeIntervalSeconds aInterval )
    {
    iMmsHeaders->SetExpiryInterval(aInterval.Int());
    }

// ---------------------------------------------------------
// CMmsClientMtm::ExpiryInterval
// ---------------------------------------------------------
//
TTimeIntervalSeconds CMmsClientMtm::ExpiryInterval() const
    {
    TTimeIntervalSeconds seconds(iMmsHeaders->ExpiryInterval());

    return seconds;
    }

// ---------------------------------------------------------
// CMmsClientMtm::SetExpiryDate
// ---------------------------------------------------------
//
void CMmsClientMtm::SetExpiryDate( TTime aDate )
    {

    TTime y1970( K1970 );
    TTimeIntervalMicroSeconds interval;
    // we can't use "seconds from" as it only returns a
    // 32 bit signed integer. If fails in 2038.
    // "microseconds from" returns a 64 bit signed integer
    interval = aDate.MicroSecondsFrom( y1970 );
    if ( interval.Int64() > 0 )
        {
        // expiry date in iMmsHeaders() in seconds from 1.1.1970.
        iMmsHeaders->SetExpiryDate( ( interval.Int64() ) / KMmsMillion );
        }
    }

// ---------------------------------------------------------
// CMmsClientMtm::ExpiryDate
// ---------------------------------------------------------
//
TTime CMmsClientMtm::ExpiryDate() const
    {

    TTime y1970( K1970 );

    // 1970 presented as microseconds after January 1st, 0 AD Gregorian.
    TInt64 ms1970 = y1970.Int64();

    // Expiry in Seconds after 1.1. 1970
    TInt64 sAfter1970 = iMmsHeaders->ExpiryDate();

    // If not defined in message headers return 0
    if ( sAfter1970 == 0 )
        {
        return TTime(0);
        }

    // Expiry in microseconds after 1.1. 1970
    TInt64 msAfter1970;
    msAfter1970 = sAfter1970;
    msAfter1970 *= KMmsMillion;

    return TTime( ms1970 + msAfter1970 );

    }

// ---------------------------------------------------------
// CMmsClientMtm::SetDeliveryTimeInterval
// ---------------------------------------------------------
//       
void CMmsClientMtm::SetDeliveryTimeInterval( 
    TTimeIntervalSeconds aInterval )
    {
    iMmsHeaders->SetDeliveryTimeInterval( aInterval.Int() );
    }

// ---------------------------------------------------------
// CMmsClientMtm::DeliveryTimeInterval
// ---------------------------------------------------------
//
TTimeIntervalSeconds CMmsClientMtm::DeliveryTimeInterval() const
    {
    TTimeIntervalSeconds seconds( iMmsHeaders->DeliveryTimeInterval() );
    return seconds;
    }

// ---------------------------------------------------------
// CMmsClientMtm::SetDeliveryDate
// ---------------------------------------------------------
//
void CMmsClientMtm::SetDeliveryDate( TTime aDate )
    {

    TTime y1970( K1970 );
    TTimeIntervalMicroSeconds interval;
    // we can't use "seconds from" as it only returns a
    // 32 bit signed integer. If fails in 2038.
    // "microseconds from" returns a 64 bit signed integer
    interval = aDate.MicroSecondsFrom( y1970 );
    if (interval.Int64() > 0 )
        {
        // Delivery date in iMmsHeaders() in seconds from 1.1.1970.
        iMmsHeaders->SetDeliveryDate( (interval.Int64() ) / KMmsMillion );
        }
    }

// ---------------------------------------------------------
// CMmsClientMtm::DeliveryDate
// ---------------------------------------------------------
//
TTime CMmsClientMtm::DeliveryDate() const
    {

    TTime y1970( K1970 );

    // 1970 presented as microseconds after January 1st, 0 AD Gregorian.
    TInt64 ms1970 = y1970.Int64();

    // Expiry in Seconds after 1.1. 1970
    TInt64 sAfter1970 = iMmsHeaders->DeliveryDate();

    // If not defined in message headers return 0
    if ( sAfter1970 == 0 )
        {
        return TTime(0);
        }

    // Expiry in microseconds after 1.1. 1970 
    TInt64 msAfter1970;
    msAfter1970 = sAfter1970;
    msAfter1970 *= KMmsMillion;

    return TTime( ms1970 + msAfter1970 );

    }

// ---------------------------------------------------------
// CMmsClientMtm::SendL
// ---------------------------------------------------------
//
CMsvOperation* CMmsClientMtm::SendL( TRequestStatus& aCompletionStatus,
    const TTime aSendingTime /* = TTime( 0 ) */  )
    {
    
    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );       // ***

    // Current entry will be the one to send
    selection->AppendL( iMsvEntry->Entry().Id() );
    
    // aSendingTime is passed on "as is".
    // SendL with selection makes the conversion if needed
    CMsvOperation* op = SendL( *selection, aCompletionStatus, aSendingTime );

    CleanupStack::PopAndDestroy( selection );
    
    return op;

    }

// ---------------------------------------------------------
// CMmsClientMtm::SendL
// ---------------------------------------------------------
//
CMsvOperation* CMmsClientMtm::SendL(
            CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus,
            TTime aSendingTime )
    {

    if ( aSelection.Count() == 0 )
        {
        User::Leave( KErrNotFound );
        }
    
    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );       // ***
    
    // Move the messages to OUTBOX synchronously
    // Parent should be the parent of the first entry in the selection
    CMsvEntry* cEntry = NULL;
    TMsvId currentParent = iMsvEntry->Entry().Parent();
    cEntry = Session().GetEntryL( aSelection.At( 0 ) );
    CleanupStack::PushL( cEntry );
    currentParent = cEntry->Entry().Parent();
    cEntry->SetEntryL( currentParent );
    
#ifndef _NO_MMSC_LOGGING_
    _LIT( KMmsBefore, "MMSC: Entry parent before move 0x%08X");
    TMmsGenUtils::Log( KMmsBefore, currentParent );
#endif
    if ( currentParent != KMsvGlobalOutBoxIndexEntryId )
        {
        TMsvLocalOperationProgress progress;
        cEntry->MoveL( aSelection, KMsvGlobalOutBoxIndexEntryId, progress );
        }

#ifndef _NO_MMSC_LOGGING_
    cEntry->SetEntryL( aSelection.At( 0 ) );
    currentParent = cEntry->Entry().Parent();
    _LIT( KMmsAfter, "MMSC: Entry parent after move 0x%08X");
    TMmsGenUtils::Log( KMmsAfter, currentParent );
#endif
    CleanupStack::PopAndDestroy( cEntry );
    
    selection->AppendL( aSelection.Back( 0 ), aSelection.Count() );

    // Call Server MTM
    TCommandParameters parameters; // initialized to zero
    TTime now;
    // All times in message entries are now UTC time
    now.UniversalTime();
    TInt error = KErrNone;
    if ( aSendingTime > now )
        {
        error = aSendingTime.SecondsFrom( now, parameters.iInitialDelay );
        }
    if ( error != KErrNone )
        {
        User::Leave( error );
        }

    TCommandParametersBuf paramPack( parameters );

    CMsvOperation* op = InvokeAsyncFunctionL(
        EMmsScheduledSend,
        *selection,
        paramPack,
        aCompletionStatus );
 
    CleanupStack::PopAndDestroy( selection );
    
    return op;

    }


// ---------------------------------------------------------
// CMmsClientMtm::FetchAllL
// ---------------------------------------------------------
//
CMsvOperation* CMmsClientMtm::FetchAllL( TRequestStatus& aCompletionStatus,
    TBool aForced )
    {
    
    iFetchOverride = aForced;
    
    CMsvEntrySelection* selection = ListMmsFolderNotificationsL();
    CleanupStack::PushL( selection );

    if ( selection->Count() == 0 )
        {
        CleanupStack::PopAndDestroy( selection );
        TPckgC < TMsvId > progress = 0;
        aCompletionStatus = KRequestPending;
        return  CMsvCompletedOperation::NewL( Session(), Type(), progress,
            KMsvLocalServiceIndexEntryId, aCompletionStatus );
        }

    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack( parameters );

    CMsvOperation* op = NULL;
    if ( iFetchOverride )
        {
        op = InvokeAsyncFunctionL(
            EMmsScheduledReceiveForced,
            *selection,
            paramPack,
            aCompletionStatus );
        }
    else
        {
        op = InvokeAsyncFunctionL(
            EMmsScheduledReceive,
            *selection,
            paramPack,
            aCompletionStatus );
        // reset override to default value
        iFetchOverride = ETrue;
        }

    CleanupStack::PopAndDestroy( selection );
    return op;
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::SendReadReportL
// ---------------------------------------------------------
//
CMsvOperation* CMmsClientMtm::SendReadReportL( TMsvId aReadMessageId,
    TRequestStatus& aCompletionStatus,
    TMmsReadStatus aReadStatus /*= EMmsReadStatusRead*/ )
    {
    // Get the entry for which the report will be sent
    
    CMmsHeaders* reportHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    CleanupStack::PushL( reportHeaders );
    CMmsHeaders* originalHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    CleanupStack::PushL( originalHeaders );
    
    CMsvEntry* cEntry = NULL;
    CMsvStore* store = NULL;
    // Restore original headers.
    // If we are sending the report to current context, use our own entry
    if ( aReadMessageId == iMsvEntry->Entry().Id() &&
        aReadMessageId != KMsvNullIndexEntryId )
        {
        store = iMsvEntry->ReadStoreL();
        CleanupStack::PushL( store );
        originalHeaders->RestoreL( *store );
        CleanupStack::PopAndDestroy( store );
        store = NULL;
        }
    else
        {
        // If we cannot get the entry, the function leaves
        cEntry = Session().GetEntryL( aReadMessageId );
        CleanupStack::PushL( cEntry );
        store = cEntry->ReadStoreL();
        CleanupStack::PushL( store );
        originalHeaders->RestoreL( *store );
        CleanupStack::PopAndDestroy( store );
        store = NULL;
        CleanupStack::PopAndDestroy( cEntry );
        cEntry = NULL;
        }
    
    if ( !iMmsSettings->ReadReplyReportSendingAllowed() ||
        originalHeaders->ReadReply() != EMmsYes ||
        originalHeaders->Sender().Length() == 0 ||
        iMmsHeaders->MessageClass() == EMmsClassAuto )
        {
        // We are not allowed to send a read reply, or the sender of the
        // original message has not requested a read reply - do not send one.
        // We are also not allowed to send a read report to automatic messages.
        CleanupStack::PopAndDestroy( originalHeaders );
        originalHeaders = NULL;
        CleanupStack::PopAndDestroy( reportHeaders );
        reportHeaders = NULL;
        
        TPckgC < TMsvId > progress = 0;
        return  CMsvCompletedOperation::NewL( Session(), Type(), progress,
            KMsvLocalServiceIndexEntryId, aCompletionStatus, KErrGeneral );
        }
    
    // Now we have loaded the original headers and decided that a read
    // report really must be sent.
    // We create a new message entry in the MMS Notification folder to keep
    // it invisible.
    
    reportHeaders->SetMessageIdL( originalHeaders->MessageId() );
    reportHeaders->SetMessageType( KMmsMessageTypeReadRecInd );
    reportHeaders->AddTypedAddresseeL( TMmsGenUtils::PureAddress( originalHeaders->Sender() ),
        EMsvRecipientTo );
    // sender must be insert-address-token because we don't know our own number
    // And if there are many recipients we cannot be sure which one is our own number
    
    // We are through with the original headers
    CleanupStack::PopAndDestroy( originalHeaders );
    originalHeaders = NULL; 
    
    TTime now;
    now.UniversalTime();
    
    TTime y1970( K1970 );
    TTimeIntervalMicroSeconds interval;
    // we can't use "seconds from" as it only returns a
    // 32 bit signed integer. If fails in 2038.
    // "microseconds from" returns a 64 bit signed integer
    interval = now.MicroSecondsFrom( y1970 );
    // date in iMmsHeaders() in seconds from 1.1.1970.
    reportHeaders->SetDate( ( interval.Int64() ) / KMmsMillion );
    reportHeaders->SetReadStatus( aReadStatus );

    TMsvId mmsFolderId = iMmsSettings->NotificationFolder();
    cEntry = Session().GetEntryL( mmsFolderId );
    CleanupStack::PushL( cEntry );

    // We create the entry immediately as complete because we store the data immediately
    // If the battery runs out at the wrong moment, garbage collection will throw the
    // extra entry away as it is marked as read rec ind.
    TMsvEntry entry;
    entry.iType = KUidMsvMessageEntry;
    entry.iMtm = KUidMsgTypeMultimedia;
    entry.SetVisible( ETrue );
    entry.SetInPreparation( EFalse );
    // We set the service as local service to get past the line
    // in case sending or receiving is ongoing
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    entry.iRelatedId = iServiceId;
    entry.iMtmData1 = KMmsMessageReadRecInd;
    cEntry->CreateL( entry );
    
    TMsvId entryId = entry.Id();
    
    cEntry->SetEntryL( entryId ); 

    store = cEntry->EditStoreL();
    CleanupStack::PushL( store );
    reportHeaders->StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    store = NULL;

    // These can go now. Our entry is ready to be sent
    CleanupStack::PopAndDestroy( cEntry );
    cEntry = NULL;
    CleanupStack::PopAndDestroy( reportHeaders );
    reportHeaders = NULL;
    
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->InsertL( 0, entryId );

    CMsvOperation * op = NULL;
    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack( parameters );
    
    op = Session().TransferCommandL( *selection, 
        EMmsScheduledReadReport,
        paramPack,
        aCompletionStatus );
    
    CleanupStack::PopAndDestroy( selection );

    return op;
    }

// ---------------------------------------------------------
// CMmsClientMtm::ResponseText
// ---------------------------------------------------------
//
TPtrC CMmsClientMtm::ResponseText() const
    {
    return iMmsHeaders->ResponseText( );
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::ResponseStatus
// ---------------------------------------------------------
//
TInt CMmsClientMtm::ResponseStatus() const
    {
    return iMmsHeaders->ResponseStatus();
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::NumberOfPreviousSenders
// ---------------------------------------------------------
//
TInt CMmsClientMtm::NumberOfPreviousSenders() const
    {
    return iMmsHeaders->PreviouslySentList().Count();
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::
// ---------------------------------------------------------
//
TPtrC CMmsClientMtm::PreviousSender( TInt aSequenceNumber ) const
    {
    if ( aSequenceNumber > iMmsHeaders->PreviouslySentList().Count() ||
        aSequenceNumber < 1 )
        {
        return TPtrC();
        }
    return iMmsHeaders->PreviouslySentList()[aSequenceNumber - 1]->Sender();
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::
// ---------------------------------------------------------
//
TTime CMmsClientMtm::PreviousSendingDate( TInt aSequenceNumber ) const
    {
    if ( aSequenceNumber > iMmsHeaders->PreviouslySentList().Count() ||
        aSequenceNumber < 1 )
        {
        return TTime( 0 );
        }
        
    TInt64 inSeconds;
    inSeconds = iMmsHeaders->PreviouslySentList()[aSequenceNumber - 1]->Date();

    // If not defined in message headers return 0
    if ( inSeconds == 0 )
        {
        return TTime(0);
        }

    TTime y1970( K1970 );
    // 1970 presented as microseconds after January 1st, 0 AD Gregorian.
    TInt64 ms1970 = y1970.Int64();

    return TTime( ms1970 + ( KMmsMillion * inSeconds ) );
    
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::MessageReceiveTime
// ---------------------------------------------------------
//
TTime CMmsClientMtm::MessageReceiveTime() const
    {
    return iMmsHeaders->ReceivingTime();
    }

// ---------------------------------------------------------
// CMmsClientMtm::MessageTransferSize
// ---------------------------------------------------------
//
TInt CMmsClientMtm::MessageTransferSize() const
    {
    return iMmsHeaders->MessageSize();
    }

// ---------------------------------------------------------
// CMmsClientMtm::MessageContentLocation
// ---------------------------------------------------------
//
TPtrC8 CMmsClientMtm::MessageContentLocation() const
    {
    return iMmsHeaders->ContentLocation();
    }   

//----------------------------------------------------------
// METHODS FROM BASE CLASS
//----------------------------------------------------------

// ---------------------------------------------------------
// CMmsClientMtm::SaveMessageL
// Stores the multimedia message
// ---------------------------------------------------------
//
void CMmsClientMtm::SaveMessageL()
    {
    // First we should assert that iMsvEntry is not NULL, and panic, if it is
    __ASSERT_DEBUG( iMsvEntry, gPanic( EMmsNoCMsvEntrySet ));
    // SaveMessageL should only be supported for message entries.
    __ASSERT_DEBUG( iMsvEntry->Entry().iType.iUid == KUidMsvMessageEntryValue,
        gPanic( EMmsNotAMessageEntry ) );

    TMsvEntry indexEntry = iMsvEntry->Entry();

    // Store headers of a multimedia message

    // Because of the way the attachments are handled using the new
    // attacment manager, the caller must store and commit the attachments
    // either one by one or after all have been added.
    // After saving all attachments the edit store used for that purpose
    // must be freed.
    // The store must be freed because all attachment info and MMS headers
    // are saved in the actual message entry, there are no separate 
    // attachment entries anymore.

    // When a message is saved in client, the possible transaction ID is cleared
    // because when the message is changed, it must get a new TID in MMS server 
    // when it is sent.
    iMmsHeaders->SetTidL( TPtrC8() );

    CMsvStore* store = iMsvEntry->EditStoreL();
    CleanupStack::PushL( store );
    // Check that sufficient disk space available
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &Session().FileSession(), 
        iMmsHeaders->Size(),
        iMessageDrive ) )
        {
        User::Leave( KErrDiskFull );
        }
        
    // Note: Body text not supported.
    
    iMmsHeaders->StoreL( *store );
    
    StoreAttributesL( *store );
    
    // Commit the stream store
    store->CommitL();
    CleanupStack::PopAndDestroy( store );

    // Start to update index entry 
    // If the caller has not set the description, we set it to messge subject -
    // if subject is available.
    if ( indexEntry.iDescription.Length() == 0 )
        {
        indexEntry.iDescription.Set( iMmsHeaders->Subject() );
        }
    // The caller must set the message to complete and visible when it is ready:
    /*
       indexEntry.SetInPreparation( EFalse );
       indexEntry.SetVisible( ETrue );
       indexEntry.iDate.UniversalTime();
    */

    // attachment size
    TInt32 totalSizeOfAllAttachments = AttachmentsSizeL();
    indexEntry.iSize = iMmsHeaders->Size() + totalSizeOfAllAttachments;

    // If there are multiple recipients then set the flag
    if (( iMmsHeaders->ToRecipients().Count() +
        iMmsHeaders->CcRecipients().Count() +
        iMmsHeaders->BccRecipients().Count() ) > 1 )
        {
        indexEntry.SetMultipleRecipients( ETrue );
        }
    else
        {
        // clear multiple recipients in case recipients have
        // been deleted after the message was saved the last time
        indexEntry.SetMultipleRecipients( EFalse );
        }

    // Set iDetails (recipient)
    // Check that MT message's details not updated,
    // although this should not be possible in UI.
    if ( !( indexEntry.iMtmData1 & KMmsMessageMobileTerminated ) )
        {
        TPtrC to;
        if ( iMmsHeaders->ToRecipients().Count() )
            {
            to.Set( TMmsGenUtils::Alias( iMmsHeaders->ToRecipients()[0] ) );
            if ( to.Length() <= 0 )
                {
                // If no alias part then set the real address in details
                to.Set( iMmsHeaders->ToRecipients()[0] );
                }
            }
        indexEntry.iDetails.Set( to );
        }

    if ( totalSizeOfAllAttachments > 0 )
        {
        indexEntry.SetAttachment( ETrue );
        }
        
    if ( iAttributes->MdcaCount() > 0 )
        {
        indexEntry.iMtmData1 |= KMmsAttributeStreamPresent;
        }
    else
        {
        indexEntry.iMtmData1 &= ~KMmsAttributeStreamPresent;
        }
        
    switch ( iMmsHeaders->MessagePriority() )
        {
        case KMmsPriorityNormal:
            indexEntry.SetPriority( EMsvMediumPriority );
            break;
        case KMmsPriorityLow:
            indexEntry.SetPriority( EMsvLowPriority );
            break;
        case KMmsPriorityHigh:
            indexEntry.SetPriority( EMsvHighPriority );
            break;
        default:            
            // if not defined default is normal
            indexEntry.SetPriority( EMsvMediumPriority );
            break;
        }

    // commit the index changes.
    iMsvEntry->ChangeL( indexEntry );
    }

// ---------------------------------------------------------
// CMmsClientMtm::LoadMessageL
// Loads the multimedia message
// ---------------------------------------------------------
//
void CMmsClientMtm::LoadMessageL()
    {
    // First we should assert that iMsvEntry is not NULL, and panic, if it is
    __ASSERT_DEBUG( iMsvEntry, gPanic( EMmsNoCMsvEntrySet ) );
    // LoadMessageL should only be supported for message entries.
    if ( iMsvEntry->Entry().iType.iUid != KUidMsvMessageEntryValue )
        {
        iAttributes->Reset();
        iMmsHeaders->Reset();
        iAddresseeList->Reset();
        return;        
        }
    
    // Old data must be reset first....
    iAttributes->Reset();

    // load the correct data
    // get read-only message store
    CMsvStore* store = iMsvEntry->ReadStoreL();
    CleanupStack::PushL( store );

    // restore headers of multimedia message
    // Attachment info is not restored.
    // It makes no sense to cache the attachment info as new attachments
    // can be added with the help of the attachment magager without 
    // informing MMS Client MTM of the additions.
    // Caller must use attachment manager to get attachment info.
    iMmsHeaders->RestoreL( *store );
    
    RestoreAttributesL( *store );
    
    CleanupStack::PopAndDestroy( store );
    store = NULL;

    // Build the iAddresseeList up
    BuildAddresseeListL();

    }

// ---------------------------------------------------------
// CMmsClientmtm::ReplyL
// Send a reply to current message
// ---------------------------------------------------------
//
CMsvOperation* CMmsClientMtm::ReplyL(
    TMsvId aDestination,
    TMsvPartList aPartList,
    TRequestStatus& aCompletionStatus )
    {

    return CMmsMessageOperation::CreateReplyL(
            aCompletionStatus,
            Session(),
            iMsvEntry->EntryId(),
            aDestination,
            aPartList,
            iServiceId );
    }

// ---------------------------------------------------------
// CMmsClientMtm::ForwardL
// Forward current message
// ---------------------------------------------------------
//
CMsvOperation* CMmsClientMtm::ForwardL(
    TMsvId aDestination,
    TMsvPartList aPartList,
    TRequestStatus& aCompletionStatus )
    {

    return CMmsMessageOperation::CreateForwardL(
            aCompletionStatus,
            Session(),
            iMsvEntry->EntryId(),
            aDestination,
            aPartList,
            iServiceId );
    }

// ---------------------------------------------------------
// CMmsClientMtm::ValidateMessage
// Validate selected parts of current message
// ---------------------------------------------------------
//
TMsvPartList CMmsClientMtm::ValidateMessage(
    TMsvPartList aPartList )
    {
    __ASSERT_DEBUG( iMsvEntry, gPanic( EMmsNoCMsvEntrySet ));
    
    TMsvPartList retVal = 0;
    if ( iMsvEntry->Entry().iType.iUid != KUidMsvMessageEntryValue )
        {
        // not a message, no part is valid
        retVal = aPartList;
        }

    if ( aPartList & KMsvMessagePartRecipient )
        {
        if ( iAddresseeList->Count() == 0)
            {
            retVal |= KMsvMessagePartRecipient;
            }
        else
            {
            // check the recipient list for valid 'addresses'
            for (TInt ii=0; ii < iAddresseeList->Count(); ++ii)
                {
                TPtrC oneAddress = (*iAddresseeList)[ii];
                TPtrC pureAddress = TMmsGenUtils::PureAddress( oneAddress );
                if ( ( pureAddress.Length() == 0 ) || 
                    !TMmsGenUtils::IsValidAddress( pureAddress, ETrue ) )
                    {
                    retVal |= KMsvMessagePartRecipient;
                    break;
                    }
                }
            }
        }

    // all attachments are considered valid - even no attachments

    return retVal;

    }

// ---------------------------------------------------------
// CMmsClientMtm::Find
// Find text in selected message parts
// ---------------------------------------------------------
//
TMsvPartList CMmsClientMtm::Find(
    const TDesC& aTextToFind,
    TMsvPartList aPartList )
    {
    // The final version will not have a rich text body, but we could
    // search for example the originator and description.

    __ASSERT_DEBUG( iMsvEntry, gPanic( EMmsNoCMsvEntrySet ) );
    TMsvPartList foundList = KMsvMessagePartNone;
    
    TRAP_IGNORE(
        {
        TMsvEntry entry = iMsvEntry->Entry();

        CMsvFindText* findText = CMsvFindText::NewL();
        
        CleanupStack::PushL( findText );

        if ( aPartList & KMsvMessagePartRecipient )
            {
            // Find from To, Cc and Bcc fields
            if ( FindInRecipientL( aTextToFind, 
                aPartList, iMmsHeaders->ToRecipients(), *findText))
                {
                foundList |= KMsvMessagePartRecipient;
                }
            else if ( FindInRecipientL( aTextToFind, 
                aPartList, iMmsHeaders->CcRecipients(), *findText ))
                {
                foundList |= KMsvMessagePartRecipient;
                }
            else if ( FindInRecipientL( aTextToFind, 
                aPartList, iMmsHeaders->BccRecipients(), *findText ))
                {
                foundList |= KMsvMessagePartRecipient;
                }
            else
                {
                // keep LINT happy
                }
            }

        if ( aPartList & KMsvMessagePartOriginator )
            {
            if ( findText->FindTextL( aTextToFind, iMmsHeaders->Sender(), 
                aPartList ) )
                {
                foundList |= KMsvMessagePartOriginator;
                }
            }

        if ( aPartList & KMsvMessagePartDescription )
            {
            if ( findText->FindTextL( aTextToFind, entry.iDescription, 
                aPartList ) )
                {
                foundList |= KMsvMessagePartDescription;
                }
            }

        CleanupStack::PopAndDestroy( findText );
        });
    return foundList;

    }

// ---------------------------------------------------------
// CMmsClientMtm::AddAddresseeL
// ---------------------------------------------------------
//
void CMmsClientMtm::AddAddresseeL( const TDesC& aRealAddress )
    {
    // Add to general list
    // When no type is specified, the address will have type "to"
    iAddresseeList->AppendL( EMsvRecipientTo, aRealAddress );
    
    // Add to "To" recipient list
    iMmsHeaders->AddTypedAddresseeL( aRealAddress, EMsvRecipientTo );

    }

// ---------------------------------------------------------
// CMmsClientMtm::AddAddresseeL
// ---------------------------------------------------------
//
void CMmsClientMtm::AddAddresseeL(
    const TDesC& aRealAddress,
    const TDesC& aAlias )
    {

    if ( aAlias.Length() > 0 )
        {
        HBufC* buf = TMmsGenUtils::GenerateAddressL( aRealAddress, aAlias );
        CleanupStack::PushL( buf );
        AddAddresseeL( buf->Des() );
        CleanupStack::PopAndDestroy( buf );
        }
    else
        {        
        AddAddresseeL( aRealAddress );
        }
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::AddAddresseeL
// ---------------------------------------------------------
//
void CMmsClientMtm::AddAddresseeL( TMsvRecipientType aType,
    const TDesC& aRealAddress )
    {
    // Add to general list
    // When no type is specified, the address will have type "to"
    iAddresseeList->AppendL( aType, aRealAddress );
    
    // Add to recipient list
    iMmsHeaders->AddTypedAddresseeL( aRealAddress, aType );

    }

// ---------------------------------------------------------
// CMmsClientMtm::AddAddresseeL
// ---------------------------------------------------------
//
void CMmsClientMtm::AddAddresseeL(
    TMsvRecipientType aType,
    const TDesC& aRealAddress,
    const TDesC& aAlias )
    {

    if ( aAlias.Length() > 0 )
        {
        HBufC* buf = TMmsGenUtils::GenerateAddressL( aRealAddress, aAlias );
        CleanupStack::PushL( buf );
        AddAddresseeL(aType, buf->Des());
        CleanupStack::PopAndDestroy( buf );
        }
    else
        {        
        AddAddresseeL( aType, aRealAddress );
        }
    }

// ---------------------------------------------------------
// CMmsClientMtm::RemoveAddressee
// ---------------------------------------------------------
//
void CMmsClientMtm::RemoveAddressee( TInt aIndex )
    {

    if ( iAddresseeList->Count() > aIndex )
        {
        // Delete from typed list
        TPtrC address = (*iAddresseeList)[ aIndex ];
        iMmsHeaders->RemoveAddressee( address );
        // delete from untyped list
        iAddresseeList->Delete( aIndex );
        }
        
    }

// ---------------------------------------------------------
// CMmsClientMtm::SetSubjectL
// ---------------------------------------------------------
//
void CMmsClientMtm::SetSubjectL( const TDesC& aSubject )
    {
    iMmsHeaders->SetSubjectL( aSubject );
    }

// ---------------------------------------------------------
// CMmsClientMtm::SubjectL
// ---------------------------------------------------------
//
const TPtrC CMmsClientMtm::SubjectL() const
    {
    return iMmsHeaders->Subject();
    }

// ---------------------------------------------------------
// CMmsClientMtm::AddAttachmentL
// ---------------------------------------------------------
//
void CMmsClientMtm::AddAttachmentL( const TDesC& aFilePath,
    const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus )
    {
    
    TInt error = KErrNone;
    TUint charset = aCharset;
    RFile file;
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0 && charset == 0 )
        {
        // try to recognize character set
        // We trap the recognization process
        // If cannot recognize, the result will be 0 and default to us-ascii
        TRAP ( error,
            {
            error = file.Open( Session().FileSession(), aFilePath,
                EFileRead|EFileShareReadersOnly );
            if ( error == KErrNone )
                {
                CleanupClosePushL( file );
                charset = RecognizeCharSetL( file );
                CleanupStack::PopAndDestroy( &file ); // close file
                }
            }
            );
        }
        
    // if the attachment character set is unicode, it should be converted to utf-8
    // (see MMS conformance document)
    
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0 &&
        ( charset == KMmsUTF16  ||
        charset == KMmsIso10646Ucs2 ||
        charset == KMmsUTF16BE ||
        charset == KMmsUTF16LE ) )
        {
        // If we have unicode character set, we must convert the file to utf8
        error = file.Open( Session().FileSession(), aFilePath,
            EFileRead|EFileShareReadersOnly );
        if ( error == KErrNone )
            {
            CleanupClosePushL( file );
              CMsvStore* store = iMsvEntry->EditStoreL();
            CleanupStack::PushL( store );
            TMsvAttachmentId attaId = 0;
            CMmsAttachmentHandler::CreateUTF8TextAttachmentFromFileL(
                *store, attaId, file, Session().FileSession(),
                Session().CurrentDriveL() );
            store->CommitL();
            CleanupStack::PopAndDestroy( store );
            CleanupStack::PopAndDestroy( &file ); // close file
            }
        TRequestStatus* status = &aStatus;
        aStatus = KRequestPending;
        User::RequestComplete( status, error );
        return;    
        }
    else
        {
        // Disk space is checked in AddFilePathAttachmentL after everything has been initialized
        AddFilePathAttachmentL( aFilePath, aMimeType, CMsvAttachment::EMsvFile, aStatus, charset );
        }
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::AddAttachmentL
// ---------------------------------------------------------
//
void CMmsClientMtm::AddAttachmentL( RFile& aFile, const TDesC8& aMimeType,
    TUint aCharset, TRequestStatus& aStatus )
    {

    TInt charset = aCharset;
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0  && charset == 0 )
        {
        // If no character set defined for a plain text attachment
        // we try to recognize the character set.
        // But if recoginzation fails, we say 0 (us-ascii)
        TRAP_IGNORE ( 
            {
            charset = RecognizeCharSetL( aFile );
            });
        }
        
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0 )
        {
        // If we have unicode character set, we must convert the file to utf8
        if ( ( charset == KMmsUTF16 ) ||
            ( charset == KMmsIso10646Ucs2 ) ||
            ( charset == KMmsUTF16BE ) ||
            ( charset == KMmsUTF16LE ) )
            {
              CMsvStore* store = iMsvEntry->EditStoreL();
            CleanupStack::PushL( store );
            TMsvAttachmentId attaId = 0;
            CMmsAttachmentHandler::CreateUTF8TextAttachmentFromFileL( *store,
                attaId, aFile, Session().FileSession(),
                Session().CurrentDriveL() );
            store->CommitL();
            CleanupStack::PopAndDestroy( store ); // store
            // We must close the file handle because the attachment manager will also
            // close the handle.
            // The open file handle is always closed unless the function leaves
            aFile.Close();
            TRequestStatus* status = &aStatus;
            aStatus = KRequestPending;
            User::RequestComplete( status, KErrNone );
            return;
            }
        }
    
    if( !iAttaWaiter )
        {
        iAttaWaiter = CMmsAttachmentWaiter::NewL();
        }

    // store must be the first item allocated because it is the last one to be popped
    CMsvStore* store = iMsvEntry->EditStoreL();
    CleanupStack::PushL( store );
    
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );
    
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mimeHeaders );
    
    // set the size
    TInt size = 0;
    User::LeaveIfError( aFile.Size( size ) );
    attachment->SetSize( size );
    
    // set the mime-type if provided
    if( aMimeType.Length() > 0 )
        {
        attachment->SetMimeTypeL( aMimeType );
        TInt position = aMimeType.Find( KMmsSlash8 );
        if ( position > 0 )
            {
            mimeHeaders->SetContentTypeL( aMimeType.Left( position ) );
            }
        if ( position < aMimeType.Length() - 1 )
            {
            mimeHeaders->SetContentSubTypeL( aMimeType.Mid( position + 1 ) );
            }
        }
        
    TFileName fileName;
    User::LeaveIfError( aFile.Name( fileName ) );
    attachment->SetAttachmentNameL( fileName );

    mimeHeaders->SetSuggestedFilenameL( attachment->AttachmentName() );
    mimeHeaders->SetMimeCharset( charset );

    size += KMmsIndexEntryExtra + mimeHeaders->Size();    
    mimeHeaders->StoreL( *attachment );
    
    // mime headers have been streamed to CMsvAttachment, they can go now
    CleanupStack::PopAndDestroy( mimeHeaders );
    
    // Check that sufficient disk space available
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &Session().FileSession(), 
        size,
        iMessageDrive ) )
            {
            CleanupStack::PopAndDestroy( attachment );
            User::Leave( KErrDiskFull );
            }

    // attachment is initialised, pass to the attachment manager
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    if ( iAttaWaiter->IsActive() )
        {
        // can't start an active operation because already active
        User::Leave(KErrInUse);
        }
    manager.AddAttachmentL( aFile, attachment, iAttaWaiter->iStatus ); 
    CleanupStack::Pop( attachment ); // ownership passed to manager
    // We cannot start waiting before we know that the function we are waiting for
    // did not leave. If we become active, and the function leaves, we are in trouble    
    iAttaWaiter->StartWaitingL( aStatus, store, &manager );
    CleanupStack::Pop( store ); // ownership passed
    }

// ---------------------------------------------------------
// CMmsClientMtm::AddLinkedAttachmentL
// ---------------------------------------------------------
//
void CMmsClientMtm::AddLinkedAttachmentL( const TDesC& aFilePath,
    const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus )
    {
    TInt error = KErrNone;
    TUint charset = aCharset;
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0 && aCharset == 0 )
        {
        // try to recognize character set
        // We trap the recognization process
        // If cannot recognize, the result will be 0 and default to us-ascii
        TRAP ( error,
            {
            RFile file;
            error = file.Open( Session().FileSession(), aFilePath,
                EFileRead|EFileShareReadersOnly);
            if ( error == KErrNone )
                {
                CleanupClosePushL( file );
                charset = RecognizeCharSetL( file );
                CleanupStack::PopAndDestroy( &file ); // close file
                }
            }
            );
        }
    // Linked files cannot be converted to utf8. They must be sent as is no
    // matter what the character set is.
    
    // Disk space is checked in AddFilePathAttachmentL after everything has
    // been initialized.
    
    
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0 &&
        ( charset == KMmsUTF16 ||
        charset == KMmsIso10646Ucs2 ||
        charset == KMmsUTF16LE ||
        charset == KMmsUTF16BE ) )
        {
        AddAttachmentL( aFilePath, aMimeType, charset, aStatus );
        }
    else
        {
        AddFilePathAttachmentL( aFilePath, aMimeType,
            CMsvAttachment::EMsvLinkedFile, aStatus, charset );
        }
    }

// ---------------------------------------------------------
// CMmsClientMtm::AddEntryAsAttachmentL
// ---------------------------------------------------------
//
void CMmsClientMtm::AddEntryAsAttachmentL( TMsvId /*aAttachmentId*/,
    TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------
// CMmsClientMtm::CreateAttachmentL
// ---------------------------------------------------------
//
void CMmsClientMtm::CreateAttachmentL( const TDesC& aFileName,
    RFile& aAttachmentFile, const TDesC8& aMimeType,
    TUint aCharset, TRequestStatus& aStatus )
    {
    // Character set cannot be checked here because the file does not exist yet
    // The caller gets an open file handle and is supposed to write data into
    // the file.
    if( !iAttaWaiter )
        {
        iAttaWaiter = CMmsAttachmentWaiter::NewL();
        }

    // store must be the first item allocated because it is the last one to be popped
    CMsvStore* store = iMsvEntry->EditStoreL();
    CleanupStack::PushL( store );
    
    CMsvAttachment* attachmentInfo = 
        CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachmentInfo );
    
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mimeHeaders );

    attachmentInfo->SetAttachmentNameL( aFileName );
    
    // set the mime-type if provided
    if( aMimeType.Length() > 0 )
        {
        attachmentInfo->SetMimeTypeL( aMimeType );
        TInt position = aMimeType.Find( KMmsSlash8 );
        if ( position > 0 )
            {
            mimeHeaders->SetContentTypeL( aMimeType.Left( position ) );
            }
        if ( position < aMimeType.Length() - 1 )
            {
            mimeHeaders->SetContentSubTypeL( aMimeType.Mid( position + 1 ) );
            }
        }
        
    // save the character set        
    mimeHeaders->SetMimeCharset( aCharset );

    mimeHeaders->SetSuggestedFilenameL( attachmentInfo->AttachmentName() );
    
    TInt size = 0;
    size = KMmsIndexEntryExtra + mimeHeaders->Size();    
    mimeHeaders->StoreL( *attachmentInfo );
    
    // mime headers have been streamed to CMsvAttachment, they can go now
    CleanupStack::PopAndDestroy( mimeHeaders );
    
    // Check that sufficient disk space available
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &Session().FileSession(), 
        size,
        iMessageDrive ) )
            {
            CleanupStack::PopAndDestroy( attachmentInfo );
            User::Leave( KErrDiskFull );
            }

    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    if ( iAttaWaiter->IsActive() )
        {
        // can't start an active operation because already active
        User::Leave(KErrInUse);
        }
    manager.CreateAttachmentL( aFileName, aAttachmentFile, attachmentInfo,
        iAttaWaiter->iStatus );
    CleanupStack::Pop( attachmentInfo ); // ownership passed to manager
    iAttaWaiter->StartWaitingL( aStatus, store, &manager );
    CleanupStack::Pop( store ); // ownership passed
    }

// ---------------------------------------------------------
// CMmsClientMtm::CancelAttachmentOperation
// ---------------------------------------------------------
//
void CMmsClientMtm::CancelAttachmentOperation()
    {
    
    if ( iAttaWaiter )
        {
        iAttaWaiter->Cancel();
        delete iAttaWaiter;
        iAttaWaiter = NULL;
        }
    }

// ---------------------------------------------------------
// CMmsClientMtm::CreateAttachment2L
// ---------------------------------------------------------
//
void CMmsClientMtm::CreateAttachment2L(
    CMsvStore& aStore,
    RFile& aFile,
    TDesC8& aMimeType,
    CMsvMimeHeaders& aMimeHeaders,
    CMsvAttachment* aAttachmentInfo,
    TMsvAttachmentId& aAttaId)
    {
    _LIT8( KMmsText, "text" );
    _LIT8( KMmsPlain, "plain" );
    
    if ( ( aMimeType.CompareF( KMmsTextPlain ) == 0  ||
        ( aMimeHeaders.ContentType().CompareF( KMmsText ) == 0 && 
        aMimeHeaders.ContentSubType().CompareF( KMmsPlain ) == 0 ) ) &&
        aMimeHeaders.MimeCharset() == 0 )
        {
        // If no character set defined for a plain text attachment
        // we try to recognize the character set.
        // But if recoginzation fails, we say 0 (us-ascii)
        TInt charset = 0;
        TRAP_IGNORE ( 
            {
            charset = RecognizeCharSetL( aFile );
            });
        aMimeHeaders.SetMimeCharset( charset );
        }
    
    return CMmsAttachmentHandler::CreateAttachmentL(
        aStore,
        aFile,
        Session().FileSession(),
        Session().CurrentDriveL(),
        aMimeType,
        aMimeHeaders,
        aAttachmentInfo,
        aAttaId );
  
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::CreateTextAttachmentL
// ---------------------------------------------------------
//
void CMmsClientMtm::CreateTextAttachmentL(
            CMsvStore& aStore,
            TMsvAttachmentId& aAttachmentId,
            const TDesC& aText,
            const TDesC& aFile,
            TBool aConvertParagraphSeparator )
    {

    CMmsAttachmentHandler::CreateTextAttachmentL( aStore,
        aAttachmentId,
        aText,
        aFile,
        Session().FileSession(),
        Session().CurrentDriveL(),
        aConvertParagraphSeparator );
 
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::CreateMessageL
// ---------------------------------------------------------
//
void CMmsClientMtm::CreateMessageL(
    TMsvId aServiceId )
    {
    // Check that sufficient disk space available
    // for index entry
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &Session().FileSession(), 
        KMmsIndexEntryExtra,
        iMessageDrive ) )
            {
            // we use standard error code here
            User::Leave( KErrDiskFull );
            }

    // just call the base class function
    CBaseMtm::CreateMessageL( aServiceId );
    
    iMmsHeaders->Reset( iMmsSettings );
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::BioTypeChangedL
// ---------------------------------------------------------
//
void CMmsClientMtm::BioTypeChangedL( TUid /*aBioTypeUid*/ )
    {
    // Do nothing. MMS does not support BIO type.
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::DefaultServiceL
// ---------------------------------------------------------
//
TMsvId CMmsClientMtm::DefaultServiceL() const
    {
    // Override base implementation so that we always use MMS service
    // even when called by Postcard or Audio Message which have different MTM tyoe
    
    if ( iServiceId != KMsvNullIndexEntryId ) 
        {
        return iServiceId;
        }
    
    // Create a new entry, showing invisible entries (because the service entry is invisible)
    
    TMsvSelectionOrdering ordering( KMsvNoGrouping, EMsvSortByNone, ETrue );
    CMsvEntry* entry = CMsvEntry::NewL( iOwnSession, KMsvRootIndexEntryId, ordering );
    CleanupStack::PushL( entry );
	
 	CMsvEntrySelection *sel=entry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
	CleanupStack::PushL( sel );
	if( sel->Count() == 0 )
	    {
	    User::Leave(KErrNotFound);
	    } 
	TMsvId service=sel->At( 0 );
 	CleanupStack::PopAndDestroy( sel );
 	CleanupStack::PopAndDestroy( entry );
	return service;
    }

// ---------------------------------------------------------
// CMmsClientMtm::RemoveDefaultServiceL
// ---------------------------------------------------------
//
void CMmsClientMtm::RemoveDefaultServiceL()
    {
    // not supported
    }

// ---------------------------------------------------------
// CMmsClientMtm::ChangeDefaultServiceL
// ---------------------------------------------------------
//
void CMmsClientMtm::ChangeDefaultServiceL( const TMsvId& /*aService*/ )
    {
    // not supported
    }

// ---------------------------------------------------------
// CMmsClientMtm::QueryCapability
// ---------------------------------------------------------
//
TInt CMmsClientMtm::QueryCapability(
    TUid aCapability,
    TInt& aResponse )
    {
    TInt error = KErrNone;
    switch ( aCapability.iUid )
        {
        // Supported:
        case KUidMtmQueryMaxTotalMsgSizeValue:
            aResponse = KMaxTInt;
            break;
        case KUidMsvMtmQueryEditorUidValue:
            aResponse = KUidMsgMmsEditor;
            break;
        case KUidMtmQueryMaxRecipientCountValue:
        // According to conformance document we must support at least 20
            aResponse = -1; // unlimited number
            break;
        case KUidMtmQuerySendAsMessageSendSupportValue:
            aResponse = ETrue;
            break;           
        case KUidMtmQuerySupportSubjectValue:
        case KUidMtmQuerySupportAttachmentsValue:
        case KUidMtmQueryCanSendMsgValue:
        case KUidMtmQueryCanReceiveMsgValue:
        case KUidMtmQuerySupportsRecipientTypeValue:
            // returns KErrNone
            break;
        // All others - Not Supported:
        default:
            error = KErrNotSupported;
            break;
        }
    return error;

    }

// ---------------------------------------------------------
// CClientMtm::InvokeSyncFunctionL
// ---------------------------------------------------------
//
void CMmsClientMtm::InvokeSyncFunctionL(
    TInt /*aFunctionId*/,
    const CMsvEntrySelection& /*aSelection*/,
    TDes8& /*aParameter*/ )
    {

    User::Leave( KErrNotSupported );

    }

// ---------------------------------------------------------
// CMmsClientMtm::InvokeAsyncFunctionL
// ---------------------------------------------------------
//
CMsvOperation* CMmsClientMtm::InvokeAsyncFunctionL(
    TInt aFunctionId,
    const CMsvEntrySelection& aSelection,
    TDes8& aParameter,
    TRequestStatus& aCompletionStatus )
    {
    
    CMsvOperation* op = NULL;
    
    if ( aSelection.Count() == 0 )
        {
        User::Leave( KErrArgument );
        }
        
    // make a copy of the selection just in case    
    CMsvEntrySelection* selection = aSelection.CopyL();
    CleanupStack::PushL( selection );

    // Change service id to related id
    // for the scheduled commands
    CMsvEntry* cEntry = NULL;
    TInt error = KErrNone;

    if ( aFunctionId == EMmsScheduledSend ||
        aFunctionId == EMmsScheduledReceive ||
        aFunctionId == EMmsScheduledReceiveForced ||
        aFunctionId == EMmsDeleteSchedule ||
        aFunctionId == EMmsScheduledForward ||
        aFunctionId == EMmsScheduledNotificationDelete ||
        aFunctionId == KMTMStandardFunctionsSendMessage )
        {
        TInt j = 0;
        TInt i;
        cEntry = Session().GetEntryL( KMsvRootIndexEntryId );
        CleanupStack::PushL( cEntry );

        if ( aSelection.At( 0 ) == iServiceId )
            {
            // skip service
            j++;
            }

        for ( i = aSelection.Count() - 1; i >=j;  --i )
            {
            TRAP( error, 
                {
                cEntry->SetEntryL( aSelection.At( i ) );
                TMsvEntry entry = cEntry->Entry();
                // check first to be sure you don't lose the original
                // service id
                if ( entry.iServiceId != KMsvLocalServiceIndexEntryId )
                    {
                    entry.iRelatedId = entry.iServiceId;
                    }
                entry.iServiceId = KMsvLocalServiceIndexEntryId;
                cEntry->ChangeL( entry );
                });
            if ( error == KErrNotFound )
                {
                selection->Delete( i );
                }
            }

        CleanupStack::PopAndDestroy( cEntry );
        cEntry = NULL;
        }
    
    if ( aFunctionId == KMTMStandardFunctionsSendMessage && selection->Count() > 0 )
        {
        // This is the SendAs send command. Ignore parameter
        TCommandParameters parameters; // initialized to zero
        TCommandParametersBuf paramPack( parameters );
        // Move the messages to OUTBOX synchronously
        cEntry = Session().GetEntryL( KMsvLocalServiceIndexEntryIdValue );
        CleanupStack::PushL( cEntry );
        cEntry->SetEntryL( selection->At( 0 ) );
        TMsvId currentParent = cEntry->Entry().Parent();
        TMsvLocalOperationProgress progress;
        if ( currentParent != KMsvGlobalOutBoxIndexEntryId )
            {
            cEntry->MoveL( *selection, KMsvGlobalOutBoxIndexEntryId, progress );
            }
        CleanupStack::PopAndDestroy( cEntry );
        cEntry = NULL;
        }
      
    if ( selection->Count() == 0 )
        {
        // we are given a selection of entries that were not found.
        User::Leave( KErrNotFound );
        }
        
    //
    // Pass all commands onto the server MTM
    //
    if ( aFunctionId == KMTMStandardFunctionsSendMessage )
        {
        // no need to put op on cleanup stack because
        // its ownership will be transferred to caller in a moment
        op = CMmsSendMessageOperation::NewL( Session(),
            *selection,
            aParameter,
            aCompletionStatus );
        }
    else
        {
        op =  Session().TransferCommandL(
            *selection, aFunctionId, aParameter, aCompletionStatus );
        }
    CleanupStack::PopAndDestroy( selection );
    return op;    
    }

// ---------------------------------------------------------
// CMmsClientMtm::ListNotificationsInInboxL
// ---------------------------------------------------------
//
CMsvEntrySelection* CMmsClientMtm::ListNotificationsInInboxL()
    {
    CMsvEntry* cEntry = Session().GetEntryL( KMsvGlobalInBoxIndexEntryId );
    CleanupStack::PushL( cEntry );

    CMsvEntrySelection* notifications = 
        cEntry->ChildrenWithMtmL( KUidMsgMMSNotification );
    CleanupStack::PushL( notifications );

    // Count only notifications that are free for a new operation
    for ( TInt i = notifications->Count() - 1 ; i >= 0; --i ) 
        {
        TMsvId notifId = notifications->At( i );
        cEntry->SetEntryL( notifId );
    
        TMsvEntry tEntry = cEntry->Entry();
        if ( tEntry.iMtmData2 & KMmsNewOperationForbidden )
            {
            // Delete notification from the notifications array.
            notifications->Delete( i );
            }
        }

    notifications->Compress();
    CleanupStack::Pop( notifications ); // ownership transferred to caller
    CleanupStack::PopAndDestroy( cEntry );
    return notifications;
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::ContextEntrySwitched
// ---------------------------------------------------------
//
void CMmsClientMtm::ContextEntrySwitched()
    {
    // Context change notification.
    // Reset data.

    // Note: Body text reset would be here if supported.
    
    iAddresseeList->Reset();
    iMmsHeaders->Reset( iMmsSettings );
    iFetchAll = EFalse;
    iFetchOverride = ETrue;
    iAttributes->Reset();
    }

// ---------------------------------------------------------
// CMmsClientMtm::HandleEntryEventL
// ---------------------------------------------------------
//
void CMmsClientMtm::HandleEntryEventL(
    TMsvEntryEvent /*aEvent*/,
    TAny* /*arg1*/,
    TAny* /*arg2*/,
    TAny* /*arg3*/ )
    {
    // No operation
    }

// ---------------------------------------------------------
// CMmsClientMtm::ConstructL
// ---------------------------------------------------------
//
void CMmsClientMtm::ConstructL()
    {
    // First loading settings
    iMmsSettings = CMmsSettings::NewL();
    iMmsSettings->LoadSettingsL();
    // Get the base values to detect changes
    iHomeMode = iMmsSettings->ReceivingModeHome();
    iRoamingMode = iMmsSettings->ReceivingModeForeign();
    iAccessPointCount = iMmsSettings->AccessPointCount();
    
    iMessageDrive = EDriveC;
    TInt error = KErrNone;

    TRAP ( error, { iMessageDrive = Session().CurrentDriveL(); } );

    if ( error != KErrNone )
        {
        // if cannot ask, use default
        iMessageDrive = EDriveC;
        }

    // if no service, creating one
    TMsvId tempServiceId = KMsvNullIndexEntryId;
    TRAP( error, tempServiceId = DefaultServiceL() )
        {
        if ( error == KErrNotFound )
            {
            tempServiceId = KMsvNullIndexEntryId;
            }
        }
    
    // service id is set after call to DefaultServiceL(), otherwise
    // DefaultServiceL() will just return iServiceId
    iServiceId = iMmsSettings->Service();
    if ( iServiceId == KMsvNullIndexEntryId ||
         iServiceId != tempServiceId )
        {
        // If there was no default service, a new one must be created.
        
        // If the service id saved in the settings differs from
        // the one read from disk, we call create service.
        // The create service function should check the actual
        // values of MMS service and MMS private folders and
        // ensure that the values in central repository and mail store match.
        
        // Check that sufficient disk space available
        // for service entry
        if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
            &Session().FileSession(), 
            KMmsIndexEntryExtra, // Service entry has no extra data
            iMessageDrive ) )
            {
            // we use standard error code here
            User::Leave( KErrDiskFull );
            }
        // creates new service entry + notification and mmbox folder entries
        iMmsSettings->CreateNewServiceL( Session() ); 
        // Save settings is not needed - service creation saves the settings.
        iServiceId = iMmsSettings->Service();
        }

    iMmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );

    // Address alias separators no longer read from resource.
    // Standard MIME separators < and > always used.
    
    iAttributes  = new(ELeave) CDesCArrayFlat( KMmsAttributeArrayGranularity );

    // Set the original context to the service entry
    SwitchCurrentEntryL( iServiceId );

    }

// ---------------------------------------------------------
// CMmsClientMtm::ValidateService
// ---------------------------------------------------------
//
TInt CMmsClientMtm::ValidateService( TMsvId aServiceId )
    {
    TInt error = KErrNone;
    // There should be only one service,
    // and it must match with the one that is in settings
    if( aServiceId != iMmsSettings->Service() )
        {
        return KErrNotFound;
        }
    
    // Check settings
    error = iMmsSettings->ValidateSettings();
    return error;
    }

// ---------------------------------------------------------
// CMmsClientMtm::MessageSize
// ---------------------------------------------------------
//
TInt32 CMmsClientMtm::MessageSize()
    {
    // First we should assert that iMsvEntry is not NULL, and panic, if it is
    __ASSERT_DEBUG( iMsvEntry, gPanic( EMmsNoCMsvEntrySet ));
    TUint size = 0;
    TRAP_IGNORE({size = AttachmentsSizeL() + iMmsHeaders->Size();} );
    return size;
    }

// ---------------------------------------------------------
// CMmsClientMtm::SetMessageDescriptionL
// ---------------------------------------------------------
//
void CMmsClientMtm::SetMessageDescriptionL( const TDesC& aText )
    {
    // First we should assert that iMsvEntry is not NULL, and panic, if it is
    __ASSERT_DEBUG( iMsvEntry, gPanic( EMmsNoCMsvEntrySet ));
    TMsvEntry entry = iMsvEntry->Entry();
    entry.iDescription.Set( aText );
    iMsvEntry->ChangeL( entry );
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::AddAttributeL
// ---------------------------------------------------------
//
void CMmsClientMtm::AddAttributeL( const TDesC& aName, const TDesC& aValue )
    {
    if ( aName.Length() == 0 || aValue.Length() == 0 )
        {
        User::Leave( KErrArgument );
        }
    TMmsGenUtils::AddAttributeL( aName, aValue, *iAttributes );
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::GetAttributeL
// ---------------------------------------------------------
//
TPtrC CMmsClientMtm::GetAttributeL( const TDesC& aName )
    {
    if ( aName.Length() == 0 )
        {
        User::Leave( KErrArgument );
        }
    return TMmsGenUtils::GetAttributeL( aName, *iAttributes );
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::FindAttribute
// ---------------------------------------------------------
//
TBool CMmsClientMtm::FindAttribute( const TDesC& aName )
    {
    if ( aName.Length() == 0 )
        {
        return EFalse;
        }
    return TMmsGenUtils::FindAttribute( aName, *iAttributes );
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::DeleteAttribute
// ---------------------------------------------------------
//
void CMmsClientMtm::DeleteAttribute( const TDesC& aName )
    {
    if ( aName.Length() == 0 )
        {
        return;
        }
    TMmsGenUtils::DeleteAttribute( aName, *iAttributes );
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::ResetAttributes
// ---------------------------------------------------------
//
void CMmsClientMtm::ResetAttributes()
    {
    iAttributes->Reset();
    }

// ---------------------------------------------------------
// CMmsClientMtm::BuildAddresseeListL
// ---------------------------------------------------------
//
void CMmsClientMtm::BuildAddresseeListL() 
    {

    iAddresseeList->Reset();

    const CDesCArray& array1 = iMmsHeaders->ToRecipients();
    const CDesCArray& array2 = iMmsHeaders->CcRecipients();
    const CDesCArray& array3 = iMmsHeaders->BccRecipients();

    BuildAddresseeListL( array1, EMsvRecipientTo );
    BuildAddresseeListL( array2, EMsvRecipientCc );
    BuildAddresseeListL( array3, EMsvRecipientBcc );

    }

// ---------------------------------------------------------
// CMmsClientMtm::BuildAddresseeListL
// ---------------------------------------------------------
//
void CMmsClientMtm::BuildAddresseeListL(
    const CDesCArray& aArray, TMsvRecipientType aValue ) 
    {

    TInt size;
    size = aArray.Count();
    for ( TInt i=0; i < size; ++i )
        {
        iAddresseeList->AppendL( aValue, aArray[i] );
        }
    }

// ---------------------------------------------------------
// CMmsClientMtm::AttachmentsSizeL
// ---------------------------------------------------------
//
TInt32 CMmsClientMtm::AttachmentsSizeL()
    {
    TInt32 size = 0;
    // We need to have store. If the caller is keeping edit store open,
    // this function leaves.
    // That makes sense, because in that case the user might have
    // uncommitted attachments, and the total size would be incorrect anyway.
    CMsvStore* store = iMsvEntry->ReadStoreL();
    CleanupStack::PushL( store );
    size = CMmsAttachmentHandler::AttachmentsSizeL( *store );
    CleanupStack::PopAndDestroy( store );
    store = NULL;    
    return size;
    }

// ---------------------------------------------------------
// CMmsClientMtm::ListMmsFolderNotificationsL()
// ---------------------------------------------------------
//
CMsvEntrySelection* CMmsClientMtm::ListMmsFolderNotificationsL() 
    {

    // list notifications in private invisible folder

    TMsvId notificationParent = iMmsSettings->NotificationFolder();

    CMsvEntrySelection* notifications = NULL;
    if ( notificationParent == KMsvNullIndexEntryId )
        {
        notifications = new ( ELeave ) CMsvEntrySelection();
        return notifications; // empty selection
        }
        
    CMsvEntry* cEntry = NULL;
    cEntry = Session().GetEntryL( notificationParent );
    CleanupStack::PushL( cEntry );

    // show invisible entries
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping,
        EMsvSortByNone, ETrue ) );
    notifications = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    CleanupStack::PushL( notifications );

    // strip off stuff other than notifications
    // We are handling the invisible MMS folder now
    // There may be delivery reports or binary notifications besides
    // the normal notifications

    for ( TInt i = notifications->Count() - 1; i >= 0; --i ) 
        {
        cEntry->SetEntryL( notifications->At( i ));
        
        TMsvEntry myEntry = cEntry->Entry();
        if ( !( ( myEntry.iMtmData1 & KMmsMessageTypeMask ) == 
            KMmsMessageMNotificationInd ) )
            {
            // this is not notification
            notifications->Delete( i );
            }
        }

    notifications->Compress();
        
    CleanupStack::Pop( notifications ); // ownership transferred to caller
    CleanupStack::PopAndDestroy( cEntry );

    return notifications;
    
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::ListInboxNotificationsL()
// ---------------------------------------------------------
//
CMsvEntrySelection* CMmsClientMtm::ListInboxNotificationsL()
    {
    CMsvEntry* cEntry = NULL;
    cEntry = Session().GetEntryL( KMsvGlobalInBoxIndexEntryId );
    CleanupStack::PushL( cEntry );
    
    CMsvEntrySelection* notifications = new( ELeave )CMsvEntrySelection;
    CleanupStack::PushL( notifications );
    
    // Notifications from inbox - but not in offline state if the user has not
    // initiated the fetch (iFetchOeverride = ETrue)
    // If the user initiates the fetch in offline mode, the entry goes to
    // failed state because the use must see why the fetching fails.
    // CMmsNotificationClientMtm overrides the FetchAllL function so that
    // we should get here only when the receiving mode changes.
    if ( iFetchOverride || TMmsGenUtils::NetworkOperationsAllowed() )
        {
        CMsvEntrySelection* notificationsInInbox = ListNotificationsInInboxL();
        CleanupStack::PushL( notificationsInInbox );
    
        // The readOnly flag has to be set on for notifications in inbox
        // in order to schedule notifications.
        for ( TInt j = notificationsInInbox->Count() - 1; j >= 0; --j )
            {
            cEntry->SetEntryL( notificationsInInbox->At( j ) );
            TMsvEntry entry = cEntry->Entry();
            entry.SetReadOnly( EFalse ); 
            cEntry->ChangeL( entry );

            notifications->AppendL( notificationsInInbox->At( j ) );
            }

        CleanupStack::PopAndDestroy( notificationsInInbox );
        }
        
    CleanupStack::Pop( notifications ); // ownership transferred to caller
    CleanupStack::PopAndDestroy( cEntry );
    
    return notifications;
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::FetchAllFromInboxL
// ---------------------------------------------------------
//
CMsvOperation* CMmsClientMtm::FetchAllFromInboxL( TRequestStatus& aCompletionStatus,
    TBool aForced )
    {
    
    iFetchOverride = aForced;
    
    CMsvEntrySelection* selection = ListInboxNotificationsL();
    CleanupStack::PushL( selection );

    if ( selection->Count() == 0 )
        {
        CleanupStack::PopAndDestroy( selection );
        TPckgC < TMsvId > progress = 0;
        aCompletionStatus = KRequestPending;
        return  CMsvCompletedOperation::NewL( Session(), Type(), progress,
            KMsvLocalServiceIndexEntryId, aCompletionStatus );
        }

    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack( parameters );

    CMsvOperation* op = NULL;
    if ( iFetchOverride )
        {
        op = InvokeAsyncFunctionL(
            EMmsScheduledReceiveForced,
            *selection,
            paramPack,
            aCompletionStatus );
        }
    else
        {
        op = InvokeAsyncFunctionL(
            EMmsScheduledReceive,
            *selection,
            paramPack,
            aCompletionStatus );
        // reset override to default value
        iFetchOverride = ETrue;
        }

    CleanupStack::PopAndDestroy( selection );
    return op;
    }
    

// ---------------------------------------------------------
// CMmsClientMtm::ConvertUTCDateToLocal
// ---------------------------------------------------------
//
/*
TInt64 CMmsClientMtm::ConvertUTCDateToLocal( TInt64 aDate ) const
    {

    TLocale locale;
    locale.Refresh();
    TInt64 localDate;
    TTimeIntervalSeconds universalTimeOffset( locale.UniversalTimeOffset() );

    localDate = aDate + universalTimeOffset.Int();

    if ( locale.QueryHomeHasDaylightSavingOn() )
        {
        TTimeIntervalSeconds daylightSaving( 60 * 60 );
        localDate += daylightSaving.Int();
        }
    
    return localDate;

    }
*/    

// ---------------------------------------------------------
// CMmsClientMtm::FindInRecipientL
// ---------------------------------------------------------
//
TBool CMmsClientMtm::FindInRecipientL( 
    const TDesC& aTextToFind,
    TMsvPartList aPartlist,
    const CDesCArray& aRecipients,
    CMsvFindText& aFindText )
    {
    TInt count = aRecipients.Count();
    TBool found = EFalse;
    for  (TInt i=0; i < count; i++ )
        {
        // Check alias and real address parts
        // separately. Otherwise separator character could 
        // spoil the check.
        if ( aFindText.FindTextL( aTextToFind, 
            TMmsGenUtils::Alias( aRecipients[i] ), aPartlist ) )
            {
            found = ETrue;
            break;
            }
        else if ( aFindText.FindTextL( aTextToFind, 
            TMmsGenUtils::PureAddress( aRecipients[i] ), aPartlist ) )
            {
            found = ETrue;
            break;
            }
        else
            {
            // keep LINT happy
            }
        }
    return found;
    }

// ---------------------------------------------------------
// CMmsClientMtm::AddFilePathAttachmentL
// ---------------------------------------------------------
//
void CMmsClientMtm::AddFilePathAttachmentL(
    const TDesC& aFilePath,
    const TDesC8& aMimeType,
    CMsvAttachment::TMsvAttachmentType aType,
    TRequestStatus& aStatus,
    const TUint aCharacterSet /* = 0 */ )
    {
    __ASSERT_DEBUG( aType != CMsvAttachment::EMsvMessageEntry,
        User::Invariant() );

    if( !iAttaWaiter )
        {
        iAttaWaiter = CMmsAttachmentWaiter::NewL();
        }
    
    // store must be the first item allocated because it is the last one to be popped
    CMsvStore* store = iMsvEntry->EditStoreL();
    CleanupStack::PushL( store );
    
    CMsvAttachment* attachment = CMsvAttachment::NewL( aType );
    CleanupStack::PushL( attachment );
    
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mimeHeaders );

    // set the size
    TEntry fileEntry;
    RFs& fs = Session().FileSession();
    User::LeaveIfError( fs.Entry( aFilePath, fileEntry ) );
    attachment->SetSize( fileEntry.iSize );
    
    // set attachment name
    TParse fileNameParser;
    User::LeaveIfError( fileNameParser.Set( aFilePath, NULL, NULL) );
    
    attachment->SetAttachmentNameL( fileNameParser.NameAndExt() );
    // set the mime-type if provided
    if( aMimeType.Length() > 0 )
        {
        attachment->SetMimeTypeL( aMimeType );
        TInt position = aMimeType.Find( KMmsSlash8 );
        if ( position > 0 )
            {
            mimeHeaders->SetContentTypeL( aMimeType.Left( position ) );
            }
        if ( position < aMimeType.Length() - 1 )
            {
            mimeHeaders->SetContentSubTypeL( aMimeType.Mid( position + 1 ) );
            }
        }

    mimeHeaders->SetSuggestedFilenameL( attachment->AttachmentName() );
    
    mimeHeaders->SetMimeCharset( aCharacterSet );
    mimeHeaders->StoreL( *attachment );
    
    TInt diskSpace = 0;
    diskSpace = KMmsIndexEntryExtra + mimeHeaders->Size();
    
    // mime headers have been streamed to CMsvAttachment, they can go now
    CleanupStack::PopAndDestroy( mimeHeaders );
    
    // now we know how much disk space we need
    if ( aType == CMsvAttachment::EMsvFile )
        {
        diskSpace += attachment->Size();
        }
        
    // Check that sufficient disk space available
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &Session().FileSession(), 
        diskSpace,
        iMessageDrive ) )
            {
            // we use standard error code here
            CleanupStack::PopAndDestroy( attachment );
            User::Leave( KErrDiskFull );
            }
    
    // attachment is initialised, pass to the attachment manager
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    if ( iAttaWaiter->IsActive() )
        {
        // can't start an active operation because already active
        User::Leave(KErrInUse);
        }
    switch( aType )
        {
        case CMsvAttachment::EMsvLinkedFile:
            manager.AddLinkedAttachmentL( aFilePath, attachment,
                iAttaWaiter->iStatus );
            break;
        default: // CMsvAttachment::EMsvFile
            manager.AddAttachmentL( aFilePath, attachment,
                iAttaWaiter->iStatus );
            break;
        }

    CleanupStack::Pop( attachment ); // ownership passed to manager
    // We cannot start waiting before we know that the function we are waiting for
    // did not leave. If we become active, and the function leaves, we are in trouble    
    iAttaWaiter->StartWaitingL( aStatus, store, &manager );
    CleanupStack::Pop( store ); // ownership passed to iAttaWaiter
    }
    
    
// ---------------------------------------------------------
// CMmsClientMtm::StoreAttributesL
// ---------------------------------------------------------
//
void CMmsClientMtm::StoreAttributesL( CMsvStore& aStore )
    {
    
    if ( iAttributes->MdcaCount() == 0 )
        {
        if ( aStore.IsPresentL( KUidMmsAttributeStream ) )
            {
            aStore.RemoveL( KUidMmsAttributeStream );
            }
        return;
        }
    
    RMsvWriteStream writeStream;
    // pushes 'writeStream' to the stack
    writeStream.AssignLC( aStore, KUidMmsAttributeStream );
    
    writeStream.WriteInt32L( iAttributes->MdcaCount() );
    
    TInt i;
    for ( i = 0; i < iAttributes->MdcaCount(); ++i )
        {
        writeStream << (*iAttributes)[i];
        }

    writeStream.CommitL();
    writeStream.Close();
    CleanupStack::PopAndDestroy( &writeStream );   // close writeStream
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::RestoreAttributesL
// ---------------------------------------------------------
//
void CMmsClientMtm::RestoreAttributesL( CMsvStore& aStore )
    {
    iAttributes->Reset();
    if ( !aStore.IsPresentL( KUidMmsAttributeStream ) )
        {
        return;
        }
        
    RMsvReadStream readStream;
    // pushes readStream to clenaup stack
    readStream.OpenLC( aStore, KUidMmsAttributeStream );
    
    TInt count = 0;
    
    count = readStream.ReadInt32L();
    TInt i;
    HBufC* desBuffer;
    
    for ( i = 0; i < count; ++i )
        {
        desBuffer = HBufC::NewLC( readStream, KMaxTInt );
        iAttributes->AppendL( *desBuffer );
        CleanupStack::PopAndDestroy( desBuffer );
        }
        
    readStream.Close();
    CleanupStack::PopAndDestroy( &readStream );   // close readStream
    }
    
// ---------------------------------------------------------
// CMmsClientMtm::RecognizeCharSetL
// ---------------------------------------------------------
//
TUint CMmsClientMtm::RecognizeCharSetL( RFile& aFile )
    {
    TUint charset = 0;
    
    // Use CMsgTextUtils for character conversion
    CMsgTextUtils* msgTextUtils = CMsgTextUtils::NewL( Session().FileSession() );
    CleanupStack::PushL( msgTextUtils );
    
    charset = msgTextUtils->RecognizeCharSetL( Session().FileSession(), aFile );
    if ( charset == 0 )
        {
        charset = KMmsUTF16; // utf16 with explicit byte order mark
        CleanupStack::PopAndDestroy( msgTextUtils );
        return charset; // unicode little endian or big endian
        }
    
    charset = msgTextUtils->CharconvIdToMibIdL( charset );
    CleanupStack::PopAndDestroy( msgTextUtils );
    return charset;
    }



// ---------------------------------------------------------
// CMmsClientMtm::
// ---------------------------------------------------------
//


// ================= OTHER EXPORTED FUNCTIONS ==============

//
// ---------------------------------------------------------
// gPanic implements
// panic function, should be used by debug version only
//
GLDEF_C void gPanic( TMmsPanic aPanic ) // error number enumerations
    {
    _LIT(KMmsPanic,"MMS");
    User::Panic( KMmsPanic, aPanic );
    }

//  End of File
