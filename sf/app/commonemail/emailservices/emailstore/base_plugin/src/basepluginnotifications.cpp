/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Email interface notification systems implementation.
*
*/



//<cmail>
#include "MsgStore.h"
#include "MsgStoreFolder.h"
//</cmail>

#include "BasePlugin.h"
#include "baseplugincommonutils.h"


/**
 *
 */
EXPORT_C void CBasePlugin::SubscribeMailboxEventsL(
    const TFSMailMsgId& aMailboxId,
    MFSMailEventObserver& aObserver )

	{
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailboxId.Id() );

    if ( KErrNotFound == mailBox.iObservers.Find( &aObserver ) )
        {
        mailBox.iObservers.AppendL( &aObserver );
        }
	}


/**
 *
 */
EXPORT_C void CBasePlugin::UnsubscribeMailboxEvents(
    const TFSMailMsgId& aMailboxId,
    MFSMailEventObserver& aObserver )

	{
	TRAP_IGNORE( UnsubscribeMailboxEventsL( aMailboxId, aObserver ) );
	}


/**
 *
 */
EXPORT_C void CBasePlugin::AddObserverL(
    MFSMailEventObserver& aObserver )
    {
    
    if ( KErrNotFound == iObservers.Find( &aObserver ) )
        {
        iObservers.AppendL( &aObserver );
        }

    if ( iObservers.Count() == 1 )
        {
        iMsgStore->AddObserverL( this );
        }
    }


/**
 *
 */
EXPORT_C void CBasePlugin::RemoveObserver(
    MFSMailEventObserver& aObserver )
    {

    TInt idx = iObservers.Find( &aObserver );
    
    if ( KErrNotFound != idx )
        {
        iObservers.Remove( idx );
        }
        
    if ( iObservers.Count() == 0 )
        {
        TRAP_IGNORE( iMsgStore->RemoveObserverL( this ) );
        }
    }


/**
 *
 */
EXPORT_C void CBasePlugin::UnregisterRequestObserver(
    TInt /*aRequestId*/ )
    {
    
    }


#pragma mark -
#pragma mark --- MSGSTORE OBSERVERS ---


/**
 *
 */
EXPORT_C void CBasePlugin::SystemEventNotify( TMsgStoreSystemEvent aEvent )
	{
	TRAP_IGNORE( SystemEventNotifyL( aEvent ) );
	}


/**
 *
 */
void CBasePlugin::SystemEventNotifyL( TMsgStoreSystemEvent aEvent )
    
    {
    switch ( aEvent )
        {
        // The message store is unavailable.  This may be due to 
        // a backup or restore being in progress, a Pointsec lock being issued,
        // the disk drive being unavailable, etc.
        case EMsgStoreUnavailable:
            {
            // since the store is unavailabe we are unable to get 
            // list of mailboxes, setting plugin id only
            TFSMailMsgId id;
            id.SetPluginId( TUid::Uid( GetPluginId() ) );
            NotifyGlobalEventL( TFSMailboxUnavailable, id );
            }
        break;

        // The message store is available again.
        case EMsgStoreAvailable:
            {
            RArray<TFSMailMsgId> mailboxes;
            CleanupClosePushL( mailboxes );
            ListMailBoxesL( mailboxes );

            for ( TInt i( 0 ); i < mailboxes.Count(); i++ )
                {
                NotifyGlobalEventL( TFSMailboxAvailable, mailboxes[i] );
                }

            CleanupStack::PopAndDestroy( &mailboxes );
            }
        break;

        // The observer event queue has overflowed and has been reset, so events have been lost.  This
        // shouldn't happen unless the client thread has been starved for an extended period of time, during
        // which many message store operations have occurred.
        case EObserverEventQueueOverflow:
        //don't see anything meaningful to do here.
        default:
        break;
        };
    }


/**
 *
 */   
