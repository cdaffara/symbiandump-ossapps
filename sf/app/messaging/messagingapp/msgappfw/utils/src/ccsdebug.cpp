/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CS general debug class 
*
*/


// INCLUDE FILES

// SYSTEM INCLUDES

// USER INCLUDES
#include "ccsdebug.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsDebugWrapper::__LatencyMarkStart
// Marks the start time for latency measurement
// ----------------------------------------------------------------------------
EXPORT_C void CCsDebugWrapper::__LatencyMarkStartL(TRefByValue<const TDesC> str) 
{
    CCsDebugArr* dbgArr = 0;
    
	// Check thread local storage:
	if ( !(Dll::Tls()))
	{	 
		dbgArr = CCsDebugArr::NewL();
		User::LeaveIfError ( Dll::SetTls( dbgArr ) );
	}
	else
	{
	  	dbgArr = static_cast<CCsDebugArr*>( Dll::Tls() );
	}

	CCsDebug *dbg = CCsDebug::NewL();
	dbg->Mark(str);
    
    dbgArr->Push(*dbg);
}

// ----------------------------------------------------------------------------
// CCsDebugWrapper::__LatencyMarkEnd
// Marks the end time for latency measurement
// Displays the difference from Latency Mark start
// ----------------------------------------------------------------------------
EXPORT_C void CCsDebugWrapper::__LatencyMarkEnd(TRefByValue<const TDesC> str) 
{
    CCsDebugArr* dbgArr = 0;
    
	// Check thread local storage:
	if ( !(Dll::Tls()) )
	{	 
	  	return;
	}
	else
	{
	  	dbgArr = static_cast<CCsDebugArr*>( Dll::Tls() );
	}

    if ( dbgArr && !(dbgArr->IsEmpty()) )
    {
    	CCsDebug* dbg = dbgArr->Pop();
		dbg->UnMark(str);
		delete dbg;			  	
    }
    
    if ( dbgArr && dbgArr->IsEmpty() )
    {
        delete dbgArr;
        Dll::SetTls( NULL );
    }
}

// ----------------------------------------------------------------------------
// CCsDebug::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
EXPORT_C CCsDebug* CCsDebug::NewL()
{
    CCsDebug* self = new (ELeave) CCsDebug();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    return self;
}

// ----------------------------------------------------------------------------
// CCsDebug::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CCsDebug::ConstructL()
{
}

// ----------------------------------------------------------------------------
// CCsDebug::Mark
// Marks the start time for latency measurement
// ----------------------------------------------------------------------------
void CCsDebug::Mark(TRefByValue<const TDesC> str,...)
{
	VA_LIST list;
    VA_START(list, str);

    // Print to log file
    TBuf<MAX_BUFF_LENGTH> buf;
    buf.FormatList(str, list);
    PRINT_LATENCY1 ( _L("#### [%S] Latency Measurement Start ####"), &buf );
    
	startTime.HomeTime();
}

// ----------------------------------------------------------------------------
// CCsDebug::UnMark
// Marks the end time for latency measurement
// Displays the difference from Latency Mark start
// ----------------------------------------------------------------------------
void CCsDebug::UnMark(TRefByValue<const TDesC> str,...)
{
	endTime.HomeTime();
	VA_LIST list;
    VA_START(list, str);

    // Print to log file
    TBuf<MAX_BUFF_LENGTH> buf;
    buf.FormatList(str, list);
	
	TTimeIntervalMicroSeconds diff = endTime.MicroSecondsFrom(startTime);
	TInt mytime = (diff.Int64()) / TIME_FACTOR;

	PRINT_LATENCY2 ( _L("#### [%S] Latency Measurement End, Time taken = %d (ms) ####"), &buf, mytime );
}


// ----------------------------------------------------------------------------
// CCsDebugArr::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
EXPORT_C CCsDebugArr* CCsDebugArr::NewL()
{
    CCsDebugArr* self = new (ELeave) CCsDebugArr();
    return self;
}

// ----------------------------------------------------------------------------
// CCsDebugArr::Push
// Push an element into the array
// ----------------------------------------------------------------------------
EXPORT_C void CCsDebugArr::Push(CCsDebug& dbg)
{
    debugArray.Append(&dbg);
}

// ----------------------------------------------------------------------------
// CCsDebugArr::Pop
// Pop an element from the array
// ----------------------------------------------------------------------------
EXPORT_C CCsDebug* CCsDebugArr::Pop()
{
     TInt index = debugArray.Count() - 1;
     CCsDebug* dbg = debugArray[index];
     debugArray.Remove(index);
	 return (dbg);
}

// ----------------------------------------------------------------------------
// CCsDebugArr::IsEmpty
// Check if array is empty
// ----------------------------------------------------------------------------
EXPORT_C TBool CCsDebugArr::IsEmpty()
{
     if ( debugArray.Count() == 0 )
          return ETrue;
     else 
          return EFalse;
}

// ----------------------------------------------------------------------------
// CCsDebugArr::~CCsDebugArr
// Destructor
// ----------------------------------------------------------------------------
CCsDebugArr::~CCsDebugArr()
{
	debugArray.ResetAndDestroy();
}

// End of file
