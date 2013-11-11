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
* Description:  Message store session client implementation.
*
*/



// ========
// INCLUDES
// ========

#include <s32mem.h>  // for descriptor read/write streams
#include <f32file.h>

#include "RMessageStoreSession.h"

#include "emailstoreuids.hrh"
#include "MessageStoreClientServer.h"
#include "RMessageStoreSession.h"
//<cmail>
#include "MsgStoreAccount.h"
#include "AccountsSerializer.h"
#include "MsgStoreSortCriteria.h"
#include "MsgStoreSearchCriteria.h"
#include "MsgStoreAddress.h"
//</cmail>

// =========
// CONSTANTS
// =========

const TUint KInitialChildrenIdsTransferBufferLength    = 5000;
const TUint KInitialPropertiesTransferBufferLength     = 10000;
const TUint KInitialPropertiesListTransferBufferLength = 15000;
const TUint KInitialAccountsTransferBufferLength       = 10000;  //shoud be more than enouth
const TUint KInitialSortedListTransferBufferLength     = 30000;
const TUint KInitialSortedIdsAndFlagsBufferLength      = 65536;  //shoud be more than enouth

// This will try to connect to the server for a maximum of 20 seconds.
const TUint KMaxServerConnectRetries         = 20;
const TUint KServerRetryIntervalMicroSeconds = 1000000;

const TUint KNumSlots = 3;

const TUint KIpcArgsIndex2 = 2;
const TUint KIpcArgsIndex3 = 3;

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
RMessageStoreSession::RMessageStoreSession()
    {
    __LOG_CONSTRUCT( "MsgClient", "RMessageStoreSession" )
    } // end constructor

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
RMessageStoreSession::~RMessageStoreSession()
    {
    __LOG_DESTRUCT
    } // end destructor

// ==========================================================================
// FUNCTION: Connect
// ==========================================================================
TInt RMessageStoreSession::Connect( TBool /*aLaunchServerIfNeeded*/, TBool aRetryIndefinitely )
    {
    __LOG_ENTER( "Connect" )

    // This flag should only be used in the case where the message store server is being launched as a separate
    // thread within some other process.  In that case, the flag is used to control exactly which process the
    // server will be launched in.  Otherwise, always launch the server on reference.
    //aLaunchServerIfNeeded = ETrue;

    TBool again = ETrue;    
    TInt  returnValue;
    TInt  retry = KMaxServerConnectRetries;
    
    while( again )
        {
        returnValue = CreateSession( KMsgStoreServerName, TVersion(KMsgStoreMajorVersion, KMsgStoreMinorVersion, KMsgStoreBuild), KNumSlots );
        
        if( returnValue == KErrNone )
            {
            // No need to retry.
            again = EFalse;            
            }
        else
            {
            __LOG_WRITE8_FORMAT1_ERROR( "CreateSession failed %i", returnValue )
            
            if( !aRetryIndefinitely )
                {                
                retry--;
                } // end if
            
            if( retry == 0 )
                {
                // Do not retry again.
                again = EFalse;
                }
            else if( (returnValue == KErrNotFound) || (returnValue == KErrServerTerminated) )
                {
                // Start the server as a thread within the current process.            
                returnValue = StartServer();
                if ( returnValue != KErrNone )
                    {
            				__LOG_WRITE8_FORMAT1_ERROR( "StartServer failed %i", returnValue )
		                // Wait before trying again.
		                User::After( KServerRetryIntervalMicroSeconds );             
		                //stay in the loop even if we failed to start the server,
		                // in the next look, the CreateSession will fail but we get to try StartServer again
                    }
                }
            else
                {
                __LOG_WRITE_ERROR( "waiting between retries" )
                
                // Wait before trying again.
                User::After( KServerRetryIntervalMicroSeconds );                             
                
                } // end if

            } // end if
            
        } // end if
    
    __LOG_WRITE8_FORMAT1_DEBUG3( "returnValue=%i", returnValue )    
    __LOG_EXIT
    return returnValue;
        
    } // end Connect
  
// ==========================================================================
// FUNCTION: StartServer
// ==========================================================================
TInt RMessageStoreSession::StartServer()
    {
    __LOG_ENTER( "StartServer" )
    
    TInt err = KErrNone;
    
    // Launch the server executable (i.e. in it its own process).
     __LOG_WRITE_INFO( "Launching server process" )
        
    // Create a new server process. Simultaneous launching of two such processes 
    // should be detected when the second one attempts to create the server 
    // object, failing with KErrAlreadyExists.
   _LIT( KMsgStoreExeName, "MessageStoreExe" );
    RProcess server;
    err = server.Create( KMsgStoreExeName, KNullDesC ); 
    
    if( err == KErrNone )
        {
        TRequestStatus stat;
        server.Rendezvous( stat );
        
        if( stat != KRequestPending )
            {            
            server.Kill( 0 );   // abort startup
            }
        else
            {            
            server.Resume();  // logon OK - start the server thread
            } // end if

        __LOG_WRITE_INFO( "Started" )
        User::WaitForRequest( stat );   // wait for start or death

        __LOG_WRITE8_FORMAT1_INFO( "stat.Int(() = %i", stat.Int() );
        __LOG_WRITE8_FORMAT1_INFO( "ExitReason() = %i", server.ExitReason() );

        TExitCategoryName exitCategory;
        exitCategory = server.ExitCategory();
        __LOG_WRITE_FORMAT1_INFO( "ExitCategory() = %S", &exitCategory );

        // we can't use the 'exit reason' if the server panicked as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone  
        err = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
        server.Close();	
        }
    else
        {
        __LOG_WRITE8_FORMAT1_ERROR( "failed to launch server, err=%i", err )
        } // end if

    __LOG_WRITE8_FORMAT1_DEBUG3( "err=%i", err )
    __LOG_EXIT
    return err;
    
    } // end StartServer

// ==========================================================================
// FUNCTION: Close
// ==========================================================================
void RMessageStoreSession::Close()
    {
    __LOG_ENTER( "Close" )
    
    RSessionBase::Close();  //basecall
    
    __LOG_EXIT
    } // end Close

// ==========================================================================
// FUNCTION: EnableEncryptionL
// ==========================================================================
void RMessageStoreSession::EnableEncryptionL()
    {
    __LOG_ENTER( "EnableEncryptionL" )
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdEnableEncryption ) );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DisableEncryptionL
// ==========================================================================
void RMessageStoreSession::DisableEncryptionL()
    {
    __LOG_ENTER( "DisableEncryptionL" )
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdDisableEncryption ) );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: AuthenticatedL
// ==========================================================================
TBool RMessageStoreSession::AuthenticatedL()
    {
    __LOG_ENTER( "AuthenticatedL" )
    
    TPckgBuf<TBool> resultPckg( EFalse );
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdAuthenticated, TIpcArgs( &resultPckg ) ) );
    
    __LOG_WRITE8_FORMAT1_DEBUG3( "result=%i", resultPckg() )
    __LOG_EXIT
    
    return resultPckg();
    
    } // end AuthenticatedL

// ==========================================================================
// FUNCTION: AuthenticateL
// ==========================================================================
TBool RMessageStoreSession::AuthenticateL( const TDesC& aPassword )
    {
    __LOG_ENTER( "AuthenticateL" )
    
    TPckgBuf<TBool> resultPckg( EFalse );
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdAuthenticate, TIpcArgs( &resultPckg, &aPassword ) ) );
    
    __LOG_WRITE8_FORMAT1_DEBUG3( "result=%i", resultPckg() )
    __LOG_EXIT
    
    return resultPckg();
    
    } // end AuthenticateL

// ==========================================================================
// FUNCTION: ClearAuthenticationL
// ==========================================================================
void RMessageStoreSession::ClearAuthenticationL()
    {
    __LOG_ENTER( "ClearAuthenticationL" )
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdClearAuthentication ) );
	
	__LOG_EXIT
    } // end ClearAuthenticationL

