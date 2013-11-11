/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  main.cpp
*
*/
#include <hbapplication.h>
#include <hbstyleloader.h>
#include <hbtranslator.h>
#include <QLocale>
#include "cpservicemainwindow.h"
#include "cpsplogger.h"

int main(int argc, char **argv)
{
    HbApplication app(argc,argv ); 
 
#ifdef ENABLE_CPSP_LOG
    INIT_LOGGER(CPSP_LOGGER_NAME,CPSP_LOGGER_CONFIG_PATH)
#endif
    
    CPSP_LOG("Entering CpServiceProvider.exe...");
    
    HbTranslator translator("control_panel");
    translator.loadCommon();
    
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem.css");
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem_color.css");
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem.widgetml");
    
    CpServiceMainWindow wnd;
    /*
    DON'T call wnd.show(),
    it will cause deadlock problem if cpserviceprovider is launched from an indicator plugin.
    */
    
    int ret = app.exec();
    
    CPSP_LOG("Exiting CpServiceProvider.exe.");
    
    return ret;     
}

//End of File
