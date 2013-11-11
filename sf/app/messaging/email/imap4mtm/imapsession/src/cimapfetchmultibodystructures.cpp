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

#include "cimapfetchmultibodystructures.h"
#include "cimapfetchresponse.h"
#include "moutputstream.h"

CImapFetchMultiBodyStructures* CImapFetchMultiBodyStructures::NewL(CImapFolderInfo* aSelectedFolderInfo, TInt aLogId, const TDesC8& aSequenceSet, const TDesC8& aHeaderFields, MImapFetchStructureObserver& aObserver)
	{
	CImapFetchMultiBodyStructures* self = new (ELeave) CImapFetchMultiBodyStructures(aSelectedFolderInfo, aLogId, aSequenceSet, aHeaderFields, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CImapFetchMultiBodyStructures::CImapFetchMultiBodyStructures(CImapFolderInfo* aSelectedFolderInfo, TInt aLogId, const TDesC8& aSequenceSet, const TDesC8& aHeaderFields, MImapFetchStructureObserver& aObserver)
	: CImapFetchBodyStructureBase(aSelectedFolderInfo, aLogId, aHeaderFields)
	, iObserver(aObserver)
	{
	iSequenceSet = &aSequenceSet;
	}

void CImapFetchMultiBodyStructures::ConstructL()
	{
	}

CImapFetchMultiBodyStructures::~CImapFetchMultiBodyStructures()
	{
	delete iFetchResponse;
	}

/**
@param aData Will contain a single line of response from the server for LOGIN command without \r\n.
@return will be any one of this
	1) If the next expected chunk is a literal block, ParseMessageL() will return the size of the block it expects.
	2) If the next expected chunk is a line, ParseMessageL() will return 0, and Result() will return EImapResponseNone.
	3) If no further data is expected (e.g. the OK or error tag has been received) then ParseMessageL() will return 0, 
	   and Result() will return one of EImapResponseOk, EImapResponseNo or EImapResponseBad.
*/
CImapCommand::TParseBlockResult CImapFetchMultiBodyStructures::DoParseBlockL(const TDesC8& aData)
	{
	if (iFetchResponse == NULL)
		{
		iFetchResponse = CImapFetchResponse::NewL();
		}

	CImapCommand::TParseBlockResult resultCode = CImapFetchBodyStructureBase::DoParseBlockL(aData);

	switch (resultCode)
		{
		case ECompleteUntagged:
			{
			if (UidDataItemFoundInResponse())
				{
				// A genuine UID FETCH response was received.
				// Pass ownership of iFetchResponse to the observer
				CImapFetchResponse* fetchResponse = iFetchResponse;
				iFetchResponse = NULL;
				iObserver.OnFetchLD(fetchResponse);
				}
			else
				{
				// An unsolicited FETCH server event was received.
				// Don't pass ownership of the fetch response to the observer, but delete it here instead.
				delete iFetchResponse;
				iFetchResponse = NULL;
				}			
			} 
			break;
		case ECompleteTagged:
			{
			// perform the final commit
			iObserver.OnFetchCommit();
			}
			// fall through
		case ENotRecognised:
			{
			delete iFetchResponse;
			iFetchResponse = NULL;
			}
			break;
		}

	return resultCode;
	}

/**
This is called when the session has processed all the blocks in its input buffer, but the 
command is not yet complete (so it is expecting more data).
Commit any headers that have been created in response to receiving fetch responses.
*/
void CImapFetchMultiBodyStructures::WaitingForMoreDataL()
	{
	iObserver.OnFetchCommit();
	}
