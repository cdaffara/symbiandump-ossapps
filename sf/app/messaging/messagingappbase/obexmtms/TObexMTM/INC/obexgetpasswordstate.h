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
// $Workfile: obexgetpasswordstate.h $
// $Author: Stevep $
// $Revision: 3 $
// $Date: 27/03/02 14:18 $
// 
//

#ifndef __OBEXGETPASSWORDSTATE_H__
#define __OBEXGETPASSWORDSTATE_H__

#include "harness.h"        //MBaseTestState

// forward declarations
class CObexClientTest;

class CObexGetPasswordState : public CBase, public MBaseTestState
/**
Test state to get the current OBEX authentication password (stored in the service entry's message store) and write
it to the logfile. If no password has been set the state will report this without error, but a service entry
for the current mtm MUST exist if this state is to be successful.
*/
	{
public:
	/**
	C++ constructor.

	@param aClientTest the parent CObexClientTest
	*/
	CObexGetPasswordState(CObexClientTest& aClientTest);

	/**
	Called by the parent to invoke the state. As this function is synchronous, the aStatus member is completed as
	the function returns. Will leave with KErrNotFound if the service entry does not exist.

	@param aStatus status to be completed when the function returns.
	@leave KErrXXX system wide error codes.
	@leave KErrNotFound if the service entry does not exist
	*/
	void StartL(TRequestStatus& aStatus);

private:
	CObexClientTest& iClientTest;	//<Reference to the parent CObexClientTest object
	};

#endif//__OBEXGETPASSWORDSTATE_H__
