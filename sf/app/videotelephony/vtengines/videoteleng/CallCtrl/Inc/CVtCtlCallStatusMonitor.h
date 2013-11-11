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
* Description:  Monitors call status.
*
*/



#ifndef CVTCTLCALLSTATUSMONITOR_H
#define CVTCTLCALLSTATUSMONITOR_H

//  INCLUDES

#include "MVtCtlCallControl.h"
#include <e32base.h>
#include <etelmm.h>

/**
* Array for resolving call status (TMobileCallStatus) transition validity.
* Transition to lower value is not possible with exception to 0 priority
* (always possible to go to idle state). Initial state is EStatusUnknown.
*/
static const TInt KVtCtlStatusTransitionPriorities[] =
    { 
      1, //    EStatusUnknown
      0, //    EStatusIdle, possible from any
      2, //    EStatusDialling
      2, //    EStatusRinging
      3, //    EStatusAnswering
      3, //    EStatusConnecting
      4, //    EStatusConnected
      5, //    EStatusDisconnecting          
      // Status values below are not valid in video calls => should not happen.
      5, //    EStatusDisconnectingWithInband      
      0, //    EStatusReconnectPending
      0, //    EStatusHold
      0, //    EStatusWaitingAlternatingCallSwitch
      0, //    EStatusTransferring
      0  //    EStatusTransferAlerting
    };

// FORWARD DECLARATIONS
class MVtCtlCallStatusObserver;

// CLASS DECLARATION

/**
*  Monitor for call status
*
*  @lib CSCallCtrl
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtCtlCallStatusMonitor ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * C++ constructor.
        * @param aCall call subsession
        * @param aObserver call status observer
        */
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        CVtCtlCallStatusMonitor( 
            RMobileCall& aCall,
            MVtCtlCallStatusObserver& aObserver );

        /**
        * Destructor.
        */
        virtual ~CVtCtlCallStatusMonitor();

    public: // New functions
        
        /**
        * Starts monitoring
        */
        void Start();

    private:  // From CActive

        /**
        * Cancel monitoring.
        */
        virtual void DoCancel();

        /**
        * Handles call status change.
        */
        virtual void RunL();

        /**
        * Handles leave in RunL.
        * @param aError error code
        * @return KErrNone
        */
        virtual TInt RunError( TInt aError );

    public: // New functions
        
        /**
        * Maps Etel status to internal state
        * @param aStatus mobile call status
        * @return session state, see TVtCtlState
        */
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        static MVtCtlCallControl::TVtCtlState MapState(
            const RMobileCall::TMobileCallStatus aStatus );
                
        /**
        * Checks if call state transition specified by arguments is legal.
        * @param aNewState proposed new call state
        * @param aCurrentState current call state
        * @return ETrue if transition is valid
        */
        static TBool IsLegalNewState(
            const RMobileCall::TMobileCallStatus aNewState,
            const RMobileCall::TMobileCallStatus aCurrentState );
                
    private:    // Data

        // Data call
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        RMobileCall& iCall;
        
        // Observer for call status changes
        MVtCtlCallStatusObserver& iObserver;

        // Call status and previous status
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        RMobileCall::TMobileCallStatus iCallStatus;
        RMobileCall::TMobileCallStatus iPreviousStatus;
    };

#endif      // CVTCTLCALLSTATUSMONITOR_H

// End of File
