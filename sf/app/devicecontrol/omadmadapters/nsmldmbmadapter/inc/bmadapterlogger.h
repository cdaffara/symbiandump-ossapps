/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Logging functions for the component.
*
*/



#ifndef BMADAPTERLOGGER_H
#define BMADAPTERLOGGER_H

#ifndef _DEBUG

// UREL BUILD:
#define BMADAPTER_LOGGING_METHOD  0   // No logging in UREL builds

#else

// UDEB BUILD:
#define BMADAPTER_LOGGING_METHOD  1     // 0 = No logging,
                                        // 1 = Flogger,
                                        // 2 = RDebug
#endif // _DEBUG


#if BMADAPTER_LOGGING_METHOD == 1 // Flogger

#include <flogger.h>
_LIT(KBMADAPTERLOGFolder,"NSMLDMBMADAPTER");
_LIT(KBMADAPTERLOGFile,"NSMLDMBMADAPTER.txt");

#define BMADAPTERLOGTEXT(x)\
    {\
    RFileLogger::Write(KBMADAPTERLOGFolder(),KBMADAPTERLOGFile(),EFileLoggingModeAppend,\
    x);\
    }

#define BMADAPTERLOGSTRING(x)\
    {\
    _LIT8(tempLogDes,x);\
    RFileLogger::Write(KBMADAPTERLOGFolder(),KBMADAPTERLOGFile(),EFileLoggingModeAppend,\
    tempLogDes());\
    }

#define BMADAPTERLOGSTRING2(x,y)\
    { _LIT8(tempLogDes,x);\
    RFileLogger::WriteFormat(KBMADAPTERLOGFolder(),\
                             KBMADAPTERLOGFile(),\
                             EFileLoggingModeAppend,\
                             TRefByValue<const TDesC8>(tempLogDes()),y);\
    }

#define BMADAPTERLOGSTRING3(x,y,z)\
    { _LIT8(tempLogDes,x);\
    RFileLogger::WriteFormat(KBMADAPTERLOGFolder(),\
                             KBMADAPTERLOGFile(),\
                             EFileLoggingModeAppend,\
    TRefByValue<const TDesC8>(tempLogDes()),y,z);\
    }

#define BMADAPTERLOGSTRING4(w,x,y,z)\
    { _LIT8(tempLogDes,w);\
    RFileLogger::WriteFormat(KBMADAPTERLOGFolder(),\
                             KBMADAPTERLOGFile(),\
                             EFileLoggingModeAppend,\
    TRefByValue<const TDesC8>(tempLogDes()),x,y,z);\
    }


#elif BMADAPTER_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#define BMADAPTERLOGSTRING(x) RDebug::Print(x);
#define BMADAPTERLOGSTRING(x) RDebug::Print(_L(x));
#define BMADAPTERLOGSTRING2(x,y) RDebug::Print(_L(x),y);
#define BMADAPTERLOGSTRING3(x,y,z) RDebug::Print(_L(x),y,z);
#define BMADAPTERLOGSTRING4(w,x,y,z) RDebug::Print(_L(w),x,y,z);

#else // BMADAPTER_LOGGING_METHOD == 0 or invalid

#define BMADAPTERLOGSTRING(x)
#define BMADAPTERLOGSTRING(x)
#define BMADAPTERLOGSTRING2(x,y)
#define BMADAPTERLOGSTRING3(x,y,z)
#define BMADAPTERLOGSTRING4(w,x,y,z)

#endif // BMADAPTER_LOGGING_METHOD

#endif  // BMADAPTERLOGGER_H

// End of File
