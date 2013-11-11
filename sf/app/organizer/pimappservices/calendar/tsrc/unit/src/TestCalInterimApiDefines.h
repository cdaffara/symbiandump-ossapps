/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/



/** Generic*/
#include <e32test.h>
#include <s32file.h>
#include <e32std.h> 
/** File Access*/
#include <calsession.h>
#include <calcategorymanager.h>
#include <calinstanceview.h>
#include <consolealarmalertservermain.h>
/** Entry/Instance*/
#include <coreappstest/testserver.h>
#include <calentry.h>
#include <calentryview.h>
#include <calrrule.h>
#include <calprogresscallback.h>
#include <caluser.h>
#include <calalarm.h>

_LIT(KColon, ":");

/**  Constants used by the ConsoleAlarmAlertServer*/
#ifdef __WINS__ 
const TUid KServerUid2={0x1000008D};
const TUid KServerUid3={0x101F502A};
const TUidType serverUid(KNullUid,KServerUid2,KServerUid3);
#endif

const TInt KNumberOfEntries = 100;
//total number of entries that must be in CITVCal.dat
const TInt KNumberOfEntriesInCITVCal = 16;

_LIT8(KGUID, "A_Global_UID");
_LIT (KCITFS326, "Executing Integration Tests for Section 3.2.6 in FS");
_LIT (KCITFS321, "Executing Integration Tests for Section 3.2.1 in FS");
_LIT(KComment, "//");

/** Test Agenda File*/
_LIT(KFileName, "C:CITFileAccess.vcs");
_LIT(KAgendaFile,"C:CITVCalv92.dat");
_LIT(KAgendaRestoreFile,"C:CITVCalv91.dat");
_LIT(KCalendarFile,"C:CITCal.vcs");

/** CTestCalInterimApiFileAccess */
_LIT(KConsoleAlarmAlertServerImg,"ConsoleAlarmAlertServer");
_LIT (KCreateNewAgendaFile, "Creating New Agenda file %S");
_LIT (KTestIfAgendaFileExists, "Testing if %S exists");
_LIT (KTestIfAgendaFileDoesExists, "Found %S");
_LIT (KTestIfAgendaFileNotExists, "ERROR -- Not Found %S");
_LIT (KListingAllAgendaFiles, "Listing All Agenda Files");
_LIT (KDeletingAgendaFile, "Deleting %S");
_LIT (KFileAccessError, "ERROR -- Failed To Create %S");
_LIT (KErrorListingAllAgendaFiles, "ERROR -- No Agenda Files Found");
_LIT (KFailDeletingAgendaFile, "Failed to Delete %S");
_LIT (KDeletedAgendaFile, "%S Has Been Deleted");
#define MAX_TEXT_MESSAGE 256
#define LARGE_TEXT_MESSAGE_UID 350

/** CTestCalInterimApiEntryData */
_LIT(KAddAgendaEntries,"Adding Agenda Entries");
_LIT(KEntriesCompleted,"Complete Entries Added = %d");
_LIT(KCompleted,"Completed Adding All Entries, Error=%d");
_LIT(KPercentComplete,"Percentage Added =%d%%");
_LIT (KCompareError, "ERROR -- Comparision error:- %S %S");
_LIT (KCompareNumberError, "ERROR -- Comparision error:- %d %d");
_LIT(KOrganizerCommonName, "Mr Symbian");
_LIT (KOpenNewAgendaFile, "Opened New Agenda file %S");
_LIT (KEntryType, "Adding %S");
#define INTERVALYEAR 365
#define REPEATOVERYEARS 5

/** Entry Types */
_LIT(KToDo,"VTODO");
_LIT(KEvent,"VEVENT");
_LIT(KReminder,"VREMINDER");
_LIT(KAppt,"VAPPT");
_LIT(KAnniv,"VANNIV");
_LIT(KRule,"RRULE:");
_LIT(KYearly,"YEARLY");
_LIT(KMonthly,"MONTHLY");
_LIT(KWeekly,"WEEKLY");
_LIT(KDaily,"DAILY");
_LIT(KNone,"");
_LIT(KDateTime,"%d/%d/%d  %d:%d:%d.%d");
/** Invalid data */
_LIT(KInvalidRole,"InvalidRole");
_LIT(KInvalidDueDate,"InvalidDueDate@DEF058001");

/** CTestCalInterimApiIterator */
_LIT(KIterateEmptyFileError,"Interating through an Empty Calendar File:- %S");
_LIT(KIterateEmptyFileGetNext,"Getting Next Entry from %S");
_LIT(KIterateEmptyFileNoEntries,"No Entries Found");
_LIT(KIterateEmptyFileNoEntriesError,"ERROR -- Invalid Error Returned %d");
_LIT(KIterateEmptyFile,"Interating through an Empty Calendar File Complete");
_LIT(KIterateCalFileError,"ERROR -- Please Run Test Step TestCalInterimApiEntryData First");
_LIT(KIterateCalFile,"Interating through %S");
_LIT(KIterateCalFileUid,"Entry Found with UID:- %S");
_LIT(KIterateCalFileInvalidEntries,"ERROR -- Entries in %S are %d, Entries Iterated %d");
_LIT(KIterateCalFileEntriesFound,"%d Entries found in %S");

/** CTestCalInterimApiFilter */
//Number of entries in CITVCal.dat, Only Modify if 
#define APPTS 1
#define REMINDERS 2
#define EVENTS 21
#define ANIV 1
#define TODO 6
#define ALARM 4

#define MAX_FILTER_TYPES 64
#define BASE2 2

_LIT(KAlarm,"Alarmed Entries");
_LIT(KCompleteToDo,"Completed ToDo");
_LIT(KInCompleteToDo,"InCompleted ToDo");

_LIT(KFilterCalFile,"Opened %S For Filtering");
_LIT(KFoundTypes,"Found %d");
_LIT(KFoundTypesError,"ERROR -- Found %d of %d %S");