EXPORT_C void CBasePlugin::AccountEventNotify(
    TMsgStoreAccountEvent aEvent,
	TInt32 aOwnerId,
	const TDesC& aName,
	const TDesC& aNewName,
	TMsgStoreId aMailboxId )
	
	{
	if ( GetPluginId() == aOwnerId )
	    {
	    TRAP_IGNORE( AccountEventNotifyL( aEvent, aOwnerId, aName, aNewName, aMailboxId ) );
	    }
	}


/**
 *
 */   
void CBasePlugin::AccountEventNotifyL(
    TMsgStoreAccountEvent aEvent,
	TInt32 /*aOwnerId*/,
	const TDesC& /*aName*/,
	const TDesC& /*aNewName*/,
	TMsgStoreId aMailboxId )

    {
    switch ( aEvent )
        {
        case EMsgStoreAccountCreated:
            {
            TFSMailMsgId mailBox( GetPluginId(), aMailboxId );
            NotifyGlobalEventL( TFSEventNewMailbox, mailBox );
            }
        break;

        case EMsgStoreAccountDeleted:
            {
            //remove from the cache.
            if ( KErrNotFound != iMailboxes.Find( aMailboxId ) )
                {
                iMailboxes.RemoveL( aMailboxId );
                }

            TFSMailMsgId mailBox( GetPluginId(), aMailboxId );
            
            HandleMailboxDeleteL( mailBox );
            
            NotifyGlobalEventL( TFSEventMailboxDeleted, mailBox );
            }
        break;

        case EMsgStoreAccountRenamed:
            {
            }
        break;
        };   
    }


EXPORT_C void CBasePlugin::HandleMailboxDeleteL( const TFSMailMsgId& /*aMailboxId*/ )
    {
    //nothing to do
    }

/**
 *
 */
EXPORT_C void CBasePlugin::ModificationNotify(
    TMsgStoreId aMailBoxId,
    TMsgStoreOperation aOperation,
    TMsgStoreContainerType aType,
    TUint32 aFlags,
    TMsgStoreId aId,
    TMsgStoreId aParentId,
    TMsgStoreId aOtherId )

    {
    TRAP_IGNORE( ModificationNotifyL( aMailBoxId, aOperation, aType, aFlags, aId, aParentId, aOtherId ) );
    }


/**
 *
 */
