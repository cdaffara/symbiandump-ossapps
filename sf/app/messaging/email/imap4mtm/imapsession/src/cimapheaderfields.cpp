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

#include "cimapheaderfields.h"

CImapHeaderFields::CImapHeaderFields()
	{
	}

void CImapHeaderFields::ConstructL(TInt aStartFieldCount)
	{
	for (TInt field = 0; field < aStartFieldCount; ++field)
		{
		iImapFieldValueArray.AppendL(NULL);
		}
	}

CImapHeaderFields::~CImapHeaderFields()
	{
	iImapFieldValueArray.ResetAndDestroy();
	}

/**
Sets the value of the field identified by the supplied field name
@param aFieldname Name of the field to set
@param aValue Value to set the field to. This routine takes immediate
       ownership of the buffer
@return Whether field has been set to the new value
*/
TBool CImapHeaderFields::SetFieldL(const TDesC8& aFieldName, HBufC8* aValue)
	{
	CleanupStack::PushL(aValue);

	TBool needToStripSpaces = EFalse;

	TInt fieldId = Match(aFieldName, needToStripSpaces);

	if (fieldId != KErrNotFound)
		{
		ASSERT(fieldId < iImapFieldValueArray.Count());

		if (needToStripSpaces)
			{
			HBufC8* strippedValue = StripSpacesL(*aValue);
			CleanupStack::PopAndDestroy(aValue);
			aValue = strippedValue;
			CleanupStack::PushL(aValue);
			}

		delete iImapFieldValueArray[fieldId];
		iImapFieldValueArray[fieldId] = aValue;
		CleanupStack::Pop(aValue);
		return ETrue;
		}

	CleanupStack::PopAndDestroy(aValue);
	return EFalse;
	}

/**
Returns the value of the field identified by the supplied field index.
If the field has not been populated, a null descriptor is returned.
@param aFieldId index of the field value to return.
@return The value of the filed or a null descriptor.
*/
const TDesC8& CImapHeaderFields::FieldValue(TInt aFieldId)
	{
	ASSERT(aFieldId < iImapFieldValueArray.Count());

	return (iImapFieldValueArray[aFieldId] != NULL) ? *(iImapFieldValueArray[aFieldId]) : KNullDesC8();
	}

/**
Returns whether the field identified by the supplied field index has been populated.
This indicates whether the field value was supplied by the server.
@param aFieldId index of the field value to return.
@return Whether field has been populated.
*/
TBool CImapHeaderFields::FieldExists(TInt aFieldId)
	{
	ASSERT(aFieldId < iImapFieldValueArray.Count());

	return (iImapFieldValueArray[aFieldId] != NULL) ? ETrue : EFalse;
	}

/**
Strips all spaces from a header field
@param aValue Field value to strip spaces from
@return Buffer containing stripped value. Ownership is passed back to caller
*/
HBufC8* CImapHeaderFields::StripSpacesL(const TDesC8& aBuffer)
	{
	TInt bufferLen = aBuffer.Length();

	HBufC8* strippedValue = HBufC8::NewL(bufferLen);
	TPtr8 strippedBuffer = strippedValue->Des();

	for (TInt bufferPos = 0; bufferPos < bufferLen; ++bufferPos)
		{
		if (aBuffer[bufferPos] > ' ')
			{
			strippedBuffer.Append(aBuffer[bufferPos]);
			}
		}

	return strippedValue;
	}
