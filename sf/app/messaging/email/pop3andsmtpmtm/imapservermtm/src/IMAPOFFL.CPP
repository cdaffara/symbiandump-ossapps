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
// IMAP4 Offline operations.
// 
//

#include "impspan.h"

#include <msventry.h>
#include <imapset.h>
#include <miutset.h>
#include <offop.h>
#include <msvreg.h>
#include <imapcmds.h>

#include "imapsess.h"
#include "imapoffl.h"

#ifdef _DEBUG
#define DBG(a) iSession->LogText a
#define PRINTING
#else
#define DBG(a)
#undef PRINTING
#endif

// ----------------------------------------------------------------------

#ifdef PRINTING

LOCAL_D TPtrC8 OffLineOpTypeString(const CImOffLineOperation& aOp)
	{
	switch (aOp.OpType())
		{
	case CImOffLineOperation::EOffLineOpNone:
		return _L8("None");

	case CImOffLineOperation::EOffLineOpCopyToLocal:
		return _L8("CopyToLocal");
	case CImOffLineOperation::EOffLineOpCopyFromLocal:
		return _L8("CopyFromLocal");
	case CImOffLineOperation::EOffLineOpCopyWithinService:
		return _L8("CopyWithinService");

	case CImOffLineOperation::EOffLineOpMoveToLocal:
		return _L8("MoveToLocal");
	case CImOffLineOperation::EOffLineOpMoveFromLocal:
		return _L8("MoveFromLocal");
	case CImOffLineOperation::EOffLineOpMoveWithinService:
		return _L8("MoveWithinService");

	case CImOffLineOperation::EOffLineOpDelete:
		return _L8("Delete");

	case CImOffLineOperation::EOffLineOpChange:
		return _L8("Change");
	case CImOffLineOperation::EOffLineOpCreate:
		return _L8("Create");

	case CImOffLineOperation::EOffLineOpMtmSpecific:
		switch (aOp.MtmFunctionId())
			{
		case EFnOffLineOpMoveDelete:
			return _L8("MoveDelete");
		case EFnOffLineOpPopulate:
			return _L8("Populate");
		default:
			return _L8("UnknownMtmSpecific");
			}
	default:
		break;
		}
	return _L8("Unknown");
	}

LOCAL_D TPtrC8 Imap4OpTypeString(CImap4OffLineControl::TImap4OpType aOpType)
	{
	switch (aOpType)
		{
	case CImap4OffLineControl::EImap4OpCopyToLocal:
		return _L8("CopyToLocal");
	case CImap4OffLineControl::EImap4OpCopyFromLocal:
		return _L8("CopyFromLocal");
	case CImap4OffLineControl::EImap4OpCopyWithinService:
		return _L8("CopyWithinService");

	case CImap4OffLineControl::EImap4OpMoveToLocal:
		return _L8("MoveToLocal");
	case CImap4OffLineControl::EImap4OpMoveFromLocal:
		return _L8("MoveFromLocal");
	case CImap4OffLineControl::EImap4OpMoveWithinService:
		return _L8("MoveWithinService");

	case CImap4OffLineControl::EImap4OpDelete:
		return _L8("Delete");

	case CImap4OffLineControl::EImap4OpMoveTypeDelete:
		return _L8("MoveDelete");
	case CImap4OffLineControl::EImap4OpPopulate:
		return _L8("Populate");

	default:
		break;
		}
	return _L8("Unknown");
	}
#endif

// ----------------------------------------------------------------------
// construction/destruction routines

CImap4OffLineControl* CImap4OffLineControl::NewL(CMsvServerEntry* aEntry, CImImap4Session *aSession)
	{
	CImap4OffLineControl* self = NewLC(aEntry,aSession);
	CleanupStack::Pop(); // self
	return self;
	}

CImap4OffLineControl* CImap4OffLineControl::NewLC(CMsvServerEntry* aEntry, CImImap4Session *aSession)
	{
	CImap4OffLineControl* self = new (ELeave) CImap4OffLineControl(aEntry,aSession);
	CleanupStack::PushL(self);
	CActiveScheduler::Add(self);

	self->ConstructL();

	return self;
	}

CImap4OffLineControl::CImap4OffLineControl(CMsvServerEntry* aEntry, CImImap4Session *aSession)
	: CMsgActive(EPriorityStandard), iEntry(aEntry), iSession(aSession)
	{
	}

void CImap4OffLineControl::ConstructL()
	{
	iCopyDirect = new (ELeave) CMsvEntrySelection;
	iMoveDirect = new (ELeave) CMsvEntrySelection;
	iMoveToLocalDirect = new (ELeave) CMsvEntrySelection;
	}

CImap4OffLineControl::~CImap4OffLineControl()
	{
	delete iCopyDirect;
	delete iMoveDirect;
	delete iMoveToLocalDirect;
	}

