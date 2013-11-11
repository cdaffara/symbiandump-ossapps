/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*
*/

#ifndef __TFLOGGER_H__
#define __TFLOGGER_H__

#include <flogger.h>
#include <e32svr.h>

_LIT(KTfLogFolder, "ussd");
_LIT(KTfLogFile, "ussdeditor.txt");

/**
* ------------------------------------------------------------------------------
*
*  Remove #define LOGGING_ENABLED from comments to enable logging
*
* ------------------------------------------------------------------------------
*/

#ifdef _DEBUG 
#define LOGGING_ENABLED
#endif

/**
* ------------------------------------------------------------------------------
*
*  LOGGING MACROs
*
*  USE THESE MACROS IN YOUR CODE
*
*  Example: TFLOGTEXT(own_desc)
*  Example: TFLOGSTRING("Test")
*  Example: TFLOGSTRING("Test %i", aValue)
*  Example: TFLOGSTRING("Test %i %i", aValue1, aValue2)
*
* ------------------------------------------------------------------------------
*/

#ifdef LOGGING_ENABLED

    #define TFLOGTEXT(TEXT) \
        {  \
        RFileLogger::Write(KTfLogFolder,KTfLogFile, \
            EFileLoggingModeAppend, TEXT); \
        RDebug::Print(TEXT); \
        }

    #define TFLOGSTRING(TEXT) \
        { \
        _LIT(tempLogDes, TEXT); \
        RFileLogger::Write(\
            KTfLogFolder, \
            KTfLogFile, \
            EFileLoggingModeAppend, \
            tempLogDes()); \
        RDebug::Print(_L(TEXT)); \
        }

    #define TFLOGSTRING2(TEXT, VAR1) \
        { \
        _LIT(tempLogDes, TEXT); \
        RFileLogger::WriteFormat(\
            KTfLogFolder, \
            KTfLogFile, \
            EFileLoggingModeAppend, \
            TRefByValue<const TDesC>(tempLogDes()), VAR1); \
        RDebug::Print(_L(TEXT), VAR1); \
        }

    #define TFLOGSTRING3(TEXT, VAR1, VAR2) \
        { \
        _LIT(tempLogDes, TEXT); \
        RFileLogger::WriteFormat(\
            KTfLogFolder, \
            KTfLogFile, \
            EFileLoggingModeAppend, \
            TRefByValue<const TDesC>(tempLogDes()), VAR1, VAR2); \
        RDebug::Print(_L(TEXT), VAR1, VAR2); \
        }

#else

    #define TFLOGTEXT(TEXT)
    #define TFLOGSTRING(TEXT)
    #define TFLOGSTRING2(TEXT, VAR1)
    #define TFLOGSTRING3(TEXT, VAR1, VAR2)

#endif // LOGGING_ENABLED

#endif // __TFLOGGER_H__
