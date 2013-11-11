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

#ifndef __CIMAPCOPY_H__
#define __CIMAPCOPY_H__

#include <e32std.h>

#include "cimapcommand.h"

/**
@internalTechnology
@prototype
*/
class CImapCopy : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapCopy* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC8& aSequenceSet, const TDesC& aMailboxName);
	~CImapCopy();
	
private:
	CImapCopy(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL(const TDesC8& aSequenceSet, const TDesC& aMailboxName);

	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);

private:
	// data for send
	HBufC8* iSequenceSet;
	HBufC8* iMailboxName;
	};
			 
#endif // __CIMAPCOPY_H__
