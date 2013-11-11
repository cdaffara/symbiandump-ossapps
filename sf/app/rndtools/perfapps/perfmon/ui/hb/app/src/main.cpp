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

#include <eikenv.h>
#include <QMessageBox>

#include "application.h"
#include "enginewrapper.h"
#include "mainwindow.h"
#include "datapopup.h"

int main(int argc, char *argv[])
{
    Application app(argc, argv);
	
    EngineWrapper engine;
    if (!engine.initialize())
    {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Unable to initialize engine. Application will now quit."));
        return 1;
    }

    // set as system application to prevent getting shut down events 
    CEikonEnv::Static()->SetSystem( ETrue );
    
    MainWindow window(engine);
	window.show();

    DataPopup dataPopup(engine, window);

    // TODO: pass false if run in background
    dataPopup.updateVisibility(true);

    return app.exec();
}