void CBasePlugin::ModificationNotifyL(
    TMsgStoreId aMailBoxId,
    TMsgStoreOperation aOperation,
    TMsgStoreContainerType aType,
    TUint32 /*aFlags*/,
    TMsgStoreId aId,
    TMsgStoreId aParentId,
    TMsgStoreId aOtherId )

    {
    __LOG_ENTER( "ModificationNotifyL" );
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId );
    
    __LOG_WRITE_FORMAT1_INFO( "aMailBoxId = 0x%X.", aMailBoxId );
    __LOG_WRITE_FORMAT1_INFO( "aId = 0x%X.", aId );
    __LOG_WRITE_FORMAT1_INFO( "aParentId = 0x%X.", aParentId );
    
    InvalidateCacheIfNecessary( aId, aParentId, aOtherId );
    
    if ( mailBox.iObservers.Count() > 0 )
        {
        switch ( aOperation )
            {
            
            case EMsgStoreAdd:
                {
                __LOG_WRITE_INFO( "EMsgStoreAdd." );
                if ( EMsgStoreMessageContainer == aType )
                    {
                    NotifyEventL( aMailBoxId, aId, aParentId, TFSEventNewMail );
                    }
                else if ( EMsgStoreFolderContainer == aType )
                    {
                    NotifyEventL( aMailBoxId, aId, aParentId, TFSEventNewFolder );
                    }
                }
            break;

            case EMsgStoreDelete:
                {
                __LOG_WRITE_INFO( "EMsgStoreDelete." );
                if ( EMsgStoreMessageContainer == aType )
                    {
                    NotifyEventL( aMailBoxId, aId, aParentId, TFSEventMailDeleted );
                    }
                else if ( EMsgStoreFolderContainer == aType )
                    {
                    for ( TInt i = EFSInbox; i <= EFSDeleted; i++ )
                        {
                        if ( mailBox.iRootFolders.iFolders[i] == aId )
                            {
                            TMsgStoreId newId = 0;
                            
                            /**@ this behavior is forced by the UI for some reason and should be revisited
                             * in the future as it allows more than one inbox folder which is not recommended. */
                            
                            //loop over all of the root folders looking for a replacement.
                            RPointerArray<CMsgStoreFolder> folders;
                            CleanupResetAndDestroyClosePushL( folders );

                            mailBox().FoldersL( aMailBoxId, folders );

                            for ( int j = 0; j < folders.Count(); j++ )
                                {
                                CMsgStoreFolder* folder = folders[j];

                                TUint index = 0;
                                if ( folder->FindProperty( KMsgStorePropertyFolderType, index ) )
                                    {
                                    TUint32 type = folder->PropertyValueUint32L( index );

                                    if ( type == i )	//i is a value in the folder type enum.
                                        {
                                        newId = folder->Id();
                                        break;
                                        }
                                    }
                                }
                            
                            CleanupStack::PopAndDestroy( &folders );
                            
                            //set the new root folder info.
                            mailBox.iRootFolders.iFolders[i] = newId;
                            break;
                            }                    		
                        }
                    NotifyEventL( aMailBoxId, aId, aParentId, TFSEventFoldersDeleted );
                    }
                }
            break;
            
            case EMsgStoreMove:
                {
                __LOG_WRITE_INFO( "EMsgStoreMove." );
                NotifyEventL( aMailBoxId, aId, aOtherId, TFSEventMailMoved, aParentId );
                break;
                }

            case EMsgStoreUpdateProperties:
                {
                __LOG_WRITE_INFO( "EMsgStoreUpdateProperties." );
                if ( EMsgStoreMessageContainer == aType )
                    {
                    NotifyEventL( aMailBoxId, aId, aParentId, TFSEventMailChanged );
                    }
                else if ( EMsgStoreFolderContainer == aType )
                    {
                    NotifyEventL( aMailBoxId, aId, aParentId, TFSEventFolderChanged );
                    }
                else if ( EMsgStoreMailboxContainer == aType )
                    {
                    TBool mailBoxNameHasChanged( EFalse );
                    TRAP_IGNORE( RefreshCachedMailBoxDisplayNameL( mailBoxNameHasChanged, aMailBoxId ) );
                    
                    if ( mailBoxNameHasChanged )
                        {
                        NotifyEventL( aMailBoxId, aId, aParentId, TFSEventMailboxRenamed );
                        }
                    }
                }
            break;

            case EMsgStoreRemoveContent:
                break;

            default:
                break;
            }
        }
        /* Reporting request status moved from above if-statement here. Now, request status is reported even there is no
         * mailbox observers.
         */ 
        switch ( aOperation )
            {
            case EMsgStoreUpdateProperties:
                {
                if ( EMsgStorePartContainer == aType )
                    {
                    ReportRequestStatusL( aMailBoxId, aOtherId, aParentId, aId );
                    }
                }
                break;
            /**@ check whether the ui removes the observer - then the observer must be kept
            until there are pending reqs ?*/
            //attachment download request handling.
            case EMsgStoreUpdateContent:
                {
                __LOG_WRITE_INFO( "EMsgStoreUpdateContent." );
                ReportRequestStatusL( aMailBoxId, aOtherId, aParentId, aId, ETrue );
                }
                break;
                
            default:
                break;
        }
    
        
        __LOG_EXIT;
    } //ModificationNotifyL.


/**
 *
 */
void CBasePlugin::NotifyEventL(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aId,
    TMsgStoreId aParentId,
    TFSMailEvent aFsEvent,
    TMsgStoreId aOtherId /*=KMsgStoreInvalidId*/ )

    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId );
    
    TFSMailMsgId mailBoxId( GetPluginId(), aMailBoxId );
    TFSMailMsgId parent( GetPluginId(), aParentId );
    TFSMailMsgId other( GetPluginId(), aOtherId );

    TFSMailMsgId* param3 = NULL;
    if ( KMsgStoreInvalidId != aOtherId )
    	{
    	param3 = &other;
    	}

    RArray<TFSMailMsgId> entries;
    CleanupClosePushL( entries );

    TFSMailMsgId entry( GetPluginId(), aId );
    entries.AppendL( entry );

    TInt count = mailBox.iObservers.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        mailBox.iObservers[i]->EventL( aFsEvent, mailBoxId, &entries, &parent, param3 );
        }

    CleanupStack::PopAndDestroy( &entries );
    }


