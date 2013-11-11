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

#ifndef __CIMAPMIMEHEADERFIELDSPARSER_H__
#define __CIMAPMIMEHEADERFIELDSPARSER_H__

#include <e32base.h>
#include "cimapheaderfieldsparser.h"

class CImapFetchBodyResponse;

/**
@internalTechnology
@prototype
*/
NONSHARABLE_CLASS(CImapMimeHeaderFieldsParser) : public CImapHeaderFieldsParser
	{
public:
	static CImapMimeHeaderFieldsParser* NewL(CImapFetchBodyResponse& aFetchBodyResponse, TInt aLogId);
	static CImapMimeHeaderFieldsParser* NewLC(CImapFetchBodyResponse& aFetchBodyResponse, TInt aLogId);
	~CImapMimeHeaderFieldsParser();

private:
	CImapMimeHeaderFieldsParser(CImapFetchBodyResponse& aFetchBodyResponse, TInt aLogId);
	void ConstructL();
	void HeaderFieldsComplete(CImapHeaderFields* aHeaderFields);

	void ParseHeaderFieldsBlockL(const TDesC8& aData);
	void ParseFieldsL();
	void ParsePartsL(RArray<TPtrC8>& aParts, TInt& aLength);

private:
	CImapFetchBodyResponse& iFetchBodyResponse;
	};

#endif // __CIMAPMIMEHEADERFIELDSPARSER_H__
