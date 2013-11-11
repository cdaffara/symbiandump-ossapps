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
* Description:  This file contains the header file of the CPEParserSSHandler
*                object
*
*/



#ifndef CPEPARSERSSHANDLER_H
#define CPEPARSERSSHANDLER_H

//  INCLUDES
#include <cphonegsmsshandler.h>
#include <pevirtualengine.h>
#include <nwdefs.h>
#include <mpsetrequestobs.h>

// FORWARD DECLARATIONS
class CPEMessageHandler;
class CPsetContainer;
class CPsetCallWaiting;
class CPsetCallBarring;
class CPsetCallDiverting;
class CPsetCli;
class CPsuiContainer;
class CPsuiWaitingObs;
class CPsuiBarringObs;
class CPsuiDivertObs;

class PSetCallDivertingWrapper;
class PSetCallWaitingWrapper;
class PSetWrapper;
class PsUiDivertNoteHandler;
class PsUiWaitingNoteHandler;

// CLASS DECLARATION

/**
*  Handles phone related messages from the MPEPhoneModelInternal object.
*
*  @lib phoneenginegsm
*  @since Series60 4.0
*/
NONSHARABLE_CLASS( CPEParserSSHandler ) : public CPhoneGsmSsHandler, 
                              public MPsetRequestObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CPEParserSSHandler* NewL
            (
            CPEMessageHandler& aOwner,   
            MPEPhoneModelInternal& aModel 
            );

        /**
        * Destructor.
        */
        virtual ~CPEParserSSHandler();

     public: // New functions

        /**
        * Returns RMobilePhone::TMobileService type of basic service code.
        *
        * @param aBasicCode It is basic service code.
        * @return basic service code.
        */
        RMobilePhone::TMobileService BasicService( const TDesC& aBasicCode ); 

        /**
        * changes parameter to basic service code
        */
        TBasicServiceGroups ChangeToBsc( const TDesC& aBasicCode );

    public: // Functions from base classes

        /**
        * Cancel SS string command. Method cancel command from Settings utility
        * subsytem or security module.
        *
        * @return possible error code
        */
        TInt Cancel();

        /**
        * Process barring procedures(derived from CPESsHandler).
        *
        * See comments for forwarding, they apply also in here.
        *
        * @param aAction It is action.
        * @param aBarring It contains barring procedure.
        * @param aPassword It should be barring password, if required.
        * @param aBasicCode It is basic service code.
        */
        void ProcessBarringL( TAction aAction, 
                              TBarringProcedure aBarring, 
                              const TDesC& aPassword, 
                              const TDesC& aBasicCode );

        /**
        * Process calling line identification presentation status
        * (derived from CPEGsmSsHandler).
        */
        void ProcessClipStatusL();

        /**
        * Process calling line idnetification restriction status
        * (derived from CPEGsmSsHandler).
        */
        void ProcessClirStatusL();

        /**
        * Process Calling Name Presentation (CNAP) status check.
        */
        void ProcessCnapStatusL();

        /**
        * Process connected line presentation status(derived from CPEGsmSsHandler).
        */
        void ProcessColpStatusL();

        /**
        * Process connected line restriction status(derived from CPEGsmSsHandler).
        */
        void ProcessColrStatusL();

        /**
        * Process enhanced Multi-Level Precedence and Pre-emption (eMLPP).
        *
        * @param aAction It is action.
        * @param aNumber It is directory number.
        * @param aServiceCode It is service code.
        */
        void ProcessEmlppL( TAction aAction, 
                            const TDesC& /*aNumber*/,
                            const TInt aServiceCode );

        /**
        * Process forwarding procedures(derived from CPEGsmSsHandler).
        *
        * If a parameter is not allowed for a procedure,
        * then it is empty. If a parameter is not explicitly given,
        * then it is empty. If a parameter is present, then it can
        * be assumed to contain appropriate values.
        *
        * @param aAction It is action.
        * @param aForwarding It contains forwarding procedure.
        * @param aNumber It is directory number.
        * @param aBasicCode It is basic service code.
        * @param aTimer It is timer(delay time).
        */
        void ProcessForwardingL( TAction aAction, 
                                 TForwardingProcedure aForwarding, 
                                 const TDesC& aNumber, 
                                 const TDesC& aBasicCode, 
                                 const TDesC& aTimer );
        
        /**
        * Process Multiple Subscriber Profile (MSP).
        *
        * @param aAction It is action.
        * @param aNumber It is directory number.
        * @param aServiceCode It is service code.
        */
        void ProcessMspL( TAction aAction, 
                          const TDesC& aNumber, 
                          const TInt aServiceCode );

        /**
        * Process password change(derived from CPEGsmSsHandler).
        *
        * @param aPassword It is password procedure.
        * @param aOldPassword It should be old password.
        * @param aNewPassword1 It is new password.
        * @param aNewPassword2 It is new password.
        */
        void ProcessPasswordChangeL( TPasswordProcedure aPassword, 
                                    const TDesC& aOldPassword, 
                                    const TDesC& aNewPassword1, 
                                    const TDesC& aNewPassword2 );

        /**
        * Process Support for Private Numbering Plan (SPNP).
        *
        * @param aAction It is action.
        * @param aServiceCode It is service code.
        */
        void ProcessSpnpL( TAction aAction, const TInt aServiceCode );

        /**
        * Process waiting(derived from CPEGsmSsHandler).
        *
        * @param aAction It is action.
        * @param aBasicCode It is basic service code.
        */
        void ProcessWaitingL( TAction aAction, const TDesC& aBasicCode );   

        /**
        * From MPsetRequestObs, called when request is completed  
        * so that also ui browsing is ended
        */
        void RequestComplete();
            
        /**
        * From MPsetRequestObs, called when request state changes  
        * @param aNewStatus Request status, when request finishes
        */
        void RequestStatusChanged( TInt aNewStatus );

    private: 

        /**
        * C++ default constructor.
        */
        CPEParserSSHandler( CPEMessageHandler& aOwner,  
                                MPEPhoneModelInternal& aModel); 
        
        /**
        * Connects class to the phonesettings
        */
        void ConnectToSsEngineL();

        /**
        * Create call barring observer
        */
        void CreateCBObsL();
    
        /**
        * Create call cli observer
        */
        void CreateCliObsL();          
        
        /**
        * Create call divert observer
        */
        void CreateCFObsL();

        /**
        * Create call waiting observer
        */
        void CreateCWObsL();

        /**
        * if ConnectSsToEngine leaves, this is called
        */
        static void DoClean( TAny* aAny );
        
        /**
        * deletes member variables
        */
        void ResetVariables();

        /**
        * Copies destructor. If does not fit, then leaves with
        * KErrOverflow error code.
        */
        static void CopyL( TDes& aDest, const TDesC& aSource );

    private:
        // CPEGsmMessageHandler object which owns this requester.
        CPEMessageHandler& iOwner;
        //MPEPhoneModelInternal is used to send message to the phone application
        MPEPhoneModelInternal& iModel;
        // Contains information of the Suplementary Services command.
        TPESSCommandInfo iSupplementaryServicesCommandInfo;     
        // Owned phone settings container.
        CPsetContainer* iSettings;
        // Owned phone settings ui container.
        CPsuiContainer* iObsContainer;
       
        // Owned call barring.
        CPsetCallBarring* iBarring;
    
        // Owned calling line identity.
        CPsetCli* iCli;
        
        // Call diverting. Not own.
        CPsetCallDiverting* iDivert;  
        // Call divert wrapper. Not own
        PSetCallDivertingWrapper* iCallDivertingWrapper;
        // Call diverting note handler, Own
        PsUiDivertNoteHandler* iCallDivertingHandler;
      
        // Owned call waiting. Now Own
        CPsetCallWaiting* iWaiting;
        // Call divert wrapper. Not own
        PSetCallWaitingWrapper* iCallWaitingWrapper;
        // Call diverting note handler, Own
        PsUiWaitingNoteHandler* iCallWaitingHandler;

        // Setting wrappers. Own. 
        PSetWrapper *iPsetWrapper;  
    };

#endif      // CPEPARSERSSHANDLER_H
            
// End of File
