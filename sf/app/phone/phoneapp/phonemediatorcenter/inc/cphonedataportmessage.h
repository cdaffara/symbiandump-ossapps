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
*     Mediator Message for notifying of used Data Port.
*
*/


#ifndef CPHONEDATAPORTMESSAGE_H
#define CPHONEDATAPORTMESSAGE_H

// INCLUDES
#include <e32base.h>
#include "mphonemediatormessage.h"
#include "mpeengineinfo.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CPhoneDataPortMessage : public CBase, public MPhoneMediatorMessage
    {
    public:  

        /**
         * Destructor.
         */
        virtual ~CPhoneDataPortMessage();
        
        /**
        * Two-phased constructor.
        */
        static CPhoneDataPortMessage* NewL( MPEEngineInfo& aEngineInfo );
        
        /**
        * Executes this Command.
        */
        virtual void ExecuteL();
        

    private:

        /**
        * C++ default constructor.
        */
        CPhoneDataPortMessage( MPEEngineInfo& aEngineInfo );
        
        /**
         * By default EPOC constructor is private.
         */
        void ConstructL();
        
             
    private:
        
        MPEEngineInfo& iEngineInfo;
    };
    
#endif // CPHONEDATAPORTMESSAGE_H

// End of File
