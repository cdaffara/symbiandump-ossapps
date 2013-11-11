// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALTESTLIB_H__
#define __CALTESTLIB_H__

#include <badesca.h>
#include <e32base.h>
#include <hal.h>
#include <f32file.h>
#include <e32test.h>
#include <calprogresscallback.h>
#include <calentry.h>
#include <coreappstest/testserver.h>
#include <caldataexchange.h>
#include <asclisession.h>
#include <asshdalarm.h>

class CCalSession;
class CCalEntry;
class CCalEntryView;
class CCalInstance;
class CCalInstanceView;
class CCalIter;

_LIT8(KDefaultTimeZone, "Europe/London");

_LIT(KCalTestLibMutex, "CalTestLib_GlobalMutex");

class CCalDataExchange;

class CSetTimezone : public CActive
	{
public:
	static CSetTimezone* NewL(const TDesC8& aZoneId, RPIMTestServer* aPIMTestServer);
	~CSetTimezone();
	void Start();
	
	// from CActive
	void RunL();
	void DoCancel();
	
private:
	void ConstructL(const TDesC8& aZoneId, RPIMTestServer* aPIMTestServer);
	CSetTimezone();
	RPIMTestServer* iPIMTestServer; // Not owned
	HBufC8* iZoneId;
	TInt8 iStep;
	};
	
/**  This is the Calendar Test Library class for Unit testing
@internalComponent
@test
*/
class CCalTestLibrary : public CBase, public MCalProgressCallBack
	{
public:

// Implementation of MCalProgressCallBack interface

	void Progress(TInt /*aPercentageCompleted*/);
	void Completed(TInt /*aError*/);
	TBool NotifyProgress();


// Constructor / Destructor

	// In its destructor, the current opened calendar file will be closed and deleted
	IMPORT_C ~CCalTestLibrary();

	// To be used by most of test code, if it does not contains any asynchronous task. 
	// Use the following functions to create an object. It will start the alarm console alert server.
	IMPORT_C static CCalTestLibrary* NewL(TBool aDelFileFlag=ETrue);
	IMPORT_C static CCalTestLibrary* NewLC(TBool aDelFileFlag=ETrue);	


// File handling

	// Return a file handle.
	IMPORT_C RFs& FileSession();

	// Create a calendar file. It leaves if the file exists.
	IMPORT_C void CreateFileL(const TDesC& aFileName);
	
	// Create a calendar file. It will overwrite the existing file.
	IMPORT_C void ReplaceFileL(const TDesC& aFileName);
	
	// Open a calendar file.
	IMPORT_C void OpenFileL(const TDesC& aFileName, TBool aDelFileFlag=EFalse);

	// Delete either a file which is in server's private directory by default or any other file in public directory.
	IMPORT_C void DeleteFileL(const TDesC& aFileName, TBool aIsCalFile = ETrue);

	// Used to append the file to the delete list
	IMPORT_C void RegisterCalFileL(const TDesC& aFileName);
	
	// Create a calendar file. It leaves if the file exists.
	IMPORT_C RPointerArray<CCalSession>& CreateCalSubSessionL(const TDesC& aFileName);


// Session and Views handling
	
	// To be used by both synchronous  and asynchronous test.
	IMPORT_C CCalSession& GetSession();

	// The following functions cause index building.
	// They should be used by asynchronous test. 
	// There is no guarantee that the views are ready to use when the function returns. 
	// The test code should use the call back function to be notified when the building index is completed.
	IMPORT_C CCalEntryView& AsynCGetEntryViewL();
	IMPORT_C CCalInstanceView& AsynCGetInstanceViewL();

	// The following functions cause index building.
	// They should be used by synchronous test. 
	// The views are ready to be used when the function returns.
	// Those two functions can be called as many times as you like.
	IMPORT_C CCalEntryView& SynCGetEntryViewL();
	IMPORT_C CCalInstanceView& SynCGetInstanceViewL();	

	// Delete entry and instance views
	IMPORT_C void DeleteViews();


// Entry handling

	// Create a GS entry. The uid of the entry will be filled with some random letters if aUid is NULL, otherwise, it use the uid specified.
	IMPORT_C CCalEntry* CreateCalEntryL(CCalEntry::TType aType, HBufC8*& aUid);

	// Set start and end time according to the type of the entry. It will set an random time its start\end time if it is not specified.
	IMPORT_C static void SetEntryStartAndEndTimeL(CCalEntry* entry, const TTime& aStartTime=Time::NullTTime(), const TTime& aEndTime=Time::NullTTime(), TInt aYear = 2000);
	
	// Fill the descriptor with some random letters.
	IMPORT_C void RandomText(TDes& aText, TBool aPossibleNulDes = EFalse);
	IMPORT_C void RandomText8(TDes8& aText,TBool aPossibleNulDes = EFalse);
	
	IMPORT_C void CleanDatabaseL();
	
	IMPORT_C CCalIter& GetIterL();

// Provide access to test server for time setting and file operations
	IMPORT_C RPIMTestServer& PIMTestServer();
	 
// Useful utility functions
	IMPORT_C void StoreEntryL(const CCalEntry& aEntry);
	IMPORT_C void FetchAllInstancesL(RPointerArray<CCalInstance>& aInstances);
	IMPORT_C void OutputInstanceDataL(const RPointerArray<CCalInstance>& aInstances);

	/* Import / Export utility functions */
	IMPORT_C void CreateTestFileL(const TDesC8& aVCalData, const TDesC& aFileName);	
	IMPORT_C void ImportL(const TDesC& aImportVCalendarFile, RPointerArray<CCalEntry>& aEntriesImported);
	IMPORT_C void ImportL(const TDesC8& aVCalendar, RPointerArray<CCalEntry>& aEntriesImported);
	IMPORT_C void ExportL(const TDesC& aExportVCalendarFile, RPointerArray<CCalEntry>& aEntriesExported);
	
	
	IMPORT_C CCalDataExchange& DataExchangeL();
	IMPORT_C CTestRegister& TestRegister();
	
	IMPORT_C void SetTimeZoneL(const TDesC8& aZoneName);
	//Used for closing the agenda serve
	IMPORT_C void CloseAgendaServer();
	IMPORT_C static void WaitForAgendaServerClose();

//Used for reconnecting the agnda server after it has been closed
	IMPORT_C void ConnectAgendaServerL();

#if __WINS__	
	IMPORT_C static void StartAlarmServerL();	
#endif	
	
	IMPORT_C TBool WaitForAlarmEvent(TAlarmId aAlarmId, TAlarmChangeEvent aExpectedEvent, TAlarmState aExpectedState, TInt aTimeout);
	
protected:

// Constructor

	// To be used by derived classes.
	IMPORT_C CCalTestLibrary(TBool aDelFileFlag=ETrue);
	// BaseConstructL(), which starts the alarm console alert server, should be called in the constructor of the derived class.
	// Typically, it is the case that the test contains some asynchronous behaviour.	
	IMPORT_C void BaseConstructL();

private:

// No copy constructor and assignment operator

	CCalTestLibrary(CCalTestLibrary& );
	CCalTestLibrary& operator = (const CCalTestLibrary& );
	void OpenCalendarSessionL(TBool aRefCountExists=ETrue);
	void CloseCalendarSession(TBool aWaitForAgendaToClose=ETrue);
public:
	IMPORT_C void __dbgClearTzClientCacheL(TBool aRestartCaching);	

private:

// Member data

	RPIMTestServer		iPIMTestServer;
	CCalSession*		iSession;
	CCalInstanceView*	iInstanceView;
	CCalEntryView*		iEntryView;
	CCalIter*			iIter;
	CCalDataExchange*	iDataExchange;
	CTestRegister*		iTestRegister;
	TInt64				iSeed;
	RFs					iFs;
	RChunk				iChunk;
	TInt*				iRefCount;
	TInt				iCompletedError;

	CDesCArray*			iFilesToDel;
	TBool				iDelFileFlag;
	RMutex				iChunkMutex;
	RASCliSession		iAlarmServer;
	RPointerArray<CCalSession> iSubCalSessions;
	};


