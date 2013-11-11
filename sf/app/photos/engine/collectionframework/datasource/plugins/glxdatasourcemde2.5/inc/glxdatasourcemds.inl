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
* Description:  Inline methods for the data source MDS class
*
*/



/**
 * @internal reviewed 10/07/2007 by M Byrne
 */

#include <glxpanic.h>

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::Session
// ----------------------------------------------------------------------------
// 
CMdESession& CGlxDataSourceMde::Session() const
    {
    __ASSERT_DEBUG(iSessionOpen, Panic(EGlxPanicDatasourceMdeSessionNotOpen));
    return *iSession;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::FileServerSession
// ----------------------------------------------------------------------------
// 
//const RFs& CGlxDataSourceMde::FileServerSession() const
 RFs& CGlxDataSourceMde::FileServerSession()
    {
    return iFs;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::CameraAlbumId
// ----------------------------------------------------------------------------
// 
const TGlxMediaId& CGlxDataSourceMde::CameraAlbumId() const
    {
    return iCameraAlbumId;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::FavoritesId
// ----------------------------------------------------------------------------
// 
const TGlxMediaId& CGlxDataSourceMde::FavoritesId() const
    {
    return iFavoritesId;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::NamespaceDef
// ----------------------------------------------------------------------------
// 
CMdENamespaceDef* CGlxDataSourceMde::NamespaceDef() const
    {
    return iNameSpaceDef;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::ObjectDef
// ----------------------------------------------------------------------------
// 
CMdEObjectDef& CGlxDataSourceMde::ObjectDef() const
    {
    return *iObjectDef;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::ImageDef
// ----------------------------------------------------------------------------
// 
CMdEObjectDef& CGlxDataSourceMde::ImageDef() const
    {
    return *iImageDef;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::VideoDef
// ----------------------------------------------------------------------------
// 
CMdEObjectDef& CGlxDataSourceMde::VideoDef() const
    {
    return *iVideoDef;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::MediaDef
// ----------------------------------------------------------------------------
// 
CMdEObjectDef& CGlxDataSourceMde::MediaDef() const
    {
    return *iMediaDef;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::AlbumDef
// ----------------------------------------------------------------------------
// 
CMdEObjectDef& CGlxDataSourceMde::AlbumDef() const
    {
    return *iAlbumDef;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::TagDef
// ----------------------------------------------------------------------------
// 
CMdEObjectDef& CGlxDataSourceMde::TagDef() const
    {
    return *iTagDef;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::MonthDef
// ----------------------------------------------------------------------------
//  
CMdEObjectDef& CGlxDataSourceMde::MonthDef() const
    {
    return *iMonthDef;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceMde::ContainsDef
// ----------------------------------------------------------------------------
//    
CMdERelationDef& CGlxDataSourceMde::ContainsDef() const
    {
    return *iContainsDef;
    }

// ---------------------------------------------------------------------------
// CGlxDataSourceMde::ContainsLocationDef
// ---------------------------------------------------------------------------
//   
CMdERelationDef&  CGlxDataSourceMde::ContainsLocationDef() const
    {
    return *iContainsLocationDef;
    }

// ---------------------------------------------------------------------------
// CGlxDataSourceMde::LocationDef
// ---------------------------------------------------------------------------
//   
CMdEObjectDef&  CGlxDataSourceMde::LocationDef() const
	{
	return *iLocationDef;
	}

