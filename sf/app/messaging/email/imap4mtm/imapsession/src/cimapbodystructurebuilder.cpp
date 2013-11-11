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

#include "cimapbodystructurebuilder.h"

#include "cimapatom.h"
#include "cimapatomwalker.h"
#include "cimapatomparser.h"
#include "cimapbodystructure.h"
#include "cimapfetchresponse.h"
#include "cimapsessionconsts.h"
#include "cimapcommand.h"
#include "imappaniccodes.h"


CImapBodyStructureBuilder* CImapBodyStructureBuilder::NewL(CImapFetchResponse& aFetchResponse, TInt aLogId)
// static method
	{
	CImapBodyStructureBuilder* self = new(ELeave)CImapBodyStructureBuilder(aFetchResponse, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CImapBodyStructureBuilder::CImapBodyStructureBuilder(CImapFetchResponse& aFetchResponse, TInt aLogId)
	: iFetchResponse(aFetchResponse)
	, iBodyStructureOwned(ETrue)
	, iProcessBlockState(EWaitLine)
	, iLogId(aLogId)
	{}
	
void CImapBodyStructureBuilder::ConstructL()
	{
	iAtomParser = CImapAtomParser::NewL(EFalse, iLogId);
	iAtomWalker = CImapAtomWalker::NewL(iLogId);
	}
	
CImapBodyStructureBuilder::~CImapBodyStructureBuilder()
	{
	delete iAtomWalker;
	delete iAtomParser;
	
	// NOTE
	//
	// CImapBodyStructure is a tree data strucutre, where any CImapBodyStructure owns and 
	// is responssible for destroying its children.
	//
	// iBodyStructureStack[0] is the root bodystructure 
	// ownership of the which is usually passed to CImapFetchResponse object before we get here
	// So destroying iBodyStructureStack[0] will cause all its children to be destroyed too.
	//
	// With the exception of the root bodystructure, iBodyStructureStack does not own any 
	// of the objects it points to, and MUST NOT destroy them.
	
	if (iBodyStructureOwned)
		{
		if (iBodyStructureStack.Count() > 0)
			{
			// delete the root bodystructure.
			delete iBodyStructureStack[0];
			}
		}
	iBodyStructureStack.Close(); // And DO NOT destroy the data that is pointed to.
	}

/**
Parses a block of incoming data from the session.
ProcessBlockL() should be called repeatedly with more data until it returns EFalse to 
indicate that enough data has been received.
This method parses the incoming data into an atom tree as the data is received.
When the last block of data is received, the method will then parse the complete atom tree,
populating iFetchResponse with a fully initialised CImapBodyStructure tree.
@param aData either a line or literal block of data.
@return Whether ProcessBlockL() expects to be called again with the next block of data from the session.
*/
TBool CImapBodyStructureBuilder::ProcessBlockL(const TDesC8& aData)
	{
	TBool wantMore = ETrue;
	switch (iProcessBlockState)
		{
		case EWaitLine:
			{
			wantMore = iAtomParser->ProcessLineL(aData);
			
			if (!wantMore)
				{				
				iAtomWalker->SetRootL(iAtomParser->RootAtom());
				
				// Get to the first "("
				__ASSERT_ALWAYS(iAtomWalker->CurrentDes(EFalse).Length()==0, CImapCommand::CorruptDataL(iLogId));
				__ASSERT_ALWAYS(iAtomWalker->PeekAcross() == NULL, CImapCommand::CorruptDataL(iLogId));
					
				iAtomWalker->WalkDownL();
				__ASSERT_ALWAYS(iAtomWalker->CurrentMatch(KImapTxtOpenBracket()), CImapCommand::CorruptDataL(iLogId));
				
				iProcessBlockState = EParsing;
				ParseLoopL();
				
				TransferBufferOwnershipToFetchResponseL();
				}
			else
				{
				iProcessBlockState = EWaitLiteral;
				}
			}
			break;
		case EWaitLiteral:
			{
			iAtomParser->ProcessLiteralBlockL(aData);
			iProcessBlockState = EWaitLine;
			}
			break;
		default:
			{
			// This is an internal programming error.
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderInvalidProcessBlockState));
			wantMore = EFalse;
			}
			break;
		}
		
	return wantMore;
	}

