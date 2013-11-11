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

#ifndef __CIMAPEXPUNGE_H__
#define __CIMAPEXPUNGE_H__

#include <e32std.h>
#include "cimapcommand.h"

/**
The CImapExpunge class encapsulates the sending of the IMAP EXPUNGE
command to the remote server. It parses the response data from
the remote server.
Note that EXPUNGE responses are likely to be generated, in response to an EXPUNGE command, 
but they are handled by the base class - so there is no override for ParseUntaggedResponseL()
@internalTechnology
@prototype
*/
class CImapExpunge : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapExpunge* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	~CImapExpunge();

private:
	CImapExpunge(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	};

#endif // __CIMAPEXPUNGE_H__
