// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if !defined (__IMAPCMDS_H__)
#define __IMAPCMDS_H__

#if !defined (__MSVSTD_HRH__)
#include <msvstd.hrh>
#endif

class TImap4RenameFolder
/** Buffer for an IMAP folder name. 

@see KIMAP4MTMRenameFolder

@publishedAll
@released
*/
	{
public:
	/** IMAP folder name. */
	TBuf<128> iNewName;
	};

/** IMAP4-specific commands that can be issued through either 
CImap4ClientMtm::InvokeAsyncFunctionL() or, in a few cases, 
CImap4ClientMtm::InvokeSyncFunctionL().
 
These functions take an input message selection parameter 
(@c const CMsvEntrySelection& @c aSelection) and an input/output buffer parameter 
(TDes8& @c aParameter). The use of these parameters is command specific, and is 
described for each command below.

The commands fall into a number of categories: 

- Connect and logon to remote server:
  - #KIMAP4MTMConnect
  - #KIMAP4MTMDisconnect
- Synchronise headers:
  - #KIMAP4MTMCancelBackgroundSynchronise
  - #KIMAP4MTMConnectAndSyncCompleteAfterConnect
  - #KIMAP4MTMConnectAndSyncCompleteAfterDisconnect
  - #KIMAP4MTMConnectAndSyncCompleteAfterFullSync
  - #KIMAP4MTMConnectAndSynchronise
  - #KIMAP4MTMFolderFullSync
  - #KIMAP4MTMFullSync
  - #KIMAP4MTMInboxNewSync
  - #KIMAP4MTMSynchronise
  - #KIMAP4MTMWaitForBackground
- Subscribe to mailboxes:
  - #KIMAP4MTMLocalSubscribe
  - #KIMAP4MTMLocalUnsubscribe
- Populate messages:
  - #KIMAP4MTMPopulate
  - #KIMAP4MTMPopulateAllMailWhenAlreadyConnected
  - #KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected
  - #KIMAP4MTMPopulateNewMailWhenAlreadyConnected
  - #KIMAP4MTMConnectAndPopulateAllMailAndDisconnect
  - #KIMAP4MTMConnectAndPopulateAllMailAndStayOnline
  - #KIMAP4MTMConnectAndPopulateMailSelectionAndDisconnect
  - #KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline
  - #KIMAP4MTMConnectAndPopulateNewMailAndDisconnect
  - #KIMAP4MTMConnectAndPopulateNewMailAndStayOnline
- Copy messages:
  - #KIMAP4MTMCopyAllMailWhenAlreadyConnected
  - #KIMAP4MTMCopyMailSelectionWhenAlreadyConnected
  - #KIMAP4MTMCopyNewMailWhenAlreadyConnected
  - #KIMAP4MTMConnectAndCopyAllMailAndDisconnect
  - #KIMAP4MTMConnectAndCopyAllMailAndStayOnline
  - #KIMAP4MTMConnectAndCopyMailSelectionAndDisconnect
  - #KIMAP4MTMConnectAndCopyMailSelectionAndStayOnline
  - #KIMAP4MTMConnectAndCopyNewMailAndDisconnect
  - #KIMAP4MTMConnectAndCopyNewMailAndStayOnline
- Move messages:
  - #KIMAP4MTMMoveAllMailWhenAlreadyConnected
  - #KIMAP4MTMMoveMailSelectionWhenAlreadyConnected
  - #KIMAP4MTMMoveNewMailWhenAlreadyConnected
  - #KIMAP4MTMConnectAndMoveAllMailAndDisconnect
  - #KIMAP4MTMConnectAndMoveAllMailAndStayOnline
  - #KIMAP4MTMConnectAndMoveMailSelectionAndDisconnect
  - #KIMAP4MTMConnectAndMoveMailSelectionAndStayOnline
  - #KIMAP4MTMConnectAndMoveNewMailAndDisconnect
  - #KIMAP4MTMConnectAndMoveNewMailAndStayOnline
- Create messages:
  - #KIMAP4MTMCreateForwardAsAttachmentEmailMessage
  - #KIMAP4MTMCreateForwardEmailMessage
  - #KIMAP4MTMCreateNewEmailMessage
  - #KIMAP4MTMCreateReceiptEmailMessage
  - #KIMAP4MTMCreateReplyEmailMessage
  .
  Note that CImEmailOperation is usually a more convenient way
  to create messages.
- Query state:
  - #KIMAP4MTMBusy
  - #KIMAP4MTMIsConnected
- Offline operations:
  - #KIMAP4MTMCancelOffLineOperations
  - #KIMAP4MTMUndeleteAll
- Miscellaneous:
  - #KIMAP4MTMEndBatch
  - #KIMAP4MTMRenameFolder
  - #KIMAP4MTMSelect
  - #KIMAP4MTMStartBatch

Note that when @c iServiceSelection is referred to in code examples for some
commands, this is assumed to be a CMsvEntrySelection that  
contain the entry ID of an IMAP service. 

@see CImap4ClientMtm::InvokeAsyncFunctionL() 
@see CImap4ClientMtm::InvokeSyncFunctionL()

@publishedAll
@released
*/	
enum TImap4Cmds {
	/** Tests if a specified service is connected.
	
	This command is synchronous, and used from @c InvokeSyncFunctionL().
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service 
	to test.
	 
	@c aParameter: a packaged TInt. On return, this is set to  
	KErrNone if the service is connected or KErrDisconnected if not. 
	
	Example:
	@code
	TPckgBuf<TInt> parameter;
	iClientMtm->InvokeSyncFunctionL(KIMAP4MTMIsConnected, *iServiceSelection, parameter);
	if (parameter()==KErrNone)
		{
		_LIT(KConnStatus, "\n\n Connected to the service");
		iConsole->Printf(KConnStatus);
		}	
	@endcode
	*/
	KIMAP4MTMIsConnected = KMtmFirstFreeMtmFunctionId,
	
