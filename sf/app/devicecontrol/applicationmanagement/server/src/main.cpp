/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */


#include <hbapplication.h>
#include <hbtransparentwindow.h>
#include <hbstackedlayout.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <QTranslator>
#include <QLocale>
#include <e32property.h>
#include "ApplicationManagementCommon.h"
#include "debug.h"
#include "amview.h"
#include "ApplicationManagementServer.h"

using namespace NApplicationManagement;

int main(int argc, char *argv[])
    { 
    RDEBUG( "ApplicationManagementServer: main" );

    HbApplication app(argc, argv);
    
    AMView* view = new AMView();
    view->hideItems(Hb::AllItems);
    view->setContentFullScreen();
    //Defining transparent window
    HbMainWindow mainWindow(0, Hb::WindowFlagTransparent);
    HbTransparentWindow *transparentWindow = new HbTransparentWindow;
    HbStackedLayout *stackedLayout = new HbStackedLayout;
    stackedLayout->addItem(transparentWindow);
    view->setLayout(stackedLayout);

    mainWindow.addView(view);
    mainWindow.setCurrentView(view);
      
    app.installEventFilter(view);

    //Installing the translator
    QTranslator *translator = new QTranslator();
    QString lang = QLocale::system().name();
    
    bool fine = translator->load("deviceupdates_" + lang, path);/*finally required once localisation available*/
    if (fine)
        qApp->installTranslator(translator);
    int err = KErrNone;
    TRAP( err, QT_TRYCATCH_LEAVING(CApplicationManagementServer::NewL(&mainWindow)));
    
    RProcess::Rendezvous(KErrNone);

    //Pushing the server to background
    mainWindow.lower();

    TInt serverLaunchStatus = 1;
    err = RProperty::Set(KProperty, KInteger, serverLaunchStatus);
    
    mainWindow.show();
    int err1 = app.exec();
    int value = 0;
    err = RProperty::Set(TUid::Uid(KOmaDMAppUid), KDownloadActive, value);
    RDEBUG_2("RProperty set (%d)", err  );
    return err1;
    }