// ----------------------------------------------------------------------

// public routines

// Store an offline copy/move/delete command: we need to determine which
// folder the offline command should be stored in dependent on the
// source of the command.

// CopyToLocal can contain whole messages or parts (but not embedded
// messages). It can also be a copy to NULL, in which case it means
// just populate the mirror

// TODO: Pass in the GetMailOptions to the copy to mirror option

// Any item can contain whole messages, but not folders, and can
// contain shadow ids

void CImap4OffLineControl::StoreOfflineCommandL(TImap4OpType aOperation,
												const CMsvEntrySelection& aSelection,
												TMsvId aDestination,
												TRequestStatus& aStatus)
	{
	TBuf8<128> params = _L8("");
	StoreOfflineCommandL( aOperation, aSelection, aDestination, params, aStatus );
	}

void CImap4OffLineControl::StoreOfflineCommandL(TImap4OpType aOperation,
												const CMsvEntrySelection& aSelection,
												TMsvId aDestination,
												const TDesC8& aParams,
												TRequestStatus& aStatus)
	{
#ifdef PRINTING
	TPtrC8 p = Imap4OpTypeString(aOperation);
	DBG((_L8("StoreOfflineCommand: op %S %d entries to %x param bytes %d"),
		 &p, aSelection.Count(), aDestination, aParams.Length()));
#endif
		
	Queue(aStatus);

	iDestination = aDestination;

	// work our which service we are dealing with
	iServiceId = ServiceOfL( aOperation == EImap4OpCopyFromLocal ||
							 aOperation == EImap4OpMoveFromLocal ?
							 aDestination : aSelection[0] );

	// clear list of Direct operations to do after storing
	// commands
	iCopyDirect->Reset();
	iMoveDirect->Reset();
	iMoveToLocalDirect->Reset();
	
	for (TInt i = 0; i < aSelection.Count(); i++)
		{
		CImOffLineOperation op;
			
		// See if the message is in fact a shadow
		TMsvId origId = aSelection[i];
		SetEntryL(origId);

		TMsvId shadowId = KMsvNullIndexEntryId;
		TMsvId shadowParentId = KMsvNullIndexEntryId;
		TMsvEmailEntry entry = iEntry->Entry();
		if (entry.iRelatedId)
			{
			shadowId = origId;
			shadowParentId = entry.Parent();
			origId = entry.iRelatedId;

			// it is possible that the original has been deleted by
			// now (if it were local). If so then skip this operation
			TInt err = iEntry->SetEntry(origId);
			if (err != KErrNone)
				origId = KMsvNullIndexEntryId;
			else
				entry = iEntry->Entry();
			}

		if (origId != KMsvNullIndexEntryId)
			{
			// entry contains original (not shadow) message details
		
			// it is an undo type operation if we are copying or moving a
			// shadow back to its original folder and the original is
			// invisible or deleted
			TBool undeleteOp = shadowId != KMsvNullIndexEntryId &&
				entry.Parent() == iDestination &&
				(!entry.Visible() || entry.DisconnectedOperation() == EDisconnectedDeleteOperation);

			// Make operation & save it
			switch(aOperation)
				{
			case EImap4OpCopyToLocal:
				if (undeleteOp)
					{
					UndeleteOperationL(origId, shadowParentId, ETrue);
					}
				else if (IdIsLocalL(origId) || entry.Complete())
					{
					// either direct local copy or copy from mirror of completely populated message
					// either way, add new entry to todo array
					iCopyDirect->AppendL(origId);
					}
				else
					{
					op.SetCopyToLocal(origId,iDestination);
					SaveOperationL(op);
					}
				break;

			case EImap4OpCopyFromLocal:
			case EImap4OpCopyWithinService:
				if (undeleteOp)
					{
					UndeleteOperationL(origId, shadowParentId, ETrue);
					}
				else if (IdIsLocalL(origId))
					{
					op.SetCopyFromLocal(origId,iDestination);
					SaveOperationL(op);
					}
				else
					{
					op.SetCopyWithinService(origId,iDestination);
					SaveOperationL(op);
					}
				break;

			case EImap4OpMoveToLocal:
				if (undeleteOp)
					{
					UndeleteOperationL(origId, shadowParentId, EFalse);
					DeleteEntryL(shadowId);
					}
				else if (IdIsLocalL(origId))
					{
					CImOffLineOperation origOp;
					if (FindOffLineOpByIdL( origId, shadowParentId, origOp, ETrue /* delete op */) == 0)
						User::Leave(KErrNotSupported);

					if ( OffLineOpIsCopy(origOp) )
						// add new local to local copy op
						iCopyDirect->AppendL(origId);
					else
						// direct local move
						iMoveDirect->AppendL(origId);

					DeleteEntryL(shadowId);
					}
				else if (entry.Complete())
					{
					//	Not local, but completely populated
					iMoveToLocalDirect->AppendL(origId);
					}
				else
					{
					op.SetMoveToLocal(origId,iDestination);
					SaveOperationL(op);
					}
				break;

			case EImap4OpMoveFromLocal:
			case EImap4OpMoveWithinService:
				if (undeleteOp)
					{
					UndeleteOperationL(origId, shadowParentId, EFalse);

					// this one can fail depending on what kind of
					// undelete operation it was
					CImOffLineOperation origOp;
					FindOffLineOpByIdL( origId, shadowParentId, origOp, ETrue /* delete op */);

					DeleteEntryL(shadowId);
					}
				else if (shadowId)
					{
					CImOffLineOperation origOp;
					if (FindOffLineOpByIdL( origId, shadowParentId, origOp, ETrue /* delete op */) == 0)
						User::Leave(KErrNotSupported);
			
					// Clean disconnected flags
					SetEntryL(origId);
					TMsvEmailEntry entry = iEntry->Entry();
					if (entry.DisconnectedOperation() != EDisconnectedMultipleOperation)
						{
						entry.SetDisconnectedOperation(ENoDisconnectedOperations);
						ChangeEntryL(entry);
						}
					
					// if shadow was the result of a copy then change
					// original copy to point to new destination

					// if shadow was result of a move then change move to
					// point to new destination
					if ( OffLineOpIsCopy(origOp) )
						{
						if (IdIsLocalL(origId))
							op.SetCopyFromLocal(origId,iDestination);
						else
							op.SetCopyWithinService(origId,iDestination);
						}
					else
						{
						if (IdIsLocalL(origId))
							op.SetMoveFromLocal(origId,iDestination);
						else
							op.SetMoveWithinService(origId,iDestination);
						}

					SaveOperationL(op);
					DeleteEntryL(shadowId);
					}
				else
					{
					if (IdIsLocalL(origId))
						op.SetMoveFromLocal(origId,iDestination);
					else
						op.SetMoveWithinService(origId,iDestination);
					SaveOperationL(op);
					}
				break;

			case EImap4OpDelete:
				// we treat shadows and real items the same for deletion
				// currently
				op.SetDelete( shadowId ? shadowId : origId );
				SaveOperationL(op);
				break;
			
			case EImap4OpUndelete:
				if (shadowId)
					{
					UndeleteOperationL(shadowId, shadowParentId, EFalse);
					}
				else
					{
					// if the entry is not a shadow then we need to
					// replace the disconnected op flags with the original
					// flags before it was deleted.
					CImOffLineOperation origOp;

					// this searches the list before the delete is
					// removed.  However since deletes are stored at
					// the end of the list then if there are any other
					// operations it will return the other, and a
					// count of 2 or greater.
					TInt count = FindOffLineOpByIdL(origId, KMsvNullIndexEntryId, origOp, EFalse);

					TImDisconnectedOperationType disconnectedType = ENoDisconnectedOperations;
					if (count == 2)
						disconnectedType = OffLineOpToDisconnectedOp( origOp );
					else if (count > 2)
						disconnectedType = EDisconnectedMultipleOperation;

					UndeleteOperationL(origId, KMsvNullIndexEntryId, EFalse, disconnectedType);
					}
				break;

			case EImap4OpPopulate:
				/* easy one, just populate the original */
				op.SetMtmSpecificCommandL(origId, iDestination, EFnOffLineOpPopulate, aParams);
				SaveOperationL(op);
				break;

			case EImap4OpMoveTypeDelete:
				__ASSERT_DEBUG(0, gPanic(EBadUseOfImap4Op));
				break;
				}
			}
		}

	// if there are entries left over then they are ones we added to
	// be done immediately
	if (!DoLocalOpL())
		{
		// Request has been queued, complete immediately
		Complete(KErrNone);
		}
	}

