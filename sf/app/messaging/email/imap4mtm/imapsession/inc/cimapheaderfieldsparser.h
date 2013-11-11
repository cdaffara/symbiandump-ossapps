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

#ifndef __CIMAPHEADERFIELDSPARSER_H__
#define __CIMAPHEADERFIELDSPARSER_H__

#include <e32base.h>

class CImapHeaderFields;

/**
@internalTechnology
@prototype
*/
NONSHARABLE_CLASS(CImapHeaderFieldsParser) : public CBase
	{
public:
	virtual ~CImapHeaderFieldsParser();

	TBool ProcessBlockL(const TDesC8& aData);
	TPtrC8 UnparsedData();

protected:
	CImapHeaderFieldsParser(TInt aLogId);
	void Construct(CImapHeaderFields* aHeaderFields);

private:
	void ParseHeaderFieldsBlockL(const TDesC8& aData);
	void ParseFieldsL();
	void ParsePartsL(RArray<TPtrC8>& aParts, TInt& aLength);
	virtual void HeaderFieldsComplete(CImapHeaderFields* aHeaderFields) = 0;

private:
	enum TState 
		{
		EWaitLine,
		EWaitLiteral,
		EParseComplete
		} iState;

	CImapHeaderFields* iHeaderFields;
	TBool iHeaderFieldsOwned;

	TPtrC8 iData;
	TInt iLogId;
	};

#endif // __CIMAPHEADERFIELDSPARSER_H__
