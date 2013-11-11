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
// $Workfile: obexresetpasswordstate.h $
// $Author: Stevep $
// $Revision: 3 $
// $Date: 27/03/02 14:18 $
// 
//

#ifndef __OBEXRESETPASSWORDSTATE_H__
#define __OBEXRESETPASSWORDSTATE_H__

#include "harness.h"        //MBaseTestState

// forward declarations
class CObexClientTest;

class CObexResetPasswordState : public CBase, public MBaseTestState
/**
Removes any existing OBEX authentication password from the service entry's message store. The service entry must exist
for this state to be succesful. Invoking this state effectively disables OBEX authentication.
*/
	{
	public:
		/**
		Called by the parent to invoke the state. As this function is synchronous, the aStatus member is completed as
		the function returns. Will leave with KErrNotFound if the service entry does not exist.

		@param aStatus status to be completed when the function returns.
		@leave KErrXXX system wide error codes.
		@leave KErrNotFound if the service entry does not exist
		*/
		void StartL(TRequestStatus& aStatus);

		/**
		C++ constructor.

		@param aClientTest the parent CObexClientTest
		*/
		CObexResetPasswordState(CObexClientTest& aClientTest);

	private:
		CObexClientTest&	iClientTest;	//<Reference to the parent CObexClientTest object
	};

#endif //__OBEXRESETPASSWORDSTATE_H__
