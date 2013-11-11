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
// IMAP4 Offline Operations.
// 
//

#if !defined(__IMAPOFFL_H__)
#define __IMAPOFFL_H__


class CImImap4Session;
class CMsvServerEntry;
class CMsvEntrySelection;

// enums for use with MtmFunctionId
enum
	{
	EFnOffLineOpMoveDelete,
	EFnOffLineOpPopulate
	};

class CImap4OffLineControl : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
	enum TImap4OpType
		{
		EImap4OpCopyToLocal,
		EImap4OpMoveToLocal,
		EImap4OpCopyFromLocal,
		EImap4OpMoveFromLocal,
		EImap4OpCopyWithinService,
		EImap4OpMoveWithinService,
		EImap4OpPopulate,
		EImap4OpDelete,
		EImap4OpUndelete,
		EImap4OpMoveTypeDelete
		};

public:
	static CImap4OffLineControl* NewL(CMsvServerEntry* aEntry, CImImap4Session *aSession);
	static CImap4OffLineControl* NewLC(CMsvServerEntry* aEntry, CImImap4Session *aSession);
	~CImap4OffLineControl();
	void ConstructL();

	// add the given offline operation to the appropriate place
	void StoreOfflineCommandL(TImap4OpType aOperation,
							  const CMsvEntrySelection& aSelection,
							  TMsvId aDestination,
							  TRequestStatus& aStatus);

	void StoreOfflineCommandL(TImap4OpType aOperation,
							  const CMsvEntrySelection& aSelection,
							  TMsvId aDestination,
							  const TDesC8& aParams,
							  TRequestStatus& aStatus);
	
		// aSelection contains a list of folders and services. Remove any
	// offline operations contained within them
	void CancelOffLineOperationsL(const CMsvEntrySelection& aSelection);

	TMsvId IdIsLocalL(TMsvId aId);
	
	CImOffLineOperationArray* OffLineOpArrayL(TMsvId aId);
	void SetOffLineOpArrayL(TMsvId aId, CImOffLineOperationArray& aArray);
	
	TBool FindShadowIdsL(const CImOffLineOperation& aOp, CMsvEntrySelection& aSelection);
	TMsvId FindShadowIdL(const CImOffLineOperation& aOp);

private:
	CImap4OffLineControl(CMsvServerEntry* aEntry, CImImap4Session *aSession);

	TBool OffLineOpIsCopy(const CImOffLineOperation& aOp);

	TImDisconnectedOperationType OffLineOpToDisconnectedOp(const CImOffLineOperation& aOp);

	TInt PosVal(const CImOffLineOperation& aOp);

	void SaveOperationL(const CImOffLineOperation& aOperation);

	TBool FindOffLineOpByIdL(TMsvId aId, TMsvId aDestFolder,
							CImOffLineOperation& aOp, TBool aDelete);

	void UndeleteOperationL(TMsvId aId, TMsvId aDestId, TBool aConvertMoveToCopy,
							TImDisconnectedOperationType aDisconnected =ENoDisconnectedOperations);

	void MakeCopyMoveShadowL(const CImOffLineOperation& aOp);
	void MakeShadowL(const CImOffLineOperation& aOp);

	void UndoOfflineOpL(const CImOffLineOperation& aOp, TBool aClearMultiples);

	void SetEntryL(TMsvId aId);
	void ChangeEntryL(TMsvEntry& aEntry);
	void DeleteEntryL(TMsvId aId);

	TMsvId FolderOfL(TMsvId aId);
	TMsvId FindOffLineSaveFolderL(TMsvId aId, TMsvId aDestId);
	TMsvId MessageOfL(TMsvId aId);
	TMsvId ServiceOfL(TMsvId aId);

	void PrepareLocalOpL(TMsvId aId);
	TBool DoLocalOpL();

protected:
	void DoCancel();
	void DoRunL();
	void DoComplete(TInt& aError);

private:
	CMsvServerEntry* iEntry;			// passed into constructor
	CImImap4Session* iSession;			// passed into constructor
	
	CMsvEntrySelection* iCopyDirect;		// created in constructor and reused as necessary
	CMsvEntrySelection* iMoveDirect;		// created in constructor and reused as necessary
	CMsvEntrySelection* iMoveToLocalDirect;	// created in constructor and reused as necessary
	
	TMsvId iDestination;		// store for copy/move destination
	TMsvId iServiceId;

	//TMsvId iArrayFolder;
	//CImOffLineOperationArray* iArray;
	};

#endif
