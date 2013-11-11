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


#ifndef IPSSOSAOSCHEDULERUTILS_H
#define IPSSOSAOSCHEDULERUTILS_H

#include <e32base.h>
//<Qmail>

//</Qmail>
enum TIpsSosAOConnectionType
    {
    EAOCDisconnectAfter = 0,
    EAOCConnectAfter
    };
        
enum TIpsSosAOValidConnectionDay
    {
    EAOVCDNoConnection = 0,
    EAOVCDConnectionAnyDay,
    EAOVCDConnectionCurrentDay,
    EAOVCDConnectionPreviousDayOnly
    };

//<Qmail>
class CIpsSosAOSettingsHandler;
//</Qmail>
/**
*  Class CIpsSosAOSchedulerUtils 
*  
*  calculates time to next interval according to 
*  mailbox's always online settings
* 
*  @since FSEmail 2.0
*/
//<Qmail>
NONSHARABLE_CLASS (CIpsSosAOSchedulerUtils) : public CBase
//</Qmail>
    {
public:  // Constructors and destructor

    /**
    * NewLC
    * @return, Constructed object
    */
    //<Qmail>
    static CIpsSosAOSchedulerUtils* NewLC(CIpsSosAOSettingsHandler& aSettings);
    //</Qmail>
    /**
    * NewL
    * @return, Constructed object
    */
    //<Qmail>
    static CIpsSosAOSchedulerUtils* NewL(CIpsSosAOSettingsHandler& aSettings);
    //</Qmail>
    /**
    * Destructor
    */
    virtual ~CIpsSosAOSchedulerUtils();

public: 
        
    /**
    * checks is mailbox allowed to connect from settings
    * NOTE: not used in FSEmail 2.0
    */
    TBool CanConnect();
        
    /**
    * Calculates seconds to next interval time according to
    * mailbox settings
    */
    TIpsSosAOConnectionType SecondsToNextMark( 
        TTimeIntervalSeconds& aSeconds );
    
private:

    /**
    * Constructor
    */
    //<Qmail>
    CIpsSosAOSchedulerUtils(CIpsSosAOSettingsHandler& aSettings);
    //</Qmail>
    /**
    * Symbian 2-phase constructor
    */
    void ConstructL();
	//<Qmail>
    /**
    * gets time values needed to do cheduling
    */
    void PrepareScheduling(
        TTime& aClock,
        TTimeIntervalSeconds& aHome,
        TTimeIntervalSeconds& aStart,
        TTimeIntervalSeconds& aStop );

    /**
    * Checks if now is an allowed time to connect
    */
    TBool CanConnect(
        const TTime& aClock,
        TTimeIntervalSeconds& aHome,
        TTimeIntervalSeconds& aStart,
        TTimeIntervalSeconds& aStop );
                        
    /**
    * performs connection checks based on day
    */
    TIpsSosAOValidConnectionDay GetValidConnectionDay( 
        const TTime& aClock );
        
    /**
    * Gets next day
    */
    TDay GetNextDay( 
        const TDay aToday, 
        const TInt aNth = 1 );
              
    /**
    * Gets previous day
    */
    TDay GetPrevDay( 
        const TDay aToday, 
        const TInt aNth = 1 );
        
    /**
    * Checks if given time in sync window
    */
    TBool IsValidTimeToConnect( 
        const TIpsSosAOValidConnectionDay aValidDay,
        TTimeIntervalSeconds& aHome,
        TTimeIntervalSeconds& aStart,
        TTimeIntervalSeconds& aStop );

    /**
    * Creates hometime with hours, minutes and seconds only
    */
    TDateTime AdjustHomeTime( 
        const TTime& aClock );
            
    /**
    * Converts time to seconds
    */
    void Times2Seconds(
        const TTime& aClock,
        TTimeIntervalSeconds& aHome,
        TTimeIntervalSeconds& aStart,
        TTimeIntervalSeconds& aStop );
                
    /**
    * Creates valid start and stop times
    */
    void OrganizeStartAndStopTimes(
        TTimeIntervalSeconds& aHome,
        TTimeIntervalSeconds& aStart,
        TTimeIntervalSeconds& aStop );
                    
    /**
    * How many seconds to next connection/disconnection
    */
    void CalculateSecondsToNextMark(
        const TIpsSosAOConnectionType aConnectionStatus,
        TTimeIntervalSeconds& aSeconds,
        const TTime& aClock,
        const TTimeIntervalSeconds& aHome,
        const TTimeIntervalSeconds& aStart,
        const TTimeIntervalSeconds& aStop );

    /**
    * How many seconds to next day we are allowed to connect
    */
    TTimeIntervalSeconds CalcSecsToNextScheduledDay(
        const TTime& aClock, 
        const TTimeIntervalSeconds& aHome,
        const TBool aScheduledDay );

    /**
    * Sub routine for CalculateSecondsToNextMark
    */
    void CalcSecsToMark(
        TTimeIntervalSeconds& aSeconds,
        const TTime& aClock,
        const TTimeIntervalSeconds& aHome,
        const TTimeIntervalSeconds& aBonus,
        const TBool aScheduledDay );

    /**
    * Checks is given day selected to be synced
    */
    TBool IsDaySelected( const TUint aDay ) const;
    //</Qmail>
private:

    
    //<Qmail>
    CIpsSosAOSettingsHandler& iSettings;
    //</Qmail>
    };


#endif /*IPSSOSAOSCHEDULERUTILS_H*/