// Cancel offline operations queued in the folders/service mentioned
// in the selection

void CImap4OffLineControl::CancelOffLineOperationsL(const CMsvEntrySelection& aSelection)
	{
	DBG((_L8("CancelOfflineOperations: %d entries"), aSelection.Count()));
		
	for (TInt i = 0; i < aSelection.Count(); i++)
		{
		TMsvId id = aSelection[i];

		SetEntryL(id);
		TMsvEmailEntry entry = iEntry->Entry();
		if (entry.iType == KUidMsvFolderEntry)
			{
			CImOffLineOperationArray* array = OffLineOpArrayL(id);
			CleanupStack::PushL(array);

			if (array->CountOperations())
				{
				// remove the queued ops
				while (array->CountOperations())
					{
					CImOffLineOperation thisOp;
					thisOp.CopyL(array->Operation(0));
					
					UndoOfflineOpL(thisOp, ETrue);
					
					array->Delete(0);
					}
				
				// write back empty array to store
				SetOffLineOpArrayL(id, *array);
				}

			CleanupStack::PopAndDestroy(); // array
			}
#if 0
		else
			{
			CImOffLineOperation op;
			while (FindOffLineOpByIdL(id, KMsvNullIndexEntryId, op, ETrue))
				{
				CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
				CleanupStack::PushL(selection);
				}
		
			CleanupStack::PopAndDestroy(); // selection
			}
#endif
		
		}
	}

