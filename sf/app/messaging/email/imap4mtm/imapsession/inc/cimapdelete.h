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

#ifndef __CIMAPDELETE_H__
#define __CIMAPDELETE_H__

#include <e32std.h>
#include "cimapcommand.h"


/**The CImapDelete class encapsulates the sending of the IMAP DELETE
command to the remote server. It will also parse the response data from
the remote server.
@internalTechnology
@prototype
*/
class CImapDelete : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapDelete* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aMailboxName);
	~CImapDelete();

private:
	CImapDelete(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL(const TDesC& aMailboxName);
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);

private:
	HBufC8* iMailboxName;
	};

#endif // __CIMAPDELETE_H__
