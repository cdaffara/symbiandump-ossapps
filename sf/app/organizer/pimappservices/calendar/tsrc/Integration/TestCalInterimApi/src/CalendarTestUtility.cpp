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
 
#include "CalendarTestUtility.h"

_LIT(KServerUid, "*[10003a5b]*" );//agendar server uid
_LIT(KDiskSize, "size: %LD, free space: %LD");
_LIT( KFileName, "c:\\EmptyDiskFiller.File" );

// how much empty space will be made available on disk, in bytes
const static TInt KDiskSpaceToLeaveFree = 5000; 

CCalendarTestUtility::CCalendarTestUtility(CTestStep& aStep) :
                        iTestStep( aStep )
    {
    //initilizes teststep reference only
    }

CCalendarTestUtility::~CCalendarTestUtility()
    {
	iFile.Close();
    }

CCalendarTestUtility* CCalendarTestUtility::NewLC(CTestStep& aTestStep)
    {
    CCalendarTestUtility* utilityObject = new (ELeave) CCalendarTestUtility( aTestStep );
    CleanupStack::PushL( utilityObject );
    User::LeaveIfError( utilityObject->iFsSession.Connect() );
    return utilityObject;
    }

//if result if value is false, fails test step, and prints message
TBool CCalendarTestUtility::PrintBooleanTest(
                	            const TBool aResult, 
                	            const TText* aCompare, 
                	            const TInt aIteration, 
                	            const TInt aSubIteration, 
                	            const TText* aFile, 
                	            const TInt aLine
                	            )
    {
	if( !aResult )
		{
		if( aIteration >= 0 && aSubIteration >= 0 )
		    {
		    //if iteration values, print them
		    _LIT(KComparePrint, "The comparison: %s failed on subiteration %d of iteration %d, at line: %d in file: %s");
		    iTestStep.ERR_PRINTF6( KComparePrint, aCompare, aSubIteration, aIteration, aLine, aFile );
		    }
		else if( aIteration >= 0 )
		    {
		    //if only one level of iteration, print that
		    _LIT(KComparePrint, "The comparison: %s failed on iteration: %d, at line: %d in file: %s");
		    iTestStep.ERR_PRINTF5( KComparePrint, aCompare, aIteration, aLine, aFile );
		    }
		else
		    {
		    //just print comparison if no iteration value
		    _LIT(KComparePrint, "The comparison: %s failed at line: %d in file: %s");
		    iTestStep.ERR_PRINTF4( KComparePrint, aCompare, aLine, aFile );
		    }
		iTestStep.TEST1( EFalse , ETrue );
		}
	return aResult;
    }

//if integers comparison fails, print value of integers and comparison and fail test
TBool CCalendarTestUtility::PrintComparison(
                	            const TInt aCompareL, 
                	            const TText* aCompareOperator, 
                	            const TInt aCompareR, 
                	            const TBool aResult, 
                	            const TInt aIteration, 
                	            const TInt aSubIteration, 
                	            const TText* aFile, 
                	            const TInt aLine
                	            )
    {
	_LIT(KComparePrint, "%d %s %d: failed, in file: %s at line: %d");
	if( !aResult )
		{
		iTestStep.ERR_PRINTF6( KComparePrint, aCompareL, aCompareOperator, aCompareR, aFile, aLine);
		if( aIteration >= 0 && aSubIteration >= 0 )
		    {
		    //if 2 iterations, print both level of iteration
	        _LIT(KIterationPrint, "On subiteration %d of iteration %d");
		    iTestStep.ERR_PRINTF3( KIterationPrint,  aSubIteration, aIteration);
		    }
		else if( aIteration >= 0 )
		    {
		    //if single iteration, print iteration value
	        _LIT(KIterationPrint, "On iteration %d");
		    iTestStep.ERR_PRINTF2( KIterationPrint, aIteration);
		    }
		iTestStep.TEST1( EFalse , ETrue );
		}
	return aResult;
    }