// ==========================================================================
// FUNCTION: HasPasswordL
// ==========================================================================
TBool RMessageStoreSession::HasPasswordL()
    {
    __LOG_ENTER( "HasPasswordL" )
    
    TPckgBuf<TBool> resultPckg( EFalse );
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdHasPassword, TIpcArgs( &resultPckg ) ) );
    
    __LOG_WRITE8_FORMAT1_DEBUG3( "result=%i", resultPckg() )   
    __LOG_EXIT
    
    return resultPckg();
    
    } // end HasPasswordL

// ==========================================================================
// FUNCTION: SetPasswordL
// ==========================================================================
void RMessageStoreSession::SetPasswordL( const TDesC& aPassword )
    {
    __LOG_ENTER( "SetPasswordL" )
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdSetPassword, TIpcArgs( &aPassword ) ) );
	
	__LOG_EXIT
    } // end SetPasswordL

// ==========================================================================
// FUNCTION: ChangePasswordL
// ==========================================================================
TBool RMessageStoreSession::ChangePasswordL( const TDesC& aOldPassword, const TDesC& aNewPassword )
    {
    __LOG_ENTER( "ChangePasswordL" )
    
    TPckgBuf<TBool> resultPckg( EFalse );
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdChangePassword, TIpcArgs( &resultPckg, &aOldPassword, &aNewPassword ) ) );
	
    __LOG_WRITE8_FORMAT1_DEBUG3( "result=%i", resultPckg() )  
    __LOG_EXIT
    
	return resultPckg();
	
    } // end ChangePasswordL

//account management
// ==========================================================================
// FUNCTION: CreateAccountL
// ==========================================================================
TMsgStoreId RMessageStoreSession::CreateAccountL( const CMsgStoreAccount& aAccount, const TDesC8& aProperties )
	{
    __LOG_ENTER( "CreateAccountL" )
    
    TPckgBuf<TMsgStoreId> resultPckg( KMsgStoreInvalidId );
    
    TInt32 ownerId = aAccount.Owner();
    const TDesC& name = aAccount.Name();
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdCreateAccount, TIpcArgs( &resultPckg, ownerId, &name, &aProperties ) ) );
	
    __LOG_WRITE8_FORMAT1_DEBUG3( "result=%u", resultPckg() )  
    __LOG_EXIT
    
	return resultPckg();
	}

// ==========================================================================
// FUNCTION: OpenAccountL
// ==========================================================================
TMsgStoreId RMessageStoreSession::OpenAccountL( const CMsgStoreAccount& aAccount, RBuf8& aBuffer )
	{
    __LOG_ENTER( "OpenAccountL" )
    
    TUint result = 1;
    TPckg<TUint> resultPckg( result );
    
	TMsgStoreOpenAccountCmdParams params;
	TPckg<TMsgStoreOpenAccountCmdParams> paramsPckg( params );
	
	params.iOwnerId = aAccount.Owner();
	
    const TDesC& name = aAccount.Name();
    
	aBuffer.CreateL( KInitialPropertiesTransferBufferLength );

    while( result != 0 )
	    {	   
	    User::LeaveIfError( SendReceive( EMsgStoreCmdOpenAccount, TIpcArgs( &resultPckg, &paramsPckg, &name, &aBuffer ) ) );
	    
	    // The buffer was too small.  Reallocate.
	    if( result != 0 )
	        {
	        __LOG_WRITE8_FORMAT1_INFO( "growing buffer to %i", result );
	        aBuffer.ReAllocL( result );
	        }
	    } // end while
	
    __LOG_WRITE8_FORMAT1_DEBUG3( "result=%u", resultPckg() )    
    __LOG_EXIT
    
	return params.iMailBoxId;
	}

// ==========================================================================
// FUNCTION: DeleteAccountL
// ==========================================================================
void RMessageStoreSession::DeleteAccountL( const CMsgStoreAccount& aAccount )
	{
    __LOG_ENTER( "DeleteAccountL" )
    
    TInt32 ownerId = aAccount.Owner();
    const TDesC& name = aAccount.Name();
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdDeleteAccount, TIpcArgs( ownerId, &name ) ) );
	
	__LOG_EXIT
	}

// ==========================================================================
// FUNCTION: RenameAccountL
// ==========================================================================
void RMessageStoreSession::RenameAccountL( TInt32 aOwnerId, const TDesC& aOldName, const TDesC& aNewName )
	{
    __LOG_ENTER( "RenameAccountL" )
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdRenameAccount, TIpcArgs( aOwnerId, &aOldName, &aNewName ) ) );
	
	__LOG_EXIT
	}

// ==========================================================================
// FUNCTION: AccountsL
// ==========================================================================
void RMessageStoreSession::AccountsL( RPointerArray<CMsgStoreAccount>& aAccounts )
	{
    __LOG_ENTER( "AccountsL" )
    
    RBuf8 resultBuf;
    CleanupClosePushL( resultBuf );
    resultBuf.CreateL( KInitialAccountsTransferBufferLength );
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdListAccounts, TIpcArgs( &resultBuf ) ) );
	
	CAccountsSerializer* serializer = new(ELeave)CAccountsSerializer();
	CleanupStack::PushL( serializer );
	
	serializer->DeserializeL( resultBuf );
	
	TInt count = serializer->Count();
	RArray<TInt32>& ownerIds = serializer->OwnerIds();
    RPointerArray<HBufC>& names = serializer->NamesArray();
	
	for ( int i = 0 ; i < count ; i++ )
		{
		CMsgStoreAccount* account = CMsgStoreAccount::NewL( ownerIds[i], *(names[i]) );
		aAccounts.AppendL( account );
		}
	CleanupStack::PopAndDestroy( serializer );
	CleanupStack::PopAndDestroy( &resultBuf );
	
	__LOG_EXIT
	}

// ==========================================================================
// FUNCTION: StartObservingL
// ==========================================================================
void RMessageStoreSession::StartObservingL()
	{
	__LOG_ENTER( "StartObservingL" )
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdStartObserving ) );
	
	__LOG_EXIT
	} // end StartObservingL

// ==========================================================================
// FUNCTION: GetEvents
// ==========================================================================
void RMessageStoreSession::GetEvents( TRequestStatus& aStatus, TDes8& aBuffer )
	{
	__LOG_ENTER( "GetEvents" )
	
	aBuffer.SetLength( 0 );
    SendReceive( EMsgStoreCmdGetEvents, TIpcArgs( &aBuffer ), aStatus );
    
    __LOG_EXIT
	} // end GetEvents
		
// ==========================================================================
// FUNCTION: StopObserving
// ==========================================================================
void RMessageStoreSession::StopObserving()
	{
	__LOG_ENTER( "StopObserving" )
	
	SendReceive( EMsgStoreCmdStopObserving );
	
	__LOG_EXIT
	} // end StopObserving
		