/** Template class for cleanup that needs ResetAndDestroy or Reset to be called

@internalComponent
@test

@code

To be used in the following way:

	// array to store the loaded instances
	
	RPointerArray<CCalInstance> instanceArray;
	CleanupResetAndDestroyPushL(instanceArray);


	iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instanceArray, CalCommon::EIncludeAll, timeRange);

	
	CleanupStack::PopAndDestroy(&instanceArray);

@endcode
*/

template<class T> class CleanupResetAndDestroy
	{
public:
	static inline void PushL(T& aRef);
private:
	static void ResetAndDestroy(TAny* aPtr);
	};

// CleanupResetAndDestroy
	
template <class T> inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
	{
	CleanupStack::PushL(TCleanupItem(&ResetAndDestroy, &aRef));
	}
	
template <class T> void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
	{
	(static_cast<T*>(aPtr))->ResetAndDestroy();
	}

template <class T> inline void CleanupResetAndDestroyPushL(T& aRef)
	{
	CleanupResetAndDestroy<T>::PushL(aRef);
	}	
	

/** Performance timer measurement class

@internalComponent
@test

@code

To be used in the following way:

	static RTest test(_L("The test"));
	
	
	<snip>


	TPerformanceTimer timer(test);

	timer.Start();
		
	<blah blah>
		
	timer.Stop();
	
	test.Printf(_L("Done\n"));	
	
	// printout the time it took to do <blah blah>
	
	timer.PrintOut();

@endcode	
*/

class TPerformanceTimer
	{
public:
	IMPORT_C TPerformanceTimer(RTest& aTest);

	IMPORT_C void Start();
	IMPORT_C void Stop();
	IMPORT_C void PrintOut() const;
	IMPORT_C TTimeIntervalMicroSeconds32 ElapsedTime() const;
	
private:
	RTest& iTest;
	TUint iStartTickCount;
	TUint iEndTickCount;
	};

#endif
