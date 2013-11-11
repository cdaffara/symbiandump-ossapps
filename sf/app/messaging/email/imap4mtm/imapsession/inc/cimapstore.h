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

#ifndef __CIMAPSTORE_H__
#define __CIMAPSTORE_H__

#include <e32std.h>

#include "cimapfetchflags.h"

/**
The CImapStore class encapsulates the sending of the IMAP STORE
command to the remote server. It parses the response data from
the remote server.
It derives from CImapFetchFlags because it receives exactly the same 
information - i.e. Fetch Flags - and stores in exacely the same way.
@internalTechnology
@prototype
*/
class CImapStore : public CImapFetchFlags
	{
public:
	// Construction and Destruction
	static CImapStore* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC8& aSequenceSet, const TDesC8& aMessageDataItems, const TDesC8& aValue, TBool aUseSilent, RArrayMessageFlagInfo& aOutMessageFlagInfo);
	~CImapStore();
			
private:
	CImapStore(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArrayMessageFlagInfo& aOutMessageFlagInfo);
	void ConstructL(const TDesC8& aSequenceSet, const TDesC8& aMessageDataItems, const TDesC8& aValue, TBool aUseSilent);

	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);

private:
	// data for send
	HBufC8* iMessageDataItems;
	HBufC8* iValue;
	TBool iUseSilent;
	};
		 
#endif // __CIMAPSTORE_H__
