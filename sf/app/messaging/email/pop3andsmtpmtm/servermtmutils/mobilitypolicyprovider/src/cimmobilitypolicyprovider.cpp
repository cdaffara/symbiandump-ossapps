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

#include <e32base.h>
#include <es_sock.h>
#include "cimmobilitypolicyprovider.h" // header file for this class

#include "mimmobilitypolicyhandler.h"  // API for the mobility manager
#include "cimmobilitypolicyplugin.h"   // API for the policy plugin
#include "timmobilityaction.h"         // actions that may be issued
#include "cimmobilitylogger.h"

#ifdef __IM_MOBILITY_LOGGING
_LIT(KLogFileName, "mobilitylog");
//_LIT(KLogFileFormat, "%S_%d");
#endif //__IM_MOBILITY_LOGGING

/**
Create the Messaging Mobility Policy Provider class.
There is only one instance of this class at any one time. If more than one
server MTM instance exists at any time, then they share this class.
This is a static function.
*/
EXPORT_C void CImMobilityPolicyProvider::CreateL()
	{
	CImMobilityPolicyProvider* self = NULL;

	TAny* tls = Dll::Tls();
	if (tls)
		{
		self = static_cast<CImMobilityPolicyProvider*>(tls);
		++self->iRefCount;
		}
	else
		{
		self = new (ELeave) CImMobilityPolicyProvider();
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop();
		Dll::SetTls(self);
		self->iRefCount = 1;
		}
	}

/**
Delete the Messaging Mobility Policy Provider class.
The class uses a reference count to ensure that there is only ever one instance
of the class at any time. Once the reference count hits 0, the class is destroyed.
This is a static function.
*/
EXPORT_C void CImMobilityPolicyProvider::Delete()
	{
	TAny* tls = Dll::Tls();

	if (tls)
		{
		CImMobilityPolicyProvider* self = static_cast<CImMobilityPolicyProvider*>(tls);
		--self->iRefCount;
		if (self->iRefCount == 0)
			{
			delete self;
			Dll::SetTls(NULL);
			}
		}
	}

/**
Get a reference to the Messaging Mobility Policy Provider class.
This is a static function.

@return Reference to Messaging Mobility Policy Provider class.
*/
EXPORT_C CImMobilityPolicyProvider& CImMobilityPolicyProvider::GetRef()
	{
	TAny* tls = Dll::Tls();	

	__ASSERT_DEBUG(tls != NULL, User::Invariant());

	CImMobilityPolicyProvider* policy = static_cast<CImMobilityPolicyProvider*>(tls);
	return *policy;
	}

CImMobilityPolicyProvider::CImMobilityPolicyProvider()
	{
	}

/**
Part of 2-phase construction.
Loads the ECom plugin that provides the actual decision making logic.
*/
void CImMobilityPolicyProvider::ConstructL()
	{
	iPolicyPlugin = CImMobilityPolicyPlugin::NewL();
	// start debug logging
#ifdef __IM_MOBILITY_LOGGING
	iLogger = CImMobilityLogger::NewL();
//	TFileName filename;
//	filename.Format(KLogFileFormat, KLogFileName, iAccountId);
	iLogger->CreateDefaultLog(KLogFileName);
	iLogger->LogFormat(KDefaultLog, "Messaging Mobility Logging");
#endif //__IM_MOBILITY_LOGGING
	}

CImMobilityPolicyProvider::~CImMobilityPolicyProvider()
	{
#ifdef __IM_MOBILITY_LOGGING
	if (iLogger)
		{
		iLogger->CloseLog(KDefaultLog);
		}
#endif //__IM_MOBILITY_LOGGING

	delete iLogger;	
	delete iPolicyPlugin;
	REComSession::FinalClose();
	}

/**
Notification of availability of a preferred carrier
*/
EXPORT_C void CImMobilityPolicyProvider::PreferredCarrierAvailable(MImMobilityPolicyHandler& aMobilityManager, TMsvId aService, TUid aMtmUid, const TDesC8& aProgress,TAccessPointInfo aOldAp, TAccessPointInfo aNewAp, TBool aIsUpgrade, TBool aIsSeamless)
	{
	iPolicyPlugin->PreferredCarrierAvailable(aMobilityManager, aService, aMtmUid, aProgress, aOldAp, aNewAp, aIsUpgrade, aIsSeamless);
	}

/**
A previous notification is no longer valid.
*/
EXPORT_C void CImMobilityPolicyProvider::Cancel(TMsvId aService)
	{
	iPolicyPlugin->Cancel(aService);
	}

EXPORT_C void CImMobilityPolicyProvider::MigrationComplete(TMsvId aService, TImMigrateComplete aMigrateCode, TAccessPointInfo aNewIap)
	{
	iPolicyPlugin->MigrationComplete(aService, aMigrateCode, aNewIap);
	}
	                               
/**
Gets a reference to the logger utility class.

@return Reference to the logger utility class.
*/
EXPORT_C CImMobilityLogger& CImMobilityPolicyProvider::Logger()
	{
	return *iLogger;
	}


