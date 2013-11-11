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

#include "FeedTimer.h"
#include "FeedEngine.h"

CFeedTimer::CFeedTimer(CFeedEngine *aFeedEngine) : CTimer(EPriorityIdle), iFeedEngine(aFeedEngine) 
	{
	CActiveScheduler::Add(this);
	}

CFeedTimer::~CFeedTimer() 
	{
	}

void CFeedTimer::ConstructL() 
	{
	CTimer::ConstructL();
	}

void CFeedTimer::RunL() 
	{
	DP("CFeedTimer::RunL BEGIN");

	TTime time;
	time.UniversalTime();

	if (time < iTriggerTime)
		{
		// timer was probably reset, this happens on Nokia every 30 minutes
		DP("Timer reset");
		AtUTC(iTriggerTime);
		}
	else 
		{
		// We need to trap this, otherwise we will not reschedule the timer
		TRAP_IGNORE(iFeedEngine->UpdateAllFeedsL(ETrue));
	
		// run again
		RunPeriodically();
		}
	DP("CFeedTimer::RunL END");
	}

void CFeedTimer::SetPeriod(TInt aPeriodMinutes) 
	{
	DP1("Setting sync period to %d", aPeriodMinutes);
	iPeriodMinutes = aPeriodMinutes;
	}

void CFeedTimer::SetSyncTime(TTime aTime) 
	{
	TTime time;
	time.HomeTime();

	DP5("Now is %4d-%02d-%02d, %02d:%02d", time.DateTime().Year(), time.DateTime().Month()+1, time.DateTime().Day()+1, time.DateTime().Hour(), time.DateTime().Minute());

	TInt hour = aTime.DateTime().Hour();
	TInt minute = aTime.DateTime().Minute();
	
	
	TDateTime dTime;
	
	dTime.Set(time.DateTime().Year(), time.DateTime().Month(),
			time.DateTime().Day(),aTime.DateTime().Hour(),
			aTime.DateTime().Minute(), 0, 0);

	TTimeIntervalMinutes tmi = 0;

	// if this time already passed, add one day
	if (time.DateTime().Hour() > hour || time.DateTime().Hour() == hour && time.DateTime().Minute() > minute) 
		{
		DP("Adding one day");
		tmi = 60*24;
		}
	
	TTime atTime(dTime);
	atTime = atTime + tmi;
	DP5("Setting sync timer to %4d-%02d-%02d, %02d:%02d", atTime.DateTime().Year(), atTime.DateTime().Month()+1, atTime.DateTime().Day()+1, atTime.DateTime().Hour(), atTime.DateTime().Minute());

	// Activate the timer
	At(atTime);
	}
	

void CFeedTimer::RunPeriodically() 
	{
	DP1("RunPeriodically; thePeriod=%d", iPeriodMinutes);
	TTime time;
	time.UniversalTime();

	TTimeIntervalMinutes tmi;
	tmi = iPeriodMinutes;
	iTriggerTime = time + tmi;
	DP("Running timer");

	AtUTC(iTriggerTime);
	}
