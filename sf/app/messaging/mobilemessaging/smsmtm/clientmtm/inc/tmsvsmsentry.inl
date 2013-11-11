// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// tmsvsmsentry.inl
//

#include <smcmmain.h>
#include <gsmumsg.h>

/*
 *	TMsvSmsEntry
 */

/**
Constructor.

@internalTechnology
@released
*/
inline TMsvSmsEntry::TMsvSmsEntry()
	{
	}

/**
Copy constuctor.

@param	aEntry
The index entry to copy.

@internalTechnology
@released
*/
inline TMsvSmsEntry::TMsvSmsEntry(const TMsvEntry& aEntry)  
: TMsvEntry(aEntry)
	{
	}

/**
Sets the User Prompt Indicator flag.

@param	aUPI
A value of True to set the flag.

@internalTechnology
@released
*/
inline void TMsvSmsEntry::SetUserPromptIndicator(TBool aUPI)
	{
	(aUPI) ? (iMtmData2 |= EMsvSmsEntryUserPromptIndicator) : (iMtmData2 &= ~EMsvSmsEntryUserPromptIndicator);
	}

/**
Gets the User Prompt Indicator flag.

@return
A value of True if the User Prompt Indicator flag is.

@internalTechnology
@released
*/
inline TBool TMsvSmsEntry::UserPromptIndicator() const
	{
	return (iMtmData2 & EMsvSmsEntryUserPromptIndicator);
	}

/**
Gets the Protocol Identifier data.

@return
The Protocol Identifier data.

@internalTechnology
@released
*/
inline TUint8 TMsvSmsEntry::ProtocolIdentifier() const
	{
	return STATIC_CAST(TUint8, iMtmData2 & EMsvSmsEntryProtocolIdentifier);
	}

/**
Sets the Protocol Identifier data.

@param	aPID
The Protocol Identifier data.

@internalTechnology
@released
*/
inline void TMsvSmsEntry::SetProtocolIdentifier(TSmsProtocolIdentifier aPID)
	{
	SetProtocolIdentifier(STATIC_CAST(TUint8, aPID));
	}

/**
Sets the Protocol Identifier data.

@param	aPID
The Protocol Identifier data.

@internalTechnology
@released
*/
inline void TMsvSmsEntry::SetProtocolIdentifier(TUint8 aPID)
	{
	iMtmData2 = (iMtmData2 & ~EMsvSmsEntryProtocolIdentifier) | (aPID & EMsvSmsEntryProtocolIdentifier);
	}

/**
Sets the SMS class data and defined flag.

@param	aClassDefined
A value of True to set the SMS class data defined flag.

@param	aClass
The SMS class data.

@internalTechnology
@released
*/
inline void TMsvSmsEntry::SetClass(TBool aClassDefined, TSmsDataCodingScheme::TSmsClass aClass)
	{
	aClassDefined ? (iMtmData2 |= EMsvSmsEntryClassDefined) : (iMtmData2 &= ~EMsvSmsEntryClassDefined);
	iMtmData2 = (iMtmData2 & ~EMsvSmsEntryClass) | ((aClass << EMsvSmsEntryClassShift) & EMsvSmsEntryClass);
	}

/**
Gets the SMS class data and defined flag.

@param	aClass
An output argument with the SMS class data.

@return
A value of True if the SMS class data defined flag is set.

@internalTechnology
@released
*/
inline TBool TMsvSmsEntry::Class(TSmsDataCodingScheme::TSmsClass& aClass) const
	{
	aClass = STATIC_CAST(TSmsDataCodingScheme::TSmsClass, ((iMtmData2 & EMsvSmsEntryClass) >> EMsvSmsEntryClassShift));
	return iMtmData2 & EMsvSmsEntryClassDefined;
	}