// ==========================================================================
// FUNCTION: CreateContainerL
// ==========================================================================
TMsgStoreId RMessageStoreSession::CreateContainerL( TMsgStoreId             aParentId,
													TMsgStoreId             aGrandparentId,
													TMsgStoreId             aMailBoxId,
													TMsgStoreContainerMasks aType,
								                    const TDesC8&           aProperties,
								                    TBool                   aCommitContainer,
								                    const TDesC&            aContentFilename )
	{
	__LOG_ENTER( "CreateContainerL" )

	TMsgStoreCreateContainerCmdParams params;
	TPckg<TMsgStoreCreateContainerCmdParams> paramsPckg( params );
	
	params.iParentId        = aParentId;
	params.iGrandparentId   = aGrandparentId;
	params.iMailBoxId       = aMailBoxId;
	params.iCommitContainer = aCommitContainer;
	params.iType            = aType;
	params.iHasContent      = ( aContentFilename.Length() > 0 );
	
	TIpcArgs ipcArgs( &paramsPckg, &aProperties );

    // Platform security documentation recommends using different file system session for
    // each file handle that is transferred between processes.
    RFs fs;
    CleanupClosePushL( fs );

    RFile file;
    CleanupClosePushL( file );
	
	if( params.iHasContent )
	    {
        User::LeaveIfError( fs.Connect() );
        fs.ShareProtected();        
        
        User::LeaveIfError( file.Open( fs, aContentFilename, EFileShareReadersOnly ) );
        User::LeaveIfError( file.TransferToServer( ipcArgs, KIpcArgsIndex2, KIpcArgsIndex3 ) );	    	    
	    } // end if

	User::LeaveIfError( SendReceive( EMsgStoreCmdCreateContainer, ipcArgs ) );

	CleanupStack::PopAndDestroy( &file );
	CleanupStack::PopAndDestroy( &fs );
		
    __LOG_WRITE8_FORMAT1_DEBUG3( "id=%x", params.iId )
    __LOG_EXIT
    
	return params.iId;
	
	} // end CreateContainerL


// ==========================================================================
// FUNCTION: CreateContainerL
// ==========================================================================
TMsgStoreId RMessageStoreSession::CreateContainerL( TMsgStoreId             aParentId,
													TMsgStoreId             aGrandparentId,
													TMsgStoreId             aMailBoxId,
													TMsgStoreContainerMasks aType,
								                    const TDesC8&           aProperties,
								                    TBool                   aCommitContainer,
								                    RFile&                  aFile )
	{
	__LOG_ENTER( "CreateContainerL" )

	TMsgStoreCreateContainerCmdParams params;
	TPckg<TMsgStoreCreateContainerCmdParams> paramsPckg( params );
	
    CleanupClosePushL( aFile );
	
	params.iParentId        = aParentId;
	params.iGrandparentId   = aGrandparentId;
	params.iMailBoxId       = aMailBoxId;
	params.iCommitContainer = aCommitContainer;
	params.iType            = aType;
	params.iHasContent      = ETrue;
	
	TIpcArgs ipcArgs( &paramsPckg, &aProperties );

    User::LeaveIfError( aFile.TransferToServer( ipcArgs, 2, 3 ) );	    	    

	User::LeaveIfError( SendReceive( EMsgStoreCmdCreateContainer, ipcArgs ) );
	
	CleanupStack::PopAndDestroy( &aFile );

    __LOG_WRITE8_FORMAT1_DEBUG3( "id=%x", params.iId )
    __LOG_EXIT
    
	return params.iId;
	
	} // end CreateContainerL

// ==========================================================================
// FUNCTION: CommitContainerL
// ==========================================================================
void RMessageStoreSession::CommitContainerL( TMsgStoreId aId,
 	 									     TMsgStoreId aDestinationId,
 	 									     TMsgStoreId aMailBoxId)
	{
	__LOG_ENTER( "CommitContainerL" )

	User::LeaveIfError( SendReceive( EMsgStoreCmdCommitContainer, TIpcArgs( aId, aDestinationId, aMailBoxId ) ) );

	__LOG_EXIT
	} // end CommitContainerL

// ==========================================================================
// FUNCTION: AbandonContainerL
// ==========================================================================
void RMessageStoreSession::AbandonContainerL( TMsgStoreId aId )
	{
	__LOG_ENTER( "AbandonContainerL" )

	User::LeaveIfError( SendReceive( EMsgStoreCmdAbandonContainer, TIpcArgs( aId ) ) );

	__LOG_EXIT
	} // end AbandonContainerL

// ==========================================================================
// FUNCTION: MoveContainerL
// ==========================================================================
void RMessageStoreSession::MoveContainerL( TMsgStoreId aId,
										   TMsgStoreId aSourceId,
	 									   TMsgStoreId aDestinationId )
	{
	__LOG_ENTER( "MoveContainerL" )

	User::LeaveIfError( SendReceive( EMsgStoreCmdMoveContainer, TIpcArgs( aId, aSourceId, aDestinationId ) ) );

	__LOG_EXIT
	} // end MoveContainerL

// ==========================================================================
// FUNCTION: CopyContainerL
// ==========================================================================
TMsgStoreId RMessageStoreSession::CopyContainerL( TMsgStoreId aId, 
                								  TMsgStoreId aSourceId,
                								  TMsgStoreId aSourceParentId,
                							  	  TMsgStoreId aDestinationId,
                								  TMsgStoreId aDestinationParentId,
                								  TMsgStoreId aMailBoxId)
	{
	__LOG_ENTER( "CopyContainerL" )

	TPckgBuf<TMsgStoreId> idPckg;	

	TMsgStoreCopyContainerCmdParams        params;
	TPckg<TMsgStoreCopyContainerCmdParams> paramsPckg( params );
	
	params.iId                  = aId;
	params.iSourceId            = aSourceId,
	params.iSourceParentId      = aSourceParentId,
	params.iDestinationId       = aDestinationId;
	params.iDestinationParentId = aDestinationParentId;
	params.iMailBoxId           = aMailBoxId;
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdCopyContainer, TIpcArgs( &paramsPckg, &idPckg ) ) );

    __LOG_WRITE8_FORMAT1_DEBUG3( "id=%x", idPckg() )
    __LOG_EXIT
    
	return idPckg();

	} // end CopyContainerL

// ==========================================================================
// FUNCTION: ContainerPropertiesL
// ==========================================================================
void RMessageStoreSession::ContainerPropertiesL( TMsgStoreId aId, TMsgStoreId& aParentId, TMsgStoreId aGrandparentId, RBuf8& aBuffer, TMsgStoreId aMailboxId )
	{
	__LOG_ENTER( "ContainerPropertiesL" )
	
	//TMsgStoreId originalParentId = aParentId;
	
	TMsgStoreFetchPropertiesCmdParams params;
	TPckg<TMsgStoreFetchPropertiesCmdParams> paramsPckg( params );
	
	params.iId            = aId;
	params.iGrandparentId = aGrandparentId;
	params.iMailboxId = aMailboxId;
	
	TPckg<TMsgStoreId> parentIdPckg( aParentId );

    TUint result = 1;
	TPckg<TUint> resultPckg( result );

	aBuffer.CreateL( KInitialPropertiesTransferBufferLength );

    while( result != 0 )
	    {	   
	    User::LeaveIfError( SendReceive( EMsgStoreCmdFetchProperties, TIpcArgs( &paramsPckg, &parentIdPckg, &aBuffer, &resultPckg ) ) );
	    
	    // The buffer was too small.  Reallocate.
	    if( result != 0 )
	        {
	        __LOG_WRITE8_FORMAT1_INFO( "growing buffer to %i", result );
	        aBuffer.ReAllocL( result );
	        }
	    } // end while
    
	__LOG_EXIT
	} // end ContainerPropertiesL

// ==========================================================================
// FUNCTION: DeleteContainerL
// ==========================================================================
void RMessageStoreSession::DeleteContainerL( TMsgStoreId aId, TMsgStoreId aParentId, TMsgStoreId aGrandparentId, TMsgStoreId aMailBoxId )
	{
	__LOG_ENTER( "DeleteContainerL" )
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdDeleteContainer, TIpcArgs( aId, aParentId, aGrandparentId, aMailBoxId ) ) );
	
	__LOG_EXIT
	} // end DeleteContainerL

