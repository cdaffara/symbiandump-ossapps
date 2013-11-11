// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CLASS0SMSUINOTIFIER_H__
#define __CLASS0SMSUINOTIFIER_H__

#include <eiknotapi.h>
#include <msvapi.h>
#include <ecom/ecom.h>
class CMsvEntrySelection;

const TUint KCountServMajorVersionNumber=0;
const TUint KCountServMinorVersionNumber=1;
const TUint KCountServBuildVersionNumber=1;

//_LIT(KDummyServerName, "DummyServer");
//_LIT(KDummyServerExe, "Dummyserver.exe");



class CClass0SmsUINotifier : /*public CBase, */ public MEikSrvNotifierBase2
	{
public:
	//	Construction
	static CClass0SmsUINotifier* NewL();
	~CClass0SmsUINotifier();
private:
	CClass0SmsUINotifier();
	void ConstructL();

	//	From MEikSrvNotifierBase2
	virtual void Release();
	virtual TNotifierInfo RegisterL();
	virtual TNotifierInfo Info() const;
	virtual TPtrC8 StartL(const TDesC8& aBuffer);
	virtual void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	virtual void Cancel();
	virtual TPtrC8 UpdateL(const TDesC8& aBuffer);
	void CreateFile(const TDesC8& aBuffer);

private:
	TNotifierInfo iInfo;
	RMessagePtr2  iMessage;
	TInt		  iReplySlot;
	TSecurityInfo iSecurityInfo;
	};

#endif	// __CLASS0SMSUINOTIFIER_H__




