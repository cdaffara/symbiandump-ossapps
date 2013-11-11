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
// IMAP Folder indexing header
// 
//

#if !defined(__FLDINDEX_H__)
#define __FLDINDEX_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

// We need this for TMsvId
#include <msvstd.h>

// Actual entries stored in the index
class CImImap4IndexEntry:public CBase
/**
@internalComponent
@released
*/
	{
public:
	CImImap4IndexEntry();
	CImImap4IndexEntry(CImImap4IndexEntry& aFrom);

	CImImap4IndexEntry& operator=(CImImap4IndexEntry& aFrom);

public:
	TUint32			iUid;
	TMsvId			iMsvId;
	};

class CImImap4FolderIndex:public CBase
/**
@internalComponent
@released
*/
	{
public:
	CImImap4FolderIndex();
	~CImImap4FolderIndex();

	// Clear index
	void Reset();

	// Note number of entries
	void SetSizeL(const TUint aEntries);
	TInt Size();

	// Access an entry directly
	CImImap4IndexEntry& operator[] (const TInt aIndex);

	// Sort index by UID
	void Sort();

	// Expunge a message
	void Expunge(const TUint aMsgNr);

private:
	// Set a UID for a message number
	void SetUid(const TUint aMsgNr, const TUint aMsgUid);

	// Set a TMsvId for a message number
	void SetMsvId(const TUint aMsgnr, const TMsvId aMsvId);

	// Find a message number
	TUint FindMsg(const TUint aMsgUid);

private:
	TInt								iSize;
	CArrayFix<CImImap4IndexEntry>*		iIndex;
	};
#endif