	/** Connects to the specified IMAP service.

	This makes a network connection and logs on the IMAP server specified 
	in the settings for the service. 
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service 
	to connect to.
	 
	@c aParameter: packages a pointer to an implementation of the connection observer 
	interface, MMsvImapConnectionObserver. Callbacks are made to this interface
	to notify the client of the stages of connection.
	
	Completion: KErrBusy if the device is already connected to the specified server.

	Example:
	@code
	TBuf8<1> parameter;
	iOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnect, *iServiceSelection, parameter, iStatus);    
	@endcode	
	*/
	KIMAP4MTMConnect,
	
	/** Connects to the specified IMAP service, and starts a background synchronisation.

	The call completes when the connection occurs and the synchronisation starts. See
	CImap4ClientMtm for a description of a background synchronisation.
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service 
	to connect to.
	 
	@c aParameter: packages a pointer to an implementation of the connection observer 
	interface, MMsvImapConnectionObserver. Callbacks are made to this interface
	to notify the client of the stages of connection and synchronisation.
	
	Completion: KErrBusy if the device is already connected to the specified server. 
	
	Example:
	@code
	TPckg<MMsvImapConnectionObserver*> parameter(this);	
  iOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndSynchronise, *iServiceSelection, parameter, iStatus);
	@endcode
	*/
	KIMAP4MTMConnectAndSynchronise,
	
	/** Cancels a background synchronisation in progress.
		
	@c aSelection: @c aSelection[0] should contain the entry ID of the service 
	being synchronised.
	 
	@c aParameter: unused.
	
	Completion: KErrCancel if a synchronisation was cancelled; KErrNone if no
	synchronisation was taking place.   

	Example:
	@code
	TBuf8<1> parameter;
  iOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMCancelBackgroundSynchronise, *iServiceSelection, parameter, iStatus);	
	@endcode			
	*/
	KIMAP4MTMCancelBackgroundSynchronise,
	
	/** Ensures that the IMAP server will remain loaded ready for more commands. 
	
	If this is not set, then the Message Server can unload the IMAP server MTM
	after a service is disconnected. It is more efficient if it stays loaded
	if further use of the MTM is expected.
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service 
	 
	@c aParameter: unused.
	
	Completion: KErrNone.
	*/
	KIMAP4MTMStartBatch,
	
	/** Ends the effect of KIMAP4MTMStartBatch. 

	@c aSelection: @c aSelection[0] should contain the TMsvId of the IMAP4service.
	 
	@c aParameter: unused.
	
	Completion: KErrNone.	
	*/
	KIMAP4MTMEndBatch,
	
