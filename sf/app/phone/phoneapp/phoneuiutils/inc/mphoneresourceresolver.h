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
*    An abstract base class for resource resolvers.
*
*/


#ifndef MPHONERESOURCERESOLVER_H
#define MPHONERESOURCERESOLVER_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
*  An abstract base class for resource resolvers.
*/
class MPhoneResourceResolver
    {
    public:
    
        /**
        * Destructor
        */
        virtual ~MPhoneResourceResolver() {};

        /**
        * Resolve resource id of given resource.
        * @param aResource: ID of needed resource ( from enumeration ).
        * @return Resource id.
        */
        virtual TInt ResolveResourceID( const TInt& aResource ) const = 0;

        
    };
#endif      // MPHONERESOURCERESOLVER_H
            
// End of File