// ==========================================================================
// FUNCTION: ChildrenCountsL
// ==========================================================================
void RMessageStoreSession::ChildrenCountsL( TMsgStoreId aId, TUint& aMessageCount, TUint& aUnreadCount )
	{
	__LOG_ENTER( "ChildrenCountsL" )
	
	TMsgStoreCounts counts;
	TPckg<TMsgStoreCounts> countsPckg( counts );
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdChildrenCounts, TIpcArgs( aId, &countsPckg ) ) );

	aMessageCount = counts.iMessagesCount;
	aUnreadCount  = counts.iUnreadCount;
		
	__LOG_EXIT
	} // end ChildrenCountsL
	
// ==========================================================================
// FUNCTION: TotalCountsL
// ==========================================================================
void RMessageStoreSession::TotalCountsL( TMsgStoreId aMailBoxId, TUint& aMessageCount, TUint& aUnreadCount )
	{
	__LOG_ENTER( "TotalCountsL" )
	
	TMsgStoreCounts counts;
	TPckg<TMsgStoreCounts> countsPckg( counts );
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdTotalCounts, TIpcArgs( aMailBoxId, &countsPckg ) ) );

	aMessageCount = counts.iMessagesCount;
	aUnreadCount  = counts.iUnreadCount;
		
	__LOG_EXIT
	} // end TotalCountsL
	
// ==========================================================================
// FUNCTION: ChildrenIdsL
// ==========================================================================
void RMessageStoreSession::ChildrenIdsL( TMsgStoreId aId, TMsgStoreContainerMasks aType, RArray<TMsgStoreId>& aChildrenIds )
	{
	__LOG_ENTER( "ChildrenIdsL" )
	
	RBuf8 childrenBuf;
	childrenBuf.CreateL( KInitialChildrenIdsTransferBufferLength );
	CleanupClosePushL( childrenBuf );
	
	TUint result = 1;
	TPckg<TUint> resultPckg( result );
	
	while( result != 0 )
	    {	    	    
	    User::LeaveIfError( SendReceive( EMsgStoreCmdChildrenIds, TIpcArgs( aId, aType, &childrenBuf, &resultPckg ) ) );	
	    
	    if( result != 0 )
	        {
	        // Overflow!
	        __LOG_WRITE8_FORMAT1_INFO( "growing transfer buffer to %i", result )
	        
	        childrenBuf.ReAllocL( result );
	        } // end if
	        
	    } // end while

	RDesReadStream readStream( childrenBuf );
	CleanupClosePushL( readStream );
	
	aChildrenIds.Reset();
	const TUint KDivider = 4;
	for( TInt count = 0; count < childrenBuf.Length() / KDivider; count++ )
		{
		aChildrenIds.AppendL( readStream.ReadUint32L() );
		} // end for
		
	CleanupStack::PopAndDestroy( &readStream );
	CleanupStack::PopAndDestroy( &childrenBuf );
	
	__LOG_EXIT	
	} // end ChildrenIdsL

// ==========================================================================
// FUNCTION: UpdatePropertiesL
// ==========================================================================
void RMessageStoreSession::UpdatePropertiesL( TMsgStoreId   aId,
											  TMsgStoreId   aParentId,
											  TMsgStoreId   aMailBoxId,
	 					                      const TDesC8& aProperties )
	{
	__LOG_ENTER( "UpdatePropertiesL" )

	User::LeaveIfError( SendReceive( EMsgStoreCmdUpdateProperties, TIpcArgs( aId, aParentId, aMailBoxId, &aProperties ) ) );

	__LOG_EXIT
	} // end UpdatePropertiesL

// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
void RMessageStoreSession::UpdatePropertyL( TMsgStoreId                aId,
	 					                    TMsgStoreId                aParentId,
	 					                    TMsgStoreId                aMailBoxId,
	 					                    const TDesC8&              aName,
	 					                    TMsgStorePropertyValueType aType,
	 					                    const TDesC8&              aValue )
	{
	__LOG_ENTER( "UpdatePropertyL" )

    TMsgStoreUpdatePropertyCmdParams        params;
    TPckg<TMsgStoreUpdatePropertyCmdParams> paramsPckg( params );
    
    params.iId       = aId;
    params.iParentId = aParentId;
    params.iMailBoxId = aMailBoxId;
    params.iType     = aType;
    
	User::LeaveIfError( SendReceive( EMsgStoreCmdUpdateProperty, TIpcArgs( &paramsPckg, &aName, &aValue ) ) );

	__LOG_EXIT
	} // end UpdatePropertyL

// ==========================================================================
// FUNCTION: WipeEverything
// ==========================================================================
void RMessageStoreSession::WipeEverythingL()
	{
	__LOG_ENTER( "WipeEverything" )
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdWipeEverything ) );
	
	__LOG_EXIT		
	} // end WipeEverything

// ==========================================================================
// FUNCTION: ContentLengthL
// ==========================================================================
TUint RMessageStoreSession::ContentLengthL( TMsgStoreId aId, TMsgStoreId aParentId )
	{
	__LOG_ENTER( "ContentLengthL" )
	
	TPckgBuf<TUint> lengthPckg;
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdContentLength, TIpcArgs( aId, aParentId, &lengthPckg ) ) );
	
    __LOG_WRITE8_FORMAT1_DEBUG3( "length=%i", lengthPckg() )	
    __LOG_EXIT

	return lengthPckg();
	
	} // end ContentLengthL

// ==========================================================================
// FUNCTION: FetchContentL
// ==========================================================================
void RMessageStoreSession::FetchContentL( TMsgStoreId aId, TMsgStoreId aParentId, TDes8& aBuffer, TUint aStartOffset )
	{
	__LOG_ENTER( "FetchContentL(1)" )
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdFetchContentToBuffer, TIpcArgs( aId, aParentId, &aBuffer, aStartOffset ) ) );
	
	__LOG_EXIT
	} // end FetchContentL

// ==========================================================================
// FUNCTION: FetchContentL
// ==========================================================================
void RMessageStoreSession::FetchContentL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC& aFilename )
	{
	__LOG_ENTER( "FetchContentL(2)" )
	
    // Platform security documentation recommends using different file system session for
    // each file handle that is transferred between processes.
    RFs   fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    fs.ShareProtected();
    
    RFile file;
    User::LeaveIfError( file.Replace( fs, aFilename, EFileWrite | EFileShareAny ) );
    CleanupClosePushL( file );

    TIpcArgs ipcArgs;
        
    ipcArgs.Set( 0, aId );
    ipcArgs.Set( 1, aParentId );

    User::LeaveIfError( file.TransferToServer( ipcArgs, KIpcArgsIndex2, KIpcArgsIndex3 ) );
	
	TInt result = SendReceive( EMsgStoreCmdFetchContentToFile, ipcArgs );

	CleanupStack::PopAndDestroy( &file );

    if( result != KErrNone )
        {
        // Something went wrong.  Cleanup file.
        fs.Delete( aFilename );
        User::Leave( result );        
        } // end if
	
	CleanupStack::PopAndDestroy( &fs );	

	__LOG_EXIT
	} // end FetchContentL

// ==========================================================================
// FUNCTION: ReplaceContentL
// ==========================================================================
void RMessageStoreSession::ReplaceContentL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aContent )
	{
	__LOG_ENTER( "ReplaceContentL" )
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdReplaceContentWithBuffer, TIpcArgs( aId, aParentId, &aContent ) ) );
	
	__LOG_EXIT
	} // end ReplaceContentL

// ==========================================================================
// FUNCTION: ReplaceContentL
// ==========================================================================
void RMessageStoreSession::ReplaceContentWithFileL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC& aFilename )
	{
	__LOG_ENTER( "ReplaceContentWithFileL" )
	
    TIpcArgs ipcArgs( aId, aParentId );
    
    // Platform security documentation recommends using different file system session for
    // each file handle that is transferred between processes.
    RFs   fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    fs.ShareProtected();
    
    RFile file;
    User::LeaveIfError( file.Open( fs, aFilename, EFileRead ) );
    CleanupClosePushL( file );
    User::LeaveIfError( file.TransferToServer( ipcArgs, KIpcArgsIndex2, KIpcArgsIndex3 ) ); 
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdReplaceContentWithFile, ipcArgs ) );
	
	CleanupStack::PopAndDestroy( &file );
	CleanupStack::PopAndDestroy( &fs );
	
	__LOG_EXIT
	} // end ReplaceContentL

