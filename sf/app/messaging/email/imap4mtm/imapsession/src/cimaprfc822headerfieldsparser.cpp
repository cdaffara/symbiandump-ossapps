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

#include "cimaprfc822headerfieldsparser.h"
#include "cimapfetchresponse.h"
#include "cimaprfc822headerfields.h"

CImapRfc822HeaderFieldsParser* CImapRfc822HeaderFieldsParser::NewL(CImapFetchResponse& aFetchResponse, TInt aLogId)
	{
	CImapRfc822HeaderFieldsParser* self = NewLC(aFetchResponse, aLogId);
	CleanupStack::Pop();
	return self;
	}

CImapRfc822HeaderFieldsParser* CImapRfc822HeaderFieldsParser::NewLC(CImapFetchResponse& aFetchResponse, TInt aLogId)
	{
	CImapRfc822HeaderFieldsParser* self = new (ELeave) CImapRfc822HeaderFieldsParser(aFetchResponse, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CImapRfc822HeaderFieldsParser::CImapRfc822HeaderFieldsParser(CImapFetchResponse& aFetchResponse, TInt aLogId) :
	CImapHeaderFieldsParser(aLogId),
	iFetchResponse(aFetchResponse)
	{
	}

void CImapRfc822HeaderFieldsParser::ConstructL()
	{
	CImapRfc822HeaderFields* headerFields = CImapRfc822HeaderFields::NewL();

	// Pass ownership of headerFields to base class
	CImapHeaderFieldsParser::Construct(headerFields);
	}

CImapRfc822HeaderFieldsParser::~CImapRfc822HeaderFieldsParser()
	{
	}

void CImapRfc822HeaderFieldsParser::HeaderFieldsComplete(CImapHeaderFields* aHeaderFields)
	{
	iFetchResponse.SetHeaderFields(static_cast<CImapRfc822HeaderFields*> (aHeaderFields));
	}
