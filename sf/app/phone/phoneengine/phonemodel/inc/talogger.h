/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef __TALOGGER_H__
#define __TALOGGER_H__

//  INCLUDES
#include <e32def.h>

/* 
-----------------------------------------------------------------------------

    USER LOG SELECTION

-----------------------------------------------------------------------------
*/

inline TUint KTALOGMASK()
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

#define TEF_LOGGING_METHOD      0   // UREL BUILD

#else
#define TEF_LOGGING_METHOD      2
#endif // _DEBUG


// Next is for backward compatibility
#if TEF_LOGGING_METHOD > 0
#define TEF_LOGGING_ENABLED     
#define TF_LOGGING_ENABLED
#endif


/* 
-----------------------------------------------------------------------------

    LOGGING OPTIONS

-----------------------------------------------------------------------------
*/

#define KTAOBJECT  0x00000001   // Construction and destruction

#define KTAERROR   0x00000002   // Error that can be handled. Describe reason for error situation.
#define KTAPANIC   0x00000004   // Critical error: Panic.

#define KTAMESIN   0x00000008   // From client to TELE message
#define KTAMESOUT  0x00000010   // From TELE to client message
#define KTAMESINT  0x00000020   // TELE internal message

#define KTAREQIN   0x00000040   // Public interface methods
#define KTAREQOUT  0x00000080   // Set outgoing request
#define KTAREQEND  0x00000100   // Complete request

#define KTAINT     0x00000200   // Internal activity

#define KTAMSTATE  0x00000400   // Component state changes

#define KTAGENERAL 0x00000800   // For deprecated TFLOGxxx-macros and for temporary usage


/* 
-----------------------------------------------------------------------------

    LOG SETTINGS

-----------------------------------------------------------------------------
*/

#if TEF_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>
_LIT(KTfLogFolder,"PE");
_LIT(KTfLogFile,"TALOG.TXT");

#elif TEF_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#endif


/*
-----------------------------------------------------------------------------

    LOGGING MACROs

    USE THESE MACROS IN YOUR CODE !

-----------------------------------------------------------------------------
*/
#define MESSAGE(A)                        _L(A)

#if TEF_LOGGING_METHOD == 1      // Flogger

#define TEFLOGTEXT(Type,AAA)            { \
                                        if((KTALOGMASK()&(Type))==(Type)) \
                                            { \
                                            RFileLogger::Write(KTfLogFolder(),KTfLogFile(), \
                                            EFileLoggingModeAppend, AAA); \
                                            } \
                                        }
#define TEFLOGSTRING(Type,AAA)          { \
                                        if((KTALOGMASK()&(Type))==(Type)) \
                                            { \
                                            _LIT(tempLogDes,AAA); \
                                            RFileLogger::Write(KTfLogFolder(),KTfLogFile(), \
                                            EFileLoggingModeAppend,tempLogDes()); \
                                            } \
                                        }
#define TEFLOGSTRING2(Type,AAA,BBB)     { \
                                        if((KTALOGMASK()&(Type))==(Type)) \
                                            { \
                                            _LIT(tempLogDes,AAA); \
                                            RFileLogger::WriteFormat(KTfLogFolder(),KTfLogFile(), \
                                            EFileLoggingModeAppend, \
                                            TRefByValue<const TDesC>(tempLogDes()),BBB); \
                                            } \
                                        }
#define TEFLOGSTRING3(Type,AAA,BBB,CCC) { \
                                        if((KTALOGMASK()&(Type))==(Type)) \
                                            { \
                                            _LIT(tempLogDes,AAA); \
                                            RFileLogger::WriteFormat(KTfLogFolder(),KTfLogFile(), \
                                            EFileLoggingModeAppend, \
                                            TRefByValue<const TDesC>(tempLogDes()),BBB,CCC); \
                                            } \
                                        }
#define TEFLOGSTRING4(Type,AAA,BBB,CCC,DDD) { \
                                            if((KTALOGMASK()&(Type))==(Type)) \
                                                { \
                                                _LIT(tempLogDes,AAA); \
                                                RFileLogger::WriteFormat(KTfLogFolder(), \
                                                KTfLogFile(),EFileLoggingModeAppend, \
                                                TRefByValue<const TDesC>(tempLogDes()), \
                                                BBB,CCC,DDD); \
                                                } \
                                            }

#elif TEF_LOGGING_METHOD == 2    // RDebug

#define TEFLOGTEXT(Type,AAA)                { \
                                            if((KTALOGMASK()&(Type))==(Type)) RDebug::Print(AAA); \
                                            }
#define TEFLOGSTRING(Type,AAA)              { \
                                            if((KTALOGMASK()&(Type))== \
                                            (Type)) RDebug::Print(_L(AAA)); \
                                            }
#define TEFLOGSTRING2(Type,AAA,BBB)         { \
                                            if((KTALOGMASK()&(Type))== \
                                            (Type)) RDebug::Print(_L(AAA),BBB); \
                                            }
#define TEFLOGSTRING3(Type,AAA,BBB,CCC)     { \
                                            if((KTALOGMASK()&(Type))== \
                                            (Type)) RDebug::Print(_L(AAA),BBB,CCC); \
                                            }
#define TEFLOGSTRING4(Type,AAA,BBB,CCC,DDD) { \
                                            if((KTALOGMASK()&(Type))== \
                                            (Type)) RDebug::Print(_L(AAA),BBB,CCC,DDD); \
                                            }

#else   // TEF_LOGGING_METHOD == 0 or invalid

#define TEFLOGTEXT(Type,AAA)              // Example: TFLOGTEXT(KTAMESOUT, own_desc)
#define TEFLOGSTRING(Type,AAA)            // Example: TFLOGSTRING(KTAMESIN, "Test")
#define TEFLOGSTRING2(Type,AAA,BBB)       // Example: TFLOGSTRING(KTAMESINT, "Test %i", aValue)
#define TEFLOGSTRING3(Type,AAA,BBB,CCC)   // Example: TFLOGSTRING(KTAMESOUT, "Test %i %i", aValue1, aValue2)
#define TEFLOGSTRING4(Type,AAA,BBB,CCC,DDD)   // Example: TFLOGSTRING(KTAMESOUT, "Test %i %i %i", aValue1, aValue2, aValue3)

#endif  // TEF_LOGGING_METHOD


/*
-----------------------------------------------------------------------------

    DEPRECATED LOGGING MACROs

    DON'T USE THESE MACROS IN YOUR CODE ANYMORE!

-----------------------------------------------------------------------------
*/

#define TFLOGTEXT(AAA)              TEFLOGTEXT(KTAGENERAL,AAA);
#define TFLOGSTRING(AAA)            TEFLOGSTRING(KTAGENERAL,AAA);
#define TFLOGSTRING2(AAA,BBB)       TEFLOGSTRING2(KTAGENERAL,AAA,BBB);
#define TFLOGSTRING3(AAA,BBB,CCC)   TEFLOGSTRING3(KTAGENERAL,AAA,BBB,CCC);
#define TFLOGSTRING4(AAA,BBB,CCC,DDD)   TEFLOGSTRING4(KTAGENERAL,AAA,BBB,CCC,DDD);

#endif  // TALOGGER_H
