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
* Description:  Message store session implementation.
*
*/



// ========
// INCLUDES
// ========

#include "MsgStoreTypes.h"
#include "MsgStorePropertyKeys.h"

#include "MessageStoreSession.h"
#include "MessageStoreServer.h"
#include "ContainerStore.h"
#include "PropertiesSerializer.h"
#include "ContainerStoreMRUAddressTable.h"
#include "bufferedmessagecreator.h"


// =========
// CONSTANTS
// =========

// The maximum number of observer events that will be queued up for a single session.
const TInt KMsgStoreMaximumEventQueueDepth = 100;
const TInt KInitialAddressBufSize          = 256;
const TInt KSortedArrayGranularity         = 100;

const TUint KValThree = 3;
const TUint KGranularity25 = 25;
const TUint KGranularity15 = 15;

// ======================
// CLASS CSearchResult
// ======================
// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CSearchResult* CSearchResult::NewL( TContainerId aMessageId, TContainerId aFolderId, const TDesC8& aPropertyBuf )
    {
    CSearchResult* self = new(ELeave) CSearchResult( aMessageId, aFolderId );
    CleanupStack::PushL( self );
    self->ConstructL( aPropertyBuf );
    CleanupStack::Pop( self );
    return self;
    }

CSearchResult* CSearchResult::SearchCompleteResultL()
    {
    return NewL( KContainerInvalidId, KContainerInvalidId, KNullDesC8 );
    }

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CSearchResult::CSearchResult( TContainerId aMessageId, TContainerId aFolderId )
    : iMessageId( aMessageId ), iFolderId( aFolderId )
    {
    }

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CSearchResult::ConstructL( const TDesC8& aPropertyBuf )
    {
    iPropertyBuf.CreateL( aPropertyBuf );
    }

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CSearchResult::~CSearchResult()
    {
    iPropertyBuf.Close();
    }
        

// ==========================
// CLASS CMessageStoreSession
// ==========================
// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMessageStoreSession::CMessageStoreSession(CMessageStoreServer& aMessageStoreServer) : 
	iServer(aMessageStoreServer),
	iIds( KGranularity25 ),
	iEventQueue( KGranularity15 ),
    iSearchHandler( NULL )
    {
  	__LOG_CONSTRUCT( "msg", "CMessageStoreSession" )
  	}

// ==========================================================================
// FUNCTION: CreateL
// ==========================================================================
void CMessageStoreSession::CreateL()
    {
    __LOG_ENTER( "CreateL" )
    
    iServer.AddSessionL( this );

    // This could leave if a session is created while the store is unavailable.
    TRAPD( err, iServer.MessageStoreL().ObserveL( this ) );
    
    // We need to leave the Session active in the case of KErrInUse.  This is
    // because if the observer is dropped during KErrInUse, they cannot
    // re-establish connection, and they will not know when MS in no longer busy.
    // As a result, they will retry for many seconds to connect, then permanently
    // give up.  This is avoided by trapping KErrInUse.
    // Thus, only dropSession and Leave for other errors
    
    if( err != KErrNone && err != KErrInUse )
    {
        iServer.DropSession( this );
        User::Leave( err );
    }
    
    __LOG_EXIT
    } // end CreateL

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMessageStoreSession::~CMessageStoreSession()	
	{
    delete iBufferedMessageCreator;
    
	iIds.Reset();
	
    iMruAddressArray.ResetAndDestroy();
    
	iPropertyNames.ResetAndDestroy();
    
	iMatchMessages.ResetAndDestroy();
    
    iSearchStrings.ResetAndDestroy();
	
	delete iSearchHandler;
	
	TRAP_IGNORE( iServer.MessageStoreL().StopObserving( this ) );
	
	//close all the sort iterators when the client exits/crashes
	for ( TInt i = 0 ; i < iSortSessionIds.Count(); i++  )
	    {
	    TRAP_IGNORE( iServer.MessageStoreL().EndSortingL( iSortSessionIds[i] ) );
	    }
	iSortSessionIds.Close();

	iEventQueue.Close();

	if( !iObserverMessage.IsNull() )
		{
		iObserverMessage.Complete( KErrCancel );
		} // end if

	if( !iGetMatchesMessage.IsNull() )
		{
		iGetMatchesMessage.Complete( KErrCancel );
		} // end if

	iServer.DropSession( this );

	__LOG_DESTRUCT
	} // end destructor

// ==========================================================================
// FUNCTION:ServiceL
// ==========================================================================
void CMessageStoreSession::ServiceL(const RMessage2& aMessage)
  {
  __LOG_ENTER_SUPPRESS( "ServiceL" )
  __LOG_WRITE8_FORMAT1_INFO( "%d", aMessage.Function() )

  // Call the appropriate message handler function, based on the message function ID.
  switch( aMessage.Function() )
    {
	case EMsgStoreCmdAuthenticated:
		DoAuthenticatedL( aMessage );
		break;
        
	case EMsgStoreCmdAuthenticate:
		DoAuthenticateL( aMessage );
		break;
        
	case EMsgStoreCmdClearAuthentication:
		DoClearAuthenticationL( aMessage );
		break;
        
	case EMsgStoreCmdHasPassword:
		DoHasPasswordL( aMessage );
		break;
        
	case EMsgStoreCmdSetPassword:
		DoSetPasswordL( aMessage );
		break;
        
	case EMsgStoreCmdChangePassword:
		DoChangePasswordL( aMessage );
		break;
        
    case EMsgStoreCmdEnableEncryption:
        DoEnableEncryptionL( aMessage );
        break;
        
    case EMsgStoreCmdDisableEncryption:
        DoDisableEncryptionL( aMessage );
        break;
        
	case EMsgStoreCmdWipeEverything:
		DoWipeEverythingL( aMessage );
		break;
        
    case EMsgStoreCmdStartObserving:
		DoStartObserving( aMessage );
		break;
        
    case EMsgStoreCmdGetEvents:
		DoGetEventsL( aMessage );
		break;
        
    case EMsgStoreCmdStopObserving:	
		DoStopObserving( aMessage );
		break;
		
    case EMsgStoreCmdCreateAccount:	
    	DoCreateAccountL( aMessage );
    	break;
    	
    case EMsgStoreCmdOpenAccount:	
		DoOpenAccountL( aMessage );
		break;
		
    case EMsgStoreCmdDeleteAccount:	
		DoDeleteAccountL( aMessage );
		break;
		
    case EMsgStoreCmdRenameAccount:	
		DoRenameAccountL( aMessage );
		break;
		
    case EMsgStoreCmdListAccounts:	
		DoListAccountsL( aMessage );
		break;
    
    case EMsgStoreCmdCreateContainer:
		DoCreateContainerL( aMessage );
		break;
        
    case EMsgStoreCmdCommitContainer:
		DoCommitContainerL( aMessage );
		break;
        
    case EMsgStoreCmdAbandonContainer:
		DoAbandonContainerL( aMessage );
		break;
        
    case EMsgStoreCmdMoveContainer:
		DoMoveContainerL( aMessage );
		break;
        
    case EMsgStoreCmdCopyContainer:
		DoCopyContainerL( aMessage );
		break;
        
	case EMsgStoreCmdDeleteContainer:
		DoDeleteContainerL( aMessage );
		break;
        
	case EMsgStoreCmdChildrenCounts:
		DoChildrenCountsL( aMessage );
		break;
        
	case EMsgStoreCmdTotalCounts:
		DoTotalCountsL( aMessage );
		break;
        
    case EMsgStoreCmdChildrenIds:
		DoChildrenIdsL( aMessage );
		break;
        
    case EMsgStoreCmdPropertiesList:
		DoPropertiesListL( aMessage );
		break;
        
	case EMsgStoreCmdFetchProperties:
		DoFetchPropertiesL( aMessage );
		break;
        
	case EMsgStoreCmdUpdateProperties:
		DoUpdatePropertiesL( aMessage );
		break;
        
	case EMsgStoreCmdUpdateProperty:
		DoUpdatePropertyL( aMessage );
		break;
        
    case EMsgStoreCmdContentLength:
		DoContentLengthL( aMessage );
		break;
        
    case EMsgStoreCmdFetchContentToBuffer:
		DoFetchContentToBufferL( aMessage );
		break;
        
    case EMsgStoreCmdFetchContentToFile:
		DoFetchContentToFileL( aMessage );
		break;
        
    case EMsgStoreCmdReplaceContentWithBuffer:
		DoReplaceContentWithBufferL( aMessage );
		break;
        
    case EMsgStoreCmdReplaceContentWithFile:
		DoReplaceContentWithFileL( aMessage );
		break;
        
    case EMsgStoreCmdAppendToContent:
        DoAppendToContentL( aMessage );
		break;
        
    case EMsgStoreCmdPrependToContent:
        DoPrependToContentL( aMessage );
        break;

    case EMsgStoreCmdRemoveContent:
		DoRemoveContentL( aMessage );
		break;
        
    case EMsgStoreCmdOpenContentFile:
        DoOpenContentFileL( aMessage );
        break;
        
    case EMsgStoreCmdPrepareSearch:
		DoPrepareSearchL( aMessage );
		break;
        
    case EMsgStoreCmdGetMatches:
		DoGetMatches( aMessage );
		break;
        
    case EMsgStoreCmdCancelSearch:
		DoCancelSearch( aMessage );
		break;
        
    case EMsgStoreCmdClearSearchResultCache:
        DoClearSearchResultCache( aMessage );
        break;
        
    case EMsgStoreCmdStartSorting:
        DoStartSortingL( aMessage );
        break;
        
    case EMsgStoreCmdEndSorting:
        DoEndSortingL( aMessage );
        break;
        
    case EMsgStoreCmdGetSortedRows:
        DoGetSortedRowsL( aMessage );
        break;
        
    case EMsgStoreCmdGetIteratorGroupCount:
        DoGetIteratorGroupCountL( aMessage );
        break;

    case EMsgStoreCmdGetIteratorIdsAndFlags:
        DoGetSortedIdsAndFlagsL( aMessage );
        break;
    
    case EMsgStoreCmdSortedIndexOf:
        DoGetSortedIndexOfL( aMessage );
        break;

    case EMsgStoreCmdGetIteratorIds:
        DoGetSortedIdsL( aMessage );
        break;
        
    case EMsgStoreCmdGetIteratorIdsAndGroupCount:
        DoGetSortedIdsAndGroupCountL( aMessage );
        break;
        
    case EMsgStoreCmdSetMaxMruAddressCount:
        DoSetMaxMruAdressCountL( aMessage );
        break;
        
    case EMsgStoreCmdSetMruAddressList:
        DoSetMruAddressListL( aMessage );
        break;
        
    case EMsgStoreCmdGetMruAddressList:
        DoGetMruAddressListL( aMessage );
        break;
        
    case EMsgStoreCmdBeginBatchInsert:
        DoBeginBatchInsertL( aMessage );
        break;

    case EMsgStoreCmdFinishBatchInsert:
        DoFinishBatchInsertL( aMessage );
        break;

#ifdef _DEBUG
        
    case EMsgStoreCmdInjectBackupRestoreEvent:
        DoInjectBackupRestoreEventL( aMessage );
        break;
    case EMsgStoreCmdInjectPointSecEvent:
        DoInjectPointSecEvent( aMessage );
        break;
    case EMsgStoreCmdSimulateLowDiskSpace:
        DoSimulateLowDiskSpaceL( aMessage );
        break;
    case EMsgStoreCmdGetEncryptionState:
        DoGetEncryptionStateL( aMessage );
        break;
    case EMsgStoreCmdShutdownServer:
        DoShutdownServer( aMessage );
        break;
    
#endif  
        
    default:
      __LOG_WRITE_ERROR( "Illegal function!" )
      PanicClient( aMessage, EPanicIllegalFunction );
      break;
    } // end switch
       
  __LOG_EXIT    
  } // end ServiceL

