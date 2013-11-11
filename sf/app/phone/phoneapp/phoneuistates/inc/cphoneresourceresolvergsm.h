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
*     Concrete resource resolver for GSM specific PhoneUi resources.
*
*/


#ifndef __CPHONERESOURCERESOLVERGSM_H
#define __CPHONERESOURCERESOLVERGSM_H

//  INCLUDES
#include <e32base.h>
#include "cphoneresourceresolverbase.h"


// FORWARD DECLARATIONS
class MPhoneResourceResolverRegister;
class CEikonEnv;

// CLASS DECLARATION
/**
*  Concrete resource resolver for GSM spesific phoneapp resources.
*/
class CPhoneResourceResolverGSM : public CPhoneResourceResolverBase

    {
    public: // Constructors and destructor
        /**
        * Virtual Destructor
        * @param None
        * @return None
        */
        IMPORT_C virtual ~CPhoneResourceResolverGSM();

        /**
        * NewL function for creating the resolver.
        * @param aMainResourceResolver: Pointer to the main resolver.
        * @return CPhoneResourceResolver*
        */
        static CPhoneResourceResolverGSM* NewL();
    
    public: // From MPhoneResourceResolver
        /**
        * Resolve resource id of given resource.
        * @param aEnumUid: Uid of enumeration which contains needed 
        *                  resource definition.
        * @param aResource: ID of needed resource ( from enumeration ).
        * @return Resource id.
        */
        IMPORT_C TInt ResolveResourceID( const TInt& aResource ) const;       

    protected:    // Constructors and destructor        
        /**
        * Protected constructor.
        * @param None
        * @return None
        */
        IMPORT_C CPhoneResourceResolverGSM();
        
        /**
        * Two phase construction - Second phase.
        * @param aMainResourceResolver: Pointer to the main resolver.
        * @return None
        */
        IMPORT_C void ConstructL();
        
    protected: // Data

        CEikonEnv& iEnv;
    };

#endif      // CPHONERESOURCERESOLVERGSM_H   
            
// End of File
