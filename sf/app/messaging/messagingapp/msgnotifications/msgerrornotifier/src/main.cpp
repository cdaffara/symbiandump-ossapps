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
#include <QLocale>
#include <QFile>
#include <hbapplication.h>

#include "msgerrornotifier.h"

#define LOC_TITLE hbTrId("")

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    QString locale = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    QTranslator translator;
    QTranslator translator_comm;
    translator.load(path + QString("messaging_") + locale);
    translator_comm.load(path + QString("common_") + locale);
    //translator.load( "messaging_en_GB", ":/translations" );
    app.installTranslator(&translator);
    app.installTranslator(&translator_comm);

    app.setApplicationName(LOC_TITLE);
    
    MsgErrorNotifier w;
    return app.exec();
}
