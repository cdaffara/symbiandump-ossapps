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


#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbview.h>

#include "mainview.h"
#include "enginewrapper.h"

int main(int argc, char *argv[])
{
	
    

    HbApplication app(argc, argv);
    HbMainWindow mainWindow;
    
    //Create main view
    MainView* view = new MainView(mainWindow);

    //initialize main view
    view->init(app);
	
	// Add View to main window and reveal it.
    mainWindow.addView(view);
    mainWindow.show();
    
    int result = app.exec();
   
    return result;
}

