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
// IMAP Folder indexing header
// 
//

#if !defined(__CIMAPFOLDERINDEX_H__)
#define __CIMAPFOLDERINDEX_H__

#include <e32std.h>
#include <e32base.h>
#include <msvstd.h>

/**
Actual entries stored in the index
@internalComponent
@prototype
*/
class CImapFolderIndexEntry:public CBase
	{
public:
	CImapFolderIndexEntry();
	CImapFolderIndexEntry(CImapFolderIndexEntry& aFrom);

	CImapFolderIndexEntry& operator=(CImapFolderIndexEntry& aFrom);

public:
	TUint32			iUid;
	TMsvId			iMsvId;
	};

class CImapFolderIndex:public CBase
/**
@internalComponent
@released
*/
	{
public:
	CImapFolderIndex();
	~CImapFolderIndex();

	// Clear index
	void Reset();

	// Note number of entries
	void SetSizeL(const TUint aEntries);
	TInt Size() const;

	// Access an entry directly
	CImapFolderIndexEntry& operator[] (const TInt aIndex);

	// Sort index by UID
	void Sort();

	// Expunge a message
	void Expunge(const TUint aMsgNr);

	// Find a message number
	TUint FindMsg(const TUint aMsgUid);

private:
	// Set a UID for a message number
	void SetUid(const TUint aMsgNr, const TUint aMsgUid);

	// Set a TMsvId for a message number
	void SetMsvId(const TUint aMsgnr, const TMsvId aMsvId);

private:
	TInt								iSize;
	CArrayFix<CImapFolderIndexEntry>*	iIndex;
	};

#endif //__CIMAPFOLDERINDEX_H__

