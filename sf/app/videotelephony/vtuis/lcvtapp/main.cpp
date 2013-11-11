/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
//SYSTEM
#include <lcviewmanager.h>
#include <QLocale>
#include <QTranslator>
#include <eikenv.h>
#include <apgwgnam.h>

//USER
#include "cvtlogger.h"
#include "lcapplication.h"


int main(int argc, char *argv[])
{
    LcHbApplication app(argc, argv);    
    CVtLogger::Initialize(); // Cannot do before app creation as activescheduler is installed at that stage
    __VTPRINT2(CVtLogger::ELogToFile, "videotelui.exe::main entered: argc=%d ",argc);
    if (argc > 0)
    {
        QString arg1Str(QString::fromUtf8(argv[0]));
        TPtrC arg1(arg1Str.utf16());
        CVtLogger::Print(CVtLogger::ELogToFile,arg1);
    }  
       
    //Load application-specific translator
    QString lang = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    QTranslator translator;
    if ( translator.load( path + "video_telephone_" + lang + ".qm") ){
        app.installTranslator(&translator);
    } else {
        __VTPRINT( DEBUG_GEN, "videotelui.exe::main, couldn't load translation" );
    }
    
    // Temporarily set VT as system app as otherwise GOOM memory releasing will shut
    // down the process when running low on graphics mem.
    CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC(CEikonEnv::Static()->WsSession());
    wgName->SetSystem(ETrue); // Allow only application with PowerManagement cap to shut us down
    wgName->SetCaptionL(_L("Videotelephony"));
    wgName->SetAppUid(TUid::Uid(0x101F8681));
    wgName->SetWindowGroupName(CEikonEnv::Static()->RootWin());
    CleanupStack::PopAndDestroy();
    
    QString engineName("Videotelephony");
    QString applicationName("Video Telephone");
    
    LcViewManager viewManager(engineName,applicationName);
 
    QObject::connect( &app, SIGNAL( handleQuit() ), 
             &viewManager, SLOT( terminateSession() ),
             Qt::QueuedConnection );

    int err = app.exec();
    __VTPRINT2(CVtLogger::ELogToFile, "videotelui.exe::main returns %d ",err);
    CVtLogger::Uninitialize();
    return err;
}
