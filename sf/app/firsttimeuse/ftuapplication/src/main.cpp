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
* Description:  First Time Use application main implementation.
*
*/


#include "ftufirsttimeuse.h"
#include "ftutest_global.h"

#include <QtGui>
#include <hbapplication.h>
#include <HbTranslator>
#include <QLocale>
#include <QTime>
#include <QDebug>

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    QDEBUG(__FILE__<<__FUNCTION__<<"~~~~~~~"<<QTime::currentTime().toString("hh:mm:ss.zzz");)
    FTUTEST_FUNC_ENTRY("FTU::FtuFirstTimeUse::main");
    
    HbApplication app(argc, argv);    

    
    HbTranslator *translator = new HbTranslator("/resource/qt/translations/","firsttimesetup");
    
    FtuFirstTimeUse ftuFirstTimeUse;
#ifdef ROM
    QDir::setCurrent("Z:/");    
#else
    QDir::setCurrent("C:/");
    QDEBUG("main() - FtuApplication's current dir set to C:/");
#endif //ROM
    QObject::connect(&app,SIGNAL(aboutToQuit()),&ftuFirstTimeUse,SLOT(stop()));
    QObject::connect(&ftuFirstTimeUse, SIGNAL(exit()), &app, SLOT(quit()),Qt::QueuedConnection);    
    ftuFirstTimeUse.start();
    int ret = app.exec();
    QDEBUG("FTU::FtuFirstTimeUse::main");
    delete translator;
    return ret;       
}
