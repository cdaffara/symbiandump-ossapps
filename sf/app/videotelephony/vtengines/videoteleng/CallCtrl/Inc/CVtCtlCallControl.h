/*
* Copyright (c) 2004 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Call control
*
*/



#ifndef CVTCTLCALLCONTROL_H
#define CVTCTLCALLCONTROL_H

// INCLUDES
#include <e32base.h>
#include "MVtCtlCallControl.h"
#include "MVtCtlCallStatusObserver.h"
#include <etelmm.h>

// FORWARD DECLARATIONS
class MVtCtlEventObserver;
class CVtCtlSessionContainer;

// CLASS DECLARATION

/**
*  Call control implementation.
*
*  @lib CSCallCtrl.lib
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtCtlCallControl ) : 
    public CBase, 
    public MVtCtlCallControl,
    public MVtCtlCallStatusObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aEventObserver reference to event observer
        * @param aCommandObserver reference to command observer
        * @return call control
        */
        static CVtCtlCallControl* NewL(
            MVtCtlEventObserver& aEventObserver );
        
        /**
        * Destructor.
        */
        virtual ~CVtCtlCallControl();

    public: // From MVtCtlCallControl

        /**
        * @see MVtCtlCallControl::GetDuration
        */
        virtual TInt GetDuration( 
            TVtCtlSessionId aId,
            TVtCtlDuration& aDuration );


        /**
        * @see MVtCtlCallControl::GetSessionInfoL
        */
        virtual void GetSessionInfoL( 
            TVtCtlSessionId aId,
            TDes8& aInfo );

        /**
        * @see MVtCtlCallControl::GetSessionState
        */
        virtual TVtCtlState GetSessionState( 
            TVtCtlSessionId aId ) const;

        /**
        * @see MVtCtlCallControl::GetPreviousPreviousState
        */
        virtual TVtCtlState GetPreviousPreviousState( 
            TVtCtlSessionId aId ) const;

    public: // from MVtCtlCallStatusObserver

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
        */
        CVtCtlCallControl(
            MVtCtlEventObserver& aEventObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
        * Validates that data passed as descriptor is of
        * correct type. If not leaves KErrNotSupported.
        * @param aData TVtCtlType derived class represented as descriptor
        * @param aExtension expected type
        */
        static void ValidateDataExtensionL( 
            TDesC8& aData,
            TInt aExtension );

    private:    // Data
        
        // Event observer
        MVtCtlEventObserver&    iEventObserver;

        // Container for ETel sessions
        CVtCtlSessionContainer* iContainer;
    };

#endif      // CVTCTLCALLCONTROL_H

// End of File