/**
Assigns the root bodystructure object, and its associated data to iFetchResponse.
iFetchResponse takes ownership of the bodystructure and its data.
*/
void CImapBodyStructureBuilder::TransferBufferOwnershipToFetchResponseL()
	{
	// Check for internal programming errors.
	__ASSERT_DEBUG(iBodyStructureOwned, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderRootNotOwned));
	__ASSERT_DEBUG(iBodyStructureStack.Count() == 1, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderExpectedRootAtomOnlyOnStack));
	__ASSERT_DEBUG(iBodyStructureStack[0] == iBodyStructure, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderCurrentAtomIsNotRoot));
	
	// Prepare to transfer ownership of bodyStructureData from iAtomParser to iFetchResponse
	// bodyStructureData will no longer be owned by iAtomParser
	HBufC8* bodyStructureData = iAtomParser->DetachBuffer();
	
	// Transfer ownership of iBodyStructure and bodyStructureData to iFetchResponse
	iFetchResponse.SetBodyStructure(iBodyStructure, bodyStructureData);
	
	// iBodyStructure is no longer owned by "this" CImapBodyStructureBuilder object.
	iBodyStructureOwned = EFalse;
	}

/**
Returns any data that was not parsed by ParseBlockL()
 - i.e any data that follows the top level BODYSTRUCTURE.
This will be a null string until ParseBlockL() has returned EFalse
to indicate that it has finished parsing.
When non-null, the returned pointer descriptor points into a section of the 
aData descriptor that was passed into ParseBlockL().  Consequently, it is
only valid while the aData descriptor it points into is valid.
@return a descriptor pointing to unparsed data.
*/
TPtrC8 CImapBodyStructureBuilder::UnparsedData()
	{
	return iAtomParser->UnparsedData();
	}	
	
/**
The main loop for parsing the bodystructure.
The loop uses a state machine and bodystructure stack in order to handle
embedded bodystructures without needing to use recursion.
*/
void CImapBodyStructureBuilder::ParseLoopL()
	{
	// Check for internal programming error
	__ASSERT_DEBUG(iBodyStructureStack.Count() == 0, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderStackNotEmpty));
	
	TParseStep parseStep = EParseNewBodyStructure;	
	
	while (parseStep != EParseComplete)
		{
		switch (parseStep)
			{
			case EParseNewBodyStructure:
				{
				// Either at the start of the root body structure,
				// Or at the start of an embedded body structure.
				PushNewBodyStructureL();
				parseStep = ParseBodyStructureTypeL();
				}
				break;
			case EParseBasic:
				{
				// Found a "basic" bodystructure - i.e. not text, rfc822 or multipart.
				ParseBodyTypeBasicL();
				ParseBodyExt1PartL();
				parseStep = EParseSubStructureComplete;
				}
				break;
			case EParseText:
				{
				// Found a "TEXT" body structure
				ParseBodyTypeTextL();
				ParseBodyExt1PartL();
				parseStep = EParseSubStructureComplete;
				}
				break;
			case EParseBodyTypeMessageRfc822:
				{
				// Found a "MESSAGE/RFC822" body structure.
				// This contains an embedded bodystructure, so parse up to the structure,
				// and then allow the loop to parse the embedded structure.
				TRAPD(err, ParseBodyTypeMessageRfc822L());
				if(err == KErrImapCorrupt)
				    {
				parseStep =EParseRemainderMessageRfc822;
				    }
				else
				    {
                    parseStep = EParseNewBodyStructure;
				    }
				
				}
				break;
			case EParseRemainderMessageRfc822:
				{
				// Just finished parsing the embedded bodystructure of a "MESSAGE/RFC822".
				// Complete parsing the parent MESSAGE/RFC822 structure here.
				ParseRemainderMessageRfc822L();
				ParseBodyExt1PartL();
				parseStep = EParseSubStructureComplete;
				}
				break;
			case EParseRemainderMultipart:
				{
				// Just finished parsing the final embedded bodystructure of a MULTIPART structure.
				// Complete parsing the parent MULTIPART structure here
				ParseRemainderMultipartL();
				parseStep = EParseSubStructureComplete;
				}
				break;
			case EParseSubStructureComplete:
				{
				// Just finished parsing a bodystructure.
				// If it is the root bodystructure then we are complete.
				// Otherwise, let ParseSubStructureCompleteL() will find out whether it is
				// * embedded in a MESSAGE/RFC822 structure - requiring the remainder to be parsed next: EParseRemainderMessageRfc822
				// * embedded in a MULTIPART structure in which case
				//   > either there is another embedded structure next: EParseNewBodyStructure
				//   > or this is the last embedded structure, so we need to parse the multipart remainder: EParseRemainderMultipart
					
				if (PopBodyStructureL())
					{
					// we were actually in the root, so we are fully complete.
					parseStep = EParseComplete;
					}
				else
					{
					// we were in a genuine substructure, so need to walk up and decide what to do.
					parseStep = ParseSubStructureCompleteL();
					}				
				}
				break;
			default:
				{
				// This is an internal programming error.
				__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderInvalidParseStep));
				User::Leave(KErrGeneral); // avoid an infinite loop.
				}
			}
		}
	}

