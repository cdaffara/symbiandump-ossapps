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

#ifndef __CIMAPCHECK_H__
#define __CIMAPCHECK_H__

#include <e32std.h>
#include "cimapcommand.h"
#include "cimapfolderinfo.h"

/**
@internalTechnology
@prototype
*/
class CImapCheck : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapCheck* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	~CImapCheck();
	
private:
	CImapCheck(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();

	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	};
			 
#endif // __CIMAPCHECK_H__
