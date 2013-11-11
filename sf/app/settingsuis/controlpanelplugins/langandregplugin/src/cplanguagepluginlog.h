/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  
 *   
 */

#ifndef CPLANGUAGEPLUGINLOG_H
#define CPLANGUAGEPLUGINLOG_H

#include <QLatin1String>
#include <logger.h>

#ifdef ENABLE_CPLANG_LOG
    #define CPLANG_LOGGER_NAME           QLatin1String("CpLanguagePlugin")
    #define CPLANG_LOGGER_CONFIG_PATH    QLatin1String(":/logconf/cplanguagepluginlog.conf")

    #define CPLANG_LOG(str)              Logger::instance(CPLANG_LOGGER_NAME)->log(str);
    #define CPLANG_LOG_FUNC_ENTRY(func)  LogFunctionEntryHelper ___cplang_log_func_entry_helper(CPLANG_LOGGER_NAME,func);
#else
    #define CPLANG_LOG(str)
    #define CPLANG_LOG_FUNC_ENTRY(func)
#endif

#endif

//End of File

