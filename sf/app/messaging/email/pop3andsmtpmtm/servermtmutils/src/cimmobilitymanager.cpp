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

#include "cimmobilitymanager.h"
#include "cimmobilitypolicyprovider.h"
#include "mobilitytestmtmapi.h"
#include "cimmobilitylogger.h"
#include <imutdll.h>

/**
The factory constructor. Part of two phased construction.
@param aAccountId - the ID of the email account
@param aMtm - Bearer Mobility API for the server MTM
*/
EXPORT_C CImMobilityManager* CImMobilityManager::NewL(TUid aMtmUid, TMsvId aAccountId, MImMobileServer& aMtm)
// static method
	{
	CImMobilityManager* self = new(ELeave) CImMobilityManager(aMtmUid, aAccountId, aMtm);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CImMobilityManager::CImMobilityManager(TUid aMtmUid, TMsvId aAccountId, MImMobileServer& aMtm)
	: iMtmUid(aMtmUid), iAccountId(aAccountId), iMtm(aMtm), iIsCancelled(EFalse)
	{
	MOBILITY_TEST_CREATE(aAccountId, *this);
	}

void CImMobilityManager::ConstructL()
	{
	// Ensure an instance of the mobility Policy Provider exists.
	CImMobilityPolicyProvider::CreateL();
	}

CImMobilityManager::~CImMobilityManager()
	{
	iIsCancelled = ETrue;
	delete iCommsMobilityApi;
	iCommsMobilityApi = NULL;
	
	if (iState == EMobilityWaitForUpgradePolicy   || 
	    iState == EMobilityWaitForDowngradePolicy || 
	    iState == EMobilityWaitForUpgradePolicyAndOldSocketClose)
		{
		CImMobilityPolicyProvider::GetRef().Cancel(iAccountId);
		}
	CImMobilityPolicyProvider::Delete();
	MOBILITY_TEST_DELETE(*this);
	}

/**
Registers with the Bearer Mobility Framework for preferred carrier notifications
@param aConnection - the connected session.
*/
EXPORT_C void CImMobilityManager::SetConnection(RCommsSubSession& aConnection)
	{
	if (iState == EMobilityUninitialised)
		{
		TRAPD(err, iCommsMobilityApi = CActiveCommsMobilityApiExt::NewL(aConnection, *this));

		if (err == KErrNone)
			{
			iState = EMobilityIdle;
			__LOG_FORMAT((KDefaultLog, "<%d> SetConnection() OK", iAccountId));
			}
		else
			{
			__LOG_FORMAT((KDefaultLog, "<%d> SetConnection() Error %d", iAccountId, err));
			}
		}
	else
		{
		// unexpected in this state
		__LOG_FORMAT((KDefaultLog, "<%d> SetConnection() unexpected state : %d", iAccountId, iState));
		__ASSERT_DEBUG(EFalse, gPanic(EPanicInvalidMobilityState));
		}
	}
	
/**
Notification of preferred carrier availability. 
This is called as a result of a new, preferred carrier becoming available (upgrade)
or the existing carrier being lost, with an alternative available (downgrade).

The notification is handed up to the Mobility Policy Provider for decision.

@param aOldAp		- info about the old carrier
@param aNewAp		- info about the new carrier
@param aIsUpgrade   - indicates if this is an upgrade
@param aIsSeamless  - indicates if a migration will be seamless
*/
void CImMobilityManager::PreferredCarrierAvailable(TAccessPointInfo aOldAp, TAccessPointInfo aNewAp, TBool aIsUpgrade, TBool aIsSeamless)
	{
	__LOG_FORMAT((KDefaultLog, "<%d> PreferredCarrierAvailable(state:%d old:%d new:%d isUp:%d isSless:%d)", iAccountId, iState, aOldAp.AccessPoint(), aNewAp.AccessPoint(), aIsUpgrade, aIsSeamless));
	// Handle the upgrade notice
	switch (iState)
		{
		case EMobilityIdle:
		case EMobilityWaitForMigrate:
		case EMobilityConnectingToNewCarrier:
			{
			// no special "extra" action to do in these states - just inform the
			// MTM (in the case of a downgrade) and notify the policy provider.
			// This is done below.
			break;
			}
		case EMobilityWaitForUpgradePolicy:
		case EMobilityWaitForDowngradePolicy:
		case EMobilityWaitForUpgradePolicyAndOldSocketClose:
			{
			// If in these states, this function call indicates that the
			// last received carrier availability notification is no longer
			// applicable. This could happen for a number of reasons:
			//   - the preferred carrier becomes unavailable
			//   - the notice changes from an upgrade to a downgrade
			//   - any others?
			// Anyway, what needs to happen is for the policy provider to have
			// its last notification cancelled, and the new one issued.
			CImMobilityPolicyProvider::GetRef().Cancel(iAccountId);
			break;
			}
		case EMobilityWaitForOldSocketClose:
		case EMobilityWaitForOldSocketCloseCarrierRejected:
			{
			// If in this state, the previous notice was for an upgrade, and 
			// the old connection is still being tidied up. If the new notice
			// is for a downgrade, then the old connection must be torn down
			// immediately. 
			if (!aIsUpgrade)
				{
				iMtm.CarrierLost();
				// If the new connection is the same as the new connection for
				// the previous notice then the carrier change has already been
				// accepted - otherwise fall through to re-notify the policy provider
				if (aNewAp.AccessPoint()==iNewAp.AccessPoint())
					{
					iState = EMobilityWaitForMigrate;

//#ifdef __MOBILITY_TEST_FRAMEWORK
//					TBool handled = CImMobilityTestFramework::Response(*this, KMobilityTestResponseMigrateToPreferredCarrier);
//					if (!handled)
//#endif //__MOBILITY_TEST_FRAMEWORK
					iCommsMobilityApi->MigrateToPreferredCarrier();
					// job done, return
					return;
					}
				// break;
				}
			else
				{
				// If the new notice is for an upgrade, it can only
				// mean that an even better carrier has become available
				// while waiting for the old old carrier to be tidied up!
				iState = EMobilityWaitForUpgradePolicyAndOldSocketClose;
				CImMobilityPolicyProvider::GetRef().PreferredCarrierAvailable(*this, iAccountId, iMtmUid, iMtm.MobilityProgress(), aOldAp, aNewAp, aIsUpgrade, aIsSeamless);
				return;
				}
			break;
			}
		case EMobilityUninitialised:
		default:
			{
			// unexpected in this state
			__ASSERT_DEBUG(EFalse, gPanic(EPanicInvalidMobilityState));
			}
		}
	
	// pass the notification up to the policy provider.
	if (aIsUpgrade)
		{
		if (iState != EMobilityWaitForUpgradePolicyAndOldSocketClose)
			{
			iState = EMobilityWaitForUpgradePolicy;
			}
		CImMobilityPolicyProvider::GetRef().PreferredCarrierAvailable(*this, iAccountId, iMtmUid, iMtm.MobilityProgress(), aOldAp, aNewAp, aIsUpgrade, aIsSeamless);
		}
	else
		{
		iMtm.CarrierLost();
		iState = EMobilityWaitForDowngradePolicy;
		CImMobilityPolicyProvider::GetRef().PreferredCarrierAvailable(*this, iAccountId, iMtmUid, iMtm.MobilityProgress(), aOldAp, aNewAp, aIsUpgrade, aIsSeamless);
		}
	}
	
/**
Notification that the new carrier is active and available for connecting sockets.

@param aNewAp		- info about the new carrier
@param aIsSeamless  - indicates if a migration will be seamless
*/
void CImMobilityManager::NewCarrierActive(TAccessPointInfo aNewAp, TBool aIsSeamless)
	{
	__LOG_FORMAT((KDefaultLog, "<%d> NewCarrierActive(state:%d new:%d isSless:%d)", iAccountId, iState, aNewAp.AccessPoint(), aIsSeamless));
	switch (iState)
		{
		case EMobilityWaitForMigrate:
			{
			iState = EMobilityConnectingToNewCarrier;
			iMtm.NewCarrierActive(aNewAp, aIsSeamless);
			break;
			}
		case EMobilityUninitialised:
		case EMobilityIdle:
		case EMobilityWaitForUpgradePolicy:
		case EMobilityWaitForDowngradePolicy:
		case EMobilityWaitForOldSocketClose:
		case EMobilityConnectingToNewCarrier:
		case EMobilityWaitForUpgradePolicyAndOldSocketClose:
		case EMobilityWaitForOldSocketCloseCarrierRejected:
		default:
			{
			// unexpected in this state
			__ASSERT_DEBUG(EFalse, gPanic(EPanicInvalidMobilityState));
			}
		}
	}

/**
Notification that an error has occured.
This indicates that the RConnection in use is no longer valid, therefore
the Server MTM will be immediately notified. Once this is done, this function
MUST NOT perform any actions that use class data, as the Server MTM may delete
its instance of CImMobilityManager.

@param aError - error code
*/
void CImMobilityManager::Error(TInt aError)
	{
	// If we're closing down, we don't need to handle further notifications from Comms,
	// and we can safely return from here.
	if(iIsCancelled)
		{
		return;
		}
	
	__LOG_FORMAT((KDefaultLog, "<%d> Error (state:%d error:%d)", iAccountId, iState, aError));
	switch (iState)
		{
		case EMobilityWaitForUpgradePolicy:
		case EMobilityWaitForDowngradePolicy:
		case EMobilityWaitForUpgradePolicyAndOldSocketClose:
			{
			// cancel the policy request.
			CImMobilityPolicyProvider::GetRef().Cancel(iAccountId);
			// fall through
			}
		case EMobilityIdle:
			{
			iState = EMobilityUninitialised;
			iMtm.MobilityError(aError);
			break;
			}
		case EMobilityWaitForOldSocketClose:
		case EMobilityWaitForMigrate:
		case EMobilityConnectingToNewCarrier:
		case EMobilityWaitForOldSocketCloseCarrierRejected:
			{
			iState = EMobilityUninitialised;
			// Tell the policy provider about it. (must happen before informing MTM)
			CImMobilityPolicyProvider::GetRef().MigrationComplete(iAccountId, KImMigrateMobilityError, iNewAp);
			// Inform the MTM. Do this last - this may delete the mobility manager
			iMtm.MobilityError(aError);
			break;
			}

		case EMobilityUninitialised:
		default:
			{
			// unexpected in this state
			__ASSERT_DEBUG(EFalse, gPanic(EPanicInvalidMobilityState));
			}
		}
	}
	
/**
Instruction from the Messaging Plugin Provider that the new carrier is to be accepted.

@param aAction - action that the server MTM should take when closing existing sockets.
*/
EXPORT_C void CImMobilityManager::AcceptNewCarrier(TImMobilityAction aAction)
	{
	__LOG_FORMAT((KDefaultLog, "<%d> Accept New Carrier (state:%d action:%d)", iAccountId, iState, aAction));
	switch (iState)
		{
		case EMobilityWaitForDowngradePolicy:
			{
			// old connections already closed - just inform the mobility framework
			iState = EMobilityWaitForMigrate;
//#ifdef __MOBILITY_TEST_FRAMEWORK
//			TBool handled = CImMobilityTestFramework::Response(*this, KMobilityTestResponseMigrateToPreferredCarrier);
//			if (!handled)
//#endif //__MOBILITY_TEST_FRAMEWORK
			iCommsMobilityApi->MigrateToPreferredCarrier();
			break;
			}
		case EMobilityWaitForUpgradePolicy:
			{
			if (aAction == KAcceptImmediately)
				{
				iMtm.CarrierLost();
				iState = EMobilityWaitForMigrate;
//#ifdef __MOBILITY_TEST_FRAMEWORK
//				TBool handled = CImMobilityTestFramework::Response(*this, KMobilityTestResponseMigrateToPreferredCarrier);
//				if (!handled)
//#endif //__MOBILITY_TEST_FRAMEWORK
				iCommsMobilityApi->MigrateToPreferredCarrier();
				}
			else
				{
				iState = EMobilityWaitForOldSocketClose;
				iMtm.PrepareForNewCarrier(aAction, iIsSeamless);
				}
			break;
			}
		case EMobilityWaitForUpgradePolicyAndOldSocketClose:
			{
			
			if (aAction == KAcceptImmediately)
				{
				iMtm.CarrierLost();
				iState = EMobilityWaitForMigrate;
//#ifdef __MOBILITY_TEST_FRAMEWORK
//				TBool handled = CImMobilityTestFramework::Response(*this, KMobilityTestResponseMigrateToPreferredCarrier);
//				if (!handled)
//#endif //__MOBILITY_TEST_FRAMEWORK
				iCommsMobilityApi->MigrateToPreferredCarrier();
				}
			else
				{
				// already waiting to finish using old carrier
				// do not update migration action, stick to first choice.
				iState = EMobilityWaitForOldSocketClose;
				}
			break;
			}

		case EMobilityUninitialised:
		case EMobilityIdle:
		case EMobilityWaitForOldSocketClose:
		case EMobilityWaitForOldSocketCloseCarrierRejected:
		case EMobilityWaitForMigrate:
		case EMobilityConnectingToNewCarrier:
		default:
			{
			// unexpected in this state
			__ASSERT_DEBUG(EFalse, gPanic(EPanicInvalidMobilityState));
			}
		}
	}

/**
Instruction from the Messaging Plugin Provider that the new carrier is to be ignored.

@param aAction - action that the server MTM should take when closing existing sockets.
*/
EXPORT_C void CImMobilityManager::IgnoreNewCarrier()
	{
	__LOG_FORMAT((KDefaultLog, "<%d> Error (state:%d)", iAccountId, iState));
	switch (iState)
		{
		case EMobilityWaitForUpgradePolicyAndOldSocketClose:
			{
			// defensive handling of "double upgrade" situation, in which an
			// upgrade is accepted, then a second upgrade notification is
			// received before the first handshake completes, and this second
			// upgrage is rejected.
			iState = EMobilityWaitForOldSocketCloseCarrierRejected;
//#ifdef __MOBILITY_TEST_FRAMEWORK
//			TBool handled = CImMobilityTestFramework::Response(*this, KMobilityTestResponseIgnorePreferredCarrier);
//			if (!handled)
//#endif //__MOBILITY_TEST_FRAMEWORK
			iCommsMobilityApi->IgnorePreferredCarrier();
			break;
			}
		case EMobilityWaitForUpgradePolicy:
		case EMobilityWaitForDowngradePolicy:
			{
			iState = EMobilityIdle;
//#ifdef __MOBILITY_TEST_FRAMEWORK
//			TBool handled = CImMobilityTestFramework::Response(*this, KMobilityTestResponseIgnorePreferredCarrier);
//			if (!handled)
//#endif //__MOBILITY_TEST_FRAMEWORK
			iCommsMobilityApi->IgnorePreferredCarrier();
			break;
			}
		case EMobilityUninitialised:
		case EMobilityIdle:
		case EMobilityWaitForOldSocketClose:
		case EMobilityWaitForOldSocketCloseCarrierRejected:
		case EMobilityWaitForMigrate:
		case EMobilityConnectingToNewCarrier:
		default:
			{
			// unexpected in this state
			__ASSERT_DEBUG(EFalse, gPanic(EPanicInvalidMobilityState));
			}
		}
	}

/**
Notification from the server MTM that existing sockets have been closed and that
it is ready for migration to the new carrier to occur.
*/
EXPORT_C void CImMobilityManager::MigrateToNewCarrier()
	{
	__LOG_FORMAT((KDefaultLog, "<%d> MigrateToNewCarrier(state:%d)", iAccountId, iState));
	// otherwise... switch on state.
	switch (iState)
		{
		case EMobilityWaitForOldSocketClose:
			{
			iState = EMobilityWaitForMigrate;
//#ifdef __MOBILITY_TEST_FRAMEWORK
//			TBool handled = CImMobilityTestFramework::Response(*this, KMobilityTestResponseMigrateToPreferredCarrier);
//			if (!handled)
//endif //__MOBILITY_TEST_FRAMEWORK
			iCommsMobilityApi->MigrateToPreferredCarrier();
			break;
			}
		case EMobilityWaitForUpgradePolicyAndOldSocketClose:
			{
			// ahha! we have since received a notice that an even better
			// carrier has come along. Wait for what we should do.
			iState = EMobilityWaitForUpgradePolicy;
			break;
			}
		case EMobilityWaitForOldSocketCloseCarrierRejected:
			{
			// instruct the MTM to reconnect - using the original AP
			// This is effectively the same as making an initial connection
			// so slip to default connected state.
			iState = EMobilityIdle;
			iMtm.NewCarrierActive(iOldAp, iIsSeamless);
			break;
			}
		case EMobilityUninitialised:
		case EMobilityIdle:
		case EMobilityWaitForUpgradePolicy:
		case EMobilityWaitForDowngradePolicy:
		case EMobilityWaitForMigrate:
		case EMobilityConnectingToNewCarrier:
		default:
			{
			// unexpected in this state
			__ASSERT_DEBUG(EFalse, gPanic(EPanicInvalidMobilityState));
			}
		}
	}

/**
Notification from the server MTM that it has successfully re-established connection
with the remote server using the new carrier.
*/
EXPORT_C void CImMobilityManager::NewCarrierAccepted()
	{
	__LOG_FORMAT((KDefaultLog, "<%d> NewCarrierAccepted(state:%d)", iAccountId, iState));
	switch (iState)
		{
		case EMobilityIdle:
			{
			// Do nothing. This might occur if previous state
			// was EMobilityWaitForOldSocketCloseCarrierRejected
			// the mobility framework is not expecting a response.
			break;
			}
		case EMobilityConnectingToNewCarrier:
			{
			iState = EMobilityIdle;
//#ifdef __MOBILITY_TEST_FRAMEWORK
//			TBool handled = CImMobilityTestFramework::Response(*this, KMobilityTestResponseNewCarrierAccepted);
//			if (!handled)
//#endif //__MOBILITY_TEST_FRAMEWORK
			iCommsMobilityApi->NewCarrierAccepted();
			// Tell the policy provider about it.
			CImMobilityPolicyProvider::GetRef().MigrationComplete(iAccountId, KImMigrateOK, iNewAp);
			break;
			}
		case EMobilityUninitialised:
		case EMobilityWaitForUpgradePolicy:
		case EMobilityWaitForDowngradePolicy:
		case EMobilityWaitForUpgradePolicyAndOldSocketClose:
		case EMobilityWaitForOldSocketCloseCarrierRejected:
		case EMobilityWaitForOldSocketClose:
		case EMobilityWaitForMigrate:
		default:
			{
			// unexpected in this state
			__ASSERT_DEBUG(EFalse, gPanic(EPanicInvalidMobilityState));
			}
		}
	}

/**
Notification from the server MTM that it has been unable to re-established connection
with the remote server using the new carrier.
*/
EXPORT_C void CImMobilityManager::NewCarrierRejected()
	{
	__LOG_FORMAT((KDefaultLog, "<%d> NewCarrierRejected(state:%d)", iAccountId, iState));
	switch (iState)
		{
		case EMobilityIdle:   // this results from the initial carrier being rejected.
		case EMobilityConnectingToNewCarrier:
			{
			iState = EMobilityIdle;
//#ifdef __MOBILITY_TEST_FRAMEWORK
//			TBool handled = CImMobilityTestFramework::Response(*this, KMobilityTestResponseNewCarrierRejected);
//			if (!handled)
//#endif //__MOBILITY_TEST_FRAMEWORK
			iCommsMobilityApi->NewCarrierRejected();
			// tell the policy provider about it.
			CImMobilityPolicyProvider::GetRef().MigrationComplete(iAccountId, KImMigrateCouldNotReconnect, iNewAp);
			break;
			}
		case EMobilityUninitialised:
		case EMobilityWaitForUpgradePolicy:
		case EMobilityWaitForDowngradePolicy:
		case EMobilityWaitForUpgradePolicyAndOldSocketClose:
		case EMobilityWaitForOldSocketCloseCarrierRejected:
		case EMobilityWaitForOldSocketClose:
		case EMobilityWaitForMigrate:
		default:
			{
			// unexpected in this state
			__ASSERT_DEBUG(EFalse, gPanic(EPanicInvalidMobilityState));
			}
		}
	}