/**
 *
 */
void CBasePlugin::NotifyGlobalEventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailBox /*=TFSMailMsgId()*/,   //defaults to a null id.
    TAny* aParam1 /*=NULL*/,
    TAny* aParam2 /*=NULL*/,
    TAny* aParam3 /*=NULL*/ )
    
    {
    TInt count = iObservers.Count();
    
    for ( TInt i = 0; i < count; i++ )
        {
        iObservers[i]->EventL( aEvent, aMailBox, aParam1, aParam2, aParam3 );
        }
    }

/**
 *
 */
EXPORT_C void CBasePlugin::NotifyMailboxEventL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailBox,
        TAny* aParam1 /*=NULL*/,
        TAny* aParam2 /*=NULL*/,
        TAny* aParam3 /*=NULL*/ )
    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBox.Id() );
    
    for ( TInt i = 0; i < mailBox.iObservers.Count(); i++ )
        {
        mailBox.iObservers[i]->EventL( aEvent, aMailBox, aParam1, aParam2, aParam3 );
        }
    }


/**
 *
 */
EXPORT_C /*protected virtual*/ void CBasePlugin::ReportRequestStatusL(
    TMsgStoreId aMailBox,
    TMsgStoreId aOtherId,
    TMsgStoreId aMsgId,
    TMsgStoreId aPartId,
    TBool aCanReportCompletion /*=EFalse*/ )

    {
    __LOG_ENTER( "ReportRequestStatusL" );

    __LOG_WRITE_FORMAT1_INFO( "aMailBox : 0x%X.", aMailBox );
    __LOG_WRITE_FORMAT1_INFO( "aMsgId : 0x%X.", aMsgId );
    __LOG_WRITE_FORMAT1_INFO( "aPartId : 0x%X.", aPartId );

    TOngoingFetchInfo fetchInfo;
    if ( FindFetchRequestL( aMailBox, aOtherId, aMsgId, aPartId, fetchInfo ) )
        {
        __LOG_WRITE_INFO( "Request match found." );

        CMailboxInfo& mailBox = GetMailboxInfoL( aMailBox );

        //get the fetched and the server sizes.                    
        CMsgStoreMessage* msg = mailBox().FetchMessageL(
        	fetchInfo.iMessageId, KMsgStoreInvalidId );
        CleanupStack::PushL( msg );
        CMsgStoreMessagePart* part = msg->ChildPartL( aPartId, ETrue );
        CleanupStack::PushL( part );

        TUint32 size = 0;
        TUint idx = 0;
        if ( part->FindProperty( KMsgStorePropertySize, idx ) )
            {
            size = part->PropertyValueUint32L( idx );
            }
        
        TUint32 fetchedSize = 0;
        idx = 0;
        if ( part->FindProperty( KMsgStorePropertyRetrievedSize, idx ) )
            {
            fetchedSize = part->PropertyValueUint32L( idx );
            }
        
        CleanupStack::PopAndDestroy( part );
        CleanupStack::PopAndDestroy( msg );

//<qmail>
        TFSProgress progress = TFSProgress();
//</qmail>
        progress.iError = KErrNone;
        progress.iMaxCount = size;
        progress.iCounter = fetchedSize;
        
        __LOG_WRITE_FORMAT2_INFO(
            "Size: %d, fetched size: %d.", size, fetchedSize );
            
        if ( fetchedSize < size )
            {
            __LOG_WRITE_INFO( "Download in progress, about to notify the UI." );

            progress.iProgressStatus = TFSProgress::EFSStatus_Status;
            fetchInfo.iRequest->iObserver.RequestResponseL(
            	progress, fetchInfo.iRequest->iRequestId );
            }
        else if ( aCanReportCompletion )
            {
            if ( DeleteFetchRequestForPart( fetchInfo ) )
            	{
            	//no more parts left for downloading thus completed.
                progress.iProgressStatus
					= TFSProgress::EFSStatus_RequestComplete;                        
                __LOG_WRITE_INFO( "Download completed for all parts." );

                fetchInfo.iRequest->iObserver.RequestResponseL(
                	progress, fetchInfo.iRequest->iRequestId );
                DeleteFetchRequest( fetchInfo );
            	}
            else
            	{
                progress.iProgressStatus
					= TFSProgress::EFSStatus_Status;
                __LOG_WRITE_INFO( "A part download completed, more is left." );

                fetchInfo.iRequest->iObserver.RequestResponseL(
                	progress, fetchInfo.iRequest->iRequestId );
            	}
            }
        }
        
    __LOG_EXIT;
    }


