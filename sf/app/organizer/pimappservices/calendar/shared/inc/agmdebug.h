// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __AGMDEBUG_H__
#define __AGMDEBUG_H__

#include <e32debug.h>

#if defined(_DEBUG)
// Basic logs - Minimum amount of logging which provides useful information to the user
#if defined (__CAL_BASIC_LOGGING__) || (__CAL_VERBOSE_LOGGING__) 
#define _DBGLOG_BASIC(s) s
#else
#define _DBGLOG_BASIC(s)
#endif

#if defined (__CAL_ENTRY_LOGGING__) || (__CAL_VERBOSE_LOGGING__) 
#define _DBGLOG_ENTRY(s) s
#else
#define _DBGLOG_ENTRY(s)
#endif

#if defined (__CAL_IPC_LOGGING__) ||  (__CAL_VERBOSE_LOGGING__) 
#define _DBGLOG_IPC(s) s
#else
#define _DBGLOG_IPC(s)
#endif

#if defined (__CAL_ALARM_LOGGING__) ||  (__CAL_VERBOSE_LOGGING__) 
#define _DBGLOG_ALARM(s) s
#else
#define _DBGLOG_ALARM(s)
#endif

#if defined (__CAL_ASYNC_LOGGING__) ||  (__CAL_VERBOSE_LOGGING__) 
#define _DBGLOG_ASYNC(s) s
#else
#define _DBGLOG_ASYNC(s)
#endif

#if defined (__CAL_INSTANCE_LOGGING__) ||  (__CAL_VERBOSE_LOGGING__) 
#define _DBGLOG_INSTANCE(s) s
#else
#define _DBGLOG_INSTANCE(s)
#endif

#if defined (__CAL_ATTACH_LOGGING__) ||  (__CAL_VERBOSE_LOGGING__) 
#define _DBGLOG_ATTACH(s) s
#else
#define _DBGLOG_ATTACH(s)
#endif

// Verbose logging - Additional logging which provides additional/verbose amounts. This includes basic logging.
#ifdef __CAL_VERBOSE_LOGGING__
#define _DBGLOG_VERBOSE(s) s
#else
#define _DBGLOG_VERBOSE(s)
#endif

#else // #ifdef _DEBUG

#define _DBGLOG_BASIC(s)
#define _DBGLOG_ENTRY(s)
#define _DBGLOG_IPC(s)
#define _DBGLOG_ALARM(s)
#define _DBGLOG_ASYNC(s)
#define _DBGLOG_INSTANCE(s)
#define _DBGLOG_ATTACH(s)
#define _DBGLOG_VERBOSE(s)

#endif // #ifdef _DEBUG

class CAgnEntry;
struct TAgnAlarmEntry;
class TAgnFilter;

const TInt KMinTTimeStrLength = 50;
const TInt KMaxGuidBufLength = 256;

/**
Enum used to toggle the level of detail for logging entry attributes
@internalTechnology
@released
*/
enum TAgmEntryDumpLevel
	{
	EDumpEntryIDs = 0x01,
	EDumpEntryTimes = 0x02,
	EDumpEntryRptInfo = 0x04,
	EDumpEntryStrData = 0x08,
	EDumpEntryAll = EDumpEntryIDs | EDumpEntryTimes | EDumpEntryRptInfo | EDumpEntryStrData
	};

/** Provides static functions for logging whilst debugging the calendar engine
@internalTechnology
@released
*/
class AgmDebug
	{
public:
	IMPORT_C static void DebugLog(const char* aFmt,...);
	IMPORT_C static void DebugLogTimeStampL(const char* aFmt,...);
	IMPORT_C static void DebugLogEntryL(CAgnEntry& aEntry, TAgmEntryDumpLevel aEntryDumpLevel);
	IMPORT_C static void DebugLogIPCL(TInt aOpCode, TUint aSessionId, TInt aErrCode);
	IMPORT_C static void DebugLogAlarmL(const TAgnAlarmEntry& aAlarmEntry);
	IMPORT_C static void TTimeStrL(const TTime& aTTime, TDes& aTimeStr);
	};

class TTruncateOverflow16 : public TDes16Overflow
	{
	public:
	   virtual void Overflow(TDes16&) { }
	};


#endif
