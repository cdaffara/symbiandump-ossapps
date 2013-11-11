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

#ifndef __POPSDELE_H__
#define __POPSDELE_H__

#include <pop3set.h>
#include <mentact.h>
#include <popsmtm.h>

class CImPop3Session;
class CImPop3Dele;

//
// CImPop3RefreshOperation
// Pop3 delete from mailbox
// 
//
//
class CImPop3Delete : public  CMsgActive
	{
public:
	static CImPop3Delete *NewL(CMsvServerEntry&, const CMsvEntrySelection&, CImPop3Session*, TMsvId aServiceId);
	~CImPop3Delete();

	void Start(TRequestStatus& aStatus);
	TPop3Progress Progress();
	void Pause();
	void ResumeL(CImPop3Session* aPopSession, TRequestStatus& aStatus);
	void CancelAllowResume();
	
private:
	CImPop3Delete(CMsvServerEntry&, CImPop3Session*, TMsvId aServiceId);
	void ConstructL(const CMsvEntrySelection&);

	void DoDelete();

	void DoRunL();
	void DoCancel();

private:
	CMsvEntrySelection*				iSource;
	CMsvServerEntry&				iRemoteEntry;
	CImPop3Session*					iPopSession;
	CImPop3Dele*					iPopDelete;

	TPop3Progress					iProgress;
	TInt							iMsgCtr;
	TBool							iMigratingToNewBearer;
	TBool							iCancelToMigrate;
	TMsvId iServiceId;
	};

#endif
