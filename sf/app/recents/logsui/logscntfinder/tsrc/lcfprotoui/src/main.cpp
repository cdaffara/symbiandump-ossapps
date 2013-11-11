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
//USER
#include "lcfmainwindow.h"
#include "logslogger.h"
#include "lcfsearchresultsview.h"
//SYSTEM
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <QObject.h>


int main(int argc, char *argv[])
{
    LOGS_QDEBUG( "logs [LCFPROTO] -> main()" )

    HbApplication app(argc, argv);
    LcfMainWindow window;
    
    LOGS_QDEBUG( "logs [LCFPROTO] application created" )
    LcfSearchResultsView* view = 0;
    LcfViewLoader loader;
    LOGS_QDEBUG( "logs [LCFPROTO] about to load view" )
    bool ok = true;
    loader.load( ":/xml/lcfSearchResultsView.xml", &ok);
    LOGS_QDEBUG_2( "logs [LCFPROTO] view loaded=", ok )
    int err = 0;
    if ( ok ) {
        view = qobject_cast<LcfSearchResultsView*> ( loader.findWidget( lcfviewname  ) );
        //LOGS_QDEBUG_2( "logs [LCFPROTO] view =", view );
        
        LOGS_QDEBUG( "logs [LCFPROTO] about to init view" )
        
        view->init( loader );
        LOGS_QDEBUG( "logs [LCFPROTO] init view done" )
        
        window.addView(view);

        window.show();
        err = app.exec();
    }
    
    LOGS_QDEBUG( "logs [LCFPROTO] <- main()" )
    return err;
}
