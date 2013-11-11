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

#ifndef __CIMAPSERVEREVENTWAIT_H__
#define __CIMAPSERVEREVENTWAIT_H__

#include <e32std.h>

#include "cimapcommand.h"

/**
This class wait for and handles unsolicited untagged responses from the server.
See section 5.3 of RFC3501.
The base class handles receipt of untagged responses.
However in our constructor, we set iCompleteOnAnyResponse to ETrue, causing the command 
to complete upon receipt of any untagged response.
@internalTechnology
@prototype
*/
class CImapServerEventWait : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapServerEventWait* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	~CImapServerEventWait();
			
private:
	CImapServerEventWait(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();
	
	// Implements CImapCommand
	TBool CanCompleteFlushNow() const;
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	TBool ParseTaggedResponseL(TInt aTagId);
	};
			 
#endif // __CIMAPSERVEREVENTWAIT_H__
