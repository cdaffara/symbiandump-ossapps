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

#include "agmdebug.h"
#include "agmentry.h"
#include "agmserv.h"
#include "agmrptdef.h"
#include "agsalarm.h"
#include "agmfilter.h"
#include <utf.h>

#if defined(_DEBUG)
const TInt KMaxLogLength = 0x100;

EXPORT_C void AgmDebug::DebugLog(const char* aFmt,...)
	{
	VA_LIST list = {NULL};
	VA_START(list,aFmt);

	TPtrC8 fmt8((const TText8*)aFmt);
		
	TBuf<KMaxLogLength> fmt16;
	fmt16.Copy(fmt8);

	TBuf<KMaxLogLength> logBuf16;
	TTruncateOverflow16 overflow;
	logBuf16.AppendFormatList(fmt16,list,&overflow);
	
	TBuf8<0x100> logBuf8;
	
	TInt err = CnvUtfConverter::ConvertFromUnicodeToUtf8(logBuf8,logBuf16);
	if(err==KErrNone)
		{
		RDebug::Printf("CAL_LOG: %S", &logBuf8);
		}
	}

EXPORT_C void AgmDebug::DebugLogTimeStampL(const char* aFmt,...)
	{
	VA_LIST list = {NULL};
	VA_START(list,aFmt);

	TPtrC8 fmt8((const TText8*)aFmt);
		
	TBuf<KMaxLogLength> fmt16;
	fmt16.Copy(fmt8);

	TBuf<KMaxLogLength> logBuf16;
	TTruncateOverflow16 overflow;
	logBuf16.AppendFormatList(fmt16,list,&overflow);
	
	TBuf8<0x100> logBuf8;
	
	TInt err = CnvUtfConverter::ConvertFromUnicodeToUtf8(logBuf8,logBuf16);
	
	TTime timeNow;
	timeNow.HomeTime();
	TBuf<KMinTTimeStrLength> timeNowBuf16;
	TTimeStrL(timeNow, timeNowBuf16);
	
	TBuf8<0x100> timeBuf8;
	
	TInt timeErr = CnvUtfConverter::ConvertFromUnicodeToUtf8(timeBuf8,timeNowBuf16);
	
	if(err==KErrNone)
		{
		RDebug::Printf("CAL_LOG: %S: %S\n", &timeBuf8, &logBuf8);
		}
	}

EXPORT_C  void AgmDebug::TTimeStrL(const TTime& aTTime, TDes& aTimeStr)
	{
	if(aTTime==Time::NullTTime())
		{
		_LIT(KNullTimeDes,"NULL");
		aTimeStr.Copy(KNullTimeDes);
		}
	else
		{
		_LIT(KTTimeDateFormat,"Time: %H:%T:%S:%*C3 Date:%*1/%2/%3");
		aTTime.FormatL(aTimeStr,KTTimeDateFormat);
		}
	}

