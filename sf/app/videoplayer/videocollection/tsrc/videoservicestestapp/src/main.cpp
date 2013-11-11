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
#include "videoservicestestappview.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Nokia");
    QCoreApplication::setApplicationName("VideoServicesTestApp");

    HbApplication app(argc, argv);
    HbMainWindow mainWindow( 0, Hb::WindowFlagTransparent );
    mainWindow.setAttribute( Qt::WA_OpaquePaintEvent );
    VideoServicesTestAppView *mainView = new VideoServicesTestAppView;
    mainWindow.addView(mainView);
    mainWindow.show();
    return app.exec();
}