/**
When the parse loop comes across a new root or embedded bodystructure, it will use this method to...
  > Create a new CImapBodyStructure object to represent the bodystructure
  > Push the object onto the stack and make it "current"
  > Associate an embedded bodystructure with its parent.

*/
void CImapBodyStructureBuilder::PushNewBodyStructureL()
	{
	// Going to create a new body structure.
	// Need to be sure that something (either the stack root or its tree)
	// is going to own and ultimatelty destroy it.
	__ASSERT_DEBUG(iBodyStructureOwned, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderRootNotOwned));
	
	CImapBodyStructure* bodyStructure = CImapBodyStructure::NewL();
	CleanupStack::PushL(bodyStructure);
	
	// root bodystructure is iBodyStructureStack[0]
	if (iBodyStructure == NULL)
		{
		// Check for internal programming error
		__ASSERT_DEBUG(iBodyStructureStack.Count() == 0, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderStackNotEmpty));
		
		iBodyStructureStack.AppendL(bodyStructure);
		
		// ownership is now transferred to the root bodystructure tree
		CleanupStack::Pop(bodyStructure);
		}
	else
		{
		// Check for internal programming error
		__ASSERT_DEBUG(iBodyStructureStack.Count() > 0, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderStackIsEmpty));
		
		iBodyStructure->AppendEmbeddedBodyStructureL(*bodyStructure);
										
		// ownership is now transferred to the root bodystructure tree
		CleanupStack::Pop(bodyStructure);
		
		iBodyStructureStack.AppendL(bodyStructure);
		}
	
	// This is now the bodystructure that we are parsing.
	iBodyStructure = bodyStructure;
	}

/**
Pops a bodystructure from the stack - except the root, which will not be popped.
@return whether we were in the root already.
*/	
TBool CImapBodyStructureBuilder::PopBodyStructureL()
	{
	TBool bRootAlready = EFalse;
	TInt stackCount = iBodyStructureStack.Count();

	if (stackCount > 1)
		{
		// Pop the bodystructure stack
		--stackCount;
		CImapBodyStructure* poppedBs = iBodyStructureStack[stackCount];
		iBodyStructureStack.Remove(stackCount); // No need to destroy the bodystructure, as it is now owned by iBodyStructureStack[stackCount-1]
		
		iBodyStructure = iBodyStructureStack[stackCount-1];
		}
	else
		{
		// Check for internal programming error: Not expecting a stack count of 0 or less
		__ASSERT_DEBUG(stackCount == 1, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderExpectedRootAtomOnlyOnStack));
		bRootAlready = ETrue;
		}
		
	return bRootAlready;
	}