// ----------------------------------------------------------------------

TImDisconnectedOperationType CImap4OffLineControl::OffLineOpToDisconnectedOp(const CImOffLineOperation& aOp)
	{
	TImDisconnectedOperationType type;
	switch (aOp.OpType())
		{
	case CImOffLineOperation::EOffLineOpMoveToLocal:
		type = EDisconnectedMoveToOperation;
		break;
	case CImOffLineOperation::EOffLineOpMoveFromLocal:
		type = EDisconnectedMoveFromOperation;
		break;
	case CImOffLineOperation::EOffLineOpMoveWithinService:
		type = EDisconnectedMoveWithinServiceOperation;
		break;

	case CImOffLineOperation::EOffLineOpCopyToLocal:
		type = EDisconnectedCopyToOperation;
		break;
	case CImOffLineOperation::EOffLineOpCopyFromLocal:
		type = EDisconnectedCopyFromOperation;
		break;
	case CImOffLineOperation::EOffLineOpCopyWithinService:
		type = EDisconnectedCopyWithinServiceOperation;
		break;
		
	case CImOffLineOperation::EOffLineOpDelete:
		type = EDisconnectedDeleteOperation;
		break;

	case CImOffLineOperation::EOffLineOpMtmSpecific:
		type = EDisconnectedSpecialOperation;
		break;
	default:
		type = EDisconnectedUnknownOperation;
		break;
		}
	return type;
	}

// This returns TRUE is it is a strict copy operation. Populate can be
// considered False by the callers of this function.

TBool CImap4OffLineControl::OffLineOpIsCopy(const CImOffLineOperation& aOp)
	{
	switch (aOp.OpType())
		{
	case CImOffLineOperation::EOffLineOpCopyToLocal:
	case CImOffLineOperation::EOffLineOpCopyFromLocal:
	case CImOffLineOperation::EOffLineOpCopyWithinService:
		return ETrue;
	case CImOffLineOperation::EOffLineOpMtmSpecific:
		if (aOp.MtmFunctionId() == EFnOffLineOpPopulate)
			{
			return ETrue;
			}
	    break; 
	
	default:
		break;
		}
	return EFalse;
	}

TInt CImap4OffLineControl::PosVal(const CImOffLineOperation& aOp)
	{
	switch (aOp.OpType())
		{	
	case CImOffLineOperation::EOffLineOpMtmSpecific: // populate
		switch (aOp.MtmFunctionId())
			{
		case EFnOffLineOpMoveDelete:
			return 5;
		case EFnOffLineOpPopulate:
			return 0;
			}
		break;

	case CImOffLineOperation::EOffLineOpCopyToLocal:
	case CImOffLineOperation::EOffLineOpCopyWithinService:
		return 1;
	case CImOffLineOperation::EOffLineOpCopyFromLocal:
		return 2;
		
	case CImOffLineOperation::EOffLineOpMoveToLocal:
	case CImOffLineOperation::EOffLineOpMoveWithinService:
		return 3;

	case CImOffLineOperation::EOffLineOpMoveFromLocal:
		return 4;	

	case CImOffLineOperation::EOffLineOpDelete:
		return 6;
	default:
		break;
		}
	return 6;
	}

// ----------------------------------------------------------------------

// Do setentry, leave if there is an error
void CImap4OffLineControl::SetEntryL(TMsvId aId)
	{
	User::LeaveIfError(iEntry->SetEntry(aId));
	}

// Change entry, leave if error
void CImap4OffLineControl::ChangeEntryL(TMsvEntry& aEntry)
	{
	User::LeaveIfError(iEntry->ChangeEntry(aEntry));
	}

// remove an id, leave if error, moves to the parent first
void CImap4OffLineControl::DeleteEntryL(TMsvId aId)
	{
	SetEntryL(aId);
	SetEntryL(iEntry->Entry().Parent());
	User::LeaveIfError(iEntry->DeleteEntry(aId));
	}

// Find the folder that encloses this message or message part. Note
// that this must be a real folder, not a folder component of a
// message, and that it may not be in our service.
TMsvId CImap4OffLineControl::FolderOfL(TMsvId aId)
	{
	SetEntryL( MessageOfL(aId) );
	return iEntry->Entry().Parent();
	}

