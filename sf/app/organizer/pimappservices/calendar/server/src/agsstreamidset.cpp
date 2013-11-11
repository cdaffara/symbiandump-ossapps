// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "agsstreamidset.h"

#include "agsentrymodel.h"
#include "agsfileconverter.h"
#include "agmpanic.h"

#include <s32stor.h>

CAgnStreamIdSet* CAgnStreamIdSet::NewL()
//
// Create a CAgnStreamIdSet
//
	{

	CAgnStreamIdSet* Self = new(ELeave) CAgnStreamIdSet;
	CleanupStack::PushL(Self);
	Self->iArray = new(ELeave) CArrayFixFlat<TStreamId>(EGranularity);
	Self->iAddRollbackArray = new(ELeave) CArrayFixFlat<TStreamId>(ERollbackGranularity);
	Self->iDeleteRollbackArray = new(ELeave) CArrayFixFlat<TStreamId>(ERollbackGranularity);
	CleanupStack::Pop();
	return (Self);
	}


CAgnStreamIdSet::~CAgnStreamIdSet()
//
// Destructor
//
	{

	delete iArray;
	delete iAddRollbackArray;
	delete iDeleteRollbackArray;
	}

TStreamId CAgnStreamIdSet::CreateL(CStreamStore& aStore)
//
// Create a new stream for the set 
//
	{
	iStreamId = aStore.ExtendL();
	return (iStreamId);
	}

void CAgnStreamIdSet::AddL(TStreamId aId)
//
// Add aId to the set
//
	{

	iArray->AppendL(aId);

	TRAPD(ret,iAddRollbackArray->AppendL(aId));
	if (ret != KErrNone)
		{
		TInt pos;
		if (Find(aId,pos)==0)
			iArray->Delete(pos);
		User::Leave(ret);
		}
	// Don't save this until Commit is called
	}

void CAgnStreamIdSet::DeleteL(TStreamId aId)
//
// Delete aId from the set
//
	{

	TInt pos;
	if (Find(aId,pos)==0) //Find returns 0 if found,+ve if not found,-ve if empty array
		{
		iDeleteRollbackArray->AppendL(aId);
		iArray->Delete(pos);
		}
	// Don't save this until Commit is called
	}

void CAgnStreamIdSet::SaveL(CStreamStore& aStore) const
//
// Save the set
//
	{

	RStoreWriteStream stream;
	stream.ReplaceLC(aStore,iStreamId);
	stream << *iArray;
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	}

void CAgnStreamIdSet::LoadL(CStreamStore& aStore,TStreamId aStreamId)
//
// Restore the token set identified by aStreamId from within aStore
//
	{
	iStreamId = aStreamId;
	iArray->Reset();
	RStoreReadStream in;
	in.OpenLC(aStore,iStreamId);
	in >> *iArray;
	CleanupStack::PopAndDestroy();
	}


void CAgnStreamIdSet::CommitL(CStreamStore& aStore)
//
// Commit changes to the in memory array and the file
//
	{
	iAddRollbackArray->Reset();
	iDeleteRollbackArray->Reset();
	iArray->Compress();
	SaveL(aStore);
	}


void CAgnStreamIdSet::RollbackL()
//
// Rollback changes made:- delete all added entries and reinstate all deleted entries
//

	{

	// remove all newly added entry stream ids
	for (TInt ii = iAddRollbackArray->Count()-1;ii >= 0;--ii)
		{
		TInt pos;
		TInt found = Find((*iAddRollbackArray)[ii],pos);		
		if (found==0) //Find returns 0 if found,+ve if not found,-ve if empty array	
			iArray->Delete(pos);
		__ASSERT_DEBUG(found==0,Panic(EAgmErrCouldNotFindNewEntryId));
		}
	iAddRollbackArray->Reset();	

	// re-add all deleted entries
	for (TInt jj = iDeleteRollbackArray->Count()-1;jj >= 0;--jj)
		iArray->AppendL((*iDeleteRollbackArray)[jj]);
	iDeleteRollbackArray->Reset();
	}

void CAgnStreamIdSet::ResetIteratorToStart()
/**
If there are items in the set then the iterator postion is set to the first item and
ETrue is returned otherwise EFalse is returned

@internalAll
*/
	{
	
	if (iArray->Count())
		{
		iPos=0;
		}
	}

TBool CAgnStreamIdSet::Next()
/**
If there is a next item in the set then the iterator is positioned to it and ETrue is
returned otherwise EFalse is returned.

@internalAll
*/
	{

	if (iPos + 1 < iArray->Count())
		{
		++iPos;
		return (ETrue);
		}
	return (EFalse);
	}

TBool CAgnStreamIdSet::At(TStreamId& aItem)	const
/**
If there are items in the set then the item at the current position of the iterator is
placed in aItem and ETrue is returned otherwise EFalse is returned.

@internalAll
*/
	{

	if (iArray->Count())
		{
		if (iPos < iArray->Count())
			{
			aItem = (*iArray)[iPos];
			return (ETrue);
			}
		}
	return (EFalse);
	}

