/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provides logging services.
*
*/


#ifndef ALWAYSONLOGGER_H
#define ALWAYSONLOGGER_H

#ifdef _DEBUG
                              // 0 = No logging,
#define LOGGING_METHOD  1     // 1 = Flogger,
                              // 2 = RDebug
#else

#define LOGGING_METHOD  0     // No logging in UREL builds

#endif // _DEBUG


#if LOGGING_METHOD == 1       // Flogger

#include <flogger.h>
_LIT(KLOGFolder,"ALWAYSONADAPTER");
_LIT(KLOGFile,"ALWAYSONADAPTER.txt");

#define LOGTEXT(x)\
    {\
    RFileLogger::Write(KLOGFolder(),KLOGFile(),EFileLoggingModeAppend,\
    x);\
    }

#define LOGSTRING(x)\
    {\
    _LIT8(tempLogDes,x);\
    RFileLogger::Write(KLOGFolder(),KLOGFile(),EFileLoggingModeAppend,\
    tempLogDes());\
    }

#define LOGSTRING2(x,y)\
    { _LIT8(tempLogDes,x);\
    RFileLogger::WriteFormat(KLOGFolder(),\
                             KLOGFile(),\
                             EFileLoggingModeAppend,\
                             TRefByValue<const TDesC8>(tempLogDes()),y);\
    }

#define LOGSTRING3(x,y,z)\
    { _LIT8(tempLogDes,x);\
    RFileLogger::WriteFormat(KLOGFolder(),\
                             KLOGFile(),\
                             EFileLoggingModeAppend,\
    TRefByValue<const TDesC8>(tempLogDes()),y,z);\
    }

#define LOGSTRING4(w,x,y,z)\
    { _LIT8(tempLogDes,w);\
    RFileLogger::WriteFormat(KLOGFolder(),\
                             KLOGFile(),\
                             EFileLoggingModeAppend,\
    TRefByValue<const TDesC8>(tempLogDes()),x,y,z);\
    }


#elif _LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#define LOGSTRING(x) RDebug::Print(x);
#define LOGSTRING(x) RDebug::Print(_L(x));
#define LOGSTRING2(x,y) RDebug::Print(_L(x),y);
#define LOGSTRING3(x,y,z) RDebug::Print(_L(x),y,z);
#define LOGSTRING4(w,x,y,z) RDebug::Print(_L(w),x,y,z);

#else // LOGGING_METHOD == 0 or invalid

#define LOGSTRING(x)
#define LOGSTRING(x)
#define LOGSTRING2(x,y)
#define LOGSTRING3(x,y,z)
#define LOGSTRING4(w,x,y,z)

#endif // LOGGING_METHOD

#endif  // ALWAYSONLOGGER_H

// End of File
