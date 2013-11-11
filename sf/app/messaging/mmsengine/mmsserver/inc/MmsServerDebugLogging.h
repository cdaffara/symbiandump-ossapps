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
* Description:  MmsServerDebugLogging  declaration
*
*/



#ifndef __MMSSERVERDEBUGLOGGING_H
#define __MMSSERVERDEBUGLOGGING_H

//  INCLUDES
#include <flogger.h>

// MACROS
#ifndef _NO_MMSS_LOGGING_
  _LIT( KMmsServerLogFile,"mmss.txt");
  _LIT( KMmsServerLogFolder,"mmss");
  #define LOG(AAA)          RFileLogger::Write(KMmsServerLogFolder(),KMmsServerLogFile(),EFileLoggingModeAppend,AAA)
  #define LOG2(AAA,BBB)     RFileLogger::WriteFormat(KMmsServerLogFolder(),KMmsServerLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB)
  #define LOG3(AAA,BBB,ZZZ) RFileLogger::WriteFormat(KMmsServerLogFolder(),KMmsServerLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB,ZZZ)
#else // i.e. _NO_MMSS_LOGGING_ defined
  #define LOG(AAA)
  #define LOG2(AAA,BBB)
  #define LOG3(AAA,BBB,ZZZ)
  #define LOG7(AAA,BBB,ZZZ,DDD,EEE,FFF,GGG)
#endif // _NO_MMSS_LOGGING_

#endif // __MMSSERVERDEBUGLOGGING_H   
            
// End of File
