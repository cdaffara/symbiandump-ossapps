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

#ifndef __CIMAPLOGOUT_H__
#define __CIMAPLOGOUT_H__

#include <e32std.h>

#include "cimapcommand.h"


/**
The CImapLogin class encapsulates the sending of the IMAP LOGOUT
command to the remote server.
The class will parse the response data from the remote server.
All untagged responses are handled by the base class.
In particular, the untagged "BYE" response is handled by the base class, 
as the untagged "BYE" response can be received at any time, not just during LOGOUT.
@internalTechnology
@prototype
*/
class CImapLogout : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapLogout* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	~CImapLogout();
	
private:
	CImapLogout(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	};
			 
#endif // __CIMAPLOGOUT_H__
