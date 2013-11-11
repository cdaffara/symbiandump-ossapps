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
* Description:  This file contains the header file of the CPECallData object
*
*/


#ifndef CPETIMER_H
#define CPETIMER_H

//  INCLUDES
#include <e32base.h>
#include <pevirtualengine.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
class MPEPhoneModelInternal;

/**
*  Offers timer service to the phone engine modules 
*  
*
*  @lib PhoneEngineBase.lib
*  @since Series60_2.7
*/
NONSHARABLE_CLASS( CPETimer ) : public CActive
    {
    public: //Constructors and descructor
        
        /**
        * Two-phased constructor.
        */
        static CPETimer* NewL( MPEPhoneModelInternal& aModel );

        /**
        * Destructor.
        */
        ~CPETimer();

    public:    //New functions
        /**
        * Gets callId
        * @since Series60_2.7
        * @param aCallId is origival callId which is going to be redialed
        */
        void GetCallId( TInt& aCallId ) const;

        /**
        * Resets Timer
        * @since Series60_2.7
        */
        void ResetTimer();

        /**
        * Starts Timer
        * @since Series60_2.7
        * @param aSenconds includes timer delay
        * @param aCallId the identification number of the response message
        * @param aResponseMessage the identification number of the response message
        * @return possible error code
        */
        TInt StartTimer( 
            const TTimeIntervalMicroSeconds32 aSeconds, 
            const MEngineMonitor::TPEMessagesFromPhoneEngine aResponseMessage, 
            const TInt aCallId );

    protected: // From CActive
        /**
        * Cancels asyncronous request(s).
        * @since Series60_2.7
        */
        void DoCancel();

        /**
        * Informs object that asyncronous request is ready.
        * @since Series60_2.7
        */
        void RunL();

    private:
        
        /**
        * C++ default constructor.
        */
        CPETimer( MPEPhoneModelInternal& aModel );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    
        // MPEPhoneModelInternal is used to message sending.
        MPEPhoneModelInternal& iModel;
        // RTimer object 
        RTimer iTimer;
        // Responce message
        MEngineMonitor::TPEMessagesFromPhoneEngine iResponseMessage;
        // The identification number of the call( it is used to rerouting responce message to the right call )
        TInt iCallId;
};

#endif // CPETIMER_H

// End of File