// If the message is not in our service then return the destination
// folder. Otherwise return its own parent folder.
TMsvId CImap4OffLineControl::FindOffLineSaveFolderL(TMsvId aId, TMsvId aDestId)
	{
	TMsvId folder = FolderOfL(aId);
	if (ServiceOfL(folder) == iServiceId)
		return folder;
	return aDestId;
	}

// Find the top level message that holds this message part. Can be
// itself if it is a real message itself. This is located by finding
// the message that is highest up the tree.
TMsvId CImap4OffLineControl::MessageOfL(TMsvId aId)
	{
	TMsvId current=aId;
	TMsvId msg=aId;
	while(current!=KMsvRootIndexEntryIdValue)
		{
		// Visit this entry
		SetEntryL(current);

		TMsvEmailEntry entry = iEntry->Entry();
		
		// if service then searched far enough
		if (entry.iType==KUidMsvServiceEntry)
			break;

		// if message type then store it
		if (entry.iType==KUidMsvMessageEntry)
			msg = entry.Id();
		
		// Go upwards
		current=entry.Parent();
		}

	return msg;
	}

// return the id of the service containing this id
TMsvId CImap4OffLineControl::ServiceOfL(TMsvId aId)
	{
	TMsvId current=aId;
	while(current!=KMsvRootIndexEntryIdValue)
		{
		// Visit this entry
		SetEntryL(current);

		TMsvEmailEntry entry = iEntry->Entry();
		
		// if service then searched far enough
		if (entry.iType==KUidMsvServiceEntry)
			break;

		// Go upwards
		current=entry.Parent();
		}

	return current;
	}

// is this id in the local service?
TMsvId CImap4OffLineControl::IdIsLocalL(TMsvId aId)
	{
	return ServiceOfL(aId) == KMsvLocalServiceIndexEntryIdValue;
	}

// ----------------------------------------------------------------------

// simple functions to get and set the offline array on an id. More
// efficient open and modify versions are possible and used elsewhere

CImOffLineOperationArray* CImap4OffLineControl::OffLineOpArrayL(TMsvId aId)
	{
	SetEntryL(aId);

	CImOffLineOperationArray* array = CImOffLineOperationArray::NewL();

	// if no store then return an empty array (easier for higher
	// layers than a NULL pointer).
	if (iEntry->HasStoreL())
		{
		CleanupStack::PushL(array);
		
		CMsvStore* store = iEntry->ReadStoreL();
		CleanupStack::PushL(store);
	
		CImOffLineArrayStore arraystore(*array);
		arraystore.RestoreL(*store);

		CleanupStack::PopAndDestroy(); // store
		CleanupStack::Pop();		   // array
		}
	
	DBG((_L8("OffLineOpArrayL: folder 0x%x count %d"), aId, array->CountOperations()));

	return array;
	}

void CImap4OffLineControl::SetOffLineOpArrayL(TMsvId aId, CImOffLineOperationArray& aArray)
	{
	DBG((_L8("SetOffLineOpArrayL: folder 0x%x count %d"), aId, aArray.CountOperations()));

	SetEntryL( aId );

	CMsvStore* store=iEntry->EditStoreL();
	CleanupStack::PushL(store);

	CImOffLineArrayStore arraystore(aArray);
	arraystore.StoreL(*store);

	store->CommitL();

	CleanupStack::PopAndDestroy(); // store
	}

// ----------------------------------------------------------------------

// Save offline operation
void CImap4OffLineControl::SaveOperationL(const CImOffLineOperation& aOperation)
	{
	DBG((_L8("SaveOperation:")));

	// We need an array, to store the current offline operations of this folder
    CImOffLineOperationArray *array=CImOffLineOperationArray::NewL();
	CleanupStack::PushL(array);
	CImOffLineArrayStore arraystore(*array);

	// find where to store the op
	TMsvId storehere = FindOffLineSaveFolderL(aOperation.MessageId(), aOperation.TargetMessageId());
	SetEntryL(storehere);

	// open the store
	CMsvStore *store=iEntry->EditStoreL();
	CleanupStack::PushL(store);

	arraystore.RestoreL(*store);

	// we add this operation after others of the same type
	TInt insertBefore = PosVal(aOperation) + 1;
	TBool done = EFalse;
	
	for(TInt a=0; a<array->CountOperations(); a++)
		{
		if (insertBefore <= PosVal(array->Operation(a)))
			{
			array->InsertOperationL(MUTABLE_CAST(CImOffLineOperation&, aOperation), a);
			done = ETrue;
			break;
			}
		}
	
	if (!done)
		array->AppendOperationL(aOperation);

	// write back
	arraystore.StoreL(*store);
	store->CommitL();

	// Dispose of store & array
	CleanupStack::PopAndDestroy(2);

	// make the shadow
	MakeShadowL(aOperation);
	}