//if string comparison fails, print value of strings and comparison and fail test
TBool CCalendarTestUtility::PrintComparison(
                	            const TDesC& aCompareL, 
                	            const TText* aCompareOperator, 
                	            const TDesC& aCompareR, 
                	            const TBool aResult, 
                	            const TInt aIteration, 
                	            const TInt aSubIteration, 
                	            const TText* aFile, 
                	            const TInt aLine
                	            )
    {
	_LIT(KComparePrint, "%S %s %S: failed, in file: %s at line: %d");
	if( !aResult )
		{
		iTestStep.ERR_PRINTF6( KComparePrint, &aCompareL, aCompareOperator, &aCompareR, aFile, aLine);
		if( aIteration >= 0 && aSubIteration >= 0 )
		    {
		    //if 2 iterations, print both level of iteration
	        _LIT(KIterationPrint, "On subiteration %d of iteration %d");
		    iTestStep.ERR_PRINTF3( KIterationPrint,  aSubIteration, aIteration);
		    }
		else if( aIteration >= 0 )
		    {
		    //if single iteration, print iteration value
	        _LIT(KIterationPrint, "On iteration %d");
		    iTestStep.ERR_PRINTF2( KIterationPrint, aIteration);
		    }
		iTestStep.TEST1( EFalse , ETrue );
		}
	return aResult;
    }

//convert 8bit strings to 16 bit strings and perform string comparison
TBool CCalendarTestUtility::PrintComparison(
                	            const TDesC8& aCompareL, 
                	            const TText* aCompareOperator, 
                	            const TDesC8& aCompareR, 
                	            const TBool aResult, 
                	            const TInt aIteration, 
                	            const TInt aSubIteration, 
                	            const TText* aFile, 
                	            const TInt aLine
                	            )
	{
	TBool result = EFalse;
	HBufC16* bufLeft = NULL;
	HBufC16* bufRight = NULL;
	TRAPD( error,
		bufLeft = HBufC16::NewL( aCompareL.Length() );
		bufRight = HBufC16::NewL( aCompareR.Length() );
		);
		
	if( error != KErrNone )
		{
        _LIT(KAllocError, "Buffer allocation error: %d");
	    iTestStep.ERR_PRINTF2( KAllocError, error);
		delete bufLeft;
		delete bufRight;
		iTestStep.TEST1( EFalse, ETrue );
		return EFalse;
		}		
						
	CleanupStack::PushL(bufLeft);
	CleanupStack::PushL(bufRight);
		
	bufLeft->Des().Copy( aCompareL );
	bufRight->Des().Copy( aCompareR );
	result = PrintComparison(bufLeft->Des(), aCompareOperator, bufRight->Des(), 
									aResult, aIteration, aSubIteration, aFile, aLine);
	CleanupStack::PopAndDestroy(bufRight);						
	CleanupStack::PopAndDestroy(bufLeft);
	return result;
	}

//logs on to process handle and waits for it to close/terminate	
void CCalendarTestUtility::WaitForProcessToClose(
		const TDesC& aProcessName, 
		const TInt aIteration,
		const TBool aVerbose)
    {
	_LIT(KillString, "Kill");
	if ( aProcessName != KNullDesC )
		{
		if( aVerbose )
			{
			_LIT(KServerRunning,"*** The Calendar process is running ***");
			_LIT(KServerIteration, "Found server process on iteration: %d");
			iTestStep.INFO_PRINTF1(KServerRunning);
			iTestStep.INFO_PRINTF2(KServerIteration, aIteration);
			}
			
		// the CntSrv process is running so wait 
		RProcess process;
		const TInt OpenError = process.Open(aProcessName);
		if ( OpenError == KErrNone)
			{
			TRequestStatus status;
			process.Logon(status); // ask for a callback when the process ends
			User::WaitForRequest(status);
			const TInt serverError = process.ExitReason();
			TExitCategoryName category = process.ExitCategory();
			if( aVerbose )
				{
				_LIT(KServerClosed,"*** The CntSrv process has ended with error type: %S, and error value: %d ***");
				iTestStep.INFO_PRINTF3(KServerClosed, &category, serverError);
				}
				
			process.Close();
			
			if( ( category != KillString ) || ( serverError != KErrNone ) )
				{
				_LIT(KServerError,"*** The server caused an error while closing ***");
				iTestStep.ERR_PRINTF1(KServerError);
				iTestStep.TEST1( EFalse, ETrue);
				}
			else if( aVerbose )
			    {		    
    			_LIT(KCloseSuccess,"Calendar Server closed successfully, after being inactive");
    			iTestStep.INFO_PRINTF1( KCloseSuccess );
			    }
			}
		else
			{
			_LIT(KProcessFailure,"*** Failed to open process, error: %d ***");
			iTestStep.ERR_PRINTF2(KProcessFailure, OpenError);
			iTestStep.TEST1(EFalse, ETrue);
			}
		}
	else
		{
		_LIT(KNoProcess,"*** The CntSrv process is NOT running ***");
		iTestStep.ERR_PRINTF1(KNoProcess);
		iTestStep.TEST1(EFalse, ETrue);
		}
    }

