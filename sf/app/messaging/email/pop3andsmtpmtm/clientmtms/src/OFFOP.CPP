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

#if !defined(__MSVAPI_H__)
#include <msvapi.h>
#endif

#include "OFFOP.H"
#include "MIUTQUE.H"

#include "MIUT_ERR.H"
#include "MIUTSET.H"	// KUidMsgTypeIMAP4


const TUid KUidImQueuedOperationList = {0x10001794};	// 2648441492 dec.


//
// CImOffLineOperation
EXPORT_C CImOffLineOperation::CImOffLineOperation() 
    : iOpType(EOffLineOpNone), iMessageId(KMsvNullIndexEntryId), iTargetMessageId(KMsvNullIndexEntryId),iMtmFunctionId(0),iMtmParameters(NULL)
    {
    };

EXPORT_C TBool CImOffLineOperation::Equals(const CImOffLineOperation& aOperation) const
    {
    if (iOpType				!= aOperation.iOpType			||
        iMessageId			!= aOperation.iMessageId		||
        iTargetMessageId	!= aOperation.iTargetMessageId	||
		iMtmFunctionId		!= aOperation.iMtmFunctionId)
        return EFalse;

    if (iMtmParameters)
        {
        if (!aOperation.iMtmParameters)
            return EFalse;
        if (iMtmParameters->Des().Compare(aOperation.iMtmParameters->Des()))
            return EFalse;
        }
    
    return ETrue;
    }

EXPORT_C void CImOffLineOperation::CopyL(const CImOffLineOperation& aOperation)
    {
    DeleteBuffer();
    iOpType				= aOperation.iOpType; 
    iMessageId			= aOperation.iMessageId;
    iTargetMessageId	= aOperation.iTargetMessageId;
	iMtmFunctionId		= aOperation.iMtmFunctionId;
    if (aOperation.iMtmParameters)
        {
        iMtmParameters = aOperation.iMtmParameters->Des().AllocL();
        }
    }

EXPORT_C CImOffLineOperation::~CImOffLineOperation()
    {
    DeleteBuffer();
    }

EXPORT_C void CImOffLineOperation::DeleteBuffer()
    {
    delete iMtmParameters;
    iMtmParameters = NULL;
    }

EXPORT_C void CImOffLineOperation::DetachMtmParameters()
    {
    iMtmParameters = NULL;
    }

EXPORT_C void CImOffLineOperation::SetMtmSpecificCommandL(TMsvId aMessageId, TMsvId aTargetMessageId, TInt aMtmFunctionId, const TDesC8& aParameters)
    {
    HBufC8* parameters = aParameters.AllocL();
    SetOperation(EOffLineOpMtmSpecific, aMessageId, aTargetMessageId, aMtmFunctionId, parameters);
    }


EXPORT_C void CImOffLineOperation::SetOperation(TOffLineOpType aOpType, TMsvId aMessageId, TMsvId aTargetMessageId, TInt aMtmFunctionId, HBufC8* aParameters)
    {
    DeleteBuffer();
    iOpType				= aOpType;
    iMessageId			= aMessageId;
    iTargetMessageId	= aTargetMessageId;
    iMtmFunctionId		= aMtmFunctionId;
    iMtmFunctionId		= aMtmFunctionId;
    iMtmParameters		= aParameters;
    }

EXPORT_C void CImOffLineOperation::SetOperation(TOffLineOpType aOpType, TMsvId aMessageId, TMsvId aTargetMessageId)
    {
    DeleteBuffer();
    iOpType				= aOpType;
    iMessageId			= aMessageId;
    iTargetMessageId	= aTargetMessageId;
    iMtmFunctionId		= 0;
    }

EXPORT_C void CImOffLineOperation::ExternalizeL( RMsvWriteStream& aWriteStream ) const
	{
    aWriteStream.WriteInt32L(iOpType);
    aWriteStream.WriteInt32L(iMessageId);
    aWriteStream.WriteInt32L(iTargetMessageId);
    aWriteStream.WriteInt32L(iMtmFunctionId);

    if (iMtmParameters)
        {
        TInt length = iMtmParameters->Length();
        aWriteStream.WriteInt32L(length);
        if (length > 0)
            aWriteStream << TPtrC8(*iMtmParameters);
        }
    else
        {
        aWriteStream.WriteInt32L(0);
        }
	return;
	}

