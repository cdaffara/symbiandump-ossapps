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

#ifndef __CIMMOBILITYMANAGER_H__
#define __CIMMOBILITYMANAGER_H__

#include <e32base.h>
#include <msvstd.h>
#include <comms-infras/cs_mobility_apiext.h>  // MMobilityProtocolResp, CActiveCommsMobilityApiExt
#include "mimmobilitypolicyhandler.h"
#include "mimmobileserver.h"
#include "msgnetteststub.h"

/**
The CImMobilityManager class manages notifications from the Bearer Mobility
Framework, passing notifications up to the Mobility Policy Provider, and
instructing the server MTM of the appropriate actions to take to perform
the requested mobility behaviour.

@internalComponent
@prototype
*/
class CImMobilityManager : public CBase,
                           public MMobilityProtocolResp,
                           public MImMobilityPolicyHandler
	{
public:
	// construction/destruction:
	IMPORT_C static CImMobilityManager* NewL(TUid aMtmUid, TMsvId aAccountId, MImMobileServer& aMtm);
	~CImMobilityManager();
	
	// initialisation. 
	// bearer mobility events are not observered until this is called.
	IMPORT_C void SetConnection(RCommsSubSession& aConnection);
	
	// from MMobilityProtocolResp
	void PreferredCarrierAvailable(TAccessPointInfo aOldAp, TAccessPointInfo aNewAp, TBool aIsUpgrade, TBool aIsSeamless);
	void NewCarrierActive(TAccessPointInfo aNewAp, TBool aIsSeamless);
	void Error(TInt aError);
	
	// from MImMobilityPolicyHandler
	IMPORT_C void AcceptNewCarrier(TImMobilityAction aAction);
	IMPORT_C void IgnoreNewCarrier();

	// the interface to the server MTM. Perhaps should define an m-class for neatness.
	IMPORT_C void MigrateToNewCarrier();
	IMPORT_C void NewCarrierAccepted();
	IMPORT_C void NewCarrierRejected();

private:
	CImMobilityManager(TUid aMtmUid, TMsvId aAccountId, MImMobileServer& aMtm);
	void ConstructL();
	
	/**
	Specifies the high-level state of the mobility manager
	*/	
	enum TMobilityState
		{
		/**
		The Mobility Manager is uninitialiased.
		*/
		EMobilityUninitialised,
		/**
		The Mobility Manager has been initialised. 
		No notifications received, nor waiting for any response.
		*/
		EMobilityIdle,
		/**
		PreferredCarrierAvailable(upgradade) notification received and policy 
		provider notified. Waiting for instruction from the policy provider.
		*/
		EMobilityWaitForUpgradePolicy,
		/**
		PreferredCarrierAvailable(downgradade) notification received and policy
		provider notified. Waiting for instruction from the policy provider.
		*/
		EMobilityWaitForDowngradePolicy,
		/**
		Second PreferredCarrierAvailable(upgrade) notification received while
		waiting for the original old sockets to be closed. Policy provider notified. 
		Waiting for instruction from the policy provider.
		Also waiting for notification that sockets using the old carrier have been
		disconnected.
		This state exists purely to handle an unexpected race condition 
		*/
		EMobilityWaitForUpgradePolicyAndOldSocketClose,
		/**
		Second PreferredCarrierAvailable(upgrade) notification received while
		waiting for the original old sockets to be closed. Policy provider notified. 
		Policy Provider has rejected the second upgrade notice.
		Waiting for notification that sockets using the old carrier have been
		disconnected. When the old sockets become disconnected, the MTM shall be
		instructed to reconnect (via NewCarrierActive(), even though this will be
		a connection via the original carrier.
		This state exists purely to handle an unexpected race condition 
		*/
		EMobilityWaitForOldSocketCloseCarrierRejected,
		/**
		Waiting for notification that sockets using the old carrier have been
		disconnected.
		*/
		EMobilityWaitForOldSocketClose,
		/**
		Waiting for notification from the Bearer Mobility Framework that the
		new carrier is active and available for creation of new sockets.
		*/
		EMobilityWaitForMigrate,
		/**
		Waiting for notification from the Server MTM that either new sockets
		have been successfully created using the new carrier, or that
		connection has been unsuccessful.
		*/ 
		EMobilityConnectingToNewCarrier
		};

private:
	TUid    iMtmUid;
	TMsvId  iAccountId;
	TUint   iState;

	// Interfaces...		
	MImMobileServer& iMtm;
	CActiveCommsMobilityApiExt* iCommsMobilityApi;
	
	// info from the last PreferredCarrierAvailable() notice
	TAccessPointInfo iNewAp;
	TAccessPointInfo iOldAp;
	TBool iIsSeamless;
	
	// A flag - set when we're closing down as we don't need further notifications from Comms.
	TBool iIsCancelled;
	};

#endif // __CIMMOBILITYMANAGER_H__