// ==========================================================================
// FUNCTION: AppendToContentL
// ==========================================================================
void RMessageStoreSession::AppendToContentL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aContent )
	{
	__LOG_ENTER( "AppendToContentL" )
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdAppendToContent, TIpcArgs( aId, aParentId, &aContent ) ) );
	
	__LOG_EXIT
	} // end AppendToContentL

// ==========================================================================
// FUNCTION: PrependToContentL
// ==========================================================================
void RMessageStoreSession::PrependToContentL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aContent )
    {
    __LOG_ENTER( "PrependToContentL" )
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdPrependToContent,
        TIpcArgs( aId, aParentId, &aContent ) ) );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: RemoveContentL
// ==========================================================================
void RMessageStoreSession::RemoveContentL( TMsgStoreId aId, TMsgStoreId aParentId )
	{
	__LOG_ENTER( "RemoveContentL" )
	
	User::LeaveIfError( SendReceive( EMsgStoreCmdRemoveContent, TIpcArgs( aId, aParentId ) ) );
	
	__LOG_EXIT
	} // end RemoveContentL	

// ==========================================================================
// FUNCTION: OpenOriginalContentFileL
// ==========================================================================
void RMessageStoreSession::OpenOriginalContentFileL( TMsgStoreId aId, TMsgStoreId aParentId, RFile& aFile )
    {
    __LOG_ENTER( "OpenOriginalContentFileL" )
    
    // Retrieve the RFs and RFile handles from the server
    TInt fsh;             // session (RFs) handle
    TPckgBuf<TInt> fh;    // sub-session (RFile) handle

    fsh = SendReceive( EMsgStoreCmdOpenContentFile, TIpcArgs( aId, aParentId, &fh ) );
    User::LeaveIfError( fsh );
    
    User::LeaveIfError( aFile.AdoptFromServer( fsh, fh() ) );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: ChildrenPropertiesL
// ==========================================================================
void RMessageStoreSession::ChildrenPropertiesL( TMsgStoreId             aId,
                                                TMsgStoreId             aParentId,
                                                TMsgStoreContainerMasks aType,
                                                TBool                   aQuickProperties,
                                                TBool                   aRecursive,
                                                MPropertiesArray&       aPropertiesArray )
    {
    __LOG_ENTER( "ChildrenPropertiesL" )

    TMsgStorePropertiesListCmdParams params;
    
	params.iUseQuery        = ETrue;
	params.iQuickProperties = aQuickProperties;
    
    TMsgStorePropertiesListQueryParams queryParams;
    
    queryParams.iId        = aId;
    queryParams.iParentId  = aParentId;
    queryParams.iType      = aType;
    queryParams.iRecursive = aRecursive;

    TPckg<TMsgStorePropertiesListQueryParams> queryParamsPckg( queryParams );    
    TBuf8<1> emptyBuf;

    DoPropertiesListL( params, queryParamsPckg, emptyBuf, aPropertiesArray );

    __LOG_EXIT
    } // end ChildrenPropertiesL

// ==========================================================================
// FUNCTION: DoPropertiesListL
// ==========================================================================
void RMessageStoreSession::DoPropertiesListL( TMsgStorePropertiesListCmdParams& aParams,
                                              const TDesC8&                     aIdBuffer, 
                                              const TDesC8&                     aSerializedPropertyNames, 
                                              MPropertiesArray&                 aPropertiesArray )        
    {
    __LOG_ENTER( "DoPropertiesListL" )
    
    TPckg<TMsgStorePropertiesListCmdParams> paramsPckg( aParams );
    
	RBuf8 serializedBuf;
	serializedBuf.CreateL( KInitialPropertiesListTransferBufferLength );
	CleanupClosePushL( serializedBuf );

	TMsgStoreId id;
	TPckg<TMsgStoreId> idPckg( id );
	
	TMsgStoreId parentId;			
	TPckg<TMsgStoreId> parentIdPckg( parentId );
	
	TUint32 length32;
	TPckg<TUint32> length32Pckg( length32 );

    aParams.iStartOver      = ETrue;
    aParams.iMoreProperties = ETrue;
	
	while( aParams.iMoreProperties )
	    {
	    __LOG_WRITE_INFO( "getting next chunk" )
        
        serializedBuf.SetLength( 0 );
        
		User::LeaveIfError( SendReceive( EMsgStoreCmdPropertiesList, 
		                                 TIpcArgs( &paramsPckg, 
		                                           &aIdBuffer, 
		                                           &aSerializedPropertyNames, 
		                                           &serializedBuf ) ) );

        __LOG_WRITE_INFO( "handling chunk" )
        	
	    if( aParams.iClearExisting )
	        {
	        __LOG_WRITE_INFO( "resetting local buffer" )
	        aPropertiesArray.Reset();
	        } // end if

		const TUint8* pointer = serializedBuf.Ptr();

		// Deserialize data until the current buffer is exhausted.
		while( pointer < serializedBuf.Ptr() + serializedBuf.Length() )
			{
    		idPckg.Copy( pointer, idPckg.Length() );
			pointer += idPckg.Length();
			
	        parentIdPckg.Copy( pointer, parentIdPckg.Length() );
	        pointer += parentIdPckg.Length();
						
			length32Pckg.Copy( pointer, length32Pckg.Length() );
			pointer += length32Pckg.Length();
			
			TPtrC8 propertiesBuf( pointer, length32 );
			pointer += length32;
		
		    aPropertiesArray.AddElementL( id, parentId, propertiesBuf );			
			} // end while			
			
		serializedBuf.SetLength( 0 );	
    	
        if( aParams.iNewBufferSize > 0 )
            {
            __LOG_WRITE8_FORMAT1_INFO( "growing buffer to %i", aParams.iNewBufferSize );	        
	        serializedBuf.ReAllocL( aParams.iNewBufferSize );                
            } // end if

        aParams.iStartOver = EFalse;

		} // end while
		
	__LOG_WRITE_INFO( "done" );
	
	CleanupStack::PopAndDestroy( &serializedBuf );	
	
	__LOG_EXIT
    } // end DoPropertiesListL
	
// ==========================================================================
// FUNCTION: SortL
// ==========================================================================
TMsgStoreId RMessageStoreSession::SortL( RMsgStoreSortCriteria& aSortCriteria, TMsgStoreId aMailBoxId, TBool aInMemorySort )
    {    
    __LOG_ENTER( "SortL" )
    
    TPckgBuf<TMsgStoreId> resultPckg( KMsgStoreInvalidId );
    
    TMsgStoreSortCriteria params;
    TPckg<TMsgStoreSortCriteria> paramsPckg( params );
    
    params.iMailBoxId          = aMailBoxId;
    params.iFolderId           = aSortCriteria.iFolderId;
    params.iSortBy             = aSortCriteria.iSortBy;
    params.iSortOrder          = aSortCriteria.iSortOrder;
    params.iSecondarySortOrder = aSortCriteria.iSecondarySortOrder;
    
    // Serialize property names.
    TUint16 nameLength;
    TPckg<TUint16> nameLengthPckg( nameLength );      
    
    const RPointerArray<TDesC8>& aPropertyNames = aSortCriteria.PropertyKeys();
    
    TInt totalLength = 0;
    for( TInt i = 0; i < aPropertyNames.Count(); i++ )
        {
        totalLength += (aPropertyNames[i]->Length() + nameLengthPckg.Length());
        } // end for
        
    RBuf8 propertyNamesBuffer;            
    CleanupClosePushL( propertyNamesBuffer );
    propertyNamesBuffer.CreateL( totalLength );
 
    for( TInt i = 0; i < aPropertyNames.Count(); i++ )
        {
        nameLength = aPropertyNames[i]->Length();
        propertyNamesBuffer.Append( nameLengthPckg );
        propertyNamesBuffer.Append( *aPropertyNames[i] );
        } // end for
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdStartSorting, TIpcArgs( &resultPckg, &paramsPckg, &propertyNamesBuffer, aInMemorySort ) ) );
    
    CleanupStack::PopAndDestroy( &propertyNamesBuffer );

    __LOG_EXIT
    
    return resultPckg();
    }