#if defined (__CAL_VERBOSE_LOGGING__) || (__CAL_ENTRY_LOGGING__)
EXPORT_C void AgmDebug::DebugLogEntryL(CAgnEntry& aEntry, TAgmEntryDumpLevel aEntryDumpLevel)
	{
	const TInt KMaxEntryTimeModeStrLength = 12;
	const TInt KMaxEntryTypeStrLength = 12;

	TBuf<KMaxLogLength> logBuf;
	TBuf<KMaxEntryTimeModeStrLength> entryModeBuf;
	TBuf<KMaxEntryTypeStrLength> entryTypeBuf;

	TBool entryTimeModeUTC;
	
	logBuf.Append(_L("Dumping Entry: "));
	if(aEntryDumpLevel & EDumpEntryIDs) logBuf.Append(_L("Ids | "));
	if(aEntryDumpLevel & EDumpEntryTimes) logBuf.Append(_L("Times | "));
	if(aEntryDumpLevel & EDumpEntryRptInfo) logBuf.Append(_L("Repeat Info | "));
	if(aEntryDumpLevel & EDumpEntryStrData) logBuf.Append(_L("String Info"));
	
	DebugLog("%S", &logBuf);
	
	// Entry Type
	switch (aEntry.Type())
		{
		case CCalEntry::EAppt: 		entryTypeBuf.Copy(_L("Appointment")); break;
		case CCalEntry::EReminder: 	entryTypeBuf.Copy(_L("Reminder")); break;
		case CCalEntry::ETodo: 		entryTypeBuf.Copy(_L("Todo")); break;
		case CCalEntry::EEvent: 	entryTypeBuf.Copy(_L("Event")); break;
		case CCalEntry::EAnniv: 	entryTypeBuf.Copy(_L("Anniversary")); break;
		case CCalEntry::ENote:      entryTypeBuf.Copy(_L("Note")); break;   
		}
		
	// Entry Time Mode
	if (aEntry.TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		entryTimeModeUTC = ETrue;
		_LIT(KFloatingDes, "Floating");
		entryModeBuf.Copy(KFloatingDes());
		}
	else
		{
		entryTimeModeUTC = EFalse;
		_LIT(KUTCDes, "UTC");
		entryModeBuf.Copy(KUTCDes());
		}
	
	// Entry IDs
	if (aEntryDumpLevel & EDumpEntryIDs)
		{
		// == GUID and Local ID
		logBuf.Copy(aEntry.Guid());
		DebugLog("%S: Local UID=%d, GUID=%S, Mode =%S", &entryTypeBuf, aEntry.LocalUid(), &logBuf,&entryModeBuf);

		// == Recurrence ID
		TBuf<KMinTTimeStrLength> recIdTimeBuf;
		if(entryTimeModeUTC)
			{
			TTimeStrL(aEntry.RecurrenceId().UtcL(), recIdTimeBuf);
			}
		else
			{
			TTimeStrL(aEntry.RecurrenceId().LocalL(), recIdTimeBuf);
			}
		DebugLog("%S: Recurrence ID is %S", &entryTypeBuf, &recIdTimeBuf);
		
		// == Recurrence Range
		switch (aEntry.RecurrenceRange())
			{
			case CalCommon::EThisAndFuture:
				logBuf.Copy(_L("This And Future"));
				break;
			case CalCommon::EThisAndPrior:
				logBuf.Copy(_L("This And Prior"));
				break;
			default:
				logBuf.Copy(_L("Not Valid"));
				break;
			}
		DebugLog("%S: Recurrence Range - %S",&entryTypeBuf, &logBuf);
		}

	// Entry Times
	if (aEntryDumpLevel & EDumpEntryTimes)
		{
		TBuf<KMinTTimeStrLength> startTimeBuf;
		TBuf<KMinTTimeStrLength> endTimeBuf;
		
		if(entryTimeModeUTC)
			{
			TTimeStrL(aEntry.StartTime().UtcL(), startTimeBuf);
			TTimeStrL(aEntry.EndTime().UtcL(), endTimeBuf);
			}
		else
			{
			TTimeStrL(aEntry.StartTime().LocalL(), startTimeBuf);
			TTimeStrL(aEntry.EndTime().LocalL(), endTimeBuf);
			}
		DebugLog("%S: Start Time - %S %S", &entryTypeBuf, &startTimeBuf, &entryModeBuf);
		DebugLog("%S: End Time - %S %S", &entryTypeBuf, &endTimeBuf, &entryModeBuf);
		}
	
	// Entry Repeat Info
	if (aEntryDumpLevel & EDumpEntryRptInfo)
		{
		if (aEntry.RptDef())
			{
			switch (aEntry.RptDef()->HasRepeatRule())
				{
				case TAgnRpt::EDaily:			logBuf.Copy(_L("Daily")); break;
				case TAgnRpt::EWeekly:			logBuf.Copy(_L("Weekly")); break;
				case TAgnRpt::EMonthlyByDates:
				case TAgnRpt::EMonthlyByDays:	logBuf.Copy(_L("Monthly")); break;
				case TAgnRpt::EYearlyByDate:
				case TAgnRpt::EYearlyByDay:		logBuf.Copy(_L("Yearly")); break;
				default:						logBuf.Copy(_L("Repeat Rule - NOT SET"));
				}
			
			if(aEntry.RptDef()->HasRepeatRule())
				{
				DebugLog("%S: Repeats - %S, Interval - %d", &entryTypeBuf, &logBuf,aEntry.RptDef()->RRule()->Interval());
						
				TBuf<KMinTTimeStrLength> untilTimeBuf;
				if(entryTimeModeUTC)
					{
					TTimeStrL(aEntry.RptDef()->RRule()->UntilTimeL().UtcL(), untilTimeBuf);
					}
				else
					{
					TTimeStrL(aEntry.RptDef()->RRule()->UntilTimeL().LocalL(), untilTimeBuf);
					}		
					
				DebugLog("%S: Repeat Until Time - %S %S",&entryTypeBuf, &untilTimeBuf, &entryModeBuf);
				}
				
			if (aEntry.RptDef()->HasSporadicDates())
				{
				TTime sporadicTTime;
				TBuf<KMinTTimeStrLength> sopradicTimeBuf;
				const TInt KRDateCount =aEntry.RptDef()->SporadicDateList()->Count();
				
				DebugLog("%S: Number of sporadic dates: %d",&entryTypeBuf,KRDateCount);
				
				for (TInt i = 0; i < KRDateCount; ++i)
					{
					if(entryTimeModeUTC)
						{
						sporadicTTime = (*aEntry.RptDef()->SporadicDateList())[i].UtcL();
						}
					else
						{
						sporadicTTime = (*aEntry.RptDef()->SporadicDateList())[i].LocalL();
						}
					TTimeStrL(sporadicTTime, sopradicTimeBuf);
					DebugLog("%S: RDate %d %S",&entryTypeBuf, i+1,&sopradicTimeBuf);
					}
				}
			else
				{
				DebugLog("%S: No sporadic dates", &entryTypeBuf);
				}
			}
		else
			{
			DebugLog("%S: No repeat definition",&entryTypeBuf);
			}
		}

	// Entry Info
	if (aEntryDumpLevel & EDumpEntryStrData)
		{
		if (aEntry.Summary()!=KNullDesC && aEntry.Summary().Length())
			{
			DebugLog("%S: Summary='%S'",&entryTypeBuf, &aEntry.Summary());
			}
		if (aEntry.Description()!=KNullDesC && aEntry.Description().Length())
			{
			DebugLog("%S: Description='%S'", &entryTypeBuf, &aEntry.Description());
			}
		if (aEntry.Location()!=KNullDesC && aEntry.Location().Length())
			{
			DebugLog("%S: Location='%S'\n", &entryTypeBuf, &aEntry.Location());
			}
		}
	}
