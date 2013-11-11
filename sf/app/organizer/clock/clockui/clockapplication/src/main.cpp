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
* Description: Contains main, the entry point for clock application.
*
*/

// System includes
#include <QtGui>
#include <HbTranslator>
#include <HbMainWindow>

// User includes
#include "clockapplication.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mainTraces.h"
#endif


/*!
	The main entry point of the clock application.

	Constructs the view manager object.
 */
int main(int argc, char *argv[])
{
	OstTraceFunctionEntry0( _MAIN_ENTRY );
	// Create and initialize an ClockApplication instance
	QScopedPointer<ClockApplication> application(
				new ClockApplication(argc, argv));
	
	// Main window widget.
	// Includes decorator such as signal strength and battery life indicator.
	HbMainWindow window;
	window.setRenderHints(
			QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);
	window.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

	// Load the translation file.
	HbTranslator translator("clock");
	translator.loadCommon();
	
	// Construct the application controller.
	application->createController();

	// Show widget
	window.show();

	// Start the event loop for the application
	return application->execution();
}

// End of file	--Don't remove this.
