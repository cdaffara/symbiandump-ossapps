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
* Description:   Command object factory
*
*/


#include <glxlog.h>
#include "glxfilterfactory.h"

// ---------------------------------------------------------------------------
// Creates a General filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateItemTypeFilterL(TGlxFilterItemType aItemType)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iItemType = aItemType;
    return CreateCombinedFilterL(filterProperties);
    }

// ---------------------------------------------------------------------------
// Creates a eclude drm image filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateExcludeDrmImageTypeFilterL(TGlxFilterItemType aFilterType)
    {
    TGlxFilterProperties filterProperties;
	filterProperties.iItemType = EGlxFilterImage;
	filterProperties.iNoDRM = ETrue;
    return CreateCombinedFilterL(filterProperties);
    }

// ---------------------------------------------------------------------------
// Creates a URI filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateURIFilterL(const TDesC& aURI)
    {
    GLX_FUNCL("TGlxFilterFactory::CreateURIFilterL()");
    
    TGlxFilterProperties filterProperties;
    filterProperties.iUri = &aURI;
    return CreateCombinedFilterL(filterProperties);
    }
    
// ---------------------------------------------------------------------------
// Creates an alpabetic sort filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateAlphabeticSortFilterL(const TGlxFilterSortDirection aSortDirection)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iSortOrder = EGlxFilterSortOrderAlphabetical;
    filterProperties.iSortDirection = aSortDirection;
    return CreateCombinedFilterL(filterProperties);
    }
    
// ---------------------------------------------------------------------------
// Creates an item count sort filter object. The containers with the greatest
// number of items are listed first
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateItemCountSortFilterL(const TGlxFilterSortDirection aSortDirection)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iSortOrder = EGlxFilterSortOrderItemCount;
    filterProperties.iSortDirection = aSortDirection;
    return CreateCombinedFilterL(filterProperties);
    }
    
// ---------------------------------------------------------------------------
// Creates a capture date sort filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateCaptureDateSortFilterL(const TGlxFilterSortDirection aSortDirection)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
    filterProperties.iSortDirection = aSortDirection;
    return CreateCombinedFilterL(filterProperties);
    }
    
// ---------------------------------------------------------------------------
// Creates a modified date sort filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateModifiedDateFilterL(const TGlxFilterSortDirection aSortDirection)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iSortOrder = EGlxFilterSortOrderModifiedDate;
    filterProperties.iSortDirection = aSortDirection;
    return CreateCombinedFilterL(filterProperties);
    }
    
// ---------------------------------------------------------------------------
// Creates a camera album exclusion filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateCameraAlbumExclusionFilterL()
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iIncludeCameraAlbum = EFalse;
    return CreateCombinedFilterL(filterProperties);
    }
    
// ---------------------------------------------------------------------------
// Creates an empty container exclusion filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateIncludeEmptyContainersFilterL(CMPXMedia* aOriginalFilter)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iMinCount = -1;
    return CreateCombinedFilterL(filterProperties, aOriginalFilter, ETrue);
    }

// ---------------------------------------------------------------------------
// Creates aa filter that excludes all caontainers that does not contain a
// specified item id.
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateExcludeContainersWithoutItemFilterL(TGlxMediaId aItemId)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iContainsItem = aItemId;
    return CreateCombinedFilterL(filterProperties);
    }

// ---------------------------------------------------------------------------
// Creates a last capture date filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateLastCaptureDateFilterL()
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
    filterProperties.iSortDirection = EGlxFilterSortDirectionAscending;
    filterProperties.iLastCaptureDate = ETrue;
    return CreateCombinedFilterL(filterProperties);
    }

// ---------------------------------------------------------------------------
// Creates a preview filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreatePreviewFilterL()
    {
    TGlxFilterProperties filterProperties;
    // Exclude empty containers
    filterProperties.iMinCount = 1;
    // If sorting on date, override sort direction to descending
    filterProperties.iSortDirection = EGlxFilterSortDirectionOverrideToDescendingIfDate;
    return CreateCombinedFilterL(filterProperties);
    }

// ---------------------------------------------------------------------------
// Creates a last capture date filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateExcludeAnimationFilterL(const TGlxFilterSortDirection aSortDirection)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iItemType = EGlxFilterImage;
    filterProperties.iExcludeAnimation = ETrue;
    filterProperties.iSortDirection = aSortDirection;
    return CreateCombinedFilterL(filterProperties);
    }

