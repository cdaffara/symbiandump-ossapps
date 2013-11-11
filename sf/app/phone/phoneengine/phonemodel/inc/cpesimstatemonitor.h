/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sim monitoring class definition
*
*/



#ifndef CPESIMSTATEMONITOR_H
#define CPESIMSTATEMONITOR_H

//  INCLUDES
#include "pevirtualengine.h"
#include <e32base.h>
#include <e32property.h>
#include <startupdomainpskeys.h>


// FORWARD DECLARATIONS
class MPEPhoneModelInternal;

// CLASS DECLARATION

/**
*  This class monitors Sim state changes from System Agent.
*
*  @lib PhoneEngine.lib
*  @since Series60_4.0
*/
NONSHARABLE_CLASS( CPESimStateMonitor ) : public CActive
    {

    public: // Constants and data types

    // Monitored Sim event given to constructor
    enum TEvent
        {
        EEventSimStatus,
        EEventSimChanged
        };

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPESimStateMonitor* NewL( 
            MPEPhoneModelInternal& aModel, 
            const TEvent aEvent );

        /**
        * Destructor.
        */
        virtual ~CPESimStateMonitor();

    public: // New functions

        /**
        * Gets Sim state.
        * @since Series60_4.0
        * @return Sim state
        */
        TPESimState SimState( ) const;

        /**
        * Completes this monitor for initialization purposes and starts monitoring
        * @since Series60_4.0
        */
        void Start();

    private: // Constructors 

        /**
        * C++ default constructor.
        */
        CPESimStateMonitor( MPEPhoneModelInternal& aModel, const TEvent aEvent );

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

    private: // New functions

         /**
        * Converts and saves Sim state
        * @since Series60_4.0
        * @param aState Sim state from System Agent
        * @return result of checking valid values.
        */
        TBool SaveSimState( const TInt aState );

        /**
        * Starts monitoring
        * @since Series60_4.0
        */
        void StartMonitoring( );

    private: // From CActive

        /**
        * Implements cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * Handles request completion event.
        */
        void RunL();

    private: // Data

        // To send messages
        MPEPhoneModelInternal&  iModel;
        // The interface to Publish And Subscribe.
        RProperty iProperty;
        // To maintain Sim property
        TInt iSimState;
        // To handle startup logic, init value ETrue
        TBool iStartUp;
        // Select Sim property of interest
        TUint iPropertyKey;
    };

#endif // CPESIMSTATEMONITOR_H

// End of File