// returns ETrue if a matching Op was found

TInt CImap4OffLineControl::FindOffLineOpByIdL(TMsvId aId, TMsvId aDestFolder,
										  CImOffLineOperation& aOp, TBool aDelete)
	{
    CImOffLineOperationArray *array=CImOffLineOperationArray::NewL();
	CleanupStack::PushL(array);
	CImOffLineArrayStore arraystore(*array);

	SetEntryL(FindOffLineSaveFolderL(aId, aDestFolder));
	CMsvStore *store=aDelete ? iEntry->EditStoreL() : iEntry->ReadStoreL();
	CleanupStack::PushL(store);

	arraystore.RestoreL(*store);

	// look in the array for an operation on this Id and optionally to
	// the matching folder
	TInt found = 0;
	TInt foundAt = -1;
	for(TInt a=0; a<array->CountOperations(); a++)
		{
		if (array->Operation(a).MessageId() == aId &&
			(aDestFolder == KMsvNullIndexEntryId ||
			 aDestFolder == array->Operation(a).TargetMessageId()) )
			{
			// only write out the first operation found
			if (found == 0)
				{
				foundAt = a;
				aOp.CopyL( array->Operation(a) );
				}
			found++;
			}
		}

	// optionally now delete the operation from the array
	if (aDelete && foundAt != -1)
		{
		array->Delete(foundAt);
		
		arraystore.StoreL(*store);
		store->CommitL();
		}
	
	CleanupStack::PopAndDestroy(2);	// store, array

	return found;
	}

// this means remove the cause of the delete, ie remove delete or
// change move to copy, unless ConvertToCopy is False in which case
// delete any move operation rather than convert it.

// there can only be one relevant operation in the array as the UI or
// MTM should have prevented further operations

// Deleting any shadow entry should be done outside this function

void CImap4OffLineControl::UndeleteOperationL(TMsvId aId, TMsvId aDestId, TBool aConvertMoveToCopy,
										 TImDisconnectedOperationType aDisconnected)
	{
	DBG((_L8("UndeleteOperation: Id %x CvtMove %d type %d"),
		 aId, aConvertMoveToCopy, aDisconnected));

	// We need an array, to store the current offline operations of this folder
    CImOffLineOperationArray *array=CImOffLineOperationArray::NewL();
	CleanupStack::PushL(array);
	CImOffLineArrayStore arraystore(*array);

	SetEntryL(FindOffLineSaveFolderL(aId, aDestId));
	DBG((_L8("UndeleteOperation: opending savefolder store %x"), iEntry->Entry().Id() ));
	CMsvStore *store=iEntry->EditStoreL();
	CleanupStack::PushL(store);

	arraystore.RestoreL(*store);

	// look in the array for a delete or move operation on this Id
	CImOffLineOperation thisOp;
	for(TInt a=0; a<array->CountOperations(); a++)
		{
		thisOp.CopyL(array->Operation(a));

		if (thisOp.MessageId() == aId)
			{
			TBool finish = ETrue;
			TBool isDelete = EFalse;
			
			switch (thisOp.OpType())
				{
				// if move then convert it to an equivalent copy
			case CImOffLineOperation::EOffLineOpMoveToLocal:
				thisOp.SetCopyToLocal(aId, thisOp.TargetMessageId());
				break;

			case CImOffLineOperation::EOffLineOpMoveFromLocal:
				thisOp.SetCopyFromLocal(aId, thisOp.TargetMessageId());
				break;

			case CImOffLineOperation::EOffLineOpMoveWithinService:
				thisOp.SetCopyWithinService(aId, thisOp.TargetMessageId());
				break;

				// if delete then get rid of the pending operation
			case CImOffLineOperation::EOffLineOpDelete:
				isDelete = ETrue;
				break;

			default:
				finish = EFalse;
				break;
				}

			if (finish)
				{
				// remove the existing operation
				array->Delete(a);

				// potentially add a new one
				if (!isDelete)
					{
					// it's become a copy so insert at head of list
					if (aConvertMoveToCopy)
						array->InsertOperationL(thisOp, 0);
					}

				// exit 'for' loop and so we don't need to fix up the
				// iterator
				break;
				}
			}
		}

	DBG((_L8("UndeleteOperation: write store")));

	// write back offline op array
	arraystore.StoreL(*store);
	store->CommitL();
	
	CleanupStack::PopAndDestroy(2);	// store, array

	DBG((_L8("UndeleteOperation: ensure visible")));

	// then make the item visible and update its pending operation
	// type
	SetEntryL(aId);
	TMsvEmailEntry entry = iEntry->Entry();

	entry.SetDisconnectedOperation(aDisconnected);
	entry.SetVisible(ETrue);

	ChangeEntryL(entry);

	DBG((_L8("UndeleteOperation: done")));
	}

