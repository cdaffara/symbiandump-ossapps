/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar's async callback
*
*/


#ifndef CALENASYNCCALLBACK_H
#define CALENASYNCCALLBACK_H

// INCLUDES
#include <e32base.h>
#include "calencontroller.h"

// CLASS DEFINITIONS
/**
 * Calendar async Callback
 */	
class CalenAsyncCallBack : public CAsyncCallBack
	{
	public: 
		static CalenAsyncCallBack* NewL(TCallBack& aCallBack, CActive::TPriority aPriority,
                                             CCalenController& aController);
		void BaseConstructL();

		virtual ~CalenAsyncCallBack();
	
	private:    
	    CalenAsyncCallBack(TCallBack& aCallBack, CActive::TPriority  aPriority,
                          CCalenController& aController);
      
	public: // Function from base class
	
		/**
		 * Handle Completion 
		 */
		void RunL();
		
        /**
        * Return reference to TRequestStatus member.
        */
        TRequestStatus& Status();
		
		//void Set(TCallBack& aCallBack);
	private:
		CCalenController& iController;
	};



#endif //CALENASYNCCALLBACK_H