// ---------------------------------------------------------------------------
// Creates a thumbnail loadibility filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateThumbnailLoadabilityFilterL(TSize aSize)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iThumbnailLoadability = aSize;
    return CreateCombinedFilterL(filterProperties);
    }

// ---------------------------------------------------------------------------
// Creates a filter most suited for the SlideShow
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateSlideShowFilterL( CMPXCollectionPath* aSelectedListPath,
                                                                TGlxFilterSortDirection aSortDirection,
                                                                TGlxFilterSortOrder aSortFilter)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iSortOrder = aSortFilter;
    filterProperties.iSortDirection = aSortDirection;
    filterProperties.iItemType = EGlxFilterImage;
    filterProperties.iPath = aSelectedListPath;
    filterProperties.iNoDRM = ETrue;;
    filterProperties.iExcludeAnimation = ETrue;
    return CreateCombinedFilterL(filterProperties);
    }

// ---------------------------------------------------------------------------
// Creates a filter most suited for the SlideShow
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateSlideShowFilterFromExistingFilterL(   CMPXFilter* aOriginalFilter,
                                                                                    CMPXCollectionPath* aSelectedListPath,
                                                                                    TBool aReverseSortDirection)
    {     
    TGlxFilterProperties filterProperties;
    filterProperties.iSortDirection = aReverseSortDirection ? EGlxFilterSortDirectionReverse : EGlxFilterSortDirectionNotUsed;
    filterProperties.iItemType = EGlxFilterImage;
    filterProperties.iPath = aSelectedListPath;
    filterProperties.iNoDRM = ETrue;;
    filterProperties.iExcludeAnimation = ETrue;;
    return CreateCombinedFilterL(filterProperties, aOriginalFilter);
    }

// ---------------------------------------------------------------------------
// Creates a URI filter object
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateMimeTypeFilterL(const TDesC& aMimeType)
    {
    TGlxFilterProperties filterProperties;
    filterProperties.iItemType = EGlxFilterImage;
    filterProperties.iNoDRM = ETrue;
    filterProperties.iMimeType = &aMimeType;
    return CreateCombinedFilterL(filterProperties);
    }

