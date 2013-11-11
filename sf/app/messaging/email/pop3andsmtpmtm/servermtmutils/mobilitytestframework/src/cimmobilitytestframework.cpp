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

#include "cimmobilitytestframework.h"
#include "cimmobilitytesttls.h"
#include "cimmobilitymanager.h"
#include "cmsgtestpropertywatcher.h"
#include "mobilitytestpropertyvalue.h"

/**
Creates a test framework.
Static method.

@param aServiceId Service ID of associated MTM
@param aMobilityManager Associated mobility manager
*/
EXPORT_C void CImMobilityTestFramework::Create(TMsvId aServiceId, CImMobilityManager& aMobilityManager)
	{
	TRAP_IGNORE(NewL(aServiceId, aMobilityManager));
	}

/**
Deletes a test framework.
Static method.

@param aMobilityManager Associated mobility manager
*/	
EXPORT_C void CImMobilityTestFramework::Delete(CImMobilityManager& aMobilityManager)
	{
	CImMobilityTestTls* testTls = static_cast<CImMobilityTestTls*>(Dll::Tls());

	if (testTls)
		{
		TBool lastOne = EFalse;
		CImMobilityTestFramework* framework = testTls->RemoveFramework(aMobilityManager, lastOne);

		if (framework)
			{
			delete framework;
			}

		if (lastOne)
			{
			delete testTls;
			}
		}
	}

/**
Called by an MTM when it reaches a specific operation state.
Static method.

@param aServiceId Service ID of MTM
@param aMtmState MTM state
*/
EXPORT_C void CImMobilityTestFramework::MtmState(TMsvId aServiceId, TImMobilityTestMtmState aMtmState)
	{
	CImMobilityTestTls* testTls = static_cast<CImMobilityTestTls*>(Dll::Tls());

	if (testTls)
		{
		CImMobilityTestFramework* framework = testTls->GetFramework(aServiceId);
		if (framework)
			{
			// Flag the call
			RProperty& property = framework->iPropertyWatcher->Property();
   		property.Set(aMtmState);
			}
		}
	}
	
/**
Called by the net test stub to determine the service id.
Static method.

@param aProtocol reference to MMobilityProtocolResp
*/
EXPORT_C TMsvId CImMobilityTestFramework::GetServiceId(MMobilityProtocolResp& aProtocol)
	{
	CImMobilityTestTls* testTls = static_cast<CImMobilityTestTls*>(Dll::Tls());

	if (testTls)
		{
		CImMobilityTestFramework* framework = testTls->GetFramework(aProtocol);
		if (framework)
			{
			return framework->ServiceId();
			}
		}

	return KErrNotFound;
	}

/**
Factory constructor.
Static method.

@param aServiceId Service ID of associated MTM
@param aMobilityManager Associated mobility manager
*/
void CImMobilityTestFramework::NewL(TMsvId aServiceId, CImMobilityManager& aMobilityManager)
	{
	CImMobilityTestTls* testTls = CImMobilityTestTls::NewL();

	CImMobilityTestFramework* self = new (ELeave) CImMobilityTestFramework(aServiceId, aMobilityManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	// Store framework. Ownership is passed.
	testTls->AddFrameworkL(*self);
	}

/**
Class constructor.

@param aServiceId Service ID of associated MTM
@param aMobilityManager Associated mobility manager
*/
CImMobilityTestFramework::CImMobilityTestFramework(TMsvId aServiceId, CImMobilityManager& aMobilityManager) :
 iMobilityManager(aMobilityManager),
 iServiceId(aServiceId)
	{
	}

/**
Second phase constructor
*/
void CImMobilityTestFramework::ConstructL()
	{
	iPropertyWatcher = CMsgTestPropertyWatcher::NewL(KPropertyCatMsgTestUtilityServer2, EMyPropertyCounter, *this);
	}

/**
Class destructor
*/
CImMobilityTestFramework::~CImMobilityTestFramework()
	{
	delete iPropertyWatcher;
	}

/**
Returns service ID of associated MTM

@return Service ID of MTM
*/
TMsvId CImMobilityTestFramework::ServiceId()
	{
	return iServiceId;
	}

/**
Returns associated mobility maanger

@return Mobility manager
*/
CImMobilityManager& CImMobilityTestFramework::MobilityManager()
	{
	return iMobilityManager;
	}

void CImMobilityTestFramework::HandleEventL(RProperty& /*aProperty*/, TUint /*aKey*/)
	{
	//Don't need to respond to events here.
	}
