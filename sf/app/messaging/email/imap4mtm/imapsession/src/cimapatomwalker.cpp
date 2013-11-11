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

#include "cimapatomwalker.h"
#include "cimapatom.h"

#include "cimapsessionconsts.h"
#include "cimapcommand.h"
#include "imappaniccodes.h"


CImapAtomWalker* CImapAtomWalker::NewL(TInt aLogId)
// static method
	{
	return new(ELeave)CImapAtomWalker(aLogId);
	}
	
CImapAtomWalker::CImapAtomWalker(TInt aLogId)
	: iLogId(aLogId)
	{}
	
CImapAtomWalker::~CImapAtomWalker()
	{
	iAtomStack.Reset();
	}

/**
Sets the root atom
*/
void CImapAtomWalker::SetRootL(CImapAtom* aRootAtom)
	{
	iAtomStack.Reset();	
	
	iAtomStack.AppendL(aRootAtom);
	iCurrentAtom = aRootAtom;
	}

/**
Walks down to the child of the current atom, and updates the current atom.
If the current atom has no child, then this method will leave with KErrCorrupt.
*/
void CImapAtomWalker::WalkDownL()
	{	
	// Try going down
	CImapAtom* candidateAtom = iCurrentAtom->Child();
	if (candidateAtom)
		{
		iAtomStack.AppendL(candidateAtom);
		iCurrentAtom = candidateAtom;
		}
	else
		{
		// WalkDownL() should only be called when the caller is expecting a child to walk down to.
		// So if there is no such child, the incoming data must be corrupt.
		CImapCommand::CorruptDataL(iLogId);
		}	
	}

/**
Walks across to the next sibling of the current atom, and updates the current atom.
@param aLeaveIfNull If ETrue, then if the current atom has no next sibling, then this method will leave with KErrCorrupt.
@return the updated current atom or NULL if not found.  Ownership is not transferred.
*/
CImapAtom* CImapAtomWalker::WalkAcrossL(TBool aLeaveIfNull)
	{
	CImapAtom* candidateAtom = iCurrentAtom->Next();
	if (candidateAtom)
		{
		// Check for internal programming error
		__ASSERT_DEBUG(iAtomStack.Count() > 0, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomWalkerStackIsEmpty));
		
		iAtomStack[iAtomStack.Count() - 1] = candidateAtom;
		iCurrentAtom = candidateAtom;
		}
	else if (aLeaveIfNull)
		{
		// Caller has signified that they are expecting the next atom to be non-NULL.
		// So of there is no next atom, the incoming data must be corrupt.
		CImapCommand::CorruptDataL(iLogId);
		}
		
	return candidateAtom;
	}

/**
Walks up to the parent of the current atom, and updates the current atom.
If the current atom has no parent, then this method will leave with KErrUnderflow.
*/
void CImapAtomWalker::WalkUpL()
	{
	TInt topOfStack = iAtomStack.Count() - 1;
	
	// Check for internal programming error
	__ASSERT_DEBUG(topOfStack > 0, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomWalkerNothingToWalkUpTo));
	
	// Go up
	iAtomStack.Remove(topOfStack);
	--topOfStack;
	
	iCurrentAtom = iAtomStack[topOfStack];
	
	// Check for internal programming error.
	__ASSERT_DEBUG(iCurrentAtom != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomWalkerNullStackEntry)); // would never expect this to be NULL as it has come from the stack!
	}

/** 
Returns a pointer to the child of the current atom if one exists, or NULL.
The current atom is NOT updated.
@return the child of the current atom or NULL.  Ownership is not transferred.
*/
CImapAtom* CImapAtomWalker::PeekDown() const
	{
	return iCurrentAtom->Child();
	}
/** 
Returns a pointer to the next sibling of the current atom if one exists, or NULL.
The current atom is NOT updated.
@return the next sibling of the current atom or NULL.  Ownership is not transferred.
*/
CImapAtom* CImapAtomWalker::PeekAcross() const
	{
	return iCurrentAtom->Next();
	}

/**
Returns whether the supplied string matches the string in the current atom.
String matching uses Folded comparison - i.e. case insensitive.
@param aValue the string to match with the current atom's string.
@return whether the strings match.
*/
TBool CImapAtomWalker::CurrentMatch(const TDesC8& aValue) const
	{
	return iCurrentAtom->Match(aValue);
	}

/**
The string assoiated with current atom.
If the string is to be interpreted as an nstring, then if the actual string 
was "NIL" and was not quoted in the input data, an empty string will be returned.
@param aNString whether to return an empty string in place of a non-quoted "NIL"
@return the string assoiated with current atom.
*/
const TDesC8& CImapAtomWalker::CurrentDes(TBool aNString) const
	{
	return iCurrentAtom->Atom(aNString);
	}

/**
@return a pointer to the current atom.  Ownership is not transferred.
*/
CImapAtom* CImapAtomWalker::Current() const
	{
	return iCurrentAtom;
	}

/**
Walks across to the next atom, updating the current atom.
This method expects the next atom to be either "NIL" or "(", and will leave 
with the KErrCorrupt code if any other atom is found.
@return an enumeration that signifies whether "NIL" or "(" was found.
*/
CImapAtomWalker::TAtomType CImapAtomWalker::WalkAcrossToNilOrOpenL()
	{
	CImapAtom* atom = WalkAcrossL(ETrue);
	
	// Check for internal programming error: WalkAcrossL(ETrue) will leave rather than return NULL
	__ASSERT_DEBUG(atom != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomWalkerWalkedAcrossToNull));

	TAtomType atomType = EAtomNil;
	
	if (atom->Match(KImapTxtNil()))
		{
		atomType = EAtomNil;
		}
	else if (atom->Match(KImapTxtOpenBracket()))
		{
		atomType = EAtomOpen;
		}
	else
		{
		// Not "NIL" or "(" as expected.
		// Data must be corrupt.
		CImapCommand::CorruptDataL(iLogId);
		}

	return atomType;
	}

TInt CImapAtomWalker::StackCount() const
	{
	return iAtomStack.Count();
	}
