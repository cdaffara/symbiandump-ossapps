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

#include "cimapmimeheaderfields.h"
#include "cimapsessionconsts.h"

CImapMimeHeaderFields* CImapMimeHeaderFields::NewL()
	{
	CImapMimeHeaderFields* self = new (ELeave) CImapMimeHeaderFields();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CImapMimeHeaderFields::CImapMimeHeaderFields()
	{
	}

void CImapMimeHeaderFields::ConstructL()
	{
	CImapHeaderFields::ConstructL(EImapCountFields);
	}

CImapMimeHeaderFields::~CImapMimeHeaderFields()
	{
	iImapUnmatchedNameArray.ResetAndDestroy();
	iImapUnmatchedValueArray.ResetAndDestroy();
	}

/**
Returns the value of the field identified by the supplied field index.
If the field has not been populated, a null descriptor is returned.
@param aFieldId index of the field value to return.
@return The value of the filed or a null descriptor.
*/
EXPORT_C const TDesC8& CImapMimeHeaderFields::FieldValue(THeaderFields aFieldId)
	{
	return CImapHeaderFields::FieldValue(aFieldId);
	}

/**
Returns whether the field identified by the supplied field index has been populated.
This indicates whether the field value was supplied by the server.
@param aFieldId index of the field value to return.
@return Whether field has been populated.
*/
EXPORT_C TBool CImapMimeHeaderFields::FieldExists(THeaderFields aFieldId)
	{
	return CImapHeaderFields::FieldExists(aFieldId);
	}

TBool CImapMimeHeaderFields::SetFieldL(const TDesC8& aFieldName, HBufC8* aValue)
	{
	// Take a copy of the value, as CImapHeaderFields::SetFieldL takes
	// ownership of it and may destroy it.
	HBufC8* valueCopy = (*aValue).AllocLC();

	if (CImapHeaderFields::SetFieldL(aFieldName, aValue))
		{
		CleanupStack::PopAndDestroy(valueCopy);
		return ETrue;
		}

	// Try to find the name in the unmatched name list
	for (TInt pos(0); pos < iImapUnmatchedNameArray.Count(); ++pos)
		{
		if (iImapUnmatchedNameArray[pos]->CompareF(aFieldName) == 0)
			{
			// The name has been found. Replace the corresponding value
			// with the new one. Ownership is passed to the array.
			delete iImapUnmatchedValueArray[pos];
			iImapUnmatchedValueArray[pos] = valueCopy;
			CleanupStack::Pop(valueCopy);
			return ETrue;
			}
		}

	// The name has not been found. We need to add a new entry to the
	// unmatched lists.
	HBufC8* name = aFieldName.AllocLC();

	// Append name to field name array. We pass ownership to the array.
	iImapUnmatchedNameArray.AppendL(name);
	CleanupStack::Pop(name);

	// Append value to field value array. We pass ownership to the array.
	iImapUnmatchedValueArray.AppendL(valueCopy);
	CleanupStack::Pop(valueCopy);

	return ETrue;
	}

void CImapMimeHeaderFields::RestartGetNextField()
	{
	iGetNextFieldPos = 0;
	}

TBool CImapMimeHeaderFields::GetNextField(TPtrC8& aFieldName, TPtrC8& aValue)
	{
	TBool found(EFalse);

	while (!found && (iGetNextFieldPos < EImapCountFields))
		{
		if (CImapHeaderFields::FieldExists(iGetNextFieldPos))
			{
			aFieldName.Set(FieldNameFromId(static_cast<THeaderFields>(iGetNextFieldPos)));
			aValue.Set(CImapHeaderFields::FieldValue(iGetNextFieldPos));
			found = ETrue;
			}

		++iGetNextFieldPos;
		}

	if (!found)
		{
		TInt offset(iGetNextFieldPos - EImapCountFields);

		if (offset < iImapUnmatchedNameArray.Count())
			{
			aFieldName.Set(*iImapUnmatchedNameArray[offset]);
			aValue.Set(*iImapUnmatchedValueArray[offset]);
			found = ETrue;
			}
		}

	++iGetNextFieldPos;
	
	return found;
	}

TInt CImapMimeHeaderFields::Match(const TDesC8& aFieldName, TBool& aNeedToStripSpaces)
	{
	THeaderFields fieldId;

	if (aFieldName.CompareF(KImapTxtContentBase) == 0)
		{
		fieldId = EImapContentBase;
		aNeedToStripSpaces = ETrue;
		}
	else if (aFieldName.CompareF(KImapTxtContentLocation) == 0)
		{
		fieldId = EImapContentLocation;
		aNeedToStripSpaces = ETrue;
		}
	else
		{
		return KErrNotFound;
		}

	return fieldId;
	}

const TDesC8& CImapMimeHeaderFields::FieldNameFromId(THeaderFields aFieldId)
	{
	switch (aFieldId)
		{
		case EImapContentBase:
			{
			return KImapTxtContentBase();
			}

		case EImapContentLocation:
			{
			return KImapTxtContentLocation();
			}

		default:
			{
			break;
			}
		}

	return KNullDesC8();
	}
