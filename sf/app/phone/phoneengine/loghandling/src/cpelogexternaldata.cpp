/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPELogExternalData class 
*                member functions.
*
*/



//  INCLUDE FILES
#include "cpelogexternaldata.h"
#include "cpeloghandling.h"
#include "cpeloginfo.h"
#include <centralrepository.h>
#include <LogsDomainCRKeys.h>
#include <talogger.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
/// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPELogExternalData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPELogExternalData* CPELogExternalData::NewL
        ( 
        CPELogHandling& aOwner
        )  
    {
    TEFLOGSTRING(KTAOBJECT, "LOG CPELogExternalData::NewL start.");
    CPELogExternalData* self = 
        new ( ELeave ) CPELogExternalData( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TEFLOGSTRING(KTAOBJECT, "LOG CPELogExternalData::NewL complete.");
    return self;
    }

// -----------------------------------------------------------------------------
// CPELogExternalData::~CPELogExternalData
// Destructor.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / REPOSITORY
*    Series 60  REPOSITORY API
*****************************************************/
CPELogExternalData::~CPELogExternalData()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogExternalData::~CPELogExternalData" );
    delete iRepository;
    }
    
// -----------------------------------------------------------------------------
// CPELogExternalData::CPELogExternalData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPELogExternalData::CPELogExternalData
        ( 
        CPELogHandling& aOwner
        ) : iOwner( aOwner ),
            iFirstCallStartTime( 0 )
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogExternalData::CPELogExternalData" );
    }

// -----------------------------------------------------------------------------
// CPELogExternalData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / REPOSITORY
*    Series 60  REPOSITORY API
*****************************************************/
void CPELogExternalData::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogExternalData::ConstructL" );

    iRepository = CRepository::NewL( KCRUidLogs );
    }

// -----------------------------------------------------------------------------
// CPELogExternalData::UpdateCallTimers
// Write call durations to TASharedData.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPELogExternalData::UpdateCallTimers
        ( 
        const CPELogInfo& aLogInfo // Call information
        )
    {
    TInt duration( 0 );

    duration = aLogInfo.Duration().Int();

    switch( aLogInfo.CurrentLine() )
        {
        case CCCECallParameters::ECCELineTypePrimary:
            {
            if ( RMobileCall::EMobileOriginated == aLogInfo.CallDirection() )
                {
                if ( CPELogInfo::EPEVoIPEvent == aLogInfo.EventType() )
                    {
                    UpdateTimer( KLogsDialledCallsTimerVoIP, 
                        duration, *iRepository );
                    }
                else
                    {
                    UpdateTimer( KLogsDialledCallsTimerLine1, 
                        duration, *iRepository );
                    }
                }
            else if ( RMobileCall::EMobileTerminated == aLogInfo.CallDirection() )
                {
                if( CPELogInfo::EPEVoIPEvent == aLogInfo.EventType() )
                    {
                    UpdateTimer( KLogsReceivedCallsTimerVoIP, 
                        duration, *iRepository );
                    }
                else
                    {
                    UpdateTimer( KLogsReceivedCallsTimerLine1, 
                        duration, *iRepository );
                    }
                }
            }
            break;
        case CCCECallParameters::ECCELineTypeAux:
            {
            if( RMobileCall::EMobileOriginated == aLogInfo.CallDirection() ) 
                {
                UpdateTimer( KLogsDialledCallsTimerLine2, 
                    duration, *iRepository );
                }
            else  if ( RMobileCall::EMobileTerminated == aLogInfo.CallDirection() )
                {
                UpdateTimer( KLogsReceivedCallsTimerLine2, 
                    duration, *iRepository );
                }
            }
            break;
        default:
            {
            TEFLOGSTRING2( KTAINT, "LOG CPELogExternalData::UpdateCallTimers(): FAILED. Unexpected line: %d", aLogInfo.CurrentLine() );
            }
            break;
        }    
    }

// -----------------------------------------------------------------------------
// CPELogExternalData::UpdateLastCallTimerByLine
// Updates line specific last call timers to shared data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / REPOSITORY
*    Series 60  REPOSITORY API
*****************************************************/
void CPELogExternalData::UpdateLastCallTimerByLine
        ( 
        const CPELogInfo& aLogInfo 
        )
    {
    TInt errorCode( KErrNone );

    switch( aLogInfo.CurrentLine() )
        {
        case CCCECallParameters::ECCELineTypePrimary:
            {
            if ( CPELogInfo::EPEVoIPEvent == aLogInfo.EventType() )
                {
                TEFLOGSTRING( KTAREQOUT, 
                "LOG CPELogExternalData::UpdateLastCallTimerByLine > CRepository::Set(KLogsLastCallTimerVoIP)" )
                errorCode = iRepository->Set( 
                    KLogsLastCallTimerVoIP, aLogInfo.Duration().Int() );
                }
            else
                {
                TEFLOGSTRING( KTAREQOUT, 
                "LOG CPELogExternalData::UpdateLastCallTimerByLine > CRepository::Set(KLogsLastCallTimerLine1)" )
                errorCode = iRepository->Set( 
                    KLogsLastCallTimerLine1, aLogInfo.Duration().Int() );                
                }
            }
            break;
        case CCCECallParameters::ECCELineTypeAux:
            {
            TEFLOGSTRING( KTAREQOUT, 
            "LOG CPELogExternalData::UpdateLastCallTimerByLine > CRepository::Set(KLogsLastCallTimerLine2)" );

            errorCode = iRepository->Set( KLogsLastCallTimerLine2, aLogInfo.Duration().Int() );
            }
            break;
        default:
            errorCode = KErrArgument;
            break;
        } 

    if ( errorCode != KErrNone )
       {
       TEFLOGSTRING2( KTAERROR, "LOG CPELogExternalData::UpdateLastCallTimerByLine(): FAILED. ErrorCode: %d", errorCode );
       }
   
    }

