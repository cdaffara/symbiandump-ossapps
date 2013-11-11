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
* Description:  This file contains the header file of the CPEAudioRoutingMonitor class.
*
*/


#ifndef CPEAUDIOROUTINGMONITOR_H
#define CPEAUDIOROUTINGMONITOR_H

// INCLUDES
#include <MTelephonyAudioRoutingObserver.h>

// FORWARD DECLARATIONS
class CPEAudioData;
class CTelephonyAudioRouting;

// CLASS DECLARATION

/**
*  Monitors audio path selection
*
*  @lib audiohandling.lib
*  @since Series60_4.0
*/  
class CPEAudioRoutingMonitor : public CBase, 
                               public MTelephonyAudioRoutingObserver
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CPEAudioRoutingMonitor* NewL( CPEAudioData& aOwner );
        
        /**
        * Destructor.
        */
        ~CPEAudioRoutingMonitor();

    private:

        /**
        * C++ default constructor.
        */
        CPEAudioRoutingMonitor( CPEAudioData& aOwner );

    public: // Methods from MTelephonyAudioRoutingObserver

        /**
        * Available audio outputs have changed.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        void AvailableOutputsChanged( 
            CTelephonyAudioRouting& aTelephonyAudioRouting );
        
        /**
        * Current output has changed.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        void OutputChanged( CTelephonyAudioRouting& aTelephonyAudioRouting );
        
        /**
        * Completion message for SetOutput request.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        void SetOutputComplete( CTelephonyAudioRouting& aTelephonyAudioRouting, 
                                TInt aError );
                       
    private: 
    
        // Owner of this object
        CPEAudioData& iOwner;
    };

#endif // CPEAUDIOROUTINGMONITOR_H

// End of File