	/** Tests if the server is busy.

	It can be used from either CImap4ClientMtm::InvokeAsyncFunctionL() or 
	CImap4ClientMtm::InvokeSyncFunctionL().
	
	@c aSelection: @c aSelection[0] should contain the TMsvId of the IMAP4service.
	 
	@c aParameter: if the command is called from InvokeSyncFunctionL(), then on return a packaged
	TInt holding the result.
		
	Completion: This completes with either KErrNone (neither session is busy) or KErrServerBusy 
	otherwise. 
	*/
	KIMAP4MTMBusy,
	
	/** Issues an IMAP SELECT command on the specified mailbox. 

	This is used to select an mailbox for the KIMAP4MTMSynchronise command.	
	
	Pre-condition: a connected service.
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the folder 
	to select.
	 
	@c aParameter: Unused.
		
	Completion: KErrDisconnected if not connected. 
	*/
	KIMAP4MTMSelect,
	
	/** Synchronises the current folder that is selected on the remote server.

	Note that after a connection is made, the Inbox folder is selected by default.
	
	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the TMsvId of the IMAP4service. 	
	 
	@c aParameter: unused.
	
	Completion: KErrDisconnected if not connected. 	
	*/
	KIMAP4MTMSynchronise,
	
	/** Synchronises the connected service's folder tree with the folders on the remote IMAP server.

	This creates and deletes folder entries in the message store under the remote service as needed,
	to match the mailboxes that exist on the remote server. Note that this step is done as part of 
	the normal service synchronisation operation.
		
	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the TMsvId of the IMAP4service.
	 
	@c aParameter: unused.

	Completion: KErrDisconnected if not connected; KErrServerBusy if a 
	background synchronisation is already in progress. 	
	*/
	KIMAP4MTMSyncTree,
	
	/** Not supported. */
	KIMAP4MTMReserved10,						// SyncSubscription
	
	/** Synchronises the specified service. 

	This is a foreground synchronisation, and completes when the synchronisation is done.
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service 
	to synchronise.
	
	@c aParameter: unused.
	
	Pre-condition: a connected service.
	
	Completion: KErrDisconnected if not connected; KErrServerBusy if a 
	background synchronisation is already in progress.
	*/
	KIMAP4MTMFullSync,
	
	/** Cancels any operations in progress and sends logout messages to server.

	If CImImap4Settings::SetDeleteEmailsWhenDisconnecting() is set,
	any messages marked as deleted while offline are deleted before logout.

	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the TMsvId of the IMAP4service.
	 
	@c aParameter: unused.

	Completion: KErrDisconnected if not connected.
	*/
	KIMAP4MTMDisconnect,

	/** Sets the local subscription flag on a specified folder. 

	This sets the local subscription flag on the folder's index entry 
	(see TMsvEmailEntry::SetLocalSubscription()), and saves the changed entry 
	to the index.
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the folder 
	to subscribe to.
	 
	@c aParameter: unused.
	
	Completion: KErrNotSupported if the entry is not a folder; KErrNone on success.   
	*/
	KIMAP4MTMLocalSubscribe,

	/** Clears the local subscription flag on the specified folder.
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the folder 
	to unsubscribe from.
	 
	@c aParameter: unused.

	Completion: KErrNotSupported if the entry is not a folder; KErrNone on success.   
	*/
	KIMAP4MTMLocalUnsubscribe,
    
	/** Not supported. */
	KIMAP4MTMReserved15,						// MakeRemoteMailboxesInvisible
	/** Not supported. */
	KIMAP4MTMReserved16,						// MakeRemoteMailboxesVisible
	
	/** Synchronises any messages received by the remote server Inbox since the
	last synchronisation operation.

	Messages that have been deleted from the remote Inbox are not deleted from
	the mirror.

	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the TMsvId of the IMAP4service.
	 
	@c aParameter: unused.

	Completion: KErrDisconnected if not connected; KErrServerBusy if a 
	background synchronisation is already in progress.
	*/
	KIMAP4MTMInboxNewSync,
	
	/** Selects and synchronises the specified folder. 
	
	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the entry ID of the folder to synchronise.
	 
	@c aParameter: unused.

	Completion: KErrDisconnected if not connected.
	*/
	KIMAP4MTMFolderFullSync,
	
