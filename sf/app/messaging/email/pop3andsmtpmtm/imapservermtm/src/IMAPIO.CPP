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
// This deals with connection to the server, sending commands, and parsing the
// returned server data. The data returned is buffered in iBuffer: this is
// simply back-to-back data, with a tree which describes the structure
// containing TPtrC's which identify the data belonging to each atom.
// This class owns both the buffer and the tree, and deals with destroying them
// as necessary. The client can request the base of the tree for it to parse
// when a line has been received.
// 980708 - modified to store flavour of ([ in parent atom
// 980802 - partial read behavior modified (see release.txt)
// 990112 - added cancel debug printing
// 990304 - Tweaks to parsing of escape characters
// 990415 - Now happy with ('s in the middle on non-quoted atoms.
// This only generally happens with non-parsable text
// (eg, after an OK response).
// 
//

#include "imapio.h"
#include "impspan.h"
#include <imsk.h>
#include <imcvtext.h>
#include <msvstore.h>
#include "imapsess.h"

#define IMAPLOG

#ifdef _DEBUG
#define DBG(a) a
#else
#define DBG(a)
#endif

// Initial parser buffer size & granularity
const TInt KIOBufferSize=1280;
const TInt KIOBufferGranularity=256;

// Specifies how long a socket is allowed to be inactive before we close it
// down. This handles the situation where the mail server closes down the
// connection, but we don't receive any indication of that. It has been seen
// when connected using GPRS, and a long telephone call is then made.
const TInt KImapSendInactivityTimeMinutes = 30;
const TInt KImapReceiveInactivityTimeMinutes = 30;

// The CImapAtom class contains a TPtrC which points to the data 'owned' by 
// this atom in the buffer. It also contains next (sibling) and child pointers,
// with which the tree is constructed.
CImapAtom::CImapAtom()
	{
	__DECLARE_NAME(_S("CImapAtom"));
	}

CImapAtom::~CImapAtom()
	{
	}

void CImapAtom::Set(const TDesC8& aAtom)
	{
	// Save this atom in here
	iAtom.Set(aAtom);
	}

void CImapAtom::AddChild(CImapAtom *aNewChild)
	{
	// Set child pointer
	iChild=aNewChild;
	}

void CImapAtom::AddNext(CImapAtom *aNewNext)
	{
	// Set next pointer
	iNext=aNewNext;
	}

CImapAtom *CImapAtom::Child()
	{
	return(iChild);
	}

CImapAtom *CImapAtom::Next()
	{
	return(iNext);
	}

// ToChildL and ToNextL provide clients with a neat way of traversing the tree
// in a direction that *should* exist: if it doesn't, a Leave occurs.
CImapAtom *CImapAtom::ToChildL()
	{
	if (!iChild)
		User::Leave(KErrNotFound);
	return(iChild);
	}

CImapAtom *CImapAtom::ToNextL()
	{
	if (!iNext)
		User::Leave(KErrNotFound);
	return(iNext);
	}

// Makes things look neater: do a CompareF
TBool CImapAtom::Compare(const TDesC8& aVal)
	{
	// Compare and return result
	return(iAtom.CompareF(aVal)==0);
	}

// Compare the right hand side of the atom with the match string.
TBool CImapAtom::CompareTail(const TDesC8& aVal)
	{
	if (aVal.Length() > iAtom.Length())
		return EFalse;
	TPtrC8 ptr = iAtom.Right(aVal.Length());
	return(ptr.CompareF(aVal)==0);
	}

// Makes things look neater: lex a decimal atom to a TUint
TInt CImapAtom::Value(TUint& aVal)
	{
	// Turn it into a value
	TLex8 lex(iAtom);

	return(lex.Val(aVal));
	}

// Makes things look neater: lex a decimal atom to a TInt
TInt CImapAtom::Value(TInt& aVal)
	{
	// Turn it into a value
	TLex8 lex(iAtom);

	return(lex.Val(aVal));
	}

// Makes things look neater: lex a decimal atom to a TInt32
TInt CImapAtom::Value(TInt32& aVal)
	{
	// Turn it into a value
	TLex8 lex(iAtom);

	return(lex.Val(aVal));
	}

// Return descriptor
TPtrC8 CImapAtom::Atom()
	{
	return(iAtom);
	}