EXPORT_C void CImOffLineOperation::InternalizeL( RMsvReadStream& aReadStream )
    {                        
    DeleteBuffer();
    iOpType				= TOffLineOpType(aReadStream.ReadInt32L());
    iMessageId			= TMsvId(aReadStream.ReadInt32L());
    iTargetMessageId	= TMsvId(aReadStream.ReadInt32L());
    iMtmFunctionId		= aReadStream.ReadInt32L();

    TInt length			= aReadStream.ReadInt32L();
    
    if (length > 0)
        {               
        iMtmParameters = HBufC8::NewL(aReadStream, length);
        }
	return;
	}

EXPORT_C int CImOffLineOperation::operator ==(const CImOffLineOperation& otherOperation) const
	{
	TBool result;

	if ((iMessageId == otherOperation.iMessageId)
		&& (iMtmFunctionId == otherOperation.iMtmFunctionId)
		&& (iOpType == otherOperation.iOpType)
		&& (iTargetMessageId == otherOperation.iTargetMessageId))
		{
		result = ETrue;
		}
	else
		{
		result = EFalse;
		}

	if (iMtmParameters != otherOperation.iMtmParameters)
		{
		if ((iMtmParameters == NULL) || (otherOperation.iMtmParameters == NULL))
			{
			result = EFalse;
			}
		else if (*iMtmParameters != *(otherOperation.iMtmParameters))
			{
			result = EFalse;
			}
		}

	return result;
	}


//
// CImOffLineOperationArrayStore

EXPORT_C void CImOffLineArrayStore::StoreL(CMsvStore& aMsvStore) const
	{
	RMsvWriteStream out;
	out.AssignLC( aMsvStore, KUidImQueuedOperationList ); 
	ExternalizeL(out);
	out.CommitL();
	CleanupStack::PopAndDestroy();
	}


EXPORT_C void CImOffLineArrayStore::RestoreL(const CMsvStore& aMessageStore )
	{
	if (aMessageStore.IsPresentL(KUidImQueuedOperationList))
		{
		RMsvReadStream in;
		in.OpenLC( aMessageStore, KUidImQueuedOperationList ); // pushes 'in' to the stack
		InternalizeL(in);
		CleanupStack::PopAndDestroy();
		}
	}

EXPORT_C void CImOffLineArrayStore::ExternalizeL(RMsvWriteStream& aWriteStream) const
    {
    TInt nr = iArray->CountOperations();
    aWriteStream.WriteUint16L(iVersion);
    aWriteStream.WriteInt32L(nr);
    
    TInt i;
    for ( i = 0 ; i < nr ; i++)
        {
        iArray->Operation( i ).ExternalizeL(aWriteStream);
        }
    }

EXPORT_C void CImOffLineArrayStore::InternalizeL(RMsvReadStream& aReadStream)
    {
    TInt nr;
    TUint16 version = aReadStream.ReadUint16L();
    if (version > iVersion)
        return;
    
    nr = aReadStream.ReadInt32L();
    CImOffLineOperation *operation=new(ELeave)CImOffLineOperation;
    CleanupStack::PushL(operation);
    
    TInt i;
    for (i = 0 ; i < nr ; i++)
        {
        operation->InternalizeL(aReadStream);
        iArray->AppendOperationL(*operation);
        }
    CleanupStack::PopAndDestroy();//operation
    }


//
// COffLineOperationArrayStore
EXPORT_C CImOffLineOperationArray* CImOffLineOperationArray::NewL()
    {
    CImOffLineOperationArray* self = new(ELeave)CImOffLineOperationArray();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();//self
    return self;
    }

EXPORT_C void CImOffLineOperationArray::ConstructL()
    {
    iArray = new(ELeave)CArrayFixFlat<CImOffLineOperation>(10);
    }

CImOffLineOperationArray::CImOffLineOperationArray()
    {
    }

EXPORT_C CImOffLineOperationArray::~CImOffLineOperationArray()
    {
	if (iArray)
		{
		TInt i;
		TInt nr = CountOperations();
		for (i=0;i<nr;i++)
			{
			(*iArray)[i].DeleteBuffer();
			}
		delete iArray;
		}
    }

TInt CImOffLineOperationArray::CountOperations() const
    {
    return iArray->Count();
    }

const CImOffLineOperation& CImOffLineOperationArray::Operation(TInt aIndex) const
    {
    return iArray->At(aIndex);
    }

void CImOffLineOperationArray::AppendOperationL(const CImOffLineOperation& aOperation)
    {
    CImOffLineOperation *operation=new(ELeave)CImOffLineOperation;
    CleanupStack::PushL(operation);
    operation->CopyL(aOperation);      // Construct by hand
    iArray->AppendL(*operation);
    operation->DetachMtmParameters();  // Delete by hand
    CleanupStack::PopAndDestroy();//operation 
    }

