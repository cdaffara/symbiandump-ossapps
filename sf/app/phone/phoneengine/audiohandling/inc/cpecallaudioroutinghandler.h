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
* Description:  This file contains the header file of the CPECallAudioRoutingHandler class.
*
*/


#ifndef CPECALLAUDIOROUTINGHANDLER_H
#define CPECALLAUDIOROUTINGHANDLER_H

// INCLUDES
#include <e32base.h>
 
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
class CPECallAudioRoutingHandler : public CBase 
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CPECallAudioRoutingHandler* NewL( CPEAudioData& aOwner );
        
        /**
        * Destructor.
        */
        ~CPECallAudioRoutingHandler();

    private:

        /**
        * C++ default constructor.
        */
        CPECallAudioRoutingHandler( CPEAudioData& aOwner );

    public:
    
        /**
        * Audio output is set.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        TInt HandleAudioRoutingCallInit( TBool aVideoCall );
        /**
        * Available audio outputs have changed.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        TInt HandleAudioRoutingAvailableChanged();
        
        /**
        * Audio route preference value have changed.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */        
        TInt HandleAudioRoutePreferenceChanged();
        
        /**
        * Answer to handset operation status is updated.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        void SetAnswerToHandset( TBool aStatus );
        
    private: 
    
        // Owner of this object
        CPEAudioData& iOwner;
        
        // AnswerToHandset
        TBool iAnswerToHandset;
        
        // Video call status
        TBool iAnyVideoCall;
    };

#endif // CPECALLAUDIOROUTINGHANDLER_H

// End of File
