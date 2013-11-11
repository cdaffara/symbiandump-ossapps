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
* Description:  Offers message creation and sending services.
 *
*/

#include <hbapplication.h>
#include <hbmainwindow.h>
#include "testshareuimainwindow.h"

int main(int argc, char *argv[])
{
    // Application
    HbApplication app(argc, argv);

    TestShareUiMainWindow* mainWin = new TestShareUiMainWindow();
    mainWin->init();
    mainWin->show();
    // Event loop
    int error = app.exec();

    // delete main window and return error
    delete mainWin;
    return error;
}

// End of file
