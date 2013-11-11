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

#ifndef __CIMAPNOOP_H__
#define __CIMAPNOOP_H__

#include <e32std.h>
#include "cimapcommand.h"
#include "cimapfolderinfo.h"

/**
The CImapNoop class encapsulates the sending of the IMAP NOOP
command to the remote server. It parses the response data from
the remote server.
@internalTechnology
@prototype
*/
class CImapNoop : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapNoop* NewL(CImapFolderInfo* aFolderInfo, TInt aLogId);
	~CImapNoop();
	
private:
	CImapNoop(CImapFolderInfo* aFolderInfo, TInt aLogId);
	void ConstructL();
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	};
			 
#endif // __CIMAPNOOP_H__
