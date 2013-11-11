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
*     Command for letting Phone Engine continue Emergency call initialization.
*     This is needed because VT has to release dataport before we can continue
*     emergency call. Otherwise VT call after emergency call fails as dataport
*     isn't released.
*
*/


#ifndef CPHONECONTINUEEMERGENCYCALLCOMMAND_H
#define CPHONECONTINUEEMERGENCYCALLCOMMAND_H

// INCLUDES
#include <e32base.h>
#include "mphoneshutdowncommand.h"
#include "mphoneenginemessagesender.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CPhoneContinueEmergencyCallCommand : public CBase, 
    public MPhoneShutdownCommand
    {
    public:  

        /**
         * Destructor.
         */
        virtual ~CPhoneContinueEmergencyCallCommand();
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPhoneContinueEmergencyCallCommand* NewL( 
            MPhoneEngineMessageSender& aEngineMessageSender );
        
        /**
        * Executes this Command.
        */
        virtual void ExecuteLD();
        

    private:

        /**
        * C++ default constructor.
        */
        CPhoneContinueEmergencyCallCommand( 
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
    
#endif // CPHONECONTINUEEMERGENCYCALLCOMMAND_H

// End of File