/**
Found the closing bracket of our substructure.
Pop the stack, walk up and decide what to do next.
If we've reached the root level, then parsing is complete.
If our parent is a multipart, then check to see if we have a sibling.  
	If not then we have come to the end of our parent's structure too - handle this in a separate loop.
If our parent is a Rfc822, then we need to parse the remainder of the Rfc822 structure.
	If not then we have come to the end of our parent's structure too - handle this in a separate loop.
@return 
*/
CImapBodyStructureBuilder::TParseStep CImapBodyStructureBuilder::ParseSubStructureCompleteL()
	{
	TParseStep nextStep = EParseComplete;
	
	iAtomWalker->WalkUpL();
	
	if (iBodyStructure->BodyStructureType() == CImapBodyStructure::ETypeMultipart)
		{
		// Expecting either an open bracket for another bodystructure
		// or the subtype. 
		CImapAtom* peekAcross = iAtomWalker->PeekAcross(); // peekAcross does not need to be destroyed as no ownership is transferred.
		if (peekAcross == NULL)
			{
			// But not expecting "nothing".
			CImapCommand::CorruptDataL(iLogId);
			}
		
		if (peekAcross->Match(KImapTxtOpenBracket()))
			{
			// position the atom walker on the open bracket, ready for ParseBodyStructureTypeL
			// to walk down into it.
			iAtomWalker->WalkAcrossL(ETrue);
			nextStep = EParseNewBodyStructure;
			}
		else
			{
			// stay where we are, so that ParseRemainderMultipartL()
			// can walk accross to the subtype, as any other Parse method would do
			nextStep = EParseRemainderMultipart;
			}
		}
	else if (iBodyStructure->BodyStructureType() == CImapBodyStructure::ETypeMessageRfc822)
		{
		// stay where we are, so that ParseRemainderMessageRfc822L()
		// can walk accross to the body-fld-lines, as any other Parse method would do
		nextStep = EParseRemainderMessageRfc822;
		}
	else
		{
		// No other bodystruct type has substructures.
		// So getting here is an internal programming error.
		__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EBodyStructureBuilderInvalidBodyStructureType));
		User::Leave(KErrGeneral);
		}
			
	return nextStep;
	}

/**
body = "(" (body-type-1part / body-type-mpart) ")"

This method expects iAtomWalker to be positioned at the opening bracket of a body structure.
The method works out the type of the bodystructure, and returns the appropriate next parse 
step to the parse loop.
For multipart structures, iAtomWalker is left positioned at the opening bracket of the embedded structure.
Foa all other structures, iAtomWalker is left positioned at the subtype field.

@return The next step for the parse loop to take.  This is one of
  > EParseNewBodyStructure for MULTIPART structures
  > EParseText for TEXT structures
  > EParseBodyTypeMessageRfc822 for MESSAGE/RFC822
  > EParseBasic for all other structures.
*/
CImapBodyStructureBuilder::TParseStep CImapBodyStructureBuilder::ParseBodyStructureTypeL()
	{
	// Start at the opening bracket
	__ASSERT_ALWAYS(iAtomWalker->CurrentMatch(KImapTxtOpenBracket()), CImapCommand::CorruptDataL(iLogId));
	iAtomWalker->WalkDownL();
		
	// What kind of body type does this represent?	
	// Assume Basic, unless we find otherwise.
	TParseStep nextStep = EParseBasic;
	
	// Is it body-type-mpart? - check for opening bracket
	// body-type-mpart = 1*body SP media-subtype [SP body-ext-mpart]
	if (iAtomWalker->CurrentMatch(KImapTxtOpenBracket()))
		{
		// According to section 6.4.5 of RFC3501, "MULTIPART" is the correct Type string for
		// multipart messages.  This is an implicit value not directly available from the
		// bodystructure input string.  So we point the bodystructure object at a constant string instead.
		iBodyStructure->SetType(KImapTxtMultipart());
		
		iBodyStructure->SetBodyStructureType(CImapBodyStructure::ETypeMultipart);
		nextStep = EParseNewBodyStructure;
		}
	else
		{
		// body-type-1part = (body-type-basic / body-type-msg / body-type-text) [SP body-ext-1part]
		iBodyStructure->SetType(iAtomWalker->CurrentDes(EFalse)); // media-basic and variants is a string, not an nstring
		__ASSERT_ALWAYS(iAtomWalker->PeekDown() == NULL, CImapCommand::CorruptDataL(iLogId));
		
		iAtomWalker->WalkAcrossL(ETrue);
		iBodyStructure->SetSubType(iAtomWalker->CurrentDes(EFalse)); // media-subtype and variants is a string, not an nstring
		
		if (iBodyStructure->Type().CompareF(KImapTxtText())==0)
		// body-type-text = media-text SP body-fields SP body-fld-lines
		// 
		// media-text = DQUOTE "TEXT" DQUOTE SP media-subtype
		// media-subtype = string
			{
			// we have media-text...
			// ... so this is body-type-text
			iBodyStructure->SetBodyStructureType(CImapBodyStructure::ETypeText);
			nextStep = EParseText;
			}
		else if (iBodyStructure->Type().CompareF(KImapTxtMessage())==0 && iBodyStructure->SubType().CompareF(KImapTxtRfc822())==0)
		// body-type-msg = media-message SP body-fields SP envelope SP body SP body-fld-lines
		// 
		// media-message = DQUOTE "MESSAGE" DQUOTE SP DQUOTE "RFC822" DQUOTE
			{
			// we have media-message...
			// ... so this is body-type-msg
			iBodyStructure->SetBodyStructureType(CImapBodyStructure::ETypeMessageRfc822);
			nextStep = EParseBodyTypeMessageRfc822;
			}
		else
			{
			// Not multipart, text or rfc822, so must be basic.
			iBodyStructure->SetBodyStructureType(CImapBodyStructure::ETypeBasic);
			}
		}
		
	return nextStep;	
	}
	
