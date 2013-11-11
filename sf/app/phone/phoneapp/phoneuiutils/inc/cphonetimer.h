/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    Utility class for CTimer object
*
*/


#ifndef CPHONETIMER_H
#define CPHONETIMER_H

//  INCLUDES
#include <e32base.h>
#include "mphonetimer.h"

// CLASS DECLARATION

/**
*  Utility class for timer 
*/
class CPhoneTimer : public CTimer
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPhoneTimer* NewL( 
            TInt aPriority = CActive::EPriorityStandard );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneTimer();

    public: // New functions
        
        /**
        * The Callback function is called after the interval
        * All former request will be canceled first
        */
        IMPORT_C void After( 
            TTimeIntervalMicroSeconds32 anInterval,
            TCallBack aCallBack );

        /**
        * Use the maxin class to notify the timer
        * All former request will be canceled first
        */
        IMPORT_C void After( 
            TTimeIntervalMicroSeconds32 anInterval, 
            MPhoneTimer* aObserver );

        /**
        * Cancel the timer if needed
        */
        IMPORT_C void CancelTimer();

    protected:  // Functions from base classes
        
        /**
        * From CTimer::RunL()
        */
        virtual void RunL();

        /**
        * From CTimer::DoCancel()
        */
        virtual void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CPhoneTimer( TInt aPriority );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CPhoneTimer( const CPhoneTimer& );

        // Prohibit assigment operator
        CPhoneTimer& operator=( const CPhoneTimer& );

    private:    // Data

        // Used by After( anInterval, aCallBack )
        TCallBack iCallBack;

        // Used by After( anInterval, aObserver )
        MPhoneTimer* iTimerObserver;
    };

#endif      // CPhoneTimer_H 
            
// End of File
