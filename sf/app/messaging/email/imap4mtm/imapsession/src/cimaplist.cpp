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


#include "cimaplist.h"
#include "cimapsessionconsts.h"

// IMAP LIST command
_LIT8(KTxtCommandFormatList, "%d LIST %S %S\r\n");

CImapList* CImapList::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aReferenceName, const TDesC& aMailboxName, RArrayImapListFolderInfo& aFolderList, const CImapSettings& aImapSettings)
	{
	CImapList* self = new (ELeave) CImapList(aSelectedFolderData, aLogId, aFolderList, aImapSettings);
	CleanupStack::PushL(self);
	self->ConstructL(aReferenceName, aMailboxName);
	CleanupStack::Pop();
	return self;		
	}	

CImapList::CImapList(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArrayImapListFolderInfo& aFolderList, const CImapSettings& aImapSettings) 
	: CImapListBase(aSelectedFolderData, aLogId, aFolderList, aImapSettings, KTxtCommandFormatList, KImapTxtList)
	{}
