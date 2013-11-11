/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
//#include <qdatetime.h>
#include <hbmainwindow.h>

#include <CalenLauncher>
#include <CalenEditor>

#include "calencontroller.h"
#include "calenserviceprovider.h"

CalenServiceProvider::CalenServiceProvider(CCalenController *controller, QObject* parent) :
 XQServiceProvider(QLatin1String("com.nokia.services.calendar.Launch"), parent)
{
	publishAll();
	mController = controller;
	//Q_IMPLEMENT_USER_METATYPE_ENUM(CalenView);
}

CalenServiceProvider::~CalenServiceProvider()
{
}

void CalenServiceProvider::launchCalendarApp(const QDateTime& date, int viewId)
{	
	
	// Check the view ID and launch the corresponding view
	switch(viewId) {
		case CalenLauncher::MonthView:
			// Launch the month view
			mController->handleServiceManagerSlot(ECalenMonthView, date);
			break;
		case CalenLauncher::AgendaView:
			// Launch the agenda view
			mController->handleServiceManagerSlot(ECalenAgendaView, date);
			break;
		case CalenLauncher::DayView:
	    // Launch the day view
	    mController->handleServiceManagerSlot(ECalenDayView, date);
	    break;
		default:
			// Keep Quiet
			break;
	}
}

// End of file	--Don't remove this.

