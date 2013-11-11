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
* Description: This is the Definition file for ClockAppController class.
*
*/

// User includes
#include "clockappcontroller.h"
#include "clockappcontrollerifimpl.h"
#include "clockviewmanager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockappcontrollerTraces.h"
#endif


/*!
	\class ClockAppController

	This is the heart of clock application. It constructs and owns the
	ClockViewManager objects.
 */

/*!
	Default constructor.
 */
ClockAppController::ClockAppController(QObject *parent)
:QObject(parent),
 mViewManager(0),
 mIfImpl(0)
{
	OstTraceFunctionEntry0( CLOCKAPPCONTROLLER_CLOCKAPPCONTROLLER_ENTRY );
	// Construct the interface implementation.
	mIfImpl = new ClockAppControllerIfImpl(this, this);

	// Construct the view manager.
	mViewManager = new ClockViewManager(*mIfImpl, this);
	Q_ASSERT_X(
			mViewManager, "clockappcontroller.cpp",
			"ClockViewManager is 0");
	connect(mViewManager, SIGNAL(appReady()), this, SLOT(handleAppReady()));
	OstTraceFunctionExit0( CLOCKAPPCONTROLLER_CLOCKAPPCONTROLLER_EXIT );
}

/*!
	Destructor.
 */
ClockAppController::~ClockAppController()
{
	OstTraceFunctionEntry0( DUP1_CLOCKAPPCONTROLLER_CLOCKAPPCONTROLLER_ENTRY );
	if (mViewManager) {
		delete mViewManager;
		mViewManager = 0;
	}
	if (mIfImpl) {
		delete mIfImpl;
		mIfImpl = 0;
	}
	OstTraceFunctionExit0( DUP1_CLOCKAPPCONTROLLER_CLOCKAPPCONTROLLER_EXIT );
}

/*!
	Emits the appReday signal.
 */
void ClockAppController::handleAppReady()
{
	OstTraceFunctionEntry0( CLOCKAPPCONTROLLER_HANDLEAPPREADY_ENTRY );
	emit appReady();
	disconnect(mViewManager, SIGNAL(appReady()), this, SLOT(handleAppReady()));
	OstTraceFunctionExit0( CLOCKAPPCONTROLLER_HANDLEAPPREADY_EXIT );
}

// End of file	--Don't remove this.