#else
	EXPORT_C void AgmDebug::DebugLogEntryL(CAgnEntry& /*aEntry*/, TAgmEntryDumpLevel /*aEntryDumpLevel*/) {}
#endif

#if defined (__CAL_VERBOSE_LOGGING__) || (__CAL_IPC_LOGGING__)
#define NM_CASE(x) case x: enumStr = _S(#x); break;
EXPORT_C void AgmDebug::DebugLogIPCL(TInt aOpCode, TUint aSessionId, TInt aErrCode)
	{
	const TText* enumStr;

	switch (aOpCode)
		{//case EOpenAgenda: enumStr = _S("EOpenAgenda");
		NM_CASE(EOpenAgenda);			
		NM_CASE(ECloseAgenda);						
		NM_CASE(ETransmitBuffer);					
		NM_CASE(EGetInstanceExtractor);	
		NM_CASE(EPreviousInstances);			
		NM_CASE(ENextInstances);				
		NM_CASE(ECreateEntryIterator);				
		NM_CASE(EEntryIteratorNext);					
		NM_CASE(EEntryIteratorPosition);		
		NM_CASE(EGetEntryUidsSinceDate);				
		NM_CASE(EGetFileId);							
		NM_CASE(EGetCategoryListCount);				
		NM_CASE(ECategoryFilter);					
		NM_CASE(EStartBuildIndex);					
		NM_CASE(EGetListFileNames);	
		NM_CASE(ECancelTask);				
		NM_CASE(EAgnResourceCount);	//	test 		
		NM_CASE(EAgnSetHeapFailure);	//	test 
		NM_CASE(EAgendaFileExists);
	 	NM_CASE(EDisableChangeBroadcast);
		NM_CASE(EEnableChangeBroadcast);
		NM_CASE(ERequestChangeNotificationParameters);
		NM_CASE(ERequestChangeNotification);
		NM_CASE(ECancelChangeNotification);
		NM_CASE(ERequestProgress);
		NM_CASE(ESetUpdateAlarm);
		NM_CASE(ESetEnablePubSubNotification);
		NM_CASE(ERestoreAlarmAction);
		NM_CASE(ETzDbChangedTime);
		// ReadUserData required 
		NM_CASE(EFetchEntry); 						
		NM_CASE(EFetchSimpleEntry);	
		NM_CASE(EFetchSimpleEntries);					
		NM_CASE(EFetchEntryByUID);					
		NM_CASE(ERestoreText);					
		NM_CASE(EGetCategoryListItem);
		NM_CASE(EGetChangesSinceLastNotification);
		NM_CASE(EFindInstances);
		NM_CASE(EFetchEntryByGuid);
		NM_CASE(ETransferAttachmentFileToClient);
		NM_CASE(EMoveFileToServer);
		NM_CASE(EFetchSortedAttachments);
		NM_CASE(EEntriesWithAttachment);
		NM_CASE(EFetchAttachmentById);
			
		// WriteUserData required 
		NM_CASE(EUpdateEntry);						
		NM_CASE(EAddEntry);							
		NM_CASE(EDeleteEntry);				
		NM_CASE(EAddCategoryToList);					
		NM_CASE(EDeleteAgendaFile);
		NM_CASE(ETidyByDateReadParams);
		NM_CASE(ETidyByDateStart);
		NM_CASE(ECategoryStart);
		NM_CASE(ECategoryStartAsyn);
		NM_CASE(ECreateAgendaFile);	
		NM_CASE(EDeleteEntriesByLocalUid);
		NM_CASE(EDeleteEntryByGuid);
		NM_CASE(ECommit);
		NM_CASE(ERollback);
		NM_CASE(ETransferAttachmentFileToServer);
		NM_CASE(ETransferFileToClientToWrite);
		NM_CASE(EAgnNotSupported);
		default: enumStr = _S("UnsupportedIPC, OpCode is: %d"), aOpCode;
		}

	DebugLog("IPC: %s, Session Id: %d, ErrCode: %d", enumStr, aSessionId, aErrCode);
	}
