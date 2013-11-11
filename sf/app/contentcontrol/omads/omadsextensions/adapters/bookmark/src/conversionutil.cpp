/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Conversion utility
*
*/

#include <feedsserverfolderitem.h>
#include "conversionutil.h"
#include "omadsfolderobject.h"
#include "vbookmarkconverter.h"
#include "logger.h"


// -----------------------------------------------------------------------------
// NewL
// Creates the object
// ----------------------------------------------------------------------------- 
CBookmarkConversionUtil* CBookmarkConversionUtil::NewL()
    {
    CBookmarkConversionUtil* self = CBookmarkConversionUtil::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// NewLC
// Creates the object and leaves it into the cleanup stack
// ----------------------------------------------------------------------------- 
CBookmarkConversionUtil* CBookmarkConversionUtil::NewLC()
    {
    CBookmarkConversionUtil* self = new ( ELeave ) CBookmarkConversionUtil();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// ConstructL
// 2nd phase constructor
// ----------------------------------------------------------------------------- 
void CBookmarkConversionUtil::ConstructL()
    {
    // Create OMA DS folder object converter
    iOmaConverter = COMADSFolderObject::NewL();

    // Create vBookmark converter
    iVBookmarkConverter = CVBookmarkConverter::NewL();    
    }

// -----------------------------------------------------------------------------
// ~CBookmarkConversionUtil
// Destroys all allocated resources
// ----------------------------------------------------------------------------- 
CBookmarkConversionUtil::~CBookmarkConversionUtil()
    {
    delete iOmaConverter;
    iOmaConverter = NULL;
    
    delete iVBookmarkConverter;
    iVBookmarkConverter = NULL;
    }



// -----------------------------------------------------------------------------
// FavouritesItemToBufferL
// Converts CFavouritesItem to it's binary sync representation
// -----------------------------------------------------------------------------  
void CBookmarkConversionUtil::FavouritesItemToBufferL(
    const CFavouritesItem& aItem, CBufFlat& aBuffer)
    {
    LOGGER_ENTERFN( "CBookmarkConversionUtil::FavouritesItemToBufferL" );
    
    if( aItem.Type() == CFavouritesItem::EFolder )
        {
        // Set input data
        iOmaConverter->ImportDbItem(aItem);
        
        // Export to buffer
        iOmaConverter->ExportFolderXmlL(aBuffer);
        }
    else if( aItem.Type() == CFavouritesItem::EItem )
        {
        // Set input data
        iVBookmarkConverter->ImportDbItemL(aItem);
        
        // Export to buffer
        iVBookmarkConverter->ExportVBookmarkL(aBuffer);
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// RssItemToBufferL
// Converts CFolderItem to it's binary sync representation
// -----------------------------------------------------------------------------  
void CBookmarkConversionUtil::RssItemToBufferL(
    const CFolderItem& aItem, CBufFlat& aBuffer)
    {
    LOGGER_ENTERFN( "CBookmarkConversionUtil::RssItemToBufferL" );
    
    if( aItem.IsFolder() )
        {
        // Set input data
        iOmaConverter->ImportDbItem(aItem);
        
        // Export to buffer
        iOmaConverter->ExportFolderXmlL(aBuffer);
        }
    else
        {
        // Set input data
        iVBookmarkConverter->ImportDbItemL(aItem);
        
        // Export to buffer
        iVBookmarkConverter->ExportVBookmarkL(aBuffer);
        }
    }

// -----------------------------------------------------------------------------
// BufferToFavouritesItemL
// Converts Binary data to CFavouritesItem
// -----------------------------------------------------------------------------      
TInt CBookmarkConversionUtil::BufferToFavouritesItemL(CFavouritesItem::TType aType,
    CBufFlat& aBuffer, CFavouritesItem& aItem)
    {
    LOGGER_ENTERFN( "CBookmarkConversionUtil::BufferToFavouritesItemL" );
    
    if ( aType == CFavouritesItem::EFolder )
        {
        // Import XML data
        TInt error = iOmaConverter->ImportFolderXml( aBuffer.Ptr( 0 ) );
        
        // Convert to CFavouritesItem
        if ( error == KErrNone )
            {
            iOmaConverter->ExportDbItemL( aItem );
            }
     
        return error;
        }
    else if ( aType == CFavouritesItem::EItem )
        {
        // Import vBookmark data
        TInt error = iVBookmarkConverter->ImportVBookmarkL( aBuffer.Ptr( 0 ) );
        
        // Convert to CFavouritesItem
        if ( error == KErrNone )
            {
            iVBookmarkConverter->ExportDbItemL( aItem );
            }
        
        return error;
        }
    else
        {
        return KErrNotSupported;
        }
    }