EXPORT_C void CImOffLineOperationArray::InsertOperationL(CImOffLineOperation& aOperation, TInt aPosition)
    {
    CImOffLineOperation *operation=new(ELeave)CImOffLineOperation;
    CleanupStack::PushL(operation);
    operation->CopyL(aOperation);      // Construct by hand
	iArray->InsertL(aPosition,*operation);
    operation->DetachMtmParameters();  // Delete by hand
    CleanupStack::PopAndDestroy();//operation 
    }

void CImOffLineOperationArray::Delete(TInt aIndex)
	{
	(*iArray)[aIndex].DeleteBuffer();
	iArray->Delete(aIndex);
	}

//
// TQueuedOperation


EXPORT_C TQueuedOperation* TQueuedOperation::NewL(TMsvId aFolderId,TInt aOperationIndex, const CImOffLineOperation& aStoredOperation)
    {
    TQueuedOperation* self = new(ELeave)TQueuedOperation(aFolderId,aOperationIndex);
    CleanupStack::PushL(self);
    self->iStoredOperation.CopyL(aStoredOperation);
    CleanupStack::Pop();//self
    return self;
    }


TQueuedOperation::TQueuedOperation(TMsvId aFolderId,TInt aOperationIndex)
	: iFolderId(aFolderId),iOperationIndex(aOperationIndex)
	{
    }

EXPORT_C TQueuedOperation::TQueuedOperation()
    : iFolderId(KMsvNullIndexEntryId), iOperationIndex(0), iStoredOperation()
    {
    }

EXPORT_C void TQueuedOperation::CopyL(const TQueuedOperation& aOperation)
    {
	iFolderId       = aOperation.iFolderId;
	iOperationIndex = aOperation.iOperationIndex;
	iStoredOperation.CopyL(aOperation.iStoredOperation);
    }

EXPORT_C TBool TQueuedOperation::operator!=(const TQueuedOperation& aOp)
	{
	return (iFolderId       != aOp.iFolderId || 
            iOperationIndex != aOp.iOperationIndex ||
            !iStoredOperation.Equals(aOp.iStoredOperation));
	}

//
// CImQueuedList

EXPORT_C CImQueuedList* CImQueuedList::NewL()
    {
    CImQueuedList* self = new(ELeave)CImQueuedList();
    CleanupStack::PushL(self);
    self->iArray = new(ELeave)CArrayFixFlat<TQueuedOperation>(10);
    CleanupStack::Pop();//self
    return self;
    }

CImQueuedList::CImQueuedList() 
    : iFolderId(KMsvNullIndexEntryId), iLine(0)
    {
    }

EXPORT_C CImQueuedList::~CImQueuedList() 
    {
	if (iArray)
		{
		TInt i;
		TInt nr = CountOperations();
		for (i=0;i<nr;i++)
			{
			(*iArray)[i].Operation().DeleteBuffer();
			}
		delete iArray;
		}
    }

EXPORT_C void CImQueuedList::SetFolder(TMsvId  aFolderId) 
    { 
    iFolderId = aFolderId;
    }

EXPORT_C void CImQueuedList::ResetLineCounter() 
    { 
    iLine = 0; 
    }

EXPORT_C TQueuedOperation& CImQueuedList::operator[](TInt anIndex)
    {
    return (*iArray)[anIndex];
    }

EXPORT_C TInt CImQueuedList::CountOperations() const
    {
    return iArray->Count();
    }

EXPORT_C const CImOffLineOperation& CImQueuedList::Operation(TInt aIndex) const
    {
    return iArray->At(aIndex).Operation();
    }

EXPORT_C void CImQueuedList::AppendOperationL(const CImOffLineOperation& aOperation)
    {
    TQueuedOperation *operation=TQueuedOperation::NewL(iFolderId, iLine, aOperation);
    CleanupStack::PushL(operation);
    iArray->AppendL(*operation);
    operation->Operation().DetachMtmParameters();
    CleanupStack::Pop();//operation
    delete operation;
    iLine++;
    }

EXPORT_C void CImQueuedList::Delete(TInt aIndex)
    {
    (*iArray)[aIndex].Operation().DeleteBuffer();
    iArray->Delete(aIndex);
    }

