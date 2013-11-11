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
* Description:  Provides macros for logging
*
*/



#ifndef __CSPLOGGER_H__
#define __CSPLOGGER_H__

//  INCLUDES
#include <e32def.h>

/* 
-----------------------------------------------------------------------------

    USER LOG SELECTION

-----------------------------------------------------------------------------
*/

inline TUint CSPLOGMASK()
    {
    return 0x8fffffff;          // This is default mask value.    
    }                           // User can change mask value to get different king of logging.

#ifndef _DEBUG

/***************************
* Logging method variants:
* 0 = No logging
* 1 = Flogger
* 2 = RDebug
***************************/ 

#define CSP_LOGGING_METHOD      0   // UREL BUILD

#else

#ifdef __WINS__
#define CSP_LOGGING_METHOD      2   // UDEB BUILD, WINS
#else
#define CSP_LOGGING_METHOD      2   // UDEB BUILD, HW
#endif // __WINS__

#endif // _DEBUG


// Next is for backward compatibility
#if CSP_LOGGING_METHOD > 0
#define CSP_LOGGING_ENABLED     
#define TF_LOGGING_ENABLED
#endif


/* 
-----------------------------------------------------------------------------

    LOGGING OPTIONS

-----------------------------------------------------------------------------
*/

#define CSPOBJECT  0x00000001   // Construction and destruction

#define CSPERROR   0x00000002   // Error that can be handled. Describe reason for error situation.
#define CSPPANIC   0x00000004   // Critical error: Panic.


#define CSPREQIN   0x00000040   // Public interface methods
#define CSPREQOUT  0x00000080   // Set outgoing request
#define CSPREQEND  0x00000100   // Complete request

#define CSPINT     0x00000200   // Internal activity

#define CSPMSTATE  0x00000400   // Component state changes

#define CSPGENERAL 0x00000800   // For deprecated TFLOGxxx-macros and for temporary usage


/* 
-----------------------------------------------------------------------------

    LOG SETTINGS

-----------------------------------------------------------------------------
*/

#if CSP_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>
_LIT(KTfLogFolder,"CSP");
_LIT(KTfLogFile,"CSPLOG.TXT");

#elif CSP_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#endif


/*
-----------------------------------------------------------------------------

    LOGGING MACROs

    USE THESE MACROS IN YOUR CODE !

-----------------------------------------------------------------------------
*/
#define MESSAGE(A)                        _L(A)

#if CSP_LOGGING_METHOD == 1      // Flogger

#define CSPLOGTEXT(Type,AAA)            { \
                                        if((CSPLOGMASK()&(Type))==(Type)) \
                                            { \
                                            RFileLogger::Write(KTfLogFolder(),KTfLogFile(), \
                                            EFileLoggingModeAppend, AAA); \
                                            } \
                                        }
#define CSPLOGSTRING(Type,AAA)          { \
                                        if((CSPLOGMASK()&(Type))==(Type)) \
                                            { \
                                            _LIT(tempLogDes,AAA); \
                                            RFileLogger::Write(KTfLogFolder(),KTfLogFile(), \
                                            EFileLoggingModeAppend,tempLogDes()); \
                                            } \
                                        }
#define CSPLOGSTRING2(Type,AAA,BBB)     { \
                                        if((CSPLOGMASK()&(Type))==(Type)) \
                                            { \
                                            _LIT(tempLogDes,AAA); \
                                            RFileLogger::WriteFormat(KTfLogFolder(),KTfLogFile(), \
                                            EFileLoggingModeAppend, \
                                            TRefByValue<const TDesC>(tempLogDes()),BBB); \
                                            } \
                                        }
#define CSPLOGSTRING3(Type,AAA,BBB,CCC) { \
                                        if((CSPLOGMASK()&(Type))==(Type)) \
                                            { \
                                            _LIT(tempLogDes,AAA); \
                                            RFileLogger::WriteFormat(KTfLogFolder(),KTfLogFile(), \
                                            EFileLoggingModeAppend, \
                                            TRefByValue<const TDesC>(tempLogDes()),BBB,CCC); \
                                            } \
                                        }
#define CSPLOGSTRING4(Type,AAA,BBB,CCC,DDD) { \
                                            if((CSPLOGMASK()&(Type))==(Type)) \
                                                { \
                                                _LIT(tempLogDes,AAA); \
                                                RFileLogger::WriteFormat(KTfLogFolder(), \
                                                KTfLogFile(),EFileLoggingModeAppend, \
                                                TRefByValue<const TDesC>(tempLogDes()), \
                                                BBB,CCC,DDD); \
                                                } \
                                            }

#elif CSP_LOGGING_METHOD == 2    // RDebug

#define CSPLOGTEXT(Type,AAA)                { \
                                            if((CSPLOGMASK()&(Type))==(Type)) \
                                                    { RDebug::Print(AAA); } \
                                            }
#define CSPLOGSTRING(Type,AAA)              { \
                                            if((CSPLOGMASK()&(Type))== \
                                            (Type)) { RDebug::Print(_L(AAA)); } \
                                            }
#define CSPLOGSTRING2(Type,AAA,BBB)         { \
                                            if((CSPLOGMASK()&(Type))== \
                                            (Type)) { RDebug::Print(_L(AAA),BBB); } \
                                            }
#define CSPLOGSTRING3(Type,AAA,BBB,CCC)     { \
                                            if((CSPLOGMASK()&(Type))== \
                                            (Type)) { RDebug::Print(_L(AAA),BBB,CCC); } \
                                            }
#define CSPLOGSTRING4(Type,AAA,BBB,CCC,DDD) { \
                                            if((CSPLOGMASK()&(Type))== \
                                            (Type)) { RDebug::Print(_L(AAA),BBB,CCC,DDD); } \
                                            }

#else   // CSP_LOGGING_METHOD == 0 or invalid

#define CSPLOGTEXT(Type,AAA)              // Example: TFLOGTEXT(CSPMESOUT, own_desc)
#define CSPLOGSTRING(Type,AAA)            // Example: TFLOGSTRING(CSPMESIN, "Test")
#define CSPLOGSTRING2(Type,AAA,BBB)       // Example: TFLOGSTRING(CSPMESINT, "Test %i", aValue)
#define CSPLOGSTRING3(Type,AAA,BBB,CCC)   // Example: TFLOGSTRING(CSPMESOUT, "Test %i %i", aValue1, aValue2)
#define CSPLOGSTRING4(Type,AAA,BBB,CCC,DDD)   // Example: TFLOGSTRING(CSPMESOUT, "Test %i %i %i", aValue1, aValue2, aValue3)

#endif  // CSP_LOGGING_METHOD


/*
-----------------------------------------------------------------------------

    DEPRECATED LOGGING MACROs

    DON'T USE THESE MACROS IN YOUR CODE ANYMORE!

-----------------------------------------------------------------------------
*/

#define TFLOGTEXT(AAA)              CSPLOGTEXT(CSPGENERAL,AAA);
#define TFLOGSTRING(AAA)            CSPLOGSTRING(CSPGENERAL,AAA);
#define TFLOGSTRING2(AAA,BBB)       CSPLOGSTRING2(CSPGENERAL,AAA,BBB);
#define TFLOGSTRING3(AAA,BBB,CCC)   CSPLOGSTRING3(CSPGENERAL,AAA,BBB,CCC);
#define TFLOGSTRING4(AAA,BBB,CCC,DDD)   CSPLOGSTRING4(CSPGENERAL,AAA,BBB,CCC,DDD);

#endif  // CSPLOGGER_H
