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

#ifndef __CIMMOBILITYPOLICYDEFAULT_H__
#define __CIMMOBILITYPOLICYDEFAULT_H__

#include <cimmobilitypolicyplugin.h>

class MImMobilityPolicyHandler;
class TAccessPointInfo;

/**
Default implementation of the Messaging Mobility Policy Plugin
@internalTechnology
@prototype
*/
class CImMobilityPolicyDefault : public CImMobilityPolicyPlugin
	{
public:
	static CImMobilityPolicyDefault* NewL();
	virtual ~CImMobilityPolicyDefault();

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

private:
	CImMobilityPolicyDefault();
	};

#endif // __CIMMOBILITYPOLICYDEFAULT_H__
