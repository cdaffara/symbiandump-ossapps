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
 *     The main window file for tone service test
 *     
 */
#include "tonetestapp.h"

#include <hbapplication.h>
#include <hbmainwindow.h>

int main(int argc, char *argv[])
{

    QCoreApplication::setOrganizationName("Nokia");
    QCoreApplication::setApplicationName("ToneTestApp");
      
    HbApplication app(argc, argv);
    HbMainWindow mainWindow;
    ToneTestApp *mainView = new ToneTestApp;
    mainWindow.addView(mainView);
    mainWindow.show();
    return app.exec();
}