// Fixup descriptor pointers
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
		if (currentAtom->Atom().Length()>0)
			{
			// Find offset from start of old buffer
			TInt start=(currentAtom->Atom().Ptr()-aOldBuffer);

 			// Make new descriptor & assign it
			TPtrC8 bufptr(aNewBuffer->Ptr()+start,currentAtom->Atom().Length());
			currentAtom->iAtom.Set(bufptr); // Note that we are setting the real iAtom not the copy returned by Atom()
			}
 
		// Add the first sibling to the stack,
		// subsequent siblings are added when this sibling is visited
		CImapAtom* siblingAtom = currentAtom->Next();
		if (siblingAtom)
			atomStack->AppendL(siblingAtom);

   
		// Add child to the stack
		CImapAtom* childAtom = currentAtom->Child();
		if (childAtom)
			atomStack->AppendL(childAtom);
   		}
   
	CleanupStack::PopAndDestroy(atomStack);
   	}

// The IO processing class
CImapIO::CImapIO(TInt aId) : CMsgActive(1), iBufferSize(KIOBufferSize), iAtomStack(8), iID(aId),iPrimaryTextServerSession(NULL)
	{
	__DECLARE_NAME(_S("CImapIO"));
	}

CImapIO::~CImapIO()
	{
	// Make sure we're cancelled
	Disconnect();

	delete iSession;

	// Dispose of buffer
	delete iBuffer;

	// Delete any atom tree that may still exist
	delete iRootAtom;

	// Get rid of log
	delete iImapLog;
	// delete the atom array
	iAtomArray.ResetAndDestroy();
	}

