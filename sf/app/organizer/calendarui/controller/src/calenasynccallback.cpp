/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar async callback implementation
 *
*/

 
 // INCLUDE FILES
 
#include "calenasynccallback.h"
#include "calendarui_debug.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenasynccallbackTraces.h"
#endif
 


// ---------------------------------------------------------------------------
// CalenAsyncCallBack::CalenAsyncCallBack
// C++ constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CalenAsyncCallBack::CalenAsyncCallBack(TCallBack& aCallBack, CActive::TPriority  aPriority,
                                         CCalenController& aController)
    :CAsyncCallBack(aCallBack, aPriority),
    iController(aController)
    {
    OstTraceFunctionEntry0( CALENASYNCCALLBACK_CALENASYNCCALLBACK_ENTRY );
     
    OstTraceFunctionExit0( CALENASYNCCALLBACK_CALENASYNCCALLBACK_EXIT );
    }

// ---------------------------------------------------------------------------
// CalenAsyncCallBack::NewL
// First phase construction
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//  
CalenAsyncCallBack* CalenAsyncCallBack::NewL(TCallBack& aCallBack, CActive::TPriority  aPriority,
                             CCalenController& aController)
    {
    OstTraceFunctionEntry0( CALENASYNCCALLBACK_NEWL_ENTRY );
    
    CalenAsyncCallBack* self = new( ELeave ) CalenAsyncCallBack( aCallBack, aPriority , aController );

    OstTraceFunctionExit0( CALENASYNCCALLBACK_NEWL_EXIT );
    return self;
    }
// ---------------------------------------------------------------------------
// CalenAsyncCallBack::~CalenAsyncCallBack
// C++ Destructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//	
CalenAsyncCallBack::~CalenAsyncCallBack()
	{
	OstTraceFunctionEntry0( DUP1_CALENASYNCCALLBACK_CALENASYNCCALLBACK_ENTRY );
	
	OstTraceFunctionExit0( DUP1_CALENASYNCCALLBACK_CALENASYNCCALLBACK_EXIT );
	}

// ---------------------------------------------------------------------------
// CalenAsyncCallBack::BaseConstructL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CalenAsyncCallBack::BaseConstructL()
	{
	OstTraceFunctionEntry0( CALENASYNCCALLBACK_BASECONSTRUCTL_ENTRY );

	OstTraceFunctionExit0( CALENASYNCCALLBACK_BASECONSTRUCTL_EXIT );
	}
	
// ---------------------------------------------------------------------------
// CalenAsyncCallBack::RunL
// Handle Completion
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//	
void CalenAsyncCallBack::RunL()
	{
	OstTraceFunctionEntry0( CALENASYNCCALLBACK_RUNL_ENTRY );
	
	if(iCallBack.CallBack())
	    {
	    // callback only if any of the command handlers 
	    // request for continuing the executing command
	    CallBack();
	    }
	
	OstTraceFunctionExit0( CALENASYNCCALLBACK_RUNL_EXIT );
	}


