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

#ifndef __CIMMOBILITYPOLICYPROVIDER_H__
#define __CIMMOBILITYPOLICYPROVIDER_H__

#include <e32base.h>
#include <msvstd.h>

#include "timmobilityaction.h" // for TImMigrateComplete

class CImMobilityPolicyPlugin;
class MImMobilityPolicyHandler;
class TAccessPointInfo;
class CImMobilityLogger;


/**
The CImMobilityPolicyProvider class

Receives notifications of bearer availibility events passed by instances
of the mobility manager class. Responds with the action to be taken.

@internalTechnology
@prototype
*/
class CImMobilityPolicyProvider : public CBase
	{
public:
	// construction
	IMPORT_C static void CreateL();
	IMPORT_C static void Delete();

	// obtain a reference to the instance of this class:
	IMPORT_C static CImMobilityPolicyProvider& GetRef();
	
	// API for receiveing notifications from the messaging mobility manager:
	IMPORT_C void PreferredCarrierAvailable(MImMobilityPolicyHandler& aMobilityManager, 
	                                        TMsvId aService,
	                                        TUid aMtmUid,
	                                        const TDesC8& aProgress,
	                                        TAccessPointInfo aOldAp, 
	                                        TAccessPointInfo aNewAp,
	                                        TBool aIsUpgrade,
	                                        TBool aIsSeamless);
	IMPORT_C void Cancel(TMsvId aService);
	
	IMPORT_C void MigrationComplete(TMsvId aService, 
	                                TImMigrateComplete aMigrateCode, 
	                                TAccessPointInfo aNewIap);
	
	// access to the debug logging utility
	IMPORT_C CImMobilityLogger& Logger();

private:
	CImMobilityPolicyProvider();
	void ConstructL();
	~CImMobilityPolicyProvider();

private:
	TInt iRefCount;
	CImMobilityPolicyPlugin* iPolicyPlugin;
	CImMobilityLogger* iLogger;
	};


#endif // __CIMMOBILITYPOLICYPROVIDER_H__


