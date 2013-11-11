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
*     Command for releasing a connection at Video Telephony shutdown.
*
*/


#ifndef CPHONERELEASECOMMAND_H
#define CPHONERELEASECOMMAND_H

// INCLUDES
#include <e32base.h>
#include "mphoneshutdowncommand.h"
#include "mphoneenginemessagesender.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CPhoneReleaseCommand : public CBase, public MPhoneShutdownCommand
    {
    public:  

        /**
         * Destructor.
         */
        virtual ~CPhoneReleaseCommand();
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPhoneReleaseCommand* NewL( 
            MPhoneEngineMessageSender& aEngineMessageSender );
        
        /**
        * Executes this Command.
        */
        virtual void ExecuteLD();
        

    private:

        /**
        * C++ default constructor.
        */
        CPhoneReleaseCommand( 
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
    
#endif // CPHONERELEASECOMMAND_H

// End of File
