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
* Description:  Message store property containers array client implementation.
*
*/



#include "MsgStorePropertyContainersArray.h"
//<cmail>
#include "MsgStorePropertyContainer.h"
//</cmail>

// ------------------------        
// TPropertyContainersArray
// ------------------------        

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
TMsgStorePropertyContainersArray::TMsgStorePropertyContainersArray( RPointerArray<CMsgStorePropertyContainer>& aContainers ) : 
iContainers( aContainers ) 
    {
    } // end constructor
       
// ==========================================================================
// FUNCTION: AddElementL
// ==========================================================================
void TMsgStorePropertyContainersArray::AddElementL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aProperties )
    {
    CMsgStorePropertyContainer *newContainer = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( newContainer );
    
    newContainer->SetIds( aId, aParentId );
    newContainer->DeserializeL( aProperties );
    
    iContainers.AppendL( newContainer );    
    
    CleanupStack::Pop( newContainer );
    } // end AddElementL
    
// ==========================================================================
// FUNCTION: Reset
// ==========================================================================
void TMsgStorePropertyContainersArray::Reset()
    {
    iContainers.ResetAndDestroy();
    } // end Reset

