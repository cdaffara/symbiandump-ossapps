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

#ifndef __CIMAPLSUB_H__
#define __CIMAPLSUB_H__

#include <e32std.h>
#include "cimaplistbase.h"

/**
Implements the sending, receiving and parsing of the IMAP LSUB command.
@internalTechnology
@prototype
*/
class CImapLsub : public CImapListBase
	{
public:
	static CImapLsub* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aReferenceName, const TDesC& aMailboxName, RArrayImapListFolderInfo& aFolderList, const CImapSettings& aImapSettings);
	
private:
	CImapLsub(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArrayImapListFolderInfo& aFolderList, const CImapSettings& aImapSettings);
	};

#endif // __CIMAPLSUB_H__
