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

#ifndef __MIMMOBILESERVER_H__
#define __MIMMOBILESERVER_H__

#include <e32std.h>
#include "timmobilityaction.h"

class TAccessPointInfo;

/**
The MImMobileServer API provides must be implemented by server MTMs that
are to support bearer mobility using an instance of CImMobilityManager.

These APIs are called by the Messaging Mobility Manager to instruct the 
server MTM to prepare for migration to a new carrier, and to notify the
server MTM when a new carrier is ready to create new connections.

@internalTechnology
@prototype
*/
class MImMobileServer
	{
public:
	/**
	The passed parameter indicates the action that shall be taken if an
	operation is currently in progress and for closing existing sockets.
	The operation is asynchronous. The mobility manager must be notified
	of completion by calling MigrateToNewCarrier();
	@param aAction The disconnection method to use.
	@param aIsSeamless Indicates if the migration will be seamless
	*/
	virtual void PrepareForNewCarrier(TImMobilityAction aAction, TBool aIsSeamless) =0;
	
	/**
	Called in the case of a downgrade, or if an immediate migration is
	required. Any current operations shall be stopped immediately and the
	sockets torn down.
	This is a synchronous operation. On completion of this method, the
	above must have been done
	*/
	virtual void CarrierLost() =0;

	/**
	Indicates the new carrier is active.
	*/
	virtual void NewCarrierActive(TAccessPointInfo aNewAp, TBool aIsSeamless) =0;
	
	/**
	Indicates an error has occurred associated with bearer mobility.
	@param aError - indicates the type of error.
	*/
	virtual void MobilityError(TUint aError) =0;
	
	/**
	Returns a packaged-up copy of the Server MTM's progress info.
	*/
	virtual const TDesC8& MobilityProgress() = 0;
	};

#endif	// __MIMMOBILESERVER_H__