	/** Completes only when a background synchronisation has finished. 
		
	This turns a background synchronisation into a foreground synchronisation.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service 
	being synchronised.
	 
	@c aParameter: unused.
	
	Completion: KErrNone when no background synchronisation is in progress.   
	
	Example:
	@code
	TBuf8<1> parameter;
	iOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMWaitForBackground, *iServiceSelection, parameter, iStatus);	
	@endcode
	*/
	KIMAP4MTMWaitForBackground,
	
	/** Renames a specified folder.
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the folder to rename. 
	 
	@c aParameter: the new name for the folder. This should be a packaged TImap4RenameFolder object.

	Pre-conditions: a connected service.
			
	Completion: KErrDisconnected if there is no connected service. 
	
	Example:
	@code
	// rename folder, assumed to be identified in *selection[0], to "newname".
	_LIT(KNewName, "newname");
	TImap4RenameFolder newName;
	newName.iNewName = KNewName;	
	TPckg<TImap4RenameFolder> name(newName);
	iOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMRenameFolder, *selection, name, iStatus);	
	@endcode
	 */
	KIMAP4MTMRenameFolder,
		
	/** Undoes any offline delete operations pending on the specified entries.
		
	@c aSelection: Entries on which to cancel offline delete operations. 
	 
	@c aParameter: Unused.

	Completion: KErrNone.
	*/
	KIMAP4MTMUndeleteAll,
	
	/** Cancels any offline operations pending on the specified entries.
	
	If any item is a folder, then it cancels all operations queued on any item 
	in that folder. If an IMAP4 service ID is given, then it cancels any Copy() 
	or MoveFromLocal() operations that are queued. A message can not have its 
	pending operations cancelled individually. 
	
	@c aSelection: Entries on which to cancel offline operations. 
	 
	@c aParameter: Unused.

	Completion: KErrNone.	
	*/
	KIMAP4MTMCancelOffLineOperations,

	/** Fetches the specified messages from the remote server to the mirror. 
		
	@c aSelection: Messages to fetch.

	@c aParameter: A packaged TImImap4GetPartialMailInfo or TImImap4GetMailInfo
	specifying the message parts to fetch and size limits on what is fetched.

	*/
	KIMAP4MTMPopulate,

	/** Not supported. */
	KIMAP4MTMSyncSubscription,

	/** Connect and synchronise the specified service, and complete after connection. 
	
	The operation completes when the connection part completes. The synchronisation
	is done in the background. 
	
	The command	has the same functionality as KIMAP4MTMConnectAndSynchronise.  

	@c aSelection: @c aSelection[0] should contain the entry ID of the service 
	to connect to.
	 
	@c aParameter: packages a pointer to an implementation of the connection observer 
	interface, MMsvImapConnectionObserver. Callbacks are made to this interface
	to notify the client of the stages of connection and synchronisation.
	
	Completion: KErrBusy if a connected session already exists. 	
	*/
	KIMAP4MTMConnectAndSyncCompleteAfterConnect,
	
	/** Connect and synchronise the specified service, and complete after synchronisation. 
	
	This is similar to KIMAP4MTMConnectAndSynchronise, except that the operation
	does not complete until synchronisation is complete.  
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service 
	to connect to.
	 
	@c aParameter: packages a pointer to an implementation of the connection observer 
	interface, MMsvImapConnectionObserver. Callbacks are made to this interface
	to notify the client of the stages of connection and synchronisation.
	
	Completion: KErrBusy if a connected session already exists. 	
	*/
	KIMAP4MTMConnectAndSyncCompleteAfterFullSync,
	
	/** Connect and synchronise the specified service, and complete after disconnection.
	
	Unlike the other synchronisation commands, this command does not complete
	after either connection or synchronisation. Instead, after the initial synchronisation
	is complete, it periodically resynchronises the Inbox until the service is disconnected.
	CImImap4Settings::SyncRate() sets the refresh period. 
	
	Disconnection can occur because the network connection is closed, either through 
	user action, or because a networking timeout occurs on the socket. 
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service
	to connect to.

	@c aParameter: packages a pointer to an implementation of the connection
	observer interface, MMsvImapConnectionObserver. Callbacks are made to this
	interface to notify the client of the stages of connection and
	synchronisation.

	Completion: KErrBusy if a connected session already exists.
	*/
	KIMAP4MTMConnectAndSyncCompleteAfterDisconnect,
	
