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

#include <QtGui/QMessageBox>

#include <hbmainwindow.h>
#include <hbview.h>
#include <hbmessagebox.h>
#include <coecntrl.h>

#include "mainview.h"
#include "application.h"
#include "enginewrapper.h"
#include "settingsview.h"



int main(int argc, char *argv[])
{
    
    SGApplication app(argc, argv);
	
    EngineWrapper engineWrapper;
	app.SetEngineWrapper(&engineWrapper);
    if (!engineWrapper.init()) {
      return EXIT_FAILURE;
    }

	
	
    HbMainWindow mainWindow;
    
    //Create main view
    MainView* view = new MainView(mainWindow, engineWrapper);
    //initialize main view
    view->init(app);

	
    SettingsView* settingsView = new SettingsView(*view, mainWindow, engineWrapper);
	
	view->setSettingsView(settingsView);
    
	// Add Views to main window and reveal it.
    mainWindow.addView(view);
    mainWindow.addView(settingsView);

    
    mainWindow.show();
     
    int result = app.exec();
   
    return result;
}

