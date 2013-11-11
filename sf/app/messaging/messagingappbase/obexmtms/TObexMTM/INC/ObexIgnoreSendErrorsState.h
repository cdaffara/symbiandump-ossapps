// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: ObexIgnoreSendErrorsState.h $
// $Author: Simonk $
// $Revision: 1 $
// $Date: 29/01/02 17:42 $
// 
//


#ifndef __OBEXIGNORESENDERRORSSTATE_H__
#define __OBEXIGNORESENDERRORSSTATE_H__

#include "harness.h"        //MBaseTestState

//forward declarations
class CObexClientTest;


class CObexIgnoreSendErrorsState : public CBase, public MBaseTestState
/*
 * A state of class to be added to a test harness so that errors
 * in the send operation don't result in the test harness finishing
 */
	{
public:
	CObexIgnoreSendErrorsState(CObexClientTest& aClientTest);
	void StartL(TRequestStatus& aStatus);

private:
	CObexClientTest& iClientTest;
	};

#endif  //__OBEXIGNORESENDERRORSSTATE_H__
