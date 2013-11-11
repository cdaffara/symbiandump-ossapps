/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implements the definition of CICalContentLineWriter.It writes the ICAL content.
*
*/



// Class include.
#include "ICalContentLineWriter.h"	// CICalContentLineWriter

//debug
#include "calendarengines_debug.h"

// System includes.
#include <s32strm.h>	// RWriteStream
#include <utf.h>		// CnvUtfConverter

// Constants.
const TInt KICalMaxLineLength = 75;	// The length of a line allowed by RFC 2445.
const TInt KICalMaxFoldedLineLength = KICalMaxLineLength - 1;	// Allows for the extra space at the start of the line.

// Line folding literals.
_LIT8(KICalCRLF,"\r\n");
_LIT8(KICalFoldLine,"\r\n ");

/**
Allocates and constructs a new CICalContentLineWriter.
@return The newly constructed CICalContentLineWriter
@internalTechnology
*/
CICalContentLineWriter* CICalContentLineWriter::NewL(RWriteStream& aStream)
	{
	TRACE_ENTRY_POINT;
	
	CICalContentLineWriter* self = CICalContentLineWriter::NewLC(aStream);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Allocates and constructs a new CICalContentLineWriter.
The pointer to the new object is left on the Cleanup Stack.
@return The newly constructed CICalContentLineWriter.
@internalTechnology
*/
CICalContentLineWriter* CICalContentLineWriter::NewLC(RWriteStream& aStream)
	{
	TRACE_ENTRY_POINT;
	
	CICalContentLineWriter* self = new (ELeave) CICalContentLineWriter(aStream);
	CleanupStack::PushL(self);
	self->ConstructL();
	
	TRACE_EXIT_POINT;
	return self;
	}
	
/**
Destructor.
@internalTechnology
*/
CICalContentLineWriter::~CICalContentLineWriter()
	{
	TRACE_ENTRY_POINT;
	
	delete iCurrentLine;
	
	TRACE_EXIT_POINT;
	}
	
/**
Writes the current content line to the stream after conversion to UTF-8.
Performs any necessary folding.
@internalTechnology
*/
void CICalContentLineWriter::WriteContentLineL()
	{
	TRACE_ENTRY_POINT;
	
	// Convert to UTF-8 for writing
	HBufC8* tmpLine = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*iCurrentLine);
	CleanupStack::PushL(tmpLine);
	
	TInt pos(0);
	TInt remaining(tmpLine->Length());
	
	// Fold the line if longer than 75 octets
	TInt lineLength(KICalMaxLineLength);
	
	while (remaining > lineLength)
		{
		iWriteStream.WriteL(tmpLine->Mid(pos), lineLength);
		iWriteStream.WriteL(KICalFoldLine);
		pos += lineLength;
		remaining -= lineLength;
		lineLength = KICalMaxFoldedLineLength;
		}
		
	// Complete the line
	iWriteStream.WriteL(tmpLine->Mid(pos));
	iWriteStream.WriteL(KICalCRLF);	

	CleanupStack::PopAndDestroy(tmpLine);
	
	iCurrentLine->Des().SetLength(0);
	
	TRACE_EXIT_POINT;
	}
	
/**
Append the contents of aLine to the current content line being formed.
iCurrentLine will be reallocated if not enough storage is available
@param aLine Descriptor to append.
@internalTechnology
*/
void CICalContentLineWriter::AppendL(const TDesC& aLine)
	{
	TRACE_ENTRY_POINT;
	
	if (!iCurrentLine)
		{
		iCurrentLine = HBufC::NewL(KICalMaxLineLength);
		}
		
	if ((iCurrentLine->Length() + aLine.Length()) > iCurrentLine->Des().MaxLength())
		{
		// Reallocate the buffer
		iCurrentLine = iCurrentLine->ReAllocL(iCurrentLine->Des().MaxLength() + aLine.Length());
		}
		
	iCurrentLine->Des().Append(aLine);	
	TRACE_EXIT_POINT;
	}
	
/**
Append a character to the current content line being formed.
iCurrentLine will be reallocated if not enough storage is available.
@param aCharacter Character to append.
@internalTechnology
*/
void CICalContentLineWriter::AppendL(const TChar& aCharacter)
	{
	TRACE_ENTRY_POINT;
	
	if (!iCurrentLine)
		{
		iCurrentLine = HBufC::NewL(KICalMaxLineLength);
		}
		
	if ((iCurrentLine->Length() + 1) > iCurrentLine->Des().MaxLength())
		{
		// Reallocate the buffer - expand by KICalMaxLineLength rather than '1'
		// to allow space for further expansion without further reallocation.
		iCurrentLine = iCurrentLine->ReAllocL(iCurrentLine->Des().MaxLength() + KICalMaxLineLength);
		}

	iCurrentLine->Des().Append(aCharacter);
	
	TRACE_EXIT_POINT;
	}

/**
Default constructor.
@internalTechnology
@param aStream The stream to write to.
@internalTechnology
*/
CICalContentLineWriter::CICalContentLineWriter(RWriteStream& aStream)
 :	iWriteStream(aStream)
	{
	TRACE_ENTRY_POINT;
	}
	
/**
Second phase construction.
@internalTechnology
*/
void CICalContentLineWriter::ConstructL()
	{
	TRACE_ENTRY_POINT;
	iCurrentLine = HBufC::NewL(KICalMaxLineLength);
	TRACE_EXIT_POINT;
	}
	
// End of File
