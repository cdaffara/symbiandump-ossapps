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
* Description:    Fetch context to retrieve thumbnails
*
*/





#include <mpxmediadrmdefs.h>
#include <glxresolutionutility.h>
#include <glxdrmutility.h>
#include <glxthumbnailattributeinfo.h>
#include <mpxmediacollectiondetaildefs.h> // for KMPXMediaColDetailSpaceId
#include <glxtracer.h>
#include <glxlog.h>

#include "glxattributecontext.h"
#include "glxthumbnailcontext.h"
#include "glxmedialist.h"
#include "glxerrormanager.h"
#include "glxthumbnailutility.h"
#include "glxlistutils.h"

/**
 * Scoring for thumbnail provision order 
 * Having no thumbnail is the most important, so it gets the highest bit
 * Proximity to focus is the final (lowest priority) criteria)
 */
const TUint KTNScoreNoThumbnail                 = 0x80000000;
// bits 0 to 24 are reserved for distance from focus (excessive, but free...)
const TUint KTNScoreMaxForDistanceFromFocus 	= 0x00FFFFFF;


// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxThumbnailContext* CGlxThumbnailContext::NewL(MGlxMediaListIterator* 
                                                                    aIterator)
    {
    TRACER( "CGlxThumbnailContext::NewL");
            
    CGlxThumbnailContext* self = new(ELeave)CGlxThumbnailContext(aIterator);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxThumbnailContext::CGlxThumbnailContext(MGlxMediaListIterator* aIterator) :
        iIterator(aIterator)
    {
    TRACER( "CGlxThumbnailContext::CGlxThumbnailContext");
    
    __ASSERT_DEBUG(aIterator, Panic(EGlxPanicNullPointer));
    }
    
// -----------------------------------------------------------------------------
// ConstructL (2nd phase construction)
// -----------------------------------------------------------------------------
//
void CGlxThumbnailContext::ConstructL()    
    {
    TRACER( "CGlxThumbnailContext::ConstructL");
    iResolutionUtility = CGlxResolutionUtility::InstanceL();

    iDrmUtility = CGlxDRMUtility::InstanceL();

    // create attribute context for DRM attributes
        iDrmFetchContext = new (ELeave) CGlxAttributeContext(iIterator);
    iDrmFetchContext->AddAttributeL(KMPXMediaDrmProtected);
#ifdef RD_MDS_2_5
    iDrmFetchContext->AddAttributeL(KGlxMediaGeneralDRMRightsValid);
#endif
    iDrmFetchContext->AddAttributeL(KMPXMediaGeneralCategory);
    iDrmFetchContext->AddAttributeL(KMPXMediaGeneralUri);
    iDrmFetchContext->AddAttributeL(KMPXMediaGeneralSize);
    iDrmFetchContext->AddAttributeL(KGlxMediaGeneralLastModifiedDate);
    // add size attribute
    iDrmFetchContext->AddAttributeL(KGlxMediaGeneralDimensions); 
    // add to speed up details retrieval  
    iDrmFetchContext->AddAttributeL(KMPXMediaGeneralTitle);
    iDrmFetchContext->AddAttributeL(KMPXMediaGeneralDate);
    iDrmFetchContext->AddAttributeL(KMPXMediaGeneralDrive);
    iDrmFetchContext->AddAttributeL(KGlxMediaGeneralFramecount);
    //Need to implement IdSpaceId() functionality.
    iDrmFetchContext->AddAttributeL(KMPXMediaColDetailSpaceId);
    // Attributes required to launch Options - This is not Required in 10.1
  /*  iDrmFetchContext->AddAttributeL(KMPXMediaGeneralMimeType);   
    iDrmFetchContext->AddAttributeL(KGlxMediaGeneralLocation);  
    iDrmFetchContext->AddAttributeL(KGlxMediaGeneralSystemItem);  
    iDrmFetchContext->AddAttributeL(KGlxMediaGeneralSlideshowableContent); */    
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxThumbnailContext::~CGlxThumbnailContext()
    {
    TRACER( "CGlxThumbnailContext::~CGlxThumbnailContext");
    
    delete iDrmFetchContext;

    if ( iDrmUtility )
        {
        iDrmUtility->Close();
        }

    iSpecs.Close();




    }

// -----------------------------------------------------------------------------
// Adds a new fetch specification for an item
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxThumbnailContext::AddSpecForItemL(
                            TInt aWidth, TInt aHeight, TInt aFocusOffset )
    {
    TRACER( "CGlxThumbnailContext::AddSpecForItemL");
    
    TReal32 width( aWidth );
    TReal32 height( aHeight );

    TAssignedFetchSpec spec;
    spec.iPossWidth = iResolutionUtility->PixelsToPoss( width );
    spec.iPossHeight = iResolutionUtility->PixelsToPoss( height );
    spec.iFocusOffset = aFocusOffset;

    // Has this focus index has already been added?
    #ifdef _DEBUG
    TIdentityRelation<TAssignedFetchSpec> match (&TAssignedFetchSpec::Match);
    #endif
    __ASSERT_DEBUG(iSpecs.Find(spec, match) == KErrNotFound, Panic(EGlxPanicAlreadyAdded)); 

    iSpecs.AppendL(spec);
    }

// -----------------------------------------------------------------------------
// Sets the default fetch specification
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxThumbnailContext::SetDefaultSpec(TInt aWidth, TInt aHeight)
    {
    TRACER( "CGlxThumbnailContext::SetDefaultSpec");
    
    iDefaultSpecSize = TSize(aWidth, aHeight);
    
    TReal32 width( aWidth );
    TReal32 height( aHeight );

    iDefaultSpec.iPossWidth = iResolutionUtility->PixelsToPoss( width );
    iDefaultSpec.iPossHeight = iResolutionUtility->PixelsToPoss( height );
    }

// ---------------------------------------------------------------------------
// SetHighQualityOnly
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxThumbnailContext::SetHighQualityOnly(
                                                    TBool aHighQualityOnly )
    {
    TRACER( "CGlxThumbnailContext::SetHighQualityOnly");
    
    iHighQualityOnly = aHighQualityOnly;
    }

// -----------------------------------------------------------------------------
// Get all attributes required for the item (whether the are fetched or not)
// From MGlxFetchContext
// -----------------------------------------------------------------------------
//
void CGlxThumbnailContext::AllAttributesL(const MGlxMediaList* aList, TInt aListIndex, 
        RArray<TMPXAttribute>& aAttributes) const 
    {
    TRACER( "CGlxThumbnailContext::AllAttributesL");
    
    if (!iIterator)
        {
        return;
        }

    // add attributes from owned attribute context
    iDrmFetchContext->AllAttributesL(aList,aListIndex, aAttributes);  
      
    // No requests outside range
    iIterator->SetToFirst(aList);
    if (!iIterator->InRange(aListIndex))
        {
        return;
        }
    TBool drmValid = EFalse;
    TSize size = iDefaultSpecSize;
    const TGlxMedia& item( aList->Item( aListIndex ) );
    // DRM is Not Valid yet, can use it if its valid
	/*if( item.GetDrmProtected(drmValid) && drmValid)
        {
        drmValid = CheckDRMStatusL( aListIndex, aList, size );	
        }*/

    TLinearOrder<TMPXAttribute> orderer (&CGlxMediaList::AttributeOrder);
    // Always want high quality thumbnail
    TMPXAttribute attr( KGlxMediaIdThumbnail,
            GlxFullThumbnailAttributeId( ETrue, size.iWidth, size.iHeight ) );
    aAttributes.InsertInOrderAllowRepeatsL( attr , orderer);

    // If don't have high quality thumbnail, find closest available thumbnail
    
    const CGlxMedia* properties = item.Properties();
    if ( properties && !properties->IsSupported( attr ) )
        {
        TMPXAttribute attr2;

        if ( item.GetClosestThumbnail( attr2, size, drmValid ) )
            {
            aAttributes.InsertInOrderAllowRepeatsL( attr2 , orderer );
            }
        }

    }

// -----------------------------------------------------------------------------
// Get attributes request for the item
// From MGlxFetchContext
// -----------------------------------------------------------------------------
// 
TInt CGlxThumbnailContext::AttributeRequestL(const MGlxMediaList* aList, 
        RArray<TInt>& aItemIndices, RArray<TMPXAttribute>& aAttributes, 
        CMPXAttributeSpecs*& aDetailedSpecs) const
    {
    TRACER( "CGlxThumbnailContext::AttributeRequestL");
    
    // Fetch the visible items attribs & thumbnail first. 
    TInt error = KErrNone;
    TInt listIndex = SelectItemL(aList, error);
    if ((listIndex >= 0 && aList->Item(listIndex).Uri().Length() == 0) || 
         aList->Count() <= GlxListUtils::VisibleItemsGranularityL())
        {
        TInt reqCount = iDrmFetchContext->AttributeRequestL(aList, 
                        aItemIndices, aAttributes, aDetailedSpecs);

        // if owned context requires DRM atributes need to request those 1st
        if ( reqCount != 0 )
            {
            GLX_DEBUG2("CGlxThumbnailContext::AttributeRequestL() reqCount=%d", reqCount);
            return reqCount; 
            }
        }
    
    // Select an item to process
    error = KErrNone;
    listIndex = SelectItemL(aList, error);
    if (KErrNotFound != listIndex)
        {
        GLX_DEBUG2("CGlxThumbnailContext::AttributeRequestL() listIndex=%d", listIndex);
        const TGlxMedia& item = aList->Item( listIndex );
        TSize size = iDefaultSpecSize;
        // DRM is Not Valid yet, can use it if its valid
        /*TBool drmValid = EFalse;
        if( item.GetDrmProtected(drmValid) && drmValid)
	        {
	        CheckDRMStatusL( listIndex, aList, size );	
	        }*/

        // Request high quality thumbnail if already have any thumbnail
        TBool quality = iHighQualityOnly
                        || HasFullThumbnail( aList->Item( listIndex ) );

        GLX_DEBUG2("CGlxThumbnailContext::AttributeRequestL() quality=%d", quality);
        TMPXAttribute attr( KGlxMediaIdThumbnail,
                            GlxFullThumbnailAttributeId(
                                    quality, size.iWidth, size.iHeight ) );

        aAttributes.AppendL(attr);

        aItemIndices.AppendL(listIndex);

        // Allocate CMPXAttributeSpecs
        CMPXAttributeSpecs* attributeSpecs = CMPXAttributeSpecs::NewL();
        CleanupStack::PushL(attributeSpecs);

        attributeSpecs->SetTObjectValueL(
            TMPXAttribute( KGlxMediaIdThumbnail,
                        KGlxAttribSpecThumbnailSize ), size );

        attributeSpecs->SetTObjectValueL(
            TMPXAttribute( KGlxMediaIdThumbnail,
                        KGlxAttribSpecThumbnailQualityOverSpeed ), quality );


        aDetailedSpecs = attributeSpecs;

        // Pop from stack
        CleanupStack::Pop(attributeSpecs);
        }
    else
        {
        // If an error was found, return KErrGeneral
        if (error != KErrNone)
            {
            return KErrGeneral;
            }
        }

    return aItemIndices.Count();
    }

// -----------------------------------------------------------------------------
// Number of current requests
// From MGlxFetchContext
// -----------------------------------------------------------------------------
TInt CGlxThumbnailContext::RequestCountL(const MGlxMediaList* aList) const
    {
    TRACER( "CGlxThumbnailContext::RequestCountL");
    
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
// SelectItemL
// -----------------------------------------------------------------------------
//
TInt CGlxThumbnailContext::SelectItemL(const MGlxMediaList* aList, TInt& aError) const
    {
    TRACER( "CGlxThumbnailContext::SelectItemL");
    
    // Order of priorities when selecting the item: (highest priority rule first)
    // Items with no thumbnail before items with thumbnail
    // Items closer to the focus before

    TInt count = aList->Count();
    if (0 == count || !iIterator) 
        {
        return KErrNotFound;
        }

    // Iterate through range from iterator in search of the highest scoring thumbnail
    TUint bestScore = 0;
    TInt bestIndex = KErrNotFound;
    iIterator->SetToFirst(aList);
    TInt distanceFromFirst = 0;
    TInt i;
    while (KErrNotFound != (i = (*iIterator)++))
        {
        // Calculate the score for this item
        TUint score = GetThumbnailScoreL(i, aList, distanceFromFirst, aError);
        __ASSERT_DEBUG(score != bestScore || score == 0, Panic(EGlxPanicLogicError)); // For safety, cannot allow two items with the same score
        if (score > bestScore)
            {
            bestScore = score;
            bestIndex = i;
            }

        distanceFromFirst++;
        } 

    return bestIndex;
    }

// -----------------------------------------------------------------------------
// GetThumbnailScoreL
// -----------------------------------------------------------------------------
//
TUint CGlxThumbnailContext::GetThumbnailScoreL(TInt aListIndex, const MGlxMediaList* aList, 
        TInt aDistance, TInt& aError) const 
    {
    TRACER( "CGlxThumbnailContext::GetThumbnailScoreL");
    
    const TGlxMedia& item = aList->Item(aListIndex);
    const CGlxMedia* properties = item.Properties();
    
    // Check for the static item in the list.
    // defualt icons needs to be displayed for static items.
    if(item.IsStatic())
    	{
    	return 0;
    	}
    	
    TBool thumbnail = HasFullThumbnail( item );
    TUint score = 0;
    
    if ( properties )
        {
        TSize size = iDefaultSpecSize;
        // DRM is Not Valid yet, can use it if its valid
        /*TBool drmValid = EFalse;
        if( item.GetDrmProtected(drmValid) && drmValid)
	        {
	        CheckDRMStatusL( aListIndex, aList, size );	
	        }*/

        // Want high quality thumbnail if already have any thumbnail
        TBool quality = iHighQualityOnly || thumbnail;
        TMPXAttribute attr( KGlxMediaIdThumbnail, 
                            GlxFullThumbnailAttributeId(
                                    quality, size.iWidth, size.iHeight ) );

        // Items that are "perfect" are skipped
        if ( quality && properties->IsSupported( attr ) )
            {
            return 0;
            }

        // Items for which fetching thumbnail failed are skipped
        TInt error = GlxErrorManager::HasAttributeErrorL( properties, attr );
        if ( error != KErrNone )
        	{
            aError = error;
        	return 0;
        	}
        }

    // Score on not having any thumbnail
    if ( !thumbnail )
        {
        score |= KTNScoreNoThumbnail;
        }

    // Score on proximity to focus 
    score |= KTNScoreMaxForDistanceFromFocus - aDistance; // More distance, less points

    __ASSERT_DEBUG(score != 0, Panic(EGlxPanicLogicError));

    return score;
    }

// -----------------------------------------------------------------------------
// SpecForIndex
// -----------------------------------------------------------------------------
//
const CGlxThumbnailContext::TFetchSpec& CGlxThumbnailContext::SpecForIndex(
        TInt aListIndex, const MGlxMediaList* aList) const
    {
    TRACER( "CGlxThumbnailContext::SpecForIndex");
    
    __ASSERT_DEBUG(aList, Panic(EGlxPanicNullPointer));

    TInt count = aList->Count();
    TInt offset = Abs(aListIndex - aList->FocusIndex());

    if ( offset > count / 2 )
        {
        offset = count - offset;
        }

    // Use specific context if one exists
    TAssignedFetchSpec spec;
    spec.iFocusOffset = offset;
    TInt index = iSpecs.Find(spec, TAssignedFetchSpec::Match);
    if ( KErrNotFound != index )
        {
        return iSpecs[index];
        }

    return iDefaultSpec;
    }

// ---------------------------------------------------------------------------
// SizeFromSpec
// ---------------------------------------------------------------------------
//
TSize CGlxThumbnailContext::SizeFromSpec( const TFetchSpec& aSpec ) const
    {
    TRACER( "CGlxThumbnailContext::SizeFromSpec");
    
    TReal32 width = iResolutionUtility->PossToPixels( aSpec.iPossWidth );
    TReal32 height = iResolutionUtility->PossToPixels( aSpec.iPossHeight );

    // Round to the nearest integers
    return TSize( width + 0.5F, height + 0.5F );
    }

// -----------------------------------------------------------------------------
// Compares two specs by focus offset. 
// -----------------------------------------------------------------------------
//
TBool CGlxThumbnailContext::TAssignedFetchSpec::Match(
        const TAssignedFetchSpec& aSpec1, const TAssignedFetchSpec& aSpec2 )
    {
    TRACER( "CGlxThumbnailContext::TAssignedFetchSpec::Match");
    
    return aSpec1.iFocusOffset == aSpec2.iFocusOffset;
    }

// -----------------------------------------------------------------------------
// HasFullThumbnail
// -----------------------------------------------------------------------------
//
TBool CGlxThumbnailContext::HasFullThumbnail( const TGlxMedia& aItem ) const
    {
    TRACER( "CGlxThumbnailContext::HasFullThumbnail");
    
    const CGlxMedia* properties = aItem.Properties();

    if ( properties )
        {
        TInt count = properties->Count();
        for ( TInt i = 0; i < count; i++ )
            {
            if ( GlxThumbnailUtility::IsFullThumbnail(
                                                properties->Attribute( i ) ) )
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// Check DRM status of specified item 
// -----------------------------------------------------------------------------
//    
TBool CGlxThumbnailContext::CheckDRMStatusL( TInt aListIndex,
                            const MGlxMediaList* aList, TSize& aSize ) const
    {
    TRACER( "CGlxThumbnailContext::CheckDRMStatusL");
    
    const TGlxMedia& item = aList->Item( aListIndex );
    TBool valid = EFalse;

    aSize = SizeFromSpec( SpecForIndex( aListIndex, aList ) );

    TGlxMediaGeneralRightsValidity isValid = EGlxDrmRightsValidityUnknown;
    item.GetDrmValidity(isValid);
    if ( EGlxDrmRightsValidityUnknown == isValid )
        {
        // check rights           
        TMPXGeneralCategory cat = item.Category();
        const TDesC& uri = item.Uri();
        if ( uri.Length() && cat != EMPXNoCategory )
            {
            valid = iDrmUtility->ItemRightsValidityCheckL( uri, ( cat == EMPXImage ) );
            CGlxMedia* properties = const_cast<CGlxMedia*>(item.Properties());
            if( valid )
                {
				
                isValid = EGlxDrmRightsValid;
                }
            else
                {
				
                isValid = EGlxDrmRightsInvalid;
                }
            properties->SetTObjectValueL(KGlxMediaGeneralDRMRightsValid, isValid);
            }
        }
    else if ( EGlxDrmRightsValid == isValid )
        {	
        valid = ETrue;
        }
        if ( !valid )
            {
            // modify spec to request 'small' thumbnail
            TSize size;
            // ignore success/failure return - accept default
            item.GetDimensions(size);
            
            TSize thumbnailSize = iDrmUtility->DRMThumbnailSize( size );

            // only update spec if either dimension of required DRM thumbnail
            // is smaller than correspnding spec dimension (so don't request
            // a new thumbnail if existing one if of appropriate size )
            if( thumbnailSize.iWidth < aSize.iWidth
                || thumbnailSize.iHeight < aSize.iHeight )
                {
                aSize = thumbnailSize;
                }
        }
    return valid;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CGlxDefaultThumbnailContext
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxDefaultThumbnailContext* CGlxDefaultThumbnailContext::NewL()
    {
    TRACER( "CGlxDefaultThumbnailContext::NewL");
    
    CGlxDefaultThumbnailContext* obj = new (ELeave) CGlxDefaultThumbnailContext();
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop(obj);
    return obj;
    }
	
// -----------------------------------------------------------------------------
// Constructor
// Sets the iterator of base class to be TGlxFromFocusOutwardIterator
// -----------------------------------------------------------------------------
//
CGlxDefaultThumbnailContext::CGlxDefaultThumbnailContext() :
        CGlxThumbnailContext(&iFromFocusIterator)
    {
    TRACER( "CGlxDefaultThumbnailContext::CGlxDefaultThumbnailContext");
    
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxDefaultThumbnailContext::~CGlxDefaultThumbnailContext()
    {
    TRACER( "CGlxDefaultThumbnailContext::~CGlxDefaultThumbnailContext");
    
    }
        
// ----------------------------------------------------------------------------
// Set range offsets
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxDefaultThumbnailContext::SetRangeOffsets(TInt aFrontOffset, TInt aRearOffset)
    {
    TRACER( "CGlxDefaultThumbnailContext::SetRangeOffsets");
    
    iFromFocusIterator.SetRangeOffsets(aFrontOffset, aRearOffset);
    }