//opens process handles and panics the process
void CCalendarTestUtility::CloseProcess( 
		const TDesC& aProcessName, 
		const TInt aIteration,
		const TBool aVerbose)
    {
	if ( aProcessName != KNullDesC )
		{
		if( aVerbose )
			{
			_LIT(KServerIteration, "Found server process on iteration: %d");
			iTestStep.INFO_PRINTF2(KServerIteration, aIteration);
			}
		
		// the CntSrv process is running so close it 
		RProcess process;
		const TInt OpenError = process.Open(aProcessName);
		if ( OpenError == KErrNone)
			{
			process.Kill(KErrNone);
			process.Close();
			if( aVerbose )
				{
				_LIT(KCloseSuccess,"Manually closed calendar server successfully");
				iTestStep.INFO_PRINTF1(KCloseSuccess);
				}
			}
		else
			{
			_LIT(KProcessFailure,"*** Failed to open process, error: %d ***");
			iTestStep.ERR_PRINTF2(KProcessFailure, OpenError);
			iTestStep.TEST1(EFalse, ETrue);
			}
		}
	else
		{
		_LIT(KNoProcess,"*** The CntSrv process is NOT running ***");
		iTestStep.ERR_PRINTF1(KNoProcess);
		iTestStep.TEST1(EFalse, ETrue);
		}
    }
 
//finds all open handles to the calendar server and either waits for them to terminate
//or forcefully closes them    
void CCalendarTestUtility::FindAndCloseCalendarServer(
			const TServerCloseType aCloseType,
			const TBool aVerbose)
    {
	// The name of the CntSrv process includes its uid like this CNTSRV.EXE[10003a73]001
	TInt findProcessResult = KErrNone;
	TFindProcess findProcess( KServerUid );
	TFullName  fullNameLastProcess;
	TFullName  fullNameNextProcess;
	
	findProcessResult = findProcess.Next(fullNameNextProcess);

	
	for ( TInt iteration = 1; findProcessResult == KErrNone; ++iteration )
		{
		fullNameLastProcess = fullNameNextProcess;
		RProcess process;
		const TInt OpenError = process.Open(fullNameLastProcess);
		if ( OpenError == KErrNone)
			{
			TExitCategoryName category = process.ExitCategory();
			//if this handle to server is still open, then wait for this handle to close
			if( 0 == category.Length() )
			    {
			    switch( aCloseType )
			        {
			        case EWaitForServerToClose:
			            {
			            //wait for calendar server to clsoe
			            WaitForProcessToClose( fullNameLastProcess, iteration, aVerbose );
			            break;
			            }
			        case EForceCloseServer:
			            {
			            //force close server
			            CloseProcess( fullNameLastProcess, iteration, aVerbose );
			            break;
			            }
			        default:
			            {
                		_LIT(KNoProcess,"Invalid Server Close Type");
                		iTestStep.ERR_PRINTF1(KNoProcess);
                		iTestStep.TEST1(EFalse, ETrue);
                		break;
			            }
			        }
			    }
			}
		findProcessResult = findProcess.Next(fullNameNextProcess);
		}
    }

//reads string from config section and converts to time object, stored in atime.
TBool CCalendarTestUtility::GetTimeFromConfig( const TDesC& aSection, const TDesC& aKey, TTime& aTime )
    {
    TPtrC timeString;
    iTestStep.GetStringFromConfig( aSection, aKey, timeString);
    TInt error = aTime.Set( timeString );
    if(error != KErrNone)
        {
        aTime = Time::NullTTime().Int64();
        return EFalse;
        }
    return ETrue;
    }

