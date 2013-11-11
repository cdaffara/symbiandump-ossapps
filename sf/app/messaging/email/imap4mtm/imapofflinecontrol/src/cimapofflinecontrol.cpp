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

#include <msventry.h>
#include <miutset.h>
#include <imapset.h>
#include "cimapofflinecontrol.h"
#include "cimaplogger.h"

#ifdef __IMAP_LOGGING

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

LOCAL_D TPtrC8 Imap4OpTypeString(CImapOfflineControl::TImap4OpType aOpType)
	{
	switch (aOpType)
		{
	case CImapOfflineControl::EImap4OpCopyToLocal:
		return _L8("CopyToLocal");
	case CImapOfflineControl::EImap4OpCopyFromLocal:
		return _L8("CopyFromLocal");
	case CImapOfflineControl::EImap4OpCopyWithinService:
		return _L8("CopyWithinService");
	case CImapOfflineControl::EImap4OpMoveToLocal:
		return _L8("MoveToLocal");
	case CImapOfflineControl::EImap4OpMoveFromLocal:
		return _L8("MoveFromLocal");
	case CImapOfflineControl::EImap4OpMoveWithinService:
		return _L8("MoveWithinService");
	case CImapOfflineControl::EImap4OpDelete:
		return _L8("Delete");
	case CImapOfflineControl::EImap4OpMoveTypeDelete:
		return _L8("MoveDelete");
	case CImapOfflineControl::EImap4OpPopulate:
		return _L8("Populate");
	default:
		break;
		}
	return _L8("Unknown");
	}
#endif


