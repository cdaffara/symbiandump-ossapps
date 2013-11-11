/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef FEEDTIMER_H
#define FEEDTIMER_H

#include <e32base.h>

class CFeedEngine;

class CFeedTimer : public CTimer 
	{
	public:
		CFeedTimer(CFeedEngine *aFeedEngine);
		~CFeedTimer();
		void ConstructL();
		void RunL();
		void RunPeriodically();
		void SetPeriod(TInt aPeriodMinutes);
		void SetSyncTime(TTime aTime);
	
	private:
		TInt iPeriodMinutes;
		TTime iTriggerTime;
		CFeedEngine *iFeedEngine;
	};
#endif

