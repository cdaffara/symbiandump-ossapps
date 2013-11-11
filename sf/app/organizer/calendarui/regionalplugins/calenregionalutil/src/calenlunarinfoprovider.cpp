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
#include "CalenLunarInfoProvider.h"
#include "CalenLunarInfo.h"
#include "CalenSolarTerms.h"

#include <calendarconverter.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenlunarinfoproviderTraces.h"
#endif



// ---------------------------------------------------------
// CCalenCalanderConverter::CalculateCelestialIndex
// Calculates a Heavenly stem's index and a Terrestrial branch's index
// ---------------------------------------------------------
//
static void CelestialIndex(const TChineseDate& aChineseDate,
                           TInt& aCelestialIndex, 
                           TInt& aTerrestrialIndex)
    {
    OstTraceFunctionEntry0( _CELESTIALINDEX_ENTRY );
    aCelestialIndex = aChineseDate.iYear % 10;
    
    if (aCelestialIndex == 0)
        {
        aCelestialIndex = 9;
        }
    else
        {
        aCelestialIndex--;
        }

    aTerrestrialIndex = aChineseDate.iYear % 12;
    if (aTerrestrialIndex == 0)
        {
        aTerrestrialIndex = 11;
        }
    else
        {
        aTerrestrialIndex--;
        }
    
    OstTraceFunctionExit0( _CELESTIALINDEX_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenCalanderConverter::ChineseFestival
// -----------------------------------------------------------------------------
//
static TCalenLunarInfo::TFestival ChineseFestival(
    const TChineseDate& aChineseDate, 
    const TChineseDate& aNextDate)
    {
    OstTraceFunctionEntry0( _CHINESEFESTIVAL_ENTRY );
    TCalenLunarInfo::TFestival index = TCalenLunarInfo::ENoFestival;

    if (!aChineseDate.iLeapMonth)
        {
        if (aChineseDate.iMonth == 1 && aChineseDate.iDay == 1)
            {
            index = TCalenLunarInfo::EFestivalSpring;
            }
        else if (aChineseDate.iMonth == 1 && aChineseDate.iDay == 15)
            {
            index = TCalenLunarInfo::EFestivalLantern;
            }
        else if (aChineseDate.iMonth == 5 && aChineseDate.iDay == 5)
            {
            index = TCalenLunarInfo::EFestivalDragonBoat;
            }
        else if (aChineseDate.iMonth == 7 && aChineseDate.iDay == 7)
            {
            index = TCalenLunarInfo::EFestivalSeventhNight;
            }
        else if (aChineseDate.iMonth == 7 && aChineseDate.iDay == 15)
            {
            index = TCalenLunarInfo::EFestivalGhost;
            }
        else if (aChineseDate.iMonth == 8 && aChineseDate.iDay == 15)
            {
            index = TCalenLunarInfo::EFestivalMidAutumn;
            }
        else if (aChineseDate.iMonth == 9 && aChineseDate.iDay == 9)
            {
            index = TCalenLunarInfo::EFestivalDoubleNinth;
            }
        else if (aChineseDate.iMonth == 12 && aChineseDate.iDay == 8)
            {
            index = TCalenLunarInfo::EFestivalTwelfthMonth;
            }
        else
            {
            // nothing
            }
        }
    if (!aNextDate.iLeapMonth &&
        aNextDate.iMonth == 1 && aNextDate.iDay == 1)
        {
        index = TCalenLunarInfo::EFestivalNewYearEve;
        }
    
    OstTraceFunctionExit0( _CHINESEFESTIVAL_EXIT );
    return index;
    }

// -----------------------------------------------------------------------------
// CCalenLunarInfoProvider::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarInfoProvider* CCalenLunarInfoProvider::NewL(RFs& aFs)
    {
    OstTraceFunctionEntry0( CCALENLUNARINFOPROVIDER_NEWL_ENTRY );
    CCalenLunarInfoProvider* self = new (ELeave) CCalenLunarInfoProvider();
    CleanupStack::PushL(self);
    self->ConstructL(aFs);
    CleanupStack::Pop(self);
    
    OstTraceFunctionExit0( CCALENLUNARINFOPROVIDER_NEWL_EXIT );
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenLunarInfoProvider::~CCalenLunarInfoProvider
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarInfoProvider::~CCalenLunarInfoProvider()
    {
    OstTraceFunctionEntry0( CCALENLUNARINFOPROVIDER_CCALENLUNARINFOPROVIDER_ENTRY );
    delete iSolarTerms; 
    delete iConverter;
    OstTraceFunctionExit0( CCALENLUNARINFOPROVIDER_CCALENLUNARINFOPROVIDER_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarInfoProvider::CCalenLunarInfoProvider
// -----------------------------------------------------------------------------
//
CCalenLunarInfoProvider::CCalenLunarInfoProvider()
    {
    OstTraceFunctionEntry0( DUP1_CCALENLUNARINFOPROVIDER_CCALENLUNARINFOPROVIDER_ENTRY );
    OstTraceFunctionExit0( DUP1_CCALENLUNARINFOPROVIDER_CCALENLUNARINFOPROVIDER_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarInfoProvider::ConstructL
// -----------------------------------------------------------------------------
//
void CCalenLunarInfoProvider::ConstructL(RFs& aFs)
    {
OstTraceFunctionEntry0( CCALENLUNARINFOPROVIDER_CONSTRUCTL_ENTRY );

    iConverter = CChineseCalendarConverter::NewL();
    iSolarTerms = CCalenSolarTerms::NewL(aFs);
    OstTraceFunctionExit0( CCALENLUNARINFOPROVIDER_CONSTRUCTL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarInfoProvider::GetLunarInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C TCalenLunarInfo CCalenLunarInfoProvider::GetLunarInfoL( const TTime& aDay ) 
    {
    OstTraceFunctionEntry0( CCALENLUNARINFOPROVIDER_GETLUNARINFOL_ENTRY );
    TDateTime dayDt = aDay.DateTime();

    TTime nextDay = aDay + TTimeIntervalDays(1);
    TDateTime nextDayDt = nextDay.DateTime();

    TChineseDate chineseDate;
    TChineseDate nextChineseDate;
    
    iConverter->DateTimeToChineseL(dayDt, chineseDate);
    iConverter->DateTimeToChineseL(nextDayDt, nextChineseDate);
    
    TCalenLunarInfo info;
    info.iLunarDate = chineseDate;
    info.iGregorianDate = aDay;
    info.iFestival = ChineseFestival( chineseDate, nextChineseDate );
    info.iSolarTerm = iSolarTerms->CheckSolarTermDateL( dayDt );
    CelestialIndex( chineseDate, info.iHeavenlyStem, info.iTerrestialBranch );
    
    OstTraceFunctionExit0( CCALENLUNARINFOPROVIDER_GETLUNARINFOL_EXIT );
    return info;
    }
