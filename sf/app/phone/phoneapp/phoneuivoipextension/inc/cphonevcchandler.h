/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Voice call continuity (VCC)-specific class.
*
*/

#ifndef C_CPHONEVCCHANDLER_H
#define C_CPHONEVCCHANDLER_H

// INCLUDES
#include <telmenuextension.h>
#include <vccuipspropertylistener.h>

// FORWARD DECLARATIONS
class MPhoneStateMachine;
class MPhoneViewCommandHandle;
class CPhoneTimer;
class CEikMenuPane;

// CLASS DECLARATION
/**
*  VoIP vcc-specific class
*/
NONSHARABLE_CLASS( CPhoneVccHandler ): public CBase, 
        public MVccPsPropertyListenerObserver
    {
    public:      

        /**
        * Destructor.
        */
        virtual ~CPhoneVccHandler();

        /**
        * Creates the VCC-specific VoIP class
        * @param aStateMachine: a container of state objects
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneVccHandler
        */
        static CPhoneVccHandler* NewL( 
            MPhoneStateMachine& aStateMachine,
            MPhoneViewCommandHandle& aViewCommandHandle );
      
        /**
        * StartManualHandover
        * @param aCommand a command parameter
        */
        void StartManualHandoverL( TInt aCommand );
        
        /**
        * Adds Manual Handover menu item to menu.
        * @param aArray Active call array.
        * @param aResourceId Menu bar resource identifier.
        * @param aMenuPane Menu pane.
        */
        void AddHandoverMenuItemL( 
            RArray<CTelMenuExtension::TCallInfo>& aArray, 
            TInt aResourceId,
            CEikMenuPane& aMenuPane );
         
    protected:

        CPhoneVccHandler( 
            MPhoneStateMachine& aStateMachine,
            MPhoneViewCommandHandle& aViewCommandHandle );
        
        void ConstructL();
        
    private:        
     
        /**
        * StartHandoverNoteTimerL
        */        
        void StartHandoverNoteTimerL();
       
        /**
        * CancelHandoverNoteTimer if running
        */ 
        void CancelHandoverNoteTimerL();
        
        /**
        * Callback function for launching Handover wait note
        * This is called when HO note timer timeout is
        * reached
        * @param aAny - not used
        */
        static TInt HandoverNoteTimerCallbackL( TAny* aAny );
        
        /**
        * StartHandoverNoteTimerL
        */ 
        void RemoveHandoverNoteL();
        
        /**
        * HandoverInProgressNote
        * @param aCommand a command parameter
        */
        void HandoverInProgressNoteL( TInt aCommand );

        // From MVccPsPropertyListenerObserver        
        /**
        *  Gets called when PS key is changed
        * @param aKeyId 
        * @param aValue 
        **/
        void VccPropertyChangedL( const TUint aKeyId,
                                    const TInt aValue );
        
        /**
        * returns notification tone mode (off=0 or on=1), or 0 in error situations
        */            
         TInt GetHoNotifToneModeL();
     
         /**
         * GetVccServiceIdL
         */             
         TInt GetVccServiceIdL();
         
    private: // Data
                
        /** 
         * Interface for setting and getting active state
         */
        MPhoneStateMachine& iStateMachine;
        
        /** 
         * Reference to the Phone UI View command handle
         */
        MPhoneViewCommandHandle& iViewCommandHandle;
        
        // Timer.
        CPhoneTimer* iHandoverNoteTimer;
        
        // Handover is ready
        TBool iVccHoReady;
        
        // Handover not available
        TBool iVccUnavailable;
        
        // Handover allowed WLAN to GSM
        TBool iVccHoAllowedToGsm;
        
        // Handover allowed GSM to WLAN
        TBool iVccHoAllowedToWlan;
        
        // Handover not allowed if multicall
        TBool iNoHoIfMultiCall;
        
        // Listens for the hand-over commands
        CVccUiPsPropertyListener* iPropListener; 
        };

#endif // C_CPHONEVCCHANDLER_H

// End of File