// ==========================================================================
// FUNCTION: ServiceError
// ==========================================================================
void CMessageStoreSession::ServiceError(const RMessage2& aMessage, TInt aError)
  {
  __LOG_ENTER_SUPPRESS( "ServiceError" )
  __LOG_WRITE8_FORMAT1_ERROR( "ServiceError %d", aError)
  CSession2::ServiceError( aMessage, aError );
  } // end ServiceError

// ==========================================================================
// FUNCTION: ContainerStoreUnavailable
// ==========================================================================
void CMessageStoreSession::ContainerStoreUnavailable()
	{
    // Cancel any ongoing searches.
    delete iSearchHandler;
    iSearchHandler = NULL;
    iMatchMessages.ResetAndDestroy();    
    
	TRAP_IGNORE( iServer.MessageStoreL().StopObserving( this ) );
	} // end ContainerStoreUnavailable

// ==========================================================================
// FUNCTION: ContainerStoreAvailable
// ==========================================================================
void CMessageStoreSession::ContainerStoreAvailable()
	{
	TRAP_IGNORE( iServer.MessageStoreL().ObserveL( this ) );
	} // end ContainerStoreAvailable

// ----------------------
// OBSERVER FUNCTIONALITY
// ----------------------

// ==========================================================================
// FUNCTION: DoStartObserving
// ==========================================================================
void CMessageStoreSession::DoStartObserving( const RMessage2& aMessage )
	{
	iIsObserving = ETrue;
	
	aMessage.Complete( KErrNone );
	} // end DoStartObserving
	
// ==========================================================================
// FUNCTION: DoStopObserving
// ==========================================================================
void CMessageStoreSession::DoStopObserving( const RMessage2& aMessage )
	{
	iIsObserving = EFalse;
	
	iEventQueue.Reset();
	
	if( !iObserverMessage.IsNull() )
		{
		iObserverMessage.Complete( KErrCancel );
		} // end if
		
	aMessage.Complete( KErrNone );
	} // end DoStopObserving
	
// ==========================================================================
// FUNCTION: DoCreateAccountL
// ==========================================================================
void CMessageStoreSession::DoCreateAccountL( const RMessage2& aMessage )
	{
	TInt32 ownerId = aMessage.Int1();

    RBuf name;    
    CleanupClosePushL( name );
    name.CreateL( aMessage.GetDesLengthL( 2 ) );
    
	aMessage.ReadL( 2, name );
	
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );
	propertiesBuf.CreateL( aMessage.GetDesLengthL( KValThree ) );
	aMessage.ReadL( KValThree, propertiesBuf );
	
	TContainerId mbId = iServer.MessageStoreL().CreateAccountL( ownerId, name, this, propertiesBuf );
	
    TPckg<TContainerId> mbIdPckg( mbId );
	aMessage.WriteL( 0, mbIdPckg );
	
	CleanupStack::PopAndDestroy( &propertiesBuf );
	CleanupStack::PopAndDestroy( &name );
	
    aMessage.Complete( KErrNone );
	}

// ==========================================================================
// FUNCTION: DoOpenAccountL
// ==========================================================================
void CMessageStoreSession::DoOpenAccountL( const RMessage2& aMessage )
	{
	__LOG_ENTER_SUPPRESS("DoOpenAccountL")
	
	//parameter 0 - result
	TUint result;
	TPckg<TUint> resultPckg( result );
	
    //parameter 1 - params
	TMsgStoreOpenAccountCmdParams params;
    TPckg<TMsgStoreOpenAccountCmdParams> paramsPckg( params );
	aMessage.ReadL( 1, paramsPckg );
	
	TInt32 ownerId = params.iOwnerId;
	
	//parameter 2 - account anme
    RBuf name;    
    CleanupClosePushL( name );
    name.CreateL( aMessage.GetDesLengthL( 2 ) );
	aMessage.ReadL( 2, name );
	
	//parameter 3 - property buf
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );
	
	params.iMailBoxId = iServer.MessageStoreL().OpenAccountL( ownerId, name, propertiesBuf );
	
	if( propertiesBuf.Length() > aMessage.GetDesMaxLengthL( KValThree ) )
	    {
	    // Overflow!
	    __LOG_WRITE8_FORMAT1_ERROR( "property buffer too long (%i)", propertiesBuf.Length() );
	    
	    // Write the required length to the result parameter, to indicate to the client that the
	    // buffer must be increased to the given size.
	    result = propertiesBuf.Length();
	    aMessage.WriteL( 0, resultPckg );	    
	    }
	else
	    {	    	
        result = 0;
	    aMessage.WriteL( 0, resultPckg );
	    
 	    aMessage.WriteL( 1, paramsPckg );

        aMessage.WriteL( KValThree, propertiesBuf );

	    } // end if
	
	CleanupStack::PopAndDestroy( &propertiesBuf );
	CleanupStack::PopAndDestroy( &name );
	
    aMessage.Complete( KErrNone );
	}

// ==========================================================================
// FUNCTION: DoRenameAccountL
// ==========================================================================
void CMessageStoreSession::DoRenameAccountL( const RMessage2& aMessage )
	{
	TInt32 ownerId = aMessage.Int0();

    RBuf oldName;    
    CleanupClosePushL( oldName );
    oldName.CreateL( aMessage.GetDesLengthL( 1 ) );
	aMessage.ReadL( 1, oldName );
	
    RBuf newName;    
    CleanupClosePushL( newName );
    newName.CreateL( aMessage.GetDesLengthL( 2 ) );
	aMessage.ReadL( 2, newName );
	
	iServer.MessageStoreL().RenameAccountL( ownerId, oldName, newName, this );
	
	CleanupStack::PopAndDestroy( &newName );
	CleanupStack::PopAndDestroy( &oldName );
	
    aMessage.Complete( KErrNone );
	}

// ==========================================================================
// FUNCTION: DoDeleteAccountL
// ==========================================================================
void CMessageStoreSession::DoDeleteAccountL( const RMessage2& aMessage )
	{
	TInt32 ownerId = aMessage.Int0();

    RBuf name;    
    CleanupClosePushL( name );
    name.CreateL( aMessage.GetDesLengthL( 1 ) );
	aMessage.ReadL( 1, name );
	
	iServer.MessageStoreL().DeleteAccountL( ownerId, name, this );
	
	CleanupStack::PopAndDestroy( &name );
	
    aMessage.Complete( KErrNone );
	}

// ==========================================================================
// FUNCTION: DoListAccountsL
// ==========================================================================
void CMessageStoreSession::DoListAccountsL( const RMessage2& aMessage )
	{
    RBuf8 resultBuf;
    CleanupClosePushL( resultBuf );
    resultBuf.CreateL( aMessage.GetDesMaxLength(0) );
    
    iServer.MessageStoreL().ListAccountsL( resultBuf );
    
    aMessage.WriteL( 0, resultBuf );
    
	CleanupStack::PopAndDestroy( &resultBuf );
	
    aMessage.Complete( KErrNone );
	}

// ==========================================================================
// FUNCTION: DoGetEventsL
// ==========================================================================
void CMessageStoreSession::DoGetEventsL( const RMessage2& aMessage )
	{
	// If there are any queued events then send them right away, otherwise
	// save the message until events become avaialable.
	if( iEventQueue.Count() > 0 )
		{
        TUint desLength = aMessage.GetDesMaxLengthL( 0 );
        
        TUint position = 0;
        
        while( iEventQueue.Count() > 0 && position < desLength )
            {
            TPckg<TMsgStoreEvent> eventPckg( iEventQueue[0] );
			aMessage.WriteL( 0, eventPckg, position );
            
            position += sizeof( TMsgStoreEvent );
            iEventQueue.Remove( 0 );		
            } // end while
            
        aMessage.Complete( KErrNone );
	    }
	else
	    {
	    iObserverMessage = aMessage;
		} // end if	
		
	} // end DoGetEventsL
	
// ==========================================================================
// FUNCTION: SendEventToObserver
// ==========================================================================
void CMessageStoreSession::SendEventToObserver( TMsgStoreEvent &aEvent )
	{
	__LOG_ENTER( "SendEventToObserver" )
	
	// If there is no observer message pending then queue up the message,
	// otherwise send it immediately.
	
	if( iObserverMessage.IsNull() )
		{
		if( iEventQueue.Count() > KMsgStoreMaximumEventQueueDepth )
		    {
		    __LOG_WRITE_ERROR( "EVENT QUEUE OVERFLOW" );
		    iEventQueue.Reset();
		    
        	TMsgStoreEvent event;
        	event.iType        = EObserverEventQueueOverflow;
        	event.iId          = KMsgStoreInvalidId;
        	event.iParentId    = KMsgStoreInvalidId;
        	event.iOtherId     = KMsgStoreInvalidId;
        	event.iFlags       = KMsgStoreFlagsNotFound;

        	// return value can be ignored because queueing is very rare
        	// case anyway and appending T-class into RArray should succeed
    		TInt ignore = iEventQueue.Append( event );
		    }
		else
		    {
    		__LOG_WRITE_INFO( "event queued" )
            // return value can be ignored because queueing is very rare
            // case anyway and appending T-class into RArray should succeed
            TInt ignore = iEventQueue.Append( aEvent );
		    } // end if		
		}
	else
		{
		__LOG_WRITE_INFO( "Sending event now" )
		
		TPckg<TMsgStoreEvent> eventPckg( aEvent );
					
		TRAPD( result, iObserverMessage.WriteL( 0, eventPckg ) );			
		
		iObserverMessage.Complete( result );
		} // end if	
		
    __LOG_EXIT		
	} // end SendEventToObserver

