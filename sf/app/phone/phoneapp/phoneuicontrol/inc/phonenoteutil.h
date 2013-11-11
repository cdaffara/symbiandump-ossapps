/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Note util class.
*
*/


#ifndef PHONENOTEUTIL_H
#define PHONENOTEUTIL_H

// INCLUDES
#include <e32base.h>
#include <pevirtualengine.h>
#include "mphoneviewcommandhandle.h"
#include "tphonecmdparamglobalnote.h"

// CLASS DECLARATION
 
/**
*  Note util class.
*
*/
class PhoneNoteUtil
    {
    public:
        
        /**
         * Shows global info note.
         */
        static void SendGlobalInfoNoteL( 
                TInt aResourceId, TBool aNotificationDialog );
        
        /**
         * Shows global warning note.
         */
        static void SendGlobalWarningNoteL( 
                TInt aResourceId, TBool aNotificationDialog );
        
        /**
         * Shows global error note.
         */
        static void SendGlobalErrorNoteL( 
                TInt aResourceId, TBool aNotificationDialog );
        
        /**
         * Shows global note.
         */
        static void SendGlobalNoteL( 
                TInt aResourceId, TBool aNotificationDialog );
        
        /**
         * Shows global note.
         */
        static void SendGlobalNoteL( 
                TPhoneNotificationType aType, const TDesC& aText );
        
        /**
         * Shows note.
         */
        static void ShowNoteL( TInt aResourceId );
        
        /**
         * Shows query.
         */
        static void ShowQueryL( TInt aResourceId );
        
        /**
         * Shows text query.
         */
        static void ShowTextQueryL(
                TInt aDialogResourceId,
                TInt aDefaultCbaResourceId,
                TInt aContentCbaResourceId,
                TDes* aDataText,
                TBool aSendKeyEnabled );
        
        /**
         * Default error handling.
         */
        static void HandleErrorL( const TPEErrorInfo& aErrorInfo );
        
        /**
         * handles remote busy note.
         */
        static void HandleRemoteBusyNoteL( TInt aCallId );
        
    private:
        
        /**
         * Sets global notifier disabled.
         */
        static void SetGlobalNotifierDisabledL( 
                MPhoneViewCommandHandle* aViewCommandHandle );
        
        /**
         * Sets common global note params.
         */
        static void SetCommonGlobalNoteParamsL(
                TPhoneCmdParamGlobalNote& aGlobalNoteParam,
                TInt aResourceId, TBool aNotificationDialog,
                TPhoneNotificationType aNoteType,
                TPhoneNotificationToneType aTone = EPhoneNoTone );
      
        /**
         * Returns view command handler.
         */
        inline static MPhoneViewCommandHandle* ViewCommandHandle();
    };

#endif      // PHONENOTEUTIL_H  
            
// End of File