	/** Not supported. */
	KIMAP4MTMCancelSendOnNextConnection,
	
	/** Copy any new messages from a specified folder to another folder,
	assuming a new connection does not need to be made.

	This is used after message headers have been synchronised. Any messages marked
	as new are populated in the mirror, and then copied to the specified destination folder.

	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder from which
	to copy the entries.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	copy the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no new entries, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMCopyNewMailWhenAlreadyConnected,
	
	/** Connect to a specified service, copy any new messages from 
	a specified folder into another folder, and stay connected. 
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to copy.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	copy the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no new entries, then KErrNone, and progress
	information is a null descriptor.			
	*/
	KIMAP4MTMConnectAndCopyNewMailAndStayOnline,
	
	/** Connect to a specified service, copy any new messages from 
	a specified folder into another folder, and disconnect afterwards. 
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to copy.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	copy the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no new entries, then KErrNone, and progress
	information is a null descriptor.			
	*/
	KIMAP4MTMConnectAndCopyNewMailAndDisconnect,
		
	/** Move any new messages from a specified folder to another folder,
	assuming a new connection does not need to be made.

	This is used after message headers have been synchronised. Any messages marked
	as new are populated in the mirror, and then moved to the specified destination folder.

	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder from which
	to move the entries.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	move the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not moved. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no new entries, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMMoveNewMailWhenAlreadyConnected,

	/** Connect to a specified service, move any new messages from 
	a specified folder into another folder, and stay connected. 
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to move.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	move the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no new entries, then KErrNone, and progress
	information is a null descriptor.			
	*/
	KIMAP4MTMConnectAndMoveNewMailAndStayOnline,
	
	/** Connect to a specified service, move any new messages from 
	a specified folder into another folder, and disconnect afterwards. 
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to move.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	move the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no new entries, then KErrNone, and progress
	information is a null descriptor.			
	*/
	KIMAP4MTMConnectAndMoveNewMailAndDisconnect,

	/** Copy the specified messages to another folder, assuming a new connection
	does not need to be made.

	The specified messages are populated in the mirror, and then copied to
	the specified destination folder.

	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service,
	followed by the IDs of the messages to copy.	

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	copy the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no entries to copy, then KErrNone, and progress
	information is a null descriptor.
	*/
	KIMAP4MTMCopyMailSelectionWhenAlreadyConnected,

	/** Connect to a specified service, copy the specified messages to another 
	folder, and stay connected.

	The specified messages are populated in the mirror, and then copied to
	the specified destination folder.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service,
	followed by the IDs of the messages to copy.	

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	copy the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no entries to copy, then KErrNone, and progress
	information is a null descriptor.
	*/
	KIMAP4MTMConnectAndCopyMailSelectionAndStayOnline,

	/** Connect to a specified service, copy the specified messages to another 
	folder, and disconnect afterwards.

	The specified messages are populated in the mirror, and then copied to
	the specified destination folder.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service,
	followed by the IDs of the messages to copy.	

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	copy the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no entries to copy, then KErrNone, and progress
	information is a null descriptor.
	*/
	KIMAP4MTMConnectAndCopyMailSelectionAndDisconnect,