EXPORT_C void CImQueuedList::Reset()
    {
    TInt i;
    TInt nr = iArray->Count();
    for (i=0;i<nr;i++)
        {
        (*iArray)[i].Operation().DeleteBuffer();
        }
    iArray->Reset();
    }

EXPORT_C void CImQueuedList::AppendL(TQueuedOperation& aOperation)
    {
    iArray->AppendL(aOperation);
    aOperation.Operation().DetachMtmParameters();
    }

//
// CImOperationQueueList
//
// definition of class CImOperationQueueList. This class is responsible for gathering all
// the queued information of a MTM service, traversing folders to gather that
// information. It will then give access to the queued operations in the list, allowing
// queued elements to be deleted.
//

EXPORT_C CImOperationQueueList* CImOperationQueueList::NewL(CMsvEntry& aServiceEntry, MImUndoOffLineOperation *aImUndoOffLineOperation)
//
// Create new list of queued operations for service entry aServiceEntry is set to.
//
	{
	CImOperationQueueList* self = new (ELeave) CImOperationQueueList(aServiceEntry, aImUndoOffLineOperation);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CImOperationQueueList::~CImOperationQueueList() 
	{
	delete iServiceEntry;
	delete iQueuedList;
	delete iDeletedList;
	}

CImOperationQueueList::CImOperationQueueList(CMsvEntry& aServiceEntry, MImUndoOffLineOperation *aImUndoOffLineOperation) 
	: iServiceEntry(&aServiceEntry) , iUndoOffline(aImUndoOffLineOperation)
	{
	iServiceId = iServiceEntry->Entry().Id();
	};

void CImOperationQueueList::ProcessFoldersL()
//
// Usage: reset iQueuedList, set iServiceEntry to the service entry, and 
// call this function if a complete list of all queued operations stored
// under the folders within the service is needed.
//
	{
	__ASSERT_DEBUG( iServiceEntry->Entry().iType == KUidMsvFolderEntry ||
				    iServiceEntry->Entry().iType == KUidMsvServiceEntry, gPanic(EOffOpEntryShouldBeFolder));
	CMsvEntrySelection* selection = iServiceEntry->ChildrenWithTypeL(KUidMsvFolderEntry);
	CleanupStack::PushL(selection);

	TInt folder;
	TInt nr = selection->Count();
	for ( folder = 0 ; folder < nr; folder ++ )
		{
		iServiceEntry->SetEntryL( (*selection)[ folder ] );
		RestoreQueuedListL(*iQueuedList);
		ProcessFoldersL();
		}

	CleanupStack::PopAndDestroy();//selection
	}

void CImOperationQueueList::ConstructL()
//
// Create the list of queued operations.
//
	{
    iQueuedList  = CImQueuedList::NewL();
    iDeletedList = CImQueuedList::NewL();
    
    iDeletedList->Reset();
	iQueuedList->Reset();
	ProcessFoldersL();
	}

EXPORT_C void CImOperationQueueList::DeleteL(TInt aLine)
//
// Remove operation from list of queued operations locally.
//
	{
	TQueuedOperation deletedOperation;
    deletedOperation.CopyL((*iQueuedList)[aLine]);
	iQueuedList->Delete(aLine);
	iDeletedList->AppendL(deletedOperation);
	}

void CImOperationQueueList::RestoreQueuedListL(CImQueuedList &aList)
//
// Append contents of current entry (pointed to by iServiceEntry)
// to aList.
//	
	{
	__ASSERT_DEBUG( iServiceEntry->Entry().iMtm  == KUidMsgTypeIMAP4, gPanic(EOffOpBadMtmTypeUid));
	__ASSERT_DEBUG( iServiceEntry->Entry().iType == KUidMsvFolderEntry ||
				    iServiceEntry->Entry().iType == KUidMsvServiceEntry, gPanic(EOffOpEntryShouldBeFolder));

    TInt err=KErrNone;
    CMsvStore* store=NULL;
// Leaves with KErrNotFound if no store exists.
	TRAP(err,store=iServiceEntry->ReadStoreL());
 	if (err)
        return;
    CleanupStack::PushL(store);

    aList.SetFolder(iServiceEntry->Entry().Id());
    aList.ResetLineCounter();

    CImOffLineArrayStore internaliser(aList);
    internaliser.RestoreL(*store);
    CleanupStack::PopAndDestroy();//store
	}

void CImOperationQueueList::StoreQueuedListL(CImQueuedList &aList)
//
// Store contents of aList back in current entry (pointed to by iServiceEntry)
// to aList.
//	
	{
	__ASSERT_DEBUG( iServiceEntry->Entry().iMtm  == KUidMsgTypeIMAP4, gPanic(EOffOpBadMtmTypeUid));
	__ASSERT_DEBUG( iServiceEntry->Entry().iType == KUidMsvFolderEntry ||
				    iServiceEntry->Entry().iType == KUidMsvServiceEntry, gPanic(EOffOpEntryShouldBeFolder));
	CMsvStore* store = iServiceEntry->EditStoreL();
	CleanupStack::PushL(store);
    CImOffLineArrayStore externaliser(aList);
    externaliser.StoreL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();//store
	}

EXPORT_C void CImOperationQueueList::ExpungeDeletedOperationsL()
//
// Remove the queued operations that were marked as deleted from the folder stores.
//
	{
    // First sort so that each store only has to be visited once, and queued operations
    // can be deleted back-to-front. Starting at the back leaves the other delete commands
    // valid.
    TQueuedOperationKey key(*iDeletedList);
	TQueuedOperationSwap swap(*iDeletedList);
	User::QuickSort(iDeletedList->CountOperations(),key,swap);

    // Current folder being updated
    TMsvId lastFolderRead = KMsvNullIndexEntryId;
    // list holds the list of queued operations for the current folder
    CImQueuedList *list = CImQueuedList::NewL();
	CleanupStack::PushL(list);

	TInt line;

	TInt nr = iDeletedList->CountOperations();
	for ( line = 0 ; line < nr; line ++ )
		{
		// Get item to delete
		TQueuedOperation toDelete;
        toDelete.CopyL((*iDeletedList)[ 0 ]);
		iDeletedList->Delete(0);

		// See if item belongs to different folder.
		if (lastFolderRead != toDelete.FolderId())
			{
			// Store changes to previous list
			if (lastFolderRead != KMsvNullIndexEntryId)
				{
				StoreQueuedListL(*list);
				}
 
			// go to folder to delete item from.
			iServiceEntry->SetEntryL(toDelete.FolderId());
			__ASSERT_DEBUG( iServiceEntry->Entry().iMtm  == KUidMsgTypeIMAP4, gPanic(EOffOpBadMtmTypeUid));
			__ASSERT_DEBUG(iServiceEntry->Entry().iType == KUidMsvFolderEntry, gPanic(EOffOpEntryShouldBeFolder));

            // Initialise for the next folder
			lastFolderRead = toDelete.FolderId();
			// Get the list of queued operations
			list->Reset();
			RestoreQueuedListL(*list);
			}

        // Sanity check: the data contained in toDelete should be EXACTLY the same as 
        // the queued operation it refers to (as retrieved from the folder)
        if (toDelete != (*list)[ toDelete.OperationIndex() ])
			{
			gPanic(EOffOpListOutOfSync);
			continue;
			}

        // Undo changes made in server to reflect the disconnected operation.
		iUndoOffline->UndoOffLineChangesL(toDelete.Operation(), toDelete.FolderId());
		// Delete queued operation from list
        list->Delete(toDelete.OperationIndex());
		}

	// Store changes to last folder.
	if (lastFolderRead != KMsvNullIndexEntryId)
		{
		StoreQueuedListL(*list);
		}
	CleanupStack::PopAndDestroy(); //list
    }

//
// TQueuedOperationSwap
EXPORT_C TQueuedOperationSwap::TQueuedOperationSwap(CImQueuedList& aList) 
    : iList(aList) 
    {
    };

void TQueuedOperationSwap::Swap(TInt aLeft,TInt aRight) const
	{
    Mem::Swap(&iList[aLeft],&iList[aRight],sizeof(TQueuedOperation));
    }

//
// TQueuedOperationKey
EXPORT_C TQueuedOperationKey::TQueuedOperationKey(CImQueuedList& aList) 
    : iList(aList) 
    {
    }

TInt TQueuedOperationKey::Compare(TInt aLeft,TInt aRight) const
	{
	if (aLeft == aRight)
		return 0;
	if (iList[ aLeft ].FolderId() != iList[ aRight ].FolderId())
		return iList[ aLeft ].FolderId() - iList[ aRight ].FolderId();
	// Sort in descending order, so last lines will be deleted first
	TInt diff = iList[ aRight ].OperationIndex() - iList[ aLeft ].OperationIndex();
	__ASSERT_DEBUG(diff != 0, gPanic(EOffOpTwoSameLinesNotAllowed));
	return diff;
	}
TAny *TQueuedOperationKey::At(TInt anIndex) const
	{
	return &iList[ anIndex ];
	}