/**
 * @return ETrue in case there are no more parts left for downloading.
 */
EXPORT_C /*protected*/ TBool CBasePlugin::DeleteFetchRequestForPart(
	TOngoingFetchInfo& aFetchInfo )
	{
    //remove the downloaded part from the request object.                        
	aFetchInfo.iRequest->iParts.Remove( aFetchInfo.iPartIndex );

	return 0 == aFetchInfo.iRequest->iParts.Count();
	}


/**
 * 
 */
EXPORT_C /*protected*/ void CBasePlugin::DeleteFetchRequest(
	TOngoingFetchInfo& aFetchInfo )
	{
	iReqs.Remove( aFetchInfo.iRequestIndex );
	delete aFetchInfo.iRequest;
	}


/**
 * 
 */
EXPORT_C TBool CBasePlugin::FindFetchRequestL(
	TMsgStoreId aMailBox,
	TMsgStoreId aOtherId,
	TMsgStoreId aMsgId,
	TMsgStoreId aPartId,
	CBasePlugin::TOngoingFetchInfo& aOngoingFetchInfo )
	{
	
	for ( TInt count = iReqs.Count(); aOngoingFetchInfo.iRequestIndex < count;
		aOngoingFetchInfo.iRequestIndex++ )
		{
		TFSMailMsgId fsId( GetPluginId(), aPartId );
		CFetchRequester* request = iReqs[aOngoingFetchInfo.iRequestIndex];
		
		//a request might specify more than one part for download, find the
		//index of the currently processed part.
		TInt partsCount = request->iParts.Count();
		for ( TInt i = 0; i < partsCount; i++ )
			{
			if ( fsId == request->iParts[i] )
				{
				aOngoingFetchInfo.iPartIndex = i;
				break;
				}
			}
	
		//there are MsgStore notification that receive the message id in the
		//"other" id, determine for sure the id of the message this part belongs
		//to. 
		if ( aMsgId == request->iMessageId.Id() )
			{
			aOngoingFetchInfo.iMessageId = aMsgId;
			}
		else if ( aOtherId 
			== request->iMessageId.Id() )
			{
			aOngoingFetchInfo.iMessageId = aOtherId;
			}
	
		//final check before choosing the download request object that
		//identifies this part download request.
		if ( aMailBox == request->iMailBoxId.Id()
			&& KMsgStoreInvalidId != aOngoingFetchInfo.iMessageId 
			&& KErrNotFound != aOngoingFetchInfo.iPartIndex )
			{
			aOngoingFetchInfo.iRequest = request;
			break;
			}
		}
	
	return NULL != aOngoingFetchInfo.iRequest;
	} //FindRequestL.


/**
 *
 */
void CBasePlugin::UnsubscribeMailboxEventsL(
    const TFSMailMsgId& aMailboxId,
    MFSMailEventObserver& aObserver )

	{
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailboxId.Id() );

    TInt idx = mailBox.iObservers.Find( &aObserver );
    
    if ( KErrNotFound != idx )
        {
        mailBox.iObservers.Remove( idx );
        }
	}
