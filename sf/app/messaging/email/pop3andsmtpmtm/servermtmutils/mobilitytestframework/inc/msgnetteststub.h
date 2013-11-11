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

#ifndef __MSGNETTESTSTUB_H__
#define __MSGNETTESTSTUB_H__

#ifdef __MOBILITY_TEST_FRAMEWORK

#include <e32std.h>
#include <comms-infras/cs_mobility_apiext.h>
#include "mmsgtestpropertywatcher.h"
#include <msvstd.h>

class CMsgTestPropertyWatcher;

namespace MSGNETTESTSTUB
{

class CActiveCommsMobilityApiExt :	public CBase,
									public MMobilityProtocolResp,
									public MMsgTestPropertyWatcher
	{
public:

	//Stubbed interface:
	IMPORT_C static CActiveCommsMobilityApiExt* NewL(RCommsSubSession& aExtensionProvider,MMobilityProtocolResp& aProtocol);
	IMPORT_C ~CActiveCommsMobilityApiExt();

	IMPORT_C void MigrateToPreferredCarrier();
	IMPORT_C void IgnorePreferredCarrier();
	IMPORT_C void NewCarrierAccepted();
	IMPORT_C void NewCarrierRejected();

protected:

	//Internal methods:
    void ConstructL();
    explicit CActiveCommsMobilityApiExt(RCommsSubSession& aExtensionProvider, MMobilityProtocolResp& aProtocol);

public:

	// from MMobilityProtocolResp
	void PreferredCarrierAvailable(TAccessPointInfo aOldAp, TAccessPointInfo aNewAp, TBool aIsUpgrade, TBool aIsSeamless);
	void NewCarrierActive(TAccessPointInfo aNewAp, TBool aIsSeamless);
	void Error(TInt aError);

	// from MPropertyWatcher
	void HandleEventL(RProperty& aProperty, TUint aKey);

private:
	
	RCommsSubSession& iCommsSubSession;
	MMobilityProtocolResp& iProtocol;

	//Pointer to the real thing
	::CActiveCommsMobilityApiExt* iCommsMobilityApi;
	
	//CMsgTestPropertyWatcher for IPC
	CMsgTestPropertyWatcher* iPropertyWatcher;
	
	TMsvId iServiceId;
	};

}

#ifndef LOCAL_INCLUDE
#define CActiveCommsMobilityApiExt MSGNETTESTSTUB::CActiveCommsMobilityApiExt
#endif // LOCAL_INCLUDE

#endif //__MOBILITY_TEST_FRAMEWORK

#endif //__MSGNETTESTSTUB_H__
