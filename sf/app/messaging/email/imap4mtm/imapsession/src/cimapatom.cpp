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
//

#include "cimapatom.h"
#include "cimapsessionconsts.h"

CImapAtom* CImapAtom::NewLC()
// static method
	{
	CImapAtom* self = new(ELeave)CImapAtom();
	CleanupStack::PushL(self);
	
	// no ConstructL()
	
	return self;
	}

CImapAtom::CImapAtom()
	{
	}

CImapAtom::~CImapAtom()
	{
	}

/**
During parsing, sets the string data of this atom.
@param The string data of this atom.
@param Whether the data was provided as a quoted string.
*/
void CImapAtom::Set(const TDesC8& aAtom, TBool aAtomIsQuoted)
	{
	// Save this atom in here
	iAtom.Set(aAtom);
	iAtomIsQuoted = aAtomIsQuoted;
	}

/**
During parsing, sets the child of this atom.
@param aNewChild The child of this atom.  Ownership is not transferred.
*/
void CImapAtom::AddChild(CImapAtom* aNewChild)
	{
	// Set child pointer
	iChild=aNewChild;
	}

/**
During parsing, sets the sibling of this atom.
@param aNewNext The sibling of this atom.  Ownership is not transferred.
*/
void CImapAtom::AddNext(CImapAtom* aNewNext)
	{
	// Set next pointer
	iNext=aNewNext;
	}

/**
@return The child of this atom, or NULL if this atom has no child.  Ownership is not transferred.
*/
CImapAtom* CImapAtom::Child()
	{
	return iChild;
	}

/**
@return The sibling of this atom, or NULL if this atom has no sibling.  Ownership is not transferred.
*/
CImapAtom* CImapAtom::Next()
	{
	return iNext;
	}

/**
Use CompareF to match the provided string with this atom's data.
@param aVal the string to match
@return whether this atom's data matches the provided string.
*/
TBool CImapAtom::Match(const TDesC8& aVal)
	{
	// Compare and return result
	return (iAtom.CompareF(aVal)==0);
	}

/**
@param bNString whether a non-quoted "NIL" should be treated as a NULL string
@return The string data of this atom
*/
const TDesC8& CImapAtom::Atom(TBool aNString)
	{
	if (aNString && !iAtomIsQuoted && (iAtom.CompareF(KImapTxtNil()) == 0))
		{
		return KNullDesC8();
		}
		
	return iAtom;
	}

/**
@return Whether the data was provided as a quoted string.
*/
TBool CImapAtom::AtomIsQuoted()
	{
	return iAtomIsQuoted;
	}

/**
This method should only be used by CImapAtomParser.
During parsing, a ReAllocL() may be required on the heap buffer that this atom and its 
descendants' string data descriptor points at.
This happens when the heap buffer needs to be expanded.
If this causes the heap buffer's address to change, then this atom and its descendants' 
pointer descriptors need updating.
@param The address of the new heap buffer
@param The address of the data within the heap buffer before the ReAllocL took place.
*/
void CImapAtom::FixupL(const HBufC8 *aNewBuffer, const TText8 *aOldBuffer)
	{
   // Fixup descriptor pointers
	CArrayFixFlat<CImapAtom*>* atomStack = new (ELeave) CArrayFixFlat<CImapAtom*>(10);
	CleanupStack::PushL(atomStack);

	atomStack->AppendL(this);
	CImapAtom* currentAtom;
	while (atomStack->Count() != 0)
   		{
		// Pop the top atom off of the stack
		currentAtom = (*atomStack)[atomStack->Count() - 1];
 		atomStack->ResizeL(atomStack->Count() - 1);
 
		// Fix up the current atom
		if (currentAtom->iAtom.Length()>0)
			{
			// Find offset from start of old buffer
			TInt start=(currentAtom->iAtom.Ptr()-aOldBuffer);

 			// Make new descriptor & assign it
			TPtrC8 bufptr(aNewBuffer->Ptr()+start,currentAtom->iAtom.Length());
			currentAtom->iAtom.Set(bufptr); // Note that we are setting the real iAtom not the copy returned by Atom()
			}
 
		// Add the first sibling to the stack,
		// subsequent siblings are added when this sibling is visited
		CImapAtom* siblingAtom = currentAtom->Next();
		if (siblingAtom)
			{
			atomStack->AppendL(siblingAtom);
			}
   
		// Add child to the stack
		CImapAtom* childAtom = currentAtom->Child();
		if (childAtom)
			{
			atomStack->AppendL(childAtom);
			}			
   		}
   
	CleanupStack::PopAndDestroy(atomStack);
   	}

