/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements geotagging 3AM timer
*
*/

#include "mylocationgeotagtimerao.h"
#include <mylocationlogger.h>
#include <aknappui.h>
#include <aknnotewrappers.h>
#include <avkon.hrh>

//Time at which the geotagging should be triggered( 3.00 AM )
const TInt GEOTAGGING_TIME_IN_HOURS = 3;
//Hour specified in minutes
const TInt HOUR_VALUE_IN_MINUTES = 60;
//Hour specified in seconds
const TInt HOUR_VALUE_IN_SECONDS = 3600;

// --------------------------------------------------------------------------
// CLocationGeoTagTimerAO::CLocationGeoTagTimerAO
// --------------------------------------------------------------------------
//
CLocationGeoTagTimerAO::CLocationGeoTagTimerAO( MyLocationTimerObserver& aObserver ):
            CTimer(EPriorityStandard ),
            iTimerObserver(&aObserver)
{

}


// --------------------------------------------------------------------------
// CLocationGeoTagTimerAO::~CLocationGeoTagTimerAO
// --------------------------------------------------------------------------
//
CLocationGeoTagTimerAO::~CLocationGeoTagTimerAO()
{
    MYLOCLOGSTRING ("CLocationGeoTagTimerAO::~CLocationGeoTagTimerAO(), begin");
    Cancel();
    MYLOCLOGSTRING ("CLocationGeoTagTimerAO::~CLocationGeoTagTimerAO(), end");
}

// --------------------------------------------------------------------------
// CLocationGeoTagTimerAO::NewL
// --------------------------------------------------------------------------
//
CLocationGeoTagTimerAO* CLocationGeoTagTimerAO::NewL( MyLocationTimerObserver& aObserver )
{
    CLocationGeoTagTimerAO* self = 
         new( ELeave ) CLocationGeoTagTimerAO( aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
   
    return self;
}


// --------------------------------------------------------------------------
// CLocationGeoTagTimerAO::ConstructL
// --------------------------------------------------------------------------
//
void CLocationGeoTagTimerAO::ConstructL()
{
    MYLOCLOGSTRING ("CLocationGeoTagTimerAO::ConstructL(), begin");
    CActiveScheduler::Add(this);
    CTimer::ConstructL();
    MYLOCLOGSTRING ("CLocationGeoTagTimerAO::ConstructL(), end");
}

// --------------------------------------------------------------------------
// CLocationGeoTagTimerAO::StartTimer
// --------------------------------------------------------------------------
//
void CLocationGeoTagTimerAO::StartTimer()
{
    MYLOCLOGSTRING ("CLocationGeoTagTimerAO::StartTimer(), begin");
    if(!IsActive())
    {
        TTime hometime;
        hometime.HomeTime();
        
        //Get the current time in Hour,Minute, Second
        TDateTime  currentDateTime = hometime.DateTime();
        TInt currentHr = currentDateTime.Hour(); 
        TInt currentMin = currentDateTime.Minute();
        TInt currentSec = currentDateTime.Second();
        
        //3 AM in seconds
        TInt targetTimeInSeconds = GEOTAGGING_TIME_IN_HOURS * HOUR_VALUE_IN_SECONDS;
        TInt timeDifference;
        
        //Find the time difference in seconds between current time to 3.00 AM 
        //Either on same day or next day.
        if ( currentHr < GEOTAGGING_TIME_IN_HOURS )
        {
           timeDifference = targetTimeInSeconds - 
                    ( ( currentHr * HOUR_VALUE_IN_SECONDS  ) + ( currentMin * HOUR_VALUE_IN_MINUTES ) + currentSec );
        }
        else
        {
           timeDifference = ( 24 * HOUR_VALUE_IN_SECONDS - ( 
                    ( currentHr * HOUR_VALUE_IN_SECONDS ) + ( currentMin * HOUR_VALUE_IN_MINUTES ) + currentSec ) )  +
                    targetTimeInSeconds ;
        }
        
        //Add the time difference to current time to set the target time ( 3.00 AM )
        TTimeIntervalSeconds interval( timeDifference );
        TTime timeToSet;
        timeToSet.HomeTime();
        timeToSet+= interval;
        
        At( timeToSet );
    }
    MYLOCLOGSTRING ("CLocationGeoTagTimerAO::StartTimer(), end");
}


// --------------------------------------------------------------------------
// CLocationGeoTagTimerAO::RunL
// --------------------------------------------------------------------------
//
void CLocationGeoTagTimerAO::RunL( )
{
    MYLOCLOGSTRING ("CLocationGeoTagTimerAO::RunL(), begin");
    TInt status = iStatus.Int();
    
    switch( status )
    {
        case KErrNone:
            //Trigger the reverse geocoding and start the timer again
            //Create the instance of geotagger class
            iTimerObserver->MyLocationThreeAMTimerExpiredL();
            
            break;
        default:
            StartTimer();
            break;
    }
    MYLOCLOGSTRING ( "CLocationGeoTagTimerAO::RunL(), end" );
}

// End of file

