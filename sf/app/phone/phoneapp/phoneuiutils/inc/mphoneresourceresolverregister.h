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
*    API for register resource resolvers.
*
*/


#ifndef __MPHONERESOURCERESOLVERREGISTER_H
#define __MPHONERESOURCERESOLVERREGISTER_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
*  API for register resource resolvers.
*/
class MPhoneResourceResolverRegister
    {
    public:     
        
        /**
        * Destructor
        */
        virtual ~MPhoneResourceResolverRegister(){};

        /**
        * Register given resolver for resolving resources.
        * @param aResolver: Pointer to the resolver to be registered.      
        * @return None.
        */
        virtual void RegisterResolver( MPhoneResourceResolver* aResolver ) = 0;

    };
#endif      // __MPHONERESOURCERESOLVERREGISTER_H
            
// End of File
