/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Concrete resource resolver for VoIP specific PhoneUI resources.
*
*/


#ifndef __CPHONERESOURCERESOLVERVOIP_H
#define __CPHONERESOURCERESOLVERVOIP_H

//  INCLUDES
#include "cphoneresourceresolvergsm.h"


// FORWARD DECLARATIONS
class MPhoneResourceResolverRegister;

// CLASS DECLARATION
/**
*  Concrete resource resolver for VoIP specific phoneapp resources.
*/
NONSHARABLE_CLASS( CPhoneResourceResolverVoIP ): public CPhoneResourceResolverGSM

    {
    public: // Constructors and destructor
        /**
        * Virtual Destructor
        * @param None
        * @return None
        */
        virtual ~CPhoneResourceResolverVoIP();

        /**
        * NewL function for creating the resolver.
        * @param aMainResourceResolver: Pointer to the main resolver.
        * @return CPhoneResourceResolver*
        */
        static CPhoneResourceResolverVoIP* NewL();
    
    public: // From MPhoneResourceResolver
        /**
        * Resolve resource id of given resource.
        * @param aEnumUid: Uid of enumeration which contains needed 
        *                  resource definition.
        * @param aResource: ID of needed resource ( from enumeration ).
        * @return Resource id.
        */
        TInt ResolveResourceID( const TInt& aResource ) const;       

    protected:    // Constructors and destructor        
        /**
        * Protected constructor.
        * @param None
        * @return None
        */
        CPhoneResourceResolverVoIP();
        
        /**
        * Two phase construction - Second phase.
        * @param aMainResourceResolver: Pointer to the main resolver.
        * @return None
        */
        void ConstructL();

    protected: // Data

        TInt iVoIPResourceOffset;       
  
    };

#endif      // CPHONERESOURCERESOLVERVOIP_H   
            
// End of File