//gets string from config section and converts to tz object    
CTzId* CCalendarTestUtility::GetTZFromConfigL( const TDesC& aSection, const TDesC& aKey )
    {
    TPtrC tzString;
    iTestStep.GetStringFromConfig( aSection, aKey, tzString);
    
    if( tzString != KNullDesC )
        {  
        //convert to  8bit string
        HBufC8* tzBuf8 = HBufC8::NewLC( tzString.Length() + 1 );
        TPtr8 tzPtr8 = tzBuf8->Des();
        tzPtr8.Copy( tzString );
        
        //create tzid object based on 8bit string
        CTzId* configTZ = CTzId::NewL( tzPtr8 );
        CleanupStack::PopAndDestroy( tzBuf8 );
        
        return configTZ;
        }
        
    return NULL;
    }

//end time for range or entry or time range may be null
//in which case don't compare null values
TBool CCalendarTestUtility::EntryWithinTimeRange(
        const CCalEntry& aEntry,
        const CalCommon::TCalTimeRange& aTimeRange
        )
	{
    const TBool KEntryEndNullTime =
				(  aEntry.EndTimeL().TimeLocalL() == Time::NullTTime() );
    const TBool KRangeEndNullTime =
    		 	( aTimeRange.EndTime().TimeLocalL() == Time::NullTTime() );
	//entry start date should be >= range start date
	//entry start date should be <= range end date
	//entry end date should be >= range start date
	//entry end date should be <= range end date    		 	
	const TBool KEntryStartLessThanRangeStart = 
				( aEntry.StartTimeL().TimeLocalL()  >= aTimeRange.StartTime().TimeLocalL() )
    		&& 	( (aEntry.StartTimeL().TimeLocalL() <= aTimeRange.EndTime().TimeLocalL())|| KRangeEndNullTime )
    		&& 	( (aEntry.EndTimeL().TimeLocalL() >= aTimeRange.StartTime().TimeLocalL())|| KEntryEndNullTime)
    		&& 	( (aEntry.EndTimeL().TimeLocalL() <= aTimeRange.EndTime().TimeLocalL())	 || KRangeEndNullTime || KEntryEndNullTime );
    return KEntryStartLessThanRangeStart;
	}

//deletes entry from calendar database
void CCalendarTestUtility::DeleteEntryL( 
		CCalEntryView& aEntryView, 
		const CCalEntry& aEntry )
	{
	CDesC8ArrayFlat* array = new(ELeave) CDesC8ArrayFlat( 1 );
	CleanupStack::PushL( array );
	array->AppendL( aEntry.UidL() );
	aEntryView.DeleteL( *array );
	
	CleanupStack::PopAndDestroy( array );
	}

//compares two entries, if entries contain the same uids, start time and end time, returns true
TBool CCalendarTestUtility::CompareEntriesL(
		const CCalEntry& aEntry1, 
		const CCalEntry& aEntry2 )
	{
	TBool matchingEntries = EFalse;
	matchingEntries = aEntry1.UidL() == aEntry2.UidL();
	matchingEntries = matchingEntries && ( aEntry1.StartTimeL().TimeLocalL() == aEntry2.StartTimeL().TimeLocalL() );
	
	//dont compare end time if either entry end time is null
	if( ( aEntry1.EndTimeL().TimeLocalL() != Time::NullTTime() ) 
		&& ( aEntry2.EndTimeL().TimeLocalL() != Time::NullTTime() ) )
		{
		matchingEntries = matchingEntries && ( aEntry1.EndTimeL().TimeLocalL() == aEntry2.EndTimeL().TimeLocalL() );
		}
	return matchingEntries;
	}

//if array of entries contain the same entries return true.
TBool CCalendarTestUtility::CompareEntryListsL(
		const RPointerArray< CCalEntry >& aList1,
		const RPointerArray< CCalEntry >& aList2) 
	{
	TBool matchingEntries = EFalse;
	const TInt KListOneCount =  aList1.Count();
	const TInt KListTwoCount =  aList2.Count();
	
	matchingEntries = COMPARE( KListOneCount, ==, KListTwoCount );
	
	if( ! matchingEntries )//if unequal length, return false
		{
		return EFalse;
		}
	else if( KListOneCount == 0 )//matching arrays, if both emtpy
		{
		return ETrue;
		}
	
	//shallow copy, no need to cleanup array, this doesnt make a copy of buffer		
	RPointerArray< CCalEntry > listTwoCopy;
	CleanupClosePushL( listTwoCopy );
	CopyArrayL( aList2, listTwoCopy );
	TIdentityRelation< CCalEntry > entryComparisonFunc( CCalendarTestUtility::CompareEntriesL );
	CCalEntry* entry = NULL;
	for( TInt i = 0; i < KListOneCount; ++i )
		{
		entry = aList1[i];
	
		TInt entryLocation = listTwoCopy.Find( entry, entryComparisonFunc );
		if( COMPAREI1( entryLocation, >, KErrNotFound, i ) )
			{
			listTwoCopy.Remove( entryLocation );
			}
		else
			{
			matchingEntries = EFalse;
			break;
			}
		entry = NULL;
		}
		
	DELETE( entry );
	matchingEntries = matchingEntries && COMPARE( listTwoCopy.Count(), ==, 0 );
	CleanupStack::PopAndDestroy( &listTwoCopy );
	return matchingEntries;
	}

