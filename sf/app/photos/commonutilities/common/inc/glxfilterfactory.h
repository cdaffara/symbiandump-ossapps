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



#ifndef T_GLXFILTERFACTORY_H
#define T_GLXFILTERFACTORY_H

#include <f32file.h>
#include <bautils.h>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <mpxcollectionpath.h>
#include <mpxmediageneraldefs.h>
#include <glxfiltergeneraldefs.h>
#include <glxfilterproperties.h>
#include <mpxfilter.h>
#include "glxmediaid.h"
/**
 * TGlxFilterFactory
 *
 * Factory that creates CMPXFilter objects 
 *
 * @lib glxcommonfilters.lib
 * @internal reviewed 06/06/2007 by Kimmo Hoikka
 */

class TGlxFilterFactory
    {
public:
    
    // The General category types
 

public:
    /**
    * Creates a basic filter, Used to specify Images of Video.
    *
    * @param TGlxMediaGeneralCategory ( EMPXImage or EMPXVideo)
    * @return a filter object with array elements set specifying the filter type (Images or Video)
    */
    IMPORT_C static CMPXFilter* CreateItemTypeFilterL(TGlxFilterItemType aItemType);
    /**
    * Creates a URI filter, Used to specify 1 particular item (no wild cards allowed).
    *
    * @param TGlxMediaGeneralCategory ( EMPXImage )
    * @return a filter object with array non drm images
    */
	IMPORT_C static CMPXFilter* TGlxFilterFactory::CreateExcludeDrmImageTypeFilterL(TGlxFilterItemType aFilterType);
	
    /**
    * Creates a URI filter, Used to specify 1 particular item (no wild cards allowed).
    *
    * @param const TDesC& aURI ( URI to item)
    * @return a filter object with array elements set specifying URI filter type, and the
    *           text object set to the URI
    */
    IMPORT_C static CMPXFilter* CreateURIFilterL(const TDesC& aURI);
    
    /**
    * Creates an Alphabetic Sort filter.
    *
    * @return a filter object with array elements set specifying an alphabetic sort
    */
	IMPORT_C static CMPXFilter* CreateAlphabeticSortFilterL(const TGlxFilterSortDirection aSortDirection = EGlxFilterSortDirectionAscending);

    /**
    * Creates an Item Count Sort filter. Containers will be sorted with the greatest number
    * of items first.
    *
    * @return a filter object with array elements set specifying an item count sort
    */
	IMPORT_C static CMPXFilter* CreateItemCountSortFilterL(const TGlxFilterSortDirection aSortDirection = EGlxFilterSortDirectionDescending);

    /**
    * Creates a Capture Date Sort filter. items will be sorted by capture date
    *
    * @return a filter object with array elements set specifying a capture date sort
    */
	IMPORT_C static CMPXFilter* CreateCaptureDateSortFilterL(const TGlxFilterSortDirection aSortDirection = EGlxFilterSortDirectionAscending);

    /**
    * Creates a Modified Date Sort filter. items will be sorted by modified date
    *
    * @return a filter object with array elements set specifying a modified date sort
    */
    IMPORT_C static CMPXFilter* CreateModifiedDateFilterL(const TGlxFilterSortDirection aSortDirection = EGlxFilterSortDirectionAscending);

    /**
    * Creates a System Camera Album exclusion filter.
    *
    * @return a filter object with array elements set specifying the Camera Album Exclusion
    */
    IMPORT_C static CMPXFilter* CreateCameraAlbumExclusionFilterL();

    /**
    * Creates an min count container exclusion filter.
    *
    * @return a filter object with array elements set specifying exclude empty containers
    */
    IMPORT_C static CMPXFilter* CreateIncludeEmptyContainersFilterL(CMPXMedia* aOriginalFilter = NULL);

    /**
    * Creates a filter that excludes all containers that do not contain the specified item
    *
    * @return a filter object with array elements set specifying exclude empty containers
    */
    IMPORT_C static CMPXFilter* CreateExcludeContainersWithoutItemFilterL(TGlxMediaId aItemId);
    
    /**
    * Creates a filter that only contains the last captured image/video
    *
    * @return a filter object with array elements set specifying the only the last captured image/video to be displayed
    */
    IMPORT_C static CMPXFilter* CreateLastCaptureDateFilterL();
    
    /**
    * Creates a filter that sets filters for the previews
    *
    * @return a filter object with array elements set for previews
    */
    IMPORT_C static CMPXFilter* CreatePreviewFilterL();
    
    /**
    * Creates a filter that excludes anything with more than one frame
    *
    * @return a filter object with array elements set specifying the only the last captured image/video to be displayed
    */
    IMPORT_C static CMPXFilter* CreateExcludeAnimationFilterL(const TGlxFilterSortDirection aSortDirection = EGlxFilterSortDirectionNotUsed);    
    
    /**
    * Creates a filter that only contains the thumbnails yet to be loaded
    *
    * @return a filter object with array elements set specifying the only the items for which thumbnail are yet to be loaded
    */
    IMPORT_C static CMPXFilter* CreateThumbnailLoadabilityFilterL(TSize aSize);
    
    /**
    * Creates a filter that most suited for the SlideShow
    * 
    * @param aSelectedListPath - this contains the list of Id's required. If NULL it is ignored
    * @param aSortDirection - Acsending or Descending
    * @param aSortFilter - Alphabetic, Item Count, Capture date, Modified date,
    * @param aItemTypeFilter - All, Image, Video, Video and Images, Album
    * @Param aExcludeAnimation - Exlude images with FrameCount > 1, include all images
    *
    * @return a filter object with array elements set according to filters specified
    */
    IMPORT_C static CMPXFilter* CreateSlideShowFilterL( CMPXCollectionPath* aSelectedListPath = NULL,
                                                        TGlxFilterSortDirection aSortDirection = EGlxFilterSortDirectionAscending,
                                                        TGlxFilterSortOrder aSortFilter = EGlxFilterSortOrderCaptureDate);

    /**
    * Creates a filter that most suited for the SlideShow
    * 
    * @param aOriginalFilter - Use values in this filter but overide with other parameters
    * @param aSelectedListPath - this contains the list of Id's required. If NULL it is ignored
    * @param aSortDirection - Acsending, Descending or use value in aOriginalFilter
    * @param aItemTypeFilter - All, Image, Video, Video and Images, Album or use value in aOriginalFilter
    * @Param aExcludeAnimation - Exlude images with FrameCount > 1, include all images or use value in aOriginalFilter
    *
    * @return a filter object with array elements set according to filters specified
    */
    IMPORT_C static CMPXFilter* CreateSlideShowFilterFromExistingFilterL(   CMPXFilter* aOriginalFilter,
                                                                            CMPXCollectionPath* aSelectedListPath = NULL,
                                                                            TBool aReverseSortDirection = EFalse);
    

    /**
    * Creates a MimeType  filter, Used to specify item of perticulr mime type.
    *
    * @param const TDesC& aMimeType ( mime type of item)
    * @return a filter object with array elements set specifying mime type filter type
    */
    IMPORT_C static CMPXFilter* TGlxFilterFactory::CreateMimeTypeFilterL(const TDesC& aMimeType);    

    /**
    * Creates a combined filter. items will be sorted by the filters specified
    *
    * @param aItemTypeFilter - Images / Video or All
    * @param aSortFilter - Type of sort
    * @param aSortDirection - Ascending or Descending
    * @param aCameraAlbumFilter - Exclusion of Camera System Album
    * @param aMinCountFilter - Min Count Fliter
    * @param Min Count value
    * @param aItemId, the Id of the item that containers must contain
    * @param aLastCaptureDateFilter return only the last captured item
    * @param aExcludeAnimation excludes images with more than 1 frame count
    * @param aThumbnailLoadability return a list of items for which a thumbnail has not been generated
    * @param aSize, size of thumnails for Thumbnail Loadibility filter
    * @param aSelectedListPath, supplies list of requiered Ids
    * @param aOriginalFilter, If supplied then aSortFilter, aSortDirection, aItemTypeFilter, and aExcludeAnimation can be inherited from this
    *
    * @return a filter object with array elements set as specified by the supplied filters
    */
    IMPORT_C static CMPXFilter* CreateCombinedFilterL(  const TGlxFilterProperties&	 aFilterProperties,
                                                        CMPXFilter* aOriginalFilter = NULL,
                                                        TBool aOverrideOriginal = ETrue);
                                                        
    /**
    * Creates a combined filter. items will be sorted by the filters specified
    *
    * @param aFilter - filter which may contain filter attributes
    *
    * @return filter properties populated from aFilter
    */
    IMPORT_C static TGlxFilterProperties ExtractAttributes(CMPXFilter* aFilter);

    /**
    * Creates an min count container(albums) exclusion filter.
    *
    * @return a filter object with array elements set specifying exclude empty containers
    */
    IMPORT_C static CMPXFilter* CreateExcludeEmptyContainersFilterL(CMPXMedia* aOriginalFilter = NULL);
    };

#endif

