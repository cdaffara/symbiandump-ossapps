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
* Description:  Monitors new calls on data line.
*
*/



#ifndef CVTCTLLINEMONITOR_H
#define CVTCTLLINEMONITOR_H

//  INCLUDES
#include <e32base.h>
#include <etelmm.h>

// FORWARD DECLARATIONS

class MVtCtlLineEventObserver;

// CLASS DECLARATION

/**
*  Monitor for new calls
*
*  @lib CSCallCtrl
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtCtlLineMonitor ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        * @param aLine line subsession
        * @param aObserver line event observer
        */
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        CVtCtlLineMonitor( 
            RMobileLine& aLine,
            MVtCtlLineEventObserver& aObserver);

        /**
        * Destructor.
        */
        virtual ~CVtCtlLineMonitor();

    public: // New functions
        
        /**
        * Starts monitoring
        */
        void Start();

    private:  // Functions from CActive

        /**
        * Cancel monitoring.
        */
        virtual void DoCancel();

        /**
        * Handles new call on line.
        */
        virtual void RunL();
                
        /**
        * Error handler for leave in RunL
        * @param aError error code
        * @return KErrNone
        */
        virtual TInt RunError( TInt aError );

    private:    // Data

        // Data line
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        RMobileLine& iLine;
        
        // Observer for new calls on line
        MVtCtlLineEventObserver& iObserver;

        // Call name
        TName        iName;

        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        RMobileCall::TMobileCallStatus iLastLineStatus;
        RMobileCall::TMobileCallStatus iLineStatus;
    };

#endif      // CVTCTLLINEMONITOR_H   

// End of File
