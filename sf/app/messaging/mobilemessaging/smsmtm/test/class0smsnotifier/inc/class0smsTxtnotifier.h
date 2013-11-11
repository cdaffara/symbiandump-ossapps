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
// class0smsnotifier.h
// 
//

#ifndef __CLASS0SMSNOTIFIER_H__
#define __CLASS0SMSNOTIFIER_H__

#include <twintnotifier.h>
#include <msvapi.h>



class CClass0SmsTxtNotifier : /* public CBase, */ public MNotifierBase2
	{
public:
	//	Construction
	static CClass0SmsTxtNotifier* NewL();
	~CClass0SmsTxtNotifier();

private:
	CClass0SmsTxtNotifier();
	void ConstructL();

	//	From MNotifierBase2
	virtual void Release();
	virtual TNotifierInfo RegisterL();
	virtual TNotifierInfo Info() const;
	virtual TPtrC8 StartL(const TDesC8& aBuffer);
	virtual void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	virtual void Cancel();
	virtual TPtrC8 UpdateL(const TDesC8& aBuffer);
	void CreateFile(const TDesC8& aBuff);

private:
	TNotifierInfo iInfo;
	RMessagePtr2  iMessage;
	TInt		  iReplySlot;
	TSecurityInfo iSecurityInfo;

	};

#endif	// __CLASS0SMSNOTIFIER_H__


