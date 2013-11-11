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
* Description:   The MClockEComObserver class, observes for change in
*                ecom registry of timesource interface.
*
*/

#ifndef __CLOCK_ECOM_OBSERVER_H__
#define __CLOCK_ECOM_OBSERVER_H__

// Class declaration
/**
* @class MClockEComObserver
* @brief The observer of ecom registry changes. 
*	     This the derived class needs to implement the functions below and will be notified by CClockEComListener.
* @see CClockEComListener
*/
class MClockEComObserver
	{
	public:			// Pure virtual functions.
	
		/**
		* @brief This has to be implemented by the derived class.
		* @param aImplInfo The implementation information of the plugin which was changed.
		* @param aInstalled ETrue if a plugin has been installed, EFalse otherwise.
		*/
		virtual void NotifyPluginInstallUninstallL( const CImplementationInformation& aImplInfo, TBool aInstalled ) = 0;
	};

#endif 			// __CLOCK_ECOM_OBSERVER_H__

// End of file
