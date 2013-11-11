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
*     Factory for creating Mediator Message objects.
*
*/


#ifndef CPHONEMEDIATORMESSAGEFACTORY_H
#define CPHONEMEDIATORMESSAGEFACTORY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPhoneMediatorMessage;
class MPEEngineInfo;

// CLASS DECLARATION

class CPhoneMediatorMessageFactory : public CBase
    {
    public:  

        /**
         * Destructor.
         */
        virtual ~CPhoneMediatorMessageFactory();
        
        /**
        * Two-phased constructor.
        */
        static CPhoneMediatorMessageFactory* NewL( MPEEngineInfo& aEngineInfo );
        
        /**
        * Creates the concrete Mediator Message object.
        * @param aMessage - Message from Phone Engine (EPEMessage).
        * @param aCallId - Caller id
        * @return MPhoneMediatorMessage object or NULL if no associate object found for
        *         EPEMessage.
        */
        MPhoneMediatorMessage* CreateMessageL( const TInt aMessage, TInt aCallId );
        

    private:

        /**
        * C++ default constructor.
        */
        CPhoneMediatorMessageFactory( MPEEngineInfo& aEngineInfo );  
        
        /**
         * By default EPOC constructor is private.
         */
        void ConstructL();
        
             
    private:
        
        // Reference to Engine Info
        MPEEngineInfo& iEngineInfo;

    };
    
#endif // CPHONEMEDIATORMESSAGEFACTORY_H

// End of File
