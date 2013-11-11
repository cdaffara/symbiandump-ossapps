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
* Description:  VoIP-specific customization class.
*
*/


#ifndef C_CPHONECUSTOMIZATIONVOIP_H
#define C_CPHONECUSTOMIZATIONVOIP_H

// INCLUDES
#include <settingsinternalcrkeys.h>
#include <telmenuextension.h>
#include <spdefinitions.h>
#include "mphonecustomization.h"
#include "mphonebuttoncustomization.h"

// FORWARD DECLARATIONS
class MPhoneStateMachine;
class MPhoneViewCommandHandle;
class CPhoneExtensionHandler;
class MPhoneDialerController;
class CPhoneViewCustomizationVoip;
class CPhoneStateUtilsVoip;
class CPhoneUnattendedTransferCmdHandler;
class CPhoneNewCallCmdHandler;
class CEikMenuPaneItem;
class CCoeEnv;
class CPhoneCallForwardQueryHandler;

// CONSTANTS

const TInt KMaxVoIPServiceName = 100;

// CLASS DECLARATION
/**
*  VoIP-specific customization class.
*/
NONSHARABLE_CLASS( CPhoneCustomizationVoip ): public CBase, 
        public MPhoneCustomization, public MPhoneButtonCustomization
    {
    public: // Data
    
    /**
     * Features for multicall case
     */    
    enum TFeature
        {
        EFeatureNone,
        EFeatureConference,
        EFeatureTransfer
        };
    public:      

        /**
        * Destructor.
        */
        virtual ~CPhoneCustomizationVoip();

        /**
        * Creates the VoIP-specific utility class
        * @param aStateMachine: a container of state objects
        * @return an instance of class CPhoneCustomizationVoip
        */
        static CPhoneCustomizationVoip* NewL( 
            MPhoneStateMachine& aStateMachine,
            MPhoneViewCommandHandle& aViewCommandHandle );
        
// from base class MPhoneCustomization  
        
        /**
        * From MPhoneCustomization.
        * Customizes menu bar.
        * @param    aResourceId     Menu bar resource identifier.
        * @param    aMenuBar        Menu bar.
        */
        void CustomizeMenuBarL( TInt aResourceId, CEikMenuBar* aMenuBar );
        
        /**
        * From MPhoneCustomization.
        * Customizes menu pane.
        * @param    aResourceId     Menu pane resource identifier.
        * @param    aMenuPane       Menu pane.
        */
        void CustomizeMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /**
        * From MPhoneCustomization.
        * Customizes call header texts shown in a call bubble.
        * @param    aCallId             Call identifier.
        * @param    aHeaderData         Call header data to be modified.
        * @param    aInCallNumberText   In call number text (Call 1, Call 2...).
        */
        void ModifyCallHeaderTexts( TInt aCallId, 
            TPhoneCmdParamCallHeaderData* aHeaderData, 
            const TDesC& aInCallNumberText );
        
        /**
        * From MPhoneCustomization.
        * Handles custom menu commands.
        * @param aCommand Command id.
        * @return ETrue if command handled.
        */
        TBool HandleCommandL( TInt aCommand );
        
        /**
        * From MPhoneCustomization.
        * Checks is alphanumeric mode allowed for number entry/
        * dialer.
        * @return ETrue if allowed.     
        */
        TBool AllowAlphaNumericMode(); 
        
        /**
        * From MPhoneCustomization.
        * Returns resource id for soft keys.
        * @return Resource id.
        */
        TInt CustomizeSoftKeys();
        
        /**
        * From MPhoneCustomization.
        * Returns text resource id for private call header.
        * @return Resource id.
        */
        TInt CustomizeCallHeaderText();
        
        /**
        * From MPhoneCustomization.
        * Returns text resource id for busy note.
        * @return Resource id.
        */
        TInt CustomizeBusyNoteText();
        
        /**
        * From MPhoneCustomization.
        * Handles calls from number entry.
        * @return ETrue when call is handled by customization.
        *         EFalse otherwice.
        */        
        TBool HandleCallFromNumberEntryL();
        
        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Id of the message 
        * @param aCallId  The call id of the call
        * @return ETrue if message was handled
        */
        TBool HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );

        /**
        * Handles dial by unknown service id.
        * @param aNumber   Number/Address 
        */
        void HandleDialL( const TDesC& aNumber );