// ---------------------------------------------------------------------------
// Creates a combined filter object.
// ---------------------------------------------------------------------------
//   
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateCombinedFilterL(  const TGlxFilterProperties& aFilterProperties,
                                                                CMPXFilter* aOriginalFilter,
                                                                TBool aOverrideOriginal)
    {
    GLX_FUNCL("TGlxFilterFactory::CreateCombinedFilterL()");
    
    CMPXFilter* filter = CMPXFilter::NewL();
    CleanupStack::PushL(filter);

    // to prevent needing to check if aOriginalFilter is not NULL each time,
    // if it is we point it to filter, which supports nothing (support is checked only before setting)
    if( !aOriginalFilter )
    	{
    	aOriginalFilter = filter;
    	}
    	
    TGlxFilterItemType itemType = aFilterProperties.iItemType;
    if( ( EGlxFilterMediaTypeSuppliedInFilter == aFilterProperties.iItemType ) || !aOverrideOriginal )
        {
        if( aOriginalFilter->IsSupported(KGlxFilterGeneralItemType) )
            {
            itemType = aOriginalFilter->ValueTObjectL<TGlxFilterItemType>(KGlxFilterGeneralItemType);
            }
        }
                                                                                              
    filter->SetTObjectValueL<TGlxFilterItemType>(KGlxFilterGeneralItemType, itemType);
        

	TGlxFilterSortOrder sortFilter = aFilterProperties.iSortOrder;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralSortType) )
        {
        if( !aOverrideOriginal || ( EGlxFilterSortOrderNotUsed == aFilterProperties.iSortOrder ) )
        	{
        	sortFilter = aOriginalFilter->ValueTObjectL<TGlxFilterSortOrder>(KGlxFilterGeneralSortType);
        	}
        }
    if( EGlxFilterSortOrderNotUsed != sortFilter )
        {
        filter->SetTObjectValueL<TGlxFilterSortOrder>(KGlxFilterGeneralSortType, sortFilter);
        }

       
 	TGlxFilterSortDirection sortDirection = aFilterProperties.iSortDirection;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralSortDirection) )
        {
        if( !aOverrideOriginal || ( EGlxFilterSortDirectionNotUsed == aFilterProperties.iSortDirection ) )
        	{
        	sortDirection = aOriginalFilter->ValueTObjectL<TGlxFilterSortDirection>(KGlxFilterGeneralSortDirection);
        	TBool descendIfDate = ( EGlxFilterSortDirectionOverrideToDescendingIfDate == sortDirection ) && ( ( EGlxFilterSortOrderCaptureDate == sortFilter ) || ( EGlxFilterSortOrderModifiedDate == sortFilter ) );
        	if( EGlxFilterSortDirectionReverse == sortDirection )
        		{
        		if( EGlxFilterSortDirectionDescending == aFilterProperties.iSortDirection )
        			{
        			sortDirection = EGlxFilterSortDirectionAscending;
        			}
        		else if( EGlxFilterSortDirectionAscending == aFilterProperties.iSortDirection )
        			{
         			sortDirection = EGlxFilterSortDirectionDescending;
        			}
        		}
        	else if( descendIfDate )
        	    {
         		sortDirection = EGlxFilterSortDirectionDescending;
        	    }
        	else if( EGlxFilterSortDirectionOverrideToDescendingIfDate == sortDirection )
        	    {
        	    sortDirection = aFilterProperties.iSortDirection;
        	    }
        	}
        else if( aOverrideOriginal && ( EGlxFilterSortDirectionReverse == aFilterProperties.iSortDirection ) )
        	{
        	switch(aOriginalFilter->ValueTObjectL<TGlxFilterSortDirection>(KGlxFilterGeneralSortDirection))
        		{
        		case EGlxFilterSortDirectionNotUsed:
        		case EGlxFilterSortDirectionReverse:
        			{
        			sortDirection = EGlxFilterSortDirectionReverse;
        			break;
        			}
        		case EGlxFilterSortDirectionAscending:
         			{
        			sortDirection = EGlxFilterSortDirectionDescending;
        			break;
        			}
	       		case EGlxFilterSortDirectionDescending:
        			{
        			sortDirection = EGlxFilterSortDirectionAscending;
        			break;
        			}
        		}
        	}
        }
 	if( EGlxFilterSortDirectionNotUsed != sortDirection )
        {
        filter->SetTObjectValueL<TGlxFilterSortDirection>(KGlxFilterGeneralSortDirection, sortDirection);        
        }
  
	TBool cameraAlbumFilter = aFilterProperties.iIncludeCameraAlbum;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralIncludeCameraAlbum) )
        {
        if( !aOverrideOriginal || !aFilterProperties.iIncludeCameraAlbum )
        	{
        	cameraAlbumFilter = aOriginalFilter->ValueTObjectL<TBool>(KGlxFilterGeneralIncludeCameraAlbum);
        	}
        }
    if( !cameraAlbumFilter )
        {
        filter->SetTObjectValueL<TBool>(KGlxFilterGeneralIncludeCameraAlbum, cameraAlbumFilter);        
        }

        
    TInt minCount = aFilterProperties.iMinCount;    
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralMinCount) )
        {
        if( !aOverrideOriginal || ( 0 == aFilterProperties.iMinCount ) )
        	{
	        minCount = aOriginalFilter->ValueTObjectL<TInt>(KGlxFilterGeneralMinCount);
        	}
        }
    if( !(minCount == 0) )
        {
        filter->SetTObjectValueL<TInt>(KGlxFilterGeneralMinCount, minCount);
        }
   
    
    TGlxMediaId itemId = aFilterProperties.iContainsItem;    
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralItemId) )
        {
        if( !aOverrideOriginal || ( TGlxMediaId(0) == aFilterProperties.iContainsItem ) )
        	{
	        itemId = (TGlxMediaId)(TUint32)aOriginalFilter->ValueTObjectL<TMPXItemId>(KGlxFilterGeneralItemId);
        	}
        }
    if( TGlxMediaId(0) != itemId )
        {
        filter->SetTObjectValueL<TMPXItemId>(KGlxFilterGeneralItemId, itemId.Value());        
        }
        
        
	TBool lastCaptureDateFilter = aFilterProperties.iLastCaptureDate;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralLastCaptureDate) )
        {
        if( !aOverrideOriginal || !aFilterProperties.iLastCaptureDate )
        	{
        	lastCaptureDateFilter = aOriginalFilter->ValueTObjectL<TBool>(KGlxFilterGeneralLastCaptureDate);
        	}
        }
    if( lastCaptureDateFilter )
        {
        filter->SetTObjectValueL<TBool>(KGlxFilterGeneralLastCaptureDate, lastCaptureDateFilter);        
        }
        
	TBool excludeAnimation = aFilterProperties.iExcludeAnimation;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralExcludeAnimation) )
        {
        if( !aOverrideOriginal || !aFilterProperties.iExcludeAnimation )
        	{
        	excludeAnimation = aOriginalFilter->ValueTObjectL<TBool>(KGlxFilterGeneralExcludeAnimation);
        	}
        }
    if( excludeAnimation )
        {
        filter->SetTObjectValueL<TBool>(KGlxFilterGeneralExcludeAnimation, excludeAnimation);
        }
        
	TBool noDRM = aFilterProperties.iNoDRM;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralNoDRM) )
        {
        if( !aOverrideOriginal || !aFilterProperties.iNoDRM )
        	{
        	noDRM = aOriginalFilter->ValueTObjectL<TBool>(KGlxFilterGeneralNoDRM);
        	}
        }
    if( noDRM )
        {
        filter->SetTObjectValueL<TBool>(KGlxFilterGeneralNoDRM, noDRM);
        }
        
	TSize thumbnailLoadability = aFilterProperties.iThumbnailLoadability;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralThumbnailLoadability) )
        {
        if( !aOverrideOriginal || ( TSize(0,0) == aFilterProperties.iThumbnailLoadability ) )
        	{
        	thumbnailLoadability = aOriginalFilter->ValueTObjectL<TSize>(KGlxFilterGeneralThumbnailLoadability);
        	}
        }
    if( TSize(0,0) != thumbnailLoadability )
        {
        filter->SetTObjectValueL<TSize>(KGlxFilterGeneralThumbnailLoadability, thumbnailLoadability);    
        }
        
	CMPXCollectionPath* path = aFilterProperties.iPath;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralMPXCollectionPath) )
        {
        if( !aOverrideOriginal || !aFilterProperties.iPath )
        	{
        	path = aOriginalFilter->ValueCObjectL<CMPXCollectionPath>(KGlxFilterGeneralMPXCollectionPath);
        	}
        }
    if( path )
        {
        filter->SetCObjectValueL<CMPXCollectionPath>(KGlxFilterGeneralMPXCollectionPath, path);    
        }
   
	TBool promoteSystemItems = aFilterProperties.iPromoteSystemItems;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralSortOrderPromoteSystemItems) )
        {
        if( !aOverrideOriginal || !aFilterProperties.iPromoteSystemItems )
        	{
        	promoteSystemItems = aOriginalFilter->ValueTObjectL<TBool>(KGlxFilterGeneralSortOrderPromoteSystemItems);
        	}
        }
    if( promoteSystemItems )
        {
        filter->SetTObjectValueL<TBool>(KGlxFilterGeneralSortOrderPromoteSystemItems, promoteSystemItems);
        }
   
	TGlxFilterOrigin origin = aFilterProperties.iOrigin;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralOrigin) )
        {
        if( !aOverrideOriginal || ( EGlxFilterOriginNotUsed == aFilterProperties.iOrigin ) )
        	{
        	origin = aOriginalFilter->ValueTObjectL<TGlxFilterOrigin>(KGlxFilterGeneralOrigin);
        	}
        }
    if( EGlxFilterOriginNotUsed != origin )
        {
        filter->SetTObjectValueL<TGlxFilterOrigin>(KGlxFilterGeneralOrigin, origin);
        }
 
	const TDesC* uri = aFilterProperties.iUri;
	if( aOriginalFilter->IsSupported(KGlxFilterGeneralUri) )
        {
        if( !aOverrideOriginal || ( !aFilterProperties.iUri ) )
        	{
        	uri = &aOriginalFilter->ValueText(KGlxFilterGeneralUri);
        	}
        }
    if( uri )
        {
        filter->SetTextValueL(KGlxFilterGeneralUri, *uri);    
        }
    const TDesC* mimeType = aFilterProperties.iMimeType;
    if( aOriginalFilter->IsSupported(KGlxFilterGeneralMimeType) )
        {
        if( !aOverrideOriginal || ( !aFilterProperties.iMimeType ) )
            {
	        mimeType = &aOriginalFilter->ValueText(KGlxFilterGeneralMimeType);
            }
        }
    if( mimeType )
        {
        filter->SetTextValueL(KGlxFilterGeneralMimeType, *mimeType);    
        }
    
    CleanupStack::Pop(filter);
    return filter;
    }
     