// ==========================================================================
// FUNCTION: ContainerModified
// ==========================================================================
void CMessageStoreSession::ContainerModified( 
        TOperation aOperation, 
        const RArray<TContainerId>& aHierarchy, 
        TContainerId aNewParentId, 
        const TDesC8& aQuickProperties )
	{
	// If the children IDs array is not empty then a series of EMsgStoreCmdChildrenProperties
	// messages are in progress, and a modified container could affect the data being streamed.
	
	if( iIds.Count() > 0 )
	    {
        // The following event types could affect the properties that are being streamed to the client.
        // Reset the ID list to indicate that the streaming operation should be restarted.
        	
	    if ( ( iUsingQuery ) && ( aHierarchy.Count() > 1 ) ) 
	        {	        
	        // If this is a query based list then the current change only affects the list if the
	        // query was for all types or if the type match.
	        if( (iQueryParams.iType == KContainerTypeAny) || (iQueryParams.iType == (aHierarchy[0] & KContainerTypeMask )) )
	            {	            
            	switch( aOperation )
            		{
            		case EAdd:
            		case EMove:
             		case EDelete:
            		case EUpdateProperties:
            
                        // The current change only affects the list if the query was recursive or
                        // if the modified container has the same parent as used in the query.
            		    if( iQueryParams.iRecursive || (iQueryParams.iId == aHierarchy[1]) )
            		        {
            		        iIds.Reset();
            		        } // end if
                        break;
                            		        
            		} // end switch
            		
	            } // end if
	        }
	    else
	        {	        
   	        // Only deletes and property updates can affect the provided list of IDs.  Adds and
	        // moves do not affect this operation.
        	switch( aOperation )
        		{
         		case EDelete:
        		case EUpdateProperties:
        		
                    iIds.Reset();
                    break;
                        		        
           		} // end switch
           		
	        } // end if
    		
	    } // end if	    
	
	// If the client session is currently observing then create an observer event.
	
	if( iIsObserving )
	    {	    
    	TMsgStoreEvent event;

    	switch( aOperation )
    		{
    		case EAdd:
    		    event.iType = EMsgStoreAdd;
    			break;
    		case EMove:
    		    event.iType = EMsgStoreMove;
    			break;
    		case EDelete:
    			event.iType = EMsgStoreDelete;
    			break;
    		case EUpdateProperties:
    			event.iType = EMsgStoreUpdateProperties;
    			break;
    		case EUpdateContent:
    			event.iType = EMsgStoreUpdateContent;
    			break;
    		case ERemoveContent:
    			event.iType = EMsgStoreRemoveContent;
    			break;
    		default:
    		    // This shouldn't happen.
    		    event.iType = 0;
    		    break;    		    
    		} // end switch
    		
    	event.iId = aHierarchy[0];
    	
    	if( aHierarchy.Count() > 1 )
    	    {    	    
    	    event.iParentId = aHierarchy[1];
    	    }
    	else
    	    {
    	    event.iParentId = KContainerInvalidId;
    	    } // end if
    	        	
	    if( aNewParentId != KContainerInvalidId )
	        {
	        // Use the new parent Id for the "other" id.
	        event.iOtherId = aNewParentId;	        
	        }
	    else if( aHierarchy.Count() > 2 )
	        {
	        // Use the grandparent ID for the "other" ID.
	        event.iOtherId = aHierarchy[2];
	        }
	    else
	        {
	        event.iOtherId = KContainerInvalidId;
	        } // end if
	    
	    //Get the MailBoxId from the buttom of the Hierarchy
	    if ( aHierarchy.Count() > 1 )
	    	{
	    	event.iMailBoxId = aHierarchy[ aHierarchy.Count() - 2 ];
	    	}
	    else
	    	{
	    	//this should not happen
	    	event.iMailBoxId = KContainerInvalidId;
	    	}
    	        	
        // Find the value of the flags property.

    	event.iFlags = KMsgStoreFlagsNotFound;
        if( aQuickProperties.Length() > 0 )
            {
            TPropertiesDeserializer deserializer( aQuickProperties );
            
            if( deserializer.Find( KMsgStorePropertyFlags ) && deserializer.Type() == EMsgStoreTypeUint32 )
                {
    			TPckgBuf<TUint32> flagsPckg;
    			flagsPckg.Copy( deserializer.Value() );

                event.iFlags = flagsPckg();                    
    			} // end if

            } // end if
        
        //initialize the names which are not used
        event.iAccountName.SetLength( 0 );
        event.iNewAccountName.SetLength( 0 );

    	SendEventToObserver( event );    	
    	
	    } // end if
	    
	} // end ContainerModified
	
// ==========================================================================
// FUNCTION: AuthenticationChanged
// ==========================================================================
void CMessageStoreSession::AuthenticationChanged( TBool aAuthenticated )
    {
    if( iIsObserving )
        {            
    	TMsgStoreEvent event;

    	if( aAuthenticated )
    	    {
    	    event.iType = EMsgStoreAuthenticated;
    	    }
    	else
    	    {
    	    event.iType = EMsgStoreNotAuthenticated;
    	    } // end if

    	event.iMailBoxId   = KMsgStoreInvalidId;
    	event.iId          = KMsgStoreInvalidId;
    	event.iParentId    = KMsgStoreInvalidId;
    	event.iOtherId     = KMsgStoreInvalidId;
    	event.iFlags       = KMsgStoreFlagsNotFound;
    	
        //initialize the names which are not used
        event.iAccountName.SetLength( 0 );
        event.iNewAccountName.SetLength( 0 );
    	
    	SendEventToObserver( event );
        } // end if
        
    } // end AuthenticationChanged

// ==========================================================================
// FUNCTION: AccountModified
// ==========================================================================
void CMessageStoreSession::AccountModified( TOperation aOperation, TInt32 aOwnerId, const TDesC8& aName, const TDesC8& aNewName, TContainerId aMailboxId )
	{
    if( iIsObserving )
        {            
    	TMsgStoreEvent event;
    	
    	switch( aOperation )
		{
		case EAdd:
		    event.iType = EMsgStoreAccountCreated;
			break;
		case EDelete:
			event.iType = EMsgStoreAccountDeleted;
			break;
		case ERenameAccount:
			event.iType = EMsgStoreAccountRenamed;
			break;
		default:
		    // This shouldn't happen.
		    event.iType = 0;
		    break;    		    
		} // end switch

    	event.iMailBoxId   = aMailboxId;
    	event.iId          = aOwnerId;
    	event.iParentId    = KMsgStoreInvalidId;
    	event.iOtherId     = KMsgStoreInvalidId;
    	event.iFlags       = KMsgStoreFlagsNotFound;
    	
        //initialize the names which are not used
        event.iAccountName.Copy( aName );
        event.iNewAccountName.Copy( aNewName );
    	
    	SendEventToObserver( event );
        } // end if
	
	}


// --------------------    	
// SEARCH FUNCTIONALITY
// -------------------- 
    
// ==========================================================================
// FUNCTION: DoPrepareSearchL
// ==========================================================================
void CMessageStoreSession::DoPrepareSearchL( const RMessage2& aMessage )
    {
    //param 0 (search cmd param)
    TMsgStoreSearchCmdParams params;   
    TPckg<TMsgStoreSearchCmdParams> paramsPckg( params );
    aMessage.ReadL( 0, paramsPckg );

    //param 1 (search string)
    ReadString16ArrayL( aMessage, 1, iSearchStrings );
    
    //param 2 (folder IDs)
    ReadIdArrayL( aMessage, 2, iIds );

    //param 3 (property keys)
    // Deserialize the property names list.
    ReadString8ArrayL( aMessage, KValThree, iPropertyNames );
    
	iMatchMessages.ResetAndDestroy();
    
    if ( iSearchHandler )
        {
        if ( iSearchHandler->IsSameSearchCriteriaL( iSearchStrings ) )
            {
            iSearchHandler->RestartL( params, iIds, iPropertyNames );
            }
        else
            {
            delete iSearchHandler;
            iSearchHandler = NULL;
            }
        }
    
    if ( !iSearchHandler )
        {
        // Create an asynchronous search handler and start it.
        iSearchHandler = iServer.MessageStoreL().SearchL( EMsgStoreMessageBits, params, iSearchStrings, iIds, iPropertyNames, *this );        
    	iSearchHandler->StartL();
        }
	
    aMessage.Complete( KErrNone );
    } // end DoPrepareSearchL
    
// ==========================================================================
// FUNCTION: DoGetMatchesL
// ==========================================================================
void CMessageStoreSession::DoGetMatches( const RMessage2& aMessage )
    {
    // TIpcArgs( &aMatchBuffer )
    
    iGetMatchesMessage = aMessage;
    
    // Always send the search results if the search has completed, even if there are not results found.
    // The end of the search is indicated by a null search handler pointer.
    TBool searchInProgress = EFalse;
    if ( iSearchHandler )
        {
        searchInProgress = iSearchHandler->IsSearchInProgress();
        }
    
    SendMatchesToClient( !searchInProgress );        
    }
    
// ==========================================================================
// FUNCTION: DoCancelSearch
// ==========================================================================
void CMessageStoreSession::DoCancelSearch( const RMessage2& aMessage )
    {
    iMatchMessages.ResetAndDestroy();
    
    if( iSearchHandler )
        {     
        //do not delete the search handler
        iSearchHandler->CancelSearch();
        } // end if
        
    if( !iGetMatchesMessage.IsNull() )
        {
        iGetMatchesMessage.Complete( KErrCancel );
        } // end if
    
    iMatchMessages.ResetAndDestroy();
    	
	aMessage.Complete( KErrNone );
    } // end DoCancelSearch

// ==========================================================================
// FUNCTION: DoClearSearchResultCache
// ==========================================================================
void CMessageStoreSession::DoClearSearchResultCache( const RMessage2& aMessage )
    {
    delete iSearchHandler;
    iSearchHandler = NULL;
    
    aMessage.Complete( KErrNone );
    }

// ==========================================================================
// FUNCTION: MatchFound
// ==========================================================================
void CMessageStoreSession::MatchFound( TContainerId aMessageId, TContainerId aFolderId, const TDesC8& aPropertyBuf )
    {
    TRAP_IGNORE( DoMatchFoundL( aMessageId, aFolderId, aPropertyBuf ) );
    } // end MatchFound

// ==========================================================================
// FUNCTION: DoMatchFoundL
// ==========================================================================
void CMessageStoreSession::DoMatchFoundL( TContainerId aMessageId, TContainerId aFolderId, const TDesC8& aPropertyBuf )
    {
    CSearchResult* result = CSearchResult::NewL( aMessageId, aFolderId, aPropertyBuf );
    iMatchMessages.AppendL( result );

    SendMatchesToClient();
    } // end MatchFound

// ==========================================================================
// FUNCTION: SearchComplete
// ==========================================================================
void CMessageStoreSession::SearchComplete( TInt /*aStatus*/ )
    {
    //do not delete the search handler
    
    // Always send the search results if the search has completed, even if there are not results found.
    SendMatchesToClient( ETrue );
    } // end SearchComplete
    
// ==========================================================================
// FUNCTION: SendMatchesToClientL
// ==========================================================================
void CMessageStoreSession::SendMatchesToClient( TBool aAlwaysSend )
    {
    // If there is a pending GetMatches message and either aAlwaysSend is true or matches were found
    // then build a nuffer of matching IDs and send to the client.
    
    if( !iGetMatchesMessage.IsNull() && (aAlwaysSend || iMatchMessages.Count() > 0 ) )
        {
        TRAPD( result, DoWriteMatchesToMessageL() );
        iGetMatchesMessage.Complete( result );
        } // end if

    } // end SendMatchesToClient
  
  
