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

#ifndef __CIMAPRFC822HEADERFIELDSPARSER_H__
#define __CIMAPRFC822HEADERFIELDSPARSER_H__

#include <e32base.h>
#include "cimapheaderfieldsparser.h"

class CImapFetchResponse;

/**
@internalTechnology
@prototype
*/
NONSHARABLE_CLASS(CImapRfc822HeaderFieldsParser) : public CImapHeaderFieldsParser
	{
public:
	static CImapRfc822HeaderFieldsParser* NewL(CImapFetchResponse& aFetchResponse, TInt aLogId);
	static CImapRfc822HeaderFieldsParser* NewLC(CImapFetchResponse& aFetchResponse, TInt aLogId);
	~CImapRfc822HeaderFieldsParser();

private:
	CImapRfc822HeaderFieldsParser(CImapFetchResponse& aFetchResponse, TInt aLogId);
	void ConstructL();
	void HeaderFieldsComplete(CImapHeaderFields* aHeaderFields);

	void ParseHeaderFieldsBlockL(const TDesC8& aData);
	void ParseFieldsL();
	void ParsePartsL(RArray<TPtrC8>& aParts, TInt& aLength);

private:
	CImapFetchResponse& iFetchResponse;
	};

#endif // __CIMAPRFC822HEADERFIELDSPARSER_H__