/**
body-type-basic = media-basic SP body-fields

This method expects media-basict to have been parsed already, as part of ParseBodyStructureTypeL()
It expects the atom walker to be positioned at the last atom of media-basic
*/
void CImapBodyStructureBuilder::ParseBodyTypeBasicL()
	{
	// body-fields
	ParseBodyFieldsL();
	}
	
/**
body-type-text = media-text SP body-fields SP body-fld-lines

This method expects media-text to have been parsed already, as part of ParseBodyStructureTypeL()
It expects the atom walker to be positioned at the last atom of media-text
*/
void CImapBodyStructureBuilder::ParseBodyTypeTextL()
	{
	// body-fields
	ParseBodyFieldsL();
	
	// body-fld-lines = number
	iAtomWalker->WalkAcrossL(ETrue);
	iBodyStructure->SetBodyLines(iAtomWalker->CurrentDes(EFalse));
	}
	
/**
body-type-msg = media-message SP body-fields SP envelope SP body SP body-fld-lines

This method parses up to and including envelope.
It then returns, allowing the parse loop to parse the nested "body" that is next.
Upon completion of the nested "body", ParseRemainderMessageRfc822L() will be called to finish parsing
the message body type

This method expects media-message to have been parsed already, as part of ParseBodyStructureTypeL()
It expects the atom walker to be positioned at the last atom of media-text
*/
void CImapBodyStructureBuilder::ParseBodyTypeMessageRfc822L()
	{
	// body-fields SP SP body SP body-fld-lines
	ParseBodyFieldsL();
    TRAPD(err, ParseEnvelopeL());
   
    // Expect a body substructure next.
    // Position iAtomWalker at the opening bracket, ready for ParseBodyStructureTypeL
    iAtomWalker->WalkAcrossL(ETrue);
    if(err == KErrImapCorrupt)
        CImapCommand::CorruptDataL(iLogId);
	}

/**
body-type-msg = media-message SP body-fields SP envelope SP body SP body-fld-lines

ParseBodyTypeMessageRfc822L processes up to and including body-fields.
The ParseLoop processes the nested "body"
This method processes the remainder - i.e. body-fld-lines
It expects the atom walker to be positioned at the field just prior to body-fld-lines
*/
void CImapBodyStructureBuilder::ParseRemainderMessageRfc822L()
	{
	// body-fld-lines = number
	iAtomWalker->WalkAcrossL(ETrue);
	iBodyStructure->SetBodyLines(iAtomWalker->CurrentDes(EFalse));
	}