// ---------------------------------------------------------------------------
// Creates an empty container(for albums with no images) exclusion filter object
// ---------------------------------------------------------------------------
//    
EXPORT_C CMPXFilter* TGlxFilterFactory::CreateExcludeEmptyContainersFilterL(CMPXMedia* aOriginalFilter)
	{
	TGlxFilterProperties filterProperties;
	filterProperties.iMinCount = 1;
	return CreateCombinedFilterL(filterProperties, aOriginalFilter, ETrue);
	}
	
	
EXPORT_C TGlxFilterProperties TGlxFilterFactory::ExtractAttributes(CMPXFilter* aFilter)
    {
    TGlxFilterProperties filterProperties;
    // Now get the KGlxFilterGeneralibutes specified in the filter
    if (aFilter->IsSupported(KGlxFilterGeneralItemType))
        {
        filterProperties.iItemType = aFilter->ValueTObjectL<TGlxFilterItemType>(KGlxFilterGeneralItemType);
        }
    if (aFilter->IsSupported(KGlxFilterGeneralSortType))
        {
        filterProperties.iSortOrder = aFilter->ValueTObjectL<TGlxFilterSortOrder>(KGlxFilterGeneralSortType);
        }
    if (aFilter->IsSupported(KGlxFilterGeneralSortDirection))
        {
        filterProperties.iSortDirection = aFilter->ValueTObjectL<TGlxFilterSortDirection>(KGlxFilterGeneralSortDirection);
        }
    if (aFilter->IsSupported(KGlxFilterGeneralIncludeCameraAlbum))
        {
        filterProperties.iIncludeCameraAlbum = aFilter->ValueTObjectL<TBool>(KGlxFilterGeneralIncludeCameraAlbum);
        }
    if (aFilter->IsSupported(KGlxFilterGeneralMinCount))
        {
        filterProperties.iMinCount = aFilter->ValueTObjectL<TInt>(KGlxFilterGeneralMinCount);
        }
    if (aFilter->IsSupported(KGlxFilterGeneralItemId))
        {
        filterProperties.iContainsItem = (TGlxMediaId)(TUint32)aFilter->ValueTObjectL<TMPXItemId>(KGlxFilterGeneralItemId);
        }
    if (aFilter->IsSupported(KGlxFilterGeneralExcludeAnimation))
        {
        filterProperties.iExcludeAnimation =  aFilter->ValueTObjectL<TBool>(KGlxFilterGeneralExcludeAnimation);
        }
    if (aFilter->IsSupported(KGlxFilterGeneralNoDRM))
        {
        filterProperties.iNoDRM =  aFilter->ValueTObjectL<TBool>(KGlxFilterGeneralNoDRM);
        }
    if (aFilter->IsSupported(KGlxFilterGeneralLastCaptureDate))
        {
        filterProperties.iLastCaptureDate = aFilter->ValueTObjectL<TBool>(KGlxFilterGeneralLastCaptureDate);
        }
    if ( aFilter->IsSupported( KGlxFilterGeneralThumbnailLoadability ) )
        {
        filterProperties.iThumbnailLoadability = aFilter->ValueTObjectL<TSize>(
                                    KGlxFilterGeneralThumbnailLoadability );
        }
    if (aFilter->IsSupported(KGlxFilterGeneralMPXCollectionPath))
        {
        filterProperties.iPath = aFilter->ValueCObjectL<CMPXCollectionPath>(KGlxFilterGeneralMPXCollectionPath);
        }
    if( aFilter->IsSupported(KGlxFilterGeneralSortOrderPromoteSystemItems) )
        {
        filterProperties.iPromoteSystemItems = aFilter->ValueTObjectL<TBool>(KGlxFilterGeneralSortOrderPromoteSystemItems);
        }
    if( aFilter->IsSupported(KGlxFilterGeneralOrigin) )
        {
        filterProperties.iOrigin = aFilter->ValueTObjectL<TGlxFilterOrigin>(KGlxFilterGeneralOrigin);
        }
    if( aFilter->IsSupported(KGlxFilterGeneralUri) )
        {
        filterProperties.iUri = &aFilter->ValueText(KGlxFilterGeneralUri);
        }
    if( aFilter->IsSupported(KGlxFilterGeneralMimeType) )
        {
        filterProperties.iMimeType = &aFilter->ValueText(KGlxFilterGeneralMimeType);
        }
    return filterProperties;
    }

