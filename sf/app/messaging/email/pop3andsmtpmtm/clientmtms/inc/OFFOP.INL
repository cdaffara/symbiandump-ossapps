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

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "miut_errconsts.h"				
#endif

inline void CImOffLineOperation::SetCopyToLocal(TMsvId aMessageId, TMsvId aTargetFolderId)
    {
    SetOperation(EOffLineOpCopyToLocal, aMessageId, aTargetFolderId);
    }

inline void CImOffLineOperation::SetCopyFromLocal(TMsvId aMessageId, TMsvId aTargetFolderId)
    {
    SetOperation(EOffLineOpCopyFromLocal, aMessageId, aTargetFolderId);
    }

inline void CImOffLineOperation::SetCopyWithinService(TMsvId aMessageId, TMsvId aTargetFolderId)
    {
    SetOperation(EOffLineOpCopyWithinService, aMessageId, aTargetFolderId);
    }

inline void CImOffLineOperation::SetMoveToLocal(TMsvId aMessageId, TMsvId aTargetFolderId)
    {
    SetOperation(EOffLineOpMoveToLocal, aMessageId, aTargetFolderId);
    }

inline void CImOffLineOperation::SetMoveFromLocal(TMsvId aMessageId, TMsvId aTargetFolderId)
    {
    SetOperation(EOffLineOpMoveFromLocal, aMessageId, aTargetFolderId);
    }

inline void CImOffLineOperation::SetMoveWithinService(TMsvId aMessageId, TMsvId aTargetFolderId)
    {
    SetOperation(EOffLineOpMoveWithinService, aMessageId, aTargetFolderId);
    }

inline void CImOffLineOperation::SetDelete(TMsvId aMessageId)
    {
    SetOperation(EOffLineOpDelete, aMessageId, KMsvNullIndexEntryId);
    }

inline void CImOffLineOperation::SetChange(TMsvId aMessageId)
    {
    SetOperation(EOffLineOpChange, aMessageId, KMsvNullIndexEntryId);
    }

inline void CImOffLineOperation::SetCreate(TMsvId aMessageId)
    {
    SetOperation(EOffLineOpCreate, aMessageId, KMsvNullIndexEntryId);
    }

inline TInt    CImOffLineOperation::MtmFunctionId() const
    {
    return iMtmFunctionId;
    }

inline CImOffLineOperation::TOffLineOpType CImOffLineOperation::OpType() const 
    {
    return iOpType;
    }

inline TMsvId  CImOffLineOperation::MessageId() const 
    {
    return iMessageId;
    }

inline TMsvId  CImOffLineOperation::TargetMessageId() const 
    {
    return iTargetMessageId;
    }

inline TPtrC8  CImOffLineOperation::MtmParameters() const 
    {
    __ASSERT_DEBUG(iMtmParameters != NULL, User::Panic(KImcmPanic, EOffOpNoMtmParameters));
    return iMtmParameters->Des();
    }

inline CImOffLineArrayStore::CImOffLineArrayStore(MImOffLineOperationArray& aArray)
    {
    iArray = &aArray;
    iVersion = KCurrentOfflineOperationArrayVersion;
    }

inline void CImOffLineArrayStore::SetVersion(TUint16 aVersion)
    {
    iVersion = aVersion;
    }

inline TUint16 CImOffLineArrayStore::Version()
    {
    return iVersion;
    }


inline TQueuedOperation& CImOperationQueueList::operator[](TInt anIndex) 
//
// Access to array of queued operations.
//
	{
	return (*iQueuedList)[anIndex];
	}

inline const TInt CImOperationQueueList::Count() const
//
// Current number of queued operations in list
//
	{
	return iQueuedList->CountOperations();
	}

//
// CQueuedOperation accessors
inline TMsvId TQueuedOperation::FolderId() const 
	{ 
	return iFolderId; 
	}

inline TInt   TQueuedOperation::OperationIndex() const 
	{ 
	return iOperationIndex; 
	}

inline CImOffLineOperation& TQueuedOperation::Operation() 
	{
	return iStoredOperation;
	}

