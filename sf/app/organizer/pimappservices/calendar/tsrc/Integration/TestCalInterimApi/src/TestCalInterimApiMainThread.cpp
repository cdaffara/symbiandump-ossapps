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

#include "TestCalInterimApiMainThread.h"

CTestCalInterimApiMainThread::CTestCalInterimApiMainThread()
:CTestCalInterimApiReadData()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalInterimApiMainThread);
	}

void CTestCalInterimApiMainThread::ResetEntryL()
	{
	TInt count = iEntryArray.Count();
	for (TInt ii= 0; ii<count; ++ii)
		{
		iEntryArray[ii]->CopyFromL(*(iOriginalEntryArray[ii]));
		}
	}

CTestCalInterimApiMainThread::~CTestCalInterimApiMainThread()
/**
 * Destructor
 */
	{
	iEntryArray.ResetAndDestroy();
	iEntryArray.Close();
	iEntryArray.ResetAndDestroy();
	iOriginalEntryArray.Close();
	iSemaphore.Close();
	iModifierThread.Close();
	iThread2.Close();
	iThread1.Close();
	iThread.Close();
	}

/** Launches the Notifier Thread which registers with the calendar server
 to receive notifications if there is any updates in the calendar file
*/
void CTestCalInterimApiMainThread::LaunchNotifierThreadsL()
	{
	User::LeaveIfError(iThread.Create(KNotifierThreadName(), RunNotifierThread, KDefaultStackSize, NULL, this, EOwnerProcess));
	User::LeaveIfError(iThread1.Create(KNotifierThreadName1(), RunNotifierThread, KDefaultStackSize, NULL, this, EOwnerProcess));
    User::LeaveIfError(iThread2.Create(KNotifierThreadName2(), RunNotifierThread, KDefaultStackSize, NULL, this, EOwnerProcess));
	}

/** Launches the modifier Thread which performs operations like add, delete, update of
entries in the calendar file and expect the notifications from the Observer Thread
when there in any change in the calendar file.
@param aAdditionalModifier	Param to add more than one modifier thread
*/
void CTestCalInterimApiMainThread::LaunchModifierThreadsL(const TBool& aAdditionalModifier)
	{
	User::LeaveIfError(iModifierThread.Create(KModifierThreadName(), RunModifierThread, KDefaultStackSize, NULL, this, EOwnerProcess));
	if ( aAdditionalModifier ) // If additional Modifier is required, then create additional thread
		{
		User::LeaveIfError(iModifierThread1.Create(KModifierThreadName1(), RunModifierThread, KDefaultStackSize, NULL, this, EOwnerProcess));
		}
	}

/**Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiMainThread::doTestStepL()
	{
	// Share the current teststep logger across threads
	Logger().ShareAuto();

	// Creates a CalChangeNotification thread
	LaunchNotifierThreadsL();

	// Creating a Global Semaphore used to synchronize the Modifier & Observer thread
	GetStringFromConfig(ConfigSection(), KNewSemaphoreName, iSemName);

	TInt error = iSemaphore.CreateGlobal(iSemName, 0);

	if (error == KErrAlreadyExists || error == KErrNone)
		{
		iSemaphore.OpenGlobal(iSemName);
		}
	else
		{
		User::Leave(error);
		}

	TBool	additionalModifier = EFalse;
	GetBoolFromConfig(ConfigSection(), KAdditionalModifier, additionalModifier);

	// Resumes the Notifier Thread/ Threads for registering with the calendar server
	ResumeNotifierThreads();

	// Retrieves the Data file and opens it
	RetreiveIniDataL();

	// Reads data from the text file and adds the entry to the calendar file
	ReadAddEntriesL();

	// Creates the modifier thread where the add,update and delete of entries are done
	LaunchModifierThreadsL(additionalModifier);

	// Resumes the Modifier Thread
	ResumeModifierThreadsL(additionalModifier);

	// Resumes the notifier Thread/ Threads to receive the notification
	ResumeNotifierThreads();

	return TestStepResult();
	}

/** Retrieves the data file from the ini and opens the data file
*/
void CTestCalInterimApiMainThread::RetreiveIniDataL()
	{
	TPtrC	dataFile;
	TESTL( GetStringFromConfig(ConfigSection(), KDataFile1, dataFile) );
	TFileName dataFileName;
#ifdef __WINS__
	dataFileName.Append(_L("c:"));
#else
	TFileName fullName = RProcess().FileName();
	TParsePtrC parse(fullName);
	dataFileName.Append(parse.Drive());
#endif
	dataFileName.Append(dataFile);
	OpenAgendaDataFileL(dataFileName);
	}

/**
* Virtual function called while parsing the entries from the data file
* Takes ownership of aEntry
@param aEntry - current entry being parsed
*/
void CTestCalInterimApiMainThread::ProcessEntryL(CCalEntry* aEntry)
	{
	iEntryArray.AppendL(aEntry);

	HBufC8* uid = aEntry->UidL().AllocLC();
	CCalEntry* entry = NULL;
	if(aEntry->RecurrenceIdL().TimeUtcL()==Time::NullTTime())
		{
		entry = CCalEntry::NewL(aEntry->EntryTypeL(),uid,CCalEntry::EMethodNone,0);
		}
	else
		{
		entry = CCalEntry::NewL(aEntry->EntryTypeL(),uid,CCalEntry::EMethodNone,0,aEntry->RecurrenceIdL(), aEntry->RecurrenceRangeL());
		}
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);
	entry->CopyFromL(*aEntry);
	iOriginalEntryArray.AppendL(entry);
	CleanupStack::Pop(entry);
	}

/** Resumes the Notifier Thread
*/
void CTestCalInterimApiMainThread::ResumeNotifierThreads()
	{
	INFO_PRINTF1(KInfoNotifierThread1);
	iThread.Resume();
	iSemaphore.Wait();

	INFO_PRINTF1(KInfoNotifierThread2);
	iThread1.Resume();
	iSemaphore.Wait();

	INFO_PRINTF1(KInfoNotifierThread3);
	iThread2.Resume();
	iSemaphore.Wait();
	}

/** Resumes the Modifier Thread
@param	aAdditionalModifier	If the flag is set, then resume the additional thread
*/
void CTestCalInterimApiMainThread::ResumeModifierThreadsL(const TBool& aAdditionalModifier)
	{
	ResetEntryL();
	INFO_PRINTF1(KInfoModifierThread1);
	iModifierThread.Resume();
	iSemaphore.Wait();

	if( aAdditionalModifier )
		{
		ResetEntryL();
		INFO_PRINTF1(KInfoModifierThread2);
		iModifierThread1.Resume();
		iSemaphore.Wait();
		}
	}

