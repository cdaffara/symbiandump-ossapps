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
*     Contains mailbox specified always online logic
*
*/


#ifndef IPSSOSAOEMNRESOLVER_H
#define IPSSOSAOEMNRESOLVER_H

// INCLUDES
#include <e32base.h>

class CIpsSosAOMBoxLogic;

/**
*  This class handles OMA Email Notification messages.
*
*/
//<Qmail>
NONSHARABLE_CLASS (CIpsSosAOEMNResolver) : public CBase
//</Qmail>
    {
    public: 

        /**
        * Creates object from CIpsSosAOEMNResolver 
        * and leaves it to cleanup stack
        * @return, Constructed object
        */
        static CIpsSosAOEMNResolver* NewLC();

        /**
        * Creates object from CIpsSosAOEMNResolver
        * @return, Constructed object
        */
        static CIpsSosAOEMNResolver* NewL();
		//<Qmail>
        /**
        * destructor
        */
        virtual ~CIpsSosAOEMNResolver();
		//</Qmail>
    public: 
    
        /**
        * @param aMailboxURI Received mailbox URI and timestamp
        * @param aMailboxLogics Array of mailbox logic instances
        * @return index of correct mailbox or KErrNotFound
        */
        TInt FindEMNMailbox( const TDesC& aMailboxURI, 
                const RPointerArray<CIpsSosAOMBoxLogic>& aMailboxLogics );
            
        /**
        * Dispatches TEMNElement struct
        * @param aParameters Received mailbox URI and timestamp
        * @param aElement Dispatched mailbox URI and timestamp
        * @return KErrNone, if dispatching is ok
        */
        TInt ParameterDispatchTEMNElement( 
            const TDesC8& aParameters,
            TEMNElement& aElement ) const;

    private:
		//<Qmail>
        /**
        * constructor
        */
        CIpsSosAOEMNResolver();

        /**
        * 2nd phase constructor
        */
        void ConstructL();
		//</Qmail>
    private: 
        
        /**
        * Checks whether the given username and server is found from given URI
        * @param aURI, mailbox URI
        * @param aUsername
        * @param aServer
        * @return TBool, if aUsername and aServer is found from aURI
        */
        TBool HandleEmnImapUserURI(
            const TDesC& aURI,
            const TDesC8& aUsername,
            const TDesC& aServer ) const;

        /**
        * Checks whether the given username and server is found from given URI
        * @param aURI, mailbox URI
        * @param aUsername
        * @param aServer
        * @return TBool, if aUsername and aServer is found from aURI
        */
        TBool HandleEmnPopUserURI(
            const TDesC& aURI,
            const TDesC8& aUsername,
            const TDesC& aServer ) const;

        /**
        * Checks whether the given username and server is found from given URI
        * @param aURI, mailbox URI
        * @param aUsername
        * @param aServer
        * @return TBool, if aUsername and aServer is found from aURI
        */
        TBool HandleEmnPopNoAuthURI( 
            const TDesC& aURI,
            const TDesC8& aUsername,
            const TDesC& aServer ) const;
            
    };

#endif /*IPSSOSAOEMNRESOLVER_H*/
