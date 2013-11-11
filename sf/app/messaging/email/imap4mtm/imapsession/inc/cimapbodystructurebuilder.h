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

#ifndef __CIMAPBODYSTRUCTUREBUILDER_H__
#define __CIMAPBODYSTRUCTUREBUILDER_H__

#include <e32base.h>
#include "cimapenvelope.h"

class CImapBodyStructure;
class CImapAtomParser;
class CImapAtomWalker;
class CImapFetchResponse;

/**
Builds a tree of CImapBodyStructure objects in order to represent the incoming 
bodystructure data from the IMAP server.
Incoming data is passed into ProcessBlockL() which parses the data into an atom tree.
Once ProcessBlockL() has completed the atom tree, it uses ParseLoopL() to parse the 
atom tree into the bodystructure tree.
Finally, ownership of the completed bodystructure tree is passed to the CImapFetchResponse
object that was provided on construction.
*/
class CImapBodyStructureBuilder : public CBase
	{	
public:
	static CImapBodyStructureBuilder* NewL(CImapFetchResponse& aFetchResponse, TInt aLogId);
	~CImapBodyStructureBuilder();
			
	TBool ProcessBlockL(const TDesC8& aData);
	
	TPtrC8 UnparsedData();
			
private:
	/**
	States used by ParseLoopL();
	*/
	enum TParseStep
		{
		/**
		Indicates the start of the root body structure,	or that an embedded body structure has been identified.
		iAtomWalker is either positioned at the opening bracket of the body structure in question.
		A new bodystructure object will be constructed and pushed onto the bodystructure stack.
		*/
		EParseNewBodyStructure,
		/**
		Indicates that a "basic" bodystructure has been identified - i.e. not text, rfc822 or multipart.
		iAtomWalker is positioned at the subtype field of the structure.
		*/		 
		EParseBasic,
		/**
		Indicates that a "TEXT" bodystructure has been identified - i.e. not text, rfc822 or multipart.
		iAtomWalker is positioned at the subtype field of the structure.
		*/
		EParseText,
		/**
		Indicates that a "MESSAGE/RFC822" bodystructure has been identified - i.e. not text, rfc822 or multipart.
		iAtomWalker is positioned at the subtype field of the structure.
		*/
		EParseBodyTypeMessageRfc822,
		/**
		Indicates that the embedded bodystructure of a "MESSAGE/RFC822" has just completed parsing.
		iAtomWalker is positioned ready to walk accross to the next field of body-type-msg.
		*/
		EParseRemainderMessageRfc822,
		/**
		Indicates that the final embedded bodystructure of a MULTIPART structure has just completed parsing.
		iAtomWalker is positioned ready to walk accross to the next field of body-type-mpart.
		*/
		EParseRemainderMultipart,
		/**
		Indicates that a bodystructure has completed parsing.
		The current CImapBodyStructure will be popped off the bodystructure stack
		and ParseSubStructureCompleteL() will be called to tidy up and decide the next state.
		*/
		EParseSubStructureComplete,
		/**
		Finished parsing the atom tree.
		This state is used to exit the parse loop.
		*/
		EParseComplete
		};

	CImapBodyStructureBuilder(CImapFetchResponse& aFetchResponse, TInt aLogId);
	void ConstructL();
	
	void TransferBufferOwnershipToFetchResponseL();
	
	void ParseLoopL();

	void PushNewBodyStructureL();
	TBool PopBodyStructureL();

	TParseStep ParseSubStructureCompleteL();
	TParseStep ParseBodyStructureTypeL();
	
	// Each of the following ParseBodyXxxL() methods is named after the corresponding 
	// formal syntax item in RFC3501 section 9 that it is responsible for parsing.
		
	void ParseBodyTypeBasicL();
	void ParseBodyTypeTextL();
	void ParseBodyTypeMessageRfc822L();
		
	void ParseBodyFieldsL();
	void ParseBodyFieldParamsL(TBool aStoreAsDisposition);
	
	void ParseBodyFieldDispL();
	void ParseBodyFieldLangL();
	void ParseBodyExt1PartL();
	void ParseRemainderMultipartL();
	void ParseRemainderMessageRfc822L();
	void ParseCommonOptionalExtensionsL();
	void ParseBodyExtensionL();
	
	void ParseEnvelopeL();
	void ParseAddressL(CImapEnvelope::TAddress& aAddress);
private:
	/**
	Receives ownership of the root body structure and data, upon successful completion of parsing.
	*/
	CImapFetchResponse& iFetchResponse;

	/**
	Whether root body structure is owned by this object.  If EFalse, then it is owned by iFetchResponse.
	*/
	TBool iBodyStructureOwned;
	
	/**
	A stack of the bodystructures that have not yet completed parsing.
	Together with the parse loop, this enables embedded bodystructures to be parsed without using recursion.
	iBodyStructureStack[0] is the root body structure, and owns the bodystructure while
	iBodyStructureOwned is ETrue.
	Indexes greater than 0 do not own the bodystructure they point to.  
	This is because	each CImapBodyStructure owns it's children, so ultimately
	the root bodystructure owns all the bodystructure in its tree.
	*/
	RPointerArray<CImapBodyStructure> iBodyStructureStack;
	/**
	Points at the top bodystructure in the stack.
	This is the CImapBodyStructure that is currently being parsed.
	*/
	CImapBodyStructure* iBodyStructure; 
	
	/**
	Parses incoming blocks of data into a tree of CImapAtom's
	*/
	CImapAtomParser* iAtomParser;
	/**
	Walks the CImapAtom tree, tracking the current position in the tree, 
	and making it possible to WalkAcross to the next sibling atom, WalkDown to the child atom
	and WalkUp to the parent.
	*/
	CImapAtomWalker* iAtomWalker;
	
	/**
	States used by ProcessBlockL()
	*/
	enum TProcessBlockState 
		{
		/**
		Expecting ProcessBlockL() to be called with a line of data
		*/
		EWaitLine,
		/**
		Expecting ProcessBlockL() to be called with a block of literal data
		*/
		EWaitLiteral,
		/**
		All data has been recieved. The Atom tree is now being parsed.
		Not Expecting ProcessBlockL() to be called again on this object.
		*/
		EParsing
		} iProcessBlockState;
		
	/**
	Log id of the owning session
	*/
	TInt iLogId;
	};

#endif // __CIMAPBODYSTRUCTUREBUILDER_H__