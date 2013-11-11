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

#include "agmutil.h"

_LIT(KAgendaPanicString, "Agenda Model");

const TInt KInitialHashValue = 5381; // start with a big prime number for the hash algorithm  

EXPORT_C void Panic(TAgmPanic aPanic)
/**
@internalAll
*/
	{
	User::Panic(KAgendaPanicString, aPanic);
	}

/** Hash a Guid
Uses the Djb2 Hash Algorithm
@internalAll
@released
*/
EXPORT_C TUint32 GenerateHash8L(const TDesC8& aString)
	{
	__ASSERT_ALWAYS(aString.Length() > 0, User::Leave(KErrArgument));

	// Uses the Djb2 Hash Algorithm

	TUint32 hash(KInitialHashValue); 

	const TInt KLen = aString.Length();

	for (TInt i = 0; i < KLen; ++i)
		{
		hash = ((hash << 5) + hash) + aString[i];
		}

	return (hash);
	}

/** Hash a Guid
Uses the Djb2 Hash Algorithm
@internalAll
@released
*/
EXPORT_C TUint32 FoldAndGenerateHashL(const TDesC& aFileName)
	{
	__ASSERT_ALWAYS(aFileName.Length() > 0, User::Leave(KErrArgument));
	
	// Uses the Djb2 Hash Algorithm

	TUint32 hash(KInitialHashValue);
	
	const TInt KLen = aFileName.Length();

	for (TInt i = 0; i < KLen; ++i)
		{
		TChar thisChar = aFileName[i];
		thisChar.Fold(); // fold as this function only used for file names
		hash = ((hash << 5) + hash) + thisChar;
		}

	return (hash);
	}
