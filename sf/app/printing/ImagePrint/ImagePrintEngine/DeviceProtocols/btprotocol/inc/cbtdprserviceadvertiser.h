/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef CBTDPRSERVICEADVERTISER_H
#define CBTDPRSERVICEADVERTISER_H

// INCLUDES
#include <e32base.h>
#include <btsdp.h>
#include <btmanclient.h>

#include "cbtsdpadvertiser.h"

// CLASS DECLARATIONS
/**
* CBtDprServiceAdvertiser
* Advertises the Direct Printing Referenced Object OBEX service using the SDP database.
*/
NONSHARABLE_CLASS(CBtDprServiceAdvertiser) : public CBtSdpAdvertiser
    {
    public: //Constructors and destructor

       /**
        * NewL()
        * Create a CObjectExchangeServiceAdvertiser object
        * @return a pointer to the created instance of
        * CObjectExchangeServiceAdvertiser
        */
        static CBtDprServiceAdvertiser* NewL();

       /**
        * NewLC()
        * Create a CObjectExchangeServiceAdvertiser object
        * @return a pointer to the created instance of
        * CObjectExchangeServiceAdvertiser
        */
        static CBtDprServiceAdvertiser* NewLC();

       /**
        * ~CObjectExchangeServiceAdvertiser()
        * Destroy the object and release all memory objects
        */
        virtual ~CBtDprServiceAdvertiser();

    protected: // from CBTServiceAdvertiser

       /**
        * BuildProtocolDescriptionL()
        * Builds the protocol description
        * @param aProtocolDescriptor the protocol descriptor
        * @param aPort the service port
        */
        void BuildProtocolDescriptionL( CSdpAttrValueDES*
                                        aProtocolDescriptor, TInt aPort );

       /**
        * ServiceClass()
        * @return the service class
        */
        TUint ServiceClass();

       /**
        * ServiceName()
        * @return the service name
        */
        const TDesC& ServiceName();

       /**
        * ServiceDescription()
        * @return the service description
        */
        const TDesC& ServiceDescription();

    private:    // Constructors

       /**
        * CObjectExchangeServiceAdvertiser()
        * Constructs this object
        */
        CBtDprServiceAdvertiser();

       /**
        * ConstructL()
        * Perform the second phase construction of a
        * CObjectExchangeServiceAdvertiser object
        */
        void ConstructL();

    private:    // Data

       /**
        * iserviceDescription,
        * Owned by CObjectExchangeServiceAdvertiser.
        */
        HBufC* iServiceDescription;

    };

#endif // CBTDPRSERVICEADVERTISER_H

// End of File
