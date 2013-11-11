// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "caliteratorimpl.h"

#include "calclient.h"
#include "agmentry.h"
#include "calsessionimpl.h"

CCalIteratorImpl::CCalIteratorImpl(CCalSessionImpl& aSessionImpl)
	:iCalSessionImpl(aSessionImpl), iServer(aSessionImpl.Server())
	{
	iCalSessionImpl.IncrementReferenceCount();
	}

CCalIteratorImpl::~CCalIteratorImpl()
	{
	iCalSessionImpl.DecrementReferenceCount();
	delete iGuid;
	}

const TDesC8& CCalIteratorImpl::FirstL()
	{
	if (iServer.CreateEntryIteratorL(iCalSessionImpl.FileId()))
		{
		iFileEntry = ETrue; //file contains an entry
		FindUidL();
		if(iGuid)
			{
			return *iGuid;
			}
		else
			{
			NextL();
			}
		__ASSERT_ALWAYS(iGuid, User::Leave(KErrCorrupt));// At least there should be one entry which holds the guid
		return *iGuid;
		}
	else
		{//There is no entries - the file is empty -
		iFileEntry = EFalse;
		return KNullDesC8();
		} 
	}
/** 
Moves the entry iterator to the next entry which is a parent (parent and its children have the same Uid) and retrive its Uid. . 

@capability None
@return a descriptor which contains the Uid of the current entry. The return value can be a KNullDesC8 if there is no more parent entry in the file. 
*/
const TDesC8& CCalIteratorImpl::NextL()
	{
	if (!iFileEntry)
		{
		User::Leave(KErrCorrupt);
		}
		
	TBool available = iServer.EntryIteratorNextL(iCalSessionImpl.FileId());
	while (available)
		{
		FindUidL();
		if(iGuid)
			{
			return *iGuid;
			}
		
		available = iServer.EntryIteratorNextL(iCalSessionImpl.FileId());
		}
	return KNullDesC8();
	}


void CCalIteratorImpl::FindUidL()
	{/*
	 Update the iGuid which hols the guid of the current entry.
	 
	 The descriptor HBufC* iGid may be one of the following values:

	 1. Contains the guid of a GS parent entry.
	 2. Null if it is a GS child entry
	 3. Unique id (converted into descriptor) if it is not a GS entry.
	 */
	delete iGuid;
	iGuid = NULL;

	CAgnEntry* entry = iServer.FetchEntryByIteratorL(iCalSessionImpl.FileId());
	
	// sanity check: if NULL something went wrong with the indexes
	
	__ASSERT_ALWAYS(entry, User::Leave(KErrGeneral));

	CleanupStack::PushL(entry);
	 
	if(entry->GsDataType()==CGsData::EParent)
		{
		iGuid = entry->Guid().AllocL();
		}

	CleanupStack::PopAndDestroy(entry);
	}
	