// ==========================================================================
// FUNCTION: DoWriteMatchesToMessageL
// ==========================================================================
void CMessageStoreSession::DoWriteMatchesToMessageL()
    {
    __LOG_ENTER_SUPPRESS( "DoWriteMatchesToMessageL" )
    
    TUint desLength = iGetMatchesMessage.GetDesMaxLengthL( 0 );        
    TUint offset = 0;
    
    TUint fixedLength = sizeof(TContainerId)*2 + sizeof(TUint);  //fixed size: 2 ids and a length
    
    TInt msgWritten = 0;
    
    while( iMatchMessages.Count() > 0 && offset < desLength )
        {
        CSearchResult* result = iMatchMessages[0];
        TUint length = result->PropertyBuf().Length();
	    TPckgBuf<TUint> lengthPkg( length );
        
        if ( desLength - offset >= fixedLength + length )
            {
            //write the message id
            TPckgBuf<TContainerId> idPkg( result->MessageId() );
            iGetMatchesMessage.WriteL( 0, idPkg, offset );
            offset += sizeof( TContainerId );
            
            //write the folder id
            TPckgBuf<TContainerId> folderIddPkg( result->FolderId() );
            iGetMatchesMessage.WriteL( 0, folderIddPkg, offset );
            offset += sizeof( TContainerId );

            //write the length
            iGetMatchesMessage.WriteL( 0, lengthPkg, offset );
            offset += sizeof( TUint );
            
            if ( length > 0 )
                {
                iGetMatchesMessage.WriteL( 0, result->PropertyBuf(), offset );
                offset += length;
                }
            delete result;
            iMatchMessages.Remove( 0 );
            msgWritten++;
            }
        else
            {
            if ( msgWritten == 0 )
                {
                __LOG_WRITE_ERROR("ERR: Client buffer size too small, not enough for even 1 message")
                User::Leave( KErrOverflow );
                }
            break;
            }
        } // end while
        
    } // end DoWriteMatchesToMessageL

// -------------------------
// MESSAGE HANDLER FUNCTIONS
// -------------------------

// ==========================================================================
// FUNCTION: DoAuthenticatedL
// ==========================================================================
void CMessageStoreSession::DoAuthenticatedL( const RMessage2& aMessage )
    {
    // TIpcArgs( &resultPckg )
    
    TBool authenticated = iServer.MessageStoreL().Authenticated();

    TPckg<TBool> authenticatedPckg( authenticated );
	aMessage.WriteL( 0, authenticatedPckg );
	
	aMessage.Complete( KErrNone );    
    } // end DoAuthenticatedL
    
// ==========================================================================
// FUNCTION: DoAuthenticateL
// ==========================================================================
void CMessageStoreSession::DoAuthenticateL( const RMessage2& aMessage )
    {
    // TIpcArgs( &resultPckg, &aPassword )
    
	RBuf password;
	CleanupClosePushL( password );
	password.CreateL( aMessage.GetDesLengthL( 1 ) );
	aMessage.ReadL( 1, password );
    
    TBool result = iServer.MessageStoreL().AuthenticateL( password );

    CleanupStack::PopAndDestroy( &password );

    TPckg<TBool> resultPckg( result );
	aMessage.WriteL( 0, resultPckg );

	aMessage.Complete( KErrNone );    
    } // end DoAuthenticateL
    
// ==========================================================================
// FUNCTION: DoClearAuthenticationL
// ==========================================================================
void CMessageStoreSession::DoClearAuthenticationL( const RMessage2& aMessage )
    {
    // No arguments
    
    iServer.MessageStoreL().ClearAuthentication();
    
	aMessage.Complete( KErrNone );    
    } // end DoClearAuthenticationL
    
// ==========================================================================
// FUNCTION: DoHasPasswordL
// ==========================================================================
void CMessageStoreSession::DoHasPasswordL( const RMessage2& aMessage )
    {
    // TIpcArgs( &resultPckg )
    
    TBool result = iServer.MessageStoreL().HasPasswordL();

    TPckg<TBool> resultPckg( result );
	aMessage.WriteL( 0, resultPckg );
	
	aMessage.Complete( KErrNone );    
    } // end DoHasPasswordL
    
// ==========================================================================
// FUNCTION: DoSetPasswordL
// ==========================================================================
void CMessageStoreSession::DoSetPasswordL( const RMessage2& aMessage )
    {
    // TIpcArgs( &aPassword )
    
	RBuf password;
	CleanupClosePushL( password );
	password.CreateL( aMessage.GetDesLengthL( 0 ) );
	aMessage.ReadL( 0, password );
    
    iServer.MessageStoreL().SetPasswordL( password );

    CleanupStack::PopAndDestroy( &password );

	aMessage.Complete( KErrNone );    
    } // end DoSetPasswordL

// ==========================================================================
// FUNCTION: DoChangePasswordL
// ==========================================================================
void CMessageStoreSession::DoChangePasswordL( const RMessage2& aMessage )
    {
    // TIpcArgs( &resultPckg, &aOldPassword, &aNewPassword )
    
	RBuf oldPassword;
	CleanupClosePushL( oldPassword );
	oldPassword.CreateL( aMessage.GetDesLengthL( 1 ) );
	aMessage.ReadL( 1, oldPassword );
    
	RBuf newPassword;
	CleanupClosePushL( newPassword );
	newPassword.CreateL( aMessage.GetDesLengthL( 2 ) );
	aMessage.ReadL( 2, newPassword );
    
    TBool result = iServer.MessageStoreL().ChangePasswordL( oldPassword, newPassword );

    CleanupStack::PopAndDestroy( &newPassword );
    CleanupStack::PopAndDestroy( &oldPassword );

    TPckg<TBool> resultPckg( result );
	aMessage.WriteL( 0, resultPckg );

	aMessage.Complete( KErrNone );    
    } // end DoChangePasswordL

// ==========================================================================
// FUNCTION: DoEnableEncryptionL
// ==========================================================================
void CMessageStoreSession::DoEnableEncryptionL( const RMessage2& aMessage )
    {
    iServer.MessageStoreL().EnableEncryptionL();
    
    aMessage.Complete( KErrNone );    
    }  // end DoEnableEncryptionL

// ==========================================================================
// FUNCTION: DoDisableEncryptionL
// ==========================================================================
void CMessageStoreSession::DoDisableEncryptionL( const RMessage2& aMessage )
    {
    iServer.MessageStoreL().DisableEncryptionL();
    
    aMessage.Complete( KErrNone );    
    } // end DoDisableEncryptionL

// ==========================================================================
// FUNCTION: DoWipeEverythingL
// ==========================================================================
void CMessageStoreSession::DoWipeEverythingL( const RMessage2& aMessage )
	{
	iServer.WipeEverythingL();
	
	aMessage.Complete( KErrNone );
	} // end DoWipeEverythingL

	
// ==========================================================================
// FUNCTION: DoCreateContainerL
// ==========================================================================
void CMessageStoreSession::DoCreateContainerL( const RMessage2& aMessage )
	{
	__LOG_ENTER( "DoCreateContainerL" )
	
	TMsgStoreCreateContainerCmdParams params;	
	TPckg<TMsgStoreCreateContainerCmdParams> paramsPckg( params );
	aMessage.ReadL( 0, paramsPckg );
	
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );
	propertiesBuf.CreateL( aMessage.GetDesLengthL( 1 ) );
	aMessage.ReadL( 1, propertiesBuf );
	
    RFile contentFile;
    CleanupClosePushL( contentFile );
    if( params.iHasContent )
        {        
	    User::LeaveIfError( contentFile.AdoptFromClient( aMessage, 2, KValThree ) );
	    }

    CBufferedMessageCreator& creator = GetBufferedMessageCreatorL();
    TBool queued = creator.EnqueueL(
        iServer.MessageStoreL(), params, propertiesBuf );
    
    //if the operation is queued then all subsequent and related
    //CreateContainerL calls will be queued until the message container is
    //committed; "related" are any direct or indirect descendants of the message
    //container.
    
    if ( queued )
        {
        if( params.iHasContent )
            {
            TRAPD( err, creator.ReplaceContentL(
                iServer.MessageStoreL(), params.iId, contentFile ) );
            
            if ( KErrNone != err )
                {
                creator.ResetL( iServer.MessageStoreL() );
                User::Leave( err );
                }
            }    
        }
    else
        {
        params.iId = iServer.MessageStoreL().CreateContainerL( params.iType, 
                                                       params.iParentId, 
                                                       params.iGrandparentId, 
                                                       propertiesBuf );
    
        if( params.iHasContent )
            {
            TRAPD( result, iServer.MessageStoreL().ReplaceContentL( params.iId, 
                                                                params.iParentId, 
                                                                contentFile,
                                                                NULL ) );

            // Abandon the entire container if the content could not be written.
            if( result != KErrNone )
                {
                __LOG_WRITE_ERROR( "failed to add content" );
                iServer.MessageStoreL().AbandonContainerL( params.iId );
                User::Leave( result );
                }
            }

        if ( params.iCommitContainer )
            {
            iServer.MessageStoreL().CommitContainerL(
                params.iId, params.iParentId, params.iMailBoxId, this );
            }
        }
   
    CleanupStack::PopAndDestroy( &contentFile );
    // if queued the ownership was transferred to the message creator.
    queued ? CleanupStack::Pop( &propertiesBuf )
        : CleanupStack::PopAndDestroy( &propertiesBuf );
	
	aMessage.WriteL( 0, paramsPckg );
	aMessage.Complete( KErrNone );
	
	__LOG_EXIT
	} // end DoCreateContainerL

// ==========================================================================
// FUNCTION: DoCommitContainerL
// ==========================================================================
void CMessageStoreSession::DoCommitContainerL( const RMessage2& aMessage )
    {
	TMsgStoreId id = aMessage.Int0();
	TMsgStoreId destinationId = aMessage.Int1();
	TMsgStoreId mailBoxId = aMessage.Int2();

	CBufferedMessageCreator& creator = GetBufferedMessageCreatorL();
	if ( creator.IsContainerBuffered( id ) )
	    {
        creator.CommitMessageL( iServer.MessageStoreL(), this );
	    }
	else
	    {
        iServer.MessageStoreL().CommitContainerL(
            id, destinationId, mailBoxId, this );
	    }
	
    aMessage.Complete( KErrNone );
    } // end DoCommitContainerL

// ==========================================================================
// FUNCTION: DoAbandonContainerL
// ==========================================================================
void CMessageStoreSession::DoAbandonContainerL( const RMessage2& aMessage )
    {
    // TIpcArgs( aId )    
    
	TMsgStoreId id = aMessage.Int0();

	CBufferedMessageCreator& creator = GetBufferedMessageCreatorL();
	if ( creator.IsContainerBuffered( id ) )
        {
        creator.AbandonMessageL( iServer.MessageStoreL(), id );
        }
	else
	    {
        iServer.MessageStoreL().AbandonContainerL( id );
	    }
    
    aMessage.Complete( KErrNone );
    } // end DoAbandonContainerL