#else
	EXPORT_C void AgmDebug::DebugLogIPCL(TInt /*aOpCode*/, TUint /*aSessionId*/, TInt /*aErrCode*/) {}
#endif

#if defined (__CAL_VERBOSE_LOGGING__) || (__CAL_ALARM_LOGGING__)
EXPORT_C void AgmDebug::DebugLogAlarmL(const TAgnAlarmEntry& aAlarmEntry)
	{
	DebugLog("Logging Alarm details");
	
	TBuf<KMinTTimeStrLength> dueDateTimeBuf;
	AgmDebug::TTimeStrL(aAlarmEntry.iDueDateTimeLocal,dueDateTimeBuf);
	
	// Print instance of entry time in local time	
	DebugLog("Entry Instance Due Date Time: %S - LOCAL", &dueDateTimeBuf );
	
	TBuf<KMinTTimeStrLength> alarmTimeBuf;
	AgmDebug::TTimeStrL(aAlarmEntry.iAlarmTime.LocalL(),alarmTimeBuf);
	
	// Print alarm time in local time (as the instance time is in local time)
	DebugLog("Alarm Time: %S - LOCAL", &alarmTimeBuf);

	DebugLog("Alarm Message: %S", &aAlarmEntry.iMessage);
	DebugLog("Alarm SoundName: %S", &aAlarmEntry.iSound);
	
	TBuf<KMinTTimeStrLength> instanceTimeBuf;
	AgmDebug::TTimeStrL(aAlarmEntry.iInstanceId.Date().LocalL(),instanceTimeBuf);
	
	DebugLog("Alarm Instance EntryId: %d Instance Time: %S", aAlarmEntry.iInstanceId.Value(),&instanceTimeBuf);
	}
#else
	EXPORT_C void AgmDebug::DebugLogAlarmL(const TAgnAlarmEntry& /*aAlarmEntry*/) {}
#endif
#else
	// Avoid linking errors for exported APIs
	EXPORT_C void AgmDebug::DebugLog(const char* /*aFmt*/,...){	}
	EXPORT_C void AgmDebug::DebugLogTimeStampL(const char* /*aFmt*/,...){}
	EXPORT_C void AgmDebug::DebugLogEntryL(CAgnEntry& /*aEntry*/, TAgmEntryDumpLevel /*aEntryDumpLevel*/) {}
	EXPORT_C void AgmDebug::DebugLogIPCL(TInt /*aOpCode*/, TUint /*aSessionId*/, TInt /*aErrCode*/) {}
	EXPORT_C void AgmDebug::DebugLogAlarmL(const TAgnAlarmEntry& /*aAlarmEntry*/) {}
	EXPORT_C  void AgmDebug::TTimeStrL(const TTime& /*aTTime*/, TDes& /*aTimeStr*/) {}
#endif