// ==========================================================================
// FUNCTION: GetSortedRowsL
// ==========================================================================
TBool RMessageStoreSession::GetSortedRowsL( TMsgStoreId                aSortSessionId,
                                            TMsgStoreId                aCurrentMessageId, 
                                            TMsgStoreIteratorDirection aDirection,
                                            TUint                      aCount, 
                                            MPropertiesArray&          aPropertiesArray,
                                            const TDesC&               aStartWith,
                                            TBool                      aSkipCurrentGroup )
    {
    __LOG_ENTER( "GetSortedRowsL" )
    
    TUint result = 1;
    TPckg<TUint> resultPckg( result );
    
    TMsgStoreGetSortedRowsCmdParams params;
    TPckg<TMsgStoreGetSortedRowsCmdParams> paramsPckg( params );
    
    params.iSortSessionId     = aSortSessionId;
    params.iStartingMessageId = aCurrentMessageId;
    params.iDirection         = aDirection;
    params.iRowsToRetrieve    = aCount;
    params.iHasStartWith      = aStartWith != KNullDesC;
    params.iSkipCurrentGroup  = aSkipCurrentGroup;
    params.iHasMoreRows       = EFalse;
    
    RBuf8 serializedBuf;
    CleanupClosePushL( serializedBuf );
    serializedBuf.CreateL( KInitialSortedListTransferBufferLength );

    while( result != 0 )
        {      
        if ( aStartWith == KNullDesC )
            {
            User::LeaveIfError( SendReceive( EMsgStoreCmdGetSortedRows, TIpcArgs( &resultPckg, &paramsPckg, &serializedBuf ) ) );
            }
        else
            {
            User::LeaveIfError( SendReceive( EMsgStoreCmdGetSortedRows, TIpcArgs( &resultPckg, &paramsPckg, &serializedBuf, &aStartWith ) ) );
            }
        
        // The buffer was too small.  Reallocate.
        if( result != 0 )
            {
            __LOG_WRITE8_FORMAT1_INFO( "growing buffer to %i", result );
            serializedBuf.ReAllocL( result );
            }
        } // end while
    
    // Deserialize data until the current buffer is exhausted.
    TMsgStoreId id;
    TPckg<TMsgStoreId> idPckg( id );
    
    TMsgStoreId parentId;           
    TPckg<TMsgStoreId> parentIdPckg( parentId );
    
    TUint32 length32;
    TPckg<TUint32> length32Pckg( length32 );
   
    const TUint8* pointer = serializedBuf.Ptr();
    
    while( pointer < serializedBuf.Ptr() + serializedBuf.Length() )
        {
        idPckg.Copy( pointer, idPckg.Length() );
        pointer += idPckg.Length();
        
        parentIdPckg.Copy( pointer, parentIdPckg.Length() );
        pointer += parentIdPckg.Length();
                    
        length32Pckg.Copy( pointer, length32Pckg.Length() );
        pointer += length32Pckg.Length();
        
        TPtrC8 propertiesBuf( pointer, length32 );
        pointer += length32;
    
        aPropertiesArray.AddElementL( id, parentId, propertiesBuf );            
        } // end while          
    
    __LOG_EXIT

    CleanupStack::PopAndDestroy( &serializedBuf );
    
    return params.iHasMoreRows;
    }

// ==========================================================================
// FUNCTION: SortIteratorGroupCountL
// ==========================================================================
TInt RMessageStoreSession::SortIteratorGroupCountL( TMsgStoreId aSortSessionId, RArray<TUint>& aItemsInGroup )
    {
    __LOG_ENTER("SortIteratorGroupCountL")
    
    TInt result = 0;
    TPckg<TInt> resultPckg( result );
    
    RBuf8 countsBuf;
    CleanupClosePushL( countsBuf );
    countsBuf.CreateL( KInitialSortedListTransferBufferLength );
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdGetIteratorGroupCount, TIpcArgs( aSortSessionId, &resultPckg, &countsBuf ) ) );
    
    TUint count;
    TPckg<TUint> countPckg( count );
    const TUint8* pointer = countsBuf.Ptr();
    
    if ( result > 0 )
        {
        while( pointer < countsBuf.Ptr() + countsBuf.Length() )
            {
            countPckg.Copy( pointer, countPckg.Length() );
            pointer += countPckg.Length();
            
            aItemsInGroup.AppendL( count );        
            }
        }
    
    CleanupStack::PopAndDestroy( &countsBuf );
    
    __LOG_EXIT
    
    return result;
    }

// ==========================================================================
// FUNCTION: SortedIdsAndFlagsL
// ==========================================================================
void RMessageStoreSession::SortedIdsAndFlagsL( TMsgStoreId aSortSessionId, RArray<TMsgStoreIdAndFlag>& aIdsAndFlags )
	{
    __LOG_ENTER("SortedIdsAndFlagsL")
    
    TInt count = 0;
    TPckg<TInt> countPckg( count );
    
    RBuf8 resultBuf;
    CleanupClosePushL( resultBuf );
    resultBuf.CreateL( KInitialSortedIdsAndFlagsBufferLength );
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdGetIteratorIdsAndFlags, TIpcArgs( aSortSessionId, &countPckg, &resultBuf ) ) );
    
    TMsgStoreId id;
    TPckg<TMsgStoreId> idPckg( id );
    
    TUint32 flag;
    TPckg<TUint32> flagPckg( flag );
    
    const TUint8* pointer = resultBuf.Ptr();
    
    if ( count > 0 )
        {
        while( pointer < resultBuf.Ptr() + resultBuf.Length() )
            {
            idPckg.Copy( pointer, idPckg.Length() );
            pointer += idPckg.Length();
            
            flagPckg.Copy( pointer, flagPckg.Length() );
            pointer += flagPckg.Length();
            
            aIdsAndFlags.AppendL( TMsgStoreIdAndFlag( id, flag ) );        
            }
        }
    
    CleanupStack::PopAndDestroy( &resultBuf );
    
    __LOG_EXIT
	}


// ==========================================================================
// FUNCTION: SortedIndexOfL
// ==========================================================================
TInt RMessageStoreSession::SortedIndexOfL( TMsgStoreId aSortSessionId, TMsgStoreId aMessageId )
    {
    __LOG_ENTER("SortedIndexOfL")
    
    TInt index = 0;
    TPckg<TInt> indexPckg( index );
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdSortedIndexOf, TIpcArgs( aSortSessionId, aMessageId, &indexPckg ) ) );
    
    __LOG_EXIT
    
    return index;
    }

