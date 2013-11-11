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
* Description: 
*     Functions to calculate timer intervals
*
*/

#include "ipssosaopluginheaders.h"

const TInt KAOSecondsIn24Hours = 86400;
//const TUint KAOEveryDayMask = 0x7f;
const TInt KA0DaysInWeek = 7;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
//<Qmail>
CIpsSosAOSchedulerUtils::CIpsSosAOSchedulerUtils( CIpsSosAOSettingsHandler& aSettings ):
        iSettings(aSettings)
//</Qmail>
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOSchedulerUtils::~CIpsSosAOSchedulerUtils()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOSchedulerUtils::ConstructL()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
//<Qmail>
CIpsSosAOSchedulerUtils* CIpsSosAOSchedulerUtils::NewL(CIpsSosAOSettingsHandler& aSettings)
//</Qmail>
    {
    FUNC_LOG;
    //<Qmail>
    CIpsSosAOSchedulerUtils* self = NewLC(aSettings);
    //</Qmail>
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
//<Qmail>
CIpsSosAOSchedulerUtils* CIpsSosAOSchedulerUtils::NewLC(CIpsSosAOSettingsHandler& aSettings)
//</Qmail>
    {
    FUNC_LOG;
    CIpsSosAOSchedulerUtils* self =
            //<Qmail>
        new ( ELeave ) CIpsSosAOSchedulerUtils(aSettings);
    //</Qmail>
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TBool CIpsSosAOSchedulerUtils::CanConnect()
    {
    FUNC_LOG;

    // Get hometime
    TTime clock;
    TTimeIntervalSeconds homeTime;
    TTimeIntervalSeconds startTime;
    TTimeIntervalSeconds stopTime;
    PrepareScheduling( clock, homeTime, startTime, stopTime ); 


    TBool ok = CanConnect( clock, homeTime, startTime, stopTime ); 
    
    
    
    
    return ok;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TIpsSosAOConnectionType CIpsSosAOSchedulerUtils::SecondsToNextMark(
    TTimeIntervalSeconds& aSeconds )
    {
    FUNC_LOG;

    // Get hometime
    aSeconds = 0;   
    TTime clock;
    TTimeIntervalSeconds homeTime;
    TTimeIntervalSeconds startTime;
    TTimeIntervalSeconds stopTime;
    PrepareScheduling( clock, homeTime, startTime, stopTime );          
        
    // At first it needs to be checked, that either previous or current day
    // is selected.
    TIpsSosAOValidConnectionDay validDay = GetValidConnectionDay( clock );
    TBool timeToConnect = ( validDay != EAOVCDNoConnection );
    
    // Next step is to check, is it time for connection or disconnection
    if ( timeToConnect )
        {        
        timeToConnect = IsValidTimeToConnect( 
            validDay, homeTime, startTime, stopTime );
        }
        

    // Make the conversion
    TIpsSosAOConnectionType connectStatus;
    
    if ( timeToConnect )
        {
        connectStatus = EAOCDisconnectAfter;
        }
    else
        {
        connectStatus = EAOCConnectAfter;
        }
    

    // Finally calculate the time to next mark       
    CalculateSecondsToNextMark(
        connectStatus, aSeconds, clock, homeTime, startTime, stopTime );      

    
    
            
    return connectStatus;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOSchedulerUtils::PrepareScheduling(
    TTime& aClock,
    TTimeIntervalSeconds& aHome,
    TTimeIntervalSeconds& aStart,
    TTimeIntervalSeconds& aStop )
    {
    FUNC_LOG;
    // Get the hometime and set the day flags
    aClock.HomeTime();    
        
    // First, all the times has to be converted to seconds, to guarrantee
    // proper comparisions for the times
    Times2Seconds( aClock, aHome, aStart, aStop );
    
    // Start and stop times needs to be reordered so, that the
    // start time is before stop time in proportion to hometime
    OrganizeStartAndStopTimes( aHome, aStart, aStop );
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TBool CIpsSosAOSchedulerUtils::CanConnect(
    const TTime& aClock,
    TTimeIntervalSeconds& aHome,
    TTimeIntervalSeconds& aStart,
    TTimeIntervalSeconds& aStop )
    {
    FUNC_LOG;

    TIpsSosAOValidConnectionDay validDay = GetValidConnectionDay( aClock );
    TBool result = EFalse;
    
    // Connecting can be ok, if either previous or current day are checked
    if ( validDay != EAOVCDNoConnection )
        {
        
        result = IsValidTimeToConnect(
            validDay, aHome, aStart, aStop );
        }

    
    return result;    
    }

// ----------------------------------------------------------------------------
// The connection is allowed in following cases:
// Day:     Mo Tu We Th Fr Sa Su
// Checked: 0  0  0  1  1  0  0
// Allowed: F  F  F  T  T  T  F
// ----------------------------------------------------------------------------
//
TIpsSosAOValidConnectionDay CIpsSosAOSchedulerUtils::GetValidConnectionDay(
    const TTime& aClock )
    {
    FUNC_LOG;

    // Get current and previous day and make sure it is selected
    TDay currentDay = aClock.DayNoInWeek();
    TDay previousDay = GetPrevDay( currentDay );
    TIpsSosAOValidConnectionDay result;
        
    // Connection can be made anyday
    if ( IsDaySelected( currentDay ) && IsDaySelected( previousDay ) )
        {
        
        result = EAOVCDConnectionAnyDay;        
        }
    // Connection can be established when start time is at current day
    else if ( IsDaySelected( currentDay ) )
        {
        
        result = EAOVCDConnectionCurrentDay;
        }
    // Connection can be established, if the connection begins from
    // checked day
    else if ( IsDaySelected( previousDay ) )
        {
        
        result = EAOVCDConnectionPreviousDayOnly;
        }
    // no selected days this is wron in settings side but still return anyday
	//<QMail>
    else if ( iSettings.SelectedWeekDays() == 0 )
        {
        result = EAOVCDConnectionAnyDay;
        }
	//</QMail>
    // Connection is not allowed
    else
        {
        
        result = EAOVCDNoConnection;
        } 
    
    return result;           
    }

// ----------------------------------------------------------------------------)
// ----------------------------------------------------------------------------
//
TDay CIpsSosAOSchedulerUtils::GetNextDay(
    const TDay aToday,
    const TInt aNth )
    {
    FUNC_LOG;
    return static_cast<TDay>( ( aToday + aNth ) % KA0DaysInWeek );    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TDay CIpsSosAOSchedulerUtils::GetPrevDay(
    const TDay aToday,
    const TInt aNth )
    {
    FUNC_LOG;
    TInt weekIncrease = ( ( aNth / KA0DaysInWeek ) + 1 ) * KA0DaysInWeek;
    return static_cast<TDay>( ( weekIncrease + aToday - aNth ) % KA0DaysInWeek );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TBool CIpsSosAOSchedulerUtils::IsValidTimeToConnect(
    const TIpsSosAOValidConnectionDay aValidDay,
    TTimeIntervalSeconds& aHome,
    TTimeIntervalSeconds& aStart,
    TTimeIntervalSeconds& aStop )
    {
    FUNC_LOG;

    TBool result = EFalse;
    
    // Combine the possible combinations.
    // Connection in current day is possible, only if the current day or 
    // all days are selected
    TBool currentDay = ( aValidDay == EAOVCDConnectionCurrentDay ||
                         aValidDay == EAOVCDConnectionAnyDay );
    // Connection can extended from previous day, only if previous day is
    // set or all days are set                         
    TBool previousDay = ( aValidDay == EAOVCDConnectionPreviousDayOnly ||
                          aValidDay == EAOVCDConnectionAnyDay );
                         
    // First it is needed to check that are the times even set
    // If the times are equal, the times can be considered to be valid for
    // all the day
    if ( currentDay && aStart == aStop )
        {
        result = ETrue;
        }
    // It's allowed to make the connection in following situations homeTime 
    // is between startTime and stopTime, and the connection is allowed
    // for the day. The connection is not allowed to be open during 
    // the last minute (aHome < aStop), but connection should be 
    // made at the start of the scheduling (aStart <= aHome).
    else if ( aStart <= aHome && aHome < aStop )
        {

        // If connection extends from previous day and only extended connection
        // is allowed
        if ( previousDay && aStart.Int() < 0 )
            {
            
            result = ETrue;
            }
        // If previous day is not checked and connection extends over day
        // make sure that the correct time is used
        else if ( currentDay && aStart.Int() >= 0 )
            {
            
            result = ETrue;
            }
        else
            {
            
            result = EFalse;
            }
        }
    // Neither of the statements were fulfilled, do not allow connection
    else
        {
        
        result = EFalse;
        }

    return result;    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TDateTime CIpsSosAOSchedulerUtils::AdjustHomeTime(
    const TTime& aClock )
    {
    FUNC_LOG;
    // Zero time is the beginning of the day
    TTime zeroTime( 0 );

    TDateTime dtHome = aClock.DateTime();
    TDateTime dtZero = zeroTime.DateTime();

    dtZero.SetHour( dtHome.Hour() );
    dtZero.SetMinute( dtHome.Minute() );
    dtZero.SetSecond( dtHome.Second() );
    
    return dtZero;    
    }

// ----------------------------------------------------------------------------
// This function converts home-, start- and stop times to secondes from the
// beginning of the day
// ----------------------------------------------------------------------------
//
void CIpsSosAOSchedulerUtils::Times2Seconds(
    const TTime& aClock,
    TTimeIntervalSeconds& aHome,
    TTimeIntervalSeconds& aStart,
    TTimeIntervalSeconds& aStop )
    {
    FUNC_LOG;

    TTime zeroTime( 0 );
    TTime adjustedHomeTime = AdjustHomeTime( aClock );
	//<QMail>
    TTime selectedTimeStart = iSettings.SelectedTimeStart();
    TTime selectedTimeStop = iSettings.SelectedTimeStop();
	//</QMail>
    adjustedHomeTime.SecondsFrom( zeroTime, aHome );
    selectedTimeStart.SecondsFrom( zeroTime, aStart );
    selectedTimeStop.SecondsFrom( zeroTime, aStop );
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOSchedulerUtils::OrganizeStartAndStopTimes(
    TTimeIntervalSeconds& aHome,
    TTimeIntervalSeconds& aStart,
    TTimeIntervalSeconds& aStop )
    {
    FUNC_LOG;

    // The start-, stop- and home times needs to be reordered so that the
    // start is always before stop, in other words, start time is smaller
    // than stop time.
    //
    // Following cases need to be considered:
    //     aStart < aStop  (OK) -       aStart > aStop (NOK)
    // aHome < aStart < aStop       ( aHome < aStop ) < aStart  (start-24h)
    // aStart < aHome < aStop       aStop < aHome < aStart  (end+24h)
    // aStart < aStop < aHome       aStop < aStart < aHome  (end+24h)
    if ( aStart > aStop )
        {       
        // If we're between the the start and stop times,
        // move this
        if ( aHome < aStop )
            {
            // Set starting time to previous day
            aStart = ( aStart.Int() - KAOSecondsIn24Hours );
            }
        else
            {           
            // Set ending time to next day
            aStop = ( aStop.Int() + KAOSecondsIn24Hours );
            }
        }

        
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOSchedulerUtils::CalculateSecondsToNextMark(
    const TIpsSosAOConnectionType aConnectionStatus,
    TTimeIntervalSeconds& aSeconds,
    const TTime& aClock,
    const TTimeIntervalSeconds& aHome,
    const TTimeIntervalSeconds& aStart,
    const TTimeIntervalSeconds& aStop )
    {
    FUNC_LOG;

    // If all day is selected, next stop should be the midnight of the next
    // (un)scheduled day depending on should the connection be on or off
    if ( aStart == aStop )
        {
                
        aSeconds = CalcSecsToNextScheduledDay(
            aClock, aHome, aConnectionStatus == EAOCConnectAfter );
        
        }
    // To calculate seconds, it needs to checked, should the connection be now
    // on or off. If between, the connection should be on and seconds should
    // be calculated to next disconnect time
    else if ( aConnectionStatus == EAOCDisconnectAfter )
        {
        
        CalcSecsToMark( aSeconds, aClock, aHome, aStop, EFalse );
        
        }
    //
    else
        {
        
        CalcSecsToMark( aSeconds, aClock, aHome, aStart, ETrue );

        }

    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOSchedulerUtils::CalcSecsToMark(
    TTimeIntervalSeconds& aSeconds,
    const TTime& aClock,
    const TTimeIntervalSeconds& aHome,
    const TTimeIntervalSeconds& aBonus,
    const TBool aScheduledDay )
    {
    FUNC_LOG;
    // If extending to next day, calculate seconds to next day
    if ( aHome > aBonus )
        {            
        aSeconds = CalcSecsToNextScheduledDay( aClock, aHome, aScheduledDay );
        aSeconds = aSeconds.Int() + aBonus.Int();
        }
    else
        {
        aSeconds = aBonus.Int() - aHome.Int();            
        }
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TTimeIntervalSeconds CIpsSosAOSchedulerUtils::CalcSecsToNextScheduledDay(
//<QMail>
    const TTime& aClock,
    const TTimeIntervalSeconds& aHome,
    const TBool aScheduledDay )
    {
    FUNC_LOG;

    TTimeIntervalSeconds seconds = 0;

    // First check if all of the days are unchecked, which should not happen
    // at all, but just in case
    
    if ( !iSettings.SelectedWeekDays() )
        {
        return seconds;
        }

    TBool hit = EFalse;
    
    TDay today = aClock.DayNoInWeek();
    TDay dayAfter = GetNextDay( today );
    TInt dayCount = KErrNotFound;
    
    while ( dayAfter != today && !hit )
        {
        dayCount++;
        hit = ( IsDaySelected( dayAfter ) == aScheduledDay );
        dayAfter = GetNextDay( dayAfter );
        }

    
    seconds = KAOSecondsIn24Hours - aHome.Int() +
        dayCount * KAOSecondsIn24Hours;

    //</QMail>
    return seconds;       
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
//<QMail>
TBool CIpsSosAOSchedulerUtils::IsDaySelected( const TUint aDay ) const
    {
    FUNC_LOG;
    return ( iSettings.SelectedWeekDays() >> aDay ) & 0x01;
    //</QMail>
    }

// End of File

