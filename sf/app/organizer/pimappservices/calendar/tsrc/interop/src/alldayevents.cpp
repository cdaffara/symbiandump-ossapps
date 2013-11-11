/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

//  Include Files  

#include "alldayevents.h"
#include "../../caltestlib.h"
#include <caltime.h>
#include <caldataformat.h>
#include <versit.h>
#include <e32base.h>
#include <e32std.h>
#include <f32file.h>        // RDir etc.
#include <e32cons.h>	    // Console
#include <e32test.h>        // RTest
#include <s32file.h>        // RFileReadStream


//  Constants
_LIT(KCalendarFile, "alldayevent");
_LIT(KDataDir, "z:\\alldayevents\\");
_LIT(KWriteDataDir,"c:\\alldayevents\\");
_LIT8(KDTSTART, "DTSTART");
_LIT8(KDTEND, "DTEND");

//  Global Variables

LOCAL_D CCalTestLibrary* calTestLibrary; //calendar utility library
RTest test(_L("all_dayevents"));

//  Local Functions
void  ImportEntriesL(const TDesC& aFileName, RPointerArray<CCalEntry>& aCalEntries)
    {
    RFileReadStream fileReadStream;
    CleanupClosePushL(fileReadStream);
    TInt err = fileReadStream.Open(calTestLibrary->FileSession(), aFileName, EFileRead);
    
    calTestLibrary->DataExchangeL().ImportL(KUidVCalendar, fileReadStream, aCalEntries);
	
	CleanupStack::PopAndDestroy(&fileReadStream);
    }
    
    
void ExportEntriesL(RPointerArray<CCalEntry>& aCalEntries, const TDesC& aFileName)
    {
    TEntry dirEntry;
    
    // Create a directory if not present
    if (calTestLibrary->FileSession().Entry(KWriteDataDir(), dirEntry) != KErrNone)
        {
    	calTestLibrary->FileSession().MkDir(KWriteDataDir());
        }
	RFileWriteStream fileWriteStream;
	CleanupClosePushL(fileWriteStream);
	User::LeaveIfError(fileWriteStream.Replace(calTestLibrary->FileSession(), aFileName, EFileWrite));
	calTestLibrary->DataExchangeL().ExportL(KUidVCalendar, fileWriteStream, aCalEntries);
	
	CleanupStack::PopAndDestroy(&fileWriteStream);
    }
    
TBool TestExportedFile(const TDesC& aFilename)
    {
	//Check for DTSTART and DTEND
	//Check it is in DATE-TIME format
	TBool DTSTART_patternFound = EFalse;
	TBool DTEND_patternFound = EFalse;    
    
	
	RFile fileHandle;
	CleanupClosePushL(fileHandle);
	fileHandle.Open(calTestLibrary->FileSession(),aFilename, 0);
	
	
	TBuf8<256> line;
	TInt KDateLength = 8;
	TInt KColon = 1;
	while (fileHandle.Read(line) == KErrNone && line.Length() != 0)
	{
	    TInt pos = line.Find(KDTSTART());
		if (pos != KErrNotFound)
		    {
		    DTSTART_patternFound = ETrue;
		    TText time;
		    time = line[pos + KDTSTART().Length() + KColon + KDateLength ];
		    test (time == 'T');	
	  	    }
	  	pos = line.Find(KDTEND());
	    if ( pos != KErrNotFound)
		    {
			DTEND_patternFound = ETrue;
			TText time;
		    time = line[pos + KDTEND().Length() + KColon + KDateLength ];
			test (time == 'T');
			
			if (DTSTART_patternFound && DTEND_patternFound)
			break;
		    }
	} 
	CleanupStack::PopAndDestroy(&fileHandle);
	return (DTSTART_patternFound && DTEND_patternFound);
	
    }

TInt CheckTimezoneRules(const CCalEntry& aCalEntry)
	{
	//See if timezone rules are present, if they are then times should be UTC and not floating
	//
	CTzRules* tzRules = aCalEntry.TzRulesL();
	TCalTime startTime = aCalEntry.StartTimeL();
	if (tzRules)
		{
		test (startTime.TimeMode() !=  TCalTime::EFloating);
		}
	
	return KErrNone;
	}


/**

@SYMTestCaseID     PIM-ALLDAYEVENTS-0001

*/

LOCAL_C void MainL()
	{
	test.Start(_L("@SYMTESTCaseID:PIM-ALLDAYEVENTS-0001 Getting list of all day events data...."));

	RDir dataDir;
	CleanupClosePushL(dataDir);
	//Open the Data directory and read the file entries
	User::LeaveIfError(dataDir.Open(calTestLibrary->FileSession(), KDataDir, KEntryAttNormal));
	
	TEntryArray* dataArray = new (ELeave) TEntryArray();

	CleanupStack::PushL(dataArray);
	
	dataDir.Read(*dataArray);
	
	TInt count = dataArray->Count();
	test (count != 0);
		
	for (TInt i=0; i < count; ++i)
	    {
	    test.Next(_L("ALLDAYEVENTS-0002"));

	    TBuf<256> name;
	    name = KDataDir;
	    name.Append((*dataArray)[i].iName);
	    
	    RPointerArray<CCalEntry> calEntries;
	    CleanupResetAndDestroyPushL(calEntries);
	    ImportEntriesL(name, calEntries);
	    TInt calEntriesCount = calEntries.Count();
	    if (calEntriesCount)	
	        {
	        //Test entry for the presence of timzone rules and UTC
	        CheckTimezoneRules(*calEntries[0]);
	        }
	    name = KWriteDataDir;
	    name.Append((*dataArray)[i].iName);
	    
	    ExportEntriesL(calEntries, name);
	    
	    TBool result = TestExportedFile(name);
	    
	    if (result)
	        {
	    	calTestLibrary->FileSession().Delete(name);
	        }
	    
	    CleanupStack::PopAndDestroy(&calEntries);
	    }
	
	CleanupStack::PopAndDestroy(dataArray);
	CleanupStack::PopAndDestroy(&dataDir);
	// This will fail if there was a parsing error in one or more of the data files
	// which will still be then present in the directory for inspecting.
	test (calTestLibrary->FileSession().RmDir(KWriteDataDir) == KErrNone);
	
	test.End();
	test.Close();
	}


LOCAL_C void DoStartL()
	{
	// Create active scheduler (to run active objects)
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
    calTestLibrary = CCalTestLibrary::NewL();
	CleanupStack::PushL(calTestLibrary);
	calTestLibrary->ReplaceFileL(KCalendarFile);
	calTestLibrary->OpenFileL(KCalendarFile);
	
	MainL();
    CleanupStack::PopAndDestroy(calTestLibrary);
	// Delete active scheduler
	CleanupStack::PopAndDestroy(scheduler);
	}


//  Global Functions

GLDEF_C TInt E32Main()
	{
	// Create cleanup stack
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(mainError, DoStartL());
	
	delete cleanup;
	__UHEAP_MARKEND;
	return mainError;
	}