	/** Move the specified messages to another folder, assuming a new connection 
	does not need to be made. 

	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service,
	followed by the IDs of the messages to move.	

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	move the messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not moved. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no entries to move, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMMoveMailSelectionWhenAlreadyConnected,
	
	/** Connect to a specified service, move the specified messages to another 
	folder, and stay connected.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service,
	followed by the IDs of the messages to move.	

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	move the messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not moved. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no entries to move, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMConnectAndMoveMailSelectionAndStayOnline,

	/** Connect to a specified service, move the specified messages to another 
	folder, and disconnect afterwards.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service,
	followed by the IDs of the messages to move.	

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	move the messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not moved. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no entries to move, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMConnectAndMoveMailSelectionAndDisconnect,

	/** Copy all messages from the current service, assuming a new connection does 
	not need to be made. 

	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder from which
	to copy the entries.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	copy the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no entries to copy, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMCopyAllMailWhenAlreadyConnected,

	/** Connect to a specified service, copy all messages from the current service, 
	and stay connected. 

	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder from which
	to copy the entries.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	copy the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no entries to copy, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMConnectAndCopyAllMailAndStayOnline,

	/** Connect to a specified service, copy all messages from the current service, 
	and disconnect afterwards. 

	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder from which
	to copy the entries.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	copy the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no entries to copy, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMConnectAndCopyAllMailAndDisconnect,

	/** Move all messages from the specified folder, assuming a new connection does 
	not need to be made. 

	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder from which
	to move the entries.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	move the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not moved. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no entries to move, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMMoveAllMailWhenAlreadyConnected,

	/** Connect to a specified service, move all messages from 
	a specified folder into another folder, and stay connected. 
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to move.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	move the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no new entries, then KErrNone, and progress
	information is a null descriptor.			
	*/	
	KIMAP4MTMConnectAndMoveAllMailAndStayOnline,
	
	/** Connect to a specified service, move all messages from 
	a specified folder into another folder, and disconnect afterwards. 
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to move.

	@c aParameter: packaged TImImap4GetMailInfo object.
	TImImap4GetMailInfo::iDestinationFolder specifies the folder into which to
	move the new messages. Messages with a size greater than TImImap4GetMailInfo::iMaxEmailSize 
	are not copied. TImImap4GetMailInfo::iGetMailBodyParts is ignored:
	the entire message is always fetched.

	Completion: note if there are no new entries, then KErrNone, and progress
	information is a null descriptor.			
	*/	
	KIMAP4MTMConnectAndMoveAllMailAndDisconnect,

	/** Populate the specified messages, assuming a new connection does not need to be made. 

	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service,
	followed by the IDs of the messages to populate.
	
	@c aParameter: A packaged TImImap4GetPartialMailInfo or TImImap4GetMailInfo
	specifying the message parts to fetch and size limits on what is fetched.

	Completion: note if there are no entries to populate, then KErrNone, and progress
	information is a null descriptor.		
	*/
	KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected,

	/** Connect to a specified service, populate the specified messages, 
	and stay connected.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service,
	followed by the IDs of the messages to populate.
	
	@c aParameter: A packaged TImImap4GetPartialMailInfo or TImImap4GetMailInfo
	specifying the message parts to fetch and size limits on what is fetched.

	Completion: note if there are no entries to populate, then KErrNone, and progress
	information is a null descriptor.		
	*/
	KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline,
	
	/** Connect to a specified service, populate the specified messages, 
	and disconnect afterwards.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service,
	followed by the IDs of the messages to populate.
	
	@c aParameter: A packaged TImImap4GetPartialMailInfo or TImImap4GetMailInfo
	specifying the message parts to fetch and size limits on what is fetched.

	Completion: note if there are no entries to populate, then KErrNone, and progress
	information is a null descriptor.		
	*/
	KIMAP4MTMConnectAndPopulateMailSelectionAndDisconnect,

	/** Populate the new messages in a specified folder, assuming a new connection does 
	not need to be made. 
	
	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to populate.

	@c aParameter: A packaged TImImap4GetPartialMailInfo or TImImap4GetMailInfo
	specifying the message parts to fetch and size limits on what is fetched.

	Completion: note if there are no entries to populate, then KErrNone, and progress
	information is a null descriptor.			
	*/
	KIMAP4MTMPopulateNewMailWhenAlreadyConnected,

	/** Populate all messages in a specified folder, assuming a new connection does 
	not need to be made. 

	Pre-condition: a connected service.

	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to populate.
	
	@c aParameter: A packaged TImImap4GetPartialMailInfo or TImImap4GetMailInfo
	specifying the message parts to fetch and size limits on what is fetched.

	Completion: note if there are no entries to populate, then KErrNone, and progress
	information is a null descriptor.		
	*/
	KIMAP4MTMPopulateAllMailWhenAlreadyConnected,

