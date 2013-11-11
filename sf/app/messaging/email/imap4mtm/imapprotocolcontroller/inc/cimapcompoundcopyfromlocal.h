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

#ifndef __CIMAPCOMPOUNDCOPYFROMLOCAL_H__
#define __CIMAPCOMPOUNDCOPYFROMLOCAL_H__

#include "cimapcompoundbase.h"

// Forward declarations
class CImapFolder;
class CImSendMessage;

/**
The compound object for the copy or move from local operation. The 
copy from local operation copies or moves a selection of messages 
from a local folder to a remote folder.

@internalTechnology
@prototype
*/
class CImapCompoundCopyFromLocal : public CImapCompoundBase
	{
public:
	~CImapCompoundCopyFromLocal();
	
	static CImapCompoundCopyFromLocal* NewL( CImapSyncManager& aSyncManager,
						 			  		 CMsvServerEntry& aServerEntry,
						 			  		 CImapSettings& aImapSettings,
						 			  		 TBool aIsMove,
						 			  		 const CMsvEntrySelection& aSourceSel,
						 			  		 const TMsvId aDestination);
	
	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	
private:
	CImapCompoundCopyFromLocal(CImapSyncManager& aSyncManager,
		  					   CMsvServerEntry& aServerEntry,
		  					   CImapSettings& aImapSettings,
		  					   TBool aIsMove,
		  					   const TMsvId aDestination);
	void ConstructL(const CMsvEntrySelection& aSourceSel);
	
	virtual TInt ProcessNegativeServerResponse();

	TBool DoRunLoopL();
	void DoCancel();
private:
	/** ETrue if it is a move operation. The value is set during
	creation of the compound operation. */
	TBool iIsMove;
	
	/** Selection of messages to operate on. */
	CMsvEntrySelection* iSourceSel;
	
	// Progress Counts
	TInt iTotalSize;
	TInt iMessageSelection;
	TInt iSelectionStillToCopy;
	TInt iSelectionStillToDelete;
	
	/** Used for appending the message. */
	CImSendMessage* iSendMessage;
	
	/** The destination folder. CImapCompoundCopyFromLocal does not
	have ownership of this pointer. */
	CImapFolder* iDestinationFolder;	// No ownership

	/** The message ID of the message currently operating on. */
	TMsvId  iCurrentMsgId;
	
	/** The ID of the destination folder were all messages in
	iSourceSel will be copied/moved to. */
	TMsvId 	iDestinationFolderId;
	};

#endif	// __CIMAPCOMPOUNDCOPYFROMLOCAL_H__