// ==========================================================================
// FUNCTION: DoMoveContainerL
// ==========================================================================
void CMessageStoreSession::DoMoveContainerL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, aSourceId, aDestinationId )
	
	TMsgStoreId id = aMessage.Int0();

	TMsgStoreId sourceId = aMessage.Int1();
	
	TMsgStoreId destinationId = aMessage.Int2();

	iServer.MessageStoreL().MoveContainerL( id, sourceId, destinationId, this );
	
	aMessage.Complete( KErrNone );	
	} // end DoMoveContainerL
	
// ==========================================================================
// FUNCTION: DoCopyContainerL
// ==========================================================================
void CMessageStoreSession::DoCopyContainerL( const RMessage2& aMessage )
	{
    // TIpcArgs( &paramsPckg, &idPckg )	

	TMsgStoreCopyContainerCmdParams        params;
	TPckg<TMsgStoreCopyContainerCmdParams> paramsPckg( params );
	aMessage.ReadL( 0, paramsPckg );
	
	TUint newId = iServer.MessageStoreL().CopyContainerL( params.iId,
	                                                      params.iSourceId,
	                                                      params.iSourceParentId,
	                                                      params.iDestinationId,
	                                                      params.iDestinationParentId,
	                                                      params.iMailBoxId,
	                                                      this );

	TPckg<TMsgStoreId> newIdPckg(newId);
	aMessage.WriteL( 1, newIdPckg );
	
	aMessage.Complete( KErrNone );	
	} // end DoCopyContainerL
	
// ==========================================================================
// FUNCTION: DoFetchPropertiesL
// ==========================================================================
void CMessageStoreSession::DoFetchPropertiesL( const RMessage2& aMessage )
	{
	__LOG_ENTER( "DoFetchPropertiesL" )
	
    // TIpcArgs( &params, &parentIdPckg, &aBuffer, &resultPckg );
    
    TMsgStoreFetchPropertiesCmdParams params;
    TPckg<TMsgStoreFetchPropertiesCmdParams> paramsPckg( params );
	aMessage.ReadL( 0, paramsPckg );

	TMsgStoreId parentId;
	TPckg<TMsgStoreId> parentIdPckg( parentId );
	aMessage.ReadL( 1, parentIdPckg );
	
	TUint result;
	TPckg<TUint> resultPckg( result );
	
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );

	iServer.MessageStoreL().FetchPropertiesL( params.iId, parentId, params.iGrandparentId, propertiesBuf, params.iMailboxId );

	if( propertiesBuf.Length() > aMessage.GetDesMaxLengthL( 2 ) )
	    {
	    // Overflow!
	    __LOG_WRITE8_FORMAT1_ERROR( "property buffer too long (%i)", propertiesBuf.Length() );
	    
	    // Write the required length to the result parameter, to indicate to the client that the
	    // buffer must be increased to the given size.
	    result = propertiesBuf.Length();
	    aMessage.WriteL( KValThree, resultPckg );	    
	    }
	else
	    {	    	
 	    aMessage.WriteL( 1, parentIdPckg );

        aMessage.WriteL( 2, propertiesBuf );

        result = 0;
	    aMessage.WriteL( KValThree, resultPckg );
	    } // end if
	    	    
   	CleanupStack::PopAndDestroy( &propertiesBuf );	

	aMessage.Complete( KErrNone );
	
	__LOG_EXIT
	} // end DoFetchPropertiesL

// ==========================================================================
// FUNCTION: DoUpdatePropertiesL
// ==========================================================================
void CMessageStoreSession::DoUpdatePropertiesL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, aParentId, properties )

	TMsgStoreId id = aMessage.Int0();

	TMsgStoreId parentId = aMessage.Int1();
	
	TMsgStoreId mailBoxId = aMessage.Int2();

	TInt propertiesLength = aMessage.GetDesLengthL( KValThree );
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );
	propertiesBuf.CreateL( propertiesLength );
	aMessage.ReadL( KValThree, propertiesBuf );

	iServer.MessageStoreL().UpdatePropertiesL( id, parentId, mailBoxId, propertiesBuf, this );
	
	CleanupStack::PopAndDestroy( &propertiesBuf );
	
	aMessage.Complete( KErrNone );
	} // end DoUpdatePropertiesL

// ==========================================================================
// FUNCTION: DoUpdatePropertyL
// ==========================================================================
void CMessageStoreSession::DoUpdatePropertyL( const RMessage2& aMessage )
	{
    // TIpcArgs( &params, &aName, &aValue )    
    
    TMsgStoreUpdatePropertyCmdParams        params;
    TPckg<TMsgStoreUpdatePropertyCmdParams> paramsPckg( params );
	aMessage.ReadL( 0, paramsPckg );

	RBuf8 nameBuf;
	CleanupClosePushL( nameBuf );
	nameBuf.CreateL( aMessage.GetDesLengthL( 1 ) );
	aMessage.ReadL( 1, nameBuf );

	RBuf8 valueBuf;
	CleanupClosePushL( valueBuf );
	valueBuf.CreateL( aMessage.GetDesLengthL( 2 ) );
	aMessage.ReadL( 2, valueBuf );

	iServer.MessageStoreL().UpdatePropertyL( params.iId, params.iParentId, params.iMailBoxId, nameBuf, params.iType, valueBuf, this );
	
	CleanupStack::PopAndDestroy( &valueBuf );
	CleanupStack::PopAndDestroy( &nameBuf );
	
	aMessage.Complete( KErrNone );
	} // end DoUpdatePropertyL

// ==========================================================================
// FUNCTION: DoChildrenCountsL
// ==========================================================================
void CMessageStoreSession::DoChildrenCountsL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, &counts )

	TMsgStoreId id = aMessage.Int0();

	RBuf8 countsBuf;
	CleanupClosePushL( countsBuf );
	countsBuf.CreateL( KMsgStoreCountsLength );
	
	iServer.MessageStoreL().ChildrenCountsL( id, countsBuf );
		
	aMessage.WriteL( 1, countsBuf );
	
	CleanupStack::PopAndDestroy( &countsBuf );
	
	aMessage.Complete( KErrNone );
	} // end DoChildrenCountsL

// ==========================================================================
// FUNCTION: DoTotalCountsL
// ==========================================================================
void CMessageStoreSession::DoTotalCountsL( const RMessage2& aMessage )
	{
	// TIpcArgs( &counts )
	
	TMsgStoreId mailBoxId = aMessage.Int0();

	RBuf8 countsBuf;
	CleanupClosePushL( countsBuf );
	countsBuf.CreateL( KMsgStoreCountsLength );
	
	iServer.MessageStoreL().TotalCountsL( mailBoxId, countsBuf );
		
	aMessage.WriteL( 1, countsBuf );
	
	CleanupStack::PopAndDestroy( &countsBuf );
	
	aMessage.Complete( KErrNone );
	} // end DoTotalCountsL

// ==========================================================================
// FUNCTION: DoChildrenIdsL
// ==========================================================================
void CMessageStoreSession::DoChildrenIdsL( const RMessage2& aMessage )	
	{
	// TIpcArgs( aId, aType, &childrenDes, &resultBuf )

	__LOG_ENTER( "DoChildrenIdsL" )
		
	TMsgStoreId id = aMessage.Int0();

	TMsgStoreId type = aMessage.Int1();

	RArray<TMsgStoreId> childIds( KGranularity25 );
	CleanupClosePushL( childIds );
	
	TUint result;
	TPckg<TUint> resultPckg( result );
	
	iServer.MessageStoreL().ListChildrenL( childIds, id, KMsgStoreInvalidId, type );

    TUint sizeNeeded = childIds.Count() * sizeof(TMsgStoreId);

    if( sizeNeeded > aMessage.GetDesMaxLengthL( 2 ) )
        {
	    // Overflow!
	    __LOG_WRITE8_FORMAT1_ERROR( "children IDs buffer too long (%i)", sizeNeeded );
	    
	    // Write the required length to the result parameter, to indicate to the client that the
	    // buffer must be increased to the given size.
        result = sizeNeeded;
        aMessage.WriteL( KValThree, resultPckg );        
        }
    else
        {        
    	for( TInt count = 0; count < childIds.Count(); count++ )
    		{
    		TPckg<TMsgStoreId> currentChild( childIds[count] );
    		const TUint KFour = 4;
    		aMessage.WriteL( 2, currentChild, count*KFour );
    		} // end for
    		
        result = 0;
        aMessage.WriteL( KValThree, resultPckg );            		
        } // end if
        
	CleanupStack::PopAndDestroy( &childIds ); 
	
	aMessage.Complete( KErrNone );
	
	__LOG_EXIT	
	} // end DoChildrenIdsL

