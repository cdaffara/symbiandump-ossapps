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
* Description: Definition file for class CalenApplication.
*
*/

#include "calencontroller.h"
#include "calenapplication.h"
#include "calenserviceprovider.h"

/*!
	CalenApplication
	Responsible for creating the CalenController object.
 */

/*!
	constructor.
 */
CalenApplication::CalenApplication(int &argc, char *argv[]):
 HbApplication(argc, argv)
{
	
}

/*!
	Destructor.
 */
CalenApplication::~CalenApplication()
{
	if (mController) {
		delete mController;
		mController = 0;
	}
}

/*!
	Creates the calencontroller object.
 */
void CalenApplication::createController()
{
	mController = new CCalenController();
	if (mController) {
		mController->constructController();
		connect(mController, SIGNAL(appReady()), this, SLOT(handleAppReady()));
		// Create the Calendar service provider
		mService = new CalenServiceProvider(mController, this);
	}
}

/*!
	Start the event loop for the application.
 */
bool CalenApplication::execution()
{
	int ret = 0;
	ret = exec();
	return ret;
}

/*!
	Deletes the controller.
 */
void CalenApplication::releaseController()
{
	// delete the controller
	mController->ReleaseCustomisations();
	mController->Release();
	
}

/*
	Emits the applicationReady signal.
 */
void CalenApplication::handleAppReady()
{
	emit applicationReady();
	disconnect(mController, SIGNAL(appReady()), this, SLOT(handleAppReady()));
}

// End of file	--Don't remove this.
