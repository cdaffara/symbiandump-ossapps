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

#ifndef __CIMAPCLOSE_H__
#define __CIMAPCLOSE_H__

#include <e32std.h>
#include "cimapcommand.h"

/**
The CImapClose class encapsulates the sending of the IMAP CLOSE
command to the remote server. It will also parse the response data from
the remote server.
@internalTechnology
@prototype
*/
class CImapClose : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapClose* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId); 
	~CImapClose();

private:
	CImapClose(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	};

#endif // __CIMAPCLOSE_H__
