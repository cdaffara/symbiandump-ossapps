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
// MSGMSGNETTESTSTUB.CPP
// 
//

#define LOCAL_INCLUDE
#include "msgnetteststub.h"
#undef LOCAL_INCLUDE
#include "cmsgtestpropertywatcher.h"
#include "cimmobilitytestframework.h"
#include "mobilitytestpropertyvalue.h"

EXPORT_C MSGNETTESTSTUB::CActiveCommsMobilityApiExt* MSGNETTESTSTUB::CActiveCommsMobilityApiExt::NewL(RCommsSubSession& aExtensionProvider,MMobilityProtocolResp& aProtocol)
{
	CActiveCommsMobilityApiExt* self = new (ELeave) CActiveCommsMobilityApiExt(aExtensionProvider, aProtocol);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void MSGNETTESTSTUB::CActiveCommsMobilityApiExt::ConstructL()
{
	iCommsMobilityApi = NULL;
	//Try to construct the real thing, but trap the 'not supported' error:
//	TRAPD(err, iCommsMobilityApi = ::CActiveCommsMobilityApiExt::NewL(aExtensionProvider, *this));

	iPropertyWatcher = CMsgTestPropertyWatcher::NewL(KPropertyCatMsgTestUtilityServer2, EMyPropertyCounter, *this);
    
	iServiceId = CImMobilityTestFramework::GetServiceId(iProtocol);
	User::LeaveIfError(iServiceId);
}

MSGNETTESTSTUB::CActiveCommsMobilityApiExt::CActiveCommsMobilityApiExt(RCommsSubSession& aExtensionProvider, MMobilityProtocolResp& aProtocol)
: iCommsSubSession(aExtensionProvider), iProtocol(aProtocol)
{
}

EXPORT_C MSGNETTESTSTUB::CActiveCommsMobilityApiExt::~CActiveCommsMobilityApiExt()
{
	delete iCommsMobilityApi;
	delete iPropertyWatcher;
}

EXPORT_C void MSGNETTESTSTUB::CActiveCommsMobilityApiExt::MigrateToPreferredCarrier()
{
//	Flag the call iCommsMobilityApi->MigrateToPreferredCarrier();
	iPropertyWatcher->Property().Set(ENTSMigrateToPreferredCarrierFlag);
}

EXPORT_C void MSGNETTESTSTUB::CActiveCommsMobilityApiExt::IgnorePreferredCarrier()
{
//	Flag the call iCommsMobilityApi->IgnorePreferredCarrier();
	iPropertyWatcher->Property().Set(ENTSIgnorePreferredCarrierFlag);
}

EXPORT_C void MSGNETTESTSTUB::CActiveCommsMobilityApiExt::NewCarrierAccepted()
{
//	Flag the call iCommsMobilityApi->NewCarrierAccepted();
	iPropertyWatcher->Property().Set(ENTSNewCarrierAcceptedFlag);
}

EXPORT_C void MSGNETTESTSTUB::CActiveCommsMobilityApiExt::NewCarrierRejected()
{
//	Flag the call iCommsMobilityApi->NewCarrierRejected();
	iPropertyWatcher->Property().Set(ENTSNewCarrierRejectedFlag);
}

//Additional functionality:

// from MMobilityProtocolResp
void MSGNETTESTSTUB::CActiveCommsMobilityApiExt::PreferredCarrierAvailable(TAccessPointInfo aOldAp, TAccessPointInfo aNewAp, TBool aIsUpgrade, TBool aIsSeamless)
{
	iProtocol.PreferredCarrierAvailable(aOldAp, aNewAp, aIsUpgrade, aIsSeamless);
}

void MSGNETTESTSTUB::CActiveCommsMobilityApiExt::NewCarrierActive(TAccessPointInfo aNewAp, TBool aIsSeamless)
{
	iProtocol.NewCarrierActive(aNewAp, aIsSeamless);
}

void MSGNETTESTSTUB::CActiveCommsMobilityApiExt::Error(TInt aError)
{
	iProtocol.Error(aError);
}

// from CMsgTestPropertyWatcher
void MSGNETTESTSTUB::CActiveCommsMobilityApiExt::HandleEventL(RProperty& aProperty, TUint /*aKey*/)
	{
	
	TInt propertyVal = ENTSInitialValueFlag;
	User::LeaveIfError(aProperty.Get(propertyVal));
	
	TAccessPointInfo oldAp;
	TAccessPointInfo newAp;		
	oldAp.SetAccessPoint(1);
	newAp.SetAccessPoint(1);
		
	TBool isSeamless = EFalse;

	TInt error = 0;

	switch(propertyVal)
		{
		// Callbacks to production code:
		case ENTSPreferredCarrierUpgradeCall :			
			PreferredCarrierAvailable(oldAp, newAp, ETrue, isSeamless);
			break;
		case ENTSPreferredCarrierDowngradeCall :
			PreferredCarrierAvailable(oldAp, newAp, EFalse, isSeamless);
			break;
		case ENTSNewCarrierActiveCall :
			NewCarrierActive(newAp, isSeamless);
			break;
		case ENTSErrorCall :
			MSGNETTESTSTUB::CActiveCommsMobilityApiExt::Error(error);
			break;

		// Ignore any other calls
		default :
			break;
		}

  }
