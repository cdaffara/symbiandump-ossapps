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
* Description: Contains main, the entry point for notes application.
*
*/

// System includes
#include <hbmainwindow.h>
#include <hbtranslator.h>

// User includes
#include "notesapplication.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mainTraces.h"
#endif


/*!
	The main() function.

	Responsible for constructing the NotesApplication object and showing the
	main window.
 */
int main(int argc, char *argv[])
{
	OstTraceFunctionEntry0( _MAIN_ENTRY );
	// Create and initialize an NotesApplication instance
	QScopedPointer<NotesApplication> application(
			new NotesApplication(argc, argv));
	//NotesApplication *application = new NotesApplication(argc, argv);

	// Main window for providing the scene context
	HbMainWindow window;
	window.setRenderHints(
			QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	window.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

	// Load the translation file.
	HbTranslator notesViewsTranslator("notes");
	notesViewsTranslator.loadCommon();

	// Construct the application controller.
	application->createController();
	
	// Show the main window.
	window.show();
	
	// Start the event loop for the application
	return application->execution();
}

// End of file	--Don't remove this.
