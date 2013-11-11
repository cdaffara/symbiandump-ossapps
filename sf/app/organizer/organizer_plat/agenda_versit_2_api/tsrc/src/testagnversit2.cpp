/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   test agnversit2 api
*
*/



//  CLASS HEADER
#include "testagnversit2.h"
#include <BADESCA.H>

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <CalEntry.h>				// CCalEntry
#include <calentryview.h>           // CCalEntryView
#include <CalSession.h>				// CCalSession
#include <f32file.h>				// RFs
#include <s32file.h>				// RFileWriteStream, RFileReadStream
#include <caliterator.h>
#include <EIKENV.H>
#include <calInstanceView.h>
#include <calinstance.h>


//  INTERNAL INCLUDES
#include "AgnVersit2.h"				// CAgnVersit2
#include "AgnVersit2ExportFlags.h"	// TAgnExportMethod
#include "AgnVersit2TestConstants.h"// Constants
#include "ICalParser.h"
#include "ICal.h"
//#include <CalenUtils.h>
#include <CalenDateUtils.h>
#include <CleanupResetAndDestroy.h>
#include "AgnImportObserver.h"		// MAgnImportObserver
#include "AgnExportObserver.h"		// MAgnExportObserver


#include <tz.h>
#include <TzLocalizer.h>
#include <TzLocalizationDataTypes.h>
#include <calsession.h>
#include <calentryview.h>

// path and filename for Calendar database,owned by calendar.
// TODO: This needs to be read from central repo to remove dependancy on calendar.
_LIT( KCalendarDatabaseFilePath, "c:Calendar" );

