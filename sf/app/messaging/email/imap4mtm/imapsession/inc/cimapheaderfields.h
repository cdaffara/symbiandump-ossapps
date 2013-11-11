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

#ifndef __CIMAPHEADERFIELDS_H__
#define __CIMAPHEADERFIELDS_H__

#include <e32base.h>

/**
Base class that is used by the RFC822 and MIME header fields parsers to store
header field data. The data is stored as a simple array of heap descriptors,
one for each field type.
@internalTechnology
@prototype
*/
NONSHARABLE_CLASS(CImapHeaderFields) : public CBase
	{
public:
	virtual ~CImapHeaderFields();

	virtual TBool SetFieldL(const TDesC8& aFieldName, HBufC8* aValue);

protected:
	CImapHeaderFields();
	void ConstructL(TInt aStartFieldCount);

	const TDesC8& FieldValue(TInt aFieldId);
	TBool FieldExists(TInt aFieldId);
	HBufC8* StripSpacesL(const TDesC8& aBuffer);

private:
	virtual TInt Match(const TDesC8& aFieldName, TBool& aNeedToStripSpaces) = 0;

	RPointerArray<HBufC8> iImapFieldValueArray;
	};

#endif // __CIMAPHEADERFIELDS_H__
