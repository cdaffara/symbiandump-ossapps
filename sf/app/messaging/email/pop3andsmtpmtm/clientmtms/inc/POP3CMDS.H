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

#if !defined (__POP3CMDS_H__)
#define __POP3CMDS_H__

#if !defined (__MSVSTD_HRH__)
#include <msvstd.hrh>
#endif

/** POP3-specific commands that can be issued through CPop3ClientMtm::InvokeAsyncFunctionL(). 
@publishedAll
@released
*/
enum TPop3Cmds 
	{
	/** Unsupported. */
	KPOP3MTMIsConnected = KMtmFirstFreeMtmFunctionId,
	/** Attempt to connect to the current service. */
	KPOP3MTMConnect,
	/** Disconnect from the current service. */
	KPOP3MTMDisconnect,
	/** Unused. */
	KPOP3MTMCreateReceipt,

	// need to pass in destination in aParameter
	/** Copy any new messages from the current service, assuming a new connection does 
	not need to be made. */
	KPOP3MTMCopyNewMailWhenAlreadyConnected,
	/** Copy any new messages from the current service, and do not disconnect afterwards. */
	KPOP3MTMConnectAndCopyNewMailAndStayOnline,
	/** Copy any new messages from the current service, and disconnect afterwards. */
	KPOP3MTMConnectAndCopyNewMailAndDisconnect,
	/** Move any new messages from the current service, assuming a new connection does 
	not need to be made. */
	KPOP3MTMMoveNewMailWhenAlreadyConnected,
	/** Move any new messages from the current service, and do not disconnect afterwards. */
	KPOP3MTMConnectAndMoveNewMailAndStayOnline,
	/** Connect to current service, move any new messages, and disconnect afterwards. */
	KPOP3MTMConnectAndMoveNewMailAndDisconnect,

	// need to pass in destination in aParameter
	/** Copy the specified messages from the current service, assuming a new connection 
	does not need to be made. */
	KPOP3MTMCopyMailSelectionWhenAlreadyConnected,
	/** Connect to current service, copy the specified messages, and do not disconnect 
	afterwards. */
	KPOP3MTMConnectAndCopyMailSelectionAndStayOnline,
	/** Connect to current service, copy the specified messages, and disconnect afterwards. */
	KPOP3MTMConnectAndCopyMailSelectionAndDisconnect,
	/** Move the specified messages from the current service, assuming a new connection 
	does not need to be made. */
	KPOP3MTMMoveMailSelectionWhenAlreadyConnected,
	/** Connect to current service, move the specified messages, and do not disconnect 
	afterwards. */
	KPOP3MTMConnectAndMoveMailSelectionAndStayOnline,
	/** Connect to current service, move the specified messages, and disconnect afterwards. */
	KPOP3MTMConnectAndMoveMailSelectionAndDisconnect,

	// need to pass in destination in aParameter
	/** Copy all messages from the current service, assuming a new connection does 
	not need to be made. */
	KPOP3MTMCopyAllMailWhenAlreadyConnected,
	/** Connect to current service, copy all messages, and do not disconnect afterwards. */
	KPOP3MTMConnectAndCopyAllMailAndStayOnline,
	/** Connect to current service, copy all messages, and disconnect afterwards. */
	KPOP3MTMConnectAndCopyAllMailAndDisconnect,
	/** Move all messages from the current service, assuming a new connection does 
	not need to be made. */
	KPOP3MTMMoveAllMailWhenAlreadyConnected,
	/** Connect to current service, move all messages, and do not disconnect afterwards. */
	KPOP3MTMConnectAndMoveAllMailAndStayOnline,
	/** Connect to current service, move all messages, and disconnect afterwards. */
	KPOP3MTMConnectAndMoveAllMailAndDisconnect,

	/** Cancel all offline operations. */
	KPOP3MTMCancelOfflineOperations,
	/** Unused. */
	KPOP3MTMCancelSendOnNextConnection,
	/** Creates a new message. */
	KPOP3MTMCreateNewEmailMessage,
	/** Creates a new reply message. */
	KPOP3MTMCreateReplyEmailMessage,
	/** Creates a new forwarded message. */
	KPOP3MTMCreateForwardEmailMessage,
	/** Creates a new forwarded message as an attachment. */
	KPOP3MTMCreateForwardAsAttachmentEmailMessage,
	/** Creates a new message-receipt message. */
	KPOP3MTMCreateReceiptEmailMessage,
	/** Populates the selection of messages from the current service, assuming a new connection does 
	not need to be made. */
	KPOP3MTMPopulate,
	/** Populates any new messages from the current service, assuming a new connection does 
	not need to be made. */
	KPOP3MTMPopulateNew,
	/** Populates all messages from the current service, assuming a new connection does 
	not need to be made. */
	KPOP3MTMPopulateAll 
	};

#endif
