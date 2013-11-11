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
// CImapAtomParser header
// 
//

#ifndef __CIMAPATOMPARSER_H__
#define __CIMAPATOMPARSER_H__

#include <e32base.h>

// forward declaration
class CImapAtom;

/**
This class takes an IMAP response string and parses it into a tree of atoms.
Whenever an open bracket is detected, a subtree is formed until the matching 
close bracket is found.
Quoted literals are treated as a single atom.
The parser will stop parsing when the a close bracket is found to match the very first open bracket.
The remaining unparsed data can be accessed via UnparsedData.
@internalComponent
@prototype
*/
class CImapAtomParser : public CBase
	{
public:
	/**
	States of reply parser
	
	@internalComponent
	@prototype
	*/
	enum TParserState
		{
		EStateAtomWait,
		EStateInAtom,
		EStateLiteralLength,
		EStateLiteralSkip,
		EStateLiteralFetch,
		EStateParseComplete,
		};

	~CImapAtomParser();

	static CImapAtomParser*	NewL(TBool aAllowParseAtTopLevel, TInt aLogId);

	TBool ProcessLineL(const TDesC8& aLine);
	void ProcessLiteralBlockL(const TDesC8& aLiteralBlock);

	// Get the root atom of the parse tree
	CImapAtom*	RootAtom();
	
	const TPtrC8& UnparsedData();

	HBufC8*	DetachBuffer();

private:
	CImapAtomParser(TBool aAllowParseAtTopLevel, TInt aLogId);
	void ConstructL();

	void BufferAppendL(const TChar aChar);

	void PushL(CImapAtom* aItem);
	CImapAtom* PopL();

	void AddAtomL();
private:
	/**
	If you have a string such as "aaaa (bbbb (xx yy)) cccc".
	With this set to ETrue, then the whole of "aaaa (bbbb (xx yy)) cccc" will be parsed.
	But when set to EFalse, only "aaaa (bbbb (xx yy))" will be parsed as the last matching bracket has been found.
	*/
	TBool					iAllowParseAtTopLevel;

	TParserState			iParserState;
	TBool					iParserQuoted;
	TBool					iGotEscape;
	TInt					iLiteralLength;
	TInt 					iLiteralSkip;

	/**
	A heap buffer that stores all the data pointed to by the CImapAtom objects.
	*/
	HBufC8*					iBuffer;
	TInt					iBufferSize; // can we use iBuffer->MaxLength() here?

	/**
	The root atom.  Owned.
	*/
	CImapAtom*				iRootAtom;
	/**
	The atom that is currently being procesed.  Not owned.
	*/
	CImapAtom*				iAtom;
	/**
	Index of the first character of the atom currently being processed
	*/
	TInt					iAtomStart;

	/**	
	Atom array owns ALL the atom objects EXCEPT the root atom.
	*/	
	RPointerArray<CImapAtom> iAtomArray; 
	
	/**
	A stack that is used to navigate back up an atom tree as it is being built.
	This arrat does not own the objects that it points at.
	*/
	RPointerArray<CImapAtom> iAtomStack;
	/**
	Whether the next atom object to be constructed should be added to the tree as
	a child of the current atom (ETrue) or as its sibling (EFalse).
	*/
	TBool					iNextIsChild;
	
	/**
	Points at the unparsed portion of the last line to be processed.
	This pointer descriptor is only valid while the descriptor it points at is valid
	This pointer descriptor will only be set after ProcessLineL() has returned
	EFalse to indicate that no more data is expected.
	*/
	TPtrC8					iUnparsedData;
	
	/**
	Log id of owning session
	*/
	TInt iLogId;
	TInt iAttachmentLength;
	};

#endif // __CIMAPATOMPARSER_H__
