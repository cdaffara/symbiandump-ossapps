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
* Description:   Calendar Lunar Plugin 
*
*/



#include "calendarui_debug.h"
#include "CalenLunarInfo.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenlunarinfoTraces.h"
#endif


// -----------------------------------------------------------------------------
// TCalenLunarInfo::HasFestival
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TCalenLunarInfo::HasFestival()
    {
    OstTraceFunctionEntry0( TCALENLUNARINFO_HASFESTIVAL_ENTRY );
    OstTraceFunctionExit0( TCALENLUNARINFO_HASFESTIVAL_EXIT );
    return iFestival >= 0;
    }

// -----------------------------------------------------------------------------
// TCalenLunarInfo::HasSolarTerm
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TCalenLunarInfo::HasSolarTerm()
    {
    OstTraceFunctionEntry0( TCALENLUNARINFO_HASSOLARTERM_ENTRY );
    OstTraceFunctionExit0( TCALENLUNARINFO_HASSOLARTERM_EXIT );
    return iSolarTerm >= 0;
    }

// -----------------------------------------------------------------------------
// TCalenLunarInfo::AnimalYear
// -----------------------------------------------------------------------------
//

EXPORT_C TCalenLunarInfo::TAnimalYear TCalenLunarInfo::AnimalYear()
    {
    OstTraceFunctionEntry0( TCALENLUNARINFO_ANIMALYEAR_ENTRY );
    ASSERT( EYearOfRat < EYearOfPig );
    ASSERT( EYearOfRat <= iTerrestialBranch );
    ASSERT( iTerrestialBranch <= EYearOfPig );
    
    // Ordering is same for animal years and terrestial branches
    return static_cast<TCalenLunarInfo::TAnimalYear>( iTerrestialBranch );
    }
