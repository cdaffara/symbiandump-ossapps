/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPESupplementaryServicesMonitor object
*
*/


#ifndef CPESUPPLEMENTARYSERVICESMONITOR_H
#define CPESUPPLEMENTARYSERVICESMONITOR_H

//INCLUDES
#include <e32base.h>
#include <pevirtualengine.h>
#include <mccessobserver.h>
 
// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class MPEMessageSender;
class MPEDataStore;

// CLASS DECLARATION

/**
* Monitors Supplementary Services
*
* @lib CallHandling.lib
* @since Series60_4.0
*/
class CPESupplementaryServicesMonitor  
        : public MCCESsObserver,
          public CBase
    {
    public:    //Constructors and descructor

        /**
        * C++ default constructor.
        */
        CPESupplementaryServicesMonitor( 
            MPEMessageSender& aOwner,
            MPEDataStore& aDataStore );
        
        /**
        * Destructor.
        */
        virtual ~CPESupplementaryServicesMonitor();

    public:
                                        
        /**
        From MCCESsObserver
        */
        IMPORT_C void BarringEventOccurred( const MCCESsObserver::TCCESsBarringEvent aBarringEvent );

        /**
        From MCCESsObserver
        */
        IMPORT_C void CLIEventOccurred( const MCCESsObserver::TCCESsCLIEvent aCallLineEvent );

        /**
        From MCCESsObserver
        */
        IMPORT_C void CallForwardEventOccurred( 
                            const MCCESsObserver::TCCESsCallForwardEvent aCallForwardEvent,
                            const TDesC& aRemoteAddress );

        /**
        From MCCESsObserver
        */
        IMPORT_C void CallCugEventOccurred( const MCCESsObserver::TCCESsCugEvent aCugEvent );

        /**
        From MCCESsObserver
        */
        IMPORT_C void NotifyCurrentActiveALSLine( TInt aLine );

    private:
        // Owner of this object
        MPEMessageSender& iOwner;
        // Data store
        MPEDataStore& iDataStore;

     };
#endif // CPESUPPLEMENTARYSERVICESMONITOR_H

// End of File
