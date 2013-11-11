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
* Description:  Header file for CTelDMCommandHandler class.
*
*/


#ifndef CTELDMCOMMANDHANDLER_H
#define CTELDMCOMMANDHANDLER_H

//  INCLUDE FILES   

#include    <e32base.h>
#include    <aknkeylock.h>  
#include    "mteldmcallstateobserver.h"

// FORWARD DECLARATIONS

//  CLASS DEFINITIONS 

class MTelDMAcessory;
class MPhoneDeviceModeObserver;
class MTelDMAccessory;
class CTelDMCallStateListener;
/**
* Class for command 
*
* @lib telephonydevicemode.dll
* @since 3.1
*/
NONSHARABLE_CLASS( CTelDMCommandHandler ): public CBase,
                            public MTelDMCallStateObserver
    {
    public: // constructor & destructor

        /** 
        * Two phase constructor 
        */
        static CTelDMCommandHandler* NewL( MPhoneDeviceModeObserver& aCallHandler, 
                                            MTelDMAccessory& aAccessory );

        // Destructor
        virtual ~CTelDMCommandHandler();

    public: 
    
        enum TCommands
            {
            EGripOpen,
            EGripClose
            };
    
        void HandleCommand( TCommands aCommand );
        
    public: // from MTelDMCallStateObserver
        
        /** See MTelDMCallStateObserver.h */
        void CallStateChangedL( TInt aCallState );
    
    
    private: // Constructor
        
        /** c++ constructor */
        CTelDMCommandHandler( MPhoneDeviceModeObserver& aCallHandler, 
                                MTelDMAccessory& aAccessory );

        /** 2nd phase constructor */
        void ConstructL();
        
    private:
    
        /**
        * Ends calls
        */
        void EndCalls();
        
        /**
        * Offer key lock query.
        */
        void OfferKeyLock();
        
        /**
        * Check emergency call
        * @return status of emergency call
        */
        TBool IsEmergencyCall() const;
        
        /**
        * Check soft reject
        * @return status of soft reject operation
        */
        TBool IsSoftRejectOngoing() const;
        
    private: 
    
        // Reference to device mode observer
        MPhoneDeviceModeObserver& iCallHandler;
        
        // Reference to accessory listene
        MTelDMAccessory& iAccessory; 
        
        // For keypad locking.
        RAknKeyLock iKeyLock;
        
        // Is grip open ?
        TBool iGripOpen;
        
        // Grip open when ringing state ?
        TBool iGripOpenOnRingingState; 
        
        // Call state
        TInt iCallState;
        
    };
    
#endif // CTELDMCOMMANDHANDLER_H

// end of file
