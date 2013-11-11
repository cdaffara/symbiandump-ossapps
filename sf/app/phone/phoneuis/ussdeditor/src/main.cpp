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
*
*/

// INCLUDE FILES
#include <hbapplication.h>
#include <hbmainwindow.h> 
#include <hbview.h>
#include <QTranslator>

#include "ussdeditorquery.h"
#include "ussdcomms.h"
#include "tflogger.h"

// ---------------------------------------------------------
// main()
// Entry point function
// Returns: 0: success
// ---------------------------------------------------------
//
int main(int argc, char *argv[])
{
    TFLOGSTRING("USSDEDITOR: main IN")

    // Create application instance
    HbApplication application(argc, argv);
    TFLOGSTRING("USSDEDITOR: main app")

    QTranslator translator;
    QString path = "Z:/resource/qt/translations/";
    QString lang = QLocale::system().name();
    bool ok = translator.load("ussd_"+lang, path);
    TFLOGSTRING2("USSDEDITOR: main, translation %d (1=OK, 0=fail)", ok )
    application.installTranslator(&translator);

    // Create main widow
    HbMainWindow window;
    window.show();
    TFLOGSTRING("USSDEDITOR: main window")

    // Add a virtual view and hidden it
    HbView *view = new HbView();
    window.addView(view);
    TFLOGSTRING("USSDEDITOR: main addView")

    // Create UssdComms - ussd client
    UssdComms *ussdComms = new UssdComms(view);
    TFLOGSTRING("USSDEDITOR: main ussdComms")

    // Create HbInputDialog
    UssdEditorQuery *query = new UssdEditorQuery(*ussdComms);
    query->show();
    TFLOGSTRING("USSDEDITOR: main query")

    // Execute application loop
    int result = application.exec();
    TFLOGSTRING2("USSDEDITOR: main application OUT %d", result)
    delete query;
    query = 0;
    return result;
}

// End of file
