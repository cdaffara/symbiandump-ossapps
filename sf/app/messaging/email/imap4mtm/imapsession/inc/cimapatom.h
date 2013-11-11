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
// CImapAtom header
// 
//

#ifndef __CIMAPATOM_H__
#define __CIMAPATOM_H__

#include <e32base.h>
#include <e32cons.h>
#include <mentact.h>
#include <imsk.h>
#include <iapprefs.h>

/**
Represents an atom of data from incoming IMAP bodystructure data.
This class contains a TPtrC which points to the data 'owned' by 
this atom in the buffer. It also contains next (sibling) and child pointers,
with which the tree is constructed.

@internalComponent
@prototype
*/
class CImapAtom : public CBase
	{
	friend class CImapAtomParser;
 
public:
	static CImapAtom* NewLC();
	~CImapAtom();
	
	void		Set(const TDesC8& aAtom, TBool aAtomIsQuoted);
	void		AddChild(CImapAtom *aNewChild);
	void		AddNext(CImapAtom *aNewNext);

	// Get child/next pointers
	CImapAtom*	Child();
	CImapAtom*	Next();

	const TDesC8& Atom(TBool aNString);
	TBool AtomIsQuoted();
	
	TBool		Match(const TDesC8& aValue);

private:
	CImapAtom();

	// Only to be used by CImapAtomParser
	void FixupL(const HBufC8 *aNewBuffer, const TText8 *aOldBuffer);

private:
	/**
	The string data for this atom.
	*/
	TPtrC8		iAtom;
	/**
	Whether the data for this atom was delivered within quotes.
	*/
	TBool		iAtomIsQuoted;
	
	/**
	Pointer to the first nested atom after an opening bracket (which may or may not be an opening bracket too)
	Not owned by this class, but can be set to NULL when this atom does not have a child.
	*/
	CImapAtom*	iChild;
	/**
	If this atom is an opening bracket, then this points to the next atom after the matching closing bracket
	Otherwise, this just points to the next atom that was found.
	Not owned by this class, but can be set to NULL when this atom does not have a next atom.
	*/
	CImapAtom*	iNext;
	};

#endif // __CIMAPATOM_H__
