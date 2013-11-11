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

#ifndef __CIMAPATOMWALKER_H__
#define __CIMAPATOMWALKER_H__

#include <e32base.h>

class CImapAtom;

/**
Navigates a tree of CImapAtom objects, by storing the current atom together 
with an atom stack that makes it possible to track the current atom's parent.

Methods are provided to "Walk" the atom tree - updating the current atom,
and to "Peek" the tree without updating the current atom.
*/
class CImapAtomWalker : public CBase
	{
public:
	static CImapAtomWalker* NewL(TInt aLogId);
	~CImapAtomWalker();

	void SetRootL(CImapAtom* aRootAtom);
	
	void WalkDownL();
	CImapAtom* WalkAcrossL(TBool aLeaveIfNull);
	void WalkUpL();
		
	CImapAtom* PeekDown() const;	
	CImapAtom* PeekAcross() const;
	
	TBool CurrentMatch(const TDesC8& aValue) const;
	
	const TDesC8& CurrentDes(TBool aNString) const;	
	CImapAtom* Current() const;
	
	TInt StackCount() const;
	
	enum TAtomType
		{
		EAtomNil, // atom matches "NIL": This corresponds to the nil component in section 9 of RFC3501.
		EAtomOpen, // atom matches "(": This atom has a child.
		};		
	TAtomType WalkAcrossToNilOrOpenL();
	
private:
	CImapAtomWalker(TInt aLogId);
	
private:
	CImapAtom* iCurrentAtom;	// not owned
	RPointerArray<CImapAtom> iAtomStack; // does not own the objects it points at
	TInt iLogId;
	};

#endif // __CIMAPATOMWALKER_H__
