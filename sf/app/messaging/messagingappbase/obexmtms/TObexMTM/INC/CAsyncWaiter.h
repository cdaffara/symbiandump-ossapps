// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CASYNCWAITER_H__
#define __CASYNCWAITER_H__

#include <e32base.h>

class CAsyncWaiter : public CActive
	{
public:
	static CAsyncWaiter* NewL();
	~CAsyncWaiter();
	
	void StartAndWait();
	TInt Result() const;
	
private:
	CAsyncWaiter();
	
	// from CActive
	virtual void RunL();
	virtual void DoCancel();
	
private:
	TInt iError;
	};

#endif // __CASYNCWAITER_H__
