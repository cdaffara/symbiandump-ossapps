/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Accessory mode.
*
*/


#ifndef CTELDMAUDIOACCESSORYLISTENER_H
#define CTELDMAUDIOACCESSORYLISTENER_H

//  INCLUDE FILES   
#include <MTelephonyAudioRoutingObserver.h>
#include "mteldmaccessory.h"

// FORWARD DECLARATIONS

//  CLASS DEFINITIONS 
class CTelephonyAudioRouting;

/**
* Class for listening accessory mode and routing changes. 
*
* @lib telephonydevicemode.dll
* @since 3.1
*/
NONSHARABLE_CLASS( CTelDMAudioAccessoryListener ): public CBase,
                          public MTelDMAccessory,
                          public MTelephonyAudioRoutingObserver
    {
    public: // constructor & destructor

        /** 
        * Two phase constructor 
        */
        static CTelDMAudioAccessoryListener* NewL();

        // Destructor
        virtual ~CTelDMAudioAccessoryListener( );


    public: // from MTelDMAccessory

        /**
        * See MTelDMAccessory
        */
        TBool IsAccessoryAttached();
        
        /**
        * See MTelDMAccessory
        */
        TBool IsAnyActiveAccessory();
    
    public: //from  MTelephonyAudioRoutingObserver 
    
        void AvailableOutputsChanged( CTelephonyAudioRouting& 
                                                aTelephonyAudioRouting);
        
        void OutputChanged( CTelephonyAudioRouting& 
                                        aTelephonyAudioRouting);
        
        void SetOutputComplete( CTelephonyAudioRouting& 
                                            aTelephonyAudioRouting, 
                                            TInt aError); 
    private: // Constructor
        
        /** c++ constructor */
        CTelDMAudioAccessoryListener();

        /** 2nd phase constructor */
        void ConstructL();
        
        
    private: 
    
        // Array of available outputs         
        RArray<CTelephonyAudioRouting::TAudioOutput> iOutputArray;
        
        // Current accessory mode.
        TBool iIsActiveAccessory;
        
        // Audio routing API 
        CTelephonyAudioRouting* iAudioRouting;
        
    };
    
#endif // CTELDMAUDIOACCESSORYLISTENER_H

// End of file
