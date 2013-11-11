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
// popservermtm module header
// @internalComponent
// @released
// 
//

#if !defined(_POPSTRAN_H__)
#define _POPSTRAN_H__

#include <e32std.h>
#include <e32base.h>
#include <msvstd.h>

class CMsvServerEntry;

//
// CImPop3TransferMessage
//
class CImPop3TransferMessage : public CActive
	{
public:
	enum TImPop3TransferMethod
		{
		EImPop3CopyTransfer,
		EImPop3MoveTransfer
		};

	static CImPop3TransferMessage* NewL(CMsvServerEntry& aEntry);
	void StartL(TMsvId aSourceMessage, TMsvId aDestFolder, TImPop3TransferMethod aTransferMethod, TRequestStatus& aStatus);
	
	void DoCancel();
	void RunL();

	~CImPop3TransferMessage();

private:
	CImPop3TransferMessage(CMsvServerEntry& aEntry);

	TImPop3TransferMethod	iTransferMethod;
	TRequestStatus*			iReportStatus;
	CMsvServerEntry&		iEntry;
	TMsvId					iSourceMessage;
	TMsvId					iDestFolder;
	TMsvId					iCopiedMessageId;
	};

#endif