// from base class MPhoneButtonCustomization
        /**
        * From MPhoneButtonCustomization.
        * Customizes touch pane buttons.
        * @param aTouchPane Interface to add and dimm
        *                   bubble touch pane buttons.
        */
        void CustomizeTouchPaneButtons(
                        /*MBubbleTouchPaneInterface& aTouchPane*/ );
    protected:

        CPhoneCustomizationVoip( 
            MPhoneStateMachine& aStateMachine,
            MPhoneViewCommandHandle& aViewCommandHandle );
        
        virtual void ConstructL();
        
    private:        
        /**
        * VoIP supported check.
        */
        TBool VoIPSupportedL();
        
        /**
        * Get VoIP services
        * @param aVoipServiceIds On return, contains the list of VoIP services
        */
        void GetVoIPServiceIdsL( RIdArray& aVoipServiceIds ) const;

        /**
        * Get the service provider name
        * @param aServiceId The service, whose name is requested
        * @param aServiceName On return, contains the service name
        */
        void GetVoipServiceNameL( TServiceId aServiceId, TDes& aServiceName ) const;
        
        /**
        * Modifies menu when there is active call.
        * @param aResourceId Menu resource id.
        * @param aMenuPane Menu pane
        */
        void ModifyInCallMenuL( TInt aResourceId, CEikMenuPane& aMenuPane );
        
        /**
         * Modifies menu when there is dialer/number entry shown.
         * 
         * @param   aResourceId     Menu pane resource identifier.
         * @param   aMenuPane       Menu pane.
         */
        void ModifyNumberAcquisitionMenuL( 
            TInt aResourceId, CEikMenuPane& aMenuPane );
        
        /**
         * Inserts "Internet call" menu item to the given menu pane.
         * 
         * @param   aResourceId     Menu pane resource identifier.
         * @param   aMenuPane       Menu pane. 
         */
        void InsertInternetCallMenuItemL( TInt aResourceId, 
            CEikMenuPane& aMenuPane );
        
        /**
         * Inserts menu item for changing between alpha and numeric input 
         * modes to the given menu pane.
         * 
         * @param   aMenuPane       Menu pane. 
         */
        void InsertTextInputModeChangeMenuItemL( CEikMenuPane& aMenuPane );
        
        /**
        * Resolves active calls from phoneengine.
        * @param aArray Active call array.
        */
        void GetActiveCallArrayL( 
                RArray<CTelMenuExtension::TCallInfo>& aArray );
        
        /**
        * Resolves most active call.
        * This is used to determine which plug-in customizes menu pane 
        * and handles command.
        * @param aArray Active call array.
        */
        TInt ResolveMostActiveCallL( 
                const RArray<CTelMenuExtension::TCallInfo>& aArray );

        /**
         * Returns service id for activest call
         * by active call array.
         * @param aArray Active call array.
         * @return Service id.
         */
        TUint ServiceIdForActivestCallL(
                const RArray<CTelMenuExtension::TCallInfo>& aArray );
        
        /**
        * Checks is feature supported.
        * @since    S60 v5.1
        * @param    aFeature A feature to be checked.
        * @return   ETrue if feature supported.
        */  
        TBool IsFeatureSupported( TFeature aFeature );  
        
        /**
        * Checks is feature supported by call type.
        * E.g. Conference not supported if held voip call
        * and active CS call.
        * @since    S60 v5.1
        * @leave    Leaves if check not succeed.
        * @param    aFeature A feature to be checked.
        * @return   ETrue if feature supported.
        */  
        TBool CheckFeatureSupportByCallTypeL( 
                TFeature aFeature );

        /**
        * Shows "Enabling %U service" wait note 
        */
        void LaunchEnablingServiceNoteL();
         
        /**
        * Show global ErrorNote
        * @param aResourceId resource id to be resolved
        */
        void SendGlobalErrorNoteL( TInt aResourceId );
        
        /**
        * Show global InfoNote
        * @param aResourceId resource id to be resolved
        */
        void SendGlobalInfoNoteL( TInt aResourceId );
        
        /**
        * Fits URI to call bubble lines.
        * @param aCLIText Full caller address.
        * @param aSipURI returns first line of bubble.
        * @param aSipURIDomainPart returns second line of bubble.
        */
        void HandleURIFittingToCallBubble( const TDesC& aCLIText,
                   TDes& aSipURI, TDes& aSipURIDomainPart );
        
        /**
         * Returns VoIP state utils.
         * @return VoIP state utils.
         */
        CPhoneStateUtilsVoip& StateUtils();
        
        /**
        * Take care of actions when callsecurestatus 
        * change.
        * @param aCallId CallId of the call.
        */ 
        void HandleCallSecureStatusChangeL( TInt aCallId );
        
        /**
         * Creates menu pane item with specified command and text resource 
         * identifiers.
         * 
         * @param   aCommandId      Command identifier.
         * @param   aTextResourceId Text resource identifier.
         * @return  Menu pane item. 
         */
        CEikMenuPaneItem* CreateMenuPaneItemLC( 
            TInt aCommandId, TInt aTextResourceId ) const;
        
        /**
         * Creates a VoIP menu pane item with specified command and text resource 
         * @param   aCommandId      Command identifier.
         * @param   aTextResourceId Text resource identifier.
         * @return  Menu pane item. 
         */
        CEikMenuPaneItem* CreateMenuPaneItemLC( 
            TInt aCommandId, TServiceId aServiceId ) const;
        
        /**
        * Adds handover menu item to menu.
        * @param aResourceId menu resource
        * @param aMenuPane Menu pane.
        */
        void AddHandoverMenuItemIfNeededL( 
            TInt aResourceId, CEikMenuPane& aMenuPane );

        /**
        * Gets phone number from the phone number entry.
        * @return  Phone number
        */
        HBufC* PhoneNumberFromEntryLC() const;
        
    private: // Data
                
        /** 
         * Control environment.
         */
        CCoeEnv& iCoeEnv;
        
        /** 
         * Interface for setting and getting active state
         */
        MPhoneStateMachine& iStateMachine;
        
        /** 
         * Reference to the Phone UI View command handle
         */
        MPhoneViewCommandHandle& iViewCommandHandle;
        
        /**
         * Extension handler.
         * Own.
         */
        CPhoneExtensionHandler* iExtensionHandler;

        /**
         * Phone view customization.
         */
        CPhoneViewCustomizationVoip* iPhoneViewCustomization;
        
        /**
         * Unattended transfer command handler.
         */
        CPhoneUnattendedTransferCmdHandler* iTransferCmdHandler;
        
        /**
         * New Internet call command handler.
         */
        CPhoneNewCallCmdHandler* iNewCallCmdHandler;
        
        /**
         * Call forward handler.
         */
        CPhoneCallForwardQueryHandler* iCallForwardHandler;
        
    };

#endif // C_CPHONECUSTOMIZATIONVOIP_H

// End of File