TInt CAgnStreamIdSet::Find(TStreamId aItem,TInt& aPos) const
//
// Returns 0 is aItem is found and sets aPos to its position, otherwise returns +ve if not
// found and -ve if the set is empty
//
	{

	TKeyArrayFix key(0,ECmpNormal8,sizeof(TStreamId));
	return (iArray->Find(aItem,key,aPos)); 	
	}


void CAgnStreamIdSet::Reset()
//
// Clear out all data
//
	{

	iArray->Reset();
	iAddRollbackArray->Reset();
	iDeleteRollbackArray->Reset();
	}



//
//                   CAgnModelStreamIdSet       
//
CAgnModelStreamIdSet* CAgnModelStreamIdSet::NewL()
//
// Create a CAgnModelStreamIdSet object
//
	{

	CAgnModelStreamIdSet* Self=new(ELeave) CAgnModelStreamIdSet;
	CleanupStack::PushL(Self);
	Self->ConstructL();
	CleanupStack::Pop();
	return (Self);
	}


void CAgnModelStreamIdSet::ConstructL()
//
// Constructs a CAgnModelStreamIdSet
//
	{

	iEntryStreamIdSet = CAgnStreamIdSet::NewL(); 
	}

CAgnModelStreamIdSet::~CAgnModelStreamIdSet()
//
// Destructor
//
	{

	delete iEntryStreamIdSet;
	}

TStreamId CAgnModelStreamIdSet::CreateL(CStreamStore& aStore,const TAgnVersion& aVersion) 
//
// Creates a network of streams in aStore, their stream id's being saved in a head
// stream and the id of this stream is returned. aVersion is the version of the model that is being used
// to create the file.
//
	{

	iVersion = aVersion;

	// create a	stream for the entry stream id set
	iEntrySetStreamId = iEntryStreamIdSet->CreateL(aStore);
	iEntryStreamIdSet->SaveL(aStore);

	// create  a stream for the unique identifier value
	iNextLocalUidValueStreamId = aStore.ExtendL();

	// create a stream to hold the entry store object's data
	iEntryManagerStreamId = aStore.ExtendL();

	// create a stream to hold information about the file (file id)
	iFileInformationStreamId = aStore.ExtendL();

	// Save the id's of the streams created to a head stream and return its stream id
	RStoreWriteStream stream;
	TStreamId headStreamId = stream.CreateLC(aStore);
	stream << *this;
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	return (headStreamId);
	}
	
void CAgnModelStreamIdSet::InternalizeL(RReadStream& aStream)
	{
	aStream >> iVersion;
	aStream >> iEntrySetStreamId;
	aStream >> iNextLocalUidValueStreamId;
	aStream >> iEntryManagerStreamId;
	aStream >> iFileInformationStreamId;
	}

void CAgnModelStreamIdSet::ExternalizeL(RWriteStream& aStream) const
	{
	TStreamId dcStreams(0);

	aStream << iVersion;
	aStream << iEntrySetStreamId;
	aStream << iNextLocalUidValueStreamId;
	aStream << iEntryManagerStreamId;
	aStream << iFileInformationStreamId;
	}

void CAgnModelStreamIdSet::LoadL(CStreamStore& aStore, TStreamId aStreamId)
	{
	RStoreReadStream in;
	in.OpenLC(aStore,aStreamId);
	
	in >> *this;
	
	CleanupStack::PopAndDestroy();
	iEntryStreamIdSet->LoadL(aStore,iEntrySetStreamId);
	}

void CAgnModelStreamIdSet::LoadL(CStreamStore& aStore, TStreamId aStreamId, CAgnCalendarConverter& aCalendarConverter)
	{
	RStoreReadStream in;
	in.OpenLC(aStore,aStreamId);
	
	// If a calendar converter was supplied, use it 
	// for internalizing this:
	aCalendarConverter.InternalizeModelStreamIdSetL(in, *this);
	
	CleanupStack::PopAndDestroy();
	iEntryStreamIdSet->LoadL(aStore,iEntrySetStreamId);
	}


void CAgnModelStreamIdSet::RollbackL()
	{
	iEntryStreamIdSet->RollbackL();
	}

void CAgnModelStreamIdSet::CommitL(CStreamStore& aStore)
	{
	iEntryStreamIdSet->CommitL(aStore);
	}


void CAgnModelStreamIdSet::Reset()
//
// Clear out all data
//
	{

	EntryStreamIdSet().Reset();
	iEntrySetStreamId = KNullStreamId;
	iNextLocalUidValueStreamId = KNullStreamId;
	iFileInformationStreamId = KNullStreamId;
	}

void CAgnModelStreamIdSet::ChangeFileVersionL(CStreamStore& aStore,TStreamId aStreamId, TAgnVersion& aNewVersion)
//
// Change the file version.
//
	{
	iVersion = aNewVersion;
	
	RStoreWriteStream out;
	out.OpenLC(aStore,aStreamId);
	out << *this;
	out.CommitL();
	CleanupStack::PopAndDestroy();
	}

