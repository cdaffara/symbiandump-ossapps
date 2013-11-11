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

#include "cimapatomparser.h"
#include "cimapatom.h"
#include "cimapcommand.h"
#include "imappaniccodes.h"

// Initial parser buffer size & granularity
const TInt KIOBufferSize=1280;
const TInt KIOBufferGranularity=256;


CImapAtomParser::CImapAtomParser(TBool aAllowParseAtTopLevel, TInt aLogId)
	: iAllowParseAtTopLevel(aAllowParseAtTopLevel)
	, iBufferSize(KIOBufferSize)
	, iLogId(aLogId)
	{}

CImapAtomParser::~CImapAtomParser()
	{
	// Dispose of buffer
	delete iBuffer;

	// Delete any atom tree that may still exist
	delete iRootAtom;

	// delete the atom array
	iAtomArray.ResetAndDestroy();
	iAtomStack.Reset();
	}

// Create and call non-trivial contructor
CImapAtomParser *CImapAtomParser::NewL(TBool aAllowParseAtTopLevel, TInt aLogId)
	{
	CImapAtomParser* self=new (ELeave) CImapAtomParser(aAllowParseAtTopLevel, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// The non-trivial constructor
void CImapAtomParser::ConstructL()
	{
	// Get initial buffer
	iBuffer=HBufC8::NewL(iBufferSize);
	
	iRootAtom=CImapAtom::NewLC();
	CleanupStack::Pop(iRootAtom);
	
	iAtom=iRootAtom;
	iNextIsChild=ETrue;
	}

// Return the root atom
CImapAtom* CImapAtomParser::RootAtom()
	{
	// Return it
	return(iRootAtom);
	}

/**
@return a descriptor that points at the unparsed portion of the last line to be processed.
This pointer descriptor is only valid while the descriptor it points at is valid
This pointer descriptor will only be set after ProcessLineL() has returned
EFalse to indicate that no more data is expected.
*/
const TPtrC8& CImapAtomParser::UnparsedData()
	{
	return iUnparsedData;
	}
	
/**
Returns the internal data buffer, transferring ownership of the buffer to the caller.
The internal data buffer pointer is set to NULL.
This method should only be called after parsing is complete.
@return the data buffer.
*/
HBufC8*	CImapAtomParser::DetachBuffer()
// Note that the iBuffer pointer can change during parsing (via ReAllocL)
// So we only want to expose iBuffer after parsing.
	{
	// Check for internal programming error
	__ASSERT_DEBUG(iParserState == EStateParseComplete, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomParserInvalidParserState));
	
	HBufC8* detachedBuffer = iBuffer;
	iBuffer = NULL;
	
	return detachedBuffer;
	}

// Deal with the atom stack: this is used when building the atom tree
void CImapAtomParser::PushL(CImapAtom* aAtom)
	{
	iAtomStack.AppendL(aAtom);
	}

CImapAtom* CImapAtomParser::PopL()
	{
	if (iAtomStack.Count() <= 0)
		{
		// Mismatched bracket
		CImapCommand::CorruptDataL(iLogId);
		}

	TInt count = iAtomStack.Count();
	CImapAtom* atom = iAtomStack[count-1];
	iAtomStack.Remove(count-1);
	return(atom);
	}

// Add to the parsed buffer
void CImapAtomParser::BufferAppendL(const TChar aChar)
	{
    ++iAttachmentLength;
	// Check for internal programming error
	__ASSERT_DEBUG(iBuffer != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomParserBufferIsNull));
	
	// Does buffer need extending?
	if (iBuffer->Length()==iBufferSize)
		{
		HBufC8 *oldbuffer=iBuffer;
		const TText8 *oldbufptr=iBuffer->Ptr();

		// Extend by granularity amount
		iBufferSize+=KIOBufferGranularity;
		iBuffer=iBuffer->ReAllocL(iBufferSize);

		// Buffer moved?
		if (iBuffer!=oldbuffer)
			{
			// Fixup buffer tree pointers
			iRootAtom->FixupL(iBuffer,oldbufptr);
			}
		}

	// Append the data
	iBuffer->Des().Append(aChar);
	}
	
