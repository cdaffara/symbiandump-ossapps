/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Item ordered for traversing lists
*
*/




#include "mglxmedialist.h"
#include "glxlistutils.h"
#include "glxmedialistiterator.h"

#include <glxtracer.h>
#include <glxlog.h>
#include <hal.h>

const TInt KSequentialIteratorInitialValue = -1;
// Default granularity of items to request for
const TUint KGlxBlockyIteratorDefaultGranularity = 15;

const TInt KGlxLowerMemoryLimitForCacheSize = 15000000;
const TInt KGlxUpperMemoryLimitForCacheSize = 25000000;

const TInt KMaxLowMemOffsetValue = 3;
const TInt KMinLowMemOffsetValue = 1;

const TInt KMaxLowMemOffsetValueList = 10;
const TInt KMinLowMemOffsetValueList = 6;
const TInt KRearOffsetListValue = 2;

EXPORT_C TGlxSequentialIterator::TGlxSequentialIterator()
    {
    TRACER("TGlxSequentialIterator::TGlxSequentialIterator");
    
    iList = NULL;
    iCurrentItem = KSequentialIteratorInitialValue;
    iRange = KMaxTInt;      // To support full list length iteration,if range is not set                         
    }

// -----------------------------------------------------------------------------
// Set to first item
// -----------------------------------------------------------------------------
//
void TGlxSequentialIterator::SetToFirst(const MGlxMediaList* aList)
    {
    TRACER("TGlxSequentialIterator::SetToFirst");
    
    __ASSERT_DEBUG(aList != NULL, Panic(EGlxPanicNullPointer));
    iList = aList;
    iCurrentItem = KSequentialIteratorInitialValue;
    }

