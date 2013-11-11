/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mmsdebuglogging  declaration
*
*/



#ifndef __MMSDEBUGLOGGING_H
#define __MMSDEBUGLOGGING_H

//  INCLUDES
#include <flogger.h>

// USERINCLUDE FILES
// CONSTANTS
// MACROS
#ifdef _DEBUG
_LIT( KMmsEngineLogFile,"settings.txt");
_LIT( KMmsEngineLogFolder,"mmssettings");
#define LOG(AAA)                 RFileLogger::Write(KMmsEngineLogFolder(),KMmsEngineLogFile(),EFileLoggingModeAppend,AAA)
#define LOG2(AAA,BBB)            RFileLogger::WriteFormat(KMmsEngineLogFolder(),KMmsEngineLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB)
#define LOG3(AAA,BBB,ZZZ)        RFileLogger::WriteFormat(KMmsEngineLogFolder(),KMmsEngineLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB,ZZZ)
#else // no _DEBUG defined
#define LOG(AAA)
#define LOG2(AAA,BBB)
#define LOG3(AAA,BBB,ZZZ)
#endif // _DEBUG

// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

#endif // __MMSDEBUGLOGGING_H   
            
// End of File
