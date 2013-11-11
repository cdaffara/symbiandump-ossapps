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
#include "cimmobilitypolicydefault.h"
#include "timmobilityaction.h"
#include "mimmobilitypolicyhandler.h"

// Define the implementation table for Ecom
const TImplementationProxy ImplementationTable[] = 
	{
	// KUidImMobilityPolicy
	IMPLEMENTATION_PROXY_ENTRY(0x10283067, CImMobilityPolicyDefault::NewL)
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
CImMobilityPolicyDefault* CImMobilityPolicyDefault::NewL()
	{
	CImMobilityPolicyDefault* self = new (ELeave) CImMobilityPolicyDefault();	
	return self;
	}

/**
Class constructor
*/
CImMobilityPolicyDefault::CImMobilityPolicyDefault()
	{
	}

/**
Class destructor
*/
CImMobilityPolicyDefault::~CImMobilityPolicyDefault()
	{
	}

/**
Notification of change in preferred carrier availablity.
This default version immediately accepts a new bearer, with the
following action to be taken:
 upgrade:   Complete any current operation before migration
 downgrade: migrate immedidiately

@param aMobilityManager - callback interface to the mobility manager
@param aService     - indicates the service to which the notice applies
@param aOldAp		- info about the old carrier
@param aNewAp		- info about the new carrier
@param aIsUpgrade   - indicates if this is an upgrade
@param aIsSeamless  - indicates if a migration will be seamless
*/
void CImMobilityPolicyDefault::PreferredCarrierAvailable(MImMobilityPolicyHandler& aMobilityManager, TMsvId /*aService*/, TUid /*aMtmUid*/, const TDesC8& /*aProgress*/, TAccessPointInfo /*aOldIap*/, TAccessPointInfo /*aNewIap*/, TBool aIsUpgrade, TBool /*aIsSeamless*/)
	{
	if (aIsUpgrade)
		{
		aMobilityManager.AcceptNewCarrier(KAcceptCompleteCurrent);
		}
	else
		{
		// downgrade
		aMobilityManager.AcceptNewCarrier(KAcceptImmediately);
		}
	}

/**
Notification that a previously issued notification for which a
response has not yet been received is no longer valid.

The default implementation is a stub function as a response is
always provided immediately - there is never an outstanding
notification to be cancelled.

@param aService - indicates the service to which the notice applies.
*/
void CImMobilityPolicyDefault::Cancel(TMsvId /*aService*/)
	{
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
void CImMobilityPolicyDefault::MigrationComplete(TMsvId /*aService*/, TImMigrateComplete /*aMigrateCode*/, TAccessPointInfo /*aNewIap*/)
	{
	}