// Create and call non-trivial contructor
CImapIO *CImapIO::NewLC(TInt aId)
	{
	CImapIO* self=new (ELeave) CImapIO(aId);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CImapIO *CImapIO::NewL(TInt aId)
	{
	CImapIO* self=NewLC(aId);
	CleanupStack::Pop();
	return self;
	}

// The non-trivial constructor
void CImapIO::ConstructL()
	{
	// Get initial buffer
	iBuffer=HBufC8::NewL(iBufferSize);
	
	// We're an active object...
	CActiveScheduler::Add(this);
	}

// Called when an asynchronous child exits with status>=0
void CImapIO::DoRunL()
	{
	DBG((LogText(_L8("CImapIO::DoRunL(state=%d, status=%d)"),iState,iStatus.Int())));

	switch(iState)
		{
	case EIOStateConnectQueued:
		// Connect has finished: OK?
		if (iStatus.Int()==KErrNone)
			{
			// Done! We're connected
			iState=EIOStateConnected;
			}
		else
			{
			// Close connection and report fail
			iSession->Disconnect();
			}
		break;

	case EIOStateReadQueued:
		// Process it
		iState=EIOStateConnected;
		iRXbytes+=iReceive.Length();
		ProcessBufferL();
		break;

	case EIOStateWriteQueued:
		// Sent! Return error code directly
		iState=EIOStateConnected;
		break;

	case EIOStateConnected:
	case EIOStateDisconnected:
	case EIOStateDummyQueued:
		// Never happens, but pleases the compiler
		break;
		}
	}

// Called when an asynchronous child exits with status <KErrNone
void CImapIO::DoComplete(TInt& /*aStatus*/)
	{
	// note this message might not get reported if the socket has closed

	// pass all status codes up as given. KErrNone and KErrEOL (1)
	// will be ignored. All others will cause a disconnect either in
	// CImImap4Session's IdleReadError(), DummyComplete() or DoComplete().
	}	

// Called when parent wants to cancel an operation
void CImapIO::DoCancel()
	{

	DBG((LogText(_L8("CImapIO::DoCancel(state=%d)"),iState)));
	
	if (iSession)
		{
		// Cancel actions
		switch(iState)
			{
		case EIOStateConnectQueued:
			// Cancel session and disconnect
			iSession->Cancel();
			iState=EIOStateDisconnected;
			break;

		case EIOStateReadQueued:
		case EIOStateWriteQueued:
			// Cancel it at the session level: this will return the cancelled error
			// upwards, which should cause the above DoComplete to return
			iSession->Cancel();
			iState=EIOStateConnected;
			break;

		case EIOStateConnected:
		case EIOStateDisconnected:
			// Never happens, but pleases the compiler
			break;
		case EIOStateDummyQueued:
			break;
			}
		}
	else
		iState=EIOStateDisconnected;

	// Get msgactive to finish up
	CMsgActive::DoCancel();

	DBG((LogText(_L8("CImapIO::DoCancel done"))));

	}

// Return the root atom
CImapAtom *CImapIO::RootAtom()
	{
	// Return it
	return(iRootAtom);
	}

// Deal with the atom stack: this is used when building the atom tree
void CImapIO::PushL(CImapAtom *aAtom)
	{
	iAtomStack.AppendL(aAtom);
	}

CImapAtom *CImapIO::PopL()
	{
	// Check it isn't empty
	if (iAtomStack.Count()==0)
		User::Leave(KErrUnderflow);

	TInt count = iAtomStack.Count();
	CImapAtom* atom = iAtomStack[count-1];
	iAtomStack.Delete(count-1);
	return(atom);
	}

// Add to the parsed buffer
void CImapIO::BufferAppendL(const TChar aChar)
	{
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
void CImapIO::AddAtomL()
	{
	// Add it
	AddAtomL(iBuffer->Length()-iAtomStart);
	}

// Add the last atom, given a length
void CImapIO::AddAtomL(const TInt aLength)
	{
	// Note buffer position in an atom
	TPtrC8 bufptr(iBuffer->Ptr()+iAtomStart,aLength);

	// Make a new current atom
	CImapAtom *newAtom=new (ELeave) CImapAtom();

	iAtomArray.AppendL(newAtom);
	// Set pointers in it
	newAtom->Set(bufptr);

	// Add it as a child/sibling to the current atom
	if (iNextIsChild)
		iAtom->AddChild(newAtom);
	else
		iAtom->AddNext(newAtom);

	// The next item should be a sibling
	iNextIsChild=EFalse;

	// Make new current
	iAtom=newAtom;
	}

// Process the received buffer, creating atoms as we go
void CImapIO::ProcessBufferL()
	{
	// Process the buffer
	TChar byte;

	DBG((LogText(_L8("CImapIO::ProcessBuffer(iParserState=%d, Length=%d)"),iParserState,iReceive.Length())));

	for(TInt pos=0;pos<iReceive.Length();pos++)
		{
		// Note that we've processed stuff
		iBytesRead++;

		// Byte to process
		byte=iReceive[pos];

		switch(iParserState)
			{
		case EIOStateAtomWait:
			switch(byte)
				{
			case '(':
			case '[':
			case '<':
				{
				// Make a new current atom
				CImapAtom *newAtom=new (ELeave) CImapAtom();

				iAtomArray.AppendL(newAtom);
				// Add it as a sibling to the current atom
				if (iNextIsChild)
					iAtom->AddChild(newAtom);
				else
					iAtom->AddNext(newAtom);

				// The next item should be a child
				iNextIsChild=ETrue;

				// Push current atom onto atom stack, make new current
				iAtom=newAtom;
				PushL(iAtom);

				// Store the open bracket in the buffer, so we can tell what it is
				TPtrC8 bufptr(iBuffer->Ptr()+iBuffer->Length(),1);
				BufferAppendL(byte);
				iAtom->Set(bufptr);

				break;
				}

			case ')':
			case ']':
				// End of this nesting level: pop last atom off stack and
				// make it the new current one
				iAtom=PopL();

				// Any new atoms will be siblings, not children
				iNextIsChild=EFalse;

				break;

			case '{':
				// Start of a literal length
				iLiteralLength=0;
				iParserState=EIOStateLiteralLength;
				break;

			case ' ':
				// Whitespace. Ignore! This state only happens with whitespace
				// after a close )] or a endquote "
				break;

			case '\r':
				// Newline after a close )] or endquote "
				iParserState=EIOStateWaitLF;
				break;

			case '\"':
				// Quotes: we don't keep them, so the atom starts at the next
				// character.
				iAtomStart=iBuffer->Length();
				iParserState=EIOStateInAtom;
				iParserQuoted=ETrue;
				iGotEscape=EFalse;
				break;

			default:
				// Start new atom in buffer
				iAtomStart=iBuffer->Length();
				BufferAppendL(byte);
				iParserState=EIOStateInAtom;
				iParserQuoted=EFalse;
				break;
				}
			break;

		case EIOStateInAtom:
			if (iParserQuoted)
				{
				// Look for another quote
				if (byte=='\"')
					{
					// Just had an escape character?
					if (iGotEscape)
						{
						// Add the character
						BufferAppendL(byte);
						iGotEscape=EFalse;
						}
					else
						{
						// It's the terminator: Add the atom, minus the quotes
						AddAtomL();
						iParserState=EIOStateAtomWait;
						}
					}
				// fix for INC51597 and DEF053082:if a " has been missed out by the server, this will end the atom at a \r
				else if(!iGotEscape && byte == '\r')
					{
					AddAtomL();
					iParserState = EIOStateWaitLF;
					}
				else
					{
					// Escape character?
					if (!iGotEscape && byte=='\\')
						{
						// Got one
						iGotEscape=ETrue;
						}
					else
						{
						// Add to buffer
						BufferAppendL(byte);
						iGotEscape=EFalse;
						}
					}
				}
			else
				{
				if (byte==' ' || byte=='\r')
					{
					AddAtomL();
				
					// Either go back to looking for an atom, or a LF
					iParserState=(byte=='\r')?EIOStateWaitLF:EIOStateAtomWait;
					}
				else if (byte=='(' || byte=='[')
					{
					// Add this atom
					AddAtomL();

					// Make a new current atom
					CImapAtom *newAtom=new (ELeave) CImapAtom();
					iAtomArray.AppendL(newAtom);

					// Add it as a sibling to the current atom
					if (iNextIsChild)
						iAtom->AddChild(newAtom);
					else
						iAtom->AddNext(newAtom);

					// The next item should be a child
					iNextIsChild=ETrue;

					// Push current atom onto atom stack, make new current
					iAtom=newAtom;
					PushL(iAtom);

					// Store the open bracket in the buffer, so we can tell what it is
					TPtrC8 bufptr(iBuffer->Ptr()+iBuffer->Length(),1);
					BufferAppendL(byte);
					iAtom->Set(bufptr);

					iParserState=EIOStateAtomWait;
					}
				else if (byte==')' || byte==']' || byte == '>')
					{
					// Although these bytes usually indicate the end of an atom,
					// they can also legitimately appear in a text field.
					// If this is the end of an atom, then it must be a child or
					// sibling atom in which case there will be an entry on the atom
					// stack. If there is no entry on the atom stack, then this must
					// be a text field so just add the byte to the buffer.
					if (iAtomStack.Count() > 0)
						{
						// Add this atom
						AddAtomL();

						// End of this nesting level: pop last atom off stack and
						// make it the new current one
						iAtom=PopL();

						// Any new atoms will be siblings, not children
						iNextIsChild=EFalse;

						iParserState=EIOStateAtomWait;
						}
					else
						{
						BufferAppendL(byte);
						}
					}
				else
					{
					// Add to buffer
					BufferAppendL(byte);
					}
				}
			break;

		case EIOStateWaitLF:
			// After LF, this is end of line, finish!
			if (byte=='\n')
				{
				// Remove everything from the buffer, we've finished
				iReceive.Delete(0,pos+1);

				// Reset bytes read count & complete
				iBytesRead=0;

				// Complete with KErrFoundEOL
				DBG((LogText(_L8("CImapIO::ProcessBuffer: Complete in EIOStateWaitLF"))));
				Complete(KErrFoundEOL);
				return;
				}
			break;

		case EIOStateLiteralLength:
			// Digit?
			if (byte.IsDigit())
				{
				// Add it to the total
				iLiteralLength=(iLiteralLength*10)+(byte-(TChar)'0');
				if (iLiteralLength <0)
					User::Leave(KErrCorrupt);
				}
			else if (byte=='}')
				{
				// Need to skip CR, LF
				iLiteralSkip=2;
				iParserState=EIOStateLiteralSkip;

				// Add the atom (with the length we know, but no data) to the
				// structure now, so that the partial structure can be parsed.
				iAtomStart=iBuffer->Length();
				AddAtomL(iLiteralLength);
				}
			break;

		case EIOStateLiteralSkip:
			// Skipping...
			if (--iLiteralSkip==0)
				{
				// Is literal 0 bytes long?
				if (iLiteralLength==0)
					{
					// Nothing to follow
					iParserState=EIOStateAtomWait;
					}
				else
					{
					// Non-empty literal: go into fetch state
					iParserState=EIOStateLiteralFetch;
					}
				}
			break;

		case EIOStateLiteralFetch:
			// Fetching
			
			TInt fetchLength(0);
			if(KReceiveBuffer<iLiteralLength)
				{
				fetchLength = KReceiveBuffer;
				}
			else
				{
				fetchLength = iLiteralLength;
				}
			
			if(fetchLength > iReceive.Length()-pos)
				{
				fetchLength = iReceive.Length()-pos;
				}
// 			need to extend buffer ?
			DBG((LogText(_L8(">>> CImapIO::ProcessBufferL(1):buflen=%d:buffsize=%d:litlen=%d:fetchlen=%d"), iBuffer->Length(),iBufferSize, iLiteralLength,fetchLength)));
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
				DBG((LogText(_L8(">>> CImapIO::ProcessBufferL(2):buflen=%d:buffsize=%d:litlen=%d:fetchlen=%d"), iBuffer->Length(),iBufferSize, iLiteralLength,fetchLength)));
				}
			iBuffer->Des().Append(iReceive.Mid(pos,fetchLength));
			// adjust loop to account for data copy
			pos+=fetchLength-1;
			iLiteralLength-=fetchLength; 			
			DBG((LogText(_L8(">>> CImapIO::ProcessBufferL(3):buflen=%d:buffsize=%d:litlen=%d:fetchlen=%d"), iBuffer->Length(),iBufferSize, iLiteralLength,fetchLength)));
			if (iLiteralLength==0)
				{
				// Atom is already saved (we add literal atoms before they
				// are stored)
				iParserState=EIOStateAtomWait;
				}
			break;
			}
		}
	
	iReceive.Zero();
	// At start of line, or if we're not doing a partial return, we need to
	// queue another read here
	if (iBytesRead==0 || !iReturnPartialLine)
		{
		// We've processed this buffer: queue a read. We only complete (above)
		// when we've had a whole reply, including terminating CRLF.
		DBG((LogText(_L8("CImapIO::ProcessBufferL(): queuing read, iBytesRead=%d, iReturnPartialLine is %d"), iBytesRead, iReturnPartialLine)));
                QueueRead();
		}
	else
		{
		// Have we got 'enough' of the partial line?
		if (iBytesRead<iBytesToRead)
			{
			// Not enough yet: queue another partial read, for the remainder
			iBytesToRead-=iBytesRead;
			QueueRead();
			}
		else
			{
			// Partial line parsed: return success (client will requeue)
			DBG((LogText(_L8("CImapIO::ProcessBuffer: Complete on partial line"))));
			Complete(KErrNone);
			}
		}
	}

void CImapIO::QueueRead()
	{
	// Fill buffer
	iLen=iBytesToRead;
	iSession->ReceiveBinaryData(iStatus,iReceive,iLen);
	iState=EIOStateReadQueued;
	SetActive();
	}

// Close connection
void CImapIO::Disconnect()
	{
	DBG((LogText(_L8("CImapIO::Disconnect(state=%d)"),iState)));

	// Reset state
	iState=EIOStateDisconnected;

	// Delete session to stop timeouts (this will disconnect too)
	delete iSession;
	iSession=NULL;
	}

void CImapIO::SetTLSResponseL()
	{
	DBG((LogText(_L8("CImapIO::SetTLSResponseL"))));

	iSession->SetSSLTLSResponseL(KIMAP_OK);
	}

/**
	@fn				TInt GetIAPValue(TUint32& aIap)
	Intended Usage	:	Gets the value of the currently connecting/connected IAP
	@param			aIap will be value of the currently connecting/connected IAP or KErrNotFound if an error occurs 
	@return			KErrNone if succesful, KErrNotFound is the session does not exist or a system-wide error code.
	
	*/
TInt CImapIO::GetIAPValue(TUint32& aIap)
	{
	return (iSession) ? (iSession->GetIAPValue(aIap)) : (KErrNotFound);
	}

/**
	@fn				TInt GetRConnectionName(TName &aName)
	Intended Usage	:	On return, the unique name of the RConnection.
	@since			9.1
	@return			KErrNone if succesful, or another of the system-wide error codes. 
	*/
TInt CImapIO::GetRConnectionName(TName &aName)
	{
	return (iSession) ? (iSession->GetRConnectionName(aName)) : (KErrNotFound);
	}

// Returns the last socket activity timeout value for the session connection
TInt CImapIO::GetLastSocketActivityTimeout(TUint32& aTimeout)
	{
	return (iSession) ? (iSession->GetLastSocketActivityTimeout(aTimeout)) : (KErrNotFound);
	}

/**
	@fn				TInt GetConnectionStage()
	Intended Usage	:	Gets the stage of the connection process as defined in nifvar.h and csdprog.h
	@since			7.0s
	@return         The current connection stage, KErrNotFound is the session does not exist or a system-wide error code

	*/
TInt CImapIO::GetConnectionStage()
	{
	return (iSession) ? (iSession->GetConnectionStage()) : (KErrNotFound);
	}


// Parent wants to queue a connection
void CImapIO::ConnectL(TRequestStatus& aStatus, const TDesC& aHost, const TUint aPortNum, const CImIAPPreferences& aPrefs, TBool aSSLWrappedSocket)
	{
	// Have to be disconnected to connect...
	__ASSERT_DEBUG(iState==EIOStateDisconnected,gPanic(EConnectWhenConnected));
	if(!(iState==EIOStateDisconnected))
		{
		User::LeaveIfError(KErrInUse);// Connect when connected
		}
	// Queue request
	Queue(aStatus);

	// Delete current session (there shouldn't be one really)
	delete iSession;
	iSession=NULL;

	// Reconstruct it
	iSession=CImTextServerSession::NewL(KImapSendInactivityTimeMinutes, KImapReceiveInactivityTimeMinutes);
	
	// check local textseverssion is active.
	if(iPrimaryTextServerSession)
		{
		// Providing primarysession's textserversession
		// Going to be set on the secondary session
		iSession->SetPrimaryTextServerSession(iPrimaryTextServerSession);	
		}

	// Ask session to connect (it does the resolving bit)
	if(aSSLWrappedSocket)
		{
		iSession->SSLQueueConnectL(iStatus,aHost,aPortNum,aPrefs);
		}
	else
		{
		iSession->QueueConnectL(iStatus,aHost,aPortNum,aPrefs);
		}

	SetActive();
	iState=EIOStateConnectQueued;
	}

// Parent wants the next line, all nicely parsed. When this completes, the
// parent calls RootAtom() to get the root of the parse tree.
TInt CImapIO::GetReply(TRequestStatus& aStatus)
	{
	// Call with maximum buffer size: no partial returns
	return(GetReply(aStatus,KReceiveBuffer,EFalse));
	}
	
TInt CImapIO::GetReply(TRequestStatus& aStatus, const TInt aFetchSize, const TBool aPartialReturn)
	{
	DBG((LogText(_L8("CImapIO::GetReply (aFetchSize=%d, aPartialReturn=%d)"),aFetchSize,aPartialReturn)));

	// Disconnected?
	if (iState==EIOStateDisconnected)
		return(KErrDisconnected);

	// Have to be connected & not busy
	__ASSERT_ALWAYS(iState==EIOStateConnected,gPanic(EIOWhenNotReady));
	
	// Queue request
	Queue(aStatus);

	// Save max number of bytes to read in next request
	iBytesToRead=aFetchSize;

	// Partial return required? (ie return tree in as-is state when read
	// completes)
	iReturnPartialLine=aPartialReturn;

	// Still busy (last request cancelled?), or still building tree?
	if (iBytesRead)
		{
		// Carry on with this one, return it when ready
		}
	else
		{
		// New line

		// Blank output buffer
		iBuffer->Des().Zero();

		// Delete existing tree
		if (iRootAtom!=NULL)
			{

			// remove atoms from atom tree
			iAtomArray.ResetAndDestroy();
			
			delete iRootAtom;
			iRootAtom=NULL;
			}

		// Reset (empty) atom stack
		iAtomStack.Reset();

		// Reset state
		iParserState=EIOStateAtomWait;

		// First created atom will be child of root
		iRootAtom=new CImapAtom();
		iAtom=iRootAtom;
		iNextIsChild=ETrue;
		}

	// Anything in receive buffer? Deal with it if we can
	TRAPD(err,ProcessBufferL());
	if (err!=KErrNone)
		{
		// An error has occurred: return this
		DBG((LogText(_L8("CImapIO::ProcessBuffer: Complete in GetReply err %d"),err)));
		Complete(KErrGeneral);
		}

	return(KErrNone);
	}

// Send with structure
TInt CImapIO::Send(TRequestStatus &aStatus, TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	iTransmit.Zero();
	iTransmit.AppendFormatList(aFmt,list);

	return(Send(aStatus, iTransmit));
	}

// Send with structure
void CImapIO::SendL(TRequestStatus &aStatus, TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	iTransmit.Zero();
	iTransmit.AppendFormatList(aFmt,list);

	User::LeaveIfError(Send(aStatus, iTransmit));
	}

// Send with structure, specifying the transport-idle timeout to be used.
void CImapIO::SendWithTimeoutL(TRequestStatus &aStatus, TInt aTimeout, TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	iTransmit.Zero();
	iTransmit.AppendFormatList(aFmt,list);

	User::LeaveIfError(SendWithTimeout(aStatus, aTimeout, iTransmit));
	}

// Send a string
TInt CImapIO::Send(TRequestStatus& aStatus, const TDesC8& aLine)
	{
	// Disconnected?
	if (iState==EIOStateDisconnected)
		return(KErrDisconnected);

	if (iState!=EIOStateConnected)
		{
		return(KErrNotReady);
		}
		
	// We're queuing a send
	iState=EIOStateWriteQueued;

	Queue(aStatus);
	iTXbytes+=aLine.Length();
	iSession->Send(iStatus,aLine);
	SetActive();

	return(KErrNone);
	}

// Send a string, specifying the transport-idle timeout to be used
TInt CImapIO::SendWithTimeout(TRequestStatus& aStatus, TInt aTimeout, const TDesC8& aLine)
	{
	// Disconnected?
	if (iState==EIOStateDisconnected)
		return(KErrDisconnected);

	if (iState!=EIOStateConnected)
		{
		return(KErrNotReady);
		}
		
	// We're queuing a send
	iState=EIOStateWriteQueued;

	Queue(aStatus);
	iTXbytes+=aLine.Length();
	iSession->SendWithTimeout(iStatus, aTimeout, aLine);
	SetActive();

	return(KErrNone);
	}

// Logging calls: passed through to IMSK
void CImapIO::LogText(const TDesC8& aString)
	{
	// Log the text
	if (iSession)
		iSession->LogText(aString);
	else
		{
#if defined(IMAPLOG)
		// Log it to secondary log file (so that we get logging after
		// connection close)
		if (!iImapLog)
			{
			TBuf<64> buf;
			buf.AppendFormat(_L("c:\\logs\\Email\\imaplog%d.txt"),iID);
			TRAP_IGNORE(iImapLog=CImLog::NewL(buf, EAppend));
			}
		
		if (iImapLog)
			iImapLog->AppendComment(aString);
#endif
		}
	}

void CImapIO::LogText(TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TBuf8<1024> aBuf;

	aBuf.AppendFormatList(aFmt,list);
	LogText(aBuf);
	}

// Pass Logging onto the Session
void CImapIO::PerformLogging(TBool aLogging)
	{
	iSession->PerformLogging(aLogging);
	}

// Bytes in/out
TInt CImapIO::RXbytes(const TBool aReset)
	{
	TInt b=iRXbytes;
	if (aReset) iRXbytes=0;
	return(b);
	}

TInt CImapIO::TXbytes(const TBool aReset)
	{
	TInt b=iTXbytes;
	if (aReset) iTXbytes=0;
	return(b);
	}

// Setting of current primaryTextServerSession, going to be use on the secondary session
void CImapIO::SetPrimaryTextServerSession(CImTextServerSession* aPrimaryTextServerSession)
	{
	iPrimaryTextServerSession=aPrimaryTextServerSession;
	}

// Retruns the current textserversession
CImTextServerSession* CImapIO::GetTextServerSession()
	{
	return iSession;
	}

// Return descriptor, without any enclosing brackets < >
TPtrC8 CImapAtom::AtomNoAngleBrackets()
	{
	TPtrC8 atom = iAtom;
	TInt len = atom.Length();

	if (len>2 && atom[0]==KImcvLeftChevron && atom[len-1]==KImcvRightChevron)
		{
		atom.Set(atom.Mid(1,len-2));
		}
	return(atom);
	}