// ==========================================================================
// FUNCTION: DoPropertiesListL
// ==========================================================================
void CMessageStoreSession::DoPropertiesListL( const RMessage2& aMessage )
	{	
    // TIpcArgs( &paramsPckg, &idList, &paramNames, &serializedBuf )	
    
    __LOG_ENTER( "DoPropertiesList" )
    
	TMsgStorePropertiesListCmdParams params;
	TPckg<TMsgStorePropertiesListCmdParams> paramsPckg( params );
	aMessage.ReadL( 0, paramsPckg );	

	TInt position = 0;

   	TMsgStoreId id;
	TPckg<TMsgStoreId> idPckg( id );

   	TMsgStoreId parentId;
	TPckg<TMsgStoreId> parentIdPckg( parentId );
    
    TUint16 length16;
    TPckg<TUint16> length16Pckg( length16 );
    
    TUint32 length32;
    TPckg<TUint32> length32Pckg( length32 );    

	CContainerStore& messageStore = iServer.MessageStoreL();

	// Suspend compaction here, to avoid suspension and resumption of compaction for each
	// fetch properties operation.	
	messageStore.SuspendCompactionLC();

	// Populate the ids and property names lists, if needed.
	
	// params.iStartOver means that the client wants to reset the list
	// iIds.Count == 0 means that the container store has been modified
	if( params.iStartOver || iIds.Count() == 0 )
		{
		__LOG_WRITE_INFO( "Starting over" )
		
		iIds.Reset();
		iPropertyNames.ResetAndDestroy();

		// Indicate to the client that any existing data should be cleared.		
	    params.iClearExisting = ETrue;

        iUsingQuery = params.iUseQuery;
        
        if( iUsingQuery )
            {
            __LOG_WRITE_INFO( "querying for children" )

            TPckg<TMsgStorePropertiesListQueryParams> queryParamsPckg( iQueryParams );    
    
       		aMessage.ReadL( 1, queryParamsPckg );		
            
            // Get the list of children IDs whose properties are to be retrieved.        		              		
    		messageStore.ListChildrenL( iIds,
    		                            iQueryParams.iId,
    		                            iQueryParams.iParentId,
    		                            iQueryParams.iType,
    		                            iQueryParams.iRecursive );
            } 
        else            
            {
            __LOG_WRITE_INFO( "deserializing ID list" )

    		// Deserialize the ID list from the message.
            ReadIdArrayL( aMessage, 1, iIds );
            } // end if
            
        __LOG_WRITE_INFO( "deserializing property names" )
            
		// Deserialize the property names list.
        ReadString8ArrayL( aMessage, 2, iPropertyNames );
		}            
    else
        {
        // The client should not clear any properties that have already been received.
        params.iClearExisting = EFalse; 
		} // end if

    __LOG_WRITE8_FORMAT1_INFO( "iIds.Count=%i", iIds.Count() )
    		    
    RBuf8 propertiesBuf;
    CleanupClosePushL( propertiesBuf );

    const TUint overhead  = idPckg.Length() + parentIdPckg.Length() + length32Pckg.Length();
	const TUint maxLength = aMessage.GetDesMaxLengthL( 3 );

 	TInt  result;
    TBool bufferFull = EFalse;
    
    position = 0;    
	
    // Continue adding properties to the buffer until there are no more properties or the buffer is full.
	while( iIds.Count() > 0 && !bufferFull )
		{
		propertiesBuf.SetLength( 0 );
		id       = iIds[0];
		parentId = KMsgStoreInvalidId;
		if( params.iUseQuery )
		    {
    		// Use the fast fetch because the ContainerModified callback will cause the query to be
    		// reissued if a potential change to a query item has been made.
            TRAP( result, messageStore.FastFetchPropertiesL( id, 
                                                             params.iQuickProperties, 
                                                             parentId, 
                                                             propertiesBuf ) );
		    }
		else
		    {
            // Don't want to return properties for deleted items, so don't use the fast fetch.		    
            TRAP( result, messageStore.FetchPropertiesL( id, 
                                                         parentId,
                                                         KMsgStoreInvalidId, 
                                                         propertiesBuf ) );
		    } // end if
                               
        if( result == KErrNone )
            {            
            // Filter the properties, if needed.
            if( iPropertyNames.Count() > 0 )
                {
                // Remove the properties that are not needed.
                TPropertiesSerializer serializer( propertiesBuf );        
                TBool moreProperties = serializer.First();        
                while( moreProperties )        
                    {
                    TBool found = EFalse;            
                    for( TInt i = 0; !found && (i < iPropertyNames.Count()); i++ )
                        {
                        found = (iPropertyNames[i]->Compare( serializer.Name() ) == 0);
                        } // end for                               
                    if( found )
                        {
                        moreProperties = serializer.Next();
                        }
                    else
                        {
                        moreProperties = serializer.RemovePropertyL();
                        } // end if                
                    } // end while   
                } // end if
                    
            if( position + propertiesBuf.Length() + overhead > maxLength )
                {
                // Insufficient room in the IPC buffer for the current set of properties.
                bufferFull = ETrue;
                }
            else
                {
                // Write the current set of properties into the IPC buffer.
        		aMessage.WriteL( KValThree, idPckg, position );		
        		position += idPckg.Length();
        		
    		    aMessage.WriteL( KValThree, parentIdPckg, position );
        		position += parentIdPckg.Length();		    

        		length32 = propertiesBuf.Length();
        		aMessage.WriteL( KValThree, length32Pckg, position );
        		position += length32Pckg.Length();

        		aMessage.WriteL( KValThree, propertiesBuf, position );        		
        		position += length32;
    		
    		    iIds.Remove( 0 );

                } // end if
            }
        else                            
            {
    	    __LOG_WRITE8_FORMAT1_ERROR( "failure during DoPropertiesListL %i", result );
    	    
    	    // This entry couldn't be read for some reason.  Skip past it.
   		    iIds.Remove( 0 );
    		    
            } // end if            
		    
		} // end for

    // If the current set of properties is larger than the transfer buffer then indicate to the client
    // that a larger transfer buffer is needed.
    params.iNewBufferSize = propertiesBuf.Length() + overhead;
	if( params.iNewBufferSize <= maxLength )
	    {	    
        __LOG_WRITE_INFO( "larger buffer needed" )
	    params.iNewBufferSize = 0;	    
	    } // end if
	    
	if( iIds.Count() > 0 )
	    {
        __LOG_WRITE_INFO( "more properties" )
        
	    // Indicate that there are more properties to stream.
	    params.iMoreProperties = ETrue;
	    }
	else
	    {
        __LOG_WRITE_INFO( "no more properties" )
        
	    // Indicate that there are no more properties to stream.
	    params.iMoreProperties = EFalse;
	    
	    // free the memory
	    iIds.Reset();  
	    iPropertyNames.ResetAndDestroy();
		} // end if

    CleanupStack::PopAndDestroy( &propertiesBuf );

	messageStore.ResumeCompaction();
	    		
	// Write the more flag.
	aMessage.WriteL( 0, paramsPckg );
	
	aMessage.Complete( KErrNone );

    __LOG_EXIT	
	} // end DoPropertiesListL

// ==========================================================================
// FUNCTION: DoContentLengthL
// ==========================================================================
void CMessageStoreSession::DoContentLengthL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, aParentId, &lengthPckg )
	
	TMsgStoreId id = aMessage.Int0();
	
	TMsgStoreId parentId = aMessage.Int1();

	TUint length = iServer.MessageStoreL().ContentLengthL( id, parentId );
	
	TPckg<TUint> lengthPckg( length );
	aMessage.WriteL( 2, lengthPckg );
	
	aMessage.Complete( KErrNone );
	} // end DoContentLengthL
	
// ==========================================================================
// FUNCTION: DoFetchContentToBufferL
// ==========================================================================
void CMessageStoreSession::DoFetchContentToBufferL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, aParentId, &aBuffer, aStartOffset )
	
	TMsgStoreId id = aMessage.Int0();

	TMsgStoreId parentId = aMessage.Int1();

	RBuf8 contentBuf;
	CleanupClosePushL( contentBuf );
	contentBuf.CreateL( aMessage.GetDesMaxLengthL( 2 ) );

	TMsgStoreId startOffset = aMessage.Int3();
		
	iServer.MessageStoreL().FetchContentL( id, parentId, contentBuf, startOffset );

	aMessage.WriteL( 2, contentBuf );

	CleanupStack::PopAndDestroy( &contentBuf );
	
	aMessage.Complete( KErrNone );
	} // end DoFetchContentToBufferL
	
// ==========================================================================
// FUNCTION: DoFetchContentToFileL
// ==========================================================================
void CMessageStoreSession::DoFetchContentToFileL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, aParentId, fs, file )
	
	TMsgStoreId id = aMessage.Int0();

	TMsgStoreId parentId = aMessage.Int1();

	RFile file;	
	User::LeaveIfError( file.AdoptFromClient( aMessage, 2, KValThree ) );
	CleanupClosePushL( file );
	
	iServer.MessageStoreL().FetchContentL( id, parentId, file );

	CleanupStack::PopAndDestroy( &file );

	aMessage.Complete( KErrNone );
	} // end DoFetchContentToFileL
	
// ==========================================================================
// FUNCTION: DoReplaceContentWithBufferL
// ==========================================================================
void CMessageStoreSession::DoReplaceContentWithBufferL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, aParentId, &aBuffer )
	
	TMsgStoreId id = aMessage.Int0();

	TMsgStoreId parentId = aMessage.Int1();

	RBuf8 contentBuf;
	CleanupClosePushL( contentBuf );
	contentBuf.CreateL( aMessage.GetDesLengthL( 2 ) );
	aMessage.ReadL( 2, contentBuf );

	CBufferedMessageCreator& creator = GetBufferedMessageCreatorL();
    if ( creator.IsContainerBuffered( id ) )
        {
        creator.ReplaceContentL(
            iServer.MessageStoreL(), id, contentBuf );
        }
    else
        {
        iServer.MessageStoreL().ReplaceContentL(
            id, parentId, contentBuf, this );
        }
    
	CleanupStack::PopAndDestroy( &contentBuf );

	aMessage.Complete( KErrNone );
	} // end DoReplaceContentWithBufferL
	
// ==========================================================================
// FUNCTION: DoReplaceContentWithFileL
// ==========================================================================
void CMessageStoreSession::DoReplaceContentWithFileL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, aParentId, fs, file )
	
	TMsgStoreId id = aMessage.Int0();

	TMsgStoreId parentId = aMessage.Int1();

    RFile file;    
    User::LeaveIfError( file.AdoptFromClient( aMessage, 2, KValThree ) );
    CleanupClosePushL( file );
    	
    CBufferedMessageCreator& creator = GetBufferedMessageCreatorL();
    if ( creator.IsContainerBuffered( id ) )
        {
        creator.ReplaceContentL(
            iServer.MessageStoreL(), id, file );
        }
    else
        {
        iServer.MessageStoreL().ReplaceContentL( id, parentId, file, this );
        }

	CleanupStack::PopAndDestroy( &file );

	aMessage.Complete( KErrNone );
	} // end DoReplaceContentWithFileL
	
// ==========================================================================
// FUNCTION: DoAppendToContentL
// ==========================================================================
void CMessageStoreSession::DoAppendToContentL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, aParentId, &aContent )
	
	TMsgStoreId id = aMessage.Int0();

	TMsgStoreId parentId = aMessage.Int1();

	RBuf8 contentBuf;
	CleanupClosePushL( contentBuf );
	contentBuf.CreateL( aMessage.GetDesLengthL( 2 ) );
	aMessage.ReadL( 2, contentBuf );

	CBufferedMessageCreator& creator = GetBufferedMessageCreatorL();
    if ( creator.IsContainerBuffered( id ) )
        {
        creator.AppendContentL(
            iServer.MessageStoreL(), id, contentBuf );
        }
    else
        {
        iServer.MessageStoreL().AppendContentL( id, parentId, contentBuf, this );
        }
	
	CleanupStack::PopAndDestroy( &contentBuf );

	aMessage.Complete( KErrNone );
	} // end DoAppendToContentL
	
// ==========================================================================
// FUNCTION: DoPrependToContentL
// ==========================================================================
void CMessageStoreSession::DoPrependToContentL( const RMessage2& aMessage )
    {
    TMsgStoreId id = aMessage.Int0();
    TMsgStoreId parentId = aMessage.Int1();

    RBuf8 contentBuf;
    CleanupClosePushL( contentBuf );
    contentBuf.CreateL( aMessage.GetDesLengthL( 2 ) );
    aMessage.ReadL( 2, contentBuf );
    
    CBufferedMessageCreator& creator = GetBufferedMessageCreatorL();
    if ( creator.IsContainerBuffered( id ) )
        {
        creator.PrependContentL(
            iServer.MessageStoreL(), id, contentBuf );
        }
    else
        {
        iServer.MessageStoreL().PrependContentL(
            id, parentId, contentBuf, this );
        }
    
    CleanupStack::PopAndDestroy( &contentBuf );
    aMessage.Complete( KErrNone );
    }

// ==========================================================================
// FUNCTION: DoRemoveContentL
// ==========================================================================
void CMessageStoreSession::DoRemoveContentL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, aParentId )
	
	TMsgStoreId id = aMessage.Int0();

	TMsgStoreId parentId = aMessage.Int1();

	iServer.MessageStoreL().RemoveContentL( id, parentId, this );	

	aMessage.Complete( KErrNone );
	} // end DoRemoveContentL


