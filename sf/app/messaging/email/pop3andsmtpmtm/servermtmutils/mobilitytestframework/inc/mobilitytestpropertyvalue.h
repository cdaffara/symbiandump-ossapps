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

/**
 @file
 @internalTechnology
 @released
*/

#ifndef __MOBILITYTESTPROPERTYVALUE_H__
#define __MOBILITYTESTPROPERTYVALUE_H__

#include <e32cmn.h>

const TUid KPropertyCatMsgTestUtilityServer2={0x102857AB};

static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);

enum TMyPropertyKeys {EMyPropertyCounter, EMyPropertyParamCall, EMyPropertyParamFlag};

const TInt KMobilityTestPropertyValueEnumerationOffset = 1000;

enum TMobilityTestPropertyValue
{
	ENTSInitialValueFlag=KMobilityTestPropertyValueEnumerationOffset,
	
	//MMobilityProtocolResp:
	ENTSPreferredCarrierUpgradeCall,
	ENTSPreferredCarrierDowngradeCall,
	ENTSNewCarrierActiveCall,
	ENTSErrorCall,
	
	//CActiveCommsMobilityApiExt:
	ENTSMigrateToPreferredCarrierFlag,
	ENTSIgnorePreferredCarrierFlag,
	ENTSNewCarrierAcceptedFlag,
	ENTSNewCarrierRejectedFlag,

	//CMobilityPolicyPlugin:
	ENTSPreferredCarrierUpgradeFlag,
	ENTSPreferredCarrierDowngradeFlag,
	ENTSCancelFlag,
	ENTSMigrationCompleteMigrateOKFlag,
	ENTSMigrationCompleteCouldNotReconnectFlag,
	ENTSMigrationCompleteMobilityErrorFlag,
	
	//MImMobilityPolicyHandler:
	ENTSAcceptImmediatelyCall,
	ENTSAcceptStopCurrentCall,
	ENTSAcceptCompleteCurrentCall,
	ENTSIgnoreCall
};

#endif //__MOBILITYTESTPROPERTYVALUE_H__
