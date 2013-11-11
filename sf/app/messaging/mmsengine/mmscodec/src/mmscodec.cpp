/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     DLL entry point
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <flogger.h>

#include "mmscodec.h"
#include "mmsconst.h"
#include "mmsservercommon.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= OTHER EXPORTED FUNCTIONS ==============

#ifndef _NO_MMSS_LOGGING_
const TInt KLogBufferLength = 256;
_LIT(KLogDir, "mmss");
_LIT(KLogFile, "mmss.txt");

void TMmsLogger::Log(TRefByValue<const TDesC> aFmt,...)
    {
    VA_LIST list;
    VA_START(list, aFmt);

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList(aFmt, list);

    // Write to log file
    RFileLogger::Write(KLogDir, KLogFile, EFileLoggingModeAppend, buf);
    }
#endif

//  End of File  
