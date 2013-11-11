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
* Description:  Implementation of main.cpp
*
*/

// Version : %version: 11.1.3 %


#include <QObject>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <xqserviceutil.h>
#include <hbtranslator.h> 

#include "videoplayerengine.h"
#include "videoplayerapp.h"

int main(int argc, char *argv[])
{
    //
    // has the application been launched via XQ Service Framework
    //
    bool isAservice(XQServiceUtil::isService(argc, argv));

    VideoPlayerApp app( argc, argv, isAservice ? Hb::NoSplash : Hb::SplashFixedVertical );

    // 
    // automatically loads & installs corresponding translation file
    //
    HbTranslator translator("videos");
    translator.loadCommon();
    
    if ( !isAservice )
    {
        app.setApplicationName( hbTrId("txt_videos_title_videos") );
    }

    HbMainWindow mainWindow( 0, Hb::WindowFlagTransparent );
    QObject::connect(&mainWindow, SIGNAL(viewReady()), &app, SLOT(viewReadySlot()));

    VideoPlayerEngine *engine = new VideoPlayerEngine( isAservice );
    engine->initialize();
    mainWindow.show();
    return app.exec();
}
