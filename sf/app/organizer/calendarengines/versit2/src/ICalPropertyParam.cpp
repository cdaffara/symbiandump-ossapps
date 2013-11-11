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
* Description:    Implements the definition of CICalPropertyParam.
*
*/



// Class include.
#include "ICalPropertyParam.h"	// CICalPropertyParam

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalContentLineWriter.h"	// CICalContentLineWriter
#include "ICalErrors.h"				// Error values
#include "ICalKeyWords.h"			// Literals
#include "ICalValue.h"				// CICalValue

/**
Static factory constructor. Takes ownership of the passed value pointer and
deletes it if this function leaves. Note that other values may be added later.
@param aType The type of parameter.
@param aValue A value of the parameter - this class takes ownership.
@return A new property parameter
@leave Leaves with KErrParameterHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C CICalPropertyParam* CICalPropertyParam::NewL(const TDesC& aType, CICalValue* aValue)
	{
	TRACE_ENTRY_POINT;
	
	CICalPropertyParam* self = CICalPropertyParam::NewLC(aType, aValue);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Static factory constructor. Takes ownership of the passed value pointer and
deletes it if this function leaves. Note that other values may be added later.
The constructed object is pushed onto the Cleanup Stack.
@param aType The type of parameter.
@param aValue A value of the parameter - this class takes ownership.
@return A new property parameter
@leave Leaves with KErrParameterHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C CICalPropertyParam* CICalPropertyParam::NewLC(const TDesC& aType, CICalValue* aValue)
	{
	TRACE_ENTRY_POINT;
	
	// Test that the value is valid before construction.
	if (!aValue)
		{
		User::Leave(KErrParameterHasNoValue);
		}
	
	// Push aValue on the Cleanup Stack.
	CleanupStack::PushL(aValue);
		
	// Create a new object.
	CICalPropertyParam* self = new (ELeave) CICalPropertyParam(aValue);
	
	// Pop aValue from the Cleanup Stack as it is temporarily owned by self.
	CleanupStack::Pop(aValue);
	
	// Construct the new object.
	CleanupStack::PushL(self);
	self->ConstructL(aType, aValue);
	
	TRACE_EXIT_POINT;
	return self;
	}
	
/**
Static factory constructor. Note that other values may be added later. The
constructed object is pushed onto the Cleanup Stack.
@param aType The type of parameter.
@param aValue A value of the parameter passed as a descriptor.
@return A new property parameter
@publishedPartner
*/
EXPORT_C CICalPropertyParam* CICalPropertyParam::NewL(const TDesC& aType, const TDesC& aValue)
	{
	TRACE_ENTRY_POINT;
	
	CICalPropertyParam* self = NewLC(aType, aValue);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}
	
/**
Static factory constructor. Note that other values may be added later. The
constructed object is pushed onto the Cleanup Stack.
@param aType The type of parameter.
@param aValue A value of the parameter passed as a descriptor.
@return A new property parameter
@publishedPartner
*/
EXPORT_C CICalPropertyParam* CICalPropertyParam::NewLC(const TDesC& aType, const TDesC& aValue)
	{
	TRACE_ENTRY_POINT;
	
	CICalPropertyParam* self = new (ELeave) CICalPropertyParam;
	CleanupStack::PushL(self);
	self->ConstructL(aType, aValue);
	
	TRACE_EXIT_POINT;
	return self;
	}
	
/**
Destructor
@internalTechnology
*/
CICalPropertyParam::~CICalPropertyParam()
	{
	TRACE_ENTRY_POINT;
	
	delete iValue;
	delete iType;
	iValues.ResetAndDestroy();
	
	TRACE_EXIT_POINT;
	}

/**
Static factory construction. The constructed object is pushed onto the Cleanup
Stack. Note that this constructor is not exported.
@return A new blank property parameter.
@internalTechnology
*/
CICalPropertyParam* CICalPropertyParam::NewLC()
	{
	TRACE_ENTRY_POINT;
	
	CICalPropertyParam* self = new (ELeave) CICalPropertyParam;
	CleanupStack::PushL(self);
	self->ConstructL(KNullDesC);	// No type set - SetTypeL() must be called on this object.
	
	TRACE_EXIT_POINT;
	return self;
	}
	
/**
Adds a string to the parameter's list of values. Any quotes are removed from
the value as they are only used to escape other characters and are not a valid
parameter value character in themselves.
@param aValue The value to add.
@leave Leaves if there is an error adding a value.
@publishedPartner
*/
EXPORT_C void CICalPropertyParam::AddValueL(const TDesC& aValue)
	{
	TRACE_ENTRY_POINT;
	
	// First remove any double quotes. These are used to escape colons, etc.
	// in property parameter values and do not need to be stored.
	// Note that double quote is not a valid character here and this assumes
	// that they should all be removed. There is no check for matching quotes.
	HBufC* parameterValue = aValue.AllocLC();
	
	TInt location(parameterValue->Locate(KICalQuoteChar));

	while (location >= 0)
		{
		parameterValue->Des().Delete(location, 1);
		location = parameterValue->Locate(KICalQuoteChar);
		}

	if (parameterValue->Length() > 0)
		{
		// Create a new value and append to the list of values.
		CICalValue* value = CICalValue::NewLC();
		value->SetTextL(*parameterValue);
		User::LeaveIfError(iValues.Append(value));
		CleanupStack::Pop(value);
		}
		
	CleanupStack::PopAndDestroy(parameterValue);
	
	TRACE_EXIT_POINT;
	}
	
