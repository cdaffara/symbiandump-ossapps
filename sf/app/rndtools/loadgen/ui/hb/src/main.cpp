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

#include <hbmainwindow.h>
#include <hbapplication.h>
#include <eikenv.h>
#include "mainview.h"


int main(int argc, char *argv[])
{
	HbApplication app(argc, argv);
    HbMainWindow mainWindow;
    
    // set as system application to prevent getting shut down events 
    CEikonEnv::Static()->SetSystem( ETrue );
    
    //Create main view
    MainView* view = new MainView(mainWindow);

    //initialize main view
    view->init(app);
	// Add Views to main window and reveal it.
    mainWindow.addView(view);
    mainWindow.show();

    return app.exec();
}
