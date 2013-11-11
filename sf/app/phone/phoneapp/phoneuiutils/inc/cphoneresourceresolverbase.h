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
*     Concrete resource resolver for common phoneapp resources.
*
*/


#ifndef __CPHONERESOURCERESOLVERBASE_H
#define __CPHONERESOURCERESOLVERBASE_H

//  INCLUDES
#include <e32base.h>
#include "mphoneresourceresolver.h"
#include "tphonetouchbuttonconfig.h"

// FORWARD DECLARATIONS
class CEikonEnv;

// CLASS DECLARATION
/**
*  Concrete resource resolver for common phoneapp resources.
*/
class CPhoneResourceResolverBase : 
    public CBase,
    public MPhoneResourceResolver
    {
    public: // Constructors and destructor
        
        /**
        * Base constructor. Should be called from the ConstructL of
        * the deriving class.
        * @param None
        * @return None
        */
        IMPORT_C void BaseConstructL();
        
        /**
        * Virtual Destructor
        * @param None
        * @return None
        */
        IMPORT_C virtual ~CPhoneResourceResolverBase();        
    
    public: // From MPAResourceResolver

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
        * Private constructor.
        * @param None        
        * @return None
        */
        IMPORT_C CPhoneResourceResolverBase();
        
        /**
        * Checkes whether a certain feature is on / or not.
        * @param aFeatureId
        * @return TBool supported /not supported
        */ 
        IMPORT_C virtual TBool IsTelephonyFeatureSupported( TInt aFeatureId ) const;
    
   protected:
       TInt iResourceOffset;     

    private:
        // Feature variation
        enum TVariationFlags
            {
            //Flag to indicate whether videocallmenu is supported.
            EVideoCallMenu = 1
            };
        TInt iVariationFlags;
        
        CEikonEnv& iEnv;
        
        TInt iCUIResourceOffset;
        TInt iPTCHResourceOffset;
        
        // Touch button config
        TPhoneTouchButtonConfig iTouchButtonConfig;
    };

#endif      // __CPHONERESOURCERESOLVERBASE_H   
            
// End of File
