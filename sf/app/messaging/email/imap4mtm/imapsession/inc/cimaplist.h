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

#ifndef __CIMAPLIST_H__
#define __CIMAPLIST_H__

#include <e32std.h>
#include "cimaplistbase.h"

/**
Implements the sending, receiving and parsing of the IMAP LIST command.
@internalTechnology
@prototype
*/
class CImapList : public CImapListBase
	{
public:
	static CImapList* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aReferenceName, const TDesC& aMailboxName, RArrayImapListFolderInfo& aFolderList, const CImapSettings& aImapSettings);
	
private:
	CImapList(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArrayImapListFolderInfo& aFolderList, const CImapSettings& aImapSettings);
	};

#endif // __CIMAPLIST_H__
