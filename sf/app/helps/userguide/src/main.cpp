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

#include <QTranslator>
#include <QtGlobal>
#include <QLocale>

#include <hbapplication.h>

#include "HelpCommon.h"
#include "HelpMainWindow.h"

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);

    QString lang = QLocale::system().name();
    
    // init translator with current system language
    QTranslator* translatorCommon = new QTranslator();
    translatorCommon->load("common_"+lang, PATH_TRANSLATIONS);
    qApp->installTranslator(translatorCommon);
    
    QTranslator* translatorSelf = new QTranslator();    
    translatorSelf->load("userguide_"+lang, PATH_TRANSLATIONS);
    qApp->installTranslator(translatorSelf);

	HelpMainWindow mainWindow;
	mainWindow.show();

    return app.exec();
}

// end of file
