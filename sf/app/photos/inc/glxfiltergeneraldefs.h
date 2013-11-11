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
* Description:   Definitions for media attributes
*
*/



#ifndef __T_GLXFILTERGENERALDEFS_H__
#define __T_GLXFILTERGENERALDEFS_H__

#include <e32base.h>
#include <mpxattribute.h>

/**
* Content ID identifying filter messages
*/
const TInt KGlxFilterIdGeneral  = 0x2000A777;

/**
*  Attributes supported for KGlxFilterIdGeneral
*/
enum
    {
    /**
    * Filter
    * specifying sort order
    */
    
    EGlxFilterSortType = 0x01,
    
    EGlxFilterSortDirection = 0x02,
    
    EGlxFilterIncludeCameraAlbum = 0x04,
    
    EGlxFilterMinItemCount = 0x08,
    
    EGlxFilterContainsItem = 0x10,
    
    EGlxFilterLastCaptureDate = 0x20,

    EGlxFilterExcludeAnimation = 0x40,
    
    EGlxFilterThumbnailLoadability = 0x80,
    
    EGlxFilterMPXCollectionPath = 0x100,
    
    EGlxFilterItemType = 0x200,
    
    EGlxFilterUri = 0x400,
    
    EGlxFilterSortOrderPromoteSystemItems = 0x0800,
    
    EGlxFilterOrigin = 0x1000,

    EGlxFilterNoDRM = 0x2000

    };
    
/**
*  Attributes supported for KGlxFilterIdGeneral
*/
const TMPXAttributeData KGlxFilterGeneralSortType = { KGlxFilterIdGeneral, EGlxFilterSortType }; // TGlxFilterSortOrder
const TMPXAttributeData KGlxFilterGeneralSortDirection = { KGlxFilterIdGeneral, EGlxFilterSortDirection }; // TGlxFilterSortDirection
const TMPXAttributeData KGlxFilterGeneralIncludeCameraAlbum = { KGlxFilterIdGeneral, EGlxFilterIncludeCameraAlbum }; // TBool
const TMPXAttributeData KGlxFilterGeneralMinCount = { KGlxFilterIdGeneral, EGlxFilterMinItemCount }; // TInt
const TMPXAttributeData KGlxFilterGeneralItemId = { KGlxFilterIdGeneral, EGlxFilterContainsItem }; // TMPXItemId
const TMPXAttributeData KGlxFilterGeneralExcludeAnimation = { KGlxFilterIdGeneral, EGlxFilterExcludeAnimation }; // TBool
const TMPXAttributeData KGlxFilterGeneralLastCaptureDate = { KGlxFilterIdGeneral, EGlxFilterLastCaptureDate }; // TBool ?
const TMPXAttributeData KGlxFilterGeneralThumbnailLoadability = { KGlxFilterIdGeneral, EGlxFilterThumbnailLoadability }; // TSize
const TMPXAttributeData KGlxFilterGeneralMPXCollectionPath = { KGlxFilterIdGeneral, EGlxFilterMPXCollectionPath }; // CMPXCollectionPath
const TMPXAttributeData KGlxFilterGeneralItemType = { KGlxFilterIdGeneral, EGlxFilterItemType }; // TGlxFilterItemType
const TMPXAttributeData KGlxFilterGeneralUri = { KGlxFilterIdGeneral, EGlxFilterUri }; // Text
const TMPXAttributeData KGlxFilterGeneralSortOrderPromoteSystemItems = { KGlxFilterIdGeneral, EGlxFilterSortOrderPromoteSystemItems }; // TBool
const TMPXAttributeData KGlxFilterGeneralOrigin = { KGlxFilterIdGeneral, EGlxFilterOrigin }; /// TGlxFilterOrigin
const TMPXAttributeData KGlxFilterGeneralNoDRM = { KGlxFilterIdGeneral, EGlxFilterNoDRM }; /// TBool
const TMPXAttributeData KGlxFilterGeneralNavigationalStateOnly = { KGlxFilterIdGeneral, 0x4000 }; /// TBool
const TMPXAttributeData KGlxFilterGeneralMaxCount = { KGlxFilterIdGeneral, 0x8000 }; // TInt
const TMPXAttributeData KGlxFilterGeneralMimeType = { KGlxFilterIdGeneral, 0x10000 }; // Text

enum TGlxFilterSortOrder
    {
    EGlxFilterSortOrderNotUsed,
    EGlxFilterSortOrderAlphabetical,
    EGlxFilterSortOrderItemCount,
    EGlxFilterSortOrderCaptureDate,
    EGlxFilterSortOrderModifiedDate
    };
    
enum TGlxFilterSortDirection
    {
    EGlxFilterSortDirectionNotUsed,
    EGlxFilterSortDirectionReverse,
    EGlxFilterSortDirectionAscending,
    EGlxFilterSortDirectionDescending,
    EGlxFilterSortDirectionOverrideToDescendingIfDate
    };

enum TGlxFilterItemType
    {
    EGlxFilterItemAll,
    EGlxFilterMediaTypeSuppliedInFilter,
    EGlxFilterImage,
    EGlxFilterVideo,
    EGlxFilterVideoAndImages,
    EGlxFilterAlbum,
    EGlxFilterTag,
    EGlxFilterMonth,
    EGlxFilterExcludeCamera,
    EGlxFilterFetcherMimeType
    };
    
enum TGlxFilterOrigin
	{
	EGlxFilterOriginNotUsed=0,
	EGlxFilterOriginCamera,
	EGlxFilterOriginDownload,
	EGlxFilterOriginAll
	};

#endif // __T_GLXFILTERGENERALDEFS_H__