// Add the last atom appended to the buffer to the tree.
void CImapAtomParser::AddAtomL()
	{
	// Check for internal programming error
	__ASSERT_DEBUG(iBuffer != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomParserBufferIsNull));
	
	TPtrC8 bufptr(iBuffer->Ptr()+iAtomStart, iBuffer->Length()-iAtomStart);

	// Make a new current atom
	CImapAtom* newAtom=CImapAtom::NewLC();
	iAtomArray.AppendL(newAtom);
	CleanupStack::Pop(newAtom);
	
	// Set pointers in it
	newAtom->Set(bufptr, iParserQuoted);

	// Add it as a child/sibling to the current atom
	if (iNextIsChild)
		{
		iAtom->AddChild(newAtom);
		}
	else
		{
		iAtom->AddNext(newAtom);
		}

	// The next item should be a sibling
	iNextIsChild=EFalse;

	// Make new current
	iAtom=newAtom;
	}

/**
Receives a line of data.
If parsing completes before the end of the line, then aLine is set to point 
at the octet immediately after the last parsed octet
@return ETrue if more data is expected.
*/
TBool CImapAtomParser::ProcessLineL(const TDesC8& aLine)
	{
	// Check for internal programming errors
	__ASSERT_DEBUG(iParserState == EStateAtomWait, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomParserInvalidParserState));
	__ASSERT_DEBUG(iBuffer != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomParserBufferIsNull));
	
	// Process the buffer
	TChar octet;

	TInt lineLength = aLine.Length();
	
	TBool bReachedTopLevel = EFalse;

	TInt pos; // used after the loop
	for(pos=0; pos<lineLength; ++pos)
		{
		// Octet to process
		octet=aLine[pos];

		switch(iParserState)
			{
		case EStateAtomWait:
			switch(octet)
				{
			case '(':
				{
				// Make a new current atom
				CImapAtom* newAtom=CImapAtom::NewLC();				
				iAtomArray.AppendL(newAtom);
				CleanupStack::Pop(newAtom);
				
				// Add it as a sibling to the current atom
				if (iNextIsChild)
					{
					iAtom->AddChild(newAtom);
					}
				else
					{
					iAtom->AddNext(newAtom);	
					}

				// The next item should be a child
				iNextIsChild=ETrue;

				// Push current atom onto atom stack, make new current
				iAtom=newAtom;
				PushL(iAtom);

				// Store the open bracket in the buffer, so we can tell what it is
				BufferAppendL(octet);
				iAtom->Set(iBuffer->Right(1), EFalse);

				break;
				}

			case ')':
				// End of this nesting level: pop last atom off stack and
				// make it the new current one
				iAtom=PopL();
				if (iAtomStack.Count() == 0)
					{
					bReachedTopLevel = ETrue;
					}

				// Any new atoms will be siblings, not children
				iNextIsChild=EFalse;

				break;

			case '{':
				// Start of a literal length
				iLiteralLength=0;
				iParserQuoted=EFalse;
				iParserState=EStateLiteralLength;
				break;

			case ' ':
			case '\r':
				// Whitespace. Ignore! This state only happens with whitespace
				// after a close )] or a endquote "
				// Note that the CRLF will already have been stripped out by CImapSession
				// so we are treating a CR on its own as whitespace
				break;
			case '\"':
				// Quotes: we don't keep them, so the atom starts at the next
				// character.
				iAtomStart=iBuffer->Length();
				iParserState=EStateInAtom;
				iParserQuoted=ETrue;
				iGotEscape=EFalse;
				break;

			default:
				// Start new atom in buffer
				iAtomStart=iBuffer->Length();
				BufferAppendL(octet);
				iParserState=EStateInAtom;
				iParserQuoted=EFalse;
				break;
				}
			break;

		case EStateInAtom:
			if (iParserQuoted)
				{
				// Look for another quote
				if (octet=='\"')
					{
					// Just had an escape character?
					if (iGotEscape)
						{
						// Add the character
						BufferAppendL(octet);
						iGotEscape=EFalse;
						}
					else
						{
						// It's the terminator: Add the atom, minus the quotes
						AddAtomL();
						iParserState=EStateAtomWait;
						}
					}
				else
					{
					// Escape character?
					if (!iGotEscape && octet=='\\')
						{
						// Got one
						iGotEscape=ETrue;
						}
					else
						{
						// Add to buffer
						BufferAppendL(octet);
						iGotEscape=EFalse;
						}
					}
				}
			else
				{
				switch(octet)
					{
					case ' ':
					case '\r':
						// Whitespace: end of atom
						// Note that the CRLF will already have been stripped out by CImapSession
						// so we are treating a CR on its own as whitespace
						{
						// if white space is there in the attched file's name,it will add to the buffer.
						if( iAttachmentLength < iLiteralLength)
						    {
                            BufferAppendL(octet);
                            break;
						    }
						    
						AddAtomL();
					
						// Either go back to looking for an atom, or a LF
						//iParserState=(octet=='\r')?EStateWaitLF:EStateAtomWait;
						iParserState = EStateAtomWait;
						}
						break;
					case '(':
						{
						// Add this atom
						AddAtomL();

						// Make a new current atom
						CImapAtom* newAtom=CImapAtom::NewLC();
						iAtomArray.AppendL(newAtom);
						CleanupStack::Pop(newAtom);

						// Add it as a sibling to the current atom
						if (iNextIsChild)
							{
							iAtom->AddChild(newAtom);
							}
						else
							{
							iAtom->AddNext(newAtom);
							}						

						// The next item should be a child
						iNextIsChild=ETrue;

						// Push current atom onto atom stack, make new current
						iAtom=newAtom;
						PushL(iAtom);

						// Store the open bracket in the buffer, so we can tell what it is
						BufferAppendL(octet);
						iAtom->Set(iBuffer->Right(1), EFalse);

						iParserState=EStateAtomWait;
						}
						break;
					case ')':
						{
						// Although these bytes usually indicate the end of an atom,
						// they can also legitimately appear in a text field.
						// If this is the end of an atom, then it must be a child or
						// sibling atom in which case there will be an entry on the atom
						// stack. If there is no entry on the atom stack, then this must
						// be a text field so just add the octet to the buffer.
						if (iAtomStack.Count() > 0)
							{
							// Add this atom
							AddAtomL();

							// End of this nesting level: pop last atom off stack and
							// make it the new current one
							iAtom=PopL();
							if (iAtomStack.Count() == 0)
								{
								bReachedTopLevel = ETrue;
								}

							// Any new atoms will be siblings, not children
							iNextIsChild=EFalse;

							iParserState=EStateAtomWait;
							}
						else
							{
							BufferAppendL(octet);
							}
						}
						break;
					default:
						{
						// Add to buffer
						BufferAppendL(octet);
						}
						break;
					}
				}
			break;
		case EStateLiteralLength:
			// Digit?
			if (octet.IsDigit())
				{
				// Add it to the total
				iLiteralLength=(iLiteralLength*10)+(octet-(TChar)'0');
				if (iLiteralLength <0)
					{
					User::Leave(KErrCorrupt);
					}					
				}
			else if (octet=='}')
				{
				// Need to skip CR, LF
				iLiteralSkip=2;
				iParserState=EStateLiteralSkip;

				// Add the atom (with the length we know, but no data) to the
				// structure now, so that the partial structure can be parsed.
				iAtomStart=iBuffer->Length();
				}
			break;
		case EStateLiteralSkip:
			// Skipping...
			if (--iLiteralSkip==0)
				{
                    iAttachmentLength = 0;
				// Is literal 0 bytes long?
				if (iLiteralLength==0)
					{
					// Nothing to follow
					iParserState=EStateAtomWait;
					}
				else
					{
					// In some cases {nnn} would be present in the middle of FETCH header info ALSO  
					// so need to process the array further
					// Eg: *43 FETCH (FLAGS (\Seen $LuukkuClean $NotJunk JunkRecorded) UID 6285 BODYSTRUCTURE (("TEXT" "PLAIN" ("CHARSET" "ISO-8859-1") NIL NIL "7BIT" 4 0 NIL NIL NIL)("APPLICATION" "PDF" ("NAME" {25}
					// Kopio_paperikirjeestä.pdf) NIL NIL "BASE64" 187346 NIL ("ATTACHMENT" ("FILENAME" {25} 
					// Kopio_paperikirjeestä.pdf)) NIL) "MIXED" ("BOUNDARY" "----=_Part_7226_1401000605.1221158570821") NIL NIL) BODY[HEADER.FIELDS (Received Date Subject From Priority X-MSMail-Priority X-Priority Importance)] {977}
					
					// Checking for pos>=2, so that array is not out of bounds
					if ((pos>=2) && (aLine[pos-2] != ']'))
						{
						 iParserState=EStateAtomWait;
						}
					else 
						{ 
						iParserState=EStateLiteralFetch;
						// Add the atom (with the length we know, but no data) to the
						// structure now, so that the partial structure can be parsed.
						iAtomStart=iBuffer->Length();
						AddAtomL();
						}								
					}
				}
			break;
		case EStateLiteralFetch:
			// Fetching
				{
				// Internal programming error:
				// This state should only be handled in ProcessLiteralBlockL()
				__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomParserInvalidParserState));
				}
			break;
			} // switch(iParserState)
			
		if (bReachedTopLevel && !iAllowParseAtTopLevel)
			{
			break; // from for loop
			}
		}// for(...pos...)

	// Exited early.  Record the un-parsed data
	if (bReachedTopLevel)
		{
		// skip the close-bracket that caused us to reach the top
		++pos;
		// and record anything that remains
		if (pos < lineLength)
			{
			iUnparsedData.Set(aLine.Right(lineLength-pos));
			}		
		}

	// The whole buffer has been read now
	if (iParserState == EStateInAtom)
		{
		AddAtomL();
		iParserState = EStateAtomWait;
		}
	else if (iParserState == EStateLiteralLength)
		{
		// the line contained a '{' but did not have a matching  '}'
		CImapCommand::CorruptDataL(iLogId);
		}
	
	// Check for internal programming error
	__ASSERT_DEBUG(iParserState == EStateAtomWait || iParserState == EStateLiteralFetch, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomParserInvalidParserState));
		
	TBool bWantMoreData = ETrue;
	
	if (iParserState == EStateAtomWait)
		{
		bWantMoreData = EFalse;
		iParserState = EStateParseComplete;
		}	
	
	return bWantMoreData;
	}

