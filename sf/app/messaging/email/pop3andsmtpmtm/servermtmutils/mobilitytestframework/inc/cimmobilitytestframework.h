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

#ifndef __CIMMOBILITYTESTFRAMEWORK_H__
#define __CIMMOBILITYTESTFRAMEWORK_H__

#include <e32base.h>
#include <msvstd.h>
#include <comms-infras/cs_mobility_apiext.h>  // MMobilityProtocolResp
#include "mmsgtestpropertywatcher.h"
#include "timmobilitytestmtmstate.h"

// Forward class references
class CImMobilityManager;
class CMsgTestPropertyWatcher;

/**
The test framework class.

@internalComponent
@released
*/
class CImMobilityTestFramework : public CBase,
									public MMsgTestPropertyWatcher
	{
public:
	IMPORT_C static void Create(TMsvId aServiceId, CImMobilityManager& aMobilityManager);
	IMPORT_C static void Delete(CImMobilityManager& aMobilityManager);
	IMPORT_C static void MtmState(TMsvId aServiceId, TImMobilityTestMtmState aMtmState);
	IMPORT_C static TMsvId GetServiceId(MMobilityProtocolResp& aProtocol);

	~CImMobilityTestFramework();
	TMsvId ServiceId();
	CImMobilityManager& MobilityManager();
	void TimerExpired();
	
	// from MMsgTestPropertyWatcher
	void HandleEventL(RProperty& aProperty, TUint /*aKey*/);

private:
	static void NewL(TMsvId aServiceId, CImMobilityManager& aMobilityManager);
	CImMobilityTestFramework(TMsvId aServiceId, CImMobilityManager& aMobilityManager);
	void ConstructL();

private:
	// Mobility manager
	CImMobilityManager& iMobilityManager;

	// Service Id of related MTM
	TMsvId iServiceId;
	
	// Property watcher for Publish&Subscribe
	CMsgTestPropertyWatcher* iPropertyWatcher;
	};

#endif //__CIMMOBILITYTESTFRAMEWORK_H__
