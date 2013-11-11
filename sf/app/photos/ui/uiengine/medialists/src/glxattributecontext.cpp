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
* Description:    Generic fetch context to retrieve attributes
*
*/



#include "glxattributecontext.h"
#include "mglxmedialist.h"
#include "glxerrormanager.h"
#include "glxtracer.h"
#include "glxlog.h"
#include "glxlistutils.h"
#include "glxthumbnailattributeinfo.h"

// Default granularity of items to request for
const TUint KGlxAttributeContextDefaultGranularity = 200;

const TInt KVisibleItemsCorrectionValue = 1;
const TInt KVisibleItemsModFactor = 2;

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxAttributeContext::CGlxAttributeContext(MGlxMediaListIterator* aIterator) :
        iIterator(aIterator),
        iGranularity(KGlxAttributeContextDefaultGranularity)
    {
    TRACER("CGlxAttributeContext::Default Constructor");
    
    __ASSERT_DEBUG(aIterator, Panic(EGlxPanicNullPointer));
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxAttributeContext::~CGlxAttributeContext()
    {
    TRACER("CGlxAttributeContext:: Destructor");
    
    iAttributes.Close();
    }

// -----------------------------------------------------------------------------
// Adds an attribute to be retrieved for all items
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxAttributeContext::AddAttributeL(const TMPXAttribute& aAttribute)
    {
    TRACER("CGlxAttributeContext::AddAttributeL");
    
    // Check that the attribute is not added twice
    TIdentityRelation<TMPXAttribute> match (&TMPXAttribute::Match);
    TInt index = iAttributes.Find(aAttribute, match);

    // Add the attribute
    if (KErrNotFound == index) 
        {
        iAttributes.AppendL(aAttribute);
        }
    }

// -----------------------------------------------------------------------------
// Removes an attribute from the "retrieval instructions"
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxAttributeContext::RemoveAttribute(const TMPXAttribute& aAttribute)
    {
    TRACER("CGlxAttributeContext::RemoveAttribute");
    
    // Check that the attribute to be removed exists
    TIdentityRelation<TMPXAttribute> match (&TMPXAttribute::Match);
    TInt index = iAttributes.Find(aAttribute, match);

    // Remove the attribute
    if (KErrNotFound != index) 
        {
        iAttributes.Remove(index);
        }
    }

// -----------------------------------------------------------------------------
// CGlxDefaultAttributeContext::AttributeCount
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CGlxAttributeContext::AttributeCount()
	{
	TRACER("CGlxAttributeContext::AttributeCount");
	
	return iAttributes.Count();
	}

// -----------------------------------------------------------------------------
// Sets granularity of the item index array returned from AttributeRequestL
// -----------------------------------------------------------------------------
EXPORT_C void CGlxAttributeContext::SetGranularity(TUint aGranularity)
    {
    TRACER("CGlxAttributeContext::SetGranularity");
    
    iGranularity = aGranularity;
    }

// -----------------------------------------------------------------------------
// Get attributes request for an item
// From MGlxFetchContext
// -----------------------------------------------------------------------------
//
TInt CGlxAttributeContext::AttributeRequestL(const MGlxMediaList* aList, 
        RArray<TInt>& aItemIndices, RArray<TMPXAttribute>& aAttributes, 
        CMPXAttributeSpecs*& aDetailedSpecs) const
    {
    TRACER("CGlxAttributeContext::AttributeRequestL");
    
    TInt itemCount = aList->Count();
    if (0 == itemCount || !iIterator) 
        {
        return 0;
        }

    // Check through the items in the order defined by the iterator
    iIterator->SetToFirst(aList);

    // Loop until iterator does not give any more indexes or granularity reached
    TInt index = KErrNotFound;
    TInt error = KErrNone;
    TBool firstItem = ETrue;
    TInt defaultVisItems = GlxListUtils::VisibleItemsGranularityL();
    if (defaultVisItems%KVisibleItemsModFactor == 0)
        {
        defaultVisItems += KVisibleItemsCorrectionValue;
        }
            
    while ((KErrNotFound != (index = (*iIterator)++)) && (aItemIndices.Count() < iGranularity) && (KErrNone == error))
        {
        // Check if this item is lacking attributes
        TBool attributesAdded = AddItemAttributesL(index, aList, aAttributes, error, firstItem);
        if (attributesAdded)
            {
            aItemIndices.AppendL(index);
            GLX_DEBUG3("CGlxAttributeContext::AttributeRequestL() index=%d, Media Id=%d", 
                                                    index, aList->Item(index).Id().Value());
            firstItem = EFalse;
                              
            // Fetch the visible items attrib first
            if (aItemIndices[0] == (aList->VisibleWindowIndex()+ (defaultVisItems/2))
                && aItemIndices.Count() == defaultVisItems)
                {
                GLX_DEBUG1("Break to fetch the visible items attrib first");
                break;
				}
            }
        else if ( firstItem && ( error != KErrNone ) )
            {
            // Continue iterating through the list
            // if the first item had errors
            // See EMYP-79VDHP
            error = KErrNone;
            }
        }

    
    // If the attribute request is for Thumbnail, Check if there is a match found.
    // And set the size and thumbnail quality
	TIdentityRelation<TMPXAttribute> matchContent(&TMPXAttribute::MatchContentId);
	TMPXAttribute tnAttr(KGlxMediaIdThumbnail, 0);
	
	if (iAttributes.Find(tnAttr, matchContent) != KErrNotFound) 
		{
		// Allocate CMPXAttributeSpecs
		CMPXAttributeSpecs* attributeSpecs = CMPXAttributeSpecs::NewL();
		CleanupStack::PushL(attributeSpecs);
		
	attributeSpecs->SetTObjectValueL(
	   TMPXAttribute( KGlxMediaIdThumbnail,
					   KGlxAttribSpecThumbnailSize ), 
					   TSize(iDefaultSpecSize.iWidth,iDefaultSpecSize.iHeight) );
		
		attributeSpecs->SetTObjectValueL(
		   TMPXAttribute( KGlxMediaIdThumbnail,
					   KGlxAttribSpecThumbnailQualityOverSpeed ), ETrue );
		
		aDetailedSpecs = attributeSpecs;
		
		// Pop from stack
		CleanupStack::Pop(attributeSpecs);
		
		}

    // If an error was found, return KErrGeneral
    if (error != KErrNone)
        {
        return KErrGeneral;
        }

    GLX_DEBUG2("CGlxAttributeContext::AttributeRequestL() aItemIndices.Count()=%d",
                                      aItemIndices.Count());
    return aItemIndices.Count();
    }

// -----------------------------------------------------------------------------
// Get all attributes required for the item (whether the are fetched or not)
// From MGlxFetchContext
// -----------------------------------------------------------------------------
//
void CGlxAttributeContext::AllAttributesL(const MGlxMediaList* aList, TInt aListIndex, 
        RArray<TMPXAttribute>& aAttributes) const 
    {
    TRACER("CGlxAttributeContext::AllAttributesL");
    
    // An empty request makes no sense, but won't do any harm.

    // If no iterator, no request
    if (!iIterator || !iAttributes.Count())
        {
        return;
        }
	
    // No requests outside range
    iIterator->SetToFirst(aList);
    if (!iIterator->InRange(aListIndex))
        {
        return;
        }

    CleanupClosePushL(aAttributes);
    // Just list all attributes specified for this context
    TInt count = iAttributes.Count();
    for (TInt i = 0; i < count; i++) 
        {
        aAttributes.AppendL(iAttributes[i]);
        }
    CleanupStack::Pop(&aAttributes);
    }

// -----------------------------------------------------------------------------
// Number of current requests
// From MGlxFetchContext
// -----------------------------------------------------------------------------
TInt CGlxAttributeContext::RequestCountL(const MGlxMediaList* aList) const
    {
    TRACER("CGlxAttributeContext::RequestCountL");
    
    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attrSpecs = NULL;

    TInt requestCount = AttributeRequestL(aList, itemIndices, attributes, attrSpecs);

    delete attrSpecs;

    CleanupStack::PopAndDestroy(&attributes);
    CleanupStack::PopAndDestroy(&itemIndices);

    return requestCount;
    }

// -----------------------------------------------------------------------------
// Check if the item at the index requires any attributes to be added to request
// -----------------------------------------------------------------------------
TBool CGlxAttributeContext::AddItemAttributesL(TInt aIndexInList, 
        const MGlxMediaList* aList, RArray<TMPXAttribute>& aAttributes,
        TInt& aError, TBool aFirstItem) const
    {
    TRACER("CGlxAttributeContext::AddItemAttributesL");
    
    TInt attributesAdded = EFalse;

    const TGlxMedia& m = aList->Item(aIndexInList);
    if (!m.IsStatic())
    	{
    	const CGlxMedia* media = m.Properties();
	    TInt attributeCount = iAttributes.Count();
	    if (!media)
	        {
	        // There are no attributes for this item
	        // Add all attributes
	        for (TInt count = 0; count < attributeCount; ++count)
	            {
	            AddItemAttributeL(aAttributes, iAttributes[count]);
	            }
	
	        if ( attributeCount > 0 )
	            {
	            attributesAdded = ETrue;
	            }
	        }
	    else
	        {
	        if ( GlxErrorManager::HasError(media) )
	            {
	            // Item has one or more errors on its attributes
	            // Check if we are going to try to add an attribute that may have an error on it
	            for (TInt count = 0; (count < attributeCount) && (KErrNone == aError); ++count)
	                {
	                aError = GlxErrorManager::HasAttributeErrorL(media, iAttributes[count]);
	                }
	            }
	
	        // Always allow attribute adding if it's the first item
	        if ( KErrNone == aError || aFirstItem )
	            {
	            // There may be some attributes for this item
	            // Add attributes that don't exist yet
	            for (TInt count = 0; count < attributeCount; ++count)
	                {
	                const TMPXAttribute& attribute = iAttributes[count];
	                if (!media->IsSupported(attribute))
	                    {
	                    if ( KErrNone == GlxErrorManager::HasAttributeErrorL(media, iAttributes[count]) )
	                        {
	                        AddItemAttributeL(aAttributes, attribute);
	                        attributesAdded = ETrue;
	                        }
	                    }
	                }
	            }
	        }
    	}

    return attributesAdded;
    }

// -----------------------------------------------------------------------------
// Add attribute to array, no duplicates
// -----------------------------------------------------------------------------
void CGlxAttributeContext::AddItemAttributeL(RArray<TMPXAttribute>& aAttributes, 
        const TMPXAttribute& aAttribute) const
    {
    TRACER("CGlxAttributeContext::AddItemAttributeL");
    CleanupClosePushL(aAttributes);
    TIdentityRelation<TMPXAttribute> match(&TMPXAttribute::Match);
    TInt index = aAttributes.Find(aAttribute, match);

    if (index == KErrNotFound)
        {
        aAttributes.AppendL(aAttribute);
        }
    CleanupStack::Pop(&aAttributes);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CGlxDefaultAttributeContext
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//

// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxDefaultAttributeContext* CGlxDefaultAttributeContext::NewL()
    {
    TRACER("CGlxDefaultAttributeContext::NewL");
    
    CGlxDefaultAttributeContext* obj = new (ELeave) CGlxDefaultAttributeContext();
    return obj;
    }
	
// -----------------------------------------------------------------------------
// Constructor
// Sets the iterator of base class to be TGlxFromFocusOutwardIterator
// -----------------------------------------------------------------------------
//
CGlxDefaultAttributeContext::CGlxDefaultAttributeContext() :
        CGlxAttributeContext(&iFromFocusIterator)
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxDefaultAttributeContext::~CGlxDefaultAttributeContext()
    {
    TRACER("CGlxDefaultAttributeContext::Destructor");
    
    }

// ----------------------------------------------------------------------------
// Set range offsets
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxDefaultAttributeContext::SetRangeOffsets(TInt aFrontOffset, TInt aRearOffset)
    {
    TRACER("CGlxDefaultAttributeContext::SetRangeOffsets");
    
    iFromFocusIterator.SetRangeOffsets(aRearOffset, aFrontOffset);
    }

// -----------------------------------------------------------------------------
// Sets the default fetch specification
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxAttributeContext::SetDefaultSpec(TInt aWidth, TInt aHeight)

    {
    TRACER( " CGlxAttributeContext::SetDefaultSpec");

    iDefaultSpecSize = TSize(aWidth, aHeight);
    }


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CGlxDefaultListAttributeContext
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//

// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxDefaultListAttributeContext* CGlxDefaultListAttributeContext::NewL()
    {
    TRACER("CGlxDefaultListAttributeContext::NewL");
    
    CGlxDefaultListAttributeContext* obj = new (ELeave) CGlxDefaultListAttributeContext();
    return obj;
    }
    
// -----------------------------------------------------------------------------
// Constructor
// Sets the iterator of base class to be TGlxFromFocusOutwardIterator
// -----------------------------------------------------------------------------
//
CGlxDefaultListAttributeContext::CGlxDefaultListAttributeContext() :
        CGlxAttributeContext(&iFromVisibleIndexIterator)
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxDefaultListAttributeContext::~CGlxDefaultListAttributeContext()
    {
    TRACER("CGlxDefaultListAttributeContext::Destructor");
    
    }

// ----------------------------------------------------------------------------
// Set range offsets
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxDefaultListAttributeContext::SetRangeOffsets(TInt aFrontOffset, TInt aRearOffset)
    {
    TRACER("CGlxDefaultListAttributeContext::SetRangeOffsets");
    
    iFromVisibleIndexIterator.SetRangeOffsets(aRearOffset, aFrontOffset);
    }

