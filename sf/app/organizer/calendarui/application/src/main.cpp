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
* Description:  main implementation.
*
*/

// system includes
#include <QtGui>
#include <hbmainwindow.h>
#include <hbtranslator.h>

// user includes
#include "calenapplication.h"

/*!
	The main() function.

	Responsible for constructing the CalenApplication object and showing the
	main window.
 */
int main(int argc, char *argv[])
{
	// Create and initialize an CalenApplication instance
	QScopedPointer<CalenApplication> application(
				new CalenApplication(argc, argv));
	//CalenApplication *application = new CalenApplication(argc, argv);

	// Main window for providing the scene context
	HbMainWindow window;
	// Flags to gain some optimizations from the framework
	// TODO: Check if orbit itself is setting these flags in mainwindow
	// If yes, then we need not set here
	window.setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);
    window.setRenderHint(QPainter::TextAntialiasing, false);

	
	//For translation, loading and installing translator
	HbTranslator translator("calendar");
	translator.loadCommon();
	
	// Backup and restore code need to write here.
	
	application->createController();

	int retValue = 0;
	
	retValue = application->execution();
	application->releaseController();
	
	return retValue;
}

//End of file