/**
Adds a value to the parameter's list of values, taking ownership. Note that the
value is deleted if this function leaves.
@param aValue The value to add.
@leave Leaves with KErrParameterHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C void CICalPropertyParam::AddValueL(CICalValue* aValue)
	{
	TRACE_ENTRY_POINT;
	
	if (!aValue)
		{
		User::Leave(KErrParameterHasNoValue);
		}
		
	// Append the value to the list of values.
	TInt err(iValues.Append(aValue));
	
	if (err)
		{
		delete aValue;
		User::Leave(err);
		}
	TRACE_EXIT_POINT;
	}
	
/**
Gets the type of the parameter
@return The type of the property parameter or an empty descriptor if no type is
set.
@publishedPartner
*/
EXPORT_C const TDesC& CICalPropertyParam::Type() const
	{
	TRACE_ENTRY_POINT;
	
	if (iType)
		{
		TRACE_EXIT_POINT;
		return *iType;
		}
		
	// Else type is not set due to SetTypeL() leaving.
	TRACE_EXIT_POINT;
	return KNullDesC;
	}

/**
Access method for the value array.
@return The array of values.
@publishedPartner
*/
EXPORT_C const RPointerArray<CICalValue>& CICalPropertyParam::Values() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iValues;
	}

/**
Sets the type of the parameter to a string value
@param aType the type of the property parameter
@internalTechnology
*/
void CICalPropertyParam::SetTypeL(const TDesC& aType)
	{
	TRACE_ENTRY_POINT;
	
	delete iType;
	iType = NULL;
	iType = aType.AllocL();
	
	TRACE_EXIT_POINT;
	}
	
/**
Export this property parameter using the given line writer. Values are quoted
if they contain characters that need to be escaped. Assumes it contains at
least one value from the check in CICalProperty::ExternalizeL().
@param aWriter The content line writer to export to.
@internalTechnology
*/
void CICalPropertyParam::ExternalizeL(CICalContentLineWriter& aWriter)
	{
	TRACE_ENTRY_POINT;

	// Export the property parameter name.
	aWriter.AppendL(Type());
	aWriter.AppendL(KICalEquals());

	// Export each property parameter value.
	
	const TInt lastValue = iValues.Count() - 1;	

	for (TInt x = 0; x <= lastValue; ++x)
		{
		// Decide if this value needs to be escaped - in other words
		// if it contains a colon, a semi-colon or a comma.
		const TDesC& value = iValues[x]->TextL();
		
		if ((value.Locate(KICalCommaChar) >= 0) ||
			(value.Locate(KICalColonChar) >= 0) ||
			(value.Locate(KICalSemiColonChar) >= 0))
			{
			// Escape by quoting entire string.
			aWriter.AppendL(KICalQuote());
			aWriter.AppendL(value);
			aWriter.AppendL(KICalQuote());
			}
		else
			{
			// No need to escape value.
			aWriter.AppendL(value);
			}

		// Write out a comma separator between values.			
		if (x < lastValue)
			{
			aWriter.AppendL(KICalComma());
			}
		}
	TRACE_EXIT_POINT;
	}
	
/**
Determines if two property parameters are equal. Comparison is case-sensitive.
@param aParam1 The first property parameter.
@param aParam2 The second property parameter.
@return ETrue if they are equal.
@internalTechnology
*/
TBool CICalPropertyParam::EqualsL(const CICalPropertyParam& aParam1, const CICalPropertyParam& aParam2)
	{
	TRACE_ENTRY_POINT;
	
	if (aParam1.Type() != aParam2.Type())
		{
		TRACE_EXIT_POINT;
		return EFalse;
		}
	
	const TInt values1 = aParam1.iValues.Count();
	const TInt values2 = aParam2.iValues.Count();
	
	if (values1 != values2)
		{
		TRACE_EXIT_POINT;
		return EFalse;
		}
		
	for (TInt i = 0; i < values1; i++)
		{
		if (aParam1.iValues[i]->TextL() != aParam2.iValues[i]->TextL())
			{
			TRACE_EXIT_POINT;
			return EFalse;
			}
		}
	
	TRACE_EXIT_POINT;
	return ETrue;
	}
	
/**
Constructor
@internalTechnology
*/
CICalPropertyParam::CICalPropertyParam()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}
	
/**
Constructor
@param aValue The initial value.
@internalTechnology
*/
CICalPropertyParam::CICalPropertyParam(CICalValue* aValue)
 :	iValue(aValue)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}
	
/**
Internal construction.
@param aType The type of parameter.
@internalTechnology
*/
void CICalPropertyParam::ConstructL(const TDesC& aType)
	{
	TRACE_ENTRY_POINT;
	
	iType = aType.AllocL();
	
	TRACE_EXIT_POINT;
	}

/**
Internal construction.
@param aType The type of parameter.
@param aValue A value of the parameter.
@internalTechnology
*/
void CICalPropertyParam::ConstructL(const TDesC& aType, const TDesC& aValue)
	{
	TRACE_ENTRY_POINT;
	
	ConstructL(aType);
	
	// Add the value.
	AddValueL(aValue);
	
	TRACE_EXIT_POINT;
	}
	
/**
Internal construction. Takes ownership of the passed value pointer and
deletes it if this function leaves.
@param aType The type of parameter.
@param aValue A value of the parameter - this class takes ownership.
@internalTechnology
*/
void CICalPropertyParam::ConstructL(const TDesC& aType, CICalValue* aValue)
	{
	TRACE_ENTRY_POINT;
	
	ConstructL(aType);
	
	// Add the value.
	iValue = NULL;	// If we got this far then we can release direct ownership of the value.
	AddValueL(aValue);	// Value is deleted if this leaves.
	
	TRACE_EXIT_POINT;
	}

// End of File
