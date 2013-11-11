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
* Description:   This class browses file system
*
*/



// INCLUDE FILES
#include <e32cmn.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxcmn.h>
#include <mpxcollectionmessagedefs.h>
#include <glxcollectionmessagedefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <glxpluginalbums.rsg>
#include <StringLoader.h>
#include <glxtracer.h> 

#include "glxcollectionpluginalbums.h"
#include "glxcollectionpluginalbums.hrh"
#include <glxmediageneraldefs.h>
#include <glxmediacollectioninternaldefs.h>
#include <glxcollectiongeneraldefs.h>
#include <hbtextresolversymbian.h>

#include <glxpanic.h>
#include <mglxdatasource.h>
#include <glxrequest.h>
#include <glxidlistrequest.h>
#include <glxfilterfactory.h>
#include <glxlog.h>

#include "glxsymbianlocalisationstrings.h"

// CONSTANTS
const TInt KFavoriteAlbumId = 1 ;
const TInt KCapturedAlbumId = 2 ;


/**
 * @internal reviewed 14/06/2007 by Alex Birkett
 */


// ============================ LOCAL FUNCTIONS ==============================

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginAlbums* CGlxCollectionPluginAlbums::NewL(TAny* aObs)
    {
    CGlxCollectionPluginAlbums* self = new (ELeave) CGlxCollectionPluginAlbums(
                            static_cast<MMPXCollectionPluginObserver*>(aObs));
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginAlbums::~CGlxCollectionPluginAlbums()
    {
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginAlbums::CGlxCollectionPluginAlbums(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginAlbums::ConstructL()
    {
    iDataSource = MGlxDataSource::OpenDataSourceL(KGlxDefaultDataSourceUid, *this);
    initializeTextResolverSymbian();
    }

void CGlxCollectionPluginAlbums::CpiAttributeAdditionalAttributes(const TMPXAttribute& aCpiAttribute, RArray<TMPXAttribute>& aAttributeArray)
    {
    TRACER("CGlxCollectionPluginAlbums::CpiAttributeAdditionalAttributes");
    // Only need to process KGlxMediaCollectionPluginSpecificSubTitle here as all the others are reading straight from resource files
    // KGlxMediaCollectionPluginSpecificSubTitle requires a usage count
    if (aCpiAttribute == KGlxMediaCollectionPluginSpecificSubTitle)
        {
        // need to add the usage count. but check first if it is already present
        TInt attrCount = aAttributeArray.Count();
        TBool found = EFalse;
        
        for ( TInt index = 0 ; index < attrCount ; index++)
            {
            if ( aAttributeArray[index] == KMPXMediaGeneralCount)
                {
                found = ETrue;
                break;
                }
            }
            
        if (!found)
            {
            aAttributeArray.Append(KGlxMediaCollectionInternalUsageCount);
            }
        }
    }
void CGlxCollectionPluginAlbums::HandleCpiAttributeResponseL(CMPXMedia* aResponse, TArray<TMPXAttribute> aCpiAttributes, TArray<TGlxMediaId> aMediaIds)
    {
    TRACER("CGlxCollectionPluginAlbums::HandleCpiAttributeResponseL");
    const TInt mediaIdCount = aMediaIds.Count();
    
    switch (mediaIdCount)
        {
    case 0:
        User::Leave(KErrNotSupported);
        break;
    case 1:
        HandleCpiAttributeResponseL(aResponse, aCpiAttributes, aMediaIds[0]);
        break;
    default:
            {
            // We have an array of CMPXMedia items
            
            if (TGlxMediaId(KGlxCollectionRootId) == aMediaIds[0])
                {
                User::Leave(KErrNotSupported);
                }
                
            CMPXMediaArray* mediaArray = aResponse->ValueCObjectL<CMPXMediaArray>(KMPXMediaArrayContents);
            CleanupStack::PushL(mediaArray);

            const TInt arrayCount = mediaArray->Count();
            
            // Sanity check
            if (arrayCount != mediaIdCount)
                {
                User::Leave(KErrArgument);
                }
            
            for (TInt index = 0; index < arrayCount; index++)
                {
                HandleCpiAttributeResponseL((*mediaArray)[index], aCpiAttributes, aMediaIds[index]);
                }

            aResponse->SetCObjectValueL(KMPXMediaArrayContents, mediaArray);
            CleanupStack::PopAndDestroy(mediaArray);
            }
        break;
        }
    }

void CGlxCollectionPluginAlbums::HandleCpiAttributeResponseL(CMPXMedia* aResponse, TArray<TMPXAttribute> aCpiAttributes, TGlxMediaId aMediaId)
    {
    _LIT(KResourceFile, "z:glxpluginalbums.rsc");

    const TInt attribCount = aCpiAttributes.Count();

    for (TInt index = 0; index < attribCount ; index++)
        {
        const TMPXAttribute attr = aCpiAttributes[index];
        
        if (attr == KGlxMediaCollectionPluginSpecificSubTitle)
            {
            TInt usageCount = 0;
            if ( aResponse->IsSupported(KMPXMediaGeneralCount) )
                {
                usageCount = aResponse->ValueTObjectL<TInt>(KMPXMediaGeneralCount);
                }
            else if ( aResponse->IsSupported(KGlxMediaCollectionInternalUsageCount) )
                {
                usageCount = aResponse->ValueTObjectL<TInt>(KGlxMediaCollectionInternalUsageCount);
                aResponse->Delete(KGlxMediaCollectionInternalUsageCount);
                }
            else
                {
                User::Leave(KErrNotSupported);
                }
            HBufC* tempTitle = NULL;
            
            if (TGlxMediaId(KGlxCollectionRootId) == aMediaId)
            	{
                if (1 == usageCount)
                	{
                    tempTitle = LoadLocalizedStringLC(KResourceFile, R_ALBUM_SUB_TITLE_SINGLE);
                	}
                else
                	{
                	tempTitle = LoadLocalizedStringLC(KResourceFile, R_ALBUM_SUB_TITLE_MULTI);
                	}
            	}
            else
            	{
                tempTitle = HbTextResolverSymbian::LoadLC(KAlbumContainer, usageCount);
         
                	
                	// Set the title in the response.
            		aResponse->SetTextValueL(attr, *tempTitle);  
            		CleanupStack::PopAndDestroy(tempTitle);
            		continue;                	
            	}
            
            TPtr formatString = tempTitle->Des();
            
            // Now create a buffer that will contain the result. needs to be length of format string plus a few extra for the number
            HBufC* title = HBufC::NewLC(formatString.Length() + 10);
            TPtr ptr = title->Des();
            StringLoader::Format(ptr, formatString, -1, usageCount);
            
            // Set the title in the response.
            aResponse->SetTextValueL(attr, *title);    

            CleanupStack::PopAndDestroy(title);
            CleanupStack::PopAndDestroy(tempTitle);
            }
        else if (attr == KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle)
            {
            HBufC* title = LoadLocalizedStringLC(KResourceFile, R_ALBUM_POPUP_TITLE);
            aResponse->SetTextValueL(attr, *title);  
            CleanupStack::PopAndDestroy(title); 
            }
        else if (attr == KGlxMediaCollectionPluginSpecificNewMediaItemTitle)
            {
            HBufC* title = LoadLocalizedStringLC(KResourceFile, R_ALBUM_ITEM_TITLE);
            aResponse->SetTextValueL(attr, *title);  
            CleanupStack::PopAndDestroy(title); 
            }
        else if (attr == KGlxMediaCollectionPluginSpecificDefaultMediaTitle)
            {
            HBufC* title = LoadLocalizedStringLC(KResourceFile, R_ALBUM_DEFAULT_TITLE);
            aResponse->SetTextValueL(attr, *title);  
            CleanupStack::PopAndDestroy(title); 
            }
        else if (attr == KMPXMediaGeneralTitle)
            {
            if( TGlxMediaId(KGlxCollectionRootId) == aMediaId )
                {
                HBufC* title = LoadLocalizedStringLC(KResourceFile, R_ALBUM_GENERAL_TITLE);
                aResponse->SetTextValueL(attr, *title);  
                CleanupStack::PopAndDestroy(title); 
                }
            else
                {
                if( aResponse->IsSupported(KGlxMediaCollectionInternalSystemItemType) )
					{
					if( TGlxMediaId(KCapturedAlbumId) == aMediaId )
						{
						 HBufC* title = HbTextResolverSymbian::LoadLC(KCameraText); 
						 aResponse->SetTextValueL(attr, *title);  
						 CleanupStack::PopAndDestroy(title);						
						}
					else if (TGlxMediaId(KFavoriteAlbumId) == aMediaId  )
						{
						HBufC* title = HbTextResolverSymbian::LoadLC(KMyFavText); 
						aResponse->SetTextValueL(attr, *title);  
						CleanupStack::PopAndDestroy(title);						
						}
					}                                             
                 aResponse->Delete(KGlxMediaCollectionInternalSystemItemType);                    
                }
            }
        }
    }

TBool CGlxCollectionPluginAlbums::IsUpdateMessageIgnored(CMPXMessage& /*aMessage*/)
    {
    TBool ignore = EFalse;
    return ignore;
    }

TGlxFilterProperties CGlxCollectionPluginAlbums::DefaultFilter(TInt aLevel)
    {
    __ASSERT_DEBUG(( (aLevel == KGlxCollectionRootLevel) || (aLevel == KGlxCollectionAlbumLevel) 
    || (aLevel == KGlxCollectionAlbumContentsLevel) || (aLevel == KGlxCollectionAlbumContentsFSLevel)), 
    Panic(EGlxPanicInvalidPathLevel));
    TGlxFilterProperties filterProperties;
    filterProperties.iSortDirection = EGlxFilterSortDirectionAscending;
    switch(aLevel)
        {
        case KGlxCollectionRootLevel:
        case KGlxCollectionAlbumLevel: 
            {
            filterProperties.iPromoteSystemItems = ETrue;
            filterProperties.iSortOrder = EGlxFilterSortOrderAlphabetical;
            filterProperties.iItemType = EGlxFilterAlbum;
            break;
            }
        case KGlxCollectionAlbumContentsLevel:
            {
            filterProperties.iSortDirection = EGlxFilterSortDirectionDescending;
			filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
            break;
            }
        case KGlxCollectionAlbumContentsFSLevel:
            {
            filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
            break;
            }
        }
    return filterProperties;
    }
    
