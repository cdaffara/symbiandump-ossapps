/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   The MClockPluginObserver class for observing plugin data changes'.
*
*/

#ifndef __CLOCK_PLUGIN_OBSERVER_H__
#define __CLOCK_PLUGIN_OBSERVER_H__

// User includes
#include "clocktimesourceinterface.h"

/** 
* @class MClockPluginObserver
* @brief This is an observer class derived by objects which need to be notified
*        by the ECOM implementations of type CClockTimeSourceInterface.
* @exe clockserver
*/
class MClockPluginObserver
	{
	public:            // Pure virtual functions

		/**
		* @brief Pure virtual, needs to be implemented by the derived class.
		*        Called when an implementation needs to notify upon a change.
		*/
		virtual void NotifyTimeChangeL( CClockTimeSourceInterface& aPluginImpl  ) = 0;

	};

#endif		// __CLOCK_PLUGIN_OBSERVER_H__

// End of file