/**
body-fields = body-fld-param SP body-fld-id SP body-fld-desc SP body-fld-enc SP body-fld-octets	

This method expects iAtomWalker to be positioned just prior to body-fld-param
*/
void CImapBodyStructureBuilder::ParseBodyFieldsL()
	{
	ParseBodyFieldParamsL(EFalse);
	
	// body-fld-id = nstring
	iAtomWalker->WalkAcrossL(EFalse);
	iBodyStructure->SetBodyId(iAtomWalker->CurrentDes(ETrue)); 
	
	// body-fld-desc = nstring
	iAtomWalker->WalkAcrossL(EFalse);
	iBodyStructure->SetBodyDescription(iAtomWalker->CurrentDes(ETrue));
	
	// body-fld-enc = (DQUOTE ("7BIT" / "8BIT" / "BINARY" / "BASE64"/ "QUOTED-PRINTABLE") DQUOTE) / string
	// i.e. it's a string that might be in quotes
	iAtomWalker->WalkAcrossL(EFalse);
	iBodyStructure->SetBodyEncoding(iAtomWalker->CurrentDes(EFalse));
	
	// body-fld-octets = number
	iAtomWalker->WalkAcrossL(EFalse);
	iBodyStructure->SetBodySizeOctets(iAtomWalker->CurrentDes(EFalse));
	}
	
/**
body-fld-param = "(" string SP string *(SP string SP string) ")" / nil

This method expects iAtomWalker to be positioned just prior to the "(" or "NIL" atom.
*/
void CImapBodyStructureBuilder::ParseBodyFieldParamsL(TBool aStoreAsDisposition)

	{
	if (iAtomWalker->WalkAcrossToNilOrOpenL() == CImapAtomWalker::EAtomNil)
		{
		// there are no params
		return;
		}
	
	// Consume the bracket
	iAtomWalker->WalkDownL();
	
	do 
		{
		CImapBodyStructure::TAttributeValuePair pair;
		pair.iAttribute.Set(iAtomWalker->CurrentDes(EFalse));
		
		iAtomWalker->WalkAcrossL(ETrue);
		pair.iValue.Set(iAtomWalker->CurrentDes(EFalse));
		
		if (aStoreAsDisposition)
			{
			iBodyStructure->AppendExtDispositionParameterListL(pair);	
			}
		else
			{
			iBodyStructure->AppendParameterListL(pair);
			}		
		} 
		while (iAtomWalker->WalkAcrossL(EFalse));
		
	iAtomWalker->WalkUpL();
	}

/** 
body-fld-dsp = "(" string SP body-fld-param ")" / nil

This method expects iAtomWalker to be positioned just prior to the "(" or "NIL" atom.
*/
void CImapBodyStructureBuilder::ParseBodyFieldDispL()
	{
	if (iAtomWalker->WalkAcrossToNilOrOpenL() == CImapAtomWalker::EAtomNil)
		{
		// there are no params
		return;
		}
		
	iAtomWalker->WalkDownL();
	
	// string
	iBodyStructure->SetExtDispositionName(iAtomWalker->CurrentDes(EFalse));
	
	// body-fld-param
	ParseBodyFieldParamsL(ETrue);
	
	iAtomWalker->WalkUpL();	
	}
	
/**
body-fld-lang = nstring / "(" string *(SP string) ")"

This method should only be called if body-fld-lang is expected.
The caller should check first, using iAtomWalker->PeekAcross()
*/
void CImapBodyStructureBuilder::ParseBodyFieldLangL()
	{
	// Consume the bracket
	iAtomWalker->WalkAcrossL(ETrue);
	
	if (iAtomWalker->CurrentMatch(KImapTxtOpenBracket()))
		{
		// we have many strings
		// "(" string *(SP string) ")"
		
		iAtomWalker->WalkDownL();
				
		do 
			{
			iBodyStructure->AppendExtLanguageListL(iAtomWalker->CurrentDes(EFalse));
			
			} while (iAtomWalker->WalkAcrossL(EFalse));
		
		iAtomWalker->WalkUpL();
		}
	else 
		{
		// we have a single nstring - only add it if it is non-empty
		const TDesC8& language = iAtomWalker->CurrentDes(ETrue);
		if (language.Length() > 0)
			{
			iBodyStructure->AppendExtLanguageListL(language);	
			}		
		}
	}

/**
body-ext-1part = body-fld-md5 [SP body-fld-dsp [SP body-fld-lang [SP body-fld-loc *(SP body-extension)]]]

This only appears in
body-type-1part = (body-type-basic / body-type-msg / body-type-text) [SP body-ext-1part]
So, this method treats the body-fld-md5 field as optional
*/
void CImapBodyStructureBuilder::ParseBodyExt1PartL()
	{
	// Return as soon as a field is not found.
	if (iAtomWalker->WalkAcrossL(EFalse))
		{
		// body-fld-md5 = nstring
		iBodyStructure->SetExtMD5(iAtomWalker->CurrentDes(ETrue));
		
		// [SP body-fld-dsp [SP body-fld-lang [SP body-fld-loc *(SP body-extension)]]]
		ParseCommonOptionalExtensionsL();
		}
	}

