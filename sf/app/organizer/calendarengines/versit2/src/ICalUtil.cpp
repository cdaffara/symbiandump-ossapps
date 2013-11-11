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
* Description:   Namespace class NICalUtil.Its an utility class.
*
*/



// Class include.
#include "ICalUtil.h"	// NICalUtil

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalErrors.h"			// Error codes
#include "ICalKeyWords.h"		// Literals
#include "ICalProperty.h"		// CICalProperty
#include "ICalPropertyParam.h"	// CICalPropertyParam

namespace NICalUtil
	{
	/**
	Finds the first occurence of a character within a string which does not occur
	between a pair of double quotes.
	@param aString The string to search.
	@param aChar The character to search for.
	@return The position of the character within the string, or KErrNotFound.
	@internalTechnology
	*/
	TInt FindFirstUnquoted(const TDesC& aString, const TChar& aChar)
		{
		TRACE_ENTRY_POINT;
		
		TBool quoted(EFalse);
		
		for (TInt pos = 0; pos < aString.Length(); ++pos)
			{
			TChar val(aString[pos]);
			
			if (val == KICalQuoteChar)
				{
				quoted = !quoted;
				}
			else if ((val == aChar) && (!quoted))
				{
				TRACE_EXIT_POINT;
				return pos;
				}
			}
			
		return KErrNotFound;
		}

	/**
	Finds the first occurence of a character within a string which is not escaped
	by a backslash character. Searching for a comma will find it in the following
	strings: "abc," and "abc\\," but will not find it in "abc\," and "abc\\\,".
	Note that backslash cannot exist unescaped, so cannot be searched for.
	@param aString The string to search.
	@param aChar The character to search for.
	@return The position of the character within the string, or KErrNotFound.
	@internalTechnology
	*/
	TInt FindFirstUnescaped(const TDesC& aString, const TChar& aChar)
		{
		TRACE_ENTRY_POINT;
		
		TBool escaped(EFalse);
		
		for (TInt pos = 0; pos < aString.Length(); ++pos)
			{
			TChar val(aString[pos]);
			
			if (val == KICalBackslashChar)
				{
				escaped = !escaped;
				}
			else 
				{
				if ((val == aChar) && (!escaped))
					{
					TRACE_EXIT_POINT;
					return pos;
					}
					
				escaped = EFalse;
				}
			}
		
		TRACE_EXIT_POINT;	
		return KErrNotFound;	
		}

	/**
	Finds the first occurence of a character within a string, whether or not it
	is escaped by a backslash character. If it was escaped, however, the
	aWasEscaped parameter will be set to true. Note that backslash cannot exist
	unescaped, so cannot be searched for.
	@param aString The string to search.
	@param aChar The character to search for.
	@param aWasEscaped True if the character was escaped, otherwise false.
	@return The position of the character within the string, or KErrNotFound.
	@internalTechnology
	*/
	TInt FindFirstAndUnescape(const TDesC& aString, const TChar& aChar, TBool& aWasEscaped)
		{
		TRACE_ENTRY_POINT;
		
		aWasEscaped = EFalse;
		TBool escaped(EFalse);
		
		for (TInt pos = 0; pos < aString.Length(); ++pos)
			{
			TChar val(aString[pos]);
			
			if (val == KICalBackslashChar)
				{
				escaped = !escaped;
				}
			else 
				{
				if (val == aChar)
					{
					aWasEscaped = escaped;
					TRACE_EXIT_POINT;
					return pos;
					}
					
				escaped = EFalse;
				}
			}
		
		TRACE_EXIT_POINT;	
		return KErrNotFound;	
		}

	/**
	Take a line and extract the name, parameter and value sections.
	@param aLine A line of text in which to find the data.
	@param aName A TPtrC referencing the section of aLine containing the name.
	@param aParameters A TPtrC referencing the section of aLine containing the
	parameters.
	@param aValues A TPtrC referencing the section of aLine containing the
	values.
	@return An error code: either KErrNone or KErrPropertyHasNoValue - avoid
	leaving so we can try and recover without the overhead of a trap.
	@internalTechnology
	*/
	TInt ExtractSectionsL(const TDesC& aLine, TPtrC& aName, TPtrC& aParameters, TPtrC& aValues)
		{
		TRACE_ENTRY_POINT;
		
		// Find the value delimiter.
		const TInt colon = FindFirstUnquoted(aLine, KICalColonChar);

		// Vague check that the property has a value.
		if ((colon == KErrNotFound) || (colon == (aLine.Length() - 1)))
			{
			TRACE_EXIT_POINT;
			return KErrPropertyHasNoValue;	// No value given - the property is invalid.
			}

		// Find the first parameter delimiter.
		const TInt firstSemiColon = aLine.Locate(KICalSemiColonChar);
			
		// Find the characters dividing each section:
		TUint endOfName(colon);

		if ((firstSemiColon != KErrNotFound) && (firstSemiColon < colon))
			{
			endOfName = firstSemiColon;
			}
		
		// Set the name, parameters and values:
		aName.Set(aLine.Left(endOfName));
	
		if (endOfName < colon)
			{
			const TInt startOfParameters = endOfName + 1;
			aParameters.Set(aLine.Mid(startOfParameters, colon - startOfParameters));
			}
		else
			{
			aParameters.Set(aLine.Left(0));	// No parameters.
			}
		
		aValues.Set(aLine.Mid(colon + 1));
		
		TRACE_EXIT_POINT;
		return KErrNone;
		}

	/**
	Takes two strings, representing the parameters and values of a property, and
	the property itself, and populates the latter with the contents of the first
	two.
	@param aParameters A string of parameters.
	@param aValues A string of values.
	@param aProperty A property to add the parameters and values to.
	@internalTechnology
	*/
	void ExtractPropertyL(const TDesC& aParameters, const TDesC& aValues, CICalProperty& aProperty)
		{
		TRACE_ENTRY_POINT;
		
		ExtractParametersL(aParameters, aProperty);
		ExtractPropertyValuesL(aValues, aProperty);
		
		TRACE_EXIT_POINT;
		}

	/**
	Takes a string representing the parameters of a property, and the property
	itself, and populates the property with the contents of the string.
	@param aParameters A string of parameters.
	@param aProperty A property to add the parameters and values to.
	@internalTechnology
	*/
	void ExtractParametersL(const TDesC& aParameters, CICalProperty& aProperty)
		{
		TRACE_ENTRY_POINT;
		
		TPtrC remainder(aParameters);
		
		while (remainder.Length() > 0)
			{
			TInt endOfParameter(FindFirstUnquoted(remainder, KICalSemiColonChar));
			
			if (endOfParameter == KErrNotFound)
				{
				endOfParameter = remainder.Length();
				}
			
			CICalPropertyParam& param = aProperty.AddPropertyParamL();
			ExtractParameterL(remainder.Left(endOfParameter), param);
			
			// Remove the parameter if it has no value.
			if (param.Values().Count() == 0)
				{
				aProperty.RemovePropertyParamL(param);
				}

			if (endOfParameter < remainder.Length())
				{
				remainder.Set(remainder.Mid(endOfParameter + 1));
				}
			else
				{
				break;
				}
			}
		TRACE_EXIT_POINT;
		}

	/**
	Takes a string representing a single parameter, and a class to store it in.
	@param aParameter A string of the form <parameter>[=<value>[,value]*].
	@param aPropertyParam A property parameter to store the data in.
	@internalTechnology
	*/
	void ExtractParameterL(const TDesC& aParameter, CICalPropertyParam& aPropertyParam)
		{
		TRACE_ENTRY_POINT;
		
		TInt firstEquals(aParameter.Locate(KICalEqualsChar));
		TUint endOfType(aParameter.Length());
		
		if (firstEquals != KErrNotFound)
			{
			endOfType = firstEquals;
			}
		
		aPropertyParam.SetTypeL(aParameter.Left(endOfType));
		
		if (endOfType < aParameter.Length())
			{
			ExtractParameterValuesL(aParameter.Mid(endOfType + 1), aPropertyParam);
			}
		TRACE_EXIT_POINT;
		}

	/**
	Takes a string of comma separated values and adds them to a parameter.
	@param aValues A string of the form <value>[,<value>]*.
	@param aPropertyParam A property parameter to store the values in.
	@internalTechnology
	*/
	void ExtractParameterValuesL(const TDesC& aValues, CICalPropertyParam& aPropertyParam)
		{
		TRACE_ENTRY_POINT;
		
		TPtrC remainder(aValues);
		
		while (remainder.Length() > 0)
			{
			TInt endOfValue(FindFirstUnquoted(remainder, KICalCommaChar));
			
			if (endOfValue == KErrNotFound)
				{
				endOfValue = remainder.Length();
				}
			
			// Add value.
			if (endOfValue > 0)
				{
				aPropertyParam.AddValueL(remainder.Left(endOfValue));
				}
			
			if (endOfValue < remainder.Length())
				{
				remainder.Set(remainder.Mid(endOfValue + 1));
				}
			else
				{
				break;
				}
			}
		TRACE_EXIT_POINT;
		}

	/**
	Takes a string of comma separated values and adds them to a property.
	@param aValues A string of the form <value>[,<value>]*.
	@param aProperty A property to store the values in.
	@param aEscaping Used for dealing with incorrect output from other applications.
	@leave Leaves with KErrUnknown if the value of aEscaping is not known.
	@internalTechnology
	*/
	void ExtractPropertyValuesL(const TDesC& aValues, CICalProperty& aProperty, TEscaping aEscaping)
		{
		TRACE_ENTRY_POINT;
		
		TPtrC remainder(aValues);
		TBool wasEscaped(EFalse);
		
		while (remainder.Length() > 0)
			{
			TInt endOfValue(KErrNotFound);
			
			switch (aEscaping)
				{
				case EEscapeNormal:
					endOfValue = FindFirstUnescaped(remainder, KICalCommaChar);
					break;
				case EEscapeValueSeparators:
					endOfValue = FindFirstAndUnescape(remainder, KICalCommaChar, wasEscaped);
					break;
				default:
					User::Leave(KErrUnknown);
					break;
				}
			
			if (endOfValue == KErrNotFound)
				{
				endOfValue = remainder.Length();
				}
			
			// Add value.
			if (endOfValue > 0)
				{
				aProperty.AddValueL(remainder.Left(endOfValue - (wasEscaped ? 1 : 0)));
				}
			
			if (endOfValue < remainder.Length())
				{
				remainder.Set(remainder.Mid(endOfValue + 1));
				}
			else
				{
				break;
				}
			}
		TRACE_EXIT_POINT;
		}
		
	/**
	Determines if the given property has a type which holds a text value.
	@param aProperty The property to test.
	@return ETrue if the property value is a "text" type.
	@internalTechnology
	*/
	TBool PropertyHasTextValueL(const CICalProperty& aProperty)
		{
		TRACE_ENTRY_POINT;
		
		const TDesC& type = aProperty.Type();
		
		// First check if it's an X-type.
		if ((type.Length() >= 2) && (type.Left(2).CompareF(KICalXProperty) == 0))
			{
			TRACE_EXIT_POINT;
			return ETrue;
			}

		// Else return ETrue if the property is PRODID, UID, CATEGORIES, COMMENT, CONTACT,
		// DESCRIPTION, LOCATION, RELATED-TO, RESOURCES, SUMMARY, TZID or TZNAME.
		TRACE_EXIT_POINT;
		return ((type.CompareF(KICalProdId) == 0) || (type.CompareF(KICalUid) == 0) ||
			(type.CompareF(KICalCategories) == 0) || (type.CompareF(KICalComment) == 0) ||
			(type.CompareF(KICalContact) == 0) || (type.CompareF(KICalDescription) == 0) || 
			(type.CompareF(KICalLocation) == 0) || (type.CompareF(KICalRelatedto) == 0) || 
			(type.CompareF(KICalResources) == 0) || (type.CompareF(KICalSummary) == 0) || 
			(type.CompareF(KICalTzid) == 0) || (type.CompareF(KICalTzname) == 0));
		}
	}

// End of File
