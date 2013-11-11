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

#include "cimapfetchsinglebodystructure.h"
#include "moutputstream.h"

CImapFetchSingleBodyStructure* CImapFetchSingleBodyStructure::NewL(CImapFolderInfo* aSelectedFolderInfo, TInt aLogId, TInt aMsgUid, const TDesC8& aHeaderFields, CImapFetchResponse& aFetchResponse)
	{		
	CImapFetchSingleBodyStructure* self = new (ELeave) CImapFetchSingleBodyStructure(aSelectedFolderInfo, aLogId, aHeaderFields, aFetchResponse);
	CleanupStack::PushL(self);
	self->ConstructL(aMsgUid);
	CleanupStack::Pop();
	return self;
	}

CImapFetchSingleBodyStructure::CImapFetchSingleBodyStructure(CImapFolderInfo* aSelectedFolderInfo, TInt aLogId, const TDesC8& aHeaderFields, CImapFetchResponse& aFetchResponse)
	: CImapFetchBodyStructureBase(aSelectedFolderInfo, aLogId, aHeaderFields)
	{
	iFetchResponse = &aFetchResponse;
	}

void CImapFetchSingleBodyStructure::ConstructL(TInt aMsgUid)
	{
	iUidBuf = HBufC8::NewL(32);
	iUidBuf->Des().Num(aMsgUid);
	iSequenceSet = &(*iUidBuf);
	}

CImapFetchSingleBodyStructure::~CImapFetchSingleBodyStructure()
	{
	delete iUidBuf;
	}
