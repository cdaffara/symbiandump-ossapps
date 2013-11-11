/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Macro definition file for logging.
*
*/


#ifndef __PHONELOGGER_H
#define __PHONELOGGER_H

// By default, logging will always be switched on.  Later, this may be 
// removed, leaving it to individual source files to enable logging where 
// needed.
#define _LOGGING

/*
 Here we have 3 possible logging methods, or 2 and no logging at all.
 Later you'll find logic that selects one of these. If you're not happy
 with that logic in your case, throw it away and just #define your
 favourite logging method. 
 set PHONEUI_LOGGER_METHOD to NO_LOGGING to get no logging at all
 set PHONEUI_LOGGER_METHOD to RDEBUG_LOGGING to get logging via RDebug::Print
 set PHONEUI_LOGGER_METHOD to FILE_LOGGING to get logging to log file
 */
    
 #define  NO_LOGGING        0   // No logging at all.
 #define  RDEBUG_LOGGING    1   // Logging via RDebug::Print
 #define  FILE_LOGGING      2   // Logging to log file

/*
 * here is default logic: if we have DEBUG build then select
 * logging vua RDebug:Print. If anything else, have no logging at all
 */
#if ( defined (_DEBUG) && defined (_LOGGING) )
#define PHONEUI_LOGGING_OUTPUT RDEBUG_LOGGING // <- Here is the place where you can set 
                                 // debugging method.
#else
#define PHONEUI_LOGGING_OUTPUT NO_LOGGING
#endif// INCLUDES

// System includes.
#include <e32std.h>
#include "cphonelogger.h"

#endif // __PHONELOGGER_H
