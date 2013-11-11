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
 * Description:  Search application main implementation.
 *
 */

#include "searchapplication.h"
#include "search_global.h"

#include <QtGui>
#include <hbapplication.h>
#include <qtranslator.h>
#include <qlocale.h>

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
    {
    SEARCH_FUNC_ENTRY("SEARCH::Search::main"); PERF_APP_LAUNCH_END("SearchApplication Main Called");

    SearchApplication search(argc, argv);
    QTranslator translator;
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    translator.load("search_" + lang, path);
    search.installTranslator(&translator);

    QTranslator commonTranslator;
    commonTranslator.load("common_" + lang, path);
    search.installTranslator(&commonTranslator);


    PERF_APP_LAUNCH_END("Search Component Cunstructions starts");

    PERF_APP_LAUNCH_END("Search Component Cunstructions Ends");
#ifdef ROM
    QDir::setCurrent("Z:/");
#else
    QDir::setCurrent("C:/");
    SEARCHDEBUG("main() - SearchApplication's current dir set to C:/");
#endif //ROM
    search.start();
    PERF_APP_LAUNCH_END("SearchApplication Ends, Main Event Loop");
    int ret = search.exec();
    SEARCH_FUNC_EXIT("SEARCH::Search::main");
    return ret;
    }