// ==========================================================================
// FUNCTION: DoOpenContentFile
// ==========================================================================
void CMessageStoreSession::DoOpenContentFileL( const RMessage2& aMessage )
    {
    TMsgStoreId id = aMessage.Int0();
    TMsgStoreId parentId = aMessage.Int1();
    
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);                                 //+fs

    User::LeaveIfError(fs.ShareProtected());

    RFile file;
    CleanupClosePushL( file );                             //+file
    
    iServer.MessageStoreL().OpenContentFileL( id, parentId, fs, file );
    
    // transfer to client: store the RFile handle into the package buffer in slot 2 
    // and complete the message with the RFs handle
    // This assumes that if TransferToClient() return an error, then
    // the standard CServer2::RunError() will complete the message
    User::LeaveIfError( file.TransferToClient(aMessage, 2) );
    
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fs );
    }

// ==========================================================================
// FUNCTION: DoDeleteContainerL
// ==========================================================================
void CMessageStoreSession::DoDeleteContainerL( const RMessage2& aMessage )
	{
	// TIpcArgs( aId, aParentId, aGrandparentId )
	
	TMsgStoreId id = aMessage.Int0();
	
	TMsgStoreId parentId = aMessage.Int1();

	TMsgStoreId grandparentId = aMessage.Int2();
	
	TMsgStoreId mailBoxId = aMessage.Int3();
	
	iServer.MessageStoreL().DeleteContainerL( id, parentId, grandparentId, mailBoxId, this );
	
	aMessage.Complete( KErrNone );
	} // end DoDeleteContainerL


