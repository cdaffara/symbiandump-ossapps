/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store sort result iterator client implementation.
*
*/



#include "MsgStoreSortResultIterator.h"
#include "MsgStoreSessionContext.h"
#include "MsgStorePropertyContainersArray.h"

// ----------------------------------
// CMsgStoreSortResultIterator
// ----------------------------------

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CMsgStoreSortResultIterator* CMsgStoreSortResultIterator::NewL( CMsgStoreSessionContext& aContext, 
                                                                TMsgStoreId              aSortSessionId )
	{
	CMsgStoreSortResultIterator* self = new( ELeave) CMsgStoreSortResultIterator( aContext, aSortSessionId );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMsgStoreSortResultIterator::CMsgStoreSortResultIterator( CMsgStoreSessionContext& aContext, 
                                                          TMsgStoreId              aSortSessionId )
: iContext( aContext ), iSortSessionId( aSortSessionId )
    {
    }
        
// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMsgStoreSortResultIterator::ConstructL()
    {
    }

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMsgStoreSortResultIterator::~CMsgStoreSortResultIterator()
    {
    //close the sort session
    TRAP_IGNORE( iContext.iSession.EndSortL( iSortSessionId ) );
    }
            
// ==========================================================================
// FUNCTION: NextL
// ==========================================================================
EXPORT_C TBool CMsgStoreSortResultIterator::NextL( TMsgStoreId                                aCurrentMessageId, 
                                                   TUint                                      aCount, 
                                                   RPointerArray<CMsgStorePropertyContainer>& aProperties )
    {
    TMsgStorePropertyContainersArray containersArray( aProperties );
    
    return iContext.iSession.GetSortedRowsL( iSortSessionId, aCurrentMessageId, EMsgStoreIteratorForward, aCount, containersArray );
    }

// ==========================================================================
// FUNCTION: NextL
// ==========================================================================
EXPORT_C TBool CMsgStoreSortResultIterator::NextL( const TDesC&                               aStartWith, 
                                                   TUint                                      aCount, 
                                                   RPointerArray<CMsgStorePropertyContainer>& aProperties )
    {
    TMsgStorePropertyContainersArray containersArray( aProperties );
    
    return iContext.iSession.GetSortedRowsL( iSortSessionId, KMsgStoreSortResultTop, EMsgStoreIteratorForward, aCount, containersArray, aStartWith );
    }
        
// ==========================================================================
// FUNCTION: PreviousL
// ==========================================================================
EXPORT_C TBool CMsgStoreSortResultIterator::PreviousL( TMsgStoreId                                aCurrentMessageId, 
                                                       TUint                                      aCount, 
                                                       RPointerArray<CMsgStorePropertyContainer>& aProperties )
    {
    TMsgStorePropertyContainersArray containersArray( aProperties );
    
    return iContext.iSession.GetSortedRowsL( iSortSessionId, aCurrentMessageId, EMsgStoreIteratorBackward, aCount, containersArray );
    }

// ==========================================================================
// FUNCTION: PreviousL
// ==========================================================================
EXPORT_C TBool CMsgStoreSortResultIterator::PreviousL( const TDesC&                               aStartWith,  
                                                       TUint                                      aCount, 
                                                       RPointerArray<CMsgStorePropertyContainer>& aProperties )
    {
    TMsgStorePropertyContainersArray containersArray( aProperties );
    
    return iContext.iSession.GetSortedRowsL( iSortSessionId, KMsgStoreSortResultTop, EMsgStoreIteratorBackward, aCount, containersArray, aStartWith );
    }

// ==========================================================================
// FUNCTION: SkipAndNextL
// ==========================================================================
EXPORT_C TBool CMsgStoreSortResultIterator::SkipAndNextL( TMsgStoreId                                aCurrentMessageId, 
                                                          TUint                                      aCount, 
                                                          RPointerArray<CMsgStorePropertyContainer>& aProperties )
    {
    TMsgStorePropertyContainersArray containersArray( aProperties );
    TBool skipCurrentGroup = ETrue;
    
    return iContext.iSession.GetSortedRowsL( iSortSessionId, 
            aCurrentMessageId, 
            EMsgStoreIteratorForward, 
            aCount, 
            containersArray, 
            KNullDesC, 
            skipCurrentGroup );
    }

// ==========================================================================
// FUNCTION: SkipAndPreviousL
// ==========================================================================
EXPORT_C TBool CMsgStoreSortResultIterator::SkipAndPreviousL( TMsgStoreId                                aCurrentMessageId, 
                                                              TUint                                      aCount, 
                                                              RPointerArray<CMsgStorePropertyContainer>& aProperties )
    {
    TMsgStorePropertyContainersArray containersArray( aProperties );
    TBool skipCurrentGroup = ETrue;
    
    return iContext.iSession.GetSortedRowsL( iSortSessionId, 
            aCurrentMessageId, 
            EMsgStoreIteratorBackward, 
            aCount, 
            containersArray, 
            KNullDesC, 
            skipCurrentGroup );
    }

// ==========================================================================
// FUNCTION: GroupCountL
// ==========================================================================
EXPORT_C TInt CMsgStoreSortResultIterator::GroupCountL( RArray<TUint>& aItemsInGroup )
    {
    return iContext.iSession.SortIteratorGroupCountL( iSortSessionId, aItemsInGroup );
    }

// ==========================================================================
// FUNCTION: IdsAndFlagsL
// ==========================================================================
EXPORT_C void CMsgStoreSortResultIterator::IdsAndFlagsL( RArray<TMsgStoreIdAndFlag>& aIdsAndFlags )
	{
    iContext.iSession.SortedIdsAndFlagsL( iSortSessionId, aIdsAndFlags );
	}

// ==========================================================================
// FUNCTION: IndexOfL
// ==========================================================================
EXPORT_C TInt CMsgStoreSortResultIterator::IndexOfL( TMsgStoreId aMessageId )
    {
    return iContext.iSession.SortedIndexOfL( iSortSessionId, aMessageId );
    }

// ==========================================================================
// FUNCTION: MessageIdsL
// ==========================================================================
EXPORT_C void CMsgStoreSortResultIterator::MessageIdsL( RArray<TMsgStoreId>& aMessageIds )
    {
    return iContext.iSession.SortedIdsL( iSortSessionId, aMessageIds );
    }

/** Retrieve all message ids in the sorted order and 
 *  retrieve the number groups in this iterator.
 *  NOTE: Customization work for Gimlet 2.0 
 */
EXPORT_C void CMsgStoreSortResultIterator::IdsAndGroupCountL( RArray<TMsgStoreId>& aMessageIds, RArray<TUint>& aItemsInGroup )
    {
    return iContext.iSession.IdsAndGroupCountL( iSortSessionId, aMessageIds, aItemsInGroup );
    }

