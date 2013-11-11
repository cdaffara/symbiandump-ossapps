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
 * 
 * Description:
 *     The main function for Tone Fetcher
 */

#include "tonefetcher.h"
#include "tonefetchermainwindow.h"
#include <hbapplication.h>
#include <tonefetcherlogger.h>

int main(int argc, char *argv[])
{
    HbApplication a(argc, argv);
    
#ifdef ENABLE_TONEFETCHER_LOG
    Logger::instance(TONEFETCHER_LOGGER_NAME)->configure(
            TF_LOGGER_CONFIG_PATH,QSettings::IniFormat);
#endif
    TF_LOG("Entering tonefetcher.exe...");
    ToneFetcherMainWindow w;   
    w.show();
    int ret = a.exec();
    TF_LOG("Leaving tonefetcher.exe...");
#ifdef ENABLE_TONEFETCHER_LOG
    Logger::closeAll();
#endif
    return ret;
}
