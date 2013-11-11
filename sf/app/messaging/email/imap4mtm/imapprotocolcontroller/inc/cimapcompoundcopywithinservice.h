// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CIMAPCOMPOUNDCOPYWITHINSERVICE_H__
#define __CIMAPCOMPOUNDCOPYWITHINSERVICE_H__

#include "cimapcompoundbase.h"
#include "tmessageflaginfo.h"

// forward declarations
class CImapFolder;

/**
The compound object for the copy or move within the service operation. 
The copy within service operation copies or moves a selection of 
messages from one or more remote folders to another.

Stop for migrate policy:
   The state machine will be stopped when messages from the current
   source folder have completed the operation of being copied/moved to the
   destination folder IF there are messages from other source folders to
   be copied.
   If there are no more messages from other source folders to be copied,
   the operation is allowed to complete as normal.
  

@internalTechnology
@prototype
*/
class CImapCompoundCopyWithinService : public CImapCompoundBase
	{
public:
	~CImapCompoundCopyWithinService();
	
	static CImapCompoundCopyWithinService* NewL( CImapSyncManager& aSyncManager, 
												 CMsvServerEntry& aServerEntry, 
												 CImapSettings& aImapSettings,
												 TBool aIsMove, 
												 const CMsvEntrySelection& aSourceSel, 
												 const TMsvId aDestination );
	
	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	
private:
	CImapCompoundCopyWithinService( CImapSyncManager& aSyncManager, 
									CMsvServerEntry& aServerEntry, 
									CImapSettings& aImapSettings,
									TBool aIsMove,
									const TMsvId aDestination );
	void ConstructL(const CMsvEntrySelection& aSourceSel);
	
	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();

	TBool SetSourceFolderL(const TMsvId aMessage);

	void CopyMessagesL();	
	void MarkMessageForDeletesL();
	void DeleteMovedMessagesL();
	void IncrementProgress(TInt aNum);
	
private:
	/** ETrue if it is a move operation. The value is set during
	creation of the compound operation. */
	TBool 	iIsMove;
	
	/** The ID of the destination folder were all messages in
	iSourceSel will be copied/moved to. */
	TMsvId  iDestinationId;
	
	/** The Source folder containing the selection of messages to operate on
	Not Owned - can only copy from subscribed folders */
	CImapFolder* iSourceFolder;			
	
	/** The destination folder where the selection of messages will
	be copied/moved to.
	Owned - can copy to non-subscribed folders */
	CImapFolder* iDestinationFolder;
	
	/** The ID of the source folder containing the message currently
	operating on. */
	TMsvId	iSourceFolderId;
	
	/** The message ID of the message currently operating on. */
	TMsvId  iCurrentMsgId;
	
	/** Selection of messages to operate on. */
	CMsvEntrySelection* iSourceSel;
	
	/** UID sequence that will be copied/moved */
	HBufC8* iUidSeq;
	
	// Progress Counts
	TInt iTotalSize;

	/** Total number of messages to move/copy */
	TInt iMsgsToDo;
	
	/** Number of messages moved/copied */
	TInt iMsgsDone;
	
	/** Number of messages copied in last command to server
	this is the number that needs to be deleted from the 
	current source when performing a move operation */
	TInt iMsgsCopied;	
	
	/** Array used to copy and delete the messaged */
	RArray<TUint> iMessageUids;
	
	RArrayMessageFlagInfo iOutMessageFlagInfo;
	};

#endif	// __CIMAPCOMPOUNDCOPYWITHINSERVICE_H__
