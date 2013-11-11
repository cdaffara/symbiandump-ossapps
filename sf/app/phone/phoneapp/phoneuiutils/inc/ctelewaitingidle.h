/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility class for waiting idle.
*
*/


#ifndef CTeleWaitingIdle_H
#define CTeleWaitingIdle_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>

class CTeleRecoverySystem;

// CLASS DECLARATION

/**
* Utility class for CTeleWaitingIdle.
* 
* @lib -
* @since S60 5.0
*/
NONSHARABLE_CLASS( CTeleWaitingIdle ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aPriority priority for the timer.
        * @return new instance of CTeleWaitingIdle.
        */
        static CTeleWaitingIdle* NewL( 
                            TInt aPriority, 
                            CTeleRecoverySystem& aRecoverySystem );
                            
        /**
        * Destructor.
        */
        virtual ~CTeleWaitingIdle();

    public: // New functions
        
        /**
        * The Callback function is called after the interval.
        *
        * @param anInterval interval.
        * @param aCallBack callback to be called after 
        *                  interval.
        */
        void StartWaitingIdleL();

    protected:  // Functions from base classes
        
        /**
        * @see CActive.
        */
        virtual void RunL();
        
        /**
        * @see CActive::DoCancel.
        */
        void DoCancel();        

    private: // Constructors

        /**
        * C++ constructor.
        */
        CTeleWaitingIdle( TInt aPriority, 
                           CTeleRecoverySystem& aRecoverySystem );

    private:    // Data
    
        // Timer callback.
        TCallBack iCallBack;
        
        // Central Repository.
        RProperty iProperty;
        
        // Recovery System.
        CTeleRecoverySystem& iRecoverySystem;
    };

#endif      // CTeleWaitingIdle_H

// End of File
