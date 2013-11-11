// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __VERSIT_INTERNAL_H__
#define __VERSIT_INTERNAL_H__
// Panic enumeration
/** Versit panic numbers. The Versit panic numbers have a category of "Versit-Parser". 
@internalComponent
@released
*/
enum TVersitParserPanic
	{
	ECurrentTokenNotFound,		//Unused
	EUnicodeUtilsNotFound,		//Unused
	ENoPropertyValue,		//Unused
	/** A CWeekDayArray contains an invalid value (not between EMonday 
	and ESunday inclusive). */
	ENoWeekdayFound,
	/** The number of a week within a month is invalid. */
	EWeekNoOverTen,
	/** The repeat type for a recurrence rule property value is not one of the values 
	specified in CVersitRecurrence::TType. */
	ENoRecognizedRepeatType,
	EVersitPanicEscapedTextAlreadyExists,		//Unused
	/** 8-bit encoding is proposed as the default for a parser but may not be appropriate. */
	EVersitPanicCannotSetEightBitEncoding,
	/** 8-bit encoding is encountered or proposed where it is not expected. */
	EVersitPanicUnexpectedEightBitEncoding,
	/** A parser was not specified when externalising a property. */
	EVersitPanicNeedToSpecifyParser,
	/** The additional storage slot for the given property has already been used */
	EVersitPanicAdditionalStorageSlotAlreadyInUse,
	/** Attempting to assign a NULL value to an additional storage slot  */
	EVersitPanicNullValueAssignedToAdditionalStorageSlot,
	//
	EVersitPanicLast
	};

/**
Used as key into additional storage within tls object, for CVersitTLSContainer
Value should not conflict with genuine compiler generated pointer values
@internalComponent
@released
*/
const static TInt* const KTLSVars = reinterpret_cast<TInt*>(1);

/**
@internalComponent
@released
*/
GLREF_C void DestroyHBufC(TAny* aHBufC);

/** Versit parser panic
@internalComponent
@released
*/
IMPORT_C void Panic(TVersitParserPanic aPanic);

#endif //__VERSIT_INTERNAL_H__