//sets start and end date for current day, week, month, year	
void CCalendarTestUtility::SetPeriodStartAndEndDates(
		TTime& aStartTime, 
		TTime& aEndTime, 
		TTimePeriod aPeriod)
	{
	TTime homeTime;
	TDateTime startDate;
	TDateTime endDate;
    
    
    homeTime.HomeTime();
    startDate 	= homeTime.DateTime();
    endDate 	= homeTime.DateTime();
    
	startDate.SetHour( 0 );
	startDate.SetMinute( 0 );
	startDate.SetSecond( 0 );
	startDate.SetMicroSecond( 0 );
	
	endDate.SetHour( 23 );
	endDate.SetMinute( 59 );
	endDate.SetSecond( 59 );
	endDate.SetMicroSecond( 999999 );
	
	switch( aPeriod )
		{
		case EDay:
			{
			break;
			}
		case EYear:
			{
			endDate.SetYear( startDate.Year() + 1 );
			break;
			}
		case EWeek:
			{
		    const TInt dayInMonth =  homeTime.DayNoInMonth();
		    const TInt daysBeforeEndOfMonth = (homeTime.DaysInMonth() - 1) - dayInMonth;
		    const TInt startOfWeek = daysBeforeEndOfMonth >= 7 ? dayInMonth : ( homeTime.DaysInMonth() - 8 );
		    
		    //set start and end day for a week view
		    startDate.SetDay(startOfWeek);
		    endDate.SetDay( startOfWeek + 7 );
			break;
			}
		case EMonth:
			{
		    //set start and end days for month view
		    startDate.SetDay( 0 );
		    endDate.SetDay( homeTime.DaysInMonth() - 1 );
		    break;
			}
		default:
			{
			_LIT(KInvalidPeriod, "Invalid time period");
			iTestStep.ERR_PRINTF1( KInvalidPeriod );
			iTestStep.TEST1( EFalse, ETrue );
			//reset values
		    startDate 	= homeTime.DateTime();
		    endDate 	= homeTime.DateTime();
			break;
			}
		}
	aStartTime = TTime( startDate ).Int64();
	aEndTime = TTime( endDate ).Int64();
	}

