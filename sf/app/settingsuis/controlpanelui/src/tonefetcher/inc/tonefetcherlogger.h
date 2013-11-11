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
 *     The header file for tone fetcher logger.
 *     
 */

#ifndef TONEFETCHERLOGGER_H
#define TONEFETCHERLOGGER_H

#include <QLatin1String>
#include <logger.h>

/*
 make LOG work
*/

//#define ENABLE_TONEFETCHER_LOG

#define TONEFETCHER_LOGGER_NAME       QLatin1String("ToneFetcher")

#if defined (Q_OS_SYMBIAN)
    #define TF_LOGGER_CONFIG_PATH QLatin1String("C:/data/.config/tonefetcherlog.conf")
#elif defined (Q_WS_WIN)
    #ifdef _DEBUG
        #define TF_LOGGER_CONFIG_PATH QLatin1String("C:/controlpanel/debug/bin/tonefetcherlog.conf")
    #else 
        #define TF_LOGGER_CONFIG_PATH QLatin1String("C:/controlpanel/release/bin/tonefetcherlog.conf")
    #endif
#endif

#ifdef ENABLE_TONEFETCHER_LOG
    #define TF_LOG(str)   Logger::instance(TONEFETCHER_LOGGER_NAME)->log(str);
#else
    #define TF_LOG(str)
#endif

#endif /* TONEFETCHERLOGGER_H */
