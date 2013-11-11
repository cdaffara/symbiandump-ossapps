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
* Description:   The MClkSrvMccObserver class for MCC changes.
*
*/

#ifndef __CLOCKSERVER_MCC_OBSERVER_H__
#define __CLOCKSERVER_MCC_OBSERVER_H__

// Class declaration
/**
* @class MClkSrvMccObserver
* @brief The observer base class for Mcc changes.
* @exe clockserver.exe
*/
class MClkSrvMccObserver
	{
	public:

		/**
		* @brief Called when Mcc is available or there is a change in the mcc. Must be implemented by the 
		*		 derived class.
		*/
		virtual void NotifyMccChangeL() = 0;
	};

#endif		// __CLOCKSERVER_MCC_OBSERVER_H__

// End of file
