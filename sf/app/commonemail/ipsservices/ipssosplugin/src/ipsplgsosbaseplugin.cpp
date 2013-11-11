/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file implements classes CIpsPlgSosBasePlugin, Plugin.
 *
*/

#include "emailtrace.h"
#include "ipsplgheaders.h"

// <qmail> FreestyleEmailUiConstants include removed

// <qmail>
#include "ipsplgsosbaseplugin.hrh"
#include "ipsplgmailstoreroperation.h"
#include "ipsplgmessagepartstoreroperation.h"
#include "BasePlugin.h"
//<Qmail>
#include "ipssosextendedsettingsmanager.h"
#include "ipssettingkeys.h"
//</Qmail>
//</qmail>

#define FREESTYLE_EMAIL_UI_SID 0x200255BA

const TInt KOpGranularity = 2;

// <qmail> remove unused literals

#ifdef __WINS__
_LIT( KEmulatorIMEI, "123456789012345" );
#endif // __WINS__

//<Qmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::ActiveFolderChanged(
        const TFSMailMsgId& aActiveMailboxId,
        const TFSMailMsgId& aActiveFolderId)
    {
    TRAP_IGNORE( HandleActiveFolderChangeL(aActiveMailboxId,aActiveFolderId) );
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CEmailExtension* CIpsPlgSosBasePlugin::ExtensionL( const TUid& aInterfaceUid )
    {
    if(aInterfaceUid != KEmailMailboxStateExtensionUid)
        {
        User::Leave(KErrNotSupported);
        }

    return iStateExtension;
    }
//</Qmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> iSettingsApi removed
CIpsPlgSosBasePlugin::CIpsPlgSosBasePlugin( const TUint aFSPluginId ) :
    iFSPluginId( aFSPluginId ),
    iSession( NULL ),
    iMsgMapper( NULL ),
    iOperations( KOpGranularity ),
    iActivitytimers( KOpGranularity ),
    iSmtpService( NULL ),
    iCachedEntry( NULL ),
    iCachedEmailMessage( NULL ),
    iMruList( NULL ),
    iSearch( NULL ),
    iEventHandler( NULL ),
    iSessionOk( ETrue ),
    iBrandingId( NULL ),
    iIsUnderUiProcess( EFalse )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> iSettingsApi, iWait removed
EXPORT_C CIpsPlgSosBasePlugin::~CIpsPlgSosBasePlugin()
    {
    FUNC_LOG;
    iOperations.ResetAndDestroy();
    iActivitytimers.ResetAndDestroy();
    iOperations.Close();
    iActivitytimers.Close();
    delete iSmtpService;
    delete iMruList;
    if ( iEventHandler )
        {
        iEventHandler->UnRegisterPropertyObserver( iSyncStateHandler );
        }
    delete iStateExtension;
    delete iEventHandler;
    delete iCachedEntry;
    delete iCachedEmailMessage;
    delete iMsgMapper;
    delete iSearch;
    delete iSyncStateHandler;
    delete iSession;
    delete iBrandingId;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::BaseConstructL()
    {
    FUNC_LOG;
    iStateExtension = CIpsStateExtension::NewL(*this);
    iEventHandler = CIpsPlgEventHandler::NewL( *this );
    iSession = CMsvSession::OpenAsyncL( *iEventHandler );
    iMsgMapper = CIpsPlgMsgMapper::NewL( *iSession, *this );
    iSmtpService = CIpsPlgSmtpService::NewL( *iSession, *this );
    iMruList = CIpsPlgMruList::NewL( );
    iSearch = CIpsPlgSearch::NewL( *iSession, *this );
	// <qmail> iSettingsApi removed
    iSyncStateHandler = CIpsPlgSyncStateHandler::NewL(
            *iSession, *this, iOperations );
    iEventHandler->RegisterPropertyObserverL( iSyncStateHandler );

    RProcess process;
    if ( process.SecureId() == FREESTYLE_EMAIL_UI_SID )
        {
        iIsUnderUiProcess = ETrue;
        }
    else
        {
        iIsUnderUiProcess = EFalse;
        }

#ifndef RD_101_EMAIL
    RAlwaysOnlineClientSession aosession;
    TInt err = aosession.Connect();
    if ( err == KErrNone )
        {
        TBuf8<1> dummyBuf;
        TRAP( err, aosession.RelayCommandL(
                EServerAPIEmailDisableAOEmailPlugin,
                dummyBuf ) );
        }
    aosession.Close();
#endif
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::CompleteConstructL()
    {
    FUNC_LOG;
    iSessionOk = ETrue;
    iEventHandler->CompleteConstructL( iSession );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::SessionTerminated()
    {
    FUNC_LOG;
    iSessionOk = EFalse;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::OpCompleted(
	CIpsPlgSingleOpWatcher& aOpWatcher,
	TInt aCompletionCode )
    {
    FUNC_LOG;
    // Get valid operation count in each, some operations could have been
    // deleted in array
    TInt opId = aOpWatcher.Operation().Id();
    for ( TInt i = iOperations.Count()-1; i >= 0; i-- )
        {
        CMsvOperation& oper = iOperations[i]->Operation();

        if ( oper.Id() == opId )
            {
            DeleteAndRemoveOperation( i, aCompletionCode );
            }

// <qmail> iWait removed
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
 TFSMailBoxStatus CIpsPlgSosBasePlugin::GetMailBoxStatus(
    const TFSMailMsgId&  aMailBoxId  )
    {
    FUNC_LOG;
    TMsvEntry tEntry;
    TMsvId service;
    TFSMailBoxStatus status;
    if( iSessionOk )
        {
        iSession->GetEntry( aMailBoxId.Id(), service, tEntry );

        if ( tEntry.Connected() )
            {
            status = EFSMailBoxOnline;
            }
        else if ( ConnOpRunning( aMailBoxId ) )
            {
            status = EFSMailBoxOnline;
            }
        else
            {
            status = EFSMailBoxOffline;
            }
        }
    else
        {
        status = EFSMailBoxOffline;
        }
    return status;
    }

// ----------------------------------------------------------------------------
// CIpsPlgSosBasePlugin::SpecifiedSendingMailbox
// Returns 'null' ID, because the method is not relevant with IPS protocols
// ----------------------------------------------------------------------------
 TFSMailMsgId CIpsPlgSosBasePlugin::SpecifiedSendingMailbox()
     {
    FUNC_LOG;
     return TFSMailMsgId();
     }

 // ----------------------------------------------------------------------------
 // ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::ListMailBoxesL( RArray<TFSMailMsgId>& aMailboxes )
	{
    FUNC_LOG;

	if( !iSessionOk )
        {
        User::Leave( KErrNotReady );
        }
	CMsvEntry* cEntry = iSession->GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( cEntry );

    CMsvEntrySelection* childEntries = cEntry->ChildrenWithMtmL( MtmId() );
    CleanupStack::PushL( childEntries );

    TInt count( childEntries->Count() );
    for ( TInt i(0); i < count; i++)
        {
      	TFSMailMsgId mailboxId;
    	TMsvEntry tEntry;
    	TMsvId serviceId;
    	TDriveUnit driveUnit = EDriveC;

        iSession->GetEntry( childEntries->At(i), serviceId, tEntry );

        if( iIMEI.Compare( tEntry.iDescription ) == 0 )
            {
            mailboxId.SetPluginId( TUid::Uid( PluginId() ) );
            mailboxId.SetId( childEntries->At(i) );
            aMailboxes.AppendL( mailboxId );
            }
        else
            {
            TRAPD( err, driveUnit = iSession->CurrentDriveL() );
            if ( err == KErrNone && driveUnit == EDriveC )
                {
                // When memory card restored to another phone mailbox
                // should be visible
                mailboxId.SetPluginId( TUid::Uid( PluginId() ) );
                mailboxId.SetId( childEntries->At(i) );
                StoreIMEIToMailboxL( mailboxId.Id() );
                aMailboxes.AppendL( mailboxId );
                }
            }
        }
    CleanupStack::PopAndDestroy( 2, cEntry ); // childEntries
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CFSMailBox* CIpsPlgSosBasePlugin::GetMailBoxByUidL(
    const TFSMailMsgId& aMailBoxId)
	{
    FUNC_LOG;
    CFSMailBox* result( NULL );
    TMsvEntry tEntry;
    TMsvId    serviceId;
    TInt      status;
    HBufC*    address( NULL );

    if( !iSessionOk )
        {
        User::Leave( KErrNotReady );
        }
    status = iSession->GetEntry( aMailBoxId.Id(), serviceId, tEntry );

    if ( status == KErrNone )
        {
        result = CFSMailBox::NewL( aMailBoxId );
        CleanupStack::PushL( result );   // << result
        result->SetName( tEntry.iDetails );
        result->SetSettingsUid( TUid::Uid( IPS_SET_ECOM_IMPLEMENTATION_UID ) );

// <qmail>
        CEmailAccounts* acc = CEmailAccounts::NewLC();
        TSmtpAccount smtpAcc;
        acc->GetSmtpAccountL( tEntry.iRelatedId , smtpAcc );
        CImSmtpSettings* smtpSet = new (ELeave) CImSmtpSettings();
        CleanupStack::PushL( smtpSet );
        acc->LoadSmtpSettingsL( smtpAcc, *smtpSet );
        address = smtpSet->EmailAddress().AllocL();
        CleanupStack::PopAndDestroy( 2, acc );
// </qmail>

        CleanupStack::PushL( address ); // << address
        CFSMailAddress* fsAddress = CFSMailAddress::NewLC();    // << fsAddress
        fsAddress->SetEmailAddress( *address );
        result->SetOwnMailAddressL( fsAddress );
        CleanupStack::Pop( fsAddress ); // >> fsAddress
        CleanupStack::PopAndDestroy( address ); // >>> address
        CleanupStack::Pop( result );    // >> result
        }

    return result;
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::DeleteMailBoxByUidL(
    const TFSMailMsgId& aMailBoxId,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId )
    {
    FUNC_LOG;
    TMsvEntry tEntry;
    TMsvId service;
    if( !iSessionOk )
        {
        User::Leave( KErrNotReady );
        }

// <qmail> RAlwaysOnlineClientSession - related code removed
    // Prepare the parameters to be forwarded to AO-server
    TPckg<TMsvId> param = aMailBoxId.Id();

    // delete MRU list from cen rep
    iMruList->ClearDataL( aMailBoxId );

    iSyncStateHandler->NotifyMailboxRemove( aMailBoxId.Id() );

// <qmail> CancelAllOnlineOperations function call removed
    iSession->GetEntry( aMailBoxId.Id(), service, tEntry );
    if ( tEntry.Connected() )
        {
        DisconnectL( aMailBoxId, aOperationObserver, aRequestId, ETrue );

        // remove activity timer from array here but leave the actual delete
        // to the disconnect operation. This is because the disconnect op
        // sometimes tries to use the timer after it was deleted here.
        TInt timerCount = iActivitytimers.Count();
        for ( TInt j = 0; j < timerCount; j++ )
            {
            if ( iActivitytimers[j]->FSMailboxId() == aMailBoxId )
                {
                iActivitytimers.Remove( j );
                timerCount--;
                j--;
                }
            }
        }
    else
        {
// <qmail> iSettingsApi removed
        TFSProgress progress = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
        progress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        progress.iError = KErrNone;
        TInt requestId = aRequestId;
        aOperationObserver.RequestResponseL( progress, requestId );

        DeleteActivityTimer( aMailBoxId  );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TDesC& CIpsPlgSosBasePlugin::GetBrandingIdL( const TFSMailMsgId& aMailBoxId )
    {
    FUNC_LOG;
    if( !iSessionOk )
        {
        User::Leave( KErrNotReady );
        }
    CMsvEntry* mboxEntry = iSession->GetEntryL( aMailBoxId.Id() );
    CleanupStack::PushL( mboxEntry );

// <qmail> iSettingsApi removed, iBrandingId removed
    CleanupStack::PopAndDestroy( mboxEntry );

    return *iBrandingId;
    }

// ----------------------------------------------------------------------------
// Pop3 has no implementation for this virtual
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::MoveMessagesL(
    const TFSMailMsgId& aMailBoxId,
    const RArray<TFSMailMsgId>& aMessageIds,
    const TFSMailMsgId& aSourceFolderId,
    const TFSMailMsgId& aDestinationFolderId )
	{
    FUNC_LOG;
	// Pop3 has no implementation for this virtual
	if( aDestinationFolderId.Id() == KMsvDraftEntryId  )
	    {
	    MoveMessagesToDraftL(
	        aMailBoxId,
	        aMessageIds,
	        aSourceFolderId,
	        aDestinationFolderId );
	    }
	}

// ----------------------------------------------------------------------------
// asynchronic version from move messages function, pop3 plugin not implent
// this virtual fucntion.
// ----------------------------------------------------------------------------
TInt CIpsPlgSosBasePlugin::MoveMessagesL(
    const TFSMailMsgId& aMailBoxId,
    const RArray<TFSMailMsgId>& aMessageIds,
    const TFSMailMsgId& aSourceFolderId,
    const TFSMailMsgId& aDestinationFolderId,
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId )
    {
    FUNC_LOG;
    TInt ret = KErrNotSupported;
    if( aDestinationFolderId.Id() == KMsvDraftEntryId  )
        {
        MoveMessagesToDraftL(
            aMailBoxId,
            aMessageIds,
            aSourceFolderId,
            aDestinationFolderId );
        ret = KErrNone;
        }
    ret = KErrNotSupported;
    TFSProgress progress = {
            TFSProgress::EFSStatus_RequestComplete, 0, 0, ret };
    aOperationObserver.RequestResponseL(
            progress, aRequestId );
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::MoveMessagesToDraftL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const RArray<TFSMailMsgId>& aMessageIds,
    const TFSMailMsgId& aSourceFolderId,
    const TFSMailMsgId& aDestinationFolderId )
    {
    FUNC_LOG;

    TInt count( aMessageIds.Count() );
    if ( !count )
        {
        User::Leave( KErrArgument );
        }

    if( !iSessionOk )
        {
        User::Leave( KErrNotReady );
        }

    TMsvId msgService;
    TMsvEntry tEntry;

    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);

    CMsvEntry* msgEntry = iSession->GetEntryL( aMessageIds[0].Id() );
    CleanupStack::PushL( msgEntry );

	for( TInt i(0); i < count; i++ )
        {
        iSession->GetEntry( aMessageIds[i].Id(), msgService, tEntry );

        if( aSourceFolderId.Id() == KMsvGlobalOutBoxIndexEntryIdValue )
            {
            if( tEntry.SendingState() != KMsvSendStateSending )
                {
                // Set all states in outbox to suspended
                tEntry.SetSendingState( KMsvSendStateSuspended );
                msgEntry->ChangeL( tEntry );
                sel->AppendL( tEntry.Id() );
                }
            else
                {
                // Not possible to move when sending, what to do???
                // Probably some note back to UI, but we leave..
                User::Leave( KErrNotSupported );
                }
            }
        else
            {
            sel->AppendL( tEntry.Id() );
            }
        }
    if( sel->Count() )
        {
        // <qmail> remove activeschedulerwait
        TMsvLocalOperationProgress progress;
        if( !aSourceFolderId.IsNullId() )
            {
            CMsvEntry* cEntry = iSession->GetEntryL( aSourceFolderId.Id() );
            CleanupStack::PushL( cEntry );
            cEntry->MoveL( *sel, aDestinationFolderId.Id(), progress );
            CleanupStack::PopAndDestroy( cEntry );
            }
        else
            {
            // Message is in editing state, we can't use parent as entry
            // because it's equal to destination.
            TMsvId parent = msgEntry->Entry().Parent();
            msgEntry->SetEntryL( parent );
            msgEntry->CopyL( *sel, aDestinationFolderId.Id(), progress );
            }
        User::LeaveIfError( progress.iError );
        // </qmail>
        }
    CleanupStack::PopAndDestroy( 2, sel ); // msgEntry, sel
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::CopyMessagesL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const RArray<TFSMailMsgId>& /*aMessageIds*/,
    RArray<TFSMailMsgId>& /*aNewMessages*/,
    const TFSMailMsgId& /*aSourceFolderId*/,
    const TFSMailMsgId& /*aDestinationFolderId*/ )
	{
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::GoOnlineL( const TFSMailMsgId& aMailBoxId )
    {
    if ( iSyncStateHandler->GetCurrentSyncState(
            aMailBoxId) == Idle)
        {
        // append mailbox id go online mailbox array
        RefreshNowL( aMailBoxId, *this, 0 );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::GoOfflineL( const TFSMailMsgId& aMailBoxId )
    {
    FUNC_LOG;
    CancelAllOnlineOperations( aMailBoxId );
    // use 0 for request id
    DisconnectL( aMailBoxId, *this, 0 );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TFSProgress CIpsPlgSosBasePlugin::GetLastSyncStatusL(
    const TFSMailMsgId& /*aMailBoxId*/ )
    {
    FUNC_LOG;
    if( !iSessionOk )
        {
        User::Leave( KErrNotReady );
        }
    TFSProgress progress = { TFSProgress::EFSStatus_Status, 0, 0, KErrNone };
    return progress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsPlgSosBasePlugin::CancelSyncL( const TFSMailMsgId& aMailBoxId )
    {
    FUNC_LOG;
    TInt err = KErrNone;

    // found correct operation
    for ( TInt i = iOperations.Count()-1; i >= 0; i-- )
        {
        const CIpsPlgBaseOperation* baseOp =
            iOperations[i]->BaseOperation();
        if ( baseOp && baseOp->FSMailboxId() == aMailBoxId &&
               ( baseOp->IpsOpType() == EIpsOpTypePop3SyncOp
                || baseOp->IpsOpType() == EIpsOpTypeImap4SyncOp
                || baseOp->IpsOpType() == EIpsOpTypeImap4PopulateOp ) )
            {
            DeleteAndRemoveOperation( i, KErrCancel );
            }
        }

#ifndef RD_101_EMAIL
    RAlwaysOnlineClientSession aosession;
    err = aosession.Connect();
    if ( err == KErrNone )
        {
        TPckgBuf<TMsvId> buf(aMailBoxId.Id());
        TRAP( err, aosession.RelayCommandL(
                EServerAPIEmailCancelAllAndDoNotDisconnect, buf ) );
        }
    aosession.Close();
#endif

    return err;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CFSMailFolder* CIpsPlgSosBasePlugin::GetFolderByUidL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolderId )
    {
    FUNC_LOG;
    CFSMailFolder* result( NULL);
    if( !iSessionOk )
        {
        User::Leave( KErrNotReady );
        }

    CMsvEntry* folderEntry = iSession->GetEntryL( aFolderId.Id() );
    CleanupStack::PushL( folderEntry ); // << folderEntry

    if ( folderEntry )
        {
        result = CFSMailFolder::NewLC( aFolderId ); // << result
        result->SetMailBoxId( aMailBoxId );

        CMsvEntrySelection* msgChilds = folderEntry->ChildrenWithTypeL(
                KUidMsvMessageEntry );
        CleanupStack::PushL( msgChilds );
        TInt msgCount( msgChilds->Count() );
        result->SetMessageCount( msgCount );
        TInt unreadCount( 0 );
        TMsvId serviceId;
        for ( TInt i(0); i < msgCount; i++ )
            {
            TMsvEmailEntry emlEntry;
            TMsvId dummy;
            TBool isRead = ETrue;
            if ( iSession->GetEntry(
                    msgChilds->At(i), dummy, emlEntry ) == KErrNone )
                {
                if ( ( PluginId() == KIpsPlgImap4PluginUid.iUid ||
                       PluginId() == KIpsPlgPop3PluginUid.iUid ) &&
                        emlEntry.Unread() )
                    {
                    isRead = EFalse;
                    }
                }
            if ( !isRead && !emlEntry.LocallyDeleted() )
                {
                unreadCount++;
                }
            }

        CleanupStack::PopAndDestroy( msgChilds );
        result->SetUnreadCount( unreadCount );
        result->SetFolderName( folderEntry->Entry().iDetails );
        result->SetFolderType( GetFolderType( folderEntry, aFolderId ) );
        TMsvEntry parentEntry;
        TInt status = iSession->GetEntry( folderEntry->Entry().Parent( ), serviceId,
                parentEntry );
        TUint parent( 0 );
		if( status == KErrNone )
			{
			if( parentEntry.iType == KUidMsvFolderEntry )
				{
				parent = static_cast<TUint>( folderEntry->Entry().Parent() );
				}
			else
				{
                parent = 0;
				}
			}

        TFSMailMsgId parentId( PluginId(), parent );
        result->SetParentFolderId( parentId );

        // Set subfolder count here for ListFolderL
        CMsvEntrySelection* fldChildren =
    		folderEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    	CleanupStack::PushL( fldChildren );    // << children
        result->SetSubFolderCount( fldChildren->Count() );
        CleanupStack::PopAndDestroy( fldChildren );    // >>> children

        // Set blocklist for FW
        BlockCopyMoveFromFoldersL( folderEntry, aFolderId, *result );

        CleanupStack::Pop( result );    // >> result
        }
	CleanupStack::PopAndDestroy( folderEntry ); // >>> folderEntry

    return result;
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CFSMailFolder* CIpsPlgSosBasePlugin::CreateFolderL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& /* aFolderId */,
    const TDesC& /* aFolderName */,
    const TBool /* aSync */)
	{
	return NULL;
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::DeleteFolderByUidL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& /* aFolderId */)
	{
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
MFSMailIterator* CIpsPlgSosBasePlugin::ListMessagesL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolderId,
    const TFSMailDetails aDetails,
    const RArray<TFSMailSortCriteria>& aSorting )
    {
    FUNC_LOG;
    CIpsPlgMsgIterator* iterator = CIpsPlgMsgIterator::NewL(
        *this, *iSession, aMailBoxId, aFolderId, aDetails, aSorting );

    return iterator;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CFSMailMessage* CIpsPlgSosBasePlugin::GetMessageByUidL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /* aFolderId */,
    const TFSMailMsgId& aMessageId,
    const TFSMailDetails aDetails)
	{
    FUNC_LOG;
	CFSMailMessage* result( NULL );
	TMsvId serviceId;
	TMsvEntry tEntry;
	TInt status( KErrNone );

	if( !iSessionOk )
	    {
	    User::Leave( KErrNotReady );
	    }

	status = iSession->GetEntry( aMessageId.Id(), serviceId, tEntry);

	const TMsvEmailEntry& emlEntry(tEntry);
	// do not give deleted marked messages
	if ( status == KErrNone &&
	    EDisconnectedDeleteOperation != emlEntry.DisconnectedOperation()
	    && !emlEntry.DeletedIMAP4Flag() )
	    {
        result = iMsgMapper->GetMailMessageL( aMailBoxId, tEntry, aDetails );
	    }

	return result;
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
 CFSMailMessage* CIpsPlgSosBasePlugin::CreateMessageToSendL(
    const TFSMailMsgId& /*aMailBoxId*/ )
	{
    // <qmail> not used any more
    User::Leave(KErrFSMailPluginNotSupported);
    return NULL; // prevents compiler warning
	}

// <qmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::CreateMessageToSendL( const TFSMailMsgId& aMailBoxId,
                                    MFSMailRequestObserver& aOperationObserver,
                                    const TInt aRequestId )
    {
    FUNC_LOG;
    // asynchronous message creation is started here to enable canceling of the operation
    TMsvPartList partList( KMsvMessagePartBody );

    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewLC(*this);

    //<qmail> removed mboxEntry from parameters + changed iSmtpService to pointer

    CIpsPlgCreateMessageOperation* op = CIpsPlgCreateMessageOperation::NewL(
        *iSmtpService,
        *iSession,
        watcher->iStatus,
        partList,
        aMailBoxId,
        aOperationObserver,
        aRequestId );
    //</qmail>
    watcher->SetOperation( op );

    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );
    }
// </qmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CFSMailMessage* CIpsPlgSosBasePlugin::CreateForwardMessageL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aOriginalMessageId*/,
    const TDesC& /*aHeaderDescriptor*/ )
    {
    // <qmail> not used any more
    User::Leave(KErrFSMailPluginNotSupported);
    return NULL; // prevents compiler warning
    }

// <qmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::CreateForwardMessageL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aOriginalMessageId,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId,
    const TDesC& /*aHeaderDescriptor*/ )
    {
    FUNC_LOG;
    // asynchronous message creation is started here to enable canceling of the operation

    // 1. part of function checs that body text and all
    // attachments are fetched
    TMsvEntry orgMsg;
    TMsvId service;
    User::LeaveIfError( iSession->GetEntry(
            aOriginalMessageId.Id(), service, orgMsg ) );

    if ( orgMsg.Id() ==  KMsvNullIndexEntryIdValue )
        {
        User::Leave(KErrNotFound);
        }

    if ( ( orgMsg.Parent() == KMsvSentEntryIdValue ) &&
         ( orgMsg.iMtm == KSenduiMtmSmtpUid ) )
        {
        iSmtpService->ChangeServiceIdL( orgMsg );
        }

    TMsvPartList partList( KMsvMessagePartBody | KMsvMessagePartAttachments );

    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewLC(*this);

    //<qmail> changed iSmtpService to pointer
    CIpsPlgCreateForwardMessageOperation* op = CIpsPlgCreateForwardMessageOperation::NewL(
        *iSmtpService,
        *iSession,
        watcher->iStatus,
        partList,
        aMailBoxId,
        orgMsg.Id(),
        aOperationObserver,
        aRequestId );
    //</qmail>
    watcher->SetOperation( op );

    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );
    }
// </qmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CFSMailMessage* CIpsPlgSosBasePlugin::CreateReplyMessageL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aOriginalMessageId*/,
    const TBool /*aReplyToAll*/,
    const TDesC& /*aHeaderDescriptor*/ )
    {
    // <qmail> not used any more
    User::Leave(KErrFSMailPluginNotSupported);
    return NULL; // prevents compiler warning
    }

// <qmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::CreateReplyMessageL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aOriginalMessageId,
    const TBool aReplyToAll,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId,
    const TDesC& /* aHeaderDescriptor */ )
    {
    FUNC_LOG;
    // asynchronous message creation is started here to enable canceling of the operation

    // find orginal message header and check that body is fetched
    TMsvEntry orgMsg;
    TMsvId service;
    User::LeaveIfError(
        iSession->GetEntry( aOriginalMessageId.Id(), service, orgMsg ) );

    if ( orgMsg.Id() ==  KMsvNullIndexEntryIdValue )
        {
        User::Leave(KErrNotFound);
        }

    if ( ( orgMsg.Parent() == KMsvSentEntryIdValue ) &&
         ( orgMsg.iMtm == KSenduiMtmSmtpUid ) )
        {
        iSmtpService->ChangeServiceIdL( orgMsg );
        }

    // partList flags control e.g. what kind of recipient set is created
    TMsvPartList partList = KMsvMessagePartBody | KMsvMessagePartDescription
    | KMsvMessagePartOriginator;
    if( aReplyToAll )
        {
        partList |= KMsvMessagePartRecipient;
        }

    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewLC(*this);

    //<qmail> changed iSmtpService to pointer
    CIpsPlgCreateReplyMessageOperation* op = CIpsPlgCreateReplyMessageOperation::NewL(
        *iSmtpService,
        *iSession,
        watcher->iStatus,
        partList,
        aMailBoxId,
        orgMsg.Id(),
        aOperationObserver,
        aRequestId );
    //</qmail>
    watcher->SetOperation( op );

    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );
    }
// </qmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsPlgSosBasePlugin::StoreMessageL(
    const TFSMailMsgId& aMailBoxId,
    CFSMailMessage& aMessage )
 	{
    FUNC_LOG;
    TInt status( KErrNone);
    TFSMailMsgId msgId( aMessage.GetMessageId( ));
    TMsvEntry tEntry;
    TMsvId serviceId;
    TBool incoming( EFalse);

    // Check whether this is IMAP4 or POP3 (incoming) message
    if ( !msgId.IsNullId() )
        {
        status = iSession->GetEntry( msgId.Id( ), serviceId, tEntry );

        if ( status == KErrNone )
            {
            incoming = ( tEntry.iMtm == KSenduiMtmImap4Uid ) ||
                ( tEntry.iMtm == KSenduiMtmPop3Uid );
            }
        }

    if ( incoming )
        {
		// <qmail> Cmail change to synchronous call (UpdateMessageFlagsL) ignored,
        // <qmail> because it causes emulator & HW to freeze on 10.1
        CIpsPlgSingleOpWatcher* opW = CIpsPlgSingleOpWatcher::NewLC( *this );
        CMsvOperation* op = iMsgMapper->UpdateMessageFlagsAsyncL(
                msgId.Id(), aMessage, opW->iStatus );

        if ( op )
            {
            opW->SetOperation( op );
            iOperations.AppendL( opW );
            CleanupStack::Pop( opW );
            }
        else
            {
            CleanupStack::PopAndDestroy( opW );
            }
        }
    else
        {
        iSmtpService->StoreMessageL( aMailBoxId, aMessage );
        }
    }

// <qmail>
/**
 *
 * @param aMailBoxId Mailbox id.
 * @param aOperationObserver Operation observer.
 * @param aRequestId The request id.
 */
void CIpsPlgSosBasePlugin::StoreMessagesL(
    const TFSMailMsgId& /*aMailBox*/,
    RPointerArray<CFSMailMessage> &messages,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId )

    {
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
    CleanupStack::PushL(watcher);
    CIpsPlgMailStorerOperation* op =
            CIpsPlgMailStorerOperation::NewLC(
                    *iSession,
                    watcher->iStatus,
                    *this,
                    messages,
                    aOperationObserver,
                    aRequestId );

    watcher->SetOperation(op);
    iOperations.AppendL(watcher);
    CleanupStack::Pop( 2, watcher );
    }
// </qmail>

// ----------------------------------------------------------------------------
// CIpsPlgSosBasePlugin::GetMessagesL()
// ----------------------------------------------------------------------------
//
void CIpsPlgSosBasePlugin::GetMessagesL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolderId,
    const RArray<TFSMailMsgId>& aMessageIds,
    RPointerArray<CFSMailMessage>& aMessageList,
    const TFSMailDetails aDetails )
	{
    FUNC_LOG;
    TInt i;
    CFSMailMessage* msg;

	for (i = 0; i < aMessageIds.Count(); i++ )
        {
        msg = GetMessageByUidL(
            aMailBoxId, aFolderId, aMessageIds[i], aDetails );
        CleanupStack::PushL( msg );
        aMessageList.AppendL( msg );
        CleanupStack::Pop( msg );
        }
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::ChildPartsL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentId,
    RPointerArray<CFSMailMessagePart>& aParts)
	{
    FUNC_LOG;
    iMsgMapper->GetChildPartsL( aMailBoxId, aMessageId, aParentId, aParts );
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CFSMailMessagePart* CIpsPlgSosBasePlugin::NewChildPartL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentPartId,
    const TFSMailMsgId& /* aInsertBefore */,
    const TDesC& aContentType )
	{
    FUNC_LOG;
    CFSMailMessagePart* result;
    result = iMsgMapper->NewChildPartL( aMailBoxId, aMessageId, aParentPartId,
        aContentType );
    return result;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CFSMailMessagePart* CIpsPlgSosBasePlugin::NewChildPartFromFileL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& /*aMessageId*/,
    const TFSMailMsgId& /*aParentPartId*/,
    const TDesC& /*aContentType*/,
    const TDesC& /*aFilePath*/ )
    {
    // <qmail> not used any more
    User::Leave(KErrFSMailPluginNotSupported);
    return NULL; // prevents compiler warning
    }

// <qmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::NewChildPartFromFileL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& /* aParentPartId */,
    const TDesC& aContentType,
    const TDesC& aFilePath,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId )
    {
    FUNC_LOG;
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewLC(*this);

    CIpsPlgNewChildPartFromFileOperation* op = CIpsPlgNewChildPartFromFileOperation::NewL(
        *iSession,
        watcher->iStatus,
        aMailBoxId,
        aMessageId,
        aContentType,
        aFilePath,
        aOperationObserver,
        aRequestId,
        iMsgMapper);
    watcher->SetOperation( op );

    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );
    }
// </qmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CFSMailMessagePart* CIpsPlgSosBasePlugin::NewChildPartFromFileL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& /*aMessageId*/,
    const TFSMailMsgId& /*aParentPartId*/,
    const TDesC& /*aContentType*/,
    RFile& /*aFile*/ )
    {
    // <qmail> not used any more
    User::Leave(KErrFSMailPluginNotSupported);
    return NULL; // prevents compiler warning
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CFSMailMessagePart* CIpsPlgSosBasePlugin::CopyMessageAsChildPartL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& /* aMessageId */,
    const TFSMailMsgId& /* aParentPartId */,
    const TFSMailMsgId& /* aInsertBefore */,
    const CFSMailMessage& /* aMessage */)
	{
	return NULL;
	}

// ----------------------------------------------------------------------------
// Supports currently deletion of attachments and multipart structures
// which are represented as folders in Symbian store)
// ----------------------------------------------------------------------------
//
void CIpsPlgSosBasePlugin::RemoveChildPartL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& /*aMessageId*/,
    const TFSMailMsgId& /*aParentPartId*/,
    const TFSMailMsgId& /*aPartId*/)
	{
    // <qmail> not used any more
    User::Leave(KErrFSMailPluginNotSupported);
	}

// <qmail>
// ----------------------------------------------------------------------------
// Supports currently deletion of attachments and multipart structures
// which are represented as folders in Symbian store)
// ----------------------------------------------------------------------------
//
void CIpsPlgSosBasePlugin::RemoveChildPartL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& /* aParentPartId */,
    const TFSMailMsgId& aPartId,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId )
    {
    FUNC_LOG;
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewLC(*this);

    CIpsPlgRemoveChildPartOperation* op = CIpsPlgRemoveChildPartOperation::NewL(
        *iSession,
        watcher->iStatus,
        aMessageId,
        aPartId,
        aOperationObserver,
        aRequestId);
    watcher->SetOperation( op );

    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );

    }
