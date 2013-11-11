// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SMUTMAIN_H__
#define __SMUTMAIN_H__

#include <e32base.h>

/**
@deprecated
@internalComponent
*/
const TInt KSmscSessionPriority = CActive::EPriorityStandard;

/**
@deprecated
@internalComponent
*/
_LIT(KSmcmDllName,"SMCM");

/**
The panic codes used by the SMS client MTM.

@internalComponent
@released
*/
enum TSmcmPanic
	{
	ESmutPanicUnsupportedMsgType		= 0,
	ESmscEntryNotSet					= 1,
	ESmscWrongContextType				= 2,
	ESmscPanicUnexpectedCommand			= 3,
	ESmscUnrecognizedEntryType			= 4,
	ESmscAddresseeIndexOutOfRange		= 5,
	ESmscRecpAddresseeMiscount			= 6,
	ESmscDefaultSCOutOfRange			= 7,
	ESmscVPOutOfRange					= 8,
/**
@deprecated	Currently not used.
*/
	ESmscRetryLimitOutOfRange			= 9,
	ESmscBioMsgIdTypeError				= 10,
	ESmscSettingsNotSet					= 11,
	ESmscServiceIdNotSet				= 12,
/**
@deprecated	Currently not used.
*/
	ESmscSimParamWrongType				= 13,
/**
@deprecated	Currently not used.
*/
	ESmscSimParamExistingOperation		= 14,
/**
@deprecated	Currently not used.
*/
	ESmscSimParamInvalidProgressCount	= 15,
/**
@deprecated	Currently not used.
*/
	ESmscWrongCommDbAction				= 16,

	ESmscFunctionNotSupported			= 17
	};

/**
Panic function.

@param	aPanic	The panic code.

@internalComponent
@released
*/
inline void Panic(TSmcmPanic aPanic)
	{
	_LIT(KSmcmPanic, "SMCM");
	User::Panic(KSmcmPanic, aPanic);
	}

#endif	// __SMUTMAIN_H__

