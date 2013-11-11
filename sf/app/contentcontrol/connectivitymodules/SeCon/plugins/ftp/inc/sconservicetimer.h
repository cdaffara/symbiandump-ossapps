/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  File Transfer Profile Plug-in Service Timer Header file
*
*/


#ifndef _SCONSERVICETIMER_H
#define _SCONSERVICETIMER_H

#include <e32base.h>

class CSConFTPplugin;


//============================================================
// Class CSConServiceTimer declaration
//============================================================

NONSHARABLE_CLASS ( CSConServiceTimer ) : public CActive
	{
	public:
		CSConServiceTimer( CSConFTPplugin* aEngine, TInt aValue );
		void ConstructL();
		~CSConServiceTimer();
		
		/**
	 	* Starts the timer
		* @return none
	 	*/
		void StartTimer();
		/**
		 * Stops the timer
		 * @return none
		 */
		void StopTimer();
	
	private:
		void DoCancel();
		void RunL();
		
	private:
		RTimer 				iTimer;
		CSConFTPplugin* 	iEngine;
		TInt 				iValue;
	};
	
#endif //_SCONSERVICETIMER_H

// End of file
