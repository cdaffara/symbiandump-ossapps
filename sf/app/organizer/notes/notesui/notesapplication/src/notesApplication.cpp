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
* Description: Definition file for class NotesApplication.
*
*/


// User includes
#include "notesapplication.h"
#include "notesappcontroller.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notesApplicationTraces.h"
#endif


/*!
	\class NotesApplication

	This class creates the NotesController object.
 */
/*!
	Constructor.
 */
NotesApplication::NotesApplication(int &argc, char *argv[]):
 HbApplication(argc, argv)
{
	OstTraceFunctionEntry0( NOTESAPPLICATION_NOTESAPPLICATION_ENTRY );
	OstTraceFunctionExit0( NOTESAPPLICATION_NOTESAPPLICATION_EXIT );
}

/*!
	Destructor.
 */
NotesApplication::~NotesApplication()
{
	OstTraceFunctionEntry0( DUP1_NOTESAPPLICATION_NOTESAPPLICATION_ENTRY );
	if (mController) {
		delete mController;
		mController = 0;
	}
	OstTraceFunctionExit0( DUP1_NOTESAPPLICATION_NOTESAPPLICATION_EXIT );
}

/*!
	creates the NotesController object.
*/
void NotesApplication::createController()
{
	OstTraceFunctionEntry0( NOTESAPPLICATION_CREATECONTROLLER_ENTRY );
	mController = new NotesAppController;
	connect(mController, SIGNAL(appReady()), this, SLOT(handleAppReady()));
	OstTraceFunctionExit0( NOTESAPPLICATION_CREATECONTROLLER_EXIT );
}

/*!
	Start the event loop for the application.
*/
bool NotesApplication::execution()
{
	OstTraceFunctionEntry0( NOTESAPPLICATION_EXECUTION_ENTRY );
	int ret = 0;
	ret = exec();
	OstTraceFunctionExit0( NOTESAPPLICATION_EXECUTION_EXIT );
	return ret;
}

/*!
	Emits the applicationReady signal.
 */
void NotesApplication::handleAppReady()
{
	OstTraceFunctionEntry0( NOTESAPPLICATION_HANDLEAPPREADY_ENTRY );
	emit applicationReady();
	disconnect(mController, SIGNAL(appReady()), this, SLOT(handleAppReady()));
	OstTraceFunctionExit0( NOTESAPPLICATION_HANDLEAPPREADY_EXIT );
}

// End of file	--Don't remove this.
