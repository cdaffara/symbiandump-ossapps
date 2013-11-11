/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file for CPENetworkRegistrationStatusMonitor class
*
*/


#ifndef CPENETWORKREGISTRATIONSTATUSMONITOR_H
#define CPENETWORKREGISTRATIONSTATUSMONITOR_H

//INCLUDES
#include "cpepubsubmonitor.h"

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class MPEPhoneModelInternal;

// CLASS DECLARATION

/**
*  CPENetworkRegistrationStatusMonitor
*  Not exposed externally
*  Monitors the network registration status P&S key for changes
*  and retrieves current settings
*
*  @since Series60_4.0
*/
NONSHARABLE_CLASS( CPENetworkRegistrationStatusMonitor ) 
    : public CPEPubSubMonitor
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CPENetworkRegistrationStatusMonitor* NewL( MPEPhoneModelInternal& aModel );
               
    protected:
        /**
        * C++ default constructor.
        */    
        CPENetworkRegistrationStatusMonitor( MPEPhoneModelInternal& aModel );

        /**
        * Second-phase constructor.
        */
        void ConstructL();    
        
    protected:
        /**
        * Defines the actions that take place when a change indication
        * is received. Most common approach would be to save the data to internal
        * data store and send a message via phone model if neccessary.
        */  
        virtual void UpdateL();
        
    };

#endif  //  CPENETWORKREGISTRATIONSTATUSMONITOR_H