/**
body-type-mpart = 1*body SP media-subtype [SP body-ext-mpart]
This method deals with media-subtype [SP body-ext-mpart]

body-ext-mpart = body-fld-param [SP body-fld-dsp [SP body-fld-lang [SP body-fld-loc *(SP body-extension)]]]

It expects iAtomWalker to be positioned just prior to media-subtype.
*/
void CImapBodyStructureBuilder::ParseRemainderMultipartL()
	{
	iAtomWalker->WalkAcrossL(ETrue);
	
	// media-subtype = string
	iBodyStructure->SetSubType(iAtomWalker->CurrentDes(EFalse));
	
	// The remainder of items are optional.  Return as soon as one is not found.
	
	// body-fld-param
	if (iAtomWalker->PeekAcross())
		{
		ParseBodyFieldParamsL(EFalse);
		
		// [SP body-fld-dsp [SP body-fld-lang [SP body-fld-loc *(SP body-extension)]]]
		ParseCommonOptionalExtensionsL();		
		}
	}
	
/**
body-ext-1part = body-fld-md5   [SP body-fld-dsp [SP body-fld-lang [SP body-fld-loc *(SP body-extension)]]]
body-ext-mpart = body-fld-param [SP body-fld-dsp [SP body-fld-lang [SP body-fld-loc *(SP body-extension)]]]
Apart from the first parameter, body extensions for 1 and multi part are the same.
So this method parses them in one place.
*/
void CImapBodyStructureBuilder::ParseCommonOptionalExtensionsL()
	{
	// Return as soon as a field is not found.
	
	// body-fld-dsp
	if (iAtomWalker->PeekAcross())
		{
		ParseBodyFieldDispL();
		
		// body-fld-lang
		if (iAtomWalker->PeekAcross())
			{
			ParseBodyFieldLangL();
		
			// body-fld-loc = nstring
			if (iAtomWalker->WalkAcrossL(EFalse))
				{
				iBodyStructure->SetExtLocation(iAtomWalker->CurrentDes(ETrue));
				
				// *(SP body-extension)
				if (iAtomWalker->PeekAcross())
					{
					ParseBodyExtensionL();
					}
				}
			}
		}
	}
	
/**
body-extension = nstring / number / "(" body-extension *(SP body-extension) ")"
RFC3501 says:  "Future expansion. Client implementations
				MUST accept body-extension fields. Server
				implementations MUST NOT generate
				body-extension fields except as defined by
				future standard or standards-track
				revisions of this specification.
As body-extension is always at the end of a (sub)bodystructure, it is safe to ignore.
This method provides a placeholder for extracting any body-extension data that we might 
be interested in, in the future.
*/
void CImapBodyStructureBuilder::ParseBodyExtensionL()
	{}

