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
* Description:  Manual call control handler
*
*/


#ifndef CPEMANUALCALLCONTROLHANDLER_H
#define CPEMANUALCALLCONTROLHANDLER_H

//  INCLUDES
#include <cphcltcommandhandler.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CPEMessageHandler;
class MPECallHandling;
class MPEDataStore;

// CLASS DECLARATION

/**
*  Handles Manual call control sequences.
*
*  @lib phoneenginebase.dll
*  @since Series60_5.0
*/
NONSHARABLE_CLASS( CPEManualCallControlHandler ) : public CBase
    {
    public:  // Destructor

        /**
        * Destructor.
        */
        virtual ~CPEManualCallControlHandler();
        
        /**
        * Two-phased constructor.
        */
        static CPEManualCallControlHandler* NewL( 
                MPECallHandling& aCallHandling, 
                CPEMessageHandler& aMessageHandler,
                MPEDataStore& aDataStore );

    public: // New functions

        /**
        * Handles Chld AT message.
        * @since Series60_5.0
        * @param aChldCommand Chld message
        * @param aCallNo Call Nro.
        */
        void HandleChldL( 
            const TPhCltChldCommand aChldCommand,
            const TUint aCallNo );

        /**
        * Handles Going private in Conference Call
        * @since Series60_5.0
        * @return possible error code from the CallHandling subsystem.
        */
        TInt HandleGoOneToOne();
            
        /**
        * Release held calls or reject a waiting call.
        */
        TInt HandleChldZero();
        
        /**
        * Release all active calls and accept other (held or waiting) call.
        */
        TInt HandleChldOne();
        
        /**
        * Release specific active call or accept specific waiting call.
        */
        void HandleChldOneXL( TInt aCallNo );
        
        /**
        * Place all active calls on hold(SWAP) and accept the other 
        * (held or waiting) call.
        */
        TInt HandleChldTwo();
        
        /**
        * Hold all active calls except specific call.
        */
        void HandleChldTwoXL( TInt aCallNo );
        
        /**
        * Add a held call to the conversation.
        */
        TInt HandleChldThree();
        
        /**
        * Connect held and active call each other. Locally both calls are disconnect
        * (Explicit call transfer).
        */
         TInt HandleChldFour();

         /**
         * Fetch call id using given call index
         */
         TInt CallIdByIndexL( const TInt& aCallIndex );
        
        
    public: // Functions from base classes
        // None.        
    protected:  // New functions
        // None.
    private:  // Functions from base classes
        // None.

    private: //Constructors

        /**
        * C++ default constructor.
        */
        CPEManualCallControlHandler( MPECallHandling& aCallHandling, 
                CPEMessageHandler& aMessageHandler,
                MPEDataStore& aDataStore );
    
    protected:  // Data
        // None.
    private:    // Data
        // MPECallHandling handles call related commands to CallHandling subsystem.
        MPECallHandling& iCallHandling;
        // MEngineMonitor handles interface structures to phone application
        CPEMessageHandler& iMessageHandler;
        // reference to data store
        MPEDataStore& iDataStore;
    public:     // Friend classes
        // None.
    protected:  // Friend classes
        // None.
    private:    // Friend classes
        // None.

    };

#endif      // CPEMANUALCALLCONTROLHANDLER_H   
            
// End of File
