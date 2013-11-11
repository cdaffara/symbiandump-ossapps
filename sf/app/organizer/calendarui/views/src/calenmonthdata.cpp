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
* Description:  CalenMonthData implementation.
*
*/


//debug
#include "calendarui_debug.h"

//  INCLUDES
#include "calenmonthdata.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenmonthdataTraces.h"
#endif



// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CalenMonthData::CalenMonthData
// 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenMonthData::CalenMonthData()
    : mDay(QDateTime()), mHasEvents(false)
    {
    OstTraceFunctionEntry0( CALENMONTHDATA_CALENMONTHDATA_ENTRY );

    OstTraceFunctionExit0( CALENMONTHDATA_CALENMONTHDATA_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenMonthData::CalenMonthData
// 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenMonthData::CalenMonthData
(QDateTime day)
    : mDay(day), mHasEvents(false)
    {
    OstTraceFunctionEntry0( DUP1_CALENMONTHDATA_CALENMONTHDATA_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CALENMONTHDATA_CALENMONTHDATA_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenMonthData::~CalenMonthData
// 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenMonthData::~CalenMonthData()
    {
    OstTraceFunctionEntry0( DUP2_CALENMONTHDATA_CALENMONTHDATA_ENTRY );
    
    OstTraceFunctionExit0( DUP2_CALENMONTHDATA_CALENMONTHDATA_EXIT );
    }

// ---------------------------------------------------------
// CalenMonthData::Day
// return day
// (other items were commented in a header).
// ---------------------------------------------------------
//
QDateTime CalenMonthData::Day()
    {
    OstTraceFunctionEntry0( CALENMONTHDATA_DAY_ENTRY );
    
    OstTraceFunctionExit0( CALENMONTHDATA_DAY_EXIT );
    return mDay;
    }

// -----------------------------------------------------------------------------
// ?classname::SetHasEvents
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CalenMonthData::SetHasEvents(bool hasEvents)
    {
    OstTraceFunctionEntry0( CALENMONTHDATA_SETHASEVENTS_ENTRY );
    
    mHasEvents = hasEvents;
    
    OstTraceFunctionExit0( CALENMONTHDATA_SETHASEVENTS_EXIT );
    }

// -----------------------------------------------------------------------------
// ?classname::HasEvents
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
bool CalenMonthData::HasEvents()
    {
    OstTraceFunctionEntry0( CALENMONTHDATA_HASEVENTS_ENTRY );
    
    OstTraceFunctionExit0( CALENMONTHDATA_HASEVENTS_EXIT );
    return mHasEvents;
    }

void CalenMonthData::setActive(bool isActive)
    {
    OstTraceFunctionEntry0( CALENMONTHDATA_SETACTIVE_ENTRY );
    
    mIsActive = isActive;
    
    OstTraceFunctionExit0( CALENMONTHDATA_SETACTIVE_EXIT );
    }

bool CalenMonthData::isActive()
    {
    OstTraceFunctionEntry0( CALENMONTHDATA_ISACTIVE_ENTRY );
    
    OstTraceFunctionExit0( CALENMONTHDATA_ISACTIVE_EXIT );
    return mIsActive;
    }
// End of File
