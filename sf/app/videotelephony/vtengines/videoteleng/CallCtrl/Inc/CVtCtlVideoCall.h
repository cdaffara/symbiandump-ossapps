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
* Description:  Video call abstraction
*
*/



#ifndef CVTCTLVIDEOCALL_H
#define CVTCTLVIDEOCALL_H

//  INCLUDES
#include <etelmm.h>
#include "MVtCtlCallControl.h"
#include "MVtCtlCallStatusObserver.h"


// FORWARD DECLARATIONS
class CVtCtlCallStatusMonitor;

// CLASS DECLARATION

/**
*  Video call abstraction
*
*  @lib CSCallCtrl
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtCtlVideoCall ) : 
    public CBase,
    public MVtCtlCallStatusObserver
    {
    public:
        
        /**
        * Call information
        */
        struct TVtCtlCallInfo
            {
            // Session state
            MVtCtlCallControl::TVtCtlState     iStatus;
            // Previous session state
            MVtCtlCallControl::TVtCtlState     iPreviousStatus;
            // Session id
            TVtCtlSessionId                    iId;
            // Session direction
            MVtCtlCallControl::TVtCtlDirection iDirection;
            };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aDataLine line subsession
        * @param aId session id
        * @param aObserver call status observer
        */
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        static CVtCtlVideoCall* NewL( 
            RMobileLine& aDataLine,
            TVtCtlSessionId aId,
            MVtCtlCallStatusObserver& aObserver
            );

        /**
        * Destructor.
        */
        virtual ~CVtCtlVideoCall();

    public: // New functions
        
        /**
        * Returns call info
        * @return reference to call info
        */
        const CVtCtlVideoCall::TVtCtlCallInfo& CallInfo() const;

        /**
        * Returns Etel call info
        * @return call info
        */
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        const RMobileCall::TMobileCallInfoV1& MobileInfo() const;

        /**
        * Opens call subsession and starts monitoring call status.
        * @param aCallName call name
        */
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        void InitializeCallL( RMobileCall& aCall );

        /**
        * Updates call information from ETel
        * @return Symbian OS error code
        */
        TInt UpdateCallInfo();

        /**
        * Returns call duration from Etel
        * @param aDuration duration
        * @return Symbian OS error code
        */
        TInt GetDuration( TVtCtlDuration& aDuration );
        
        /**
        * Obtains video call state from the server and returns it.
        * @param aState updated call state
        * @return Symbian OS error code
        * @since S60 3.1
        */
        TInt GetCallState( MVtCtlCallControl::TVtCtlState& aState );

    protected:  // From MVtCtlCallStatusObserver
        
        /**
        * @see MVtCtlCallStatusObserver::HandleCallStatusChangeL
        */
        virtual void HandleCallStatusChangeL( 
            TVtCtlSessionId aId,
            MVtCtlCallControl::TVtCtlState aStatus,
            MVtCtlCallControl::TVtCtlState aPreviousStatus );

    private:

        /**
        * C++ constructor.
        * @param aDataLine data line
        * @param aId session id
        * @param aObserver observer
        */
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        CVtCtlVideoCall( 
            RMobileLine& aDataLine, 
            TVtCtlSessionId aId,
            MVtCtlCallStatusObserver& aObserver );

    private:    // Data

        // Call information
        TVtCtlCallInfo iCallInfo;

        // Etel call
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        RMobileCall* iCall;

        // Data line
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        RMobileLine& iLine;

        // Etel call information
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        RMobileCall::TMobileCallInfoV1 iMobileCallInfo;
        
        // Call status monitor
        CVtCtlCallStatusMonitor* iStatusMonitor;

        // Call status observer
        MVtCtlCallStatusObserver& iObserver;
    };

#endif      // CVTCTLVIDEOCALL_H   
            
// End of File