// </qmail>

// ----------------------------------------------------------------------------
// The implementation supoorts the atachment and body parts at the moment.
// ----------------------------------------------------------------------------
//
CFSMailMessagePart* CIpsPlgSosBasePlugin::MessagePartL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aMessagePartId)
	{
    FUNC_LOG;
    CFSMailMessagePart* result( NULL );
    result = iMsgMapper->GetMessagePartL( aMessagePartId.Id(), aMailBoxId,
        aMessageId );
    return result;
	}

// ----------------------------------------------------------------------------
// The implementation bypass CImEmailMessage and its email's attachment
// manager. Instead, it calls Symbian framework classes directly by
// accessing the attachment entry.
// Qmail change: returns file handle for read/write
// ----------------------------------------------------------------------------
TInt CIpsPlgSosBasePlugin::GetMessagePartFileL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& /* aMessageId */,
    const TFSMailMsgId& aMessagePartId,
    RFile& aFileHandle,
    TBool aForWriting)
	{
    FUNC_LOG;
    TInt status( KErrNone );
    CMsvEntry* cEntry = iSession->GetEntryL( aMessagePartId.Id() );
    CleanupStack::PushL( cEntry );
    CMsvStore* store = NULL;
    TBool hasStore = cEntry->HasStoreL();
    if ( hasStore )
        {
//<qmail>
        // We need to open store for edit to support multipart/alternative
        // structure: we must have a possibility to modify text/html message part
        //store = cEntry->ReadStoreL();
        store = cEntry->EditStoreL();
//</qmail>
        }

    if ( !store || !hasStore )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PushL( store );
    MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();

    // It is assumed that the attachment file is always in the index 0
    if ( attachmentMgr.AttachmentCount() )
        {
//<qmail>
    if (aForWriting)
        {
        // We need to open store for edit to support multipart/alternative
        // structure: we must have a possibility to modify text/html message part
        aFileHandle = attachmentMgr.GetAttachmentFileForWriteL( 0 );
        }
    else
        {
        aFileHandle = attachmentMgr.GetAttachmentFileL( 0 );
        }
//</qmail>
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( store );
    CleanupStack::PopAndDestroy( cEntry );
	return status;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsPlgSosBasePlugin::CopyMessagePartFileL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& /*aMessageId*/,
    const TFSMailMsgId& aMessagePartId,
    const TDesC& aFilePath)
	{
    FUNC_LOG;
    //<qmail>
    CMsvEntry* cEntry = iSession->GetEntryL( aMessagePartId.Id() );
    CleanupStack::PushL( cEntry );
    CMsvStore* store = NULL;
    TBool hasStore = cEntry->HasStoreL();
    if ( hasStore )
        {
        store = cEntry->ReadStoreL();
        }
    if ( !store || !hasStore )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PushL( store );
    MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
    // It is assumed that the attachment file is always in the index 0
    if ( attachmentMgr.AttachmentCount() )
        {
        RFile attachmentFile =
            attachmentMgr.GetAttachmentFileL( 0 );
        CleanupClosePushL( attachmentFile );
        CFileMan* fileMgr = CFileMan::NewL( iSession->FileSession() );
        CleanupStack::PushL( fileMgr );
        User::LeaveIfError( fileMgr->Copy( attachmentFile, aFilePath ) );
        CleanupStack::PopAndDestroy( fileMgr );
        CleanupStack::PopAndDestroy(); // attachmentFile.Close()
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( store );
    CleanupStack::PopAndDestroy( cEntry );
    //</qmail>
	}

// ----------------------------------------------------------------------------
// The method supports only reading of the plain text body currently.
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::GetContentToBufferL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aMessagePartId,
    TDes& aBuffer,
    const TUint aStartOffset)
	{
    FUNC_LOG;
    CMsvEntry* cEntry( NULL );
    CImEmailMessage* message( NULL );

    // We trust that the message ID really refers to a message
    GetMessageEntryL( aMessageId.Id(), cEntry, message );
    if ( message )
        {
    message->GetBodyTextEntryIdL(
        cEntry->Entry().Id(), CImEmailMessage::EThisMessageOnly );

    if ( message->Selection().Count() > 0 )
        {
        // Check whether the body text is requested
        if ( message->Selection()[0] == aMessagePartId.Id() )
            {
            CParaFormatLayer* globalParaLayer = CParaFormatLayer::NewL();
            CleanupStack::PushL(globalParaLayer);
            CCharFormatLayer* globalCharLayer = CCharFormatLayer::NewL();
            CleanupStack::PushL(globalCharLayer);

            CRichText* bodyText = CRichText::NewL(
                globalParaLayer, globalCharLayer);
            CleanupStack::PushL( bodyText );

            message->GetBodyTextL(
                aMessageId.Id(), CImEmailMessage::EThisMessageOnly,
                *bodyText, *globalParaLayer, *globalCharLayer );

            bodyText->Extract( aBuffer, aStartOffset, aBuffer.MaxLength() );

            CleanupStack::PopAndDestroy(bodyText);
            CleanupStack::PopAndDestroy(globalCharLayer);
            CleanupStack::PopAndDestroy(globalParaLayer);
            }
        }
	}
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::SetContentL(
    const TDesC& aBuffer,
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& /* aMessageId */,
    const TFSMailMsgId& aMessagePartId )
	{
    FUNC_LOG;

//<qmail> Rewritten in Qmail: CIpsPlgOperationWait is no longer used
    // Notice that SetContentL sets only the content of text/plain message part:
    // text/html part can be modified directly using GetMessagePartFileL
	CMsvEntry* cEntry( NULL );
    CMsvStore* store = NULL;

    // following code should fill text/plain content
    cEntry = iSession->GetEntryL( aMessagePartId.Id() );
    CleanupStack::PushL( cEntry );
    TBool hasStore = cEntry->HasStoreL();
    if ( hasStore ) {
        store = cEntry->EditStoreL();
        CleanupStack::PushL(store);
        CParaFormatLayer* globalParaLayer = CParaFormatLayer::NewL();
        CleanupStack::PushL(globalParaLayer);
        CCharFormatLayer* globalCharLayer = CCharFormatLayer::NewL();
        CleanupStack::PushL(globalCharLayer);
        CRichText* text = CRichText::NewL( globalParaLayer, globalCharLayer );
        CleanupStack::PushL( text );
        // insert text
        text->InsertL(0, aBuffer );
        store->StoreBodyTextL(*text);
        store->CommitL();
        CleanupStack::PopAndDestroy( 4, store );
    }
    CleanupStack::PopAndDestroy(cEntry);
//</qmail>
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::RemovePartContentL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& /*aMessageId*/,
    const RArray<TFSMailMsgId>& /*aPartIds*/ )
    {
    // <qmail>
    User::Leave( KErrFSMailPluginNotSupported );
    // </qmail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::SetPartContentFromFileL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& /* aMessageId */,
    const TFSMailMsgId& /* aMessagePartId */,
    const TDesC& /* aFilePath */)
    {
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::StoreMessagePartL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& /* aParentFolderId */,
    const TFSMailMsgId& /* aMessageId */,
    CFSMailMessagePart& /* aMessagePart */)
    {
    }

//<qmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::StoreMessagePartsL(
    RPointerArray<CFSMailMessagePart>&  aMessageParts,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId )
    {
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
    CleanupStack::PushL(watcher);
    CIpsPlgMessagePartStorerOperation* op =
            CIpsPlgMessagePartStorerOperation::NewLC(*iSession,
                    watcher->iStatus, *this, aMessageParts,
                    aOperationObserver, aRequestId);

    watcher->SetOperation(op);
    iOperations.AppendL(watcher);
    CleanupStack::Pop( 2, watcher );
    }
//<//qmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::UnregisterRequestObserver( TInt /* aRequestId */)
    {
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::SendL(TFSMailMsgId aMessageId )
 	{
    FUNC_LOG;
 	CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
 	CleanupStack::PushL(watcher);
// <qmail>
    CIpsPlgSmtpOperation* op = CIpsPlgSmtpOperation::NewLC(
        *iSession, watcher->iStatus );
// </qmail>
    op->SetEventHandler(iEventHandler);
    watcher->SetOperation(op);
    CleanupStack::Pop( op ); // op added as member of watcher
    op->StartSendL( aMessageId.Id() );
    iOperations.AppendL(watcher);
    CleanupStack::Pop( watcher );
 	}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::SendMessageL( CFSMailMessage& /*aMessage*/ )
    {
    FUNC_LOG;
    // <qmail>
    User::Leave( KErrFSMailPluginNotSupported );
    // </qmail>
    }

// <qmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::SendMessageL(
        CFSMailMessage& aMessage,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId )
    {
    FUNC_LOG;
    // <qmail> Not activated yet
    // is EFSMsgFlag_CalendarMsg enabled,
    // then move send to back ground process
    //if ( aMessage.GetFlags() & EFSMsgFlag_CalendarMsg )
    //    {
    //    iEventHandler->SetNewPropertyEvent(
    //            aMessage.GetMessageId().Id(),
    //            KIPSSosSmtpEmptyOutboxNow, KErrNone );
    //    }
    //else
    //    {
        CIpsPlgSingleOpWatcher* watcher =
            CIpsPlgSingleOpWatcher::NewLC(*this);

        // <qmail> priority parameter has been removed
        CIpsPlgSmtpOperation* op = CIpsPlgSmtpOperation::NewL(
            *iSession,
            watcher->iStatus,
            &aOperationObserver,
            aRequestId );
        watcher->SetOperation(op); // ownership is transferred
        op->StartSendL( aMessage.GetMessageId().Id() );
        iOperations.AppendL(watcher);
        CleanupStack::Pop( watcher );
    //    }
    // </qmail>
    }
// </qmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSProgress CIpsPlgSosBasePlugin::StatusL( TInt aRequestId )
	{
    FUNC_LOG;
	TFSProgress status = TFSProgress();
	status.iError = KErrNone;
	status.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
	for ( TInt i = 0; i < iOperations.Count(); i++ )
        {
        const CIpsPlgBaseOperation* op = iOperations[i]->BaseOperation();
        if ( op && op->FSRequestId() == aRequestId )
            {
            status = op->GetFSProgressL();
            }
        }
	return status;
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::CancelL(TInt aRequestId)
	{
    FUNC_LOG;
	const TInt count = iOperations.Count();
    for ( TInt i = count-1; i >= 0; i-- )
        {
        const CIpsPlgBaseOperation* oper = iOperations[i]->BaseOperation();
        // all Fs operations are derived from ips base operation
        if ( oper && oper->FSRequestId() == aRequestId )
            {
            DeleteAndRemoveOperation( i, KErrCancel );
            break;
            }
        }
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::SearchL(
    const TFSMailMsgId& aMailBoxId,
    const RArray<TFSMailMsgId>& aFolderIds,
    const RPointerArray<TDesC>& aSearchStrings,
    const TFSMailSortCriteria& aSortCriteria,
    MFSMailBoxSearchObserver& aSearchObserver )
    {
    FUNC_LOG;
	iSearch->SearchL(
	    aMailBoxId,
	    aFolderIds,
	    aSearchStrings,
	    aSortCriteria,
	    aSearchObserver );
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::CancelSearch( const TFSMailMsgId& /* aMailBoxId */ )
	{
    FUNC_LOG;
	iSearch->Cancel();
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::ClearSearchResultCache(
    const TFSMailMsgId& /* aMailBoxId */ )
	{
    FUNC_LOG;
	iSearch->ClearCache();
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::AddObserverL(MFSMailEventObserver& aObserver)
	{
    FUNC_LOG;
	iEventHandler->AddPluginObserverL( &aObserver );
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::RemoveObserver(MFSMailEventObserver& aObserver)
	{
	//don't delete. we don't own this.
	iEventHandler->RemovePluginObserver( &aObserver );
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::DeleteMessagesByUidL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aFolderId*/,
	const RArray<TFSMailMsgId>& aMessages )
    {
    FUNC_LOG;
    CMsvEntrySelection* sel=new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);

    TInt count = aMessages.Count();
    TMsvEntry tEntry;
    TMsvId service;

    for(TInt i=0; i<count; i++)
        {
        iSession->GetEntry( aMessages[i].Id(), service, tEntry );
        //make sure that only messages get deleted.
        if( tEntry.iType == KUidMsvMessageEntry )
            {
			// <qmail>
            sel->AppendL( tEntry.Id() );
			// </qmail>
            }
        }

    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( watcher );
    //<qmail>
    CMsvOperation* op = CIpsPlgDeleteOperation::NewL( *iSession,
        watcher->iStatus, sel );
    //</qmail>
    watcher->SetOperation( op );
    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );
    //<qmail>
    CleanupStack::Pop( sel );
    //</qmail>
    }

// <qmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::DeleteMessagesByUidL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aFolderId*/,
	const RArray<TFSMailMsgId>& aMessages,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    {
    FUNC_LOG;
    CMsvEntrySelection* sel=new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);

    TInt count = aMessages.Count();
    TMsvEntry tEntry;
    TMsvId service;

    for(TInt i=0; i<count; i++)
        {
        iSession->GetEntry( aMessages[i].Id(), service, tEntry );
        //make sure that only messages get deleted.
        if( tEntry.iType == KUidMsvMessageEntry )
            {
            sel->AppendL( tEntry.Id() );
            }
        }

    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( watcher );
    CMsvOperation* op = CIpsPlgDeleteOperation::NewL( *iSession,
        watcher->iStatus, sel, aOperationObserver, aRequestId );
    watcher->SetOperation( op );
    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );
    CleanupStack::Pop( sel );
    }
// </qmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::SubscribeMailboxEventsL(
    const TFSMailMsgId& aMailboxId,
    MFSMailEventObserver& aObserver)
    {
    FUNC_LOG;
// <qmail>
    TUint32 key(0);
// </qmail>

    iEventHandler->SubscribeMailboxEventsL( aMailboxId, aObserver, key );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::UnsubscribeMailboxEvents(
    const TFSMailMsgId& aMailboxId,
    MFSMailEventObserver& aObserver)
    {
    FUNC_LOG;
    iEventHandler->UnsubscribeMailboxEvents( aMailboxId, aObserver );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TSSMailSyncState CIpsPlgSosBasePlugin::CurrentSyncState(
    const TFSMailMsgId& aMailBoxId )
    {
    FUNC_LOG;
    return iSyncStateHandler->GetCurrentSyncState( aMailBoxId );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsPlgSosBasePlugin::WizardDataAvailableL( )
    {
    FUNC_LOG;
    TInt error = KErrNone;
// <qmail> iSettingsApi not available in Qmail
    /*error = iSettingsApi->HandleMailboxCreation( MtmId(), *iSession );
    if ( error == KErrNotSupported )
        {
        // this means that wizard data is not meaned for this plugin (instance)
        // just return KErrNone at the moment
        return KErrNone;
        }*/
// </qmail>
    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsPlgSosBasePlugin::GetConnectionId(
        TFSMailMsgId /*aMailBoxId*/,
        TUint32& /*aConnectionId*/ )
    {
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsPlgSosBasePlugin::IsConnectionAllowedWhenRoaming(
             TFSMailMsgId /*aMailBoxId*/,
             TBool& /*aConnectionAllowed*/ )
    {
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// The method is not relevant for the Symbian based implementation
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::AuthenticateL(
    MFSMailRequestObserver& /* aOperationObserver */,
    TInt /* aRequestId */)
    {
    }

// ----------------------------------------------------------------------------
// method sets authentication popup data
// ----------------------------------------------------------------------------
//
void CIpsPlgSosBasePlugin::SetCredentialsL( const TFSMailMsgId& /*aMailBoxId*/,
	const TDesC& /*aUsername*/, const TDesC& /*aPassword*/ )
	{
// <qmail> not used; should be removed
//    FUNC_LOG;
//	TBool cancelled = EFalse;
//
//	if ( aPassword.Length() > 0 )
//	    {
//    	//Set new password and signal (possible) ongoing connect operation
//    	//CIpsSetDataApi* api = CIpsSetDataApi::NewL( *iSession );
//        //CleanupStack::PushL( api );
//
//        //CMsvEntry* cEntry = iSession->GetEntryL( aMailBoxId.Id() );
//        //CleanupStack::PushL( cEntry );
//
//        //api->SetNewPasswordL( *cEntry, aPassword );
//
//        //CleanupStack::PopAndDestroy( 2, api );//cEntry, api
//
//        //now signal through eventhandler that credientials have been set
//	    }
//	else
//	    {
//	    cancelled = ETrue;
//	    }
//	iEventHandler->SignalCredientialsSetL( aMailBoxId.Id(), cancelled );
// </qmail>
	}

// ----------------------------------------------------------------------------
// CIpsPlgSosBasePlugin::GetMessageEntryL( )
// Checks whether the requested message is already cached. If not, the cached
// objects are deleted and new objects are created.
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::GetMessageEntryL(
    TMsvId aId,
    CMsvEntry*& aMessageEntry,
    CImEmailMessage*& aImEmailMessage )
    {
    FUNC_LOG;
    if ( !iCachedEntry || ( aId != iCachedEntry->Entry().Id() ) ||
            iCachedEmailMessage->IsActive() )
        {
        CleanCachedMessageEntries();

        iCachedEntry = iSession->GetEntryL( aId );
        if ( iCachedEntry->Entry().iType == KUidMsvMessageEntry )
            {
            iCachedEmailMessage = CImEmailMessage::NewL( *iCachedEntry );
            }
        }
    aMessageEntry = iCachedEntry;
    aImEmailMessage = iCachedEmailMessage;
    }


// ----------------------------------------------------------------------------
// CIpsPlgSosBasePlugin::TakeMessageEntryL( )
// Takes ownership of the cached objects or creates new ones
// ----------------------------------------------------------------------------

void CIpsPlgSosBasePlugin::TakeMessageEntryLC(
    TMsvId aId,
    CMsvEntry*& aMessageEntry,
    CImEmailMessage*& aImEmailMessage )
    {
    FUNC_LOG;
    if ( !iCachedEntry || ( aId != iCachedEntry->Entry().Id() ) ||
            iCachedEmailMessage->IsActive() )
        {
        // Can't use the ones that are in cache, create new ones and don't replace the ones in cache
        aMessageEntry = iSession->GetEntryL( aId );
        aImEmailMessage = 0;

        if ( aMessageEntry->Entry().iType == KUidMsvMessageEntry )
            {
            CleanupStack::PushL( aMessageEntry );
            aImEmailMessage = CImEmailMessage::NewL( *aMessageEntry );
            CleanupStack::Pop();
            }
        }
    else
    	{
    	// Take ownership of the cached objects
    	aMessageEntry = iCachedEntry;
    	aImEmailMessage = iCachedEmailMessage;
    	iCachedEntry = 0;
    	iCachedEmailMessage = 0;
    	}

    // Ownership is transferred to the caller
    CleanupStack::PushL( aMessageEntry );
    CleanupStack::PushL( aImEmailMessage );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::ReturnMessageEntry(
        CMsvEntry* aMessageEntry,
        CImEmailMessage* aImEmailMessage )
	{
	// Clean old ones from the cache
	CleanCachedMessageEntries();

	// Always save the latest ones in the cache
	iCachedEntry = aMessageEntry;
	iCachedEmailMessage = aImEmailMessage;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsPlgSosBasePlugin::CleanCachedMessageEntries()
    {
    FUNC_LOG;
    delete iCachedEmailMessage;
    iCachedEmailMessage = NULL;
    delete iCachedEntry;
    iCachedEntry = NULL;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSFolderType CIpsPlgSosBasePlugin::GetFolderType(
    CMsvEntry* aEntry,
    TFSMailMsgId aFolderId )
	{
    FUNC_LOG;
    TFSFolderType folderType( EFSOther );

	if( ( aEntry->Entry().iDetails.CompareF( KIpsPlgInbox ) == 0 ) &&
		( aEntry->Entry().iType == KUidMsvFolderEntry ) )
        {
        folderType = EFSInbox;
        }
	else if( ( aEntry->Entry().iMtm == KSenduiMtmPop3Uid ) &&
	         ( aEntry->Entry().iType == KUidMsvServiceEntry ) &&
	         ( aEntry->Entry().iServiceId == aFolderId.Id() ) )
	    {
	    folderType = EFSInbox;
        }
    else if( aFolderId.Id() == KMsvGlobalOutBoxIndexEntryId )
        {
        folderType = EFSOutbox;
        }
    else if( aFolderId.Id() == KMsvDraftEntryId )
        {
        folderType = EFSDraftsFolder;
        }
    else if( aFolderId.Id() == KMsvSentEntryId )
        {
        folderType = EFSSentFolder;
        }
    else if( aFolderId.Id() == KMsvDeletedEntryFolderEntryId )
        {
        folderType = EFSDeleted;
        }
    else if( aFolderId.Id() == KMsvUnknownServiceIndexEntryId )
        {
        folderType = EFSOther;
        }
    return folderType;
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgTimerOperation& CIpsPlgSosBasePlugin::ActivityTimerL(
    const TFSMailMsgId& aMailBoxId )
    {
    FUNC_LOG;
    CIpsPlgTimerOperation* timer=NULL;
    for ( TInt i = 0; i < iActivitytimers.Count(); i++ )
        {
        if ( iActivitytimers[i]->FSMailboxId().Id() == aMailBoxId.Id() )
            {
            timer = iActivitytimers[i];
            }
        }

    if ( !timer )
        {
        // No timer for mailbox found create new
        timer = CIpsPlgTimerOperation::NewL( aMailBoxId, *this );
        CleanupStack::PushL( timer );
        User::LeaveIfError( iActivitytimers.Append( timer ) );
        CleanupStack::Pop( timer );
        }
    return *timer;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgSyncStateHandler& CIpsPlgSosBasePlugin::GetSyncStateHandler()
    {
    return *iSyncStateHandler;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::CancelAllOnlineOperations(
    const TFSMailMsgId& aMailboxId )
    {
    FUNC_LOG;
    // cancel all ongoing operations
    for ( TInt i = iOperations.Count()-1; i >= 0; i-- )
        {
        const CIpsPlgBaseOperation* baseOp = iOperations[i]->BaseOperation();
        if ( baseOp && baseOp->FSMailboxId() == aMailboxId )
            {
            delete iOperations[i];
            iOperations.Remove(i);
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::DeleteAndRemoveOperation(
        const TInt aOpArrayIndex, TInt aCompleteCode )
    {
    FUNC_LOG;
    CIpsPlgSingleOpWatcher* opWatcher = iOperations[aOpArrayIndex];

// <qmail> removed; does nothing
    // The operations matches, handle it in protocol plugin...if needed.
    //TRAP_IGNORE( HandleOpCompletedL( *opWatcher, aCompleteCode ) );
// </qmail>
    const CIpsPlgBaseOperation* op = opWatcher->BaseOperation();
    TMsvId service = KErrNotFound;
    TUint pluginId = PluginId();
    if ( op && (
            op->IpsOpType() == EIpsOpTypeImap4SyncOp  ||
             op->IpsOpType() == EIpsOpTypePop3SyncOp ||
             op->IpsOpType() == EIpsOpTypeImap4PopulateOp ) )
        {
        service = op->Service();
        }
    iOperations.Remove( aOpArrayIndex );
    delete opWatcher;
    opWatcher = NULL;
    // need to remove operation first because after signaling
    // sync complete mailbox status is asked immediatelly
    // and function checks connects ops also (see GetMailBoxStatus)
    if ( service != KErrNotFound )
        {
        iEventHandler->SetNewPropertyEvent(
                service, KIpsSosEmailSyncCompleted, aCompleteCode );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::FixReplyForwardHeaderL(
        CFSMailMessage* aMessage,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aOriginalMessageId,
        const TDesC& aHeaderDescriptor )
    {
    FUNC_LOG;
    CFSMailMessagePart* textBodyPart = aMessage->PlainTextBodyPartL();
    if ( textBodyPart )
        {
        CleanupStack::PushL( textBodyPart );
        CFSMailMessage* origMsg = GetMessageByUidL(
                                        aMailBoxId,
                                        TFSMailMsgId(),
                                        aOriginalMessageId,
                                        EFSMsgDataStructure );
        if ( origMsg )
            {
            CleanupStack::PushL( origMsg );
            CFSMailMessagePart* origMsgTextBodyPart =
                origMsg->PlainTextBodyPartL();
                if ( origMsgTextBodyPart )
                    {
                    CleanupStack::PushL( origMsgTextBodyPart );
                    // Use the content provided in aHeaderDescriptor
                    // instead of what is provided by
                    // CreateForwardSmtpMessage..
                    TPckgBuf<TReplyForwardParams> pckg;
                    pckg.Copy( aHeaderDescriptor );
                    TPtr hPtr( pckg().iHeader->Des() );
                    HBufC* body = HBufC::NewLC(
                            textBodyPart->FetchedContentSize() );
                    TPtr bPtr( body->Des() );
                    origMsgTextBodyPart->GetContentToBufferL( bPtr, 0 );
                    HBufC* content = HBufC::NewLC(
                            hPtr.Length() + bPtr.Length() );
                    TPtr cPtr( content->Des() );
                    cPtr.Append( hPtr );
                    cPtr.Append( bPtr );
                    textBodyPart->SetContent( cPtr );
                    textBodyPart->SaveL();
                    CleanupStack::PopAndDestroy( content );
                    CleanupStack::PopAndDestroy( body );
                    CleanupStack::PopAndDestroy( origMsgTextBodyPart );
                    }
            CleanupStack::PopAndDestroy( origMsg );
            }
        CleanupStack::PopAndDestroy( textBodyPart );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::DisconnectL(
    const TFSMailMsgId& aMailBoxId,
    MFSMailRequestObserver& aObserver,
    const TInt aRequestId,
    TBool /*aRemoveAccountAlso*/ )
    {
    FUNC_LOG;
    TMsvId service = aMailBoxId.Id();
    TMsvEntry tEntry;
    TMsvId serv;
    iSession->GetEntry( service, serv, tEntry );

    if ( tEntry.Connected() )
        {
        CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
        CleanupStack::PushL( watcher );

        TBuf8<1> dummyParams;
        dummyParams.Zero();
        CMsvEntrySelection* sel=new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL(sel);

        sel->AppendL( service );

        CIpsPlgBaseOperation* op = CIpsPlgDisconnectOp::NewL(
            *iSession,
            watcher->iStatus,
            service,
            ActivityTimerL( aMailBoxId ),
            aMailBoxId,
            &aObserver,
            aRequestId );

        watcher->SetOperation( op );
        CleanupStack::PopAndDestroy( sel );
        iOperations.AppendL( watcher );
        CleanupStack::Pop( watcher );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::HandleTimerFiredL( const TFSMailMsgId& aMailboxId )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iActivitytimers.Count(); i++ )
        {
        // do not disconnect automatically mailboxes that are set to
        // "connected"
        if ( iActivitytimers[i]->FSMailboxId().Id() == aMailboxId.Id() )
            {
            // 0 for request id
            DisconnectL( iActivitytimers[i]->FSMailboxId(), *this, 0 );
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::RequestResponseL(
    TFSProgress /*aEvent*/,
    TInt /*aRequestId*/ )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::EmptyOutboxL( const TFSMailMsgId& aMailBoxId )
    {
    FUNC_LOG;
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
 	CleanupStack::PushL(watcher);
// <qmail>
    CIpsPlgSmtpOperation* op = CIpsPlgSmtpOperation::NewLC( *iSession, watcher->iStatus );
// </qmail>
    op->SetEventHandler(iEventHandler);
    watcher->SetOperation(op);
    op->EmptyOutboxFromPendingMessagesL( aMailBoxId.Id() );
    iOperations.AppendL(watcher);
    CleanupStack::Pop( 2, watcher );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgSosBasePlugin::CanConnectL( const TFSMailMsgId& /*aMailboxId*/,
    TInt& /*aReason*/ )
    {
    FUNC_LOG;
#ifdef __WINS__
    return ETrue;
#endif
    TBool ret=ETrue;
    //check offline mode. If set, we can't connect.
    ret = !OfflineModeSetL();
    if( ret )
        {
        ret = RoamingCheckL();
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgSosBasePlugin::IsUnderUiProcess()
    {
    return iIsUnderUiProcess;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgSosBasePlugin::OfflineModeSetL()
    {
    FUNC_LOG;
    return !LocalFeatureL( KCRUidCoreApplicationUIs,
        KCoreAppUIsNetworkConnectionAllowed, 1 );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgSosBasePlugin::LocalFeatureL(
    const TUid& aCenRepUid,
    const TUint32 aKeyId,
    const TUint32 aFlag )
    {
    FUNC_LOG;
    // Create the connection to Central Repository
    CRepository* cenrep = CRepository::NewLC( aCenRepUid );

    // Fetch the value from the
    TInt flags(0);
    if ( cenrep )
        {
        cenrep->Get( aKeyId, flags );
        }

    // Remove the connection
    CleanupStack::PopAndDestroy( cenrep );
    cenrep = NULL;

    // Return the result as a boolean value
    return ( flags & aFlag ) == aFlag;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgSosBasePlugin::RoamingCheckL()
    {
    FUNC_LOG;
    //first check our registration status
    TInt regStatus = RegistrationStatusL();

    if ( regStatus == ENetworkRegistrationHomeNetwork )
        {
        return ETrue;
        }
    else if ( regStatus == ENetworkRegistrationRoaming )
        {
        //we're roaming. by default no auto connect when roaming.
        return EFalse;
        }
    else
        {
        //other reg status states indicate that we cannot connect
        //in any case.
        //any different handling needed here?
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TInt CIpsPlgSosBasePlugin::RegistrationStatusL()
    {
    FUNC_LOG;
    TRequestStatus status;
    TInt registrationStatus( 0 );

    //check network status
    iConMon.ConnectL();

    iConMon.GetIntAttribute(
        EBearerIdGSM, 0, KNetworkRegistration,
        registrationStatus, status );

    User::WaitForRequest( status ); // faulty CS warning

    iConMon.Close();

    return registrationStatus;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgSosBasePlugin::BlockCopyMoveFromFoldersL(
    CMsvEntry* aFolderEntry,
    TFSMailMsgId aFolderId,
    CFSMailFolder& aFSMailFolder )
    {
    FUNC_LOG;
    // Currently IPS plugin can connect automatically when doing the actual
    // move/copy operation, so no need to block offline moves separately
    RArray<TFSFolderType> blockFolders;
    CleanupClosePushL( blockFolders );

    // Move/Copy operations only between remote folders (+ IMAP Inbox) and
    // Outbox -> Drafts, block others.
    blockFolders.Reset();

    if( ( GetFolderType( aFolderEntry, aFolderId ) != EFSOther ) &&
        ( GetFolderType( aFolderEntry, aFolderId ) != EFSInbox ) )
        {
        blockFolders.Append( EFSInbox );
        blockFolders.Append( EFSOther );
        }

    if( GetFolderType( aFolderEntry, aFolderId ) != EFSDraftsFolder )
        {
        blockFolders.Append( EFSOutbox );
        }
    blockFolders.Append( EFSSentFolder );
    blockFolders.Append( EFSDraftsFolder );
    blockFolders.Append( EFSDeleted );

    // Block move/copy to this folder from blocklist, same blocklist
    // applies to both online and offline moves/copies
    aFSMailFolder.BlockCopyFromL( blockFolders, EFSMailBoxOnline );
    aFSMailFolder.BlockMoveFromL( blockFolders, EFSMailBoxOnline );
    aFSMailFolder.BlockCopyFromL( blockFolders, EFSMailBoxOffline );
    aFSMailFolder.BlockMoveFromL( blockFolders, EFSMailBoxOffline );

    CleanupStack::PopAndDestroy( &blockFolders );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::StoreIMEIToMailboxL( const TMsvId aMailboxId )
    {
    FUNC_LOG;
    CMsvEntry* centry = iSession->GetEntryL( aMailboxId );
    CleanupStack::PushL( centry );

    TMsvEntry tentry = centry->Entry();
    tentry.iDescription.Set( iIMEI );
    centry->ChangeL( tentry );

    CleanupStack::PopAndDestroy( centry );
    centry = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgSosBasePlugin::ConnOpRunning( const TFSMailMsgId& aMailBoxId  )
    {
    FUNC_LOG;
    return iSyncStateHandler->ConnOpRunning( aMailBoxId );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::SetMailboxName(
        const TFSMailMsgId& aMailboxId,
        const TDesC& /*aMailboxName*/ )
    {
    FUNC_LOG;
    TMsvEntry tEntry;
    TMsvId service;
    iSession->GetEntry( aMailboxId.Id(), service, tEntry );
// <qmail> iSettingsApi removed
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TUid CIpsPlgSosBasePlugin::MtmId() const
    {
    FUNC_LOG;
    TUid ret = KSenduiMtmImap4Uid;
    if ( iFSPluginId == KIpsPlgPop3PluginUidValue )
        {
        ret = KSenduiMtmPop3Uid;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TUint CIpsPlgSosBasePlugin::PluginId() const
    {
    return iFSPluginId;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSosBasePlugin::DeleteActivityTimer( const TFSMailMsgId& aMailboxId  )
    {
    FUNC_LOG;
    TInt timerCount = iActivitytimers.Count();
    for ( TInt j = 0; j < timerCount; j++ )
        {
        if ( iActivitytimers[j]->FSMailboxId() == aMailboxId )
            {
            delete iActivitytimers[j];
            iActivitytimers[j] = NULL;
            iActivitytimers.Remove( j );
            timerCount--;
            j--;
            }
        }
    }

// <qmail> new function
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgSosBasePlugin::HasOperations( const TFSMailMsgId& aMailboxId )
    {
    FUNC_LOG;
    TBool ret( EFalse );
    for ( TInt i = 0; i < iOperations.Count(); i++ )
        {
        if( iOperations[i]->BaseOperation() &&
            iOperations[i]->BaseOperation()->FSMailboxId() == aMailboxId )
            {
            ret = ETrue;
            }
        }
    return ret;
    }
//</Qmail>
//<Qmail>
// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::HandleActiveFolderChangeL
// ---------------------------------------------------------------------------
//
void CIpsPlgSosBasePlugin::HandleActiveFolderChangeL(
        const TFSMailMsgId& aActiveMailboxId,
        const TFSMailMsgId& aActiveFolderId)
    {
    TMsvId service;
    TMsvEntry folder;
    iSession->GetEntry( aActiveFolderId.Id(), service, folder );


    //currently, no actions unless this is inbox
    //also, if id is '0', it means inbox before first sync...it doesn't really exist yet
    if( folder.iDetails.CompareF( KIpsPlgInbox ) == 0 || folder.Id() == 0 ||
            ( folder.iMtm == KSenduiMtmPop3Uid ) &&
            ( folder.iType == KUidMsvServiceEntry ) &&
            ( folder.iServiceId == aActiveFolderId.Id() ) )
        {
        //folder is inbox
        if ( iSyncStateHandler->GetMailboxIpsState( aActiveMailboxId.Id() )
                    == KIpsSosEmailSyncStarted )
            {
            //we won't do anything if sync is already started
            return;
            }

        //check are we in polling mode
        NmIpsSosExtendedSettingsManager* eMgr=
                new NmIpsSosExtendedSettingsManager(aActiveMailboxId.Id());

        QVariant value;
        bool ok = eMgr->readSetting(IpsServices::ReceptionActiveProfile, value);
        delete eMgr;

        if ( ok )
            {
            TInt profile = value.toInt();
            if ( profile != IpsServices::EmailSyncProfileManualFetch )
                {
                // let's sync
                GoOnlineL(aActiveMailboxId);
                }
            }
        }
    }
// <qmail>
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TUid CIpsPlgSosBasePlugin::Id( ) const
    {
    return TUid::Uid( PluginId() );
    }
// </qmail>
