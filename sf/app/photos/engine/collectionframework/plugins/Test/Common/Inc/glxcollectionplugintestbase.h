/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Album collection plugin definition
*
*/



#ifndef C_GLXCOLLECTIONPLUGINTESTBASE_H
#define C_GLXCOLLECTIONPLUGINTESTBASE_H

// INCLUDES

#include <e32cmn.h>
#include <mpxcollectionplugin.h>
#include "mglxasynccallbackobserver.h"
#include "glxasyncnotifier.h"

// FORWARD DECLARATIONS

class CGlxCollectionPluginTestBase;
class CGlxAsyncNotifier;

// CLASS DECLARATION

/**
* Glx Collection Plugin
* This class wraps the CGlxAsyncNotifier. It inherits from CMPXCollectionPlugin. and should be inherited by any plugin that requires it.
*/

class CGlxCollectionPluginTestBase : public CMPXCollectionPlugin, public MGlxAsyncCallBackObserver
	{
	protected:
    /**
    * AsyncNotify - Asynchronously notifies the observer (iObs defined in CMPXCollectionPlugin)
    *
    * @param aMPXMedia. The MPXMedia data to pass to the observer. 
    *					Ownership is passed to this class. Take note that it takes a reference to a pointer.
    *					This enables the class to set the pointer to zero, in the class that created it.
    *					Therefore the pointer must not reside on the stack. 
    * @param aError. Error to pass to the Observer.
    * @param aCallType, Identifies the type of call to complete. OpenL, MediaL, FindAllL...
    */
		void AsyncNotifyL(CMPXMedia*& aMPXMedia, TInt aError, CGlxAsyncNotifier::TCallType aCallType);
		
    /**
    * AsyncNotify - Asynchronously notifies the observer (iObs defined in CMPXCollectionPlugin)
    * This method is called only when no MPXMedia data is required to send to the observer (see FindAllL).
    * @param aError. Error to pass to the Observer.
    * @param aCallType, Identifies the type of call to complete. OpenL, MediaL, FindAllL...
    */
		void AsyncNotifyL(TInt aError, CGlxAsyncNotifier::TCallType aCallType);
		
	/**
	* Cancels any outstanding Async request
	*/
	virtual void CancelRequest();
	
	private:
		void AsyncCallBackCompleted(CGlxAsyncNotifier* aPtr);
		
	private:
		CGlxAsyncNotifier* 	iAsyncNotifier;		// The Async Notifier
		CMPXMedia*			iDummyMedia;		// A Dummy CMPXMedia. Used when sending errors to client when no real data is avaiable.
												// The Observer interface passes a reference to a CMPXMedia so one must exist, even if it's not needed.
	};

#endif	//C_GLXCOLLECTIONPLUGINTESTBASE_H
