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

#include "cimaplsub.h"
#include "cimapsessionconsts.h"

// IMAP LSUB command
_LIT8(KTxtCommandFormatLSub, "%d LSUB %S %S\r\n");

CImapLsub* CImapLsub::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aReferenceName, const TDesC& aMailboxName, RArrayImapListFolderInfo& aFolderList, const CImapSettings& aImapSettings)
	{
	CImapLsub* self = new (ELeave) CImapLsub(aSelectedFolderData, aLogId, aFolderList, aImapSettings);
	CleanupStack::PushL(self);
	self->ConstructL(aReferenceName, aMailboxName);
	CleanupStack::Pop();
	return self;		
	}	

CImapLsub::CImapLsub(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArrayImapListFolderInfo& aFolderList, const CImapSettings& aImapSettings) 
	: CImapListBase(aSelectedFolderData, aLogId, aFolderList, aImapSettings, KTxtCommandFormatLSub, KImapTxtLSub)
	{}
