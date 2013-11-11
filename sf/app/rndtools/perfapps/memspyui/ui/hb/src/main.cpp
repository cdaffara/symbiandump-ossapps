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
* Description:
*  
*/

#include <HbApplication>
#include <QMessageBox>
#include <HbMainWindow>

#include "enginewrapper.h"
#include "memspyview.h"
#include "memspyprocessview.h"
#include "viewmanager.h"

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
	
    EngineWrapper engine;
    if (!engine.initialize()) {
		QMessageBox::critical(0, "Error", "Engine failed to initialize. Closing.");
		return 1;
    }
        
    HbMainWindow window;
    ViewManager viewManager(window, engine);
    viewManager.showView(MainView);
	
    window.show();
	return app.exec();
}
