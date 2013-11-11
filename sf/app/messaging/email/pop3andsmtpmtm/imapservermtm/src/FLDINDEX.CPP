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
// IMAP Folder indexing
// 
//

#include "fldindex.h"
#include "impspan.h"

// Illegal UID we use for marker
const TUint KIllegalUID	= 0xffffffff;

CImImap4IndexEntry::CImImap4IndexEntry()
	{
	// Default values
	iUid=0;
	iMsvId=-1;
	}

CImImap4IndexEntry::CImImap4IndexEntry(CImImap4IndexEntry& aFrom)
	{
	// Copy entry
	iUid=aFrom.iUid;
	iMsvId=aFrom.iMsvId;
	}

CImImap4IndexEntry& CImImap4IndexEntry::operator=(CImImap4IndexEntry& aFrom)
	{
	// Copy entry
	iUid=aFrom.iUid;
	iMsvId=aFrom.iMsvId;
	return(*this);
	}

CImImap4FolderIndex::CImImap4FolderIndex()
	{
	__DECLARE_NAME(_S("CImImap4FolderIndex"));
	}

CImImap4FolderIndex::~CImImap4FolderIndex()
	{
	// Dispose of index
	delete iIndex;
	}

void CImImap4FolderIndex::Reset()
	{
	// Dispose of index
	delete iIndex;
	iIndex=NULL;
	iSize=0;
	}

void CImImap4FolderIndex::SetSizeL(const TUint aEntries)
	{
	// Set number of entries
	if (!iIndex)
		{
		// Create new array (granularity 8 entries)
		iIndex=new (ELeave) CArrayFixFlat<CImImap4IndexEntry>(8);
		}

	// Alter size, filling unused entries with 0xffffffff
	CImImap4IndexEntry blank;
	blank.iUid=KIllegalUID;
	blank.iMsvId=0;
	iIndex->ResizeL(aEntries,blank);

	// Save new size
	iSize=aEntries;
	}

TInt CImImap4FolderIndex::Size()
	{
	// Return current size
	return(iSize);
	}

void CImImap4FolderIndex::SetUid(const TUint aMsgNr, const TUint aMsgUid)
	{
	__ASSERT_ALWAYS(aMsgNr<=TUint(iSize),gPanic(EMsgnrOutOfRange));

	// Is UID already set?
	if ((*iIndex)[aMsgNr-1].iUid!=KIllegalUID && (*iIndex)[aMsgNr-1].iUid!=aMsgUid)
		{
		// CHANGING a UID? No way!
		gPanic(ECantChangeUID);
		}

	// Set a UID for a message number
	(*iIndex)[aMsgNr-1].iUid=aMsgUid;
	}

void CImImap4FolderIndex::SetMsvId(const TUint aMsgNr, const TMsvId aMsvId)
	{
	__ASSERT_ALWAYS(aMsgNr<=TUint(iSize),gPanic(EMsgnrOutOfRange));

	// Is MsvId already set?
	if ((*iIndex)[aMsgNr-1].iMsvId!=0 && (*iIndex)[aMsgNr-1].iMsvId!=aMsvId)
		{
		// CHANGING a MsvId? No way!
		gPanic(ECantChangeMsvId);
		}

	// Set a MsvId for a message number
	(*iIndex)[aMsgNr-1].iMsvId=aMsvId;
	}

void CImImap4FolderIndex::Expunge(const TUint aMsgNr)
	{
	__ASSERT_ALWAYS(aMsgNr<=TUint(iSize),gPanic(EMsgnrOutOfRange));

	// Remove entry from index
	iIndex->Delete(aMsgNr-1);
	iSize--;
	}

TUint CImImap4FolderIndex::FindMsg(const TUint aMsgUid)
	{
	__ASSERT_ALWAYS(iSize>0,gPanic(EIndexEmpty));

	// Replace this with a binary search...
	TInt i=0;
	do
		{
		// UID match?
		if ((*iIndex)[i].iUid==aMsgUid)
			return(i+1);

		i++;
		}
	while(i<iSize);

	// Failure (0 not a legal message number)
	return(0);
	}

// Access entry directly
CImImap4IndexEntry& CImImap4FolderIndex::operator[] (const TInt aIndex)
	{
	return((*iIndex)[aIndex]);
	}

// Sort index by UID
void CImImap4FolderIndex::Sort()
	{
	// Sorting object
	TKeyArrayFix uidKey(_FOFF(CImImap4IndexEntry,iUid),ECmpTUint32);

	// Perform the sort
	iIndex->Sort(uidKey);
	}
