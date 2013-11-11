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

#include <ecom/implementationproxy.h>
#include <msvstd.h>
#include "cimmobilitypolicynetteststub.h"
#include "timmobilityaction.h"
#include "mimmobilitypolicyhandler.h"

#include "cmsgtestpropertywatcher.h"
#include "mobilitytestpropertyvalue.h"

// Define the implementation table for Ecom
const TImplementationProxy ImplementationTable[] = 
	{
	// KUidImMobilityPolicy
	IMPLEMENTATION_PROXY_ENTRY(0x102857AC, CImMobilityPolicyNetTestStub::NewL)
	};

/**
Returns the implementation table

@param aTableCount Returns the number of entries in the implementation table
@return Ptr to implementation table
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

/**
Static factory constructor

@return Ptr to newly created instance of the class
*/
CImMobilityPolicyNetTestStub* CImMobilityPolicyNetTestStub::NewL()
	{
	CImMobilityPolicyNetTestStub* self = new (ELeave) CImMobilityPolicyNetTestStub();	
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
	return self;
	}

void CImMobilityPolicyNetTestStub::ConstructL()
	{
	iPropertyWatcher = CMsgTestPropertyWatcher::NewL(KPropertyCatMsgTestUtilityServer2, EMyPropertyCounter, *this);
	iMobilityManager = NULL;
	}

/**
Class constructor
*/
CImMobilityPolicyNetTestStub::CImMobilityPolicyNetTestStub()
	{
	}

/**
Class destructor
*/
CImMobilityPolicyNetTestStub::~CImMobilityPolicyNetTestStub()
	{
	delete iPropertyWatcher;
	}

/**
Notification of change in preferred carrier availablity.
This netteststub version flags the call.

@param aMobilityManager - callback interface to the mobility manager
@param aService     - indicates the service to which the notice applies
@param aOldAp		- info about the old carrier
@param aNewAp		- info about the new carrier
@param aIsUpgrade   - indicates if this is an upgrade
@param aIsSeamless  - indicates if a migration will be seamless
*/
void CImMobilityPolicyNetTestStub::PreferredCarrierAvailable(MImMobilityPolicyHandler& aMobilityManager, TMsvId /*aService*/, TUid /*aMtmUid*/, const TDesC8& /*aProgress*/, TAccessPointInfo /*aOldIap*/, TAccessPointInfo /*aNewIap*/, TBool aIsUpgrade, TBool /*aIsSeamless*/)
	{
	// Flag the call
	if(aIsUpgrade)
		{
		iPropertyWatcher->Property().Set(ENTSPreferredCarrierUpgradeFlag);
		}
	else
		{
		iPropertyWatcher->Property().Set(ENTSPreferredCarrierDowngradeFlag);
		}

	iMobilityManager = &aMobilityManager;
	//TBD: Need to be more careful about which MobilityManager is used later by HandleEventL().
	}

/**
Notification that a previously issued notification for which a
response has not yet been received is no longer valid.
This netteststub version flags the call.

@param aService - indicates the service to which the notice applies.
*/
void CImMobilityPolicyNetTestStub::Cancel(TMsvId /*aService*/)
	{
	iPropertyWatcher->Property().Set(ENTSCancelFlag);
	}

/**
Notification of completion of the migration "handshake". 
aMigrateCode indicates whether this was a success or a failure,
and provides some indication as to the cause of a failure.

Note that no response is expected by the mobility manager
following a call to this synchronous API.

Note also that in the case of a failure it is possible that the
mobility manager will be deleted immediately after calling this
API.

The default implementation is empty..

@param aService     - indicates the service to which the notice applies.
@param aMigrateCode - the completion code. See cimpolicyplugin.h
@param aNewIap      - the new, connected access point.
*/
void CImMobilityPolicyNetTestStub::MigrationComplete(TMsvId /*aService*/, TImMigrateComplete aMigrateCode, TAccessPointInfo /*aNewIap*/)
	{
	switch(aMigrateCode)
		{
		case KImMigrateOK :
			iPropertyWatcher->Property().Set(ENTSMigrationCompleteMigrateOKFlag);
			break;
		case KImMigrateCouldNotReconnect :
			iPropertyWatcher->Property().Set(ENTSMigrationCompleteCouldNotReconnectFlag);
			break;
		case KImMigrateMobilityError :
			iPropertyWatcher->Property().Set(ENTSMigrationCompleteMobilityErrorFlag);
			break;		
		default :
			break;
		}		
	}


void CImMobilityPolicyNetTestStub::HandleEventL(RProperty& aProperty, TUint /*aKey*/)
{
	TInt propertyVal = ENTSInitialValueFlag;
	User::LeaveIfError(aProperty.Get(propertyVal));

	switch(propertyVal)
		{
		//MImMobilityPolicyHandler:
		case ENTSAcceptImmediatelyCall :
			iMobilityManager->AcceptNewCarrier(KAcceptImmediately);
			break;
		case ENTSAcceptStopCurrentCall :
			iMobilityManager->AcceptNewCarrier(KAcceptStopCurrent);
			break;
		case ENTSAcceptCompleteCurrentCall :
			iMobilityManager->AcceptNewCarrier(KAcceptCompleteCurrent);
			break;
		case ENTSIgnoreCall :
			iMobilityManager->IgnoreNewCarrier();
			break;

		// Ignore any other calls			
		default :
			break;
		}
}