// ==========================================================================
// FUNCTION: SortedIdsL
// ==========================================================================
void RMessageStoreSession::SortedIdsL( TMsgStoreId aSortSessionId, RArray<TMsgStoreId>& aMessageIds )
	{
    __LOG_ENTER("SortedIdsL")
    
    TInt count = 0;
    TPckg<TInt> countPckg( count );
    
    RBuf8 resultBuf;
    CleanupClosePushL( resultBuf );
    resultBuf.CreateL( KInitialSortedIdsAndFlagsBufferLength );
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdGetIteratorIds, TIpcArgs( aSortSessionId, &countPckg, &resultBuf ) ) );
    
    TMsgStoreId id;
    TPckg<TMsgStoreId> idPckg( id );
    
    const TUint8* pointer = resultBuf.Ptr();
    
    if ( count > 0 )
        {
        while( pointer < resultBuf.Ptr() + resultBuf.Length() )
            {
            idPckg.Copy( pointer, idPckg.Length() );
            pointer += idPckg.Length();
            
            aMessageIds.AppendL( id );        
            }
        }
    
    CleanupStack::PopAndDestroy( &resultBuf );
    
    __LOG_EXIT
	}

// ==========================================================================
// FUNCTION: IdsAndGroupCountL
// ==========================================================================
void RMessageStoreSession::IdsAndGroupCountL( TMsgStoreId aSortSessionId, RArray<TMsgStoreId>& aMessageIds, RArray<TUint>& aItemsInGroup )
    {
    __LOG_ENTER("IdsAndGroupCountL")
    
    TInt idCount = 0;
    TPckg<TInt> idCountPckg( idCount );
    TInt groupCount = 0;
    TPckg<TInt> groupCountPckg( groupCount );
    
    RBuf8 resultBuf;
    CleanupClosePushL( resultBuf );
    resultBuf.CreateL( KInitialSortedIdsAndFlagsBufferLength );
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdGetIteratorIdsAndGroupCount, TIpcArgs( aSortSessionId, &idCountPckg, &groupCountPckg, &resultBuf ) ) );

    //get the ID array
    TMsgStoreId id;
    TPckg<TMsgStoreId> idPckg( id );
    
    const TUint8* pointer = resultBuf.Ptr();
    
    while( (pointer < resultBuf.Ptr() + resultBuf.Length() ) && (idCount > 0) )
        {
        idPckg.Copy( pointer, idPckg.Length() );
        pointer += idPckg.Length();
        
        aMessageIds.AppendL( id );
        --idCount;
        }
    
    //get the count array
    TUint count;
    TPckg<TUint> countPckg( count );
    while( ( pointer < resultBuf.Ptr() + resultBuf.Length() ) && (groupCount > 0) )
        {
        countPckg.Copy( pointer, countPckg.Length() );
        pointer += countPckg.Length();
        
        aItemsInGroup.AppendL( count ); 
        --groupCount;
        }
    
    CleanupStack::PopAndDestroy( &resultBuf );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: EndSortL
// ==========================================================================
void RMessageStoreSession::EndSortL( TMsgStoreId aSortSessionId )
    {
    __LOG_ENTER("EndSortL")
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdEndSorting, TIpcArgs( aSortSessionId ) ) );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: PrepareSearchL
// ==========================================================================
void RMessageStoreSession::PrepareSearchL( RMsgStoreSearchCriteria& aSearchCriteria )
    {
    __LOG_ENTER( "PrepareSearchL" )
    
    TMsgStoreSearchCmdParams params;
    TPckg<TMsgStoreSearchCmdParams> paramsPckg( params );
    
    params.iSearchFields       = aSearchCriteria.iSearchFields;
    params.iSortBy             = aSearchCriteria.iSortBy;
    params.iSortOrder          = aSearchCriteria.iSortOrder;
    params.iSecondarySortOrder = aSearchCriteria.iSecondarySortOrder;
   
    // Serialize ID list.
    TMsgStoreId        id;
    TPckg<TMsgStoreId> idPckg( id );
    
    RArray<TMsgStoreId>& aIds = aSearchCriteria.FolderIds();
    
    RBuf8 idBuffer;            
    idBuffer.CreateL( aIds.Count() * idPckg.Length() );
    CleanupClosePushL( idBuffer );
    
    for( TInt i = 0; i < aIds.Count(); i++ )
        {
        id = aIds[i];
        idBuffer.Append( idPckg );
        } // end for

    //serialize the search names
    TUint16 searchStringLen;
    TPckg<TUint16> searchStringLenPckg( searchStringLen ); 
    
    const RPointerArray<TDesC>& searchStrings = aSearchCriteria.SearchStrings();
    
    TInt totalLength = 0;
    for( TInt i = 0; i < searchStrings.Count(); i++ )
        {
        totalLength += (searchStrings[i]->Length() * 2 + searchStringLenPckg.Length());
        } // end for
    
    RBuf8 searchStringsBuffer;            
    searchStringsBuffer.CreateL( totalLength );
    CleanupClosePushL( searchStringsBuffer );

    for( TInt i = 0; i < searchStrings.Count(); i++ )
        {
        TDesC& val = *searchStrings[i];
        searchStringLen = val.Length() * 2;
        searchStringsBuffer.Append( searchStringLenPckg );
        
        const TUint8* valuePtr = reinterpret_cast<const TUint8*>( val.Ptr() );
        TPtrC8 valueDes8( valuePtr, searchStringLen );  
        searchStringsBuffer.Append( valueDes8 );
        } // end for
    
    // Serialize property names.
    TUint16 nameLength;
    TPckg<TUint16> nameLengthPckg( nameLength );        

    const RPointerArray<TDesC8>& aPropertyNames = aSearchCriteria.PropertyKeys();
    
    totalLength = 0;
    for( TInt i = 0; i < aPropertyNames.Count(); i++ )
        {
        totalLength += (aPropertyNames[i]->Length() + nameLengthPckg.Length());
        } // end for
    
    RBuf8 propertyNamesBuffer;            
    propertyNamesBuffer.CreateL( totalLength );
    CleanupClosePushL( propertyNamesBuffer );

    for( TInt i = 0; i < aPropertyNames.Count(); i++ )
        {
        nameLength = aPropertyNames[i]->Length();
        propertyNamesBuffer.Append( nameLengthPckg );
        propertyNamesBuffer.Append( *aPropertyNames[i] );
        } // end for
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdPrepareSearch, TIpcArgs( &paramsPckg, &searchStringsBuffer, &idBuffer, &propertyNamesBuffer ) ) );
    
    CleanupStack::PopAndDestroy( &propertyNamesBuffer );
    CleanupStack::PopAndDestroy( &searchStringsBuffer );
    CleanupStack::PopAndDestroy( &idBuffer );
	
	__LOG_EXIT
    } // end PrepareSearchL

// ==========================================================================
// FUNCTION: GetMatchesL
// ==========================================================================

void RMessageStoreSession::GetMatchesL( TRequestStatus& aStatus, TDes8& aBuffer )
    {
    __LOG_ENTER( "GetMatchesL" )
    
    aBuffer.SetLength( 0 );
	SendReceive( EMsgStoreCmdGetMatches, TIpcArgs( &aBuffer ), aStatus );    
	
	__LOG_EXIT
    } // end GetMatchesL

// ==========================================================================
// FUNCTION: CancelSearch
// ==========================================================================
void RMessageStoreSession::CancelSearch()
    {
    __LOG_ENTER( "CancelSearch" )
    
	SendReceive( EMsgStoreCmdCancelSearch );        
	
	__LOG_EXIT
    } // end CancelSearch

