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
* Description:   Implements the definition of CICalProperty.
*
*/



// Class include.
#include "ICalProperty.h"	// CICalProperty

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalContentLineWriter.h"	// CICalContentLineWriter
#include "ICalErrors.h"				// Error values
#include "ICalKeyWords.h"			// Literals
#include "ICalPropertyParam.h"		// CICalPropertyParam
#include "ICalUtil.h"				// NICalUtil
#include "ICalValue.h"				// CICalValue

/**
Constructs a new CICalProperty with given type and value and returns it.
Note that other values may be added later.
@param aType The type of the property.
@param aValue The value of the property.
@return a pointer to a new CICalProperty
@publishedPartner
*/
EXPORT_C CICalProperty* CICalProperty::NewL(const TDesC& aType, const TDesC& aValue)
	{
	TRACE_ENTRY_POINT;
	
	CICalProperty* self = CICalProperty::NewLC(aType, aValue);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Constructs a new CICalProperty with given type and value, pushes it onto the
Cleanup Stack, and returns it. Note that other values may be added later.
@param aType The type of the property.
@param aValue The value of the property.
@return a pointer to a new CICalProperty
@publishedPartner
*/
EXPORT_C CICalProperty* CICalProperty::NewLC(const TDesC& aType, const TDesC& aValue)
	{
	TRACE_ENTRY_POINT;
	
	CICalProperty* self = new (ELeave) CICalProperty;
	CleanupStack::PushL(self);
	self->ConstructL(aType, aValue);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Constructs a new CICalProperty with given type and value and returns it. Takes
ownership of the passed value pointer and deletes it if this function leaves.
Note that other values may be added later.
@param aType The type of the property.
@param aValue The value of the property.
@return a pointer to a new CICalProperty
@leave Leaves with KErrPropertyHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C CICalProperty* CICalProperty::NewL(const TDesC& aType, CICalValue* aValue)
	{
	TRACE_ENTRY_POINT;
	
	CICalProperty* self = CICalProperty::NewLC(aType, aValue);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Constructs a new CICalProperty with given type and value, pushes it onto the
Cleanup Stack, and returns it. Takes ownership of the passed value pointer and
deletes it if this function leaves. Note that other values may be added later.
@param aType The type of the property.
@param aValue The value of the property.
@return a pointer to a new CICalProperty
@leave Leaves with KErrPropertyHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C CICalProperty* CICalProperty::NewLC(const TDesC& aType, CICalValue* aValue)
	{
	TRACE_ENTRY_POINT;
	
	// Test that the value is valid before construction.
	if (!aValue)
		{
		User::Leave(KErrPropertyHasNoValue);
		}
		
	// Push the passed value onto the Cleanup Stack.
	CleanupStack::PushL(aValue);
	
	// Create a new property.
	CICalProperty* self = new (ELeave) CICalProperty(aValue);

	// Pop the passed value from the Cleanup Stack as it is temporarily owned by self.
	CleanupStack::Pop(aValue);

	// Construct the property.
	CleanupStack::PushL(self);
	self->ConstructL(aType, aValue);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Destructor.
@internalTechnology
*/
CICalProperty::~CICalProperty()
	{
	TRACE_ENTRY_POINT;
	
	delete iValue;
	delete iType;
	iParams.ResetAndDestroy();
	iValues.ResetAndDestroy();
	
	TRACE_EXIT_POINT;
	}

/**
Constructs a new CICalProperty with given type and returns it
@param aType The type of the property.
@return a pointer to a new CICalProperty
@internalTechnology
*/
CICalProperty* CICalProperty::NewL(const TDesC& aType)
	{
	TRACE_ENTRY_POINT;
	
	CICalProperty* self = CICalProperty::NewLC(aType);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Constructs a new CICalProperty with given type, pushes it onto the Cleanup
Stack, and returns it.
@param aType The type of the property.
@return a pointer to a new CICalProperty.
@internalTechnology
*/
CICalProperty* CICalProperty::NewLC(const TDesC& aType)
	{
	TRACE_ENTRY_POINT;
	
	CICalProperty* self = new (ELeave) CICalProperty;
	CleanupStack::PushL(self);
	self->ConstructL(aType);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Gets the type of the property.
@return The type of the property as a string.
@publishedPartner
*/
EXPORT_C const TDesC& CICalProperty::Type() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return *iType;
	}
	
/**
Adds a parameter with given type and value to the property, taking ownership of
the value. Note that further values may be added later.
@param aType The type of property parameter.
@param aValue A value of the property parameter - this will be deleted if this function leaves.
@return A new property parameter.
@leave Leaves with KErrParameterHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C CICalPropertyParam& CICalProperty::AddPropertyParamL(const TDesC& aType, CICalValue* aValue)
	{
	TRACE_ENTRY_POINT;
	
	if (!aValue)
		{
		User::Leave(KErrParameterHasNoValue);
		}
	
	CICalPropertyParam* param = CICalPropertyParam::NewLC(aType, aValue);
	User::LeaveIfError(iParams.Append(param));
	CleanupStack::Pop(param);
	
	TRACE_EXIT_POINT;
	return *param;
	}

/**
Adds a parameter with given type and value to the property.
Note that further values may be added later.
@param aType The type of property parameter.
@param aValue A value of the property parameter.
@return A new property parameter.
@leave Leaves with if there is an error adding a parameter.
@publishedPartner
*/
EXPORT_C CICalPropertyParam& CICalProperty::AddPropertyParamL(const TDesC& aType, const TDesC& aValue)
	{
	TRACE_ENTRY_POINT;
	
	CICalPropertyParam* param = CICalPropertyParam::NewLC(aType, aValue);
	User::LeaveIfError(iParams.Append(param));
	CleanupStack::Pop(param);
	
	TRACE_EXIT_POINT;
	return *param;
	}

/**
Adds a value to the property. Any escaped characters in the value string are
translated to the actual characters.
@param aValue The text string of the value, as would be entered into an
iCalendar file.
@leave Leaves if there is an error adding a value.
@publishedPartner
*/
EXPORT_C void CICalProperty::AddValueL(const TDesC& aValue)
	{
	TRACE_ENTRY_POINT;
	
	HBufC* parameterValue = NULL;

	// Create a new value.
	CICalValue* value = CICalValue::NewLC();

	// If the property has a text value then find any escaped characters and convert
	// them. Property values use backslash to escape the following characters:
	// commas, semi-colons, newlines (using \n or \N) and backslash itself.

	if (NICalUtil::PropertyHasTextValueL(*this))
		{
		parameterValue = aValue.AllocLC();
		TInt length(parameterValue->Length());
		TBool escaped(EFalse);
		
		for (TInt location = 0; location < length; location++)
			{
			if (escaped)
				{
				if (((*parameterValue)[location] == KICalUpperCaseNChar) ||
					((*parameterValue)[location] == KICalLowerCaseNChar))
					{
					// Found an escaped newline - replace with a single linefeed character.
					(parameterValue->Des()).operator[](location) = KICalNewlineChar;
					}
				
				// Else found an escaped backslash, semi-colon or comma, or
				// a single backslash (which is not valid).
				
				// In all cases remove the previous backslash character.
				// This assumes that (location - 1) >= 0, since escaped == ETrue
				// (The previous character was a backslash, so there *is* a previous character.)
				parameterValue->Des().Delete(location - 1, 1);
				length--;	// The length of the descriptor has shrunk by one.
				location--;	// The current character has slipped back a place.
				escaped = EFalse;
				}
			else 
			    {
			    if ((*parameterValue)[location] == KICalBackslashChar)
				    {
				    escaped = ETrue;
				
				    if (location == (length - 1))
					    {
					    // Found a single backslash at the end - this is not valid, so remove it.
					    parameterValue->Des().Delete(location, 1);
					    break;
					    }
				    }			    
			    }
			}
		}

	// Set the value.
	if (parameterValue)
		{
		value->SetTextL(*parameterValue);
		CleanupStack::PopAndDestroy(parameterValue);
		}
	else
		{
		// If the property does not have a text type then just use the given string as is...
		value->SetTextL(aValue);
		}	

	// Append the new value to the list of values.
	User::LeaveIfError(iValues.Append(value));
	CleanupStack::Pop(value);
	TRACE_EXIT_POINT;
	}

/**
Adds value to this property, taking ownership of the passed value pointer and
deleting it if this function leaves.
@param aValue The value to add - this must not be NULL.
@leave Leaves with KErrPropertyHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C void CICalProperty::AddValueL(CICalValue* aValue)
	{
	TRACE_ENTRY_POINT;
	
	if (!aValue)
		{
		User::Leave(KErrPropertyHasNoValue);
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
Access method for the array of parameters.
@return The array of parameters.
@publishedPartner
*/
EXPORT_C const RPointerArray<CICalPropertyParam>& CICalProperty::Parameters() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iParams;
	}
	
/**
Access method for the array of properties.
@return The array of properties.
@publishedPartner
*/
EXPORT_C const RPointerArray<CICalValue>& CICalProperty::Values() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iValues;
	}
	
/**
Find a parameter with the given name. Ownership is not passed out.
@param aType The type of the parameter to search for
@return A pointer to the parameter, or NULL if it is not found.
@publishedPartner
*/
EXPORT_C const CICalPropertyParam* CICalProperty::FindParam(const TDesC& aType) const
	{
	TRACE_ENTRY_POINT;
	
	const TInt count = iParams.Count();
	
	for (TInt p = 0; p < count; ++p)
		{
		if (iParams[p]->Type().CompareF(aType) == 0)
			{
			return iParams[p];
			}
		}

    TRACE_EXIT_POINT;		
	return NULL;
	}

/**
Adds an empty parameter to the property.
@return A new, empty parameter.
@leave Leaves if there is an error adding a parameter.
@internalTechnology
*/
CICalPropertyParam& CICalProperty::AddPropertyParamL()
	{
	TRACE_ENTRY_POINT;
	
	CICalPropertyParam* param = CICalPropertyParam::NewLC();
	User::LeaveIfError(iParams.Append(param));
	CleanupStack::Pop(param);
	
	TRACE_EXIT_POINT;
	return *param;
	}

/**
Removes the given parameter from the property.
@param aParam The parameter to remove.
@leave Leaves in debug mode if the parameter is not found (release mode ignores
this).
@internalTechnology
*/
void CICalProperty::RemovePropertyParamL(const CICalPropertyParam& aParam)
	{
	TRACE_ENTRY_POINT;
	
	// Find the parameter to remove.
	TInt index = iParams.Find(&aParam, CICalPropertyParam::EqualsL);

#ifdef _DEBUG
	User::LeaveIfError(index);	// Not expecting this.
#else	// _DEBUG
	if (index < 0)
		{
		return;	// Return quietly in release builds
		}
#endif	// _DEBUG
	
	// Destroy the parameter and remove it.
	delete iParams[index];
	iParams.Remove(index);
	
	TRACE_EXIT_POINT;
	}
	
/**
Export this property using the given line writer, escaping any necessary
characters. Assumes it contains at least one value from the check in
CICalBase::ExternalizeL().
@param aWriter The content line writer to export to.
@internalTechnology
*/
void CICalProperty::ExternalizeL(CICalContentLineWriter& aWriter)
	{
	TRACE_ENTRY_POINT;
	
	// Export property name.
	aWriter.AppendL(*iType);
	
	// Export parameters. 
	const TInt paramCount = iParams.Count();
	
	for (TInt x = 0; x < paramCount; ++x)
		{
		if (iParams[x]->Values().Count() > 0)
			{
			aWriter.AppendL(KICalSemiColon());
			iParams[x]->ExternalizeL(aWriter);
			}
		}
	
	// Export values.
	aWriter.AppendL(KICalColon());
	
	const TInt valCount = iValues.Count();
	
	for (TInt x = 0; x < valCount; ++x)
		{
		const TDesC& value = iValues[x]->TextL();
		
		// If the property has a text value then escape any necessary characters.
		if (NICalUtil::PropertyHasTextValueL(*this))
			{
			const TInt valueLength = value.Length();
			
			// Export each value a character at a time, translating any characters that need escaping.
			for (TInt character = 0; character < valueLength; character++)
				{
				// Property values use backslash to escape the following characters:
				// commas, semi-colons, newlines (using \n or \N) and backslash itself.
				if ((value[character] == KICalBackslashChar) ||
					(value[character] == KICalSemiColonChar) ||
					(value[character] == KICalCommaChar))
					{
					aWriter.AppendL(KICalBackslash());
					aWriter.AppendL(value[character]);
					}
				else if (value[character] == KICalNewlineChar)
					{
					aWriter.AppendL(KICalBackslashN());
					}
				else
					{
					aWriter.AppendL(value[character]);
					}
				}
			}
		else	// No escaping needed.
			{
			aWriter.AppendL(value);
			}
		
		if (x < valCount -1)
			{
			aWriter.AppendL(KICalComma());
			}
		}
		
	aWriter.WriteContentLineL();
	
	TRACE_EXIT_POINT;
	}

/**
Constructor
@internalTechnology
*/
CICalProperty::CICalProperty()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Constructor
@param aValue The initial value.
@internalTechnology
*/
CICalProperty::CICalProperty(CICalValue* aValue)
 :	iValue(aValue)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Internal construction
@param aType The type of property.
@internalTechnology
*/
void CICalProperty::ConstructL(const TDesC& aType)
	{
	TRACE_ENTRY_POINT;
	
	iType = aType.AllocL();
	
	TRACE_EXIT_POINT;
	}

/**
Internal construction.
@param aType The type of property.
@param aValue A value of the property.
@internalTechnology
*/
void CICalProperty::ConstructL(const TDesC& aType, const TDesC& aValue)
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
@param aType The type of property.
@param aValue A value of the property - this class takes ownership.
@internalTechnology
*/
void CICalProperty::ConstructL(const TDesC& aType, CICalValue* aValue)
	{
	TRACE_ENTRY_POINT;
	
	ConstructL(aType);
	
	// Add the value.
	iValue = NULL;	// If we got this far then we can release direct ownership of the value.
	AddValueL(aValue);	// Value is deleted if this leaves.
	
	TRACE_EXIT_POINT;
	}

// End of File
