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
#include "glxcollectionplugindownloads.h"

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
#include <glxmediageneraldefs.h>
#include <glxcollectiongeneraldefs.h>
#include <glxplugindownloads.rsg>
#include <StringLoader.h>
#include <glxpanic.h>
#include <mglxdatasource.h>
#include <glxrequest.h>
#include <glxidlistrequest.h>
#include <glxfilterfactory.h>
#include <glxlog.h>
#include <glxtracer.h>

#include "glxcollectionplugindownloads.hrh"
/**
 * @internal reviewed 14/06/2007 by Alex Birkett
 */

// CONSTANTS

// ============================ LOCAL FUNCTIONS ==============================
    
// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginDownloads* CGlxCollectionPluginDownloads::NewL(TAny* aObs)
    {
    CGlxCollectionPluginDownloads* self = new (ELeave) CGlxCollectionPluginDownloads(
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
CGlxCollectionPluginDownloads::~CGlxCollectionPluginDownloads()
    {
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginDownloads::CGlxCollectionPluginDownloads(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginDownloads::ConstructL()
    {
    iDataSource = MGlxDataSource::OpenDataSourceL(KGlxDefaultDataSourceUid, *this);
	}
// ----------------------------------------------------------------------------
// CpiAttributeAdditionalAttributes
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginDownloads::CpiAttributeAdditionalAttributes(const TMPXAttribute& aCpiAttribute, RArray<TMPXAttribute>& aAttributeArray)
	{
    TRACER("CGlxCollectionPluginDownloads::CpiAttributeAdditionalAttributes");
    // Only need to process KGlxMediaCollectionPluginSpecificSubTitle here as all the others are reading straight from resource files
    // KGlxMediaCollectionPluginSpecificSubTitle requires a usage count
	if (aCpiAttribute == KGlxMediaCollectionPluginSpecificSubTitle)
	    {
	    // need to add the usage count. but check first if it is already present
	    TInt attrCount = aAttributeArray.Count();
	    TBool found = EFalse;
	    
	    for ( TInt index = 0 ; index < attrCount ; index++)
	        {
	        if (aAttributeArray[index] == KMPXMediaGeneralCount)
	            {
	            found = ETrue;
	            break;
	            }
	        }
	        
	    if (!found)
	        {
	        aAttributeArray.Append(KMPXMediaGeneralCount);
	        }
	    }
	}

// ----------------------------------------------------------------------------
// HandleCpiAttributeResponseL
// ----------------------------------------------------------------------------
// 
void CGlxCollectionPluginDownloads::HandleCpiAttributeResponseL(CMPXMedia* aResponse, TArray<TMPXAttribute> aCpiAttributes, TArray<TGlxMediaId> /* aMediaIds */)
	{
    TRACER("CGlxCollectionPluginDownloads::HandleCpiAttributeResponseL");
    
    _LIT(KResourceFile, "z:glxplugindownloads.rsc");
    
	TInt count = aCpiAttributes.Count();
	
	for (TInt index = 0; index < count ; index++)
	    {
	    const TMPXAttribute attr = aCpiAttributes[index];
	    
	    if (attr == KGlxMediaCollectionPluginSpecificSubTitle)
	        {
	        GLX_LOG_INFO("Attribute : Sub Title");
	        
	        if (!aResponse->IsSupported(KMPXMediaGeneralCount))
	            {
	            User::Leave(KErrNotSupported);
	            }
            else
                {
                TInt usageCount = aResponse->ValueTObjectL<TInt>(KMPXMediaGeneralCount);
                
                // Get the format string
                HBufC* tempTitle = NULL;
                
#ifdef GLX_SUB_TITLE_REL8

				if(0 == usageCount)
					{
					GLX_LOG_INFO("Count is 0 items");
					
					tempTitle = LoadLocalizedStringLC(KResourceFile,R_DOWNLOADS_SUB_TITLE_NO_IMG_NO_VID);
					aResponse->SetTextValueL(attr, *tempTitle); 
					}
					
				else if (1 == usageCount)
					{
					GLX_LOG_INFO("Count is 1 item");
						                    
					tempTitle = LoadLocalizedStringLC(KResourceFile, R_DOWNLOADS_SUB_TITLE_SINGLE);
					aResponse->SetTextValueL(attr, *tempTitle); 
					}
				else
					{
					GLX_LOG_INFO1("Count is %d items",usageCount);
					
					tempTitle = LoadLocalizedStringLC(KResourceFile, R_DOWNLOADS_SUB_TITLE_MULTI);
					//Format the string
					TPtr formatString = tempTitle->Des();

					// Now create a buffer that will contain the result. needs to be length of format string plus a few extra for the number
					HBufC* title = HBufC::NewLC(formatString.Length() + 10);
					TPtr ptr = title->Des();
					StringLoader::Format(ptr, formatString, -1, usageCount);

					// Set the title in the response.
					aResponse->SetTextValueL(attr, *title);  
					CleanupStack::PopAndDestroy(title);
					}
                CleanupStack::PopAndDestroy(tempTitle);
#else
            	if(0 == usageCount)
            		{
                	tempTitle = LoadLocalizedStringLC(KResourceFile, R_DOWNLOADS_SUB_TITLE_NO_IMG_NO_VID);                	
                	// Set the title in the response.
            		aResponse->SetTextValueL(attr, *tempTitle);  
            		CleanupStack::PopAndDestroy(tempTitle);
            		continue;                	
            		}  
				// Get the format string
            	else if (1 == usageCount)
                    {
                    tempTitle = LoadLocalizedStringLC(KResourceFile, R_DOWNLOADS_SUB_TITLE_SINGLE);
                    }
                else
                    {
                    tempTitle = LoadLocalizedStringLC(KResourceFile, R_DOWNLOADS_SUB_TITLE_MULTI);
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
#endif                
                }
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle)
	        {
	        GLX_LOG_INFO("Attribute : MediaPopupTitle");
	        
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificNewMediaItemTitle)
	        {
	        GLX_LOG_INFO("Attribute : NewMediaItemTitle");
	        
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificDefaultMediaTitle)
	        {
	        GLX_LOG_INFO("Attribute : DefaultMediaTitle");
	        
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KMPXMediaGeneralTitle)
	        {
	        GLX_LOG_INFO("Attribute : GeneralTitle");
	        
	        HBufC* title = LoadLocalizedStringLC(KResourceFile, R_DOWNLOADS_GENERAL_TITLE);
            // Set the title in the response.
            aResponse->SetTextValueL(attr, *title);  
            CleanupStack::PopAndDestroy(title); 
	        }
	    }
	    
	}

 
// ----------------------------------------------------------------------------
// IsUpdateMessageIgnoredL
// ----------------------------------------------------------------------------
// 
TBool CGlxCollectionPluginDownloads::IsUpdateMessageIgnoredL(CMPXMessage& /*aMessage*/)
	{
	TRACER("CGlxCollectionPluginDownloads::IsUpdateMessageIgnoredL");	
	/// @todo origin could be passed up to allow downloads to ignore irrelevant adds, wait until inheritance is resolved
	TBool ignore = EFalse;
	return ignore;
	}


// ----------------------------------------------------------------------------
// DefaultFilter
// ----------------------------------------------------------------------------
// 
TGlxFilterProperties CGlxCollectionPluginDownloads::DefaultFilter(TInt aLevel)
    {
    TRACER("CGlxCollectionPluginDownloads::DefaultFilter");
    
    __ASSERT_DEBUG(( (aLevel == KGlxCollectionRootLevel) || (aLevel == KGlxCollectionRootLevel+1) 
    || (aLevel == KGlxCollectionRootLevel+2) ), Panic(EGlxPanicInvalidPathLevel));
    TGlxFilterProperties filterProperties;
    filterProperties.iSortOrder = EGlxFilterSortOrderModifiedDate;
    filterProperties.iSortDirection = EGlxFilterSortDirectionDescending;
    filterProperties.iOrigin = EGlxFilterOriginDownload;

    return filterProperties;
    }

// End of file
