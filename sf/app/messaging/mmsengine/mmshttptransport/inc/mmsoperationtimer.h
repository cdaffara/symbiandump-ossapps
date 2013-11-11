/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class implements a timer used when waiting for information from WAP GW
*
*/



#ifndef CMMSOPERATIONTIMER_H
#define CMMSOPERATIONTIMER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MMmsTransportObserver;

// CLASS DECLARATION

/**
*   Class implementing timer
*/
NONSHARABLE_CLASS( CMmsOperationTimer ):public CTimer
    {
    public:  // Constructors and destructor
            
        /**
        * Two-phased constructor.
        */
        static CMmsOperationTimer* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMmsOperationTimer();

    public: // New functions
            
        /**
        * Start the timer.
        * @param aRequester Caller object
        * @param aTimerValue Value for the timer
        */
        void Start( MMmsTransportObserver* aRequester, const TInt aTimerValue );
        
        /**
        * Pauses the timer.
        */
        void Pause();

        /**
        * Continues the timer.
        */
        void Continue();

    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
        /**
        * Stops the timer.
        */
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CMmsOperationTimer();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Active object completion
        */
        void RunL();
      
        // By default, prohibit copy constructor
        CMmsOperationTimer( const CMmsOperationTimer& );
        
        // Prohibit assignment operator
        CMmsOperationTimer& operator= ( const CMmsOperationTimer& );

    public:     // Data
    
    protected:  // Data

    private:    // Data
                
        TTime   iStartTime;
        TTime   iEndTime;
        TTime   iCurrentTime;
        TTimeIntervalMicroSeconds iTimeLeft;
        TTimeIntervalMicroSeconds iOriginalTimerValue;

        MMmsTransportObserver* iRequester;
 
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // CMMSOPERATIONTIMER_H   
            
// End of File
