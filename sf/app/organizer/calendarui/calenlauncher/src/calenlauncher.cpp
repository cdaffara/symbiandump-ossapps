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
* Description:  Calendar launcher
*
*/
// System includes
#include <xqservicerequest.h>

// User includes
#include <calenlauncher.h>

/*!
	\class CalenLauncher
 */
/*!
	Constructor.

	\param parent QObject pointer.
 */
CalenLauncher::CalenLauncher(QObject* parent)
:QObject(parent)
{
}

/*!
  Destructor
 */
CalenLauncher::~CalenLauncher()
{
}

/*!
	API to launch the calendar application
	\param view Claendar view to be launches
	\param dateTime Date pertaining to which view has to be populated
	\param synchronousLaunch bool value to tell whethe calendar has
			to be launched synchronously or asynchronously
 */
void CalenLauncher::launchCalendarApp(CalenView view, 
									QDateTime& dateTime, 
									bool synchronousLaunch)
{
	// launch the calendar application
	XQServiceRequest request("com.nokia.services.calendar.Launch", 
	                         "launchCalendarApp(QDateTime,int)",synchronousLaunch);
	// Pass the arguments needed
	request << dateTime;
	request << ((int)view);
	
	int retValue;
	bool res = request.send(retValue);
	if (!res && synchronousLaunch) {
		int error = request.latestError();
		// Emit the signal with the error id to the caller
		emit calendarLaunchFailed(error);
	}
	
	if (!synchronousLaunch) {
		// Connect to the signal for async request
		connect(&request, SIGNAL(requestError(int)), this, SLOT(handleError(int)));
	}
}																			

void CalenLauncher::handleError(int error)
{
	// emit the signal to the client
	emit calendarLaunchFailed(error);
}
// End of file	--Don't remove this.