// ==========================================================================
// FUNCTION: CleanupSearchResultCache
// ==========================================================================
void RMessageStoreSession::ClearSearchResultCache()
    {
    __LOG_ENTER( "CleanupSearchResultCache" )
    
    SendReceive( EMsgStoreCmdClearSearchResultCache );        
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: SetMaxMruAddressCountL
// ==========================================================================
void RMessageStoreSession::SetMaxMruAddressCountL( TInt aMaxCount )
    {
    __LOG_ENTER( "SetMaxMruAddressCountL" )
    
    SendReceive( EMsgStoreCmdSetMaxMruAddressCount, TIpcArgs( aMaxCount ) );        
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: AddMruAddressesL
// ==========================================================================
void RMessageStoreSession::AddMruAddressesL( TMsgStoreId aMailBoxId, RPointerArray<CMsgStoreAddress>& aAddressArray )
    {
    __LOG_ENTER( "AddMruAddressesL" )
    
    //find out the size needed for serializing the addresses
    TInt totalSize = 0 ;
    TInt count = aAddressArray.Count();
    TInt sizeOfLengthFields = sizeof( TUint16 ) * 2;  //two lengths
    for ( TInt i = 0 ; i < count ; i++ )
        {
        totalSize += sizeOfLengthFields;
        CMsgStoreAddress* address = aAddressArray[i];
        totalSize += (address->EmailAddress().Length() * 2);
        totalSize += (address->DisplayName().Length() * 2);
        }
    
    TUint16 length;
    TPckg<TUint16> lengthPckg( length );                        
    
    RBuf8 serializedBuf;
    CleanupClosePushL( serializedBuf );
    serializedBuf.CreateL( totalSize );
    
    for( TInt i = 0 ; i < count ; i++ )
        {
        CMsgStoreAddress* address = aAddressArray[i];
        
        //write the email address
        //length first
        length = address->EmailAddress().Length() * 2;
        serializedBuf.Append( lengthPckg );
        
        //convert the 16 bit descriptor to 8 bit descriptor
        const TUint8* addressPtr = reinterpret_cast<const TUint8*>( address->EmailAddress().Ptr() );
        TPtrC8 addressDes( addressPtr, length );  
        serializedBuf.Append( addressDes );
        
        //write the displayName
        //length first
        length = address->DisplayName().Length() * 2;
        serializedBuf.Append( lengthPckg );
        
        //convert the 16 bit descriptor to 8 bit descriptor
        const TUint8* namePtr = reinterpret_cast<const TUint8*>( address->DisplayName().Ptr() );
        TPtrC8 nameDes( namePtr, length );  
        serializedBuf.Append( nameDes );
        }
    SendReceive( EMsgStoreCmdSetMruAddressList, TIpcArgs( aMailBoxId, &serializedBuf ) );        
    
    CleanupStack::PopAndDestroy( &serializedBuf );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: MruAddressesL
// ==========================================================================
void RMessageStoreSession::MruAddressesL( TMsgStoreId aMailBoxId, RPointerArray<CMsgStoreAddress>& aAddressArray )
    {
    __LOG_ENTER( "MruAddressesL" )
    
    TUint result = 1;
    TPckg<TUint> resultPckg( result );
    
    RBuf8 serializedBuf;
    CleanupClosePushL( serializedBuf );
    serializedBuf.CreateL( KInitialPropertiesTransferBufferLength );
    
    while( result != 0 )
        {      
        User::LeaveIfError( SendReceive( EMsgStoreCmdGetMruAddressList, TIpcArgs( aMailBoxId, &resultPckg, &serializedBuf ) ) );
        
        // The buffer was too small.  Reallocate.
        if( result != 0 )
            {
            __LOG_WRITE8_FORMAT1_INFO( "growing buffer to %i", result );
            serializedBuf.ReAllocL( result );
            }
        } // end while

    TUint16 length;
    TPckg<TUint16> lengthPckg( length );                        
    
    const TUint8* pointer = serializedBuf.Ptr();
    const TUint8* endPointer = serializedBuf.Ptr() + serializedBuf.Length();

    // Deserialize data until the current buffer is exhausted.
    while( pointer < endPointer )
        {
        //get the length of email address
        lengthPckg.Copy( pointer, lengthPckg.Length() );
        pointer += lengthPckg.Length();
        
        //get the 16 bit email address
        const TUint16* addrPtr16 = reinterpret_cast<const TUint16*>( pointer );
        TPtrC16 addrDes16( addrPtr16, length/2 );  
        pointer += length;
                    
        //get the length of display name
        lengthPckg.Copy( pointer, lengthPckg.Length() );
        pointer += lengthPckg.Length();
        
        //get the 16 bit display name
        const TUint16* namePtr16 = reinterpret_cast<const TUint16*>( pointer );
        TPtrC16 nameDes16( namePtr16, length/2 );  
        pointer += length;
    
        //create the address object and add it to the output array
        CMsgStoreAddress *address = CMsgStoreAddress::NewL( addrDes16, nameDes16 );
        aAddressArray.AppendL( address );            
        } // end while          
    
    CleanupStack::PopAndDestroy( &serializedBuf );
    
    __LOG_EXIT
    }


// ==========================================================================
// FUNCTION: ContainerTypeById
// ==========================================================================
TMsgStoreContainerType RMessageStoreSession::ContainerTypeById( TMsgStoreId aId )
    {
    
    TMsgStoreContainerType resultType = EMsgStoreUnknownContainer;
    
    TMsgStoreId type = aId & EMsgStoreContainerMask;
    
    switch( type )
        {
        case EMsgStoreMailBoxBits:
            resultType = EMsgStoreMailboxContainer;
            break;
            
        case EMsgStoreFolderBits:
            resultType = EMsgStoreFolderContainer;
            break;
            
        case EMsgStoreMessageBits:
            resultType = EMsgStoreMessageContainer;
            break;
            
        case EMsgStorePartBits:
            resultType = EMsgStorePartContainer;
            break;
        }
    
    return resultType;
    }


// ==========================================================================
// FUNCTION: PropertiesL
// ==========================================================================
void RMessageStoreSession::PropertiesL( const RArray<TMsgStoreId>&   aIds, 
                                        const RPointerArray<TDesC8>& aPropertyNames, 
                                        MPropertiesArray&            aPropertiesArray )
    {
    __LOG_ENTER( "PropertiesL" )
    
    TMsgStorePropertiesListCmdParams params;
    
	params.iUseQuery        = EFalse;
	params.iQuickProperties = EFalse;   

    // Serialize ID list.
    TMsgStoreId        id;
    TPckg<TMsgStoreId> idPckg( id );
    
    RBuf8 idBuffer;            
    idBuffer.CreateL( aIds.Count() * idPckg.Length() );
    CleanupClosePushL( idBuffer );
    
    for( TInt i = 0; i < aIds.Count(); i++ )
        {
        id = aIds[i];
        idBuffer.Append( idPckg );
        } // end for

    // Serialize property names.
    TUint16 nameLength;
    TPckg<TUint16> nameLengthPckg( nameLength );                        
    
    TInt totalLength = 0;
    for( TInt i = 0; i < aPropertyNames.Count(); i++ )
        {
        totalLength += (aPropertyNames[i]->Length() + nameLengthPckg.Length());
        } // end for
        
    RBuf8 propertyNamesBuffer;            
    propertyNamesBuffer.CreateL( totalLength );
    CleanupClosePushL( propertyNamesBuffer );

    for( TInt i = 0; i < aPropertyNames.Count(); i++ )
        {
        nameLength = aPropertyNames[i]->Length();
        propertyNamesBuffer.Append( nameLengthPckg );
        propertyNamesBuffer.Append( *aPropertyNames[i] );
        } // end for
                
    // Call server.
    DoPropertiesListL( params, idBuffer, propertyNamesBuffer, aPropertiesArray );

    CleanupStack::PopAndDestroy( &propertyNamesBuffer );
    CleanupStack::PopAndDestroy( &idBuffer );
    	
	__LOG_EXIT    
    } // end PropertiesL

/**
 * 
 */
void RMessageStoreSession::BeginBatchInsertL()
    {
    User::LeaveIfError( SendReceive( EMsgStoreCmdBeginBatchInsert ) );
    }

/**
 * 
 */
void RMessageStoreSession::FinishBatchInsertL()
    {
    User::LeaveIfError( SendReceive( EMsgStoreCmdFinishBatchInsert ) );
    }
