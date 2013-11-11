/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Phone view command handle
*
*/

#ifndef MPHONEVIEWCOMMANDHANDLE_H
#define MPHONEVIEWCOMMANDHANDLE_H

//  INCLUDES
#include <e32std.h>
#include "phoneviewcommanddefinitions.h"

// DATA TYPES

// FORWARD DECLARATION
class TPhoneCommandParam;

// CLASS DECLARATION
/**
*  Interface class for executing UI commands
*
*/
class MPhoneViewCommandHandle
    {
    public:  // Constructors and destructor
        /**
        * Destructor.
        */
        virtual ~MPhoneViewCommandHandle(){};

    public: // New functions
        
        /**
        * Execute Command with given Command Id
        * @param aCmdId Command Id
        * @return None
        */
        virtual void ExecuteCommandL( 
            TPhoneViewCommandId aCmdId ) = 0;

        /**
        * Execute Command with given Command Id  and Call Id
        * @param aCmdId Command Id
        * @param aCallId Call Id
        * @return None
        */
        virtual void ExecuteCommandL( 
            TPhoneViewCommandId aCmdId, 
            TInt aCallId ) = 0;

         /**
        * Execute Command with given command id, call id and command parameter.
        * @param aCmdId Command Id
        * @param aCommandParam Command parameter
        * @return None
        */
        virtual void ExecuteCommandL( 
            TPhoneViewCommandId aCmdId, 
            TPhoneCommandParam* aCommandParam ) = 0;

         /**
        * Execute Command with given command id, call id and command parameter.
        * @param aCmdId Command Id
        * @param aCallId Call Id
        * @param aCommandParam Command parameter
        * @return None
        */
        virtual void ExecuteCommandL( 
            TPhoneViewCommandId aCmdId, 
            TInt aCallId, 
            TPhoneCommandParam* aCommandParam ) = 0;

        /**
        * Execute Command with given command id, call id and message.
        * @param aCmdId Command Id
        * @param aCallId Call Id
        * @param aMessage Command message
        */
        virtual void ExecuteCommandL( 
            TPhoneViewCommandId aCmdId, 
            TInt aCallId, 
            TDesC& aMessage ) = 0;

        /**
        * Handle Command with given Command Id
        * @param aCmdId Command Id
        * @return TPhoneViewResponseId
        */
        virtual TPhoneViewResponseId HandleCommandL( 
            TPhoneViewCommandId aCmdId ) = 0;

        /**
        * Handle Command with given command id and command parameter.
        * @param aCmdId Command Id
        * @param aCommandParam Command parameter
        * @return TPhoneViewResponseId
        */
        virtual TPhoneViewResponseId HandleCommandL( 
            TPhoneViewCommandId aCmdId, 
            TPhoneCommandParam* aCommandParam ) = 0;
            
        /**
        * Execute Command with given Command Id
        * @param aCmdId Command Id
        * @return None
        */
        virtual void ExecuteCommand( 
            TPhoneViewCommandId aCmdId ) = 0;
            
        /**
        * Execute Command with given command id and command parameter.
        * @param aCmdId Command Id
        * @param aCommandParam Command parameter
        * @return None
        */
        virtual void ExecuteCommand( 
            TPhoneViewCommandId aCmdId, 
            TPhoneCommandParam* aCommandParam ) = 0;                        
       
        /**
        * Execute Command with given Command Id
        */     
        virtual const TDesC& FetchContent() = 0;          
    };

#endif // MPHONEVIEWCOMMANDHANDLE_H   
            
// End of File
