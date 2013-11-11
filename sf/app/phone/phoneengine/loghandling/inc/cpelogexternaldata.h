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
* Description:  This file contains the header file of the CPELogExternalData class.
*
*/


#ifndef    CPELOGEXTERNALDATA_H
#define    CPELOGEXTERNALDATA_H

//INCLUDES
#include <pevirtualengine.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CPELogInfo;
class CPELogHandling;
class CRepository;

// CLASS DECLARATION

class CPELogExternalData : public CBase
    {
    public:

        /**
        * Two-phased constructor
        */
        static CPELogExternalData* NewL( CPELogHandling& aOwner );
             
        /**
         * Destructor.
         */
        /*****************************************************
        *    Series 60 Customer / REPOSITORY
        *    Series 60  REPOSITORY API
        *****************************************************/                
        virtual ~CPELogExternalData();

    protected:

        /**
        * C++ default constructor.
        */
        CPELogExternalData( CPELogHandling& aOwner );

        /**
        * Constructor, second phase. 
        */
        /*****************************************************
        *    Series 60 Customer / REPOSITORY
        *    Series 60  REPOSITORY API
        *****************************************************/                
        void ConstructL();

    public:
 
        /**
        * Update call timers value.
        * @since N/A
        * @param  CPELogInfo aCallInfo, call information.
        * @return None.
        */
        void UpdateCallTimers( const CPELogInfo& aLogInfo );

        /**
        * Update line specific last call timers.
        * @since since S90 1.1
        * @param  CPELogInfo aLogInfo, call information.
        * @return None.
        */
        /*****************************************************
        *    Series 60 Customer / REPOSITORY
        *    Series 60  REPOSITORY API
        *****************************************************/                
        void UpdateLastCallTimerByLine( const CPELogInfo& aLogInfo );

        /**
        * Update the last call timer when no calls left.
        * @since since S90 1.1
        * @param  CPELogInfo aLogInfo, call information.
        * @return in multicall case duration from start of first to end of last.
        */
        /*****************************************************
        *    Series 60 Customer / REPOSITORY
        *    Series 60  REPOSITORY API
        *****************************************************/                
        TInt UpdateLastCallTimer( const CPELogInfo& aLogInfo );

        /**
        * Increase missed call's counter value.
        * @since N/A
        * @param None.
        * @return None.
        */
        /*****************************************************
        *    Series 60 Customer / REPOSITORY
        *    Series 60  REPOSITORY API
        *****************************************************/                
        void IncreaseMissedCallValue( const TInt aCallId );
        
        /**
        * Set first call start time.
        * @since N/A
        * @param: TTime aFirstCallStartTime 
        * @return: None.
        */
        void SetFirstCallStartTime( const TTime& aFirstCallStartTime );

    private:
        /**
        * Update call timers value.
        * @since  since S90 1.1
        * @param  aTimer            Timer key
        * @param  aDuration         Call duration
        * @param  aRepository       Repository to use
        * @return None.
        */
        /*****************************************************
        *    Series 60 Customer / REPOSITORY
        *    Series 60  REPOSITORY API
        *****************************************************/                
        void UpdateTimer( const TUint32 aTimer, TInt aDuration, 
            CRepository& aRepository );

    private:
        // Owner of the object.   
        CPELogHandling&  iOwner; 
        // First call start time
        TTime iFirstCallStartTime;
        // Repository
        /*****************************************************
        *    Series 60 Customer / REPOSITORY
        *    Series 60  REPOSITORY API
        *****************************************************/                
        CRepository* iRepository;
    };

#endif // CPELOGSHANDLER_H

// End of File

