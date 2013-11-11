// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
 @released
*/

#ifndef __TIMMOBILITYTESTMTMSTATE_H__
#define __TIMMOBILITYTESTMTMSTATE_H__

/**
Defines the set of states where the MTM will make a call into the test
framework to see whether some mobility action should be performed.

Note that if you add to this, you also need to add to the lists which
are found in ....
*/
enum TImMobilityTestMtmState
	{
	KMobilityTestMtmStateNone,
	KMobilityTestMtmStateInitialReject,

	// IMAP MTM states
	KMobilityTestMtmStateImapConnect,
	KMobilityTestMtmStateImapIdle,				// this is the first IDLE call after bg sync completes

	KMobilityTestMtmStateImapCopyFromLocal1,	// APPEND message
	KMobilityTestMtmStateImapCopyFromLocal2, 	// SELECT destination folder
	KMobilityTestMtmStateImapCopyFromLocal3,	// sync'ing destination folder
	
	KMobilityTestMtmStateImapCopyToLocal1,		// SELECT source folder
	KMobilityTestMtmStateImapCopyToLocal2,		// FETCHing the message
	KMobilityTestMtmStateImapCopyToLocal3,		// local async copy
	KMobilityTestMtmStateImapCopyToLocal4,		//(10) STORE /deleted flag (move only)
	KMobilityTestMtmStateImapCopyToLocal5,		// EXPUNGE (move only)
	KMobilityTestMtmStateImapCopyToLocal6,		// CLOSE FOLDER (move only)
	KMobilityTestMtmStateImapCopyToLocal7,		// SELECT FOLDER (move only)
	
	KMobilityTestMtmStateImapCopyWithinService1,// SELECT source mailbox
	KMobilityTestMtmStateImapCopyWithinService2,// COPY message(s)
	KMobilityTestMtmStateImapCopyWithinService3,// STORE /deleted flag (move only)
	KMobilityTestMtmStateImapCopyWithinService4,// EXPUNGE source folder (move only)
	KMobilityTestMtmStateImapCopyWithinService5,// SELECT destination folder
	KMobilityTestMtmStateImapCopyWithinService6,// sync'ing destination folder
	KMobilityTestMtmStateImapCopyWithinService7,//(20) CLOSE folder to delete messages
	KMobilityTestMtmStateImapCopyWithinService8,// SELECT folder after close to delete messages

	KMobilityTestMtmStateImapCreateFolder,		// CREATE command issued

	KMobilityTestMtmStateImapDelete1,			// SELECT source folder
	KMobilityTestMtmStateImapDelete2,			// STORE /deleted command
	KMobilityTestMtmStateImapDelete3,			// EXPUNGE following a delete
	KMobilityTestMtmStateImapDelete4,			// CLOSE FOLDER following a delete
	KMobilityTestMtmStateImapDelete5,			// SELECT FOLDER following close to delete

	KMobilityTestMtmStateImapDeleteFolder1, 	// SELECT source folder
	KMobilityTestMtmStateImapDeleteFolder2, 	// STORE /deleted (all messages)
	KMobilityTestMtmStateImapDeleteFolder3, 	//(30) CLOSE folder
	KMobilityTestMtmStateImapDeleteFolder4, 	// DELETE folder
	
	KMobilityTestMtmStateImapDisconnect1,		// sync'ing late deletes
	KMobilityTestMtmStateImapDisconnect2,		// LOGOUT etc
	
	KMobilityTestMtmStateImapRename1, 			// RENAME issued
	KMobilityTestMtmStateImapRename2, 			// SUBSCRIBE issued
	
	KMobilityTestMtmStateImapSyncFolder1,		// SELECT issued
 	KMobilityTestMtmStateImapSyncFolder2,		// sync'ing the folder
 	
 	KMobilityTestMtmStateImapSyncFolderTree,	// sync'ing the folder tree 	
	
	KMobilityTestMtmImapSyncEInboxEarlyDeletes,
	KMobilityTestMtmImapSyncEInboxEarlyExpunge, //(40)
	KMobilityTestMtmImapSyncEInboxSync,
	KMobilityTestMtmImapSyncESyncTree,
	KMobilityTestMtmImapSyncESyncSubscriptions,
	KMobilityTestMtmImapSyncEFolderEarlyDeletes,
	KMobilityTestMtmImapSyncEFolderEarlyExpunge,
	KMobilityTestMtmImapSyncESyncFolder,
	KMobilityTestMtmImapEInboxLateDeletes,
	KMobilityTestMtmImapSyncEFolderLateDeletes,
	KMobilityTestMtmImapSyncEFolderLateExpunge,

	// POP MTM states
	KMobilityTestMtmStatePopConnecting = 300,
	KMobilityTestMtmStatePopRefreshing,
	KMobilityTestMtmStatePopTidying,
	KMobilityTestMtmStatePopFindingFirstOfflineOp,
	KMobilityTestMtmStatePopCancellingOfflineOps,
	KMobilityTestMtmStatePopCopying,
	KMobilityTestMtmStatePopMoving,
	KMobilityTestMtmStatePopPopulating,
	KMobilityTestMtmStatePopTopPopulating,
	KMobilityTestMtmStatePopDeleting,
	KMobilityTestMtmStatePopConnectedAndIdle,
	KMobilityTestMtmStatePopQuitting,
	KMobilityTestMtmStatePopRetrieving,

	//SMTP MTM states
	KMobilityTestMtmStateSmtpConnectingToSmtp = 500,
	KMobilityTestMtmStateSmtpWaitingForReply,
	KMobilityTestMtmStateSmtpAuthorisingSmtp,
	KMobilityTestMtmStateSmtpAuthInProgress,
	KMobilityTestMtmStateSmtpSendingStartTls,
	KMobilityTestMtmStateSmtpSettingSecurity,
	KMobilityTestMtmStateSmtpResetSmtp,
	KMobilityTestMtmStateSmtpClosingSmtp,
	KMobilityTestMtmStateSmtpSendFile

	// Do not put any values at 1000 or above as this range is used
	// elsewhere in the test framework
	};

#endif //__TIMMOBILITYTESTMTMSTATE_H__
