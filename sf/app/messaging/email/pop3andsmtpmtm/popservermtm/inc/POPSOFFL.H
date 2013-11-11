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
// popservermtm module header
// @internalComponent
// @released
// 
//

#if !defined(_POPSOFFL_H__)
#define _POPSOFFL_H__

#include <e32std.h>
#include <e32base.h>    // For CMsgActive, TFileName
#include <msvstd.h>
#include <offop.h>
#include <mentact.h>
#include <miuthdr.h>
#include <pop3set.h>

class CMsvServerEntry;

//
// CImPop3OfflineOperationFinder
//
class CImPop3OfflineOperationFinder : public CMsgActive
	{
public:
	struct TOperationDetails
		{
		CImOffLineOperation::TOffLineOpType iOpType;
		TInt iOperationNumber;
		TInt iOperationsOfType;
		};
	
public:
	static CImPop3OfflineOperationFinder* NewL(CMsvServerEntry& aEntry);
	static CImPop3OfflineOperationFinder* NewLC(CMsvServerEntry& aEntry);

	void FindFirstL(TMsvId aServiceId, TBool aQuitting, TRequestStatus &aStatus);
	void FindNext();

	TBool OperationFound() const;
	const CImOffLineOperation& OfflineOperation() const;
	const TOperationDetails& OperationDetails() const;

	~CImPop3OfflineOperationFinder();
	
private:
	void DoRunL();
	void DoCancel();
	void DoComplete();
	void ConstructL();
	CImPop3OfflineOperationFinder(CMsvServerEntry& aEntry);
	void CheckNextChildMessageL();
	inline void AppendOfflineOpsL();
	inline TBool AppropriateOperation(const CImOffLineOperation& aOperation) const;

private:
	enum TImOfflineOpFinderState
		{
		EPopsOffOpLookingForMessages,
		EPopsOffOpGettingOperation
		};

	TOperationDetails iOperationDetails;
	CMsvEntrySelection* iChildMessages;
	TInt iMessageIndex;

	CArrayVarFlat<CImOffLineOperationArray*>* iOfflineOperations;
	TInt iOperationArrayIndex;
	TInt iOperationIndex;
	TImOfflineOpFinderState iState;
	TMsvId iServiceId;
	CMsvServerEntry& iEntry;
	TBool iQuitting;
	};


class CImPop3TransferMessage;

//
// CImPop3SetOfflineOps 
//
class CImPop3SetOfflineOps : public CMsgActive
	{
public:
	static CImPop3SetOfflineOps* NewL(CMsvServerEntry& aEntry);
	static CImPop3SetOfflineOps* NewLC(CMsvServerEntry& aEntry);

	void AddOfflineOperationL(const CMsvEntrySelection *aMessageSelection,
								CImOffLineOperation::TOffLineOpType aOperationType,
								TMsvId aTargetFolderId,
								TRequestStatus& aStatus);

	void CancelOfflineOperationsL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	void ResumeCancelOfflineOperationsL(TRequestStatus& aStatus);

	~CImPop3SetOfflineOps();

	void Progress(TPop3Progress& rPop3Progress) const;
	
private:
	void DoRunL();
	void DoCancel();
	void DoComplete(TInt& status);
	CImPop3SetOfflineOps(CMsvServerEntry& aEntry);
	void AddOfflineOperationL();
	void CancelOfflineOperationL();
	void CopyLocalMessageL();
	void ConstructL();
	void DeleteOfflineOperationL(TInt messageCounter);

private:
	enum TSetOfflineOpState
		{
		EAddingOfflineOperations,
		ECancellingOfflineOperations,
		ECopyingLocalEntries
		};

	TSetOfflineOpState iState;
	TInt iMessageCounter;
	CMsvEntrySelection* iSourceMessages;
	CMsvEntrySelection* iMessagesToCopyLocally;
	TMsvId iDestinationEntryId;
	CImOffLineOperation::TOffLineOpType iOperationType;
	CImOffLineOperation::TOffLineOpType iModifiedOperationType;
	CMsvServerEntry& iEntry;
	CImPop3TransferMessage* iTransfer;
	RArray<TBool> iOfflineOperationArrayFlag;
	};

// Utility functions used by the operation finder and the operation setter.

//
// CImPop3OfflineUtilities
//
class CImPop3OfflineUtilities
	{
public:
	static void DeleteL(const CImOffLineOperation& aOperation, CMsvServerEntry& aEntry);
	static void SetOfflineFlags(const CImOffLineOperationArray& aOperationArray, TMsvEmailEntry& aEntry);
	};

#endif