EXPORT_C CImapOfflineControl* CImapOfflineControl::NewL(CMsvServerEntry& aEntry)
	{
	CImapOfflineControl* self = new (ELeave) CImapOfflineControl(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CImapOfflineControl::CImapOfflineControl(CMsvServerEntry& aEntry)
	: CMsgActive(EPriorityStandard), iEntry(aEntry) 
	{
	CActiveScheduler::Add(this);
	}

void CImapOfflineControl::ConstructL()
	{
	iCopyDirect = new (ELeave) CMsvEntrySelection;
	iMoveDirect = new (ELeave) CMsvEntrySelection;
	iMoveToLocalDirect = new (ELeave) CMsvEntrySelection;
	}

CImapOfflineControl::~CImapOfflineControl()
	{
	delete iCopyDirect;
	delete iMoveDirect;
	delete iMoveToLocalDirect;
	}

/**
 public routines

 Store an offline copy/move/delete command: we need to determine which
 folder the offline command should be stored in dependent on the
 source of the command.

 CopyToLocal can contain whole messages or parts (but not embedded
 messages). It can also be a copy to NULL, in which case it means
 just populate the mirror

 Any item can contain whole messages, but not folders, and can
 contain shadow ids
*/

EXPORT_C void CImapOfflineControl::StoreOfflineCommandL(TImap4OpType aOperation,
												const CMsvEntrySelection& aSelection,
												TMsvId aDestination,
												TRequestStatus& aStatus)
	{
	TBuf8<128> params = _L8("");
	StoreOfflineCommandL( aOperation, aSelection, aDestination, params, aStatus );
	}

EXPORT_C void CImapOfflineControl::StoreOfflineCommandL(TImap4OpType aOperation,
												const CMsvEntrySelection& aSelection,
												TMsvId aDestination,
												const TDesC8& aParams,
												TRequestStatus& aStatus)
	{
#ifdef __IMAP_LOGGING
	TPtrC8 p = Imap4OpTypeString(aOperation);
	__LOG_FORMAT((KDefaultLog,"StoreOfflineCommand: op %S %d entries to %x param bytes %d", &p, aSelection.Count(), aDestination, aParams.Length()));
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
	
	//update the progress info
	iProgressMsgsToDo=aSelection.Count();
	
	for (TInt i = 0; i < aSelection.Count(); i++)
		{
		CImOffLineOperation* op = new(ELeave)CImOffLineOperation();
		CleanupStack::PushL(op);
			
		// See if the message is in fact a shadow
		TMsvId origId = aSelection[i];
		SetEntryL(origId);

		TMsvId shadowId = KMsvNullIndexEntryId;
		TMsvId shadowParentId = KMsvNullIndexEntryId;
		TMsvEmailEntry entry = iEntry.Entry();
		if (entry.iRelatedId)
			{
			shadowId = origId;
			shadowParentId = entry.Parent();
			origId = entry.iRelatedId;

			// it is possible that the original has been deleted by
			// now (if it were local). If so then skip this operation
			TInt err = iEntry.SetEntry(origId);
			if (err != KErrNone)
				origId = KMsvNullIndexEntryId;
			else
				entry = iEntry.Entry();
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
				iRequestedOperation=TImap4GenericProgress::EOffLineCopyToLocal;
				if (undeleteOp)
					{
					UndeleteOperationL(origId, shadowParentId, ETrue);
					}
				else if (IdIsLocalL(origId) || entry.Complete())
					{
					// either direct local copy or copy from mirror of completely populated message
					// either way, add new entry to array
					iCopyDirect->AppendL(origId);
					}
				else
					{
					op->SetCopyToLocal(origId,iDestination);
					SaveOperationL(*op);
					}
				break;

			case EImap4OpCopyFromLocal:
				iRequestedOperation=TImap4GenericProgress::EOffLineCopyFromLocal;
			case EImap4OpCopyWithinService:
				iRequestedOperation=TImap4GenericProgress::EOffLineCopyWithinService;
				if (undeleteOp)
					{
					UndeleteOperationL(origId, shadowParentId, ETrue);
					}
				else if (IdIsLocalL(origId))
					{
					op->SetCopyFromLocal(origId,iDestination);
					SaveOperationL(*op);
					}
				else
					{
					op->SetCopyWithinService(origId,iDestination);
					SaveOperationL(*op);
					}
				break;

			case EImap4OpMoveToLocal:
				iRequestedOperation=TImap4GenericProgress::EOffLineMoveToLocal;
				if (undeleteOp)
					{
					UndeleteOperationL(origId, shadowParentId, EFalse);
					DeleteEntryL(shadowId);
					}
				else if (IdIsLocalL(origId))
					{
					CImOffLineOperation* origOp = new(ELeave)CImOffLineOperation();
					CleanupStack::PushL(origOp);
					
					if (FindOffLineOpByIdL( origId, shadowParentId, *origOp, ETrue /* delete op */) == 0)
						{
						User::Leave(KErrNotSupported);
						}

					if ( OffLineOpIsCopy(*origOp) )
						{
						// add new local to local copy op
						iCopyDirect->AppendL(origId);
						}
					else
						{
						// direct local move
						iMoveDirect->AppendL(origId);
						}

					DeleteEntryL(shadowId);
					CleanupStack::PopAndDestroy(origOp);
					}
				else if (entry.Complete())
					{
					//	Not local, but completely populated
					iMoveToLocalDirect->AppendL(origId);
					}
				else
					{
					op->SetMoveToLocal(origId,iDestination);
					SaveOperationL(*op);
					}
				break;

			case EImap4OpMoveFromLocal:
				iRequestedOperation=TImap4GenericProgress::EOffLineMoveFromLocal;
			case EImap4OpMoveWithinService:
				iRequestedOperation=TImap4GenericProgress::EOffLineMoveWithinService;
				if (undeleteOp)
					{
					UndeleteOperationL(origId, shadowParentId, EFalse);

					// this one can fail depending on what kind of
					// undelete operation it was
					CImOffLineOperation* origOp = new(ELeave)CImOffLineOperation();
					CleanupStack::PushL(origOp);
					
					FindOffLineOpByIdL( origId, shadowParentId, *origOp, ETrue /* delete op */);

					DeleteEntryL(shadowId);					
					CleanupStack::PopAndDestroy(origOp);
					}
				else if (shadowId)
					{
					CImOffLineOperation* origOp = new(ELeave)CImOffLineOperation();
					CleanupStack::PushL(origOp);
					
					if (FindOffLineOpByIdL( origId, shadowParentId, *origOp, ETrue /* delete op */) == 0)
						{
						User::Leave(KErrNotSupported);
						}
			
					// Clean disconnected flags
					SetEntryL(origId);
					TMsvEmailEntry entry = iEntry.Entry();
					if (entry.DisconnectedOperation() != EDisconnectedMultipleOperation)
						{
						entry.SetDisconnectedOperation(ENoDisconnectedOperations);
						ChangeEntryL(entry);
						}
					
					// if shadow was the result of a copy then change
					// original copy to point to new destination

					// if shadow was result of a move then change move to
					// point to new destination
					if ( OffLineOpIsCopy(*origOp) )
						{
						if (IdIsLocalL(origId))
							op->SetCopyFromLocal(origId,iDestination);
						else
							op->SetCopyWithinService(origId,iDestination);
						}
					else
						{
						if (IdIsLocalL(origId))
							op->SetMoveFromLocal(origId,iDestination);
						else
							op->SetMoveWithinService(origId,iDestination);
						}

					SaveOperationL(*op);
					
					DeleteEntryL(shadowId);
					CleanupStack::PopAndDestroy(origOp);
					}
				else
					{
					if (IdIsLocalL(origId))
						op->SetMoveFromLocal(origId,iDestination);
					else
						op->SetMoveWithinService(origId,iDestination);
					SaveOperationL(*op);
					}
				break;

			case EImap4OpDelete:
				iRequestedOperation=TImap4GenericProgress::EOffLineDelete;
				// we treat shadows and real items the same for deletion
				// currently
				op->SetDelete( shadowId ? shadowId : origId );
				SaveOperationL(*op);
				break;
			
			case EImap4OpUndelete:
				iRequestedOperation=TImap4GenericProgress::EOffLineUndelete;
				if (shadowId)
					{
					UndeleteOperationL(shadowId, shadowParentId, EFalse);
					}
				else
					{
					// if the entry is not a shadow then we need to
					// replace the disconnected op flags with the original
					// flags before it was deleted.
					CImOffLineOperation* origOp = new(ELeave)CImOffLineOperation();
					CleanupStack::PushL(origOp);

					// this searches the list before the delete is
					// removed.  However since deletes are stored at
					// the end of the list then if there are any other
					// operations it will return the other, and a
					// count of 2 or greater.
					TInt count = FindOffLineOpByIdL(origId, KMsvNullIndexEntryId, *origOp, EFalse);

					TImDisconnectedOperationType disconnectedType = ENoDisconnectedOperations;
					if (count == 2)
						disconnectedType = OffLineOpToDisconnectedOp( *origOp );
					else if (count > 2)
						disconnectedType = EDisconnectedMultipleOperation;

					UndeleteOperationL(origId, KMsvNullIndexEntryId, EFalse, disconnectedType);
					
					CleanupStack::PopAndDestroy(origOp);
					}
					
				++iProgressMsgsDone; // this is normally done in SaveOperationL() but the undelete op does not use that method.
				break;

			case EImap4OpPopulate:
				iRequestedOperation=TImap4GenericProgress::EOffLinePopulate;
				/* easy one, just populate the original */
				op->SetMtmSpecificCommandL(origId, iDestination, EFnOffLineOpPopulate, aParams);
				SaveOperationL(*op);
				break;

			case EImap4OpMoveTypeDelete:
				__ASSERT_DEBUG(0, User::Invariant());
				break;
				}
			}
		
		CleanupStack::PopAndDestroy(op);
		
		} // end of for loop

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

EXPORT_C void CImapOfflineControl::CancelOffLineOperationsL(const CMsvEntrySelection& aSelection)
	{
	__LOG_FORMAT((KDefaultLog, "CancelOfflineOperations: %d entries", aSelection.Count()));
		
	for (TInt i = 0; i < aSelection.Count(); i++)
		{
		TMsvId id = aSelection[i];

		SetEntryL(id);
		TMsvEmailEntry entry = iEntry.Entry();
		if (entry.iType == KUidMsvFolderEntry)
			{
			CImOffLineOperationArray* array = OffLineOpArrayL(id);
			CleanupStack::PushL(array);

			if (array->CountOperations())
				{
				// remove the queued ops
				while (array->CountOperations())
					{
					CImOffLineOperation* thisOp = new(ELeave)CImOffLineOperation();
					CleanupStack::PushL(thisOp);
					
					thisOp->CopyL(array->Operation(0));
					
					UndoOfflineOpL(*thisOp, ETrue);
					
					array->Delete(0);
					CleanupStack::PopAndDestroy(thisOp);
					}
				
				// write back empty array to store
				SetOffLineOpArrayL(id, *array);
				}

			CleanupStack::PopAndDestroy(); // array
			}
		}
	}


TImDisconnectedOperationType CImapOfflineControl::OffLineOpToDisconnectedOp(const CImOffLineOperation& aOp)
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

TBool CImapOfflineControl::OffLineOpIsCopy(const CImOffLineOperation& aOp)
	{
	switch (aOp.OpType())
		{
	case CImOffLineOperation::EOffLineOpCopyToLocal:
	case CImOffLineOperation::EOffLineOpCopyFromLocal:
	case CImOffLineOperation::EOffLineOpCopyWithinService:
		return ETrue;
	default:
		break;
		}
	return EFalse;
	}

TInt CImapOfflineControl::PosVal(const CImOffLineOperation& aOp)
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


// Do setentry, leave if there is an error
void CImapOfflineControl::SetEntryL(TMsvId aId)
	{
	User::LeaveIfError(iEntry.SetEntry(aId));
	}

// Change entry, leave if error
void CImapOfflineControl::ChangeEntryL(TMsvEntry& aEntry)
	{
	User::LeaveIfError(iEntry.ChangeEntry(aEntry));
	}

// remove an id, leave if error, moves to the parent first
void CImapOfflineControl::DeleteEntryL(TMsvId aId)
	{
	SetEntryL(aId);
	SetEntryL(iEntry.Entry().Parent());
	User::LeaveIfError(iEntry.DeleteEntry(aId));
	}

// Find the folder that encloses this message or message part. Note
// that this must be a real folder, not a folder component of a
// message, and that it may not be in our service.
TMsvId CImapOfflineControl::FolderOfL(TMsvId aId)
	{
	SetEntryL( MessageOfL(aId) );
	return iEntry.Entry().Parent();
	}

// If the message is not in our service then return the destination
// folder. Otherwise return its own parent folder.
TMsvId CImapOfflineControl::FindOffLineSaveFolderL(TMsvId aId, TMsvId aDestId)
	{
	TMsvId folder = FolderOfL(aId);
	if (ServiceOfL(folder) == iServiceId)
		return folder;
	return aDestId;
	}

// Find the top level message that holds this message part. Can be
// itself if it is a real message itself. This is located by finding
// the message that is highest up the tree.
TMsvId CImapOfflineControl::MessageOfL(TMsvId aId)
	{
	TMsvId current=aId;
	TMsvId msg=aId;
	while(current!=KMsvRootIndexEntryIdValue)
		{
		// Visit this entry
		SetEntryL(current);

		TMsvEmailEntry entry = iEntry.Entry();
		
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
TMsvId CImapOfflineControl::ServiceOfL(TMsvId aId)
	{
	TMsvId current=aId;
	while(current!=KMsvRootIndexEntryIdValue)
		{
		// Visit this entry
		SetEntryL(current);

		TMsvEmailEntry entry = iEntry.Entry();
		
		// if service then searched far enough
		if (entry.iType==KUidMsvServiceEntry)
			break;

		// Go upwards
		current=entry.Parent();
		}

	return current;
	}

// is this id in the local service?
EXPORT_C TMsvId CImapOfflineControl::IdIsLocalL(TMsvId aId)
	{
	return ServiceOfL(aId) == KMsvLocalServiceIndexEntryIdValue;
	}


// simple functions to get and set the offline array on an id. More
// efficient open and modify versions are possible and used elsewhere

EXPORT_C CImOffLineOperationArray* CImapOfflineControl::OffLineOpArrayL(TMsvId aId)
	{
	SetEntryL(aId);

	CImOffLineOperationArray* array = CImOffLineOperationArray::NewL();
	CleanupStack::PushL(array);

	// if no store then return an empty array (easier for higher
	// layers than a NULL pointer).
	if (iEntry.HasStoreL())
		{
		CMsvStore* store = iEntry.ReadStoreL();
		CleanupStack::PushL(store);
	
		CImOffLineArrayStore arraystore(*array);
		arraystore.RestoreL(*store);

		CleanupStack::PopAndDestroy(); // store
		}
	
	// DBG((_L8("OffLineOpArrayL: folder 0x%x count %d"), aId, array->CountOperations()));
	
	CleanupStack::Pop();		   // array
	return array;
	}

EXPORT_C void CImapOfflineControl::SetOffLineOpArrayL(TMsvId aId, CImOffLineOperationArray& aArray)
	{
	// DBG((_L8("SetOffLineOpArrayL: folder 0x%x count %d"), aId, aArray.CountOperations()));

	SetEntryL( aId );

	CMsvStore* store=iEntry.EditStoreL();
	CleanupStack::PushL(store);

	CImOffLineArrayStore arraystore(aArray);
	arraystore.StoreL(*store);

	store->CommitL();

	CleanupStack::PopAndDestroy(); // store
	}


// Save offline operation
void CImapOfflineControl::SaveOperationL(const CImOffLineOperation& aOperation)
	{
	// DBG((_L8("SaveOperation:")));

	// We need an array, to store the current offline operations of this folder
    CImOffLineOperationArray *array=CImOffLineOperationArray::NewL();
	CleanupStack::PushL(array);
	CImOffLineArrayStore arraystore(*array);

	// find where to store the op
	TMsvId storehere = FindOffLineSaveFolderL(aOperation.MessageId(), aOperation.TargetMessageId());
	SetEntryL(storehere);

	// open the store
	CMsvStore *store=iEntry.EditStoreL();
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
	
	//update the progrees info
	++iProgressMsgsDone;
	}

// returns ETrue if a matching Op was found

TInt CImapOfflineControl::FindOffLineOpByIdL(TMsvId aId, TMsvId aDestFolder,
										  CImOffLineOperation& aOp, TBool aDelete)
	{
    CImOffLineOperationArray *array=CImOffLineOperationArray::NewL();
	CleanupStack::PushL(array);
	CImOffLineArrayStore arraystore(*array);

	SetEntryL(FindOffLineSaveFolderL(aId, aDestFolder));
	CMsvStore *store=aDelete ? iEntry.EditStoreL() : iEntry.ReadStoreL();
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

void CImapOfflineControl::UndeleteOperationL(TMsvId aId, TMsvId aDestId, TBool aConvertMoveToCopy,
										 TImDisconnectedOperationType aDisconnected)
	{
	// DBG((_L8("UndeleteOperation: Id %x CvtMove %d type %d"),
	//	 aId, aConvertMoveToCopy, aDisconnected));

	// We need an array, to store the current offline operations of this folder
    CImOffLineOperationArray *array=CImOffLineOperationArray::NewL();
	CleanupStack::PushL(array);
	CImOffLineArrayStore arraystore(*array);

	SetEntryL(FindOffLineSaveFolderL(aId, aDestId));
	// DBG((_L8("UndeleteOperation: opending savefolder store %x"), iEntry.Entry().Id() ));
	CMsvStore *store=iEntry.EditStoreL();
	CleanupStack::PushL(store);

	arraystore.RestoreL(*store);

	// look in the array for a delete or move operation on this Id
	CImOffLineOperation* thisOp = new(ELeave)CImOffLineOperation();
	CleanupStack::PushL(thisOp);
	
	for(TInt a=0; a<array->CountOperations(); a++)
		{
		thisOp->CopyL(array->Operation(a));

		if (thisOp->MessageId() == aId)
			{
			TBool finish = ETrue;
			TBool isDelete = EFalse;
			
			switch (thisOp->OpType())
				{
				// if move then convert it to an equivalent copy
			case CImOffLineOperation::EOffLineOpMoveToLocal:
				thisOp->SetCopyToLocal(aId, thisOp->TargetMessageId());
				break;

			case CImOffLineOperation::EOffLineOpMoveFromLocal:
				thisOp->SetCopyFromLocal(aId, thisOp->TargetMessageId());
				break;

			case CImOffLineOperation::EOffLineOpMoveWithinService:
				thisOp->SetCopyWithinService(aId, thisOp->TargetMessageId());
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
						array->InsertOperationL(*thisOp, 0);
					}

				break;
				}
			}
			
		} // end of for loop
	
	// DBG((_L8("UndeleteOperation: write store")));

	// write back offline op array
	arraystore.StoreL(*store);
	store->CommitL();

	CleanupStack::PopAndDestroy(thisOp);
	thisOp = NULL;
	CleanupStack::PopAndDestroy(store);
	store = NULL;
	CleanupStack::PopAndDestroy(array);
	array = NULL;

	// DBG((_L8("UndeleteOperation: ensure visible")));

	// then make the item visible and update its pending operation
	// type
	SetEntryL(aId);
	TMsvEmailEntry entry = iEntry.Entry();

	entry.SetDisconnectedOperation(aDisconnected);
	entry.SetVisible(ETrue);

	ChangeEntryL(entry);

	// DBG((_L8("UndeleteOperation: done")));
	}

// Make shadow for offline operation - this shadow indicates what
// *will* happen at the next sync

// Note if we want to copy the entire structure of the message then
// there is a ready made function Imap4Session->CopyMessageL() to do
// this
void CImapOfflineControl::MakeCopyMoveShadowL(const CImOffLineOperation& aOp)
	{
	// get copy of the original message
	SetEntryL(aOp.MessageId());
	TMsvEmailEntry origMsg = iEntry.Entry();

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
		User::LeaveIfError(iEntry.CreateEntry(newMsg));
		
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

void CImapOfflineControl::MakeShadowL(const CImOffLineOperation& aOp)
	{
	// DBG((_L8("MakeShadow: of %x in folder %x"), aOp.MessageId(), aOp.TargetMessageId()));

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
		TMsvEmailEntry msg = iEntry.Entry();
		msg.SetDisconnectedOperation(EDisconnectedDeleteOperation);
		ChangeEntryL(msg);
		}
		break;
	
	case CImOffLineOperation::EOffLineOpNone:
	case CImOffLineOperation::EOffLineOpChange:
	case CImOffLineOperation::EOffLineOpCreate:
		__ASSERT_DEBUG(0, User::Invariant());
		break;
		}

	}

// look in the folder for an item whose iRelatedId matches
TBool CImapOfflineControl::FindShadowIdsL(const CImOffLineOperation& aOp, CMsvEntrySelection& aSelection)
	{
	CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);

	SetEntryL(aOp.TargetMessageId());
	User::LeaveIfError(iEntry.GetChildren(*selection));

	TBool foundOne = EFalse;
	for(TInt child=0;child<selection->Count();child++)
		{
		TMsvId childId = (*selection)[child];
		SetEntryL(childId);
		TMsvEntry message = iEntry.Entry();
		if (message.iRelatedId == aOp.MessageId())
			{
			aSelection.InsertL(0, childId);
			foundOne = ETrue;
			}
		}

	CleanupStack::PopAndDestroy();

	return foundOne;
	}

EXPORT_C TMsvId CImapOfflineControl::FindShadowIdL(const CImOffLineOperation& aOp)
	{
	CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);

	TMsvId id = KMsvNullIndexEntryId;

	// the target folder might have been deleted - in which case just
	// return that the id was not found
	if (iEntry.SetEntry(aOp.TargetMessageId()) == KErrNone)
		{
		User::LeaveIfError(iEntry.GetChildren(*selection));
		for(TInt child=0;child<selection->Count();child++)
			{
			TMsvId childId = (*selection)[child];
			SetEntryL(childId);
			TMsvEntry message = iEntry.Entry();
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

void CImapOfflineControl::UndoOfflineOpL(const CImOffLineOperation& aOp, TBool aClearMultiples)
	{
#ifdef __IMAP_LOGGING
	TPtrC8 p = ::OffLineOpTypeString(aOp);
	__LOG_FORMAT((KDefaultLog, "UndoOfflineOp: %S Id %x TargetFolder %x",&p, aOp.MessageId(), aOp.TargetMessageId()));
#endif
	
	// get the first id related to the source of this message, unless
	// it has no destination (ie it is a delete op)
	if (aOp.TargetMessageId())
		{
		TMsvId id = FindShadowIdL(aOp);
		if (id != KMsvNullIndexEntryId)
			{
			SetEntryL(aOp.TargetMessageId());
			iEntry.DeleteEntry(id);
			}
		}

	// remove the disconnected op flags from the source entry and make
	// it visible (does't harm if it was visible anyway), if it has
	// multiple ops then we leave it as we don't know what to do.

	// entry might not exist if it was a shadow
	if (iEntry.SetEntry(aOp.MessageId()) == KErrNone)
		{
		TMsvEmailEntry entry = iEntry.Entry();
		if (!entry.Visible() || aClearMultiples ||
			entry.DisconnectedOperation() != EDisconnectedMultipleOperation)
			{
			entry.SetDisconnectedOperation(ENoDisconnectedOperations);
			entry.SetVisible(ETrue);
			ChangeEntryL(entry);
			}
		}
	}

void CImapOfflineControl::PrepareLocalOpL(TMsvId aId)
	{
	SetEntryL(aId);

	// clear the disconnected op flag
	TMsvEmailEntry entry = iEntry.Entry();
	entry.SetDisconnectedOperation(ENoDisconnectedOperations);
	ChangeEntryL(entry);
		
	SetEntryL(iEntry.Entry().Parent());
	}

TBool CImapOfflineControl::DoLocalOpL()
	{
	
	
	
	if (iCopyDirect->Count())
		{
		TMsvId id = (*iCopyDirect)[0];

		__LOG_FORMAT((KDefaultLog, "CImapOfflineControl::DoLocalOpL  Copy id %x to do %d",id, iCopyDirect->Count()));

		PrepareLocalOpL(id);
		
		iEntry.CopyEntryL(id, iDestination, iStatus);
		SetActive();
		return ETrue;
		}

	if (iMoveDirect->Count())
		{
		TMsvId id = (*iMoveDirect)[0];

		__LOG_FORMAT((KDefaultLog, "CImapOfflineControl::DoLocalOpL  Move id %x to do %d",id, iMoveDirect->Count()));

		PrepareLocalOpL(id);

		iEntry.MoveEntryL(id, iDestination, iStatus);
		SetActive();
		return ETrue;
		}

	if (iMoveToLocalDirect->Count())
		{
		TMsvId id = (*iMoveToLocalDirect)[0];
		
		__LOG_FORMAT((KDefaultLog, "CImapOfflineControl::DoLocalOpL  MoveToLocal id %x to do %d",id, iMoveToLocalDirect->Count()));
	
		PrepareLocalOpL(id);
	
		iEntry.CopyEntryL(id, iDestination, iStatus);	//	I do mean Copy
		SetActive();
		return ETrue;
		}
	
	return EFalse;
	}


void CImapOfflineControl::DoCancel()
	{
	CMsgActive::DoCancel();
	}

void CImapOfflineControl::DoComplete(TInt& /*aStatus*/)
	{

	}

void CImapOfflineControl::DoRunL()
	{

	// DBG((_L8("::DoRunL")));
	__LOG_FORMAT((KDefaultLog, "CImapOfflineControl::DoRunL"));

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
		CImOffLineOperation* op = new(ELeave)CImOffLineOperation();
		CleanupStack::PushL(op);
				
		op->SetDelete((*iMoveToLocalDirect)[0]);
		iMoveToLocalDirect->Delete(0,1);
		SaveOperationL(*op);
		
		CleanupStack::PopAndDestroy(op);
		}

	// Operation done. Do next one in selection
	DoLocalOpL();
	
	//update the progrees info
	++iProgressMsgsDone;
	}


EXPORT_C TImap4CompoundProgress CImapOfflineControl::Progress()
	{	
	iProgress.iGenericProgress.iType=EImap4GenericProgressType;
	iProgress.iGenericProgress.iOperation=iRequestedOperation;
	iProgress.iGenericProgress.iMsgsToDo=iProgressMsgsToDo;
	iProgress.iGenericProgress.iMsgsDone=iProgressMsgsDone;

	return iProgress;
	}