#ifndef __WINSCW__
/**
populates the disk so that only a very small amount of disk space is available,
invoking disk full status in file server
*/
void CCalendarTestUtility::FillDiskL()
	{

	TVolumeInfo tv;
	User::LeaveIfError( iFsSession.Volume(tv) );
	
	iTestStep.INFO_PRINTF3(KDiskSize, tv.iSize, tv.iFree);
	TInt frees = 0;
	iManyFiles = static_cast<TInt>(tv.iFree / KMaxTInt);
	
	_LIT(KTestNumberOfFiles, "Test will create %d full sized empty files");
	iTestStep.INFO_PRINTF2(KTestNumberOfFiles, iManyFiles );
	
	//max size of file is 2gb, as specified by int size
	if ( ( tv.iFree - KDiskSpaceToLeaveFree ) > KMaxTInt )
		{
		TPtrC tname( KFileName );
		TInt i = 0;
		for( ; i < iManyFiles; ++i )//create imanyfile maxsized empty files, until no more can be created
			{
			HBufC *fval=HBufC::NewLC( tname.Length()+4 );//assume #files < 10000
			TPtr fptr = fval->Des() ;
			fptr.Append( tname );
			fptr.AppendNum( i );
			
			User::LeaveIfError( iFile.Replace( iFsSession, fptr, EFileWrite ) );
			User::LeaveIfError( iFile.SetSize( KMaxTInt - KDiskSpaceToLeaveFree) );
			iFile.Close();
			
			CleanupStack::PopAndDestroy( fval );
			}
		User::LeaveIfError( iFsSession.Volume(tv) );
		
		iTestStep.INFO_PRINTF3(KDiskSize, tv.iSize, tv.iFree);
		//check to see if there is still significant free space and create empty file to leave no empty space
		frees = static_cast<TInt>( tv.iFree - KDiskSpaceToLeaveFree );
		if( frees <= 0 )
			{
			frees = static_cast<TInt>( tv.iFree );
			}
		User::LeaveIfError( iFile.Replace( iFsSession, KFileName, EFileWrite ) );
		while( iFile.SetSize( frees++ ) == KErrNone )
			{
			}
		iFile.Close(); 
		}
	else
		{
		//if free space can be filled by one file (on hw) then create only one file
		frees = static_cast<TInt>( tv.iFree - KDiskSpaceToLeaveFree );
		if( frees <= 0 )
			{
			frees = static_cast<TInt>( tv.iFree );
			}
		User::LeaveIfError( iFile.Replace( iFsSession, KFileName, EFileWrite ) );
		_LIT(KSingleFileSize,"Size of lone file %d");
		iTestStep.INFO_PRINTF2(KSingleFileSize, frees );
		while( iFile.SetSize( frees++ ) == KErrNone )
			{
			}
		iFile.Close();
		}
		
	User::LeaveIfError( iFsSession.Volume(tv) );
	
	iTestStep.INFO_PRINTF3(KDiskSize, tv.iSize, tv.iFree);
	}	

/**
deletes all empty files and created by FillDiskL
*/
void CCalendarTestUtility::ClearDiskL()
	{
	TPtrC tname( KFileName );
	TInt err = KErrNone;

	TInt i = 0;
	for(i = 0 ; KErrNotFound != err ; ++i )
		{
		HBufC *fval=HBufC::NewLC( tname.Length()+4 );//assume #files < 10000
		TPtr fptr=fval->Des();
		fptr.Append( tname );
		fptr.AppendNum( i );
		err = iFsSession.Delete( fptr );
		CleanupStack::PopAndDestroy( fval );
		
		if( KErrNone != err && KErrNotFound != err )
			{
			User::Leave( err );
			}
		}
	err = iFsSession.Delete( KFileName );
		
	if( err != KErrNone && err != KErrNotFound )
		{
		User::Leave( err );
		}
	}

/**
delete an empty file so that we are no longer in lowdisk mode
*/
void CCalendarTestUtility::HighDiskModeL()
	{
	TInt err = iFsSession.Delete( KFileName );
	if( KErrNotFound == err )
		{
		TPtrC tname( KFileName );
		HBufC *fval=HBufC::NewLC( tname.Length()+4 );//assume #files < 10000
		TPtr fptr=fval->Des();
		fptr.Append( tname );
		fptr.AppendNum( iManyFiles-1 );
		err = iFsSession.Delete( fptr );
		User::LeaveIfError( err );
		CleanupStack::PopAndDestroy( fval );		
		}
	else if( err != KErrNone )
		{
		User::LeaveIfError( err );
		}
	
	TVolumeInfo tv;
	User::LeaveIfError( iFsSession.Volume(tv) );
	iTestStep.INFO_PRINTF3(KDiskSize, tv.iSize, tv.iFree);	
	}

/**
increase size of last empty file so that we reach diskfull mode again
*/	
void CCalendarTestUtility::RefillL()
	{
	TVolumeInfo tv;
	User::LeaveIfError( iFsSession.Volume(tv) );
	
	//if not in lowdisk mode or disk is full, return
	if(  tv.iFree <= KDiskSpaceToLeaveFree )
		{
		return;
		}
		
	TInt err = iFile.Replace( iFsSession, KFileName, EFileWrite );
	User::LeaveIfError( err );
		
	TInt newsize =  static_cast<TInt>(tv.iFree - KDiskSpaceToLeaveFree);
	
	//increases size of file, until error generated
	while( iFile.SetSize( newsize++ ) == KErrNone  )
		{
		}
	iFile.Close();
	
	User::LeaveIfError( iFsSession.Volume(tv) );
	iTestStep.INFO_PRINTF3(KDiskSize, tv.iSize, tv.iFree);	
	}
#endif
