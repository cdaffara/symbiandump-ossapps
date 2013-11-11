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

#ifndef __CIMMOBILITYTESTTLS_H__
#define __CIMMOBILITYTESTTLS_H__

#include <e32base.h>
#include <msvstd.h>

#include "cimmobilitytestframework.h"
#include <comms-infras/cs_mobility_apiext.h>  // MMobilityProtocolResp

class CImMobilityManager;

/**
Holds a list of the test frameworks and the list of actions read from
the test script.
A pointer to this class is held in thread local storage.

@internalComponent
@released
*/
NONSHARABLE_CLASS(CImMobilityTestTls) : public CBase
	{
public:
	static CImMobilityTestTls* NewL();
	CImMobilityTestTls();
	void ConstructL();
	~CImMobilityTestTls();

	void AddFrameworkL(CImMobilityTestFramework& aFramework);
	CImMobilityTestFramework* RemoveFramework(CImMobilityManager& aMobilityManager, TBool& aLastOne);
	CImMobilityTestFramework* GetFramework(TMsvId aServiceId);
	CImMobilityTestFramework* GetFramework(CImMobilityManager& aMobilityManager);
	CImMobilityTestFramework* GetFramework(MMobilityProtocolResp& aProtocol);

private:
	// Test framework list
  RPointerArray<CImMobilityTestFramework> iFrameworkList;
	};

#endif //__CIMMOBILITYTESTTLS_H__