// -----------------------------------------------------------------------------
// CPELogExternalData::UpdateLastCallTimer
// Updates the last call timer to shared data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / REPOSITORY
*    Series 60  REPOSITORY API
*****************************************************/
TInt CPELogExternalData::UpdateLastCallTimer
        ( 
        const CPELogInfo& aLogInfo
        )
    {
    TInt errorCode( KErrNone );

    // No ongoing entries (calls) -> update last active call timer to shared data.
    // In multicall cases time is from start of first to end of last.
    TTimeIntervalSeconds startsDelta;
    aLogInfo.CallStartTime().SecondsFrom( iFirstCallStartTime, startsDelta );
    TEFLOGSTRING2( KTAINT, "LOG CPELogExternalData::UpdateLastCallTimer(): StartsDelta: %d", startsDelta.Int() );

    TInt duration = startsDelta.Int() + aLogInfo.Duration().Int();

    TEFLOGSTRING2( KTAREQOUT, 
        "LOG CPELogExternalData::UpdateLastCallTimer > CRepository::Set(KLogsActualLastCallTimer, %d )", duration );
    errorCode = iRepository->Set( KLogsActualLastCallTimer, duration );
    if ( errorCode != KErrNone )
        {
        TEFLOGSTRING2( KTAERROR, "LOG CPELogExternalData::UpdateLastCallTimer(): FAILED setting actual last call timer. ErrorCode: %d", errorCode );
        }        
    iFirstCallStartTime = 0;
    return duration;
    }

// -----------------------------------------------------------------------------
// CPELogExternalData::IncreaseMissedCallValue
// Increase missed call's counter.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / REPOSITORY
*    Series 60  REPOSITORY API
*****************************************************/
void CPELogExternalData::IncreaseMissedCallValue( const TInt aCallId )
    {
    TInt errorCode( KErrNone );
    TInt value;
    
    TEFLOGSTRING( KTAREQOUT, 
        "LOG CPELogExternalData::IncreaseMissedCallValue > CRepository::Get" );
    errorCode = iRepository->Get( KLogsNewMissedCalls, value );

    if ( errorCode == KErrNone )
        {
        ++value;
        TEFLOGSTRING2( KTAREQOUT, 
            "LOG CPELogExternalData::IncreaseMissedCallValue > CRepository::Set. value = %d"
            , value );
        errorCode = iRepository->Set( KLogsNewMissedCalls, value );
        iOwner.ResetMissedCalls( aCallId );
        }
    else
        {
        TEFLOGSTRING2( KTAERROR, 
            "LOG CPELogExternalData::IncreaseMissedCallValue(): Failed: %d", errorCode );
        }
    }

// -----------------------------------------------------------------------------
// CPELogExternalData::SetFirstCallStartTime
// Set the call start time on the connected state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPELogExternalData::SetFirstCallStartTime
        (
        const TTime& aFirstCallStartTime
        )
    {
    iFirstCallStartTime = aFirstCallStartTime;
    }    

// -----------------------------------------------------------------------------
// CPELogExternalData::UpdateTimer
// Updates the desired timer to shared data.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / REPOSITORY
*    Series 60  REPOSITORY API
*****************************************************/
void CPELogExternalData::UpdateTimer
        ( 
        const TUint32 aTimer, 
        TInt aDuration,
        CRepository& aRepository
        )
    {
    TEFLOGSTRING( KTAREQOUT, 
        "LOG CPELogExternalData::UpdateTimer > CRepository::Get" );    
    TInt errorCode( KErrNone );
    TInt value( 0 );
    errorCode = aRepository.Get( aTimer, value );
    
    if ( errorCode == KErrNone )
        {
        value += aDuration;
        
        TEFLOGSTRING( KTAREQOUT, 
            "LOG CPELogExternalData::UpdateTimer > CRepository::Set" );
        
        errorCode = aRepository.Set( aTimer, value );
        }
    
    if ( errorCode != KErrNone )
        {
        TEFLOGSTRING2( KTAERROR, 
            "LOG CPELogExternalData::UpdateTimer(): Failed: %d", errorCode );
        }
    }
        
// End of File
