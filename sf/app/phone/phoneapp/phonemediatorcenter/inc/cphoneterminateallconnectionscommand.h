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
*     Command for terminating all connections at Video Telephony shutdown.
*
*/


#ifndef CPHONETERMINATEALLCONNECTIONSCOMMAND_H
#define CPHONETERMINATEALLCONNECTIONSCOMMAND_H

// INCLUDES
#include <e32base.h>
#include "mphoneshutdowncommand.h"
#include "mphoneenginemessagesender.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CPhoneTerminateAllConnectionsCommand : public CBase, 
    public MPhoneShutdownCommand
    {
    public:  

        /**
         * Destructor.
         */
        virtual ~CPhoneTerminateAllConnectionsCommand();
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPhoneTerminateAllConnectionsCommand* NewL( 
            MPhoneEngineMessageSender& aEngineMessageSender );
        
        /**
        * Executes this Command.
        */
        virtual void ExecuteLD();
        

    private:

        /**
        * C++ default constructor.
        */
        CPhoneTerminateAllConnectionsCommand( 
            MPhoneEngineMessageSender& aEngineMessageSender );
        
        /**
         * By default EPOC constructor is private.
         */
        void ConstructL();
        
        
    private:

        /**
        * For sending message to Phone Engine.
        */    
        MPhoneEngineMessageSender& iEngineMessageSender;
                     
    };
    
#endif // CPHONETERMINATEALLCONNECTIONSCOMMAND_H

// End of File
