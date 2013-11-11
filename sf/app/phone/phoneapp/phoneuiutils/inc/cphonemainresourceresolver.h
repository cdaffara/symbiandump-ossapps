/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    Phone resource resolver
*
*/


#ifndef __CPHONEMAINRESOURCERESOLVER_H
#define __CPHONEMAINRESOURCERESOLVER_H

//  INCLUDES
#include <e32base.h>
#include <coemain.h>
#include "mphoneresourceresolver.h"
#include "mphoneresourceresolverregister.h"


// CLASS DECLARATION

/**
*  CPhoneMainResourceResolver realizes the MPARegisterResourceResolver interface, which
*  allows for any resource resolver to offer their services for the Main Resource
*  Resolver. Basically, the Main Resource Resolver does not have any hard coded resolving
*  functionality. It only serves as access point for all the individual resource resolvers
*  scattered around the application ( via MPARegisterResourceResolver interface ).
*/
class CPhoneMainResourceResolver : 
    public CCoeStatic,
    public MPhoneResourceResolver,
    public MPhoneResourceResolverRegister
    {
    public: // Constructors and destructor
        /**
        * Virtual Destructor
        * @param None
        * @return None
        */
        virtual ~CPhoneMainResourceResolver();
    
    public: // From MPAResourceResolver
        /**
        * Resolve resource id of given resource.
        * @param aEnumUid: Uid of enumeration which contains needed resource 
        *                  definition.
        * @param aResource: ID of needed resource ( from enumeration ).
        * @return Resource id.
        */
        TInt ResolveResourceID( const TInt& aResource ) const;

    public: // From MPARegisterResourceResolver
        /**
        * Register given resolver for resolving resources from given 
        * enumeration.
        * @param aResolver: Pointer to the resolver to be registered.
        * @param aEnumUid: Uid of enumeration which resources given resolver 
        *                  resolves.        
        * @return None.
        */
        void RegisterResolver( MPhoneResourceResolver* aResolver );

    public: // New methods
     
        /**
        * First call initializes the singleton object. Subsequent calls return
        * instance.
        * @param None.
        * @return Pointer to the one and only instance of main resouce 
        *         resolver -object.
        */
        IMPORT_C static  CPhoneMainResourceResolver* Instance();
        
    protected:  // Constructors and destructor
        /**
        * Protected constructor.
        * @param None
        * @return None
        */
        CPhoneMainResourceResolver();

    private:    // Constructors and destructor
        /**
        * NewL function for creating the main resolver.
        * @param None
        * @return CPhoneMainResourceResolver*
        */
        static CPhoneMainResourceResolver* NewL();
        
    private:    // Data      
        
        MPhoneResourceResolver* iResolver;

    };
#endif      // __CPHONERESOURCERESOLVER_H
            
// End of File
