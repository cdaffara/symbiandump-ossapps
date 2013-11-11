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
* Description:  Implementation of HtiAdmin main.
*
*/

#include <hbmainwindow.h>
//#include <hbview.h>
#include <coecntrl.h>
#include <hbapplication.h>


#include "mainview.h"
#include "htienginewrapper.h"





int main(int argc, char *argv[])
{
	// Initialization
    HbApplication app(argc, argv);
    
    // Create HtiEngineWrapper
    HtiEngineWrapper engineWrapper;
    
    if (!engineWrapper.init()) {
            return EXIT_FAILURE;
        }
 
    // Main window widget. 
    // Includes decorators such as signal strength and battery life indicator.
    HbMainWindow mainWindow;

    //Create main view
	MainView* view = new MainView(mainWindow, engineWrapper);

	//initialize main view
	view->init(app);
	
	// Add View to main window and reveal it.
	mainWindow.addView(view);

    // Show widget
    //mainWindow.show();
    mainWindow.showFullScreen();

    int result = app.exec();
      
    return result;
}
