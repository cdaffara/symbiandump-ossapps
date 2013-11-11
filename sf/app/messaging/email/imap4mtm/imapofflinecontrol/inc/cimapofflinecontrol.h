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
// cimapofflinecontrol.cpp.h
// 
//

#ifndef __CIMAPOFFLINECONTROL_H__
#define __CIMAPOFFLINECONTROL_H__

#include <mentact.h>
#include <offop.h>

class CMsvServerEntry;
class CMsvEntrySelection;
class TImap4CompoundProgress;

// enums for use with MtmFunctionId
enum
	{
	EFnOffLineOpMoveDelete,
	EFnOffLineOpPopulate
	};

class CImapOfflineControl : public CMsgActive
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
	~CImapOfflineControl();
	IMPORT_C static CImapOfflineControl* NewL(CMsvServerEntry& aEntry);

	// add the given offline operation to the appropriate place
	IMPORT_C void StoreOfflineCommandL(TImap4OpType aOperation,
							  const CMsvEntrySelection& aSelection,
							  TMsvId aDestination,
							  TRequestStatus& aStatus);

	IMPORT_C void StoreOfflineCommandL(TImap4OpType aOperation,
							  const CMsvEntrySelection& aSelection,
							  TMsvId aDestination,
							  const TDesC8& aParams,
							  TRequestStatus& aStatus);
	
	// aSelection contains a list of folders and services. Remove any
	// offline operations contained within them
	IMPORT_C void CancelOffLineOperationsL(const CMsvEntrySelection& aSelection);
	IMPORT_C TMsvId IdIsLocalL(TMsvId aId);
	IMPORT_C CImOffLineOperationArray* OffLineOpArrayL(TMsvId aId);
	IMPORT_C void SetOffLineOpArrayL(TMsvId aId, CImOffLineOperationArray& aArray);
	IMPORT_C TMsvId FindShadowIdL(const CImOffLineOperation& aOp);
	IMPORT_C TImap4CompoundProgress Progress();

private:
	void ConstructL();
	CImapOfflineControl(CMsvServerEntry& aEntry);
	TBool FindShadowIdsL(const CImOffLineOperation& aOp, CMsvEntrySelection& aSelection);
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
	CMsvServerEntry& iEntry;				// passed into constructor	
	CMsvEntrySelection* iCopyDirect;		// created in constructor and reused as necessary
	CMsvEntrySelection* iMoveDirect;		// created in constructor and reused as necessary
	CMsvEntrySelection* iMoveToLocalDirect;	// created in constructor and reused as necessary
	TMsvId iDestination;		// store for copy/move destination
	TMsvId iServiceId;
	TInt iProgressMsgsToDo;
	TInt iProgressMsgsDone;
	TImap4GenericProgress::TImap4GenericProgressOp iRequestedOperation;
	TImap4CompoundProgress iProgress;
	};

#endif  // __CIMAPOFFLINECONTROL_H__
