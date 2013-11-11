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
// $Workfile: CreateServiceEntryState.h $
// $Author: Simonk $
// $Revision: 1 $
// $Date: 23/01/02 14:53 $
// 
//


#ifndef __CREATESERVICEENTRYSTATE_H__
#define __CREATESERVICEENTRYSTATE_H__

#include "harness.h"        //MBaseTestState

class CObexClientTest;


class CCreateServiceEntryState : public CBase, public MBaseTestState
/*
 * A state of class to be added to a test harness that encapsulates the
 * creation of a service entry
 */
	{
public:
	CCreateServiceEntryState(CObexClientTest& aClientTest);
	void StartL(TRequestStatus& aStatus);

private:
	CObexClientTest& iClientTest;
	};



#endif // __CREATESERVICEENTRYSTATE_H__
