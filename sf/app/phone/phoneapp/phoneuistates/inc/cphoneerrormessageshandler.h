/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*       for all GSM states. Most of the GSM states are intressed of these 
*       messages but due to the class architecture they don't have any single
*       state class to be put into. This class exists to minimize duplicate
*       code so that not every GSM class need to implement these same handlings
*       for the same messages.
*
*/


#ifndef __CPHONEERRORMESSAGESHANDLER_H
#define __CPHONEERRORMESSAGESHANDLER_H

// INCLUDES
#include <w32std.h>
#include "mphoneerrormessageshandler.h"

// FORWARD DECLARATIONS
class MPhoneErrorMessagesHandlerRegister;
class MPhoneViewCommandHandle;
class MPhoneStateMachine;

// CLASS DECLARATION

/**
* Class that does processing for Phone Engine error messages that are common
* for all protocols.
*
*/
class CPhoneErrorMessagesHandler : public CBase, 
                                   public MPhoneErrorMessagesHandler
    {
    public:
        /**
        * Creates the error handler instance
        * @return an instance of class CPhoneErrorMessagesHandler
        */
        static CPhoneErrorMessagesHandler* NewL( 
                MPhoneViewCommandHandle* aViewCommandHandle,
                MPhoneStateMachine* aStateMachine ); 
    
    public: //From MPhoneErrorMessagesHandler
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneErrorMessagesHandler();

        /**
        * Shows error specific notes, ie. error, warning and info notes
        * related to the given error.
        * @param aErrorInfo - Error info from Phone Engine
        */
        IMPORT_C void ShowErrorSpecificNoteL( const TPEErrorInfo& aErrorInfo );
        
    protected:      
        /**
        * By default EPOC constructor is private.
        */
        IMPORT_C CPhoneErrorMessagesHandler( 
                    MPhoneViewCommandHandle* aViewCommandHandle,
                    MPhoneStateMachine* aStateMachine );
                
        /**
        * Two phase construction - Second phase.
        * @return None
        */
        void ConstructL();
    
        /**
        * Show global InfoNote
        * @param aResourceId resource id to be resolved
        */
        IMPORT_C void SendGlobalInfoNoteL( TInt aResourceId, 
                                           TBool aNotificationDialog = EFalse );
 
        /**
        * Show global ErrorNote
        * @param aResourceId resource id to be resolved
        */
        IMPORT_C void SendGlobalErrorNoteL( TInt aResourceId,
                                            TBool aNotificationDialog = EFalse );

        /**
        * Show global WarningNote
        * @param aResourceId resource id to be resolved
        */        
        IMPORT_C void SendGlobalWarningNoteL( 
                TInt aResourceId,
                TBool aNotificationDialog = EFalse );
        
    private:
        
        /**
        * Return SimState.
        */
        TPESimState SimState() const;
        
        /**
        * Return ETrue if video call.
        */
        TBool IsVideoCall( const TInt aCallId ) const;
        
        /**
        * Return ETrue if voice call.
        */
        TBool IsVoiceCall( const TInt aCallId ) const;

        /**
        * Gets cause code and resource id.
        */
        TBool GetCauseCode( TInt &aCauseCode, 
                TInt &aResourceId, TBool &aNotification ) const; 
				
    protected:
        
        /**
        * Handle for sending view commands.
        */
        MPhoneViewCommandHandle* iViewCommandHandle;
        
        MPhoneStateMachine* iStateMachine;
        
        TBool iCauseCodeVariation;
        
        TInt iCallId;
              
    };

#endif // __CPHONEERRORMESSAGESHANDLER_H
            
// End of File
