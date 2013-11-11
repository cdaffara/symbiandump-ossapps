// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CIMMOBILITYPOLICYNETTESTSTUB_H__
#define __CIMMOBILITYPOLICYNETTESTSTUB_H__

#include <cimmobilitypolicyplugin.h>
#include "mmsgtestpropertywatcher.h"

class MImMobilityPolicyHandler;
class CMsgTestPropertyWatcher;

/**
Default implementation of the Messaging Mobility Policy Plugin
@internalTechnology
@prototype
*/
class CImMobilityPolicyNetTestStub : public CImMobilityPolicyPlugin,
									public MMsgTestPropertyWatcher
	{
public:
	static CImMobilityPolicyNetTestStub* NewL();
	virtual ~CImMobilityPolicyNetTestStub();

	//from CImMobilityPolicyPlugin
	virtual void PreferredCarrierAvailable(MImMobilityPolicyHandler& aMobilityManager,
	                                       TMsvId aService,
	                                       TUid aMtmUid,
	                                       const TDesC8& aProgress,
	                                       TAccessPointInfo aOldIap,
	                                       TAccessPointInfo aNewIap,
	                                       TBool aIsUpgrade,
	                                       TBool aIsSeamless);
	virtual void Cancel(TMsvId aService);

	virtual void MigrationComplete(TMsvId aService, 
	                               TImMigrateComplete aMigrateCode, 
    	                           TAccessPointInfo aNewIap);


	// from MMsgTestPropertyWatcher
	void HandleEventL(RProperty& aProperty, TUint /*aKey*/);
	
private:
	void ConstructL();
	CImMobilityPolicyNetTestStub();

private:
	CMsgTestPropertyWatcher* iPropertyWatcher;
	MImMobilityPolicyHandler* iMobilityManager;
	};

#endif // __CIMMOBILITYPOLICYNETTESTSTUB_H__
