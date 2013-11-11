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
// $Workfile: obexsetpasswordstate.h $
// $Author: Stevep $
// $Revision: 3 $
// $Date: 27/03/02 14:18 $
// 
//

#ifndef __OBEXSETPASSWORDSTATE_H__
#define __OBEXSETPASSWORDSTATE_H__

#include "harness.h"        //MBaseTestState

// forward declarations
class CObexClientTest;

class CObexSetPasswordState : public CBase, public MBaseTestState
/**
Sets the OBEX authentication password of the current MTM, stored in the service entry's message store. Any existing
password will be overwritten. To suceed, a service entry must exist.
*/
	{
	public:
		/**
		Canonical two-phase constructor.

		@param aClientTest parent CObexClientTest object
		@param aPassword the new password to set
		@leave KErrXXX system wide error codes
		*/
		static CObexSetPasswordState* NewL(CObexClientTest& aClientTest, const TDesC& aPassword);

		/**
		Called by the parent to invoke the state. As this function is synchronous, the aStatus member is completed as
		the function returns. Will leave with KErrNotFound if the service entry does not exist.

		@param aStatus status to be completed when the function returns.
		@leave KErrXXX system wide error codes.
		@leave KErrNotFound if the service entry does not exist
		*/
		void StartL(TRequestStatus& aStatus);

		/**
		Destructor
		*/
		~CObexSetPasswordState();
	private:
		/**
		C++ constructor

		@param aClientTest parent CObexClientTest object
		*/
		CObexSetPasswordState(CObexClientTest& aClientTest);

		/**
		Canonical second-phase constructor. 

		@param aPassword the new password to set
		@leave KErrXXX system wide error codes
		*/
		void ConstructL(const TDesC& aPassword);

	private:
		CObexClientTest&	iClientTest;	//<Reference to the parent CObexClientTest object
		HBufC*				iPassword;		//<Local copy of the password
	};

#endif //__OBEXSETPASSWORDSTATE_H__
