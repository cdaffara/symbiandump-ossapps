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

#ifndef __CIMAPMIMEHEADERFIELDS_H__
#define __CIMAPMIMEHEADERFIELDS_H__

#include "cimapheaderfields.h"

/**
Stores the MIME header information that is returned by a BODY[X.MIME] request.
The data is stored as a simple array of heap descriptors, one for each field type.
@internalTechnology
@prototype
*/
NONSHARABLE_CLASS(CImapMimeHeaderFields) : public CImapHeaderFields
	{
public:
	/**
	Provides an index into the field value array
	*/
	enum THeaderFields
		{
		EImapContentBase,
		EImapContentLocation,

		EImapCountFields
		};

	static CImapMimeHeaderFields* NewL();
	~CImapMimeHeaderFields();

	IMPORT_C const TDesC8& FieldValue(THeaderFields aFieldId);
	IMPORT_C TBool FieldExists(THeaderFields aFieldId);

	TBool SetFieldL(const TDesC8& aFieldName, HBufC8* aValue);
	void RestartGetNextField();
	TBool GetNextField(TPtrC8& aFieldName, TPtrC8& aValue);

private:
	CImapMimeHeaderFields();
	void ConstructL();
	TInt Match(const TDesC8& aFieldName, TBool& aNeedToStripSpaces);
	const TDesC8& FieldNameFromId(THeaderFields aFieldId);

private:
	RPointerArray<HBufC8> iImapUnmatchedNameArray;
	RPointerArray<HBufC8> iImapUnmatchedValueArray;
	TInt iGetNextFieldPos;
	};

#endif // __CIMAPMIMEHEADERFIELDS_H__
