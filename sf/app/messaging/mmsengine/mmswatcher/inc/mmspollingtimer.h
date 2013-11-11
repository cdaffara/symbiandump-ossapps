/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     This creates an event at regular intervals.
*
*/



#ifndef CMMSPOLLINGTIMER_H
#define CMMSPOLLINGTIMER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MMsvSessionObserver;

// CLASS DECLARATION

/**
* CMmsPollingTimer creates an event at regular intervals and calls requester's
* HandleSessionEvent method. I.e. MMsvSessionObserver interface is reused here.
* NOTE: THIS CLASS IS USED ONLY IN EMULATOR ENVIRONMENT, AND ONLY IF PARTICULAR
* CONFIGURATION FILE IS FOUND AT STARTUP.
* @lib mmswatcher.lib
* @since 2.6
*/
class CMmsPollingTimer : public CTimer
    {
    public:  // Constructors and destructor
            
        /**
        * Two-phased constructor.
        */
        static CMmsPollingTimer* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMmsPollingTimer();

    public: // New functions
            
        /**
        * Start the timer.
        * @param aRequester Caller object
        * @param aTimerValue Value for the timer
        */
        void Start( MMsvSessionObserver* aRequester, const TInt aTimerValue );

    protected:  // Functions from base classes
        
        /**
        * Stops the timer.
        */
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CMmsPollingTimer();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Active object completion
        */
        void RunL();

    private:    // Data

        MMsvSessionObserver* iRequester;
        TInt iTimeout;
    };
    
#endif // CMMSPOLLINGTIMER_H

// End of File
