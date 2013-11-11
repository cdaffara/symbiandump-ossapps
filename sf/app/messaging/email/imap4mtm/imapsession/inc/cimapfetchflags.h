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

#ifndef __CIMAPFETCHFLAGS_H__
#define __CIMAPFETCHFLAGS_H__

#include <e32std.h>
#include "cimapcommand.h"
#include "tmessageflaginfo.h"

/**
Sends a FETCH FLAGS command to the IMAP server and processes the response.
This is the base class for CImapStore, which can also receive FETCH FLAGS responses.
@internalTechnology
@prototype
*/
class CImapFetchFlags : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapFetchFlags* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC8& aSequenceSet, RArrayMessageFlagInfo& aOutMessageFlagInfo);
	~CImapFetchFlags();
	
protected:
	CImapFetchFlags(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArrayMessageFlagInfo& aOutMessageFlagInfo);
	void ConstructL(const TDesC8& aSequenceSet);

private:
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	TParseBlockResult ParseUntaggedResponseL();

	void ProcessFetchFlagsResponseL();

protected:
	// data for send
	HBufC8* iSequenceSet;
	
private:
	// data for receive
	RArrayMessageFlagInfo& iOutMessageFlagInfo;
	};
			 
#endif // __CIMAPFETCHFLAGS_H__