// Make shadow for offline operation - this shadow indicates what
// *will* happen at the next sync

// Note if we want to copy the entire structure of the message then
// there is a ready made function Imap4Session->CopyMessageL() to do
// this
void CImap4OffLineControl::MakeCopyMoveShadowL(const CImOffLineOperation& aOp)
	{
	// get copy of the original message
	SetEntryL(aOp.MessageId());
	TMsvEmailEntry origMsg = iEntry->Entry();

	// check this is a real message, we don't make shadows of parts
	if (origMsg.iType != KUidMsvMessageEntry)
		return;

	// if this is not a copy to mirror only operation then make shadow
	if ( aOp.OpType() != CImOffLineOperation::EOffLineOpMtmSpecific )
		{
		// copy out the non embedded data
		HBufC* details = origMsg.iDetails.AllocL();
		CleanupStack::PushL(details);
		HBufC* description = origMsg.iDescription.AllocL();
		CleanupStack::PushL(description);

		// set up the new message, clearing any disconnected op flags
		// it may have
		TMsvEmailEntry newMsg = origMsg;
		newMsg.iRelatedId = aOp.MessageId();
		newMsg.SetComplete(EFalse);
		newMsg.SetDisconnectedOperation(ENoDisconnectedOperations);
		// ensure that this one is visible (may be copied from one
		// that wasn't)
		newMsg.SetVisible(ETrue);
		
		// create shadow entry
		SetEntryL(aOp.TargetMessageId());

		newMsg.iDetails.Set(details->Des());
		newMsg.iDescription.Set(description->Des());
		User::LeaveIfError(iEntry->CreateEntry(newMsg));
		
		CleanupStack::PopAndDestroy(2);	// description, details
		}
	
	// set flags on the original message
	SetEntryL(origMsg.Id());

	if (origMsg.DisconnectedOperation() == ENoDisconnectedOperations)
		origMsg.SetDisconnectedOperation( OffLineOpToDisconnectedOp(aOp) );
	else
		origMsg.SetDisconnectedOperation( EDisconnectedMultipleOperation );

	// make original invisible if this was a move operation
	if (!OffLineOpIsCopy(aOp))
		origMsg.SetVisible(EFalse);

	// write back changes
	ChangeEntryL(origMsg);
	}

void CImap4OffLineControl::MakeShadowL(const CImOffLineOperation& aOp)
	{
	DBG((_L8("MakeShadow: of %x in folder %x"), aOp.MessageId(), aOp.TargetMessageId()));

	switch (aOp.OpType())
		{
	case CImOffLineOperation::EOffLineOpMtmSpecific: // populate
	case CImOffLineOperation::EOffLineOpMoveToLocal:
	case CImOffLineOperation::EOffLineOpMoveFromLocal:
	case CImOffLineOperation::EOffLineOpMoveWithinService:
	case CImOffLineOperation::EOffLineOpCopyToLocal:
	case CImOffLineOperation::EOffLineOpCopyFromLocal:
	case CImOffLineOperation::EOffLineOpCopyWithinService:
		MakeCopyMoveShadowL(aOp);
		break;
		
	case CImOffLineOperation::EOffLineOpDelete:
		// Set the pending operation to Delete, we don't care if there
		// were other operations already pending
		{
		SetEntryL(aOp.MessageId());
		TMsvEmailEntry msg = iEntry->Entry();
		msg.SetDisconnectedOperation(EDisconnectedDeleteOperation);
		ChangeEntryL(msg);
		}
		break;
	
	case CImOffLineOperation::EOffLineOpNone:
	case CImOffLineOperation::EOffLineOpChange:
	case CImOffLineOperation::EOffLineOpCreate:
		__ASSERT_DEBUG(0, gPanic(EBadUseOfOffLineOp));
		break;
		}

	}

// look in the folder for an item whose iRelatedId matches
TBool CImap4OffLineControl::FindShadowIdsL(const CImOffLineOperation& aOp, CMsvEntrySelection& aSelection)
	{
	CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);

	SetEntryL(aOp.TargetMessageId());
	User::LeaveIfError(iEntry->GetChildren(*selection));

	TBool foundOne = EFalse;
	for(TInt child=0;child<selection->Count();child++)
		{
		TMsvId childId = (*selection)[child];
		SetEntryL(childId);
		TMsvEntry message = iEntry->Entry();
		if (message.iRelatedId == aOp.MessageId())
			{
			aSelection.InsertL(0, childId);
			foundOne = ETrue;
			}
		}

	CleanupStack::PopAndDestroy();

	return foundOne;
	}