// -------------------------
// SORTING HANDLER FUNCTIONS
// -------------------------
// ==========================================================================
// FUNCTION: DoStartSortingL
// ==========================================================================
void CMessageStoreSession::DoStartSortingL( const RMessage2& aMessage )
    {
    __LOG_ENTER( "DoStartSortingL" )
    
    TMsgStoreSortCriteria sortCriteria;
    TPckg<TMsgStoreSortCriteria> paramsPckg( sortCriteria );
    aMessage.ReadL( 1, paramsPckg );
    
    TUint16 length16;
    TPckg<TUint16> length16Pckg( length16 );
    
    // Deserialize the property names list.
    ReadString8ArrayL( aMessage, 2, iPropertyNames );
    
    //parameter 3
    TBool inMemorySort = aMessage.Int3();

    TContainerId sessionId = iServer.MessageStoreL().StartSortingL( sortCriteria, iPropertyNames, inMemorySort );
    iSortSessionIds.AppendL( sessionId );
    
    TPckg<TContainerId> sessionIdPckg( sessionId );
    aMessage.WriteL( 0, sessionIdPckg );
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DoEndSortingL
// ==========================================================================
void CMessageStoreSession::DoEndSortingL( const RMessage2& aMessage )
    {
    __LOG_ENTER( "DoEndSortingL" )
    
    TMsgStoreId sessionId = aMessage.Int0();
    
    iServer.MessageStoreL().EndSortingL( sessionId );
    
    TInt index = iSortSessionIds.Find( sessionId );
    if ( index != KErrNotFound )
        {
        iSortSessionIds.Remove( index );
        }
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: GetSortedRowsL
// ==========================================================================
void CMessageStoreSession::DoGetSortedRowsL( const RMessage2& aMessage )
    {
    __LOG_ENTER( "DoGetSortedRowsL" )
    
    //param 0   
    TUint result;
    TPckg<TUint> resultPckg( result );
    
    //param 1
    TMsgStoreGetSortedRowsCmdParams params;
    TPckg<TMsgStoreGetSortedRowsCmdParams> paramsPckg( params );
    aMessage.ReadL( 1, paramsPckg );
    
    //param 2
    RBuf8 propertiesBuf;
    CleanupClosePushL( propertiesBuf );
    
    //param 3
    TBool hasMore;
    if ( params.iHasStartWith )
        {
        RBuf startWith;
        CleanupClosePushL( startWith );
        
        startWith.CreateL( aMessage.GetDesLengthL( KValThree ) );
        aMessage.ReadL( KValThree, startWith );
        
        hasMore = iServer.MessageStoreL().GetSortedRowsL( params, propertiesBuf, startWith );
        CleanupStack::PopAndDestroy( &startWith );
        }
    else
        {
        hasMore = iServer.MessageStoreL().GetSortedRowsL( params, propertiesBuf );
        }
    params.iHasMoreRows = hasMore;
    
    if( propertiesBuf.Length() > aMessage.GetDesMaxLengthL( 2 ) )
        {
        // Overflow!
        __LOG_WRITE8_FORMAT1_ERROR( "property buffer too long (%i)", propertiesBuf.Length() );
        
        // Write the required length to the result parameter, to indicate to the client that the
        // buffer must be increased to the given size.
        result = propertiesBuf.Length();
        aMessage.WriteL( 0, resultPckg );       
        }
    else
        {           
        result = 0;
        aMessage.WriteL( 0, resultPckg );
        aMessage.WriteL( 1, paramsPckg );
        aMessage.WriteL( 2, propertiesBuf );
        } // end if
    
    CleanupStack::PopAndDestroy( &propertiesBuf );
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DoGetIteratorGroupCountL
// ==========================================================================
void CMessageStoreSession::DoGetIteratorGroupCountL( const RMessage2& aMessage )
    {
    __LOG_ENTER("DoGetIteratorGroupCountL")
    
    //param 0
    TMsgStoreId sessionId = aMessage.Int0();
    
    //param 1
    TInt count;
    TPckg<TInt> resultPckg( count );
    
    RArray<TUint> itemsInGroup;
    CleanupClosePushL( itemsInGroup );
    
    count = iServer.MessageStoreL().IteratorGroupCountL( sessionId, itemsInGroup );
    aMessage.WriteL( 1, resultPckg );
    
    TUint items;
    TPckg<TUint> itemsPkg( items );
    TInt position = 0;
    
    for ( TInt i = 0; i < count; i++ )
        {
        items = itemsInGroup[i];
        aMessage.WriteL( 2, itemsPkg, position );     
        position += itemsPkg.Length();
        }
    
    CleanupStack::PopAndDestroy( &itemsInGroup );
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DoGetSortedIdsAndFlagsL
// ==========================================================================
void CMessageStoreSession::DoGetSortedIdsAndFlagsL( const RMessage2& aMessage )
	{
    __LOG_ENTER("DoGetSortedIdsAndFlags")
    
    //param 0
    TMsgStoreId sessionId = aMessage.Int0();
    
    //param 1
    TInt count;
    TPckg<TInt> countPckg( count );
    
    RArray<TContainerId> idArray( KSortedArrayGranularity );
    CleanupClosePushL( idArray );
    
    RArray<TUint32> flagArray ( KSortedArrayGranularity );
    CleanupClosePushL( flagArray );
    
    iServer.MessageStoreL().SortedIdsAndFlagsL( sessionId, idArray, flagArray );
    if ( idArray.Count() != flagArray.Count() )
    	{
    	User::Leave( KErrCorrupt );
    	}
    count = idArray.Count();
    aMessage.WriteL( 1, countPckg );
    
    TContainerId id;
    TPckg<TContainerId> idPkg( id );
    
    TUint32 flag;
    TPckg<TUint32> flagPkg( flag );
    
    TInt position = 0;
    
    for ( TInt i = 0; i < count; i++ )
        {
        id = idArray[i];
        aMessage.WriteL( 2, idPkg, position );     
        position += idPkg.Length();
        
        flag = flagArray[i];
        aMessage.WriteL( 2, flagPkg, position );     
        position += flagPkg.Length();
        }
    
    CleanupStack::PopAndDestroy( &flagArray );
    CleanupStack::PopAndDestroy( &idArray );
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
	}


// ==========================================================================
// FUNCTION: DoGetSortedIndexOfL
// ==========================================================================
void CMessageStoreSession::DoGetSortedIndexOfL( const RMessage2& aMessage )
    {
    __LOG_ENTER("DoGetSortedIndexOfL")
    
    //param 0
    TContainerId sessionId = aMessage.Int0();
    
    //param 1
    TContainerId messageId = aMessage.Int1();
    
    //param 2
    TInt index;
    TPckg<TInt> indexPckg( index );
    
    index = iServer.MessageStoreL().SortedIndexOfL( sessionId, messageId );
    
    aMessage.WriteL( 2, indexPckg );
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DoGetSortedIdsL
// ==========================================================================
void CMessageStoreSession::DoGetSortedIdsL( const RMessage2& aMessage )
	{
    __LOG_ENTER("DoGetSortedIdsL")
    
    //param 0
    TMsgStoreId sessionId = aMessage.Int0();
    
    //param 1
    TInt count;
    TPckg<TInt> countPckg( count );
    
    RArray<TContainerId> idArray( KSortedArrayGranularity );
    CleanupClosePushL( idArray );
    
    iServer.MessageStoreL().SortedIdsL( sessionId, idArray );
    count = idArray.Count();
    aMessage.WriteL( 1, countPckg );
    
    TContainerId id;
    TPckg<TContainerId> idPkg( id );
    
    TInt position = 0;
    
    for ( TInt i = 0; i < count; i++ )
        {
        id = idArray[i];
        aMessage.WriteL( 2, idPkg, position );     
        position += idPkg.Length();
        }
    
    CleanupStack::PopAndDestroy( &idArray );
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
	}


// ==========================================================================
// FUNCTION: DoGetSortedIdsAndGroupCountL
// ==========================================================================
void CMessageStoreSession::DoGetSortedIdsAndGroupCountL( const RMessage2& aMessage )
    {
    __LOG_ENTER("DoGetSortedIdsAndGroupCountL")
    
    //param 0
    TMsgStoreId sessionId = aMessage.Int0();
    
    //param 1
    TInt idCount;
    TPckg<TInt> idCountPckg( idCount );
    
    //param 2
    TInt groupCount;
    TPckg<TInt> groupCountPckg( groupCount );
    
    RArray<TContainerId> idArray( KSortedArrayGranularity );
    CleanupClosePushL( idArray );
    
    RArray<TUint> itemsInGroup;
    CleanupClosePushL( itemsInGroup );
    
    iServer.MessageStoreL().SortedIdsAndGroupCountL( sessionId, idArray, itemsInGroup );
    
    idCount = idArray.Count();
    aMessage.WriteL( 1, idCountPckg );
    
    groupCount = itemsInGroup.Count();
    aMessage.WriteL( 2, groupCountPckg );
    
    TContainerId id;
    TPckg<TContainerId> idPkg( id );
    
    TInt position = 0;
    
    for ( TInt i = 0; i < idCount; i++ )
        {
        id = idArray[i];
        aMessage.WriteL( KValThree, idPkg, position );     
        position += idPkg.Length();
        }
    
    TUint items;
    TPckg<TUint> itemsPkg( items );
    
    for ( TInt i = 0 ; i < groupCount ; i++ )
        {
        items = itemsInGroup[i];
        aMessage.WriteL( KValThree, itemsPkg, position );     
        position += itemsPkg.Length();
        }
    
    CleanupStack::PopAndDestroy( &itemsInGroup );
    CleanupStack::PopAndDestroy( &idArray );
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DoSetMaxMruAdressCountL
// ==========================================================================
void CMessageStoreSession::DoSetMaxMruAdressCountL( const RMessage2& aMessage )
    {
    __LOG_ENTER("DoSetMaxMruAdressCountL")
    
    TInt maxCount = aMessage.Int0();
    
    iServer.MessageStoreL().SetMaxMruCountL( maxCount );
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DoSetMruAddressListL
// ==========================================================================
void CMessageStoreSession::DoSetMruAddressListL( const RMessage2& aMessage )
    {
    __LOG_ENTER("DoSetMruAddressListL")
    
    TMsgStoreId mailboxId = aMessage.Int0();
    
    iMruAddressArray.ResetAndDestroy();
    
    //read the address array
    TInt bufSize = aMessage.GetDesLengthL( 1 );
    
    TUint16 length16;
    TPckg<TUint16> length16Pckg( length16 );
    
    RBuf8 addressBuf;
    CleanupClosePushL( addressBuf );
    addressBuf.CreateL( KInitialAddressBufSize );
    
    RBuf8 dispNameBuf;
    CleanupClosePushL( dispNameBuf );
    dispNameBuf.CreateL( KInitialAddressBufSize );
    
    TInt position = 0;
    while( position < bufSize )
        {
        //read the email address
        aMessage.ReadL( 1, length16Pckg, position ); 
        position += length16Pckg.Length();
        
        if ( addressBuf.MaxLength() < length16 )
            {
            addressBuf.ReAllocL( length16 );
            }
        aMessage.ReadL( 1, addressBuf, position );
        
        //now read convert the 8 bit descriptor to 16 descriptor
        const TUint16* addressPtr = reinterpret_cast<const TUint16*>( addressBuf.Ptr() );
        TPtrC addressDes( addressPtr, length16 / 2 );  
        
        position += length16;

        //read the display name
        aMessage.ReadL( 1, length16Pckg, position ); 
        position += length16Pckg.Length();
        
        if ( dispNameBuf.MaxLength() < length16 )
            {
            dispNameBuf.ReAllocL( length16 );
            }
        aMessage.ReadL( 1, dispNameBuf, position );
        
        //now read convert the 8 bit descriptor to 16 descriptor
        const TUint16* dispNamePtr = reinterpret_cast<const TUint16*>( dispNameBuf.Ptr() );
        TPtrC dispNameDes( dispNamePtr, length16 / 2 );  
        
        position += length16;
        
        CMruAddress* mruAddress = CMruAddress::NewL(0, addressDes, dispNameDes );
        
        iMruAddressArray.AppendL( mruAddress );
        
        } // end while
    
    iServer.MessageStoreL().SetMruAddressListL( mailboxId, iMruAddressArray );
    
    CleanupStack::PopAndDestroy( &dispNameBuf );
    CleanupStack::PopAndDestroy( &addressBuf );
    
    iMruAddressArray.ResetAndDestroy();
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DoGetMruAddressListL
// ==========================================================================
void CMessageStoreSession::DoGetMruAddressListL( const RMessage2& aMessage )
    {
    __LOG_ENTER("DoGetMruAddressListL")
    
    TUint result;
    TPckg<TUint> resultPckg( result );
    
    TMsgStoreId mailboxId = aMessage.Int0();
    
    const RPointerArray<CMruAddress> addressArray = iServer.MessageStoreL().MruAddressListL( mailboxId );
    
    TUint16 length16;
    TPckg<TUint16> length16Pckg( length16 );
    
    //Find out the size
    TInt totalSize = 0;
    TInt count = addressArray.Count();
    TInt sizeOfLengthFields = sizeof( TUint16 ) * 2;  //two lengths
    for ( TInt i = 0 ; i < count ; i++ )
        {
        totalSize += sizeOfLengthFields;
        CMruAddress* address = addressArray[i];
        totalSize += (address->EmailAddress().Length() * 2);
        totalSize += (address->DisplayName().Length() * 2);
        }
    
    //now check if the client buffer is big enough
    if ( totalSize > aMessage.GetDesMaxLengthL( 2 ) )
        {
        // Overflow!
        __LOG_WRITE8_FORMAT1_ERROR( "Mru buffer too long (%i)", totalSize );
        
        // Write the required length to the result parameter, to indicate to the client that the
        // buffer must be increased to the given size.
        result = static_cast<TUint>(totalSize);
        aMessage.WriteL( 1, resultPckg );       
        }
    else
        {
        //now serialize it to the buffer
        TUint position = 0;
        
        for ( TInt i = 0 ; i < count ; i++ )
            {
            CMruAddress* address = addressArray[i];
            
            //write the email address
            //write the length and update the cursor
            length16 = address->EmailAddress().Length() * 2 ;
            aMessage.Write( 2, length16Pckg, position );
            position += length16Pckg.Length();
            
            //convert the 16 bit descriptor to 8 bit descriptor
            const TUint8* addressPtr = reinterpret_cast<const TUint8*>( address->EmailAddress().Ptr() );
            TPtrC8 addressDes( addressPtr, length16 );  

            //write it and update the cursor
            aMessage.Write( 2, addressDes, position );
            position += length16;
            
            //write the displayName
            //write the length and update the cursor
            length16 = address->DisplayName().Length() * 2 ;
            aMessage.Write( 2, length16Pckg, position );
            position += length16Pckg.Length();
            
            //convert the 16 bit descriptor to 8 bit descriptor
            const TUint8* namePtr = reinterpret_cast<const TUint8*>( address->DisplayName().Ptr() );
            TPtrC8 nameDes( namePtr, length16 );  
            
            //write it and update the cursor
            aMessage.Write( 2, nameDes, position );
            position += length16;
            }
        
        result = 0;
        aMessage.WriteL( 1, resultPckg );       
        }    
    
    aMessage.Complete( KErrNone );
    
    __LOG_EXIT
    }

/**
 * 
 */
void CMessageStoreSession::DoBeginBatchInsertL( const RMessage2& aMessage )
    {
    CBufferedMessageCreator& creator = GetBufferedMessageCreatorL();
    creator.BeginBatchInsertL( iServer.MessageStoreL() );
    
    aMessage.Complete( KErrNone );
    }

/**
 * 
 */
void CMessageStoreSession::DoFinishBatchInsertL( const RMessage2& aMessage )
    {
    CBufferedMessageCreator& creator = GetBufferedMessageCreatorL();
    creator.FinishBatchInsertL( iServer.MessageStoreL(), this );
    
    aMessage.Complete( KErrNone );
    }

// ==========================================================================
// FUNCTION: ReadString8ArrayL
// ==========================================================================
void CMessageStoreSession::ReadString8ArrayL(  const RMessage2& aMessage, TInt aIndex, RPointerArray<HBufC8>& aArray )
    {
    TUint16 length16;
    TPckg<TUint16> length16Pckg( length16 );
    
    // Deserialize the property names list.
    aArray.ResetAndDestroy();
    
    TInt position = 0;
    while( position < aMessage.GetDesLengthL( aIndex ) )
        {
        aMessage.ReadL( aIndex, length16Pckg, position );        
        position += length16Pckg.Length();
        
        HBufC8* buf8 = HBufC8::NewLC( length16 );            
        TPtr8 buf8Ptr( buf8->Des() );
        aMessage.ReadL( aIndex, buf8Ptr, position );
        buf8->Des().SetLength( length16 ); 
        aArray.AppendL( buf8 );
        CleanupStack::Pop(buf8);
        position += length16;
        } // end while
    }

// ==========================================================================
// FUNCTION: ReadString16ArrayL
// ==========================================================================
void CMessageStoreSession::ReadString16ArrayL(  const RMessage2& aMessage, TInt aIndex, RPointerArray<HBufC>& aArray )
    {
    TUint16 length16;
    TPckg<TUint16> length16Pckg( length16 );
    
    // Deserialize the property names list.
    aArray.ResetAndDestroy();
    
    RBuf8 readBuf;
    CleanupClosePushL( readBuf );
    
    TInt position = 0;
    while( position < aMessage.GetDesLengthL( aIndex ) )
        {
        aMessage.ReadL( aIndex, length16Pckg, position );        
        position += length16Pckg.Length();
        
        if ( readBuf.MaxLength() < length16 )
            {
            readBuf.ReAllocL( length16 );
            }
        aMessage.ReadL( aIndex, readBuf, position );
        
        //now read convert the 8 bit descriptor to 16 descriptor
        const TUint16* valuePtr = reinterpret_cast<const TUint16*>( readBuf.Ptr() );
        TPtrC valueDes( valuePtr, length16 / 2 );  
        
        HBufC *p = valueDes.AllocLC();
        aArray.AppendL( p );
        CleanupStack::Pop( p );
        
        position += length16;
        } // end while
    
    CleanupStack::PopAndDestroy( &readBuf );
    }

// ==========================================================================
// FUNCTION: ReadIdArrayL
// ==========================================================================
void CMessageStoreSession::ReadIdArrayL( const RMessage2& aMessage, TInt aIndex, RArray<TContainerId>& aArray )
    {
    TMsgStoreId id;
    TPckg<TMsgStoreId> idPckg( id );
    TInt position = 0;

    aArray.Reset();
    for( position = 0; position < aMessage.GetDesLengthL( aIndex ); position += idPckg.Length() )
        {
        aMessage.ReadL( aIndex, idPckg, position );      
        aArray.AppendL( id );
        } // end for
    }

/**
 * 
 */
/*private*/
CBufferedMessageCreator& CMessageStoreSession::GetBufferedMessageCreatorL()
    {
    if ( NULL == iBufferedMessageCreator )
        {
        iBufferedMessageCreator = CBufferedMessageCreator::NewL(
            iServer.MessageStoreL() );
        }
    
    return *iBufferedMessageCreator;
    }

// FUNCTIONS TO SUPPORT AUTOMATED UNIT TESTING
#ifdef _DEBUG

void CMessageStoreSession::DoInjectBackupRestoreEventL( const RMessage2& aMessage )
    {
    // parameter 1 contains the event to inject
	TUint event = aMessage.Int0();
    iServer.InjectBackupRestoreEventL( event );
    aMessage.Complete( KErrNone );
    }

void CMessageStoreSession::DoInjectPointSecEvent( const RMessage2& aMessage )
    {
    // parameter 1 contains the event to inject
    TUint event = aMessage.Int0();
    iServer.InjectPointSecEvent( event );
    aMessage.Complete( KErrNone );
    }
	
void CMessageStoreSession::DoSimulateLowDiskSpaceL( const RMessage2& aMessage )
    {    
    // parameter 1 contains the number of calls to delay before indicating low disk space.
	TUint latency = aMessage.Int0();
    iServer.MessageStoreL().SimulateLowDiskSpace( latency );
    aMessage.Complete( KErrNone );
    }

void CMessageStoreSession::DoGetEncryptionStateL( const RMessage2& aMessage )
    {
    TInt state = iServer.MessageStoreL().GetEncryptionStateL();
    
    TPckg<TInt> statePckg( state );
    aMessage.WriteL( 0, statePckg );
    
    aMessage.Complete( KErrNone );
    }

void CMessageStoreSession::DoShutdownServer( const RMessage2& aMessage )
    {
    aMessage.Complete( KErrNone );
    iServer.Shutdown();
    }

#endif
