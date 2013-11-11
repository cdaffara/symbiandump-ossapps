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


#ifndef __CIMAPEXAMINE_H__
#define __CIMAPEXAMINE_H__

#include <e32std.h>
#include "cimapcommand.h"
#include "cimapselect.h"

/**
The CImapExamine class encapsulates the sending of the IMAP EXAMINE
command to the remote server. It parses the response data from
the remote server.
@internalTechnology
@prototype
*/
class CImapExamine : public CImapSelect
	{
public:
	// Construction and Destruction
	static CImapExamine* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	~CImapExamine();
	
private:
	CImapExamine(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	};
			 
#endif // __CIMAPEXAMINE_H__
