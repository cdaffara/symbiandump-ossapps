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
#include <launchermainwindow.h>


int main(int argc, char *argv[])
{

    // Create application, main window.
    HbApplication app(argc, argv);
    // Create main window
    LauncherMainWindow mainWindow;
    // init main window
    int error = mainWindow.init(app);
    if(error)
        return error;
    // enter event loop
    return app.exec();
}

