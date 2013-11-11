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

#include <hbapplication.h>
#include <hbmainwindow.h>
#include <QDebug>
#include "logsengineapitester.h"

int main(int argc, char **argv)
{
        
    qDebug() << "LogsEngineApiTester::main ->";
    
    
    HbApplication app(argc, argv);

    HbMainWindow mainWindow;

    TestView testView;
    mainWindow.addView(&testView);
    
    // Show widget
    mainWindow.show();
    
    // Enter event loop
    int err = app.exec();
    qDebug() << "LogsEngineApiTester::main <-";
    return err;
}

