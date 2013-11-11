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
* Description: This is the Definition file for NotesAppController class.
*
*/

// User includes
#include "notesappcontroller.h"
#include "notesappcontrollerifimpl.h"
#include "notesviewmanager.h"
#include "notesmodelhandler.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notesappcontrollerTraces.h"
#endif


/*!
	\class NotesAppController

	This is the heart of notes application. It constructs and owns the
	NotesViewManager and NotesModelHandler objects.
 */

/*!
	Default constructor.
 */
NotesAppController::NotesAppController(QObject *parent)
:QObject(parent),
 mViewManager(0),
 mNotesModelHandler(0),
 mIfImpl(0)
{
	OstTraceFunctionEntry0( NOTESAPPCONTROLLER_NOTESAPPCONTROLLER_ENTRY );
	// Construct the interface implementation.
	mIfImpl = new NotesAppControllerIfImpl(this);

	// Construct the model handler.
	mNotesModelHandler = new NotesModelHandler(this);
	Q_ASSERT_X(
			mNotesModelHandler, "notesappcontroller.cpp",
			"NotesModelHandler is 0");

	// Construct the view manager.
	mViewManager = new NotesViewManager(*mIfImpl, this);
	Q_ASSERT_X(
			mViewManager, "notesappcontroller.cpp",
			"NotesViewManager is 0");
	connect(mViewManager, SIGNAL(appReady()), this, SLOT(handleAppReady()));
	OstTraceFunctionExit0( NOTESAPPCONTROLLER_NOTESAPPCONTROLLER_EXIT );
}

/*!
	Destructor.
 */
NotesAppController::~NotesAppController()
{
	OstTraceFunctionEntry0( DUP1_NOTESAPPCONTROLLER_NOTESAPPCONTROLLER_ENTRY );
	if (mViewManager) {
		delete mViewManager;
		mViewManager = 0;
	}
	if (mNotesModelHandler) {
		delete mNotesModelHandler;
		mNotesModelHandler = 0;
	}
	if (mIfImpl) {
		delete mIfImpl;
		mIfImpl = 0;
	}
	OstTraceFunctionExit0( DUP1_NOTESAPPCONTROLLER_NOTESAPPCONTROLLER_EXIT );
}

/*!
	Emits the appReday signal.
 */
void NotesAppController::handleAppReady()
{
	OstTraceFunctionEntry0( NOTESAPPCONTROLLER_HANDLEAPPREADY_ENTRY );
	emit appReady();
	disconnect(mViewManager, SIGNAL(appReady()), this, SLOT(handleAppReady()));
	OstTraceFunctionExit0( NOTESAPPCONTROLLER_HANDLEAPPREADY_EXIT );
}

// End of file	--Don't remove this.
