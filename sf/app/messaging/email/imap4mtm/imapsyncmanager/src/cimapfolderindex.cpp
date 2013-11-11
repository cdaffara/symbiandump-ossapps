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
// IMAP Folder indexing
// 
//

#include "imappaniccodes.h"
#include "cimapfolderindex.h"

// Illegal UID we use for marker
const TUint KIllegalUID	 = 0xffffffff;

CImapFolderIndexEntry::CImapFolderIndexEntry()
	{
	// Default values
	iUid = 0;
	iMsvId = -1;
	}

CImapFolderIndexEntry::CImapFolderIndexEntry(CImapFolderIndexEntry& aFrom)
	{
	// Copy entry
	iUid = aFrom.iUid;
	iMsvId = aFrom.iMsvId;
	}

CImapFolderIndexEntry& CImapFolderIndexEntry::operator = (CImapFolderIndexEntry& aFrom)
	{
	// Copy entry
	iUid = aFrom.iUid;
	iMsvId = aFrom.iMsvId;
	return(*this);
	}

CImapFolderIndex::CImapFolderIndex()
	{
	__DECLARE_NAME(_S("CImapFolderIndex"));
	}

CImapFolderIndex::~CImapFolderIndex()
	{
	// Dispose of index
	delete iIndex;
	}

void CImapFolderIndex::Reset()
	{
	// Dispose of index
	delete iIndex;
	iIndex = NULL;
	iSize = 0;
	}

void CImapFolderIndex::SetSizeL(const TUint aEntries)
	{
	// Set number of entries
	if (!iIndex)
		{
		// Create new array (granularity 8 entries)
		iIndex = new (ELeave) CArrayFixFlat<CImapFolderIndexEntry>(8);
		}

	// Alter size, filling unused entries with 0xffffffff
	CImapFolderIndexEntry blank;
	blank.iUid = KIllegalUID;
	blank.iMsvId = 0;
	iIndex->ResizeL(aEntries,blank);

	// Save new size
	iSize = aEntries;
	}

TInt CImapFolderIndex::Size() const
	{
	// Return current size
	return(iSize);
	}

void CImapFolderIndex::SetUid(const TUint aMsgNr, const TUint aMsgUid)
	{
	__ASSERT_ALWAYS(aMsgNr <= TUint(iSize),User::Panic(_L("CImapFolderIndex"), TImapServerPanic::EMsgnrOutOfRange));

	// Is UID already set?
	if ((*iIndex)[aMsgNr-1].iUid != KIllegalUID && (*iIndex)[aMsgNr-1].iUid != aMsgUid)
		{
		// CHANGING a UID? No way!
		User::Panic(_L("CImapFolderIndex"),TImapServerPanic::ECantChangeUID);
		}

	// Set a UID for a message number
	(*iIndex)[aMsgNr-1].iUid = aMsgUid;
	}

void CImapFolderIndex::SetMsvId(const TUint aMsgNr, const TMsvId aMsvId)
	{
	__ASSERT_ALWAYS(aMsgNr <= TUint(iSize),User::Panic(_L("CImapFolderIndex"), TImapServerPanic::EMsgnrOutOfRange));

	// Is MsvId already set?
	if ((*iIndex)[aMsgNr-1].iMsvId != 0 && (*iIndex)[aMsgNr-1].iMsvId != aMsvId)
		{
		// CHANGING a MsvId? No way!
		User::Panic(_L("CImapFolderIndex"), TImapServerPanic::ECantChangeMsvId);
		}

	// Set a MsvId for a message number
	(*iIndex)[aMsgNr-1].iMsvId = aMsvId;
	}

void CImapFolderIndex::Expunge(const TUint aMsgNr)
	{
	__ASSERT_ALWAYS(aMsgNr <= TUint(iSize),User::Panic(_L("CImapFolderIndex"),TImapServerPanic::EMsgnrOutOfRange));

	// Remove entry from index
	iIndex->Delete(aMsgNr-1);
	iSize--;
	}

TUint CImapFolderIndex::FindMsg(const TUint aMsgUid)
	{
	__ASSERT_ALWAYS(iSize>0,User::Panic(_L("CImapFolderIndex"),TImapServerPanic::EIndexEmpty));

	// Binary search.
	TInt max = iIndex->Count() - 1;   //max index
	TInt min = 0;   //min index
	TInt index;

	index = max/2;

	while (((*iIndex)[index].iUid != aMsgUid) && (min <= max))
		{
		if ((*iIndex)[index].iUid > aMsgUid)
			{
		    max = --index;
		    }
		else
			{
		    min = ++index;
		    }

		index = (max + min)/2;
		}

	if(index < iIndex->Count())
		{
		if((*iIndex)[index].iUid == aMsgUid)
			{
			return(++index);
			}
		}
		
	// Failure (0 not a legal message number)
	return(0);
	}

// Access entry directly
CImapFolderIndexEntry& CImapFolderIndex::operator[] (const TInt aIndex)
	{
	return((*iIndex)[aIndex]);
	}

// Sort index by UID
void CImapFolderIndex::Sort()
	{
	// Sorting object
	TKeyArrayFix uidKey(_FOFF(CImapFolderIndexEntry,iUid),ECmpTUint32);

	// Perform the sort
	iIndex->Sort(uidKey);
	}
