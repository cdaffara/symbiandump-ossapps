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

#ifndef __MIMMOBILITYPOLICYHANDLER_H__
#define __MIMMOBILITYPOLICYHANDLER_H__

#include <e32std.h>
#include "timmobilityaction.h"

/**
The MImMobilityPolicyHandler API provides support to handle responses from
the Messaging Mobility Policy Provider/Plugin.

These APIs are called by the Messaging Mobility Policy Provider to indicate
the action that should be taken by the server MTM subsequent to a 
PreferredCarrierAvailable() notification having been issued.

@internalTechnology
@prototype
*/
class MImMobilityPolicyHandler
	{
public:
	/**
	The new carrier will be accepted.
	The passed parameter indicates the action that shall be taken if an
	operation is currently in progress and for closing existing sockets.
	In the case of a bearer downgrade, the action will have no meaning
	as the previous sockets will no longer be connected. In this case an
	immdiate migration will occur.
	@param aAction The disconnection method to use.
	*/
	virtual void AcceptNewCarrier(TImMobilityAction aAction) =0;

	/**
	The new carrier shall be rejected.
	*/
	virtual void IgnoreNewCarrier() =0;
	};

#endif	// __MIMMOBILITYPOLICYHANDLER_H__
