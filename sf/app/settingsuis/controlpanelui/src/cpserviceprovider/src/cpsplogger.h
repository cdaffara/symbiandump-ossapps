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
* Description:  
*
*/

#ifndef CPSPLOGGER_H
#define CPSPLOGGER_H

#include <QLatin1String>
#include <logger.h>

#ifdef ENABLE_CPSP_LOG
    #define CPSP_LOGGER_NAME          QLatin1String("CpServiceProvider")
    #define CPSP_LOGGER_CONFIG_PATH   QLatin1String(":/logconf/cpserviceproviderlog.conf")

    #define CPSP_LOG(str)             Logger::instance(CPSP_LOGGER_NAME)->log(str);
    #define CPSP_LOG_FUNC_ENTRY(func) LogFunctionEntryHelper ___cpsp_log_func_entry_helper(CPSP_LOGGER_NAME,func);
#else
    #define CPSP_LOG(str)
    #define CPSP_LOG_FUNC_ENTRY(func)
#endif


#endif