const TUint32 KLondonTimeZoneId(2632);

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::NewL
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CCAgnVersit2Test* CCAgnVersit2Test::NewL()
    {
    CCAgnVersit2Test* self = CCAgnVersit2Test::NewLC();
    CleanupStack::Pop();

    return self;
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::NewLC
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CCAgnVersit2Test* CCAgnVersit2Test::NewLC()
    {
    CCAgnVersit2Test* self = new( ELeave ) CCAgnVersit2Test();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::~CCAgnVersit2Test
// Destructor (virtual by CBase)
// -----------------------------------------------------------------------------
//
CCAgnVersit2Test::~CCAgnVersit2Test()
    {
    
  /*  SetupL();
    CleanDatabaseL();
    Teardown(); */
	SetCalendarViewStatus(ECalViewCreateNone);

	if(iEntryAsyncWait)
		{
		delete iEntryAsyncWait;
		iEntryAsyncWait =  NULL;
		}
	
	if(iQueueManagerAsw)
		{
		delete iQueueManagerAsw;
		iQueueManagerAsw =  NULL;
		}
		
	if(iEntryView)
		{
		delete iEntryView;
		iEntryView = NULL;	
		}    		

	if( iCalSession )
	    {
	    delete iCalSession;
	    iCalSession = NULL;
	    }
        
     }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::CCAgnVersit2Test
// Default constructor
// -----------------------------------------------------------------------------
//
CCAgnVersit2Test::CCAgnVersit2Test()
    {
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::ConstructL
// Second phase construction
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    TInt err = KErrNone;

	// Creating session with Calendar server
	if(!iCalSession)
		{
		iCalSession = CCalSession::NewL();
		TRAP( err, iCalSession->OpenL(KCalendarDatabaseFilePath));
		User::LeaveIfError( err );

		// active scheduler wait object to wait for Calendar view creation
		iEntryAsyncWait = new(ELeave) CActiveSchedulerWait();
		
		// active scheduler wait object to stop new view creation req. while one is ongoing.
		iQueueManagerAsw = new(ELeave) CActiveSchedulerWait();
		}

	if(!iEntryView && !(iEntryAsyncWait->IsStarted()) )
		{
		
		// Creating Calendar Entry View for retrieving event details.
	    iEntryView = CCalEntryView::NewL( *iCalSession, *this );
		SetCalendarViewStatus(ECalViewCreateReqPending);
		
		// Since View creation is aynshronous,the thread need to wait till view creation completed
		iEntryAsyncWait->Start();

		// If Async wait  queue manager is still in in waiting,Cancel it since view creation completed.
		if(iQueueManagerAsw->IsStarted())
			{
			iQueueManagerAsw->AsyncStop();
			}
		
		}
	else
		{	
		// Waiting for completion of current view creation request.
		if(iQueueManagerAsw && !iQueueManagerAsw->IsStarted())
			{
			iQueueManagerAsw->Start();
			}
		}
	// Leaves if view creation failed...
	User::LeaveIfError(CalendarViewStatus());
    
 	/*   SetupL();
    CleanDatabaseL();
    Teardown();*/
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SetupL
// Setup
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SetupL()
    {

	//Create file server session
	EUNIT_ASSERT_EQUALS(iFs.Connect(), KErrNone);

	//Create AgnVersit2 interface through ECOM
	iInterface = CAgnExternalInterface::NewL(KICalMimeType);

	iInputDir 	= HBufC::NewL(KFileNameLength);
	iOutputDir	= HBufC::NewL(KFileNameLength);

	iInputDir->Des().Append(_L("E://testing//data//unittest//agnversit2//input//"));
	iOutputDir->Des().Append(_L("E://testing//data//unittest//agnversit2//output//"));

	//Make an output directory if needed
	iFs.MkDirAll(*iOutputDir);

	//Get the filenames to use from the ini file
	iImportFile = HBufC::NewL(KFileNameLength);
	iExportFile1 = HBufC::NewL(KFileNameLength);
	iExportFile2 = HBufC::NewL(KFileNameLength);	
	iCheckFile	= HBufC::NewL(KFileNameLength);


    iEikEnv = CEikonEnv::Static();
       
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::Teardown
// Teardown
// -----------------------------------------------------------------------------
//  
void CCAgnVersit2Test::Teardown()
    {
        
    delete iString;
    iString = NULL;
	delete iImportFile;
	iImportFile = NULL;
	delete iExportFile1;
	iExportFile1 = NULL;
	delete iExportFile2;
	iExportFile2 = NULL;	
	delete iCheckFile;
	iCheckFile = NULL;
	delete iInputDir;
	iInputDir = NULL;
	delete iOutputDir;
	iOutputDir = NULL;
	iImportedEntries.ResetAndDestroy();
   	iEntriesFromDb1.ResetAndDestroy();
   	iEntriesFromDb2.ResetAndDestroy();
	delete iInterface;
	iInterface = NULL;
	iFs.Close();

    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::AgnImportErrorL
// From MAgnImportObserver. Reports errors ocurring during import. Displays a
// warning message, and requests the import should attempt to continue if possible.
// @param aType The type of error
// @param aUid uid of the component the error ocurred in
// @param aContext type of the component that the error ocurred in.
// @return Value specified in ini file as OnError
// @leave KErrDied if OnError is Die
// @internalTechnology
// -----------------------------------------------------------------------------
//
MAgnImportObserver::TImpResponse CCAgnVersit2Test::AgnImportErrorL(
	TImpError aType,
	const TDesC8& /*aUid*/, 
	const TDesC& aContext)
	{
	if (!iOOMTesting)
		{
		switch (aType)
			{
			case EImpErrorMissingData:
			    EUNIT_PRINT(KImpErrorMissingData());
			    EUNIT_PRINT(aContext);
				break;
			case EImpErrorNotSupported:
			    EUNIT_PRINT(KImpErrorNotSupported());
			    EUNIT_PRINT(aContext);
				break;
			case EImpErrorInvalidData:
			    EUNIT_PRINT(KImpErrorInvalidData());
			    EUNIT_PRINT(aContext);
				break;
			default:
			    EUNIT_PRINT(KImpError());
			    EUNIT_PRINT(aContext);
				break;
			}
		}
		
	if (iDieOnError)
		{
		EUNIT_ASSERT_SPECIFIC_LEAVE(User::Leave(KErrDied), KErrDied);
		}
	return iOnImpError;
	}

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::
// From MAgnExportObserver. Reports errors ocurring during export. Displays a
// warning message, and requests the export should attempt to continue if possible.
// @param aType The type of error
// @param aUid uid of the component the error ocurred in
// @param aContext type of the component that the error ocurred in.
// @return Value specified in ini file as OnError
// @leave KErrDied if OnError is Die
// @internalTechnology
// -----------------------------------------------------------------------------
//
MAgnExportObserver::TExpResponse CCAgnVersit2Test::AgnExportErrorL(
	TExpError /*aType*/,
	const TDesC8& /*aUid*/, 
	const TDesC& aContext)
	{
	if (!iOOMTesting)
		{
		EUNIT_PRINT(KExpError());
		EUNIT_PRINT(aContext);
		}
	if (iDieOnError)
		{
		EUNIT_ASSERT_SPECIFIC_LEAVE(User::Leave(KErrDied), KErrDied);
		}
	return iOnExpError;
	}

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::ImportL
// Imports an iCalendar entry from iImportFile.
// @param aExportFile the file to import from. Default 0 means take from ini file.
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::ImportL()
	{
	RFileReadStream fileStream;
	EUNIT_ASSERT_EQUALS(fileStream.Open(iFs, *iImportFile, EFileRead), KErrNone);
	CleanupClosePushL(fileStream);
	iInterface->ImportL(iImportedEntries, fileStream, 0, *this);
	CleanupStack::PopAndDestroy(); // fileStream
	}

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::ExportL
// Exports an iCalendar entry to iExportFile.
// @param aExportFile the file to export to. Default 0 means take from ini file.
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::ExportL(const RPointerArray<CCalEntry> &aEntryArray,  HBufC* aExportFile )
	{
	RFileWriteStream writeStream;
	EUNIT_ASSERT_EQUALS(writeStream.Replace(iFs, *aExportFile, EFileWrite), KErrNone);
	CleanupClosePushL(writeStream);
	
	//Export the entries
	TInt count = aEntryArray.Count();
	
	TInt flags = EExportAll;
		
	for (TInt ent = 0; ent < count; ++ent)
		{
		flags = GetExportFlagsL(*aEntryArray[ent]);	
		iInterface->ExportL(*aEntryArray[ent], writeStream, flags, *this);
		}
	
	CleanupStack::PopAndDestroy(); // Close writeStream.
	}

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::GetExportFlagsL
// Get the correct flags for export
// -----------------------------------------------------------------------------
//
TInt CCAgnVersit2Test::GetExportFlagsL(const CCalEntry& aEntry)
	{
	switch (aEntry.EntryTypeL())
		{
		case CCalEntry::EEvent:
			{
			switch (aEntry.MethodL())
				{
				case CCalEntry::EMethodNone:
					return EExportAll;
				
				case CCalEntry::EMethodPublish:
					return EVEventPublish;
				
				case CCalEntry::EMethodRequest:
					return EVEventRequest;
				
				case CCalEntry::EMethodReply:
					return EVEventReply;
				
				case CCalEntry::EMethodAdd:
					return EVEventAdd;
				
				case CCalEntry::EMethodCancel:
					return EVEventCancel;
				
				case CCalEntry::EMethodRefresh:
					return EVEventRefresh;
				
				case CCalEntry::EMethodCounter:
					return EVEventCounter;
				
				case CCalEntry::EMethodDeclineCounter:
					return EVEventDeclineCounter;
				}
			}
		break;
		case CCalEntry::ETodo:
			{
			switch (aEntry.MethodL())
				{
				case CCalEntry::EMethodNone:
					return EExportAll;
			
				case CCalEntry::EMethodPublish:
					return EVTodoPublish;
				
				case CCalEntry::EMethodRequest:
					return EVTodoRequest;
				
				case CCalEntry::EMethodReply:
					return EVTodoReply;
				
				case CCalEntry::EMethodAdd:
					return EVTodoAdd;
				
				case CCalEntry::EMethodCancel:
					return EVTodoCancel;
				
				case CCalEntry::EMethodRefresh:
					return EVTodoRefresh;
				
				case CCalEntry::EMethodCounter:
					return EVTodoCounter;
				
				case CCalEntry::EMethodDeclineCounter:
					return EVTodoDeclineCounter;
				
				}
			}
		}
	return EExportAll;
	}

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::CompareFilesL
// Compares the contents of iImportFile against the contents of iExportFile line by line
// @return ETrue if the files are identical, otherwise EFalse 
// -----------------------------------------------------------------------------
//
TBool CCAgnVersit2Test::CompareFilesL( HBufC* aExportFile, HBufC* aCheckFile )
	{
	TBool identical = ETrue;

	// Open a read stream on both files
	RFileReadStream fileStream1;
	RFileReadStream fileStream2;
	EUNIT_ASSERT_EQUALS(fileStream1.Open(iFs, *aExportFile, EFileRead), KErrNone);
	CleanupClosePushL(fileStream1);
	EUNIT_ASSERT_EQUALS(fileStream2.Open(iFs, *aCheckFile, EFileRead), KErrNone);
	CleanupClosePushL(fileStream2);

	// Buffer to hold each line of a file
	HBufC8* buf1 = HBufC8::NewLC(KICalMaxContentLineLength);
	HBufC8* buf2 = HBufC8::NewLC(KICalMaxContentLineLength);

	TPtr8 buf1Ptr = buf1->Des();
	TPtr8 buf2Ptr = buf2->Des();

	TInt error1(KErrNone);
	TInt error2(KErrNone);

	// Compare the files line by line, excluding certain
	// properties we expect to be different (eg the time stamp)
	while (error1 == KErrNone && error2 == KErrNone && identical)
		{
		TRAP(error1, GetLineL(fileStream1, buf1Ptr));
		TRAP(error2, GetLineL(fileStream2, buf2Ptr));
		
		if (error1 != error2)
			{
			identical = EFalse;
			}
		else if (error1 == KErrNone)
			{
			if (buf1Ptr.Find(KDtStamp) == 0)
				{
				EUNIT_ASSERT_EQUALS(identical = buf2Ptr.Find(KDtStamp), 0);
				identical = (buf2Ptr.Find(KDtStamp) == 0);
				}
			else if (buf1Ptr.Find(KCreated) == 0)
				{
				EUNIT_ASSERT_EQUALS(identical = buf2Ptr.Find(KCreated), 0);
				identical = (buf2Ptr.Find(KCreated) == 0);
				}
			else if (buf1Ptr.Find(KLastMod) == 0)
				{
				EUNIT_ASSERT_EQUALS(identical = buf2Ptr.Find(KLastMod), 0);
				identical = (buf2Ptr.Find(KLastMod) == 0);
				}
			else if (buf1Ptr.Find(KProdId)==0)
				{
				EUNIT_ASSERT_EQUALS(identical = buf2Ptr.Find(KProdId), 0);
				identical = (buf2Ptr.Find(KProdId) == 0);	
				}
	
			else
				{
				EUNIT_ASSERT_EQUALS(identical = buf1Ptr.CompareF(buf2Ptr), 0);
				identical = (buf1Ptr.CompareF(buf2Ptr) == 0);
				}
			}
		}
		
	CleanupStack::PopAndDestroy(buf2);
	CleanupStack::PopAndDestroy(buf1);
	CleanupStack::PopAndDestroy(&fileStream2);
	CleanupStack::PopAndDestroy(&fileStream1);
		
	return identical;
	}

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::GetLineL
// Helper function used by CompareFilesL.
// Reads from a RFileReadStream into a descriptor up to the next new line
// @param aStream stream to read from
// @param aPtr TPtr8 to read the data into
// @leave KErrEof when the end of the stream is reached
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::GetLineL(RFileReadStream& aStream, TPtr8& aPtr)
	{
	aStream.ReadL(aPtr, TChar(KNewline));	// Read up to the next newline.
	}

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::GetStringFromFileL
// Get the string from File
// -----------------------------------------------------------------------------
//	
void CCAgnVersit2Test::GetStringFromFileL(const TDesC8& aSectName, const TDesC8& aKeyName, TPtrC& aResult)
    {     
    HBufC* configFile	= HBufC::NewLC(KFileNameLength);

    configFile->Des().Insert(0, *iInputDir);
	configFile->Des().Append(_L("AgnVersit2Test.ini"));    

    RFileReadStream fileStream;
	EUNIT_ASSERT_EQUALS(fileStream.Open(iFs, *configFile, EFileRead), KErrNone);
	CleanupClosePushL(fileStream);

	// Buffer to hold each line of a file
	HBufC8* buf = HBufC8::NewLC(KICalMaxContentLineLength);
	TPtr8 bufPtr = buf->Des();
	
	TInt error(KErrNone);
	TBool found = EFalse;
    aResult.Set(NULL, 0); 
    
    while (error == KErrNone && !found)
		{
        TRAP(error, GetLineL(fileStream, bufPtr));
		
		if (bufPtr.Find(aSectName) == 1)
		    {
		    while(error == KErrNone && !found)
		        {
		        if(bufPtr.Find(aKeyName) == 0)
		            {
		            TInt i;

		            TPtrC8 ptr(bufPtr);
		            i = ptr.Find(KSpace2)+1;
		            ptr.Set(ptr.Right(ptr.Length() - i));
                    i = ptr.Find(KEndofLine);
                    
                    RDebug::RawPrint( ptr );
                    RDebug::RawPrint( ptr.Left(i) );
                    
                    ptr.Set(ptr.Left(i));

                   if(iString)
                        {
                        delete iString;
                        iString = NULL;
                        }

                    iString = Convert8To16BitL(ptr);
                    aResult.Set( iString->Des().PtrZ(), ptr.Length());
                    RDebug::RawPrint( aResult );
                	found = ETrue;
		            }
		        else
		            {
		            TRAP(error, GetLineL(fileStream, bufPtr)); 
		            }
		        }
		    }
		}
	CleanupStack::PopAndDestroy(buf);
    CleanupStack::PopAndDestroy(&fileStream);
    CleanupStack::PopAndDestroy(configFile);
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::Convert8To16BitL
// Conversion
// -----------------------------------------------------------------------------
//
HBufC16* CCAgnVersit2Test::Convert8To16BitL(const TDesC8& aDes8)
    {
    HBufC16* myBuf = HBufC16::NewL(aDes8.Length()+1);
    TPtr16 ptr = myBuf->Des();
    ptr.Copy(aDes8);
    return myBuf;
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::ErrorCheck
// Check Error
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::ErrorCheck(const TDesC& aDes)
    {
    iDieOnError = EFalse;
	if (aDes.CompareF(KResponseSkip) == 0)
		{
		iOnImpError = MAgnImportObserver::EImpResponseSkip;
		iOnExpError = MAgnExportObserver::EExpResponseStop; // exports one at once so no skip
		}
	else if (aDes.CompareF(KResponseLeave) == 0)
		{
		iOnImpError = MAgnImportObserver::EImpResponseLeave;
		iOnExpError = MAgnExportObserver::EExpResponseLeave;
		}
	else if (aDes.CompareF(KResponseStop) == 0)
		{
		iOnImpError = MAgnImportObserver::EImpResponseStop;
		iOnExpError = MAgnExportObserver::EExpResponseStop;
		}
	else if (aDes.CompareF(KResponseDie) == 0)
		{
		iDieOnError = ETrue;
		iOnImpError = MAgnImportObserver::EImpResponseStop;
		iOnExpError = MAgnExportObserver::EExpResponseStop;
		}
	else
		{
		iOnImpError = MAgnImportObserver::EImpResponseContinue;
		iOnExpError = MAgnExportObserver::EExpResponseContinue;
		}
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::GetFilesL
// Get files
// -----------------------------------------------------------------------------
//   
void CCAgnVersit2Test::GetFilesL(const TDesC8& aDes8)
    {

    TPtrC fileName(NULL, KICalMaxContentLineLength);
    TPtr test(NULL, 0);

    GetStringFromFileL(aDes8, KImportFileName, fileName);
	iImportFile->Des().Insert(0, *iInputDir);
  	test.Set(const_cast<TUint16*>(fileName.Ptr()), fileName.Length(), fileName.Length());
	test.TrimAll();
	iImportFile->Des().Append(test);

    GetStringFromFileL(aDes8, KExportFileName1, fileName);
	iExportFile1->Des().Insert(0, *iOutputDir);
	test.Set(const_cast<TUint16*>(fileName.Ptr()), fileName.Length(), fileName.Length());
	test.TrimAll();
	iExportFile1->Des().Append(test);

    GetStringFromFileL(aDes8, KExportFileName2, fileName);    
	iExportFile2->Des().Insert(0, *iOutputDir);	
	test.Set(const_cast<TUint16*>(fileName.Ptr()), fileName.Length(), fileName.Length());
	test.TrimAll();
	iExportFile2->Des().Append(test);
	
    GetStringFromFileL(aDes8, KCheckFileName, fileName);
	iCheckFile->Des().Insert(0,*iInputDir);
	test.Set(const_cast<TUint16*>(fileName.Ptr()), fileName.Length(), fileName.Length());
	test.TrimAll();
	iCheckFile->Des().Append(test);

    GetStringFromFileL(aDes8, KError, fileName);
	test.Set(const_cast<TUint16*>(fileName.Ptr()), fileName.Length(), fileName.Length());
	test.TrimAll();
    ErrorCheck(test);    
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::doTestStepL
// Execute the test steps
// -----------------------------------------------------------------------------
//    
void CCAgnVersit2Test::doTestStepL(  )
    {    
    
    TInt err = KErrNone;
    
    // Initialise Time Zone to London Time Zone
    SetInitialTimeZoneL();
    /**
    
    IN.ICS --> PART 1 ---> CCalEntry Array iEntriesFromDb1  
                      ---> <TestCaseName>Out1.ics 
    
    ---> Do some cleanup of Part 1 before Part 2 
    (i.e.  Delete iEntriesFromDb1 entries from dB, 
           ClearGuidAndLuidL for iEntriesFromDb1 )  
    
    ---> PART 2 ---> CCalEntry Array iEntriesFromDb2
                ---> <TestCaseName>Out2.ics
                
    <TestCaseName>Out1.ics --->  
    <TestCaseName>Out2.ics ---> PART 3 --> Final Test Result     
    <TestCaseName>Check.ics -->                
    
    PART 1: Given IN.ICS as input, what do we get back from Symbian Db ?
    
    PART 2: Given Symbians output, Do we get the same output if
            we write Symbians own putput back to DB ?
    
    PART 3: Compare PART 1 & PART 2's results with an expected result
    
    */
    
    /*********************** PART 1 ***************************/
    // 1. Import the iCal to iImportedEntries
    TRAP(err, ImportL());
    if(err)
        {
        EUNIT_PRINT( _L( "1. Import the iCal to iImportedEntries fails" ) );
        EUNIT_ASSERT( EFalse ); // fail test
        }
    
    // 2. Write iImportedEntries to dB    
    TRAP(err, WriteToDbL(iImportedEntries));
    if(err)
        {
        EUNIT_PRINT(_L( "2. Write iImportedEntries to dB fails" ) );
        EUNIT_ASSERT( EFalse ); // fail test
        }

    // 3. Fetch iImportedEntries from DB into iEntriesFromDb1
    TRAP(err, FetchFromDbL(iImportedEntries, iEntriesFromDb1));
    if(err)
        {
        EUNIT_PRINT(_L( "3. Fetch iImportedEntries from DB into iEntriesFromDb1 fails" ) );
        EUNIT_ASSERT( EFalse ); // fail test
        }
    
    // 4. Export iEntriesFromDb1 to "<TestCaseName>Out1.ics" file
    TRAP(err, ExportL(iEntriesFromDb1, iExportFile1 ));
    if(err)
        {
        EUNIT_PRINT(_L( "4. Export iEntriesFromDb1 to <TestCaseName>Out1.ics file fails" ) );
        EUNIT_ASSERT( EFalse );
        }
   
    /*********** END OF PART 1 **********************/    
    
    // 5. Delete iEntriesFromDb1 entries from dB
    TRAP(err, RemoveImportedEntriesFromDBL(iEntriesFromDb1));
    if(err)
        {
        EUNIT_PRINT(_L( "5. Delete iEntriesFromDb1 entries from dB fails" ) );
        EUNIT_ASSERT( EFalse );
        }        

    // 6. ClearGuidAndLuid
    //    Now the imported entries are deleted from Db.
    //    if we dont clear the Guid and Luid, CEntryView::StoreL would leave with
    //    -1         
    TRAP(err, ClearGuidAndLuidL(iEntriesFromDb1));
    if(err)
        {
        EUNIT_PRINT(_L( "6. ClearGuidAndLuid fails" ) );
        EUNIT_ASSERT( EFalse );
        }        
       
    /********************* PART 2 ****************************/
        
    // 7. Write iEntriesFromDb1 to dB back again
    //    Lets see, if Symbian will export its "own stuff" correctly    
    TRAP(err, WriteToDbL(iEntriesFromDb1));
    if(err)
        {
        EUNIT_PRINT(_L( "7. Write iEntriesFromDb1 to dB back again fails" ) );
        EUNIT_ASSERT( EFalse );
        }

    // 8. Fetch iEntriesFromDb1 from DB into iEntriesFromDb2
    TRAP(err, FetchFromDbL(iEntriesFromDb1, iEntriesFromDb2));
    if(iEntriesFromDb2.Count() == 0)
        {
        EUNIT_PRINT(_L( "8. Fetch iEntriesFromDb1 from DB into iEntriesFromDb2" ) );
        EUNIT_ASSERT( EFalse );
        }

    // 9. Compare iEntriesFromDb1 & iEntriesFromDb2
    TBool equal = EFalse;
    TRAP(err, equal = CompareL(iEntriesFromDb1, iEntriesFromDb2));
    if(err || !equal)
        {
        EUNIT_PRINT(_L( "9. Compare iEntriesFromDb1 & iEntriesFromDb2" ) );
        EUNIT_ASSERT( EFalse );
        }
         
    // iEntriesFromDb2 & iEntriesFromDb1 are equal 
    // 10. Export iEntriesFromDb2    
    TRAP(err, ExportL(iEntriesFromDb2, iExportFile2 ));
    if(err)
        {
        EUNIT_PRINT(_L( "10. Export iEntriesFromDb2 fails" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    // 11. Compare *Check.ics & *Out1.ics
    // Y:\epoc32\RELEASE\winscw\udeb\Z\private\101FDAB0\<TestCaseName>Check.ics
    // Y:\epoc32\WINSCW\C\AgnVersit2Test\<TestCaseName>Out1.ics
    // Y:\epoc32\WINSCW\C\AgnVersit2Test\<TestCaseName>Out2.ics
    equal = EFalse;
    TRAP(err, equal = CompareFilesL(iExportFile1, iCheckFile) );
    if(err || !equal)
        {
        EUNIT_PRINT(_L( "11. Compare *Check.ics & *Out1.ics fails" ) );
        EUNIT_ASSERT( EFalse );
        }

    // 12. Compare *Check.ics & *Out2.ics
    // Y:\epoc32\RELEASE\winscw\udeb\Z\private\101FDAB0\<TestCaseName>Check.ics
    // Y:\epoc32\WINSCW\C\AgnVersit2Test\<TestCaseName>Out1.ics
    // Y:\epoc32\WINSCW\C\AgnVersit2Test\<TestCaseName>Out2.ics
    equal = EFalse;    
    TRAP(err, equal = CompareFilesL(iExportFile2, iCheckFile) );
    if(err || !equal)
        {
        EUNIT_PRINT(_L( "12. Compare *Check.ics & *Out2.ics fails" ) );
        EUNIT_ASSERT( EFalse );
        }

    // 13. Clean DB totally. This is a MUST for the next test
    TBool databaseHasEntries(EFalse);    
    TRAP(err, databaseHasEntries = CleanDatabaseL() );
    if(err || databaseHasEntries)
        {
        EUNIT_PRINT(_L( "13. Clean DB totally fails" ) );
        EUNIT_ASSERT( EFalse );
        }
        
    
    // test passed..            
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::Completed
// From MCalProgressCallBack
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::Completed(TInt aError)
    {
    DoCompletedL(aError);
    }
            
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::NotifyProgress
// Notify the observer
// -----------------------------------------------------------------------------
//
TBool CCAgnVersit2Test::NotifyProgress()
    {
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::Progress
// Notify the progress
// -----------------------------------------------------------------------------
//       
void CCAgnVersit2Test::Progress(TInt /*aPercentageCompleted*/)
    {            
    // do nothing, we are not interested in Progress notifications
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::DoCompletedL
// 
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::DoCompletedL(TInt aFirstPassError)
    {
    
    if(iEntryAsyncWait->IsStarted())
		{
		iEntryAsyncWait->AsyncStop();
		}

	if(iQueueManagerAsw->IsStarted())
		{	
		iQueueManagerAsw->AsyncStop();
		}
		
	
	if(KErrNone == aFirstPassError)
		{
		SetCalendarViewStatus(ECalViewCreateCompleted);
		}
	else
		{
		SetCalendarViewStatus(ECalViewCreateError);
		}
         
    
   // CActiveScheduler::Stop();
        
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::CleanDatabaseL
// Clean data base
// -----------------------------------------------------------------------------
//
TBool CCAgnVersit2Test::CleanDatabaseL()
    {     
    
          
 /*  TCalTime start;
    TCalTime end; 
    CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
        
    start.SetTimeUtcL(TCalTime::MinTime());
    end.SetTimeUtcL(TCalTime::MaxTime());    
    CalCommon::TCalTimeRange range(start, end);
        
   iEntryView->DeleteL(range, filter, *this);
   	 
    
   // TTimeIntervalMicroSeconds32 wait(500000);
   // User::After(wait);
    
    //CActiveScheduler::Start();

    // database must be totaly empty..    
    CCalIter* iter=CCalIter::NewL(iGlobalData->CalSessionL());
    CleanupStack::PushL(iter);
    
    TPtrC8 Uid(iter->FirstL());
	TInt ii = 0;
	while (Uid != KNullDesC8())
		{
		++ii;
		Uid.Set(iter->NextL());
		}
		
    CleanupStack::PopAndDestroy(iter);

    TBool notEmpty = (ii > 0) ? ETrue : EFalse ;*/
    
    TInt c1 = iEntriesFromDb1.Count();
    TInt c2 = iEntriesFromDb2.Count();
   	TInt c3 = iImportedEntries.Count();
   	
   	if(CalendarViewStatus()== ECalViewCreateCompleted && NULL!=iEntryView)
		{   
	     for(TUint i = 0; i < c1; ++i)
	        {
	        iEntryView->DeleteL(*iEntriesFromDb1[i]);
	        }
	     
	     for(TUint i = 0; i < c2; ++i)    
	     	{
	     //	iEntryView->DeleteL(*iEntriesFromDb2[i]);
	     	}
	   	
	   	 for(TUint i = 0; i < c3; ++i)
	   	 	{
	   	 //	iEntryView->DeleteL(*iImportedEntries[i]);
	   	 	}
		}
	else
		{
		// leaving if Calender Entry view is not available.
		User::Leave(KErrNotFound);
		}	
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::ClearGuidAndLuidL
// Clear GUid and Luid
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::ClearGuidAndLuidL(RPointerArray<CCalEntry>& aEntries)    
    {
    TUint count = aEntries.Count();
    for(TUint i = 0; i < count; ++i)
        {
        // get entry i
        CCalEntry* entry = aEntries[i];
        
        // copy uid
        TInt size = (entry->UidL()).Size();
        HBufC8* uid = HBufC8::NewLC(size);
        uid->operator=(entry->UidL()); // owner ship to be passed
        
        // create a new empty entry with this uid
        CCalEntry::TMethod method = CCalEntry::EMethodNone; // default
       	TUint entrySeq = 0; // default
       	CCalEntry::TType type = entry->EntryTypeL();
		CCalEntry* newEntry = CCalEntry::NewL(type, uid, method, entrySeq);       
        CleanupStack::Pop(uid); // ownership passed
        
        // keep Guid & set Luid to default
        newEntry->CopyFromL(*entry); // Luid & Guid
        newEntry->SetLocalUidL(TCalLocalUid(0));
        
        // replace aEntries[i] with newEntry
        delete entry;
        aEntries.Insert(newEntry, i);
        aEntries.Remove(i+1); // remove "entry"        
        }
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::CompareL
// Comparre entries
// -----------------------------------------------------------------------------
//
TBool CCAgnVersit2Test::CompareL(RPointerArray<CCalEntry>& aEntries1, RPointerArray<CCalEntry>& aEntries2)
    {
    TUint count = aEntries1.Count();
    
    if(count != aEntries2.Count())
        {
        return EFalse;
        }
    
    for(TUint i = 0; i < count; ++i)
        {
        CCalEntry* entry1 = aEntries1[i];
        CCalEntry* entry2 = aEntries2[i];
        
        TBool equal = entry1->CompareL(*entry2);
        if(!equal)
            {
            return equal;
            }
        }
        
    return ETrue;   // equal
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::RemoveImportedEntriesFromDBL
// Remove Imported Entries From DBL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::RemoveImportedEntriesFromDBL(RPointerArray<CCalEntry>& aEntriesToDelete)
    {
    TUint count = aEntriesToDelete.Count();
        
    TBuf8<300> entryUid;
   	CCalEntry* entry = NULL;
  	CDesC8ArrayFlat *entries = new CDesC8ArrayFlat(aEntriesToDelete.Count());
         
    TBool fetch;       
    for(TInt j = 0; j < count ; ++j)
    	{
        fetch = EFalse;
        entry = aEntriesToDelete.operator[](j);
        entryUid = entry->UidL();
        
        for(TInt i=0;i<entries->Count();i++)
        	{   
           if(!((entries->MdcaPoint(i)).Compare(entryUid)))
        		{
        		fetch = ETrue;
        		}
        	}
        if(!fetch)
        	{
        	if(CalendarViewStatus()== ECalViewCreateCompleted && NULL!=iEntryView)
				{   
	            iEntryView->DeleteL(*aEntriesToDelete[j]);  
				}
			else
				{
				// leaving if Calender Entry view is not available.
				User::Leave(KErrNotFound);
				}
        	entries->AppendL(entryUid);       
        	}
   		}
   	          
    delete entries;
        
    // Make sure that entries are indeed deleted from DB
  //  FetchFromDbL(iEntriesFromDb1, iEntriesFromDb2);
  //  if(iEntriesFromDb2.Count())
  //      {
  //      User::Leave(KErrAbort);
  //      }
        
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::WriteToDbL
// Write to database
// -----------------------------------------------------------------------------
//    
void CCAgnVersit2Test::WriteToDbL(const RPointerArray<CCalEntry> &aEntryArray )
    {
    TInt countIn = 0;    
    if(CalendarViewStatus()== ECalViewCreateCompleted && NULL!=iEntryView)
		{   
	    iEntryView->StoreL(aEntryArray, countIn);
		}
    else
		{
		// leaving if Calender Entry view is not available.
		User::Leave(KErrNotFound);
		}
    
    if(countIn != aEntryArray.Count())
        {
        User::Leave(KErrAbort);
        }
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::FetchFromDbL
// Fetch entry from data base
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::FetchFromDbL(RPointerArray<CCalEntry>& aEntriesToFetch, RPointerArray<CCalEntry>& aFethedEntries)
	{
    TUint count = aEntriesToFetch.Count();
    TBuf8<300> entryUid;   	
   	CCalEntry* entry = NULL;
  	CDesC8ArrayFlat *entries = new CDesC8ArrayFlat(aEntriesToFetch.Count());
         
    TInt i=0; 
    TBool fetch;       
    for(i = 0; i < count ; ++i)
    {
        fetch = EFalse;
        entry = aEntriesToFetch.operator[](i);
        entryUid = entry->UidL();
        
        for(TInt i=0;i<entries->Count();i++)
        {   
        	if(!((entries->MdcaPoint(i)).Compare(entryUid)))
        	{
        		fetch = ETrue;
        	}
        }
        if(!fetch)
        {
        	entries->AppendL(entry->UidL());
        	iEntryView->FetchL(entryUid, aFethedEntries);  
        	       
        }
   	}
   	delete entries;
	}

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventL
// -----------------------------------------------------------------------------
//    
void CCAgnVersit2Test::SVEventL()
    {
    	GetFilesL(KAgnVersit2SVEvent);
   		doTestStepL();    
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventAttendeeL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventAttendeeL()
    {
    GetFilesL(KAgnVersit2SVEventAttendee);

	doTestStepL();    
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventCategoriesL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventCategoriesL()
    {
	GetFilesL(KAgnVersit2SVEventCategories);
	
	doTestStepL();    
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventDescriptionL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventDescriptionL()
    {
	GetFilesL(KAgnVersit2SVEventDescription);
	
	doTestStepL();     
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventDurationL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventDurationL()
    {
	GetFilesL(KAgnVersit2SVEventDuration);
	
	doTestStepL();         
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventExDateL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventExDateL()
    {
	GetFilesL(KAgnVersit2SVEventExDate);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventLastModL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventLastModL()
    {
	GetFilesL(KAgnVersit2SVEventLastMod);
	
	doTestStepL();    
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventLocationL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventLocationL()
    {
	GetFilesL(KAgnVersit2SVEventLocation);
	
	doTestStepL();    
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventOrganizerL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventOrganizerL()
    {
	GetFilesL(KAgnVersit2SVEventOrganizer);
	
	doTestStepL();    
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventRDateL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventRDateL()
    {
	GetFilesL(KAgnVersit2SVEventRDate);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventRecurIdL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventRecurIdL()
    {
	GetFilesL(KAgnVersit2SVEventRecurId);
	
	doTestStepL();    
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventRRuleL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventRRuleL()
    {
	GetFilesL(KAgnVersit2SVEventRRule);
	
	doTestStepL();     
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventSequenceL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventSequenceL()
    {
	GetFilesL(KAgnVersit2SVEventSequence);
	
	doTestStepL(); 
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventStatusL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventStatusL()
    {
	GetFilesL(KAgnVersit2SVEventStatus);
	
	doTestStepL(); 
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventSummaryL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventSummaryL()
    {
	GetFilesL(KAgnVersit2SVEventSummary);
	
	doTestStepL();     
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SVEventRRuleTzL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SVEventRRuleTzL()
    {
	GetFilesL(KAgnVersit2SVEventRRuleTz);
	
	doTestStepL();     
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::OutlookAllDayRepeatWeeklyL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::OutlookAllDayRepeatWeeklyL()
    {
	GetFilesL(KAgnVersit2OutlookAllDayRepeatWeekly);
	
	doTestStepL();    
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::NotSupportedContinueL
// -----------------------------------------------------------------------------
// 
void CCAgnVersit2Test::NotSupportedContinueL()
    {
    GetFilesL(KAgnVersit2NotSupportedContinue);
	
	doTestStepL(); 
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::NotSupportedSkipL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::NotSupportedSkipL()
    {
    GetFilesL(KAgnVersit2NotSupportedSkip);
	
	doTestStepL(); 
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::NotSupportedStopL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::NotSupportedStopL()
    {
    GetFilesL(KAgnVersit2NotSupportedStop);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::NotSupportedLeaveL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::NotSupportedLeaveL()
    {
    GetFilesL(KAgnVersit2NotSupportedLeave);
	
    EUNIT_ASSERT_SPECIFIC_LEAVE( ImportL(), KErrAbort );
    
    // FIX ME
    //ExportL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::RRuleNumberingWeeklyL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::RRuleNumberingWeeklyL()
    {
    GetFilesL(KAgnVersit2RRuleNumberingWeekly);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::RRuleNumberingMonthlyL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::RRuleNumberingMonthlyL()
    {
    GetFilesL(KAgnVersit2RRuleNumberingMonthly);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::RRuleNumberingYearlyL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::RRuleNumberingYearlyL()
    {
    GetFilesL(KAgnVersit2RRuleNumberingYearly);
	
	doTestStepL();
    }

// iTip Method Tests
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VEventPublishL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VEventPublishL()
    {
    GetFilesL(KAgnVersit2VEventPublish);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VEventRequestL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VEventRequestL()
    {
    GetFilesL(KAgnVersit2VEventRequest);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VEventReplyL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VEventReplyL()
    {
    GetFilesL(KAgnVersit2VEventReply);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VEventAddL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VEventAddL()
    {
    GetFilesL(KAgnVersit2VEventAdd);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VEventCancelL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VEventCancelL()
    {
    GetFilesL(KAgnVersit2VEventCancel);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VEventRefreshL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VEventRefreshL()
    {
    GetFilesL(KAgnVersit2VEventRefresh);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VEventCounterL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VEventCounterL()
    {
    GetFilesL(KAgnVersit2VEventCounter);
	
	doTestStepL();
    }
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VEventDeclineCounterL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VEventDeclineCounterL()
    {
    GetFilesL(KAgnVersit2VEventDeclineCounter);
	
	doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VTodoPublishL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VTodoPublishL()
    {
    GetFilesL(KAgnVersit2VTodoPublish);
    
    doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VTodoRequestL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VTodoRequestL()
    {
    GetFilesL(KAgnVersit2VTodoRequest);
    
    doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VTodoReplyL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VTodoReplyL()
    {
    GetFilesL(KAgnVersit2VTodoReply);
    
    doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VTodoAddL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VTodoAddL()
    {
    GetFilesL(KAgnVersit2VTodoAdd);
    
    doTestStepL();
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VTodoCancelL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VTodoCancelL()
    {
    GetFilesL(KAgnVersit2VTodoCancel);
    
    doTestStepL();
    }
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VTodoRefreshL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VTodoRefreshL()
    {
    GetFilesL(KAgnVersit2VTodoRefresh);
    
    doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VTodoCounterL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VTodoCounterL()
    {
    GetFilesL(KAgnVersit2VTodoCounter);
    
    doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VTodoDeclineCounterL
// -----------------------------------------------------------------------------
//    
void CCAgnVersit2Test::VTodoDeclineCounterL()
    {
    GetFilesL(KAgnVersit2VTodoDeclineCounter);
    
    doTestStepL();
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::NotAnIcalL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::NotAnIcalL()
    {
    GetFilesL(KAgnVersit2NotAnIcal);
    EUNIT_ASSERT_SPECIFIC_LEAVE( ImportL(), KErrCorrupt);
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::RRuleTzAllDaysFromIntL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::RRuleTzAllDaysFromIntL()
    {
    GetFilesL(KAgnVersit2RRuleTzAllDaysFromInt);
	
    doTestStepL();
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::AlternativeExportL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::AlternativeExportL()
    {
/*    
    GetFilesL(KAgnVersit2SVEvent);

    // 1) Read from iCal
    // 2) Export the iCal to file A via streaming
    // 3) Import the iCal from A via streaming
    // 4) Write the iCal to dB
    // 5) Fetch the entries to iEntriesFromDb
    // 6) Export iEntriesFromDb
    // 7) Compare with the expected result
    
    // 1) Import entries from file KAgnVersit2SVEvent  
    ImportL(); // sets iEntriesToDb
    
	RFileWriteStream writeStream;
	EUNIT_ASSERT_EQUALS(writeStream.Replace(iFs, *iExportFile1, EFileWrite), KErrNone);
	CleanupClosePushL(writeStream);
	
	//  2) Export the entries to iExportFile1 
	TInt flags = EExportAll;		
	iInterface->ExportL(iEntriesToDb, writeStream, EExportAll, *this);	
	CleanupStack::PopAndDestroy(); // Close writeStream. 
    
    // we need to read iExportFile1 again
    // to test ExportL/ImportL
    iEntriesToDb.ResetAndDestroy(); 
    
    //  3) Read back from iExportFile1
	RFileReadStream readStream;
	EUNIT_ASSERT_EQUALS(readStream.Open(iFs, *iExportFile1, EFileRead), KErrNone);
	CleanupClosePushL(readStream); 
    iInterface->ImportL(iEntriesToDb, readStream, EExportAll, *this);
	CleanupStack::PopAndDestroy(); // Close readStream
		
    // 4) Write the iCal to dB
    WriteToDbL(); // iEntriesToDb is written to Db
    
    // 5) Fetch the entries to iEntriesFromDb
    FetchFromDbL(); // iEntriesFromDb is set
    
    // 6) Export iEntriesFromDb
    ExportL();
    
    // 7) Compare with the expected result
    EUNIT_ASSERT( CompareFilesL() );
*/    
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::FloatingTimeL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::FloatingTimeL()
    {
    GetFilesL(KAgnVersit2FloatingTime);
  
    doTestStepL();
    }
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::AddAnniversaryL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::AddAnniversaryL()
    {
    GetFilesL(KAgnVersit2AddAnniversary);
	
    doTestStepL();
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::AllCategoriesL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::AllCategoriesL()
    {
    GetFilesL(KAgnVersit2AllCategories);
    
    doTestStepL();
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::AttendeeRoleStatusL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::AttendeeRoleStatusL()
    {
    GetFilesL(KAgnVersit2AttendeeRoleStatus);
    
    doTestStepL();
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VTodoStatusL
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::VTodoStatusL()
    {
	    GetFilesL(KAgnVersit2VTodoStatus);    
	    doTestStepL();
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::VEventStatusL
// -----------------------------------------------------------------------------
//   
void CCAgnVersit2Test::VEventStatusL()
    {
    GetFilesL(KAgnVersit2VEventStatus);
    
    doTestStepL();
  //  TBool bRet = CleanDatabaseL();
  
    // Set the original time zone
    SetOriginalTimeZoneL();
    }

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::MyOwnTestL
// Use this if you need to test your own vCalendar import/export
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::MyOwnTestL()
    {
/*
	iImportFile->Des().Insert(0, *iInputDir);
	iImportFile->Des().Append(_L("myownIn.ics"));

	iExportFile->Des().Insert(0, *iOutputDir);
	iExportFile->Des().Append(_L("myownOut.ics"));
	
    doTestStepL();
*/
    }
    
// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SetInitialTimeZoneL
// Sets the time Zone to London TimeZone
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SetInitialTimeZoneL()
	{
	
	// Establish connection with RTz to get the timezone ID
	RTz tz;
	User::LeaveIfError( tz.Connect() );
	CleanupClosePushL( tz );
	
	// get the current time zone id 
	CTzId* tzID = tz.GetTimeZoneIdL();
	CleanupStack::PushL( tzID );
	
	// read current time zone numeric id
	iCurrentZone = tzID->TimeZoneNumericID();
	
	// Sets the current time zone to London Time Zone
	if(KLondonTimeZoneId != iCurrentZone)
		{
		CTzLocalizer* tzLocalizer = CTzLocalizer::NewL();
		CleanupStack::PushL(tzLocalizer);
				
		tzLocalizer->SetTimeZoneL(KLondonTimeZoneId);
		
		CleanupStack::PopAndDestroy(tzLocalizer);
		}
	
	CleanupStack::PopAndDestroy( tzID );
	CleanupStack::PopAndDestroy( &tz );
	
	}

// -----------------------------------------------------------------------------
// CCAgnVersit2Test::SetOriginalTimeZoneL
// Sets the original time Zone
// -----------------------------------------------------------------------------
//
void CCAgnVersit2Test::SetOriginalTimeZoneL()	
	{
	// if current time zone is not London, set the original
	// time zone
	if(iCurrentZone != KLondonTimeZoneId)
		{
		CTzLocalizer* tzLocalizer = CTzLocalizer::NewL();
		CleanupStack::PushL(tzLocalizer);
		
		tzLocalizer->SetTimeZoneL(iCurrentZone);
	
		CleanupStack::PopAndDestroy(tzLocalizer);
		}
			
	}
	
// ----------------------------------------------------------------------------
// Sets the Calendar view creation status.
//  ----------------------------------------------------------------------------

void CCAgnVersit2Test::SetCalendarViewStatus(TInt aStatus)
	{
	
	iCalEntryViewStatus = aStatus ;	

	}

// ----------------------------------------------------------------------------
//Gets the Calendar view creation status.
// ----------------------------------------------------------------------------
TInt CCAgnVersit2Test::CalendarViewStatus()
	{
	return iCalEntryViewStatus;
	}


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    CCAgnVersit2Test,
    "AgnVersit2 unit tests",
    "UNIT" )

EUNIT_TEST(
    "SVEvent",
    "AgnVersit2",
    "SVEventL",
    "FUNCTIONALITY",
    SetupL, SVEventL, Teardown)
    
EUNIT_TEST(
    "SVEventAttendee",
    "AgnVersit2",
    "SVEventAttendeeL",
    "FUNCTIONALITY",
    SetupL, SVEventAttendeeL, Teardown)

EUNIT_TEST(
    "SVEventCategories",
    "AgnVersit2",
    "SVEventCategoriesL",
    "FUNCTIONALITY",
    SetupL, SVEventCategoriesL, Teardown)

EUNIT_TEST(
    "SVEventDescription",
    "AgnVersit2",
    "SVEventDescriptionL",
    "FUNCTIONALITY",
    SetupL, SVEventDescriptionL, Teardown)

EUNIT_TEST(
    "SVEventDuration",
    "AgnVersit2",
    "SVEventDurationL",
    "FUNCTIONALITY",
    SetupL, SVEventDurationL, Teardown)

EUNIT_TEST(
    "SVEventExDate",
    "AgnVersit2",
    "SVEventExDateL",
    "FUNCTIONALITY",
    SetupL, SVEventExDateL, Teardown)
     
EUNIT_TEST(
    "SVEventLastMod",
    "AgnVersit2",
    "SVEventLastModL",
    "FUNCTIONALITY",
    SetupL, SVEventLastModL, Teardown)

EUNIT_TEST(
    "SVEventLocation",
    "AgnVersit2",
    "SVEventLocationL",
    "FUNCTIONALITY",
    SetupL, SVEventLocationL, Teardown)

EUNIT_TEST(
    "SVEventOrganizer",
    "AgnVersit2",
    "SVEventOrganizerL",
    "FUNCTIONALITY",
    SetupL, SVEventOrganizerL, Teardown)

EUNIT_TEST(
    "SVEventRDate",
    "AgnVersit2",
    "SVEventRDateL",
    "FUNCTIONALITY",
    SetupL, SVEventRDateL, Teardown)
    
EUNIT_TEST(
    "SVEventRecurId",
    "AgnVersit2",
    "SVEventRecurIdL",
    "FUNCTIONALITY",
    SetupL, SVEventRecurIdL, Teardown)

EUNIT_TEST(
    "SVEventRRule",
    "AgnVersit2",
    "SVEventRRuleL",
    "FUNCTIONALITY",
    SetupL, SVEventRRuleL, Teardown)

EUNIT_TEST(
    "SVEventSequence",
    "AgnVersit2",
    "SVEventSequenceL",
    "FUNCTIONALITY",
    SetupL, SVEventSequenceL, Teardown)

EUNIT_TEST(
    "SVEventStatus",
    "AgnVersit2",
    "SVEventStatusL",
    "FUNCTIONALITY",
    SetupL, SVEventStatusL, Teardown)

EUNIT_TEST(
    "SVEventSummary",
    "AgnVersit2",
    "SVEventSummaryL",
    "FUNCTIONALITY",
    SetupL, SVEventSummaryL, Teardown)

EUNIT_TEST(
    "SVEventRRuleTz",
    "AgnVersit2",
    "SVEventRRuleTzL",
    "FUNCTIONALITY",
    SetupL, SVEventRRuleTzL, Teardown)

EUNIT_TEST(
    "OutlookAllDayRepeatWeekly",
    "AgnVersit2",
    "OutlookAllDayRepeatWeeklyL",
    "FUNCTIONALITY",
    SetupL, OutlookAllDayRepeatWeeklyL, Teardown)

EUNIT_TEST(
    "NotSupportedContinue",
    "AgnVersit2",
    "NotSupportedContinueL",
    "FUNCTIONALITY",
    SetupL, NotSupportedContinueL, Teardown)

EUNIT_TEST(
    "NotSupportedSkip",
    "AgnVersit2",
    "NotSupportedSkipL",
    "FUNCTIONALITY",
    SetupL, NotSupportedSkipL, Teardown)

EUNIT_TEST(
    "NotSupportedStop",
    "AgnVersit2",
    "NotSupportedStopL",
    "FUNCTIONALITY",
    SetupL, NotSupportedStopL, Teardown)

EUNIT_TEST(
    "NotSupportedLeave",
    "AgnVersit2",
    "NotSupportedLeaveL",
    "FUNCTIONALITY",
    SetupL, NotSupportedLeaveL, Teardown)

EUNIT_TEST(
    "RRuleNumberingWeekly",
    "AgnVersit2",
    "RRuleNumberingWeeklyL",
    "FUNCTIONALITY",
    SetupL, RRuleNumberingWeeklyL, Teardown)

EUNIT_TEST(
    "RRuleNumberingMonthly",
    "AgnVersit2",
    "RRuleNumberingMonthlyL",
    "FUNCTIONALITY",
    SetupL, RRuleNumberingMonthlyL, Teardown)

EUNIT_TEST(
    "RRuleNumberingYearly",
    "AgnVersit2",
    "RRuleNumberingYearlyL",
    "FUNCTIONALITY",
    SetupL, RRuleNumberingYearlyL, Teardown)

EUNIT_TEST(
    "VEventPublish",
    "AgnVersit2",
    "VEventPublishL",
    "FUNCTIONALITY",
    SetupL, VEventPublishL, Teardown)

EUNIT_TEST(
    "VEventRequest",
    "AgnVersit2",
    "VEventRequestL",
    "FUNCTIONALITY",
    SetupL, VEventRequestL, Teardown)

EUNIT_TEST(
    "VEventReply",
    "AgnVersit2",
    "VEventReplyL",
    "FUNCTIONALITY",
    SetupL, VEventReplyL, Teardown)

EUNIT_TEST(
    "VEventAdd",
    "AgnVersit2",
    "VEventAddL",
    "FUNCTIONALITY",
    SetupL, VEventAddL, Teardown)

EUNIT_TEST(
    "VEventCancel",
    "AgnVersit2",
    "VEventCancelL",
    "FUNCTIONALITY",
    SetupL, VEventCancelL, Teardown)

EUNIT_TEST(
    "VEventRefresh",
    "AgnVersit2",
    "VEventRefreshL",
    "FUNCTIONALITY",
    SetupL, VEventRefreshL, Teardown)

EUNIT_TEST(
    "VEventCounter",
    "AgnVersit2",
    "VEventCounterL",
    "FUNCTIONALITY",
    SetupL, VEventCounterL, Teardown)

EUNIT_TEST(
    "VEventDeclineCounter",
    "AgnVersit2",
    "VEventDeclineCounterL",
    "FUNCTIONALITY",
    SetupL, VEventDeclineCounterL, Teardown)

EUNIT_TEST(
    "VTodoPublish",
    "AgnVersit2",
    "VTodoPublishL",
    "FUNCTIONALITY",
    SetupL, VTodoPublishL, Teardown)
    
EUNIT_TEST(
    "VTodoRequest",
    "AgnVersit2",
    "VTodoRequestL",
    "FUNCTIONALITY",
    SetupL, VTodoRequestL, Teardown) 

EUNIT_TEST(
    "VTodoReply",
    "AgnVersit2",
    "VTodoReplyL",
    "FUNCTIONALITY",
    SetupL, VTodoReplyL, Teardown) 

EUNIT_TEST(
    "VTodoAdd",
    "AgnVersit2",
    "VTodoAddL",
    "FUNCTIONALITY",
    SetupL, VTodoAddL, Teardown) 

EUNIT_TEST(
    "VTodoCancel",
    "AgnVersit2",
    "VTodoCancelL",
    "FUNCTIONALITY",
    SetupL, VTodoCancelL, Teardown) 
    
EUNIT_TEST(
    "VTodoRefresh",
    "AgnVersit2",
    "VTodoRefreshL",
    "FUNCTIONALITY",
    SetupL, VTodoRefreshL, Teardown) 
    
EUNIT_TEST(
    "VTodoCounter",
    "AgnVersit2",
    "VTodoCounterL",
    "FUNCTIONALITY",
    SetupL, VTodoCounterL, Teardown) 
    
EUNIT_TEST(
    "VTodoDeclineCounter",
    "AgnVersit2",
    "VTodoDeclineCounterL",
    "FUNCTIONALITY",
    SetupL, VTodoDeclineCounterL, Teardown) 

EUNIT_TEST(
    "NotAnIcal",
    "AgnVersit2",
    "NotAnIcalL",
    "FUNCTIONALITY",
    SetupL, NotAnIcalL, Teardown)

EUNIT_TEST(
    "RRuleTzAllDaysFromInt",
    "AgnVersit2",
    "RRuleTzAllDaysFromIntL",
    "FUNCTIONALITY",
    SetupL, RRuleTzAllDaysFromIntL, Teardown)

EUNIT_TEST(
    "AlternativeExport",
    "AgnVersit2",
    "AlternativeExportL",
    "FUNCTIONALITY",
    SetupL, AlternativeExportL, Teardown)

EUNIT_TEST(
    "FloatingTime",
    "AgnVersit2",
    "FloatingTimeL",
    "FUNCTIONALITY",
    SetupL, FloatingTimeL, Teardown)

EUNIT_TEST(
    "AddAnniversary",
    "AgnVersit2",
    "AddAnniversaryL",
    "FUNCTIONALITY",
    SetupL, AddAnniversaryL, Teardown)
    
EUNIT_TEST(
    "AllCategories",
    "AgnVersit2",
    "AllCategoriesL",
    "FUNCTIONALITY",
    SetupL, AllCategoriesL, Teardown)

EUNIT_TEST(
    "AttendeeRoleStatus",
    "AgnVersit2",
    "AttendeeRoleStatusL",
    "FUNCTIONALITY",
    SetupL, AttendeeRoleStatusL, Teardown)  
    
EUNIT_TEST(
    "VTodoStatus",
    "AgnVersit2",
    "VTodoStatusL",
    "FUNCTIONALITY",
    SetupL, VTodoStatusL, Teardown)

EUNIT_TEST(
    "VEventStatus",
    "AgnVersit2",
    "VEventStatusL",
    "FUNCTIONALITY",
    SetupL, VEventStatusL, Teardown)    
    
EUNIT_END_TEST_TABLE

//  END OF FILE