/**
envelope = "(" env-date SP env-subject SP env-from SP env-sender SP env-reply-to SP env-to SP env-cc SP env-bcc SP env-in-reply-to SP env-message-id ")"

This method expects iAtomWalker to be positioned at the opening bracket.
*/
void CImapBodyStructureBuilder::ParseEnvelopeL()
	{
	// Always expect an open bracket here, so allow atom walker to leave if there is one
	iAtomWalker->WalkAcrossL(ETrue);
	iAtomWalker->WalkDownL();
	
	CImapEnvelope& envelope = iBodyStructure->GetRfc822EnvelopeStructureL();
		
	// env-date = nstring
	envelope.SetEnvDate(iAtomWalker->CurrentDes(ETrue));
	
	// env-subject = nstring
	iAtomWalker->WalkAcrossL(ETrue);
	envelope.SetEnvSubject(iAtomWalker->CurrentDes(ETrue));
	
	// This single address structure will be *copied* into various envelope address arrays
	CImapEnvelope::TAddress address; // this will copied into many en
	
	// env-from = "(" 1*address ")" / nil
	if (iAtomWalker->WalkAcrossToNilOrOpenL() == CImapAtomWalker::EAtomOpen)
		{
		iAtomWalker->WalkDownL();
		do
			{
			ParseAddressL(address);
			envelope.AppendEnvFromL(address);
			
			} while(iAtomWalker->WalkAcrossL(EFalse));
		
		iAtomWalker->WalkUpL();
		}
		
	// env-sender = "(" 1*address ")" / nil
	if (iAtomWalker->WalkAcrossToNilOrOpenL() == CImapAtomWalker::EAtomOpen)
		{
		iAtomWalker->WalkDownL();
		do
			{
			ParseAddressL(address);
			envelope.AppendEnvSenderL(address);
			
			} while(iAtomWalker->WalkAcrossL(EFalse));
		
		iAtomWalker->WalkUpL();
		}
	// env-reply-to = "(" 1*address ")" / nil
	if (iAtomWalker->WalkAcrossToNilOrOpenL() == CImapAtomWalker::EAtomOpen)
		{
		iAtomWalker->WalkDownL();
		do
			{
			ParseAddressL(address);
			envelope.AppendEnvReplyToL(address);
			
			} while(iAtomWalker->WalkAcrossL(EFalse));
		
		iAtomWalker->WalkUpL();
		}
		
	// env-to = "(" 1*address ")" / nil
	if (iAtomWalker->WalkAcrossToNilOrOpenL() == CImapAtomWalker::EAtomOpen)
		{
		iAtomWalker->WalkDownL();
		do
			{
			ParseAddressL(address);
			envelope.AppendEnvToL(address);
			
			} while(iAtomWalker->WalkAcrossL(EFalse));
		
		iAtomWalker->WalkUpL();
		}
		
	// env-cc = "(" 1*address ")" / nil
	if (iAtomWalker->WalkAcrossToNilOrOpenL() == CImapAtomWalker::EAtomOpen)
		{
		iAtomWalker->WalkDownL();
		do
			{
			ParseAddressL(address);
			envelope.AppendEnvCcL(address);
			
			} while(iAtomWalker->WalkAcrossL(EFalse));
		
		iAtomWalker->WalkUpL();
		}
		
	// env-bcc = "(" 1*address ")" / nil
	if (iAtomWalker->WalkAcrossToNilOrOpenL() == CImapAtomWalker::EAtomOpen)
		{
		iAtomWalker->WalkDownL();
		do
			{
			ParseAddressL(address);
			envelope.AppendEnvBccL(address);
			
			} while(iAtomWalker->WalkAcrossL(EFalse));
		
		iAtomWalker->WalkUpL();
		}
	
	// env-in-reply-to = nstring
	iAtomWalker->WalkAcrossL(ETrue);
	envelope.SetEnvInReplyTo(iAtomWalker->CurrentDes(ETrue));
	
	// env-message-id = nstring	
	iAtomWalker->WalkAcrossL(ETrue);
	envelope.SetEnvMessageId(iAtomWalker->CurrentDes(ETrue));
	
	iAtomWalker->WalkUpL();
	}

/**
address = "(" addr-name SP addr-adl SP addr-mailbox SP addr-host ")"

This method expects iAtomWalker to be positioned at the opening bracket.
*/
void CImapBodyStructureBuilder::ParseAddressL(CImapEnvelope::TAddress& aAddress)
	{
	// Always expect an open bracket here, so allow atom walker to leave if there is one
	iAtomWalker->WalkDownL();
	
	// addr-name = nstring
	aAddress.SetName(iAtomWalker->CurrentDes(ETrue));
	
	// addr-adl = nstring
	iAtomWalker->WalkAcrossL(ETrue);
	aAddress.SetAdl(iAtomWalker->CurrentDes(ETrue));
	
	// addr-mailbox = nstring
	iAtomWalker->WalkAcrossL(ETrue);
	aAddress.SetMailbox(iAtomWalker->CurrentDes(ETrue));
	
	// addr-host = nstring
	iAtomWalker->WalkAcrossL(ETrue);
	aAddress.SetHost(iAtomWalker->CurrentDes(ETrue));
	
	iAtomWalker->WalkUpL();
	}