TMsvId CImap4OffLineControl::FindShadowIdL(const CImOffLineOperation& aOp)
	{
	CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);

	TMsvId id = KMsvNullIndexEntryId;

	// the target folder might have been deleted - in which case just
	// return that the id was not found
	if (iEntry->SetEntry(aOp.TargetMessageId()) == KErrNone)
		{
		User::LeaveIfError(iEntry->GetChildren(*selection));
		for(TInt child=0;child<selection->Count();child++)
			{
			TMsvId childId = (*selection)[child];
			SetEntryL(childId);
			TMsvEntry message = iEntry->Entry();
			if (message.iRelatedId == aOp.MessageId())
				{
				id = childId;
				break;
				}
			}
		}

	CleanupStack::PopAndDestroy();

	return id;
	}

void CImap4OffLineControl::UndoOfflineOpL(const CImOffLineOperation& aOp, TBool aClearMultiples)
	{
#ifdef PRINTING
	TPtrC8 p = OffLineOpTypeString(aOp);
	DBG((_L8("UndoOfflineOp: %S Id %x TargetFolder %x"),
		 &p, aOp.MessageId(), aOp.TargetMessageId()));
#endif
	
	// get the first id related to the source of this message, unless
	// it has no destination (ie it is a delete op)
	if (aOp.TargetMessageId())
		{
		TMsvId id = FindShadowIdL(aOp);
		if (id != KMsvNullIndexEntryId)
			{
			SetEntryL(aOp.TargetMessageId());
			iEntry->DeleteEntry(id);
			}
		}

	// remove the disconnected op flags from the source entry and make
	// it visible (does't harm if it was visible anyway), if it has
	// multiple ops then we leave it as we don't know what to do.

	// entry might not exist if it was a shadow
	if (iEntry->SetEntry(aOp.MessageId()) == KErrNone)
		{
		TMsvEmailEntry entry = iEntry->Entry();
		if (!entry.Visible() || aClearMultiples ||
			entry.DisconnectedOperation() != EDisconnectedMultipleOperation)
			{
			entry.SetDisconnectedOperation(ENoDisconnectedOperations);
			entry.SetVisible(ETrue);
			ChangeEntryL(entry);
			}
		}
	}

void CImap4OffLineControl::PrepareLocalOpL(TMsvId aId)
	{
	SetEntryL(aId);

	// clear the disconnected op flag
	TMsvEmailEntry entry = iEntry->Entry();
	entry.SetDisconnectedOperation(ENoDisconnectedOperations);
	ChangeEntryL(entry);
		
	SetEntryL(iEntry->Entry().Parent());
	}

TBool CImap4OffLineControl::DoLocalOpL()
	{
	if (iCopyDirect->Count())
		{
		TMsvId id = (*iCopyDirect)[0];

		DBG((_L8("CImap4OffLineControl::DoLocalOp Copy id %x to do %d"),
			 id, iCopyDirect->Count()));

		PrepareLocalOpL(id);
		
		SetActive();
		iEntry->CopyEntryL(id, iDestination, iStatus);
		return ETrue;
		}

	if (iMoveDirect->Count())
		{
		TMsvId id = (*iMoveDirect)[0];

		DBG((_L8("CImap4OffLineControl::DoLocalOp Move id %x to do %d"),
			 id, iMoveDirect->Count()));

		PrepareLocalOpL(id);

		SetActive();
		iEntry->MoveEntryL(id, iDestination, iStatus);
		return ETrue;
		}

	if (iMoveToLocalDirect->Count())
		{
		TMsvId id = (*iMoveToLocalDirect)[0];
	
		DBG((_L8("CImap4OffLineControl::DoDirectMoveToLocalOp Move id %x to do %d"),
			 id, iMoveToLocalDirect->Count()));
	
		PrepareLocalOpL(id);
	
		SetActive();
		iEntry->CopyEntryL(id, iDestination, iStatus);	//	I do mean Copy
		return ETrue;
		}
	
	return EFalse;
	}

// ----------------------------------------------------------------------

void CImap4OffLineControl::DoCancel()
	{
	CMsgActive::DoCancel();
	}

void CImap4OffLineControl::DoComplete(TInt& /*aStatus*/)
	{

	}

void CImap4OffLineControl::DoRunL()
	{
	DBG((_L8("CImap4OffLineControl::DoRunL")));

	// successfully copied/moved the item
	
	// Remove completed item from selection
	if (iCopyDirect->Count())
		iCopyDirect->Delete(0,1);
	else if (iMoveDirect->Count())
		iMoveDirect->Delete(0,1);
	else
		{
		//	We managed to do the copy portion of a move to local
		//	Now we need to queue up a delete of the original which
		//	is still in the remote mailbox.
		CImOffLineOperation op;
		op.SetDelete((*iMoveToLocalDirect)[0]);
		iMoveToLocalDirect->Delete(0,1);
		SaveOperationL(op);
		}

	// Operation done. Do next one in selection
	DoLocalOpL();
	}

// ----------------------------------------------------------------------
