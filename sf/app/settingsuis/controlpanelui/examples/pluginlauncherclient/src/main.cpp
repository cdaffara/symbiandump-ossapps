/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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
#include <QDir>
#include <hbmainwindow.h>
#include <hbstyleloader.h>
#include "mainview.h"

int main(int argc, char **argv)
{
    HbApplication app(argc, argv);
    
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem.css");
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem_color.css");
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem.widgetml");
    
    HbMainWindow mainWindow;
    MainView *mainView = new MainView();
    mainWindow.addView(mainView);
    mainWindow.show();
    
    return app.exec();
}

//End of File

