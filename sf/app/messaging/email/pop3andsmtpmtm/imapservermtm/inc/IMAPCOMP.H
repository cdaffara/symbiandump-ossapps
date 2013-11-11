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
// CImImap4Compound header
// 
//

#if !defined(__IMAPCOMP_H__)
#define __IMAPCOMP_H__

#include <mentact.h>
#include <msventry.h>
#include <imapset.h>

// Forward declarations
class CImImap4Session;

class CImImap4Compound : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
	~CImImap4Compound();
	static CImImap4Compound* NewL(CImImap4Session* aSession);
	void ConstructL(CImImap4Session* aSession);
	void SetSession(CImImap4Session* aSession);
	void SetEntry(CMsvServerEntry* aEntry);

	// The compound operations we provide
	void CopyToLocalL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination);
	void MoveToLocalL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination);
	void CopyWithinServiceL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination);
	void CopyWithinServiceL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination);
	void MoveWithinServiceL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination);
	void MoveWithinServiceL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination);
	void CopyFromLocalL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination);
	void MoveFromLocalL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination);
	void DeleteL(TRequestStatus& aStatus, const TMsvId aSource);
	void DeleteL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel);
	void DeleteFolderL(TRequestStatus& aStatus, const TMsvId aSource);
	void NewOnlySyncL(TRequestStatus& aStatus, const TMsvId aFolder);
	void FullSyncL(TRequestStatus& aStatus, const TMsvId aFolder);
	void SelectL(TRequestStatus& aStatus, const TMsvId aFolder);
	void SynchroniseL(TRequestStatus& aStatus);
	void CreateL(TRequestStatus& aStatus, const TMsvId aParent, const TDesC& aLeafName, const TBool aFolder);
	void RenameL(TRequestStatus& aStatus, const TMsvId aTarget, const TDesC& aNewName);
	void PopulateL(TRequestStatus& aStatus, const TMsvId aSource, TImImap4GetPartialMailInfo aGetPartialMailInfo);
	void SetMessageCount(const TInt aCount);
	// Compound operations for when called from the Synchronise task
	void SyncCopyToLocalL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination);

	void UpdatePartialMailInfoToDefaults(TMsvId aDestination);
	// Progress reporting
	TImap4GenericProgress Progress();

private:
	CImImap4Compound();
	void DoRunL();
	TBool DoRunLoopL();
	void DoCancel();

	TMsvId FindFolderL(const TMsvId aMessage);
	void GenericCopyL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination, TInt* aSequence);
	void GenericCopyL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination, TInt* aSequence);
	void SetEntryL(const TMsvId aId);

	// Sequences of operations
	TInt SeqPopulateAttachment[3];
	TInt SeqCopyToLocal[7];
	TInt SeqMoveToLocal[8];
	TInt SeqCopyWithinService[8];
	TInt SeqMoveWithinService[9];
	TInt SeqCopyFromLocal[4];
	TInt SeqMoveFromLocal[8];
	TInt SeqDelete[6];
	TInt SeqDeleteFolder[8];
	TInt SeqNewSync[6];
	TInt SeqFullSync[6];
	TInt SeqSelect[3];
	TInt SeqSynchronise[5];
	TInt SeqCreate[5];
	TInt SeqRename[5];
	TInt SeqSyncCopyToLocal[3];

private:
	// IMAP4 session to use
	CImImap4Session*		iSession;

	// Internal state
	TInt					iStep;
	TInt*					iSequence;
	TMsvId					iSource;
	TInt					iSelectionStillToCopy;
 	TInt					iSelectionStillToDelete;
 	TInt					iMessageSelection;
 	CMsvEntrySelection*		iSourceSel;	
	TMsvId					iSourceFolder;
	TMsvId					iDestinationFolder;
	TFileName				iLeafName;
	TBool					iFolder;
	TBool					iIdleBeforeCommand;
	TBool					iPopulateCommand;
	TBool 					iIdleBeforeFirstPopulate;
	TInt 					iMsgCount;
		
	// Access to message database
	CMsvServerEntry*		iEntry;

	// saved partial populate info
	TImImap4GetPartialMailInfo	iGetPartialMailInfo;
	};

#endif
