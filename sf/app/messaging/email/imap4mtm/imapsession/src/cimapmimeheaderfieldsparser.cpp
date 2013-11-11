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

#include "cimapmimeheaderfieldsparser.h"
#include "cimapfetchbodyresponse.h"
#include "cimapmimeheaderfields.h"

CImapMimeHeaderFieldsParser* CImapMimeHeaderFieldsParser::NewL(CImapFetchBodyResponse& aFetchBodyResponse, TInt aLogId)
	{
	CImapMimeHeaderFieldsParser* self = NewLC(aFetchBodyResponse, aLogId);
	CleanupStack::Pop();
	return self;
	}

CImapMimeHeaderFieldsParser* CImapMimeHeaderFieldsParser::NewLC(CImapFetchBodyResponse& aFetchBodyResponse, TInt aLogId)
	{
	CImapMimeHeaderFieldsParser* self = new (ELeave) CImapMimeHeaderFieldsParser(aFetchBodyResponse, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CImapMimeHeaderFieldsParser::CImapMimeHeaderFieldsParser(CImapFetchBodyResponse& aFetchBodyResponse, TInt aLogId) :
	CImapHeaderFieldsParser(aLogId),
	iFetchBodyResponse(aFetchBodyResponse)
	{
	}

void CImapMimeHeaderFieldsParser::ConstructL()
	{
	CImapMimeHeaderFields* headerFields = CImapMimeHeaderFields::NewL();

	// Pass ownership of headerFields to base class
	CImapHeaderFieldsParser::Construct(headerFields);
	}

CImapMimeHeaderFieldsParser::~CImapMimeHeaderFieldsParser()
	{
	}

void CImapMimeHeaderFieldsParser::HeaderFieldsComplete(CImapHeaderFields* aHeaderFields)
	{
	iFetchBodyResponse.SetHeaderFields(static_cast<CImapMimeHeaderFields*> (aHeaderFields));
	}