	/** Connect to a specified service, populate the new messages in a specified 
	folder, and stay connected. 
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to populate.

	@c aParameter: A packaged TImImap4GetPartialMailInfo or TImImap4GetMailInfo
	specifying the message parts to fetch and size limits on what is fetched.

	Completion: note if there are no entries to populate, then KErrNone, and progress
	information is a null descriptor.			
	*/
	KIMAP4MTMConnectAndPopulateNewMailAndStayOnline,
	
	/** Connect to a specified service, populate the new messages in a specified 
	folder, and disconnect afterwards. 
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to populate.

	@c aParameter: A packaged TImImap4GetPartialMailInfo or TImImap4GetMailInfo
	specifying the message parts to fetch and size limits on what is fetched.

	Completion: note if there are no entries to populate, then KErrNone, and progress
	information is a null descriptor.			
	*/
	KIMAP4MTMConnectAndPopulateNewMailAndDisconnect,

	/** Connect to a specified service, populate all messages in a specified 
	folder, and stay connected. 

	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to populate.
	
	@c aParameter: A packaged TImImap4GetPartialMailInfo or TImImap4GetMailInfo
	specifying the message parts to fetch and size limits on what is fetched.

	Completion: note if there are no entries to populate, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMConnectAndPopulateAllMailAndStayOnline,
	
	/** Connect to a specified service, populate all messages in a specified 
	folder, and disconnect afterwards. 

	@c aSelection: @c aSelection[0] should contain the entry ID of the service;
	@c aSelection[1] should contain the entry ID of the folder containing
	the messages to populate.
	
	@c aParameter: A packaged TImImap4GetPartialMailInfo or TImImap4GetMailInfo
	specifying the message parts to fetch and size limits on what is fetched.

	Completion: note if there are no entries to populate, then KErrNone, and progress
	information is a null descriptor.	
	*/
	KIMAP4MTMConnectAndPopulateAllMailAndDisconnect,

	/** Creates a new email message. 

	@c aSelection: @c aSelection[0] should contain the entry ID of the folder in 
	which to create the message.
	 
	@c aParameter: a packaged TImCreateMessageOptions (see miutmsg.h) object, specifying
	message creation flags and message type.
	
	The operation object returned is a CImEmailOperation. Final progress information 
	from this contains the ID of the newly created message.
	*/
	KIMAP4MTMCreateNewEmailMessage,
	
	/** Creates a new reply message. 

	@c aSelection: @c aSelection[0] should contain the entry ID of the folder in 
	which to create the message; @c aSelection[1] should contain the entry ID 
	of the message to which to create a reply.
	 
	@c aParameter: a packaged TImCreateMessageOptions (see miutmsg.h) object, specifying
	message creation flags and message type.
	
	The operation object returned is a CImEmailOperation. Final progress information 
	from this contains the ID of the newly created message.
	*/
	KIMAP4MTMCreateReplyEmailMessage,
		
	/** Creates a new forwarded message.
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the folder in 
	which to create the message; @c aSelection[1] should contain the entry ID 
	of the message to forward.
	 
	@c aParameter: a packaged TImCreateMessageOptions (see miutmsg.h) object, specifying
	message creation flags and message type.
	
	The operation object returned is a CImEmailOperation. Final progress information 
	from this contains the ID of the newly created message.
	*/
	KIMAP4MTMCreateForwardEmailMessage,

	/** Creates a new forwarded message, with the original message as an attachment.
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the folder in 
	which to create the message; @c aSelection[1] should contain the entry ID 
	of the message to forward.
	 
	@c aParameter: a packaged TImCreateMessageOptions (see miutmsg.h) object, specifying
	message creation flags and message type.
	
	The operation object returned is a CImEmailOperation. Final progress information 
	from this contains the ID of the newly created message.
	*/
	KIMAP4MTMCreateForwardAsAttachmentEmailMessage,

	/** Creates a new message-receipt message.
	
	@c aSelection: @c aSelection[0] should contain the entry ID of the folder in 
	which to create the message; @c aSelection[1] should contain the entry ID 
	of the original message for which the receipt is required.
	 
	@c aParameter: a packaged TImCreateMessageOptions (see miutmsg.h) object, specifying
	message creation flags and message type.
	
	The operation object returned is a CImEmailOperation. Final progress information 
	from this contains the ID of the newly created message.
	*/
	KIMAP4MTMCreateReceiptEmailMessage
};

#endif
