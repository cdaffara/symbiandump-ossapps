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

#ifndef __CIMAPCOMPOUNDCOPYTOLOCAL_H__
#define __CIMAPCOMPOUNDCOPYTOLOCAL_H__

#include <msvstd.h>
#include <imapset.h> // for TImImap4GetPartialMailInfo and TImap4GenericProgress
#include "cimapcompoundbase.h"
#include "tmessageflaginfo.h"

// Forward declarations
class CImapOpFetchBody;
class CImapFolder;
class CImapMailStore;

/**
The compound object for the copy or move to local operation. The 
copy to local operation copies or moves a selection of messages 
from a remote folder to a local folder.

@internalTechnology
@prototype
*/
class CImapCompoundCopyToLocal : public CImapCompoundBase
	{
public:
	~CImapCompoundCopyToLocal( );
	
	static CImapCompoundCopyToLocal* NewL(CImapSyncManager& aSyncManager, 
										  CMsvServerEntry& aServerEntry, 
										  CImapSettings& aImapSettings,
										  CImapMailStore& aImapMailStore,
										  TBool aIsMove, 
										  const CMsvEntrySelection& aSourceSel, 
										  const TMsvId aDestination);

	static CImapCompoundCopyToLocal* NewL(CImapSyncManager& aSyncManager, 
										  CMsvServerEntry& aServerEntry, 
										  CImapSettings& aImapSettings,
										  CImapMailStore& aImapMailStore, 
										  TBool aIsMove, 
										  const CMsvEntrySelection& aSourceSel, 
										  const TMsvId aDestination,
										  const TImImap4GetPartialMailInfo& aGetPartialMailInfo);

	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void CancelEnableResume();

	// allow messages to be removed from the selection of messages to fetch
	void RemoveFromSelection(CMsvEntrySelection& aDeleteSel);

private:
	CImapCompoundCopyToLocal(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, TBool aIsMove, const TMsvId aDestination);
	CImapCompoundCopyToLocal(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, TBool aIsMove, const TMsvId aDestination, const TImImap4GetPartialMailInfo& aGetPartialMailInfo);
	void ConstructL(const CMsvEntrySelection& aSourceSel);

	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();

	void UpdatePartialMailInfoToDefaults(TMsvId aDestination);
	
	TInt CopyMessage(const TMsvId aSource, const TMsvId aDestinationFolder, const TBool aRemoveOriginal);
	void CopyMessageL(const TMsvId aSource, const TMsvId aDestinationFolder, const TBool aRemoveOriginal);
	void CopyCompleteL();

	TBool SetSourceFolderL(const TMsvId aMessage);
	void  MarkMessageForDeleteL(const TMsvId& aTarget);
private:
	/** Used to store the message in the local message store */
	CImapMailStore& iMailStore;
	
	/** Selection of messages to operate on. */
	CMsvEntrySelection* iSourceSel;
	
	/** ETrue if it is a move operation. The value is set during
	creation of the compound operation. */
	TBool iIsMove;
	
	/** The ID of the destination folder were all messages in
	iSourceSel will be copied/moved to. */
	TMsvId iDestinationFolderId;
	
	/** Filter used when retrieving the messages */
	TImImap4GetPartialMailInfo	iGetPartialMailInfo;
	
	/** ETrue if a populate operation was requested. */
	TBool iPopulateCommand;
	
	// Progress Counts
	TInt iTotalSize;
	TInt iMessageSelection;
	TInt iSelectionStillToCopy;
	
	/** dedicated CMsvServerEntry for move operations, required 
	because move operations are asynchronous. */
	CMsvServerEntry* iMoveEntry;

	/** The source folder. CImapCompoundCopyFromLocal does not
	have ownership of this pointer. */
	CImapFolder* iSourceFolder;
	
	/** The ID of the source folder containing the message currently
	operating on. */
	TMsvId  iSourceFolderId;
	
	/** The message ID of the message currently operating on. */
	TMsvId  iCurrentMsgId;
	
	/** ETrue if a cancelled fetch should be allowed to be resumed by download rules based fetch */
	TBool iResume;
	
	/** Dedicated object for fetching message content. */
	CImapOpFetchBody* iBodyFetcher;
	
	RArrayMessageFlagInfo iOutMessageFlagInfo;
	};

#endif	// __CIMAPCOMPOUNDCOPYFROMLOCAL_H__