void CImapAtomParser::ProcessLiteralBlockL(const TDesC8& aLiteralBlock)
	{
	// Check for internal programming errors
	__ASSERT_DEBUG(iParserState == EStateLiteralFetch, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomParserInvalidParserState));
	__ASSERT_DEBUG(aLiteralBlock.Length() == iLiteralLength, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomParserLiteralBlockLengthMismatch));
	__ASSERT_DEBUG(iBuffer != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EAtomParserBufferIsNull));

	// BufferAppendL() appends a character, whereas this appends a whole string
	if (iBuffer->Length() + iLiteralLength > iBufferSize)
		{
		HBufC8 *oldbuffer=iBuffer;
		const TText8 *oldbufptr=iBuffer->Ptr();
	
		// Extend by extra amount + round up by KIOBufferGranularity
		iBufferSize += iLiteralLength;
		iBufferSize += (KIOBufferGranularity - (iBufferSize % KIOBufferGranularity));
		iBuffer=iBuffer->ReAllocL(iBufferSize);

		// Buffer moved?
		if (iBuffer!=oldbuffer)
			{
			// Fixup buffer tree pointers
			iRootAtom->FixupL(iBuffer,oldbufptr);
			}
		}
		
	iBuffer->Des().Append(aLiteralBlock);
	AddAtomL();

	// literal is always followed by a line, so we need to be waiting for the next atom
	iParserState=EStateAtomWait;
	}

