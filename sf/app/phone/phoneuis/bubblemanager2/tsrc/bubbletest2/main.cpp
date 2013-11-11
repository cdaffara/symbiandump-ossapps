/*!
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
* Description:  Animated icon.
*
*/

#include <hbapplication.h>
#include <hbmainwindow.h>

#include "bubbletestview.h"

int main(int argc, char *argv[])
{
    // Initialization
    HbApplication app(argc, argv);

    // Main window widget. 
    // Includes decorators such as signal strength and battery life indicator.
    HbMainWindow mainWindow;

    // View
    BubbleTestView *view = new BubbleTestView(mainWindow);
    mainWindow.addView(view);

    // Show widget
    mainWindow.setAttribute( Qt::WA_InputMethodEnabled, false ); 
    mainWindow.show();
    
    mainWindow.scene()->setFocusItem( view );

    // Enter event loop
    return app.exec();
}
