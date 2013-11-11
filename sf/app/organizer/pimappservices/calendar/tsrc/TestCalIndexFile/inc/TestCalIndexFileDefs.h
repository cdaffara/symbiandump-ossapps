/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TEST_CAL_INDEX_FILE_DEFS__
#define __TEST_CAL_INDEX_FILE_DEFS__


_LIT8(KTCIUid, "testindexfileuid@symbian.com123");

// for reading entry data from config file

// for "type"
_LIT(KTCIEntryTypeAppt, "appt");
_LIT(KTCIEntryTypeToDo, "todo");
_LIT(KTCIEntryTypeEvent, "event");
_LIT(KTCIEntryTypeRemind, "reminder");
_LIT(KTCIEntryTypeAnniv, "anniv");

// for "method"
_LIT(KTCIEntryMethodNone, "none");
_LIT(KTCIEntryMethodPub, "publish");
_LIT(KTCIEntryMethodReq, "request");
_LIT(KTCIEntryMethodReply, "reply");
_LIT(KTCIEntryMethodAdd, "add");
_LIT(KTCIEntryMethodCancel, "cancel");
_LIT(KTCIEntryMethodRefresh, "refresh");
_LIT(KTCIEntryMethodCounter, "counter");
_LIT(KTCIEntryMethodDeclineCounter, "declinecntr");

// for "rpttype"
_LIT(KTCIEntryRptDaily, "daily");
_LIT(KTCIEntryRptWeekly, "weekly");
_LIT(KTCIEntryRptMonthly, "monthly");
_LIT(KTCIEntryRptYearly, "yearly");

// booleans
_LIT(KTCIEntryTrue, "true");
_LIT(KTCIEntryFalse, "false");

//
_LIT(KTCINumEntries, "numentriespresent");
_LIT(KTCIEntry,	"entry");
_LIT(KAddEntry, "entrytoadd");
_LIT(KDeleteEntry, "entrytodelete");
_LIT(KNumDeleteEntry, "entrynumtodelete");
_LIT(KModifyEntry, "entrytomodify");
_LIT(KNumModifyEntry, "entrynumtomodify");


//
_LIT(KTCIModifiedText, "Modified Description");

#endif //__TEST_CAL_INDEX_FILE_DEFS__
