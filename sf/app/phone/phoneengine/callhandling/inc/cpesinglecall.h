/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPESingleCall object
*
*/


#ifndef CPESINGLECALL_H
#define CPESINGLECALL_H

//INCLUDES
#include <pevirtualengine.h>

#include "cpesinglecallobserver.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None. 

// FORWARD DECLARATIONS
class MPEMessageSender;

// CLASS DECLARATION

/**
*  Create call handling requests to Call Command object
*  @lib CallHandling.lib
*  @since Series60_5.0
*/
class CPESingleCall
        : public CPESingleCallObserver
    {
    public:
        /**
        * Destructor.
        */
        ~CPESingleCall();
        
        /**
        * Two-phased constructor.
        */
        static CPESingleCall* NewL( MPEMessageSender& aOwner );

    private:
    
        /**
        * C++ default constructor.
        */
        CPESingleCall( MPEMessageSender& aOwner );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    public:     // Overwrite CPECall::SendMessage
        /**
        * Reroutes call messages from other objects to CPEPhoneData object.
        * @param const TInt aMessage, message code of the message.
        * @return
        */            
        void SendMessage( const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage );

    public:    //New functions
    
        /**
        * Creates answer request to data call command object
        * @return possible error code
        */    
        TInt Answer();
        
        /**
        * Creates dial request to call command object
        * @param aNumber remote number    
        * @return possible error code
        */                
        virtual TInt Dial( const RMobileCall::TTelNumberC& aNumber );

        /**
        * Disables FDN checking for SIM ATK originated calls
        */  
        void DisableFDNCheck();
        
        /**
        * Creates deflect request to cce call object
        * @param aMobileAddress the address of the remote party
        * @param aDeflectType determinates the destination of the deflected call.
        */    
        void Deflect( 
            const RMobilePhone::TMobileAddress& aMobileAddress, 
            const RMobileCall::TMobileCallDeflect& aDeflectType );
        
        /**
        * Creates emergency dial request to cce call  object
        * @param aEmergencyNumber an emergency number to dial
        */        
        void DialEmergency( const TPEPhoneNumber& aEmergencyNumber );

        /**
        * Gets call control caps
        * @param aCallControlCaps is the value of the call control caps
        */    
        //void IMPORT_C GetCallControlCaps( TPECallControlCaps& aCallControlCaps );

        /**
        * Creates hold request to cce call  object
        * @return possible error code
        */    
        TInt Hold();

        /**
        * Creates resume request to cce call  object
        * @return possible error code
        */    
        TInt Resume();

        /**
        * Creates swap request to cce call  object
        * @return possible error code
        */    
        TInt Swap();

        /**
        * Creates transfer request to cce call  object
        */    
        void Transfer( const TDesC& aTransferTarget );
        
        /**
        * Returns the inband tone needed to play in state transition
        */    
        TCCPTone Tone();
        
        /**
        * Creates hangup request to cce call  object
        * @return possible error code
        */ 
        TInt HangUp();
        
        /**
        * Offers call parameters
        * @param aCallInfo the current call information.
        * @return KErrNone or possible error code from the GetMobileCallInfo method.
        */
        TInt GetCallInfo( RMobileCall::TMobileCallInfoV3& aCallInfo );

        /**
        * Sets Missed call indicator.
        * @param aMissed, Missed call indicator. 
        * @return KErrNone.
        */
        void SetMissedCall( TBool aMissed );

        /**
        * Gets Missed call indicator.
        * @param aMissedCall, Missed call indicator. 
        * @return KErrNone.
        */
        TInt GetMissedCall( TBool& aMissedCall ) const;
        
        /**
        * Get call's secure status.
        * @param None.
        * @return TBool ETrue if security is enabled, else EFalse.
        */
        TBool IsSecureCall();

        /**
        * Get call's secure specified status.
        * @param None.
        * @return TBool ETrue if secure specified
        */
        TBool SecureSpecified();
        
        /**
         * Accepts unattended transfer request.
         * @return  System wide error code.
         */
        TInt AcceptUnattendedTransfer();
        
        /**
         * Rejects unattended transfer request.
         * @return  System wide error code.
         */
        TInt RejectUnattendedTransfer();

        /**
        * Creates unattended transfer request to cce call  object
        * @param aTransferTo Target address of the transfer
        * @return System wide error code
        */   
        TInt UnattendedTransfer( const TDesC& aTransferTo );
        
        /**
         * Starts call forwarding to selected address.
         *
         * @param aIndex Address index.
         * @return System wide error code.
         */
        TInt ForwardCallToAddress( TInt aIndex );
        
    protected:
    
        // Phone information from the etel          
        RTelServer::TPhoneInfo iPhoneInfo;
        // The information of the current call 
        RMobileCall::TMobileCallInfoV3 iCallInfo;
        // Missed call indicator
        TBool iMissedCall;
        
    };

#endif // CPESINGLECALL_H

//    End of File
