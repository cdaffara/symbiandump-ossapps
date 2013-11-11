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


#ifndef __CIMAPSEARCH_H__
#define __CIMAPSEARCH_H__

#include <e32std.h>
#include "cimapcommand.h"


/**
The CImapSearch class encapsulates the sending of the IMAP SEARCH
command to the remote server. It will also parse the response data from
the remote server.
@internalTechnology
@prototype
*/
class CImapSearch : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapSearch* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC8& aSearchCriteria, RArray<TUint>& aMatchingMessageIds);
	~CImapSearch();
		
private:
	CImapSearch(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArray<TUint>& aMatchingMessageIds);
	void ConstructL(const TDesC8& aSearchCriteria);

	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	TParseBlockResult ParseUntaggedResponseL();

	void ParseSearchResponseL();

private:
	// data for send
	HBufC8* iSearchCriteria;
	
	// data for receive
	RArray<TUint>& iMatchingMessageIds;
	};
			 
#endif // __CIMAPSEARCH_H__
