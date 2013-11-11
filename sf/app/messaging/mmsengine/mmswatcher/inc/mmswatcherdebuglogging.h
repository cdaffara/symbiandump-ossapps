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
* Description:  mmswatcherdebuglogging  declaration
*
*/



#ifndef __MMSWATCHERDEBUGLOGGING_H
#define __MMSWATCHERDEBUGLOGGING_H

//  INCLUDES
#include <flogger.h>
#include "mmsservercommon.h" // this has _NO_MMSS_LOGGING_ handlinga and debug overrides

// MACROS
#ifndef _NO_MMSS_LOGGING_
  _LIT( KMmsWatcherLogFile,"mmswatcher.txt");
  _LIT( KMmsWatcherLogFolder,"mmswatcher");
  #define LOG(AAA)          RFileLogger::Write(KMmsWatcherLogFolder(),KMmsWatcherLogFile(),EFileLoggingModeAppend,AAA)
  #define LOG2(AAA,BBB)     RFileLogger::WriteFormat(KMmsWatcherLogFolder(),KMmsWatcherLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB)
  #define LOG3(AAA,BBB,ZZZ) RFileLogger::WriteFormat(KMmsWatcherLogFolder(),KMmsWatcherLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB,ZZZ)
  #define LOG7(AAA,BBB,ZZZ,DDD,EEE,FFF,GGG) RFileLogger::WriteFormat(KMmsWatcherLogFolder(),KMmsWatcherLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB,ZZZ,DDD,EEE,FFF,GGG)
#else // i.e. _NO_MMSS_LOGGING_ defined
  #define LOG(AAA)
  #define LOG2(AAA,BBB)
  #define LOG3(AAA,BBB,ZZZ)
  #define LOG7(AAA,BBB,ZZZ,DDD,EEE,FFF,GGG)
#endif // _NO_MMSS_LOGGING_

#endif // __MMSWATCHERDEBUGLOGGING_H   
            
// End of File
