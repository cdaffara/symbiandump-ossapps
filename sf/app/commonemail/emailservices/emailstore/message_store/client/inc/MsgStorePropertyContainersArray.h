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
* Description:  Message store client property containers array.
*
*/



#ifndef _MSG_STORE_PROPERTY_CONTAINERS_ARRAY_H_
#define _MSG_STORE_PROPERTY_CONTAINERS_ARRAY_H_

#include "RMessageStoreSession.h"

class CMsgStorePropertyContainer;

// =========================================
// Internal CLASS: TPropertyContainersArray
// =========================================
class TMsgStorePropertyContainersArray : public MPropertiesArray
{
    public:
    
        // ==============
        // PUBLIC METHODS
        // ==============
        
        TMsgStorePropertyContainersArray( RPointerArray<CMsgStorePropertyContainer>& aContainers );
        
        // inherited from MPropertiesArray              
        virtual void AddElementL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aProperties );
        virtual void Reset();        
        
    private:
    
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
    
        RPointerArray<CMsgStorePropertyContainer>& iContainers;
    
}; // end class TMsgStorePropertyContainersArray


#endif /*_MSG_STORE_PROPERTY_CONTAINERS_ARRAY_H_*/
