/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*       Class that does processing for Phone Engine messages that are common
*       for all VoIP states. Most of the GSM states are intressed of these 
*       messages but due to the class architecture they don't have any single
*       state class to be put into. This class exists to minimize duplicate
*       code so that not every VoIP class need to implement these same handlings
*       for the same messages.
*
*/


#ifndef __CPHONEVOIPERRORMESSAGESHANDLER_H
#define __CPHONEVOIPERRORMESSAGESHANDLER_H

// INCLUDES
#include <w32std.h>
#include "cphoneerrormessageshandler.h"

// FORWARD DECLARATIONS
class MPhoneErrorMessagesHandlerRegister;
class MPhoneViewCommandHandle;
class CPhoneCustomizationVoip;

// CLASS DECLARATION

/**
* Class that does processing for Phone Engine error messages that are common
* for all protocols.
*
*/
NONSHARABLE_CLASS( CPhoneVoIPErrorMessagesHandler ): public CPhoneErrorMessagesHandler
    {
    public:
        /**
        * Creates the error handler instance
        * @param aPhoneState: active state
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneErrorMessagesHandler
        */
        static CPhoneVoIPErrorMessagesHandler* NewL(            
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneStateMachine* aPhoneStateMachine );

    public: //From MPhoneErrorMessagesHandler
        
        /**
        * Destructor.
        */
        virtual ~CPhoneVoIPErrorMessagesHandler();

        /**
        * Shows error specific notes, ie. error, warning and info notes
        * related to the given error.
        * @param aErrorInfo - Error info from Phone Engine
        */
        void ShowErrorSpecificNoteL( const TPEErrorInfo& aErrorInfo );
       
        
    public:
           
        /**
        * Shows global error note 
        * @param aResourceId Text resource id for error note
        * @param aText Text to be shown in error text
        */
        void SendGlobalErrorNoteWithTextL( TInt aResourceId, RBuf& aText );
           
        /**
        * Gets name of specific service provider
        * @param aServiceName Contains service provider name on return
        * @param aServiceId Id of the service
        */
        void GetServiceProviderNameL( RBuf& aServiceName, TUint32 aServiceId );
        
        /**
        * Shows error note that contains service name
        * @param aResourceId Text resource id for error note
        * @param aServiceId Id of the service which name will be shown on note
        */
        void ShowErrorNoteWithServiceNameL( 
            TInt aResourceId, 
            TUint32 aServiceId );
        
    protected:
        
        CPhoneVoIPErrorMessagesHandler( 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneStateMachine* aPhoneStateMachine );
            
        void ConstructL();

    private:

        /**
        * Return ETrue if voip call.
        */
        TBool IsVoIPCall( const TInt aCallId ) const;        
        
        /**
        * Handle hold and resume error notes.
        */
        void HandleHoldErrorNotesL( const TInt aCallId, TBool aHold );
        
        /**
        * Return remote info data, if available
        * @param aCallid call id
        * @param aData the returned remote info data
        */
        void GetRemoteInfoDataL( TInt aCallId, TDes& aData ) const;
        
        /**
        * Shows service registration specific error notes
        * @param aErrorCode - CCH error code  
        */
        void ShowRegistrationErrorNotesL( TInt aErrorCode );
        
        /**
         * Check if Swap operation has failed
         * @ret Return ETrue if Swap has failed
         */
        TBool HasSwapFailed() const;
            
    };

#endif // CPHONEVOIPERRORMESSAGESHANDLER_H
            
// End of File