// -----------------------------------------------------------------------------
// Return the next item index or KErrNotFound
// -----------------------------------------------------------------------------
//
TInt TGlxSequentialIterator::operator++(TInt) 
    {
    TRACER("TGlxSequentialIterator::operator++");

    iCurrentItem++;
    if (iCurrentItem < iList->Count() && iCurrentItem < iRange )
        {
        return iCurrentItem;
        }
    else
        {
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// Return ETrue if index is within range, EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool TGlxSequentialIterator::InRange(TInt aIndex) const
    {
    TRACER("TGlxSequentialIterator::InRange");
    
    return (aIndex < iRange && aIndex < iList->Count() && aIndex >= 0);
    }
// -----------------------------------------------------------------------------
// SetRange
// -----------------------------------------------------------------------------
//
EXPORT_C void TGlxSequentialIterator::SetRange( TInt aRange )
    {
    TRACER("TGlxSequentialIterator::SetRange");
    
    iRange = aRange; 
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxSpecificIdIterator::TGlxSpecificIdIterator(const TGlxIdSpaceId& aIdSpaceId, TGlxMediaId aMediaId)
    : iMediaId(aMediaId), iIdSpaceId(aIdSpaceId), iFinished(EFalse)
    {
    TRACER("TGlxSpecificIdIterator::TGlxSpecificIdIterator");
    
    }

// -----------------------------------------------------------------------------
// Set to first item
// -----------------------------------------------------------------------------
//
void TGlxSpecificIdIterator::SetToFirst(const MGlxMediaList* aList)
    {
    TRACER("TGlxSpecificIdIterator::SetToFirst");
    
    __ASSERT_DEBUG(aList != NULL, Panic(EGlxPanicNullPointer));
    iList = aList;
    iFinished = EFalse;
    }

// -----------------------------------------------------------------------------
// Return the next item index or KErrNotFound
// -----------------------------------------------------------------------------
//
TInt TGlxSpecificIdIterator::operator++(TInt) 
    {
    TRACER("TGlxSpecificIdIterator::operator++");
    
    if (iFinished)
        {
        return KErrNotFound;
        }
    else
        {
        iFinished = ETrue;        
        return iList->Index(iIdSpaceId, iMediaId);
        }
    }
// -----------------------------------------------------------------------------
// Return ETrue if index is within range, EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool TGlxSpecificIdIterator::InRange(TInt aIndex) const
    {
    TRACER("TGlxSpecificIdIterator::InRange");
    
    return (aIndex == iList->Index(iIdSpaceId, iMediaId));
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFirstThenLastIterator::TGlxFirstThenLastIterator()
    {
    TRACER("TGlxFirstThenLastIterator::TGlxFirstThenLastIterator");
    
    iCurrentItem = 0;
    iList = NULL;
  }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFirstThenLastIterator::~TGlxFirstThenLastIterator()
    {
    TRACER("TGlxFirstThenLastIterator::~TGlxFirstThenLastIterator");
    
    }

// -----------------------------------------------------------------------------
// Set to first item
// -----------------------------------------------------------------------------
//
void TGlxFirstThenLastIterator::SetToFirst(const MGlxMediaList* aList) 
    {
    TRACER("TGlxFirstThenLastIterator::SetToFirst");    
    __ASSERT_DEBUG(aList, Panic(EGlxPanicNullPointer));

    iList = aList;
    iCurrentItem = 0;
  }

// -----------------------------------------------------------------------------
// Return the next item index or KErrNotFound
// -----------------------------------------------------------------------------
//
TInt TGlxFirstThenLastIterator::operator++(TInt) 
    {
    TRACER("TGlxFirstThenLastIterator::operator++");    
    __ASSERT_DEBUG(iList != NULL, Panic(EGlxPanicNullPointer));

    TInt count = iList->Count();
    if (count <= 0)
        {
        return KErrNotFound;
        }

    if (iCurrentItem == 0) 
        {
        // Return first item index
       iCurrentItem++;
       return 0; 
        }

    if (iCurrentItem == 1) 
        {
        // Return last item index
        iCurrentItem++;
        return count - 1;
        }
    
  // At last item already
    return KErrNotFound; 
    }
// -----------------------------------------------------------------------------
// Return ETrue if index is within range, EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool TGlxFirstThenLastIterator::InRange(TInt aIndex) const
    {
    TRACER("TGlxFirstThenLastIterator::InRange");
    __ASSERT_DEBUG(aIndex >= 0 && aIndex < iList->Count(), Panic(EGlxPanicIllegalArgument));

    // Return ETrue if first or last item
    return (aIndex == 0) || (aIndex == iList->Count() - 1);
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromFocusOutwardIterator::TGlxFromFocusOutwardIterator()
    {
    TRACER("TGlxFromFocusOutwardIterator::TGlxFromFocusOutwardIterator");
    iCurrentItem = 0;
    iFrontOffset = 0;
    iRearOffset = 0;
    iList = NULL;
  }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromFocusOutwardIterator::~TGlxFromFocusOutwardIterator()
    {
    TRACER("TGlxFromFocusOutwardIterator::~TGlxFromFocusOutwardIterator");
    } 

// ----------------------------------------------------------------------------
// Set range offsets
// ----------------------------------------------------------------------------
//
EXPORT_C void TGlxFromFocusOutwardIterator::SetRangeOffsets(TInt aRearOffset,
    TInt aFrontOffset)
    { 
    TRACER("TGlxFromFocusOutwardIterator::SetRangeOffsets");
    __ASSERT_DEBUG(aRearOffset >= 0 && aFrontOffset >= 0, Panic(EGlxPanicIllegalArgument)); 
    iFrontOffset = aFrontOffset;
    iRearOffset = aRearOffset;
    iOriginalFrontOffset = iFrontOffset;
    iOriginalRearOffset = iRearOffset;    
    }

// -----------------------------------------------------------------------------
// Set to first item
// -----------------------------------------------------------------------------
//
void TGlxFromFocusOutwardIterator::SetToFirst(const MGlxMediaList* aList) 
    {
    TRACER("TGlxFromFocusOutwardIterator::SetToFirst");
    __ASSERT_DEBUG(aList != NULL, Panic(EGlxPanicNullPointer));

    iList = aList;
    iCurrentItem = 0;
    }

// -----------------------------------------------------------------------------
// Return the item index or KErrNotFound, and goes to next
// -----------------------------------------------------------------------------
//
TInt TGlxFromFocusOutwardIterator::operator++(TInt) 
    {
    TRACER("TGlxFromFocusOutwardIterator::operator++");
    __ASSERT_DEBUG(iList != NULL, Panic(EGlxPanicNullPointer));

    TInt count = iList->Count();
    if (count <= 0)
        {
        return KErrNotFound;
        }
  
    if (iOriginalFrontOffset > KMaxLowMemOffsetValue && 
        iOriginalRearOffset > KMaxLowMemOffsetValue)
        {
        TInt freeMemory = 0;
        HAL::Get( HALData::EMemoryRAMFree, freeMemory );    
        if ( freeMemory < KGlxUpperMemoryLimitForCacheSize &&
             freeMemory > KGlxLowerMemoryLimitForCacheSize )
            {
            iFrontOffset = KMaxLowMemOffsetValue;
            iRearOffset = KMaxLowMemOffsetValue;
            }
        else if ( freeMemory < KGlxLowerMemoryLimitForCacheSize )
            {
            iFrontOffset = KMinLowMemOffsetValue;
            iRearOffset = KMinLowMemOffsetValue;
            }
        else if (iFrontOffset != iOriginalFrontOffset 
                 && iRearOffset!= iOriginalRearOffset)
            {
            iFrontOffset = Max(iFrontOffset, iOriginalFrontOffset );
            iRearOffset = Max(iRearOffset, iOriginalRearOffset );
            }
        }
  
   // Check if out of bounds
    if (iFrontOffset + iRearOffset < iCurrentItem || count <= iCurrentItem)
        {
        return KErrNotFound;
        }
    
    // The ranges may be inequal, which means there won't be any jumping between
    // front and rear. 
    
    // |-------F----------------------|
    // |< jumping zone>|

    TInt index = iList->FocusIndex();

    TInt min = Min(iFrontOffset, iRearOffset);
    TInt jumpingZoneLength = min * 2;
    if (iCurrentItem <= jumpingZoneLength) 
        {
        // Still within the (rear-front-rear-front) jumping zone
        TInt distanceFromFocus = (iCurrentItem + 1) / 2;
        TBool rear = !(iCurrentItem & 1); // Rear if number is even
        if (rear) 
             {
             index -= distanceFromFocus; 
             }
        else 
             {
             index += distanceFromFocus; 
             }
        }
     else 
         {
         __ASSERT_DEBUG(iFrontOffset != iRearOffset, Panic(EGlxPanicLogicError));
    
         // index is currently focus index. Figure out how much need to move.
         TInt indexesFromFocus = iCurrentItem - min; 
         if (iRearOffset < iFrontOffset)
             {
             // Front range is longer than rear, so the item is on the front side
             index += indexesFromFocus;
             }
         else 
             {
             // Rear range is longer than front, so the item is on the rear side
             index -= indexesFromFocus;
             }
         }

      iCurrentItem++;
    
      // The index may be below 0 or above count. Normalise back to list indexes.
      return GlxListUtils::NormalizedIndex(index, count); 
      } 
  
// -----------------------------------------------------------------------------
// Return ETrue if index is within range, EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool TGlxFromFocusOutwardIterator::InRange(TInt aIndex) const
    {
    TRACER("TGlxFromFocusOutwardIterator::InRange");
    TInt count = iList->Count();
  
    // Handle the case where range is longer than count separately, because looping will
    // confuse otherwise
    if (count <= iRearOffset + iFrontOffset) 
        {
        // Range is longer than count, must be in range
        return ETrue;
        }
  
    TInt focusIndex = iList->FocusIndex();
    TInt firstInRange = GlxListUtils::NormalizedIndex(focusIndex - iRearOffset, count);
    TInt lastInRange = GlxListUtils::NormalizedIndex(focusIndex + iFrontOffset, count);
  
    if (firstInRange <= lastInRange)
        {
        // Normal case:  |    F-------L   |
        return aIndex >= firstInRange && aIndex <= lastInRange;
        }
    else 
        {
        // Looping case: |----L      F----|
        return aIndex <= lastInRange || aIndex >= firstInRange;
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromVisibleIndexOutwardListIterator::TGlxFromVisibleIndexOutwardListIterator()
    {
    TRACER("TGlxFromVisibleIndexOutwardListIterator::TGlxFromVisibleIndexOutwardListIterator");
    iCurrentItem = 0;
    iFrontOffset = 0;
    iRearOffset = 0;
    iList = NULL;
  }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromVisibleIndexOutwardListIterator::~TGlxFromVisibleIndexOutwardListIterator()
    {
    TRACER("TGlxFromVisibleIndexOutwardListIterator::~TGlxFromVisibleIndexOutwardListIterator");
    } 

// ----------------------------------------------------------------------------
// Set range offsets
// ----------------------------------------------------------------------------
//
EXPORT_C void TGlxFromVisibleIndexOutwardListIterator::SetRangeOffsets(TInt aRearOffset,
    TInt aFrontOffset)
    { 
    TRACER("TGlxFromVisibleIndexOutwardListIterator::SetRangeOffsets");
    __ASSERT_DEBUG(aRearOffset >= 0 && aFrontOffset >= 0, Panic(EGlxPanicIllegalArgument)); 
    iFrontOffset = aFrontOffset;
    iRearOffset = aRearOffset;
    iOriginalFrontOffset = iFrontOffset;
    iOriginalRearOffset = iRearOffset;    
    }

// -----------------------------------------------------------------------------
// Set to first item
// -----------------------------------------------------------------------------
//
void TGlxFromVisibleIndexOutwardListIterator::SetToFirst(const MGlxMediaList* aList) 
    {
    TRACER("TGlxFromVisibleIndexOutwardListIterator::SetToFirst");
    __ASSERT_DEBUG(aList != NULL, Panic(EGlxPanicNullPointer));

    iList = aList;
    iCurrentItem = 0;
    }

// -----------------------------------------------------------------------------
// Return the item index or KErrNotFound, and goes to next
// -----------------------------------------------------------------------------
//
TInt TGlxFromVisibleIndexOutwardListIterator::operator++(TInt) 
    {
    TRACER("TGlxFromVisibleIndexOutwardListIterator::operator++");
    __ASSERT_DEBUG(iList != NULL, Panic(EGlxPanicNullPointer));

    TInt count = iList->Count();
    if (count <= 0)
        {
        return KErrNotFound;
        }

    if (iOriginalFrontOffset > KMaxLowMemOffsetValue && 
            iOriginalRearOffset > KMaxLowMemOffsetValue)
        {
        TInt freeMemory = 0;
        HAL::Get( HALData::EMemoryRAMFree, freeMemory );    
        if ( freeMemory < KGlxUpperMemoryLimitForCacheSize &&
                freeMemory > KGlxLowerMemoryLimitForCacheSize )
            {
            iFrontOffset = KMaxLowMemOffsetValueList;
            iRearOffset = KRearOffsetListValue;
            }
        else if ( freeMemory < KGlxLowerMemoryLimitForCacheSize )
            {
            iFrontOffset = KMinLowMemOffsetValueList;
            iRearOffset = KRearOffsetListValue;
            }
        else if (iFrontOffset != iOriginalFrontOffset 
                && iRearOffset!= iOriginalRearOffset)
            {
            iFrontOffset = Max(iFrontOffset, iOriginalFrontOffset );
            iRearOffset = Max(iRearOffset, iOriginalRearOffset );
            }
        }

    // Check if out of bounds
    if (iFrontOffset + iRearOffset < iCurrentItem || count <= iCurrentItem)
        {
        return KErrNotFound;
        }

    TInt index = iList->VisibleWindowIndex() + iCurrentItem ;
    iCurrentItem++;

    // The index may be below 0 or above count. Normalise back to list indexes.
    return GlxListUtils::NormalizedIndex(index, count); 
    } 
  
// -----------------------------------------------------------------------------
// Return ETrue if index is within range, EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool TGlxFromVisibleIndexOutwardListIterator::InRange(TInt aIndex) const
    {
    TRACER("TGlxFromVisibleIndexOutwardListIterator::InRange");
    TInt count = iList->Count();
  
    // Handle the case where range is longer than count separately, because looping will
    // confuse otherwise
    if (count <= iRearOffset + iFrontOffset) 
        {
        // Range is longer than count, must be in range
        return ETrue;
        }
    TInt focusIndex;
    if (!iList->VisibleWindowIndex())
        focusIndex = 0;
    else
        focusIndex = iList->VisibleWindowIndex();
    
    TInt firstInRange = GlxListUtils::NormalizedIndex(focusIndex - iRearOffset, count);
    TInt lastInRange = GlxListUtils::NormalizedIndex(focusIndex + iFrontOffset, count);
  
    if (firstInRange <= lastInRange)
        {
        // Normal case:  |    F-------L   |
        return aIndex >= firstInRange && aIndex <= lastInRange;
        }
    else 
        {
        // Looping case: |----L      F----|
        return aIndex <= lastInRange || aIndex >= firstInRange;
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromIndexOutwardBlockyIterator::TGlxFromIndexOutwardBlockyIterator(
    const MGlxIndex& aIndexFunctor ) 
        : iIndexFunctor( aIndexFunctor )
    {
    TRACER("TGlxFromIndexOutwardBlockyIterator::TGlxFromIndexOutwardBlockyIterator");
            
    iCurrentItem = 0;
    iFrontOffset = 2 * KGlxBlockyIteratorDefaultGranularity;
    iRearOffset = 2 * KGlxBlockyIteratorDefaultGranularity;
    iList = NULL;
  }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromIndexOutwardBlockyIterator::~TGlxFromIndexOutwardBlockyIterator()
    {
    TRACER("TGlxFromIndexOutwardBlockyIterator::~TGlxFromIndexOutwardBlockyIterator");
    
    } 

// ----------------------------------------------------------------------------
// Set range offsets
// ----------------------------------------------------------------------------
//
EXPORT_C void TGlxFromIndexOutwardBlockyIterator::SetRangeOffsets(TInt aRearOffset,
    TInt aFrontOffset)
    { 
    TRACER("TGlxFromIndexOutwardBlockyIterator::SetRangeOffsets");
    
    __ASSERT_DEBUG(aRearOffset >= 0 && aFrontOffset >= 0, Panic(EGlxPanicIllegalArgument)); 
    iFrontOffset = Max(aFrontOffset, KGlxBlockyIteratorDefaultGranularity );
    iRearOffset = Max(aRearOffset, KGlxBlockyIteratorDefaultGranularity );
    }

// -----------------------------------------------------------------------------
// Set to first item
// -----------------------------------------------------------------------------
//
void TGlxFromIndexOutwardBlockyIterator::SetToFirst(const MGlxMediaList* aList) 
    {
    TRACER("TGlxFromIndexOutwardBlockyIterator::SetToFirst");
    __ASSERT_DEBUG(aList != NULL, Panic(EGlxPanicNullPointer));

    iList = aList;
    iCurrentItem = 0;
    }

// -----------------------------------------------------------------------------
// Return the item index or KErrNotFound, and goes to next
// -----------------------------------------------------------------------------
//
TInt TGlxFromIndexOutwardBlockyIterator::operator++(TInt) 
    {
    TRACER("TGlxFromIndexOutwardBlockyIterator::operator++");
    __ASSERT_DEBUG(iList != NULL, Panic(EGlxPanicNullPointer));

    TInt count = iList->Count();
    if (count <= 0)
        {
        return KErrNotFound;
        }
  
   // Check if out of bounds
    if (iFrontOffset + iRearOffset < iCurrentItem || count <= iCurrentItem)
        {
        return KErrNotFound;
        }
    
    // The ranges may be inequal, which means there won't be any jumping between
    // front and rear. 
    
    // |-------F----------------------|
    // |< jumping zone>|

    TInt index = iIndexFunctor.Index();
    __ASSERT_ALWAYS( index >= 0 && index < iList->Count(), Panic( EGlxPanicIllegalState ) );
    index -= index % KGlxBlockyIteratorDefaultGranularity;

    TInt min = Min(iFrontOffset, iRearOffset);
    TInt jumpingZoneLength = min * 2;
    if (iCurrentItem <= jumpingZoneLength) 
        {
        // Still within the (rear-front-rear-front) jumping zone
        TInt distanceFromFocus = (iCurrentItem + 1) / 2;
        TBool rear = !(iCurrentItem & 1); // Rear if number is even
        if (rear) 
             {
             index -= distanceFromFocus; 
             }
        else 
             {
             index += distanceFromFocus; 
             }
        }
     else 
         {
         __ASSERT_DEBUG(iFrontOffset != iRearOffset, Panic(EGlxPanicLogicError));
    
         // index is currently focus index. Figure out how much need to move.
         TInt indexesFromFocus = iCurrentItem - min; 
         if (iRearOffset < iFrontOffset)
             {
             // Front range is longer than rear, so the item is on the front side
             index += indexesFromFocus;
             }
         else 
             {
             // Rear range is longer than front, so the item is on the rear side
             index -= indexesFromFocus;
             }
         }

      iCurrentItem++;
    
      // The index may be below 0 or above count. Normalise back to list indexes.
      return GlxListUtils::NormalizedIndex(index, count); 
      } 
  
// -----------------------------------------------------------------------------
// Return ETrue if index is within range, EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool TGlxFromIndexOutwardBlockyIterator::InRange(TInt aIndex) const
    {
    TRACER("TGlxFromIndexOutwardBlockyIterator::InRange");
    TInt count = iList->Count();
  
    // Handle the case where range is longer than count separately, because looping will
    // confuse otherwise
    if (count <= iRearOffset + iFrontOffset) 
        {
        // Range is longer than count, must be in range
        return ETrue;
        }
  
    TInt index = iIndexFunctor.Index();
    __ASSERT_ALWAYS( index >= 0 && index < iList->Count(), Panic( EGlxPanicIllegalState ) );
    index -= index % KGlxBlockyIteratorDefaultGranularity;
    TInt firstInRange = GlxListUtils::NormalizedIndex(index - iRearOffset, count);
    TInt lastInRange = GlxListUtils::NormalizedIndex(index + iFrontOffset, count);
  
    if (firstInRange <= lastInRange)
        {
        // Normal case:  |    F-------L   |
        return aIndex >= firstInRange && aIndex <= lastInRange;
        }
    else 
        {
        // Looping case: |----L      F----|
        return aIndex <= lastInRange || aIndex >= firstInRange;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromFocusOutwardBlockyIterator::TGlxFromFocusOutwardBlockyIterator()
        : TGlxFromIndexOutwardBlockyIterator(*static_cast<TGlxFromIndexOutwardBlockyIterator::MGlxIndex*>(this)  )
    {
    TRACER("TGlxFromIndexOutwardBlockyIterator::TGlxFromFocusOutwardBlockyIterator");
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromFocusOutwardBlockyIterator::~TGlxFromFocusOutwardBlockyIterator()
    {
    TRACER("TGlxFromIndexOutwardBlockyIterator::~TGlxFromFocusOutwardBlockyIterator");
    }

// -----------------------------------------------------------------------------
// Index
// -----------------------------------------------------------------------------
//
TInt TGlxFromFocusOutwardBlockyIterator::Index() const
    {
    TRACER("TGlxFromIndexOutwardBlockyIterator::Index");
    return iList->FocusIndex();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromManualIndexOutwardBlockyIterator::TGlxFromManualIndexOutwardBlockyIterator()
        : TGlxFromIndexOutwardBlockyIterator( *static_cast<TGlxFromIndexOutwardBlockyIterator::MGlxIndex*>(this) ), iIndex( 0 )
    {
    TRACER("TGlxFromManualIndexOutwardBlockyIterator::TGlxFromManualIndexOutwardBlockyIterator");
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromManualIndexOutwardBlockyIterator::~TGlxFromManualIndexOutwardBlockyIterator()
    {
    TRACER("TGlxFromManualIndexOutwardBlockyIterator::~TGlxFromManualIndexOutwardBlockyIterator");
    }
   
// -----------------------------------------------------------------------------
// SetIndex
// -----------------------------------------------------------------------------
//
EXPORT_C void TGlxFromManualIndexOutwardBlockyIterator::SetIndex( TInt aIndex )
    {
    TRACER("TGlxFromManualIndexOutwardBlockyIterator::SetIndex");
    
    __ASSERT_DEBUG( aIndex >= 0 && aIndex < iList->Count(), Panic( EGlxPanicIllegalArgument ) );
    iIndex = aIndex;
    }
    
// -----------------------------------------------------------------------------
// Index
// -----------------------------------------------------------------------------
//
TInt TGlxFromManualIndexOutwardBlockyIterator::Index() const
    {
    TRACER("TGlxFromManualIndexOutwardBlockyIterator::Index");
    
    return iIndex;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxSelectionIterator::TGlxSelectionIterator()
    {
    TRACER("TGlxSelectionIterator::TGlxSelectionIterator");
    
    iCurrentItem = 0;
    iList = NULL;
    iRange = KMaxTInt;
    iDisabledIfMoreThanRangeSelected = EFalse;
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxSelectionIterator::~TGlxSelectionIterator()
    {
    TRACER("TGlxSelectionIterator::~TGlxSelectionIterator");
    }

// -----------------------------------------------------------------------------
// Set to first item
// -----------------------------------------------------------------------------
//
EXPORT_C void TGlxSelectionIterator::SetToFirst(const MGlxMediaList* aList) 
    {
    TRACER("TGlxSelectionIterator::SetToFirst");
    __ASSERT_DEBUG(aList != NULL, Panic(EGlxPanicNullPointer));
  
    iList = aList;
    iCurrentItem = 0;
    }

// -----------------------------------------------------------------------------
// Return the next item index or KErrNotFound
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TGlxSelectionIterator::operator++(TInt) 
    {
    TRACER("TGlxSelectionIterator::operator++");
    __ASSERT_DEBUG(iList != NULL, Panic(EGlxPanicNullPointer));

    TInt result = KErrNotFound;

    TInt selectionCount = iList->SelectionCount();
  
    if (!iDisabledIfMoreThanRangeSelected || (selectionCount <= iRange))
    { 
    if (selectionCount == 0)
        {
        // If no items are selected, treat the focused index as the selection
        if ( iCurrentItem == 0 )
            {
            result = iList->FocusIndex();
            iCurrentItem++;
            }
        }
    else
        {
        if ( iCurrentItem < iRange && iCurrentItem < selectionCount )
            {
            result = iList->SelectedItemIndex(iCurrentItem);
            iCurrentItem++;
            }
        }
    }
  return result; 
  }

// -----------------------------------------------------------------------------
// TGlxSelectionIterator::InRange
// -----------------------------------------------------------------------------
//

EXPORT_C TBool TGlxSelectionIterator::InRange(TInt aIndex) const
    {
    TRACER("TGlxSelectionIterator::InRange");
    __ASSERT_DEBUG(aIndex >= 0 && aIndex < iList->Count(), Panic(EGlxPanicIllegalArgument));

    TInt selectionCount = iList->SelectionCount();
    TBool inRange = EFalse;
  
    if (selectionCount)
        {
        TBool legalIndex = EFalse;
        if (selectionCount > iRange)
            {
            if (iDisabledIfMoreThanRangeSelected)
                {
                legalIndex = EFalse;
                }
            else
                {
                legalIndex = (iList->SelectedItemIndex(iRange) > aIndex);
                }
            }
        else
            {
            legalIndex = ETrue;
            }
    
        if(legalIndex)
            {
            inRange = iList->IsSelected(aIndex);
            }
       }
    else
       {
       if (!(iDisabledIfMoreThanRangeSelected && (iRange == 0)))
           {
           inRange = (aIndex == iList->FocusIndex());
           }
       }
    return inRange;
}

// -----------------------------------------------------------------------------
// TGlxSelectionIterator::SetRange
// -----------------------------------------------------------------------------
//
EXPORT_C void TGlxSelectionIterator::SetRange(TInt aMaxItems)
    {
    TRACER("TGlxSelectionIterator::SetRange");
    iRange = aMaxItems;
    }

// -----------------------------------------------------------------------------
// TGlxSelectionIterator::SetDisabledIfMoreThanRangeSelected
// -----------------------------------------------------------------------------
//
EXPORT_C void TGlxSelectionIterator::SetDisabledIfMoreThanRangeSelected(TBool aDisabledIfMoreThanRangeSelected)
    {
    TRACER("TGlxSelectionIterator::SetDisabledIfMoreThanRangeSelected");
    iDisabledIfMoreThanRangeSelected = aDisabledIfMoreThanRangeSelected;
    }
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxExclusionIterator::TGlxExclusionIterator( MGlxMediaListIterator& aIncludedItemsIterator,
                                                       MGlxMediaListIterator& aExcludedItemsIterator )
                                                       :iIncludedItemsIterator( aIncludedItemsIterator ),
                                                       iExcludedItemsIterator( aExcludedItemsIterator )
    {           
    TRACER("TGlxExclusionIterator::TGlxExclusionIterator" );
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxExclusionIterator::~TGlxExclusionIterator()
    {
    TRACER("TGlxExclusionIterator::~TGlxExclusionIterator");
    } 
    
// -----------------------------------------------------------------------------
// Set to first item
// -----------------------------------------------------------------------------
//
EXPORT_C void TGlxExclusionIterator::SetToFirst(const MGlxMediaList* aList)
    {
    TRACER("TGlxExclusionIterator::SetToFirst");
    __ASSERT_DEBUG(aList != NULL, Panic(EGlxPanicNullPointer));

    //implementation delegates to both the iterators         
    iIncludedItemsIterator.SetToFirst( aList );
    iExcludedItemsIterator.SetToFirst( aList ); 
    }
    
// -----------------------------------------------------------------------------
// Return the next item index or KErrNotFound
// -----------------------------------------------------------------------------
//
TInt TGlxExclusionIterator::operator++(TInt ) 
    { 
    TRACER("TGlxExclusionIterator::operator++");

    // This method returns the next valid item of the iterator such that 
    // (1) The index is within the range of iIncludedItemsIterator
    // and
    // (2) The index is not in the range of iExcludedItemsIterator.
    // If iIncludedItemsIterator reaches its end in this process then we loop to the 
    // first item and repeat the above steps.

    TInt index = KErrNotFound; // assign initial value for maintenance safety

    do  {
        index = iIncludedItemsIterator++;
        } while ( KErrNotFound != index && iExcludedItemsIterator.InRange( index ) );

    return index;
    }
    
    
// -----------------------------------------------------------------------------
// Return ETrue if index is within range, EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool TGlxExclusionIterator::InRange(TInt aIndex) const
    { 
    TRACER("TGlxExclusionIterator::InRange");
    
    // Verify that aIndex is 
    // (1) In the range of iIncludedItemsIterator 
    // and 
    // (2) Not in the range of iExcludedItemsIterator  
    

    return iIncludedItemsIterator.InRange(aIndex) &&
                    !iExcludedItemsIterator.InRange(aIndex); 
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromManualIndexBlockyIterator::TGlxFromManualIndexBlockyIterator( )
    {
    TRACER("TGlxFromManualIndexBlockyIterator::TGlxFromManualIndexBlockyIterator");
    iDefaultVisItems = GlxListUtils::VisibleItemsGranularityL();       
    iCurrentItem = 0;
    iFrontOffset = 4 * iDefaultVisItems;
    iRearOffset = 2 * iDefaultVisItems;
    iOriginalFrontOffset = iFrontOffset;
    iOriginalRearOffset = iRearOffset;
    iList = NULL;
  }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFromManualIndexBlockyIterator::~TGlxFromManualIndexBlockyIterator()
    {
    TRACER("TGlxFromManualIndexBlockyIterator::~TGlxFromManualIndexBlockyIterator");
    
    } 

// ----------------------------------------------------------------------------
// Set range offsets
// ----------------------------------------------------------------------------
//
EXPORT_C void TGlxFromManualIndexBlockyIterator::SetRangeOffsets(TInt aRearOffset,
    TInt aFrontOffset)
    { 
    TRACER("TGlxFromManualIndexBlockyIterator::SetRangeOffsets");
    
    __ASSERT_DEBUG(aRearOffset >= 0 && aFrontOffset >= 0, Panic(EGlxPanicIllegalArgument)); 
    iFrontOffset = Max(aFrontOffset, iDefaultVisItems );
    iRearOffset = Max(aRearOffset, iDefaultVisItems );
    iOriginalFrontOffset = iFrontOffset;
    iOriginalRearOffset = iRearOffset;    
    }

// -----------------------------------------------------------------------------
// Set to first item
// -----------------------------------------------------------------------------
//
void TGlxFromManualIndexBlockyIterator::SetToFirst(const MGlxMediaList* aList) 
    {
    TRACER("TGlxFromManualIndexBlockyIterator::SetToFirst");
    __ASSERT_DEBUG(aList != NULL, Panic(EGlxPanicNullPointer));

    iList = aList;
    iCurrentItem = 0;
    }

// -----------------------------------------------------------------------------
// Return the item index or KErrNotFound, and goes to next
// -----------------------------------------------------------------------------
//
TInt TGlxFromManualIndexBlockyIterator::operator++(TInt) 
    {
    TRACER("TGlxFromManualIndexBlockyIterator::operator++");
    __ASSERT_DEBUG(iList != NULL, Panic(EGlxPanicNullPointer));

    TInt count = iList->Count();
    if (count <= 0)
        {
        return KErrNotFound;
        }

    if (iOriginalFrontOffset > KMaxLowMemOffsetValue* iDefaultVisItems &&
        iOriginalRearOffset > KMaxLowMemOffsetValue * iDefaultVisItems)
        {
        TInt freeMemory = 0;
        HAL::Get( HALData::EMemoryRAMFree, freeMemory );    
        if ( freeMemory < KGlxUpperMemoryLimitForCacheSize &&
             freeMemory > KGlxLowerMemoryLimitForCacheSize )
            {
            iFrontOffset = 2*iDefaultVisItems;
            iRearOffset = 3*iDefaultVisItems;
            }
        else if ( freeMemory < KGlxLowerMemoryLimitForCacheSize )
            {
            iFrontOffset = iDefaultVisItems;
            iRearOffset = iDefaultVisItems;
            }
        else if (iFrontOffset != iOriginalFrontOffset 
                 && iRearOffset!= iOriginalRearOffset)
            {
            iFrontOffset = Max(iFrontOffset, iOriginalFrontOffset );
            iRearOffset = Max(iRearOffset, iOriginalRearOffset );
            }
        }
  
   // Check if out of bounds
    if (iFrontOffset + iRearOffset < iCurrentItem || count <= iCurrentItem)
        {
        return KErrNotFound;
        }
    
    // The ranges may be inequal, which means there won't be any jumping between
    // front and rear. 
    
    // |-------F----------------------|
    // |< jumping zone>|

    TInt visIndex = iList->VisibleWindowIndex();
    TInt index = visIndex;
    TInt listCount = iList->Count();
    __ASSERT_ALWAYS( index >= 0 && index <= listCount, Panic( EGlxPanicIllegalState ) );
    //Inorder to ensure that the start of refresh of thumbnails is contained in
    //in the current visible window. We set the startIndex to center element of 
    //the current visible window.
    TInt startIndex = index + iDefaultVisItems/2;
    if (listCount < iDefaultVisItems)
        {
        startIndex = listCount/2;
        }
    
    index = ( (startIndex) <= (listCount)) ? startIndex : listCount;	    	

    TInt min = Min(iFrontOffset, iRearOffset);
    TInt jumpingZoneLength = min * 2;
    if (iCurrentItem <= jumpingZoneLength) 
        {
        // Still within the (rear-front-rear-front) jumping zone
        TInt distanceFromFocus = (iCurrentItem + 1) / 2;
        TBool rear = !(iCurrentItem & 1); // Rear if number is even
        // Check if out of bounds
        if (rear) 
             {
             if (index - distanceFromFocus <= visIndex - iRearOffset)
                 {
                 return KErrNotFound;
                 }
             index -= distanceFromFocus; 
             }
        else 
             {
             if (index + distanceFromFocus >= visIndex + iFrontOffset)
                 {
                 return KErrNotFound;
                 }
             index += distanceFromFocus; 
             }
        }
     else 
         {
         __ASSERT_DEBUG(iFrontOffset != iRearOffset, Panic(EGlxPanicLogicError));
    
         // index is currently visible index. Figure out how much need to move.
         TInt indexesFromFocus = iCurrentItem - min; 
         if (iRearOffset < iFrontOffset)
             {
             // Front range is longer than rear, so the item is on the front side
             index += indexesFromFocus;
             }
         else 
             {
             // Rear range is longer than front, so the item is on the rear side
             index -= indexesFromFocus;
             }
         }

      iCurrentItem++;
    
      // The index may be below 0 or above count. Normalise back to list indexes.
      return GlxListUtils::NormalizedIndex(index, count); 
      } 
  
// -----------------------------------------------------------------------------
// Return ETrue if index is within range, EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool TGlxFromManualIndexBlockyIterator::InRange(TInt aIndex) const
    {
    TRACER("TGlxFromManualIndexBlockyIterator::InRange");
    TInt count = iList->Count();
  
    // Handle the case where range is longer than count separately, because looping will
    // confuse otherwise
    if (count <= iRearOffset + iFrontOffset) 
        {
        // Range is longer than count, must be in range
        return ETrue;
        }
  
    TInt index = iList->VisibleWindowIndex();
    __ASSERT_ALWAYS( index >= 0 && index < iList->Count(), Panic( EGlxPanicIllegalState ) );
    
    TInt firstInRange = GlxListUtils::NormalizedIndex(index - iRearOffset, count);
    TInt lastInRange = GlxListUtils::NormalizedIndex(index + iFrontOffset, count);
  
    if (firstInRange <= lastInRange)
        {
        // Normal case:  |    F-------L   |
        return aIndex >= firstInRange && aIndex <= lastInRange;
        }
    else 
        {
        // Looping case: |----L      F----|
        return aIndex <= lastInRange || aIndex >= firstInRange;
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxScrollingDirectionIterator::TGlxScrollingDirectionIterator( )
    {
    TRACER("TGlxScrollingDirectionIterator::TGlxScrollingDirectionIterator");
    iMovingDirection = EForward;
    iExVisindex = 0;
    iDefaultVisItems = GlxListUtils::VisibleItemsGranularityL();       
    iCurrentItem = 0;
    iFrontOffset = 4 * iDefaultVisItems;
    iRearOffset = 2 * iDefaultVisItems;  // Rear Is Not Handled Yet, We Need To ?
    iList = NULL;
  }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxScrollingDirectionIterator::~TGlxScrollingDirectionIterator()
    {
    TRACER("TGlxScrollingDirectionIterator::~TGlxScrollingDirectionIterator");
    
    } 

// ----------------------------------------------------------------------------
// Set range offsets
// ----------------------------------------------------------------------------
//
EXPORT_C void TGlxScrollingDirectionIterator::SetRangeOffsets(TInt aRearOffset,
    TInt aFrontOffset)
    { 
    TRACER("TGlxScrollingDirectionIterator::SetRangeOffsets");
    
    __ASSERT_DEBUG(aRearOffset >= 0 && aFrontOffset >= 0, Panic(EGlxPanicIllegalArgument)); 
    iFrontOffset = Max(aFrontOffset, iDefaultVisItems );
    iRearOffset = Max(aRearOffset, iDefaultVisItems );
    }

// -----------------------------------------------------------------------------
// Set to first item
// -----------------------------------------------------------------------------
//
void TGlxScrollingDirectionIterator::SetToFirst(const MGlxMediaList* aList) 
    {
    TRACER("TGlxScrollingDirectionIterator::SetToFirst");
    __ASSERT_DEBUG(aList != NULL, Panic(EGlxPanicNullPointer));

    iList = aList;
    iCurrentItem = 0;
    }

// -----------------------------------------------------------------------------
// Return the item index or KErrNotFound, and goes to next
// -----------------------------------------------------------------------------
//
TInt TGlxScrollingDirectionIterator::operator++(TInt) 
    {
    TRACER("TGlxScrollingDirectionIterator::operator++");
    __ASSERT_DEBUG(iList != NULL, Panic(EGlxPanicNullPointer));

    TInt listCount = iList->Count();
    TInt index = iList->VisibleWindowIndex();
    __ASSERT_ALWAYS( index >= 0 && index <= listCount, Panic( EGlxPanicIllegalState ) );
    
    if (listCount <= 0 || iFrontOffset + iRearOffset < iCurrentItem || listCount <= iCurrentItem)
        {
        return KErrNotFound;
        }
  
    if(iExVisindex < index)
        {
        iMovingDirection = EForward;
        iExVisindex = index;
        }
    else if(iExVisindex > index)
        {
        iMovingDirection = EBackward;
        iExVisindex = index;
        }
    
    if(EForward == iMovingDirection)
        {
        if ((index + iCurrentItem  < index + iFrontOffset) && (index + iCurrentItem  < listCount))
            {
            index += iCurrentItem;
            }
        else
            {
            return KErrNotFound;
            }
        }
    else
        {
        if((index + iDefaultVisItems - iCurrentItem > index + iDefaultVisItems - iFrontOffset) && (index + iDefaultVisItems - iCurrentItem < listCount))
            {
            index += iDefaultVisItems - iCurrentItem;
            }
        else
            {
            return KErrNotFound;
            }
        }

      iCurrentItem++;
    
      // The index may be below 0 or above count. Normalise back to list indexes.
      return GlxListUtils::NormalizedIndex(index, listCount); 
      } 
  
// -----------------------------------------------------------------------------
// Return ETrue if index is within range, EFalse otherwise,
// -----------------------------------------------------------------------------
//
TBool TGlxScrollingDirectionIterator::InRange(TInt aIndex) const
    {
    TRACER("TGlxScrollingDirectionIterator::InRange");
    
    // Rear Window is Not Handled yet , Still We include Rear window in Consideration for Garbage Collection
    // Need to Re-Visit
    TInt count = iList->Count();
  
    // Handle the case where range is longer than count separately, because looping will
    // confuse otherwise
    if (count <= iRearOffset + iFrontOffset) 
        {
        // Range is longer than count, must be in range
        return ETrue;
        }
  
    TInt index = iList->VisibleWindowIndex();
    __ASSERT_ALWAYS( index >= 0 && index < iList->Count(), Panic( EGlxPanicIllegalState ) );
    
    TInt firstInRange = GlxListUtils::NormalizedIndex(index - iRearOffset, count);
    TInt lastInRange = GlxListUtils::NormalizedIndex(index + iFrontOffset, count);
  
    if (firstInRange <= lastInRange)
        {
        // Normal case:  |    F-------L   |
        return aIndex >= firstInRange && aIndex <= lastInRange;
        }
    else 
        {
        // Looping case: |----L      F----|
        return aIndex <= lastInRange || aIndex >= firstInRange;
        }
    }
