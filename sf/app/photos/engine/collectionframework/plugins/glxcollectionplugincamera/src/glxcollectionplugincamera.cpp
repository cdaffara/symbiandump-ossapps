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

#include "glxcollectionplugincamera.h"

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
#include <glxplugincamera.rsg>
#include <StringLoader.h>
#include <glxmediacollectioninternaldefs.h>
#include <glxpanic.h>
#include <mglxdatasource.h>
#include <glxrequest.h>
#include <glxidlistrequest.h>
#include <glxfilterfactory.h>
#include <glxlog.h>
#include <glxtracer.h>

#include "glxcollectionplugincamera.hrh"

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
CGlxCollectionPluginCamera* CGlxCollectionPluginCamera::NewL(TAny* aObs)
    {
    CGlxCollectionPluginCamera* self = new (ELeave) CGlxCollectionPluginCamera(
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
CGlxCollectionPluginCamera::~CGlxCollectionPluginCamera()
    {
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginCamera::CGlxCollectionPluginCamera(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginCamera::ConstructL()
    {
    iDataSource = MGlxDataSource::OpenDataSourceL(KGlxDefaultDataSourceUid, *this);
	}
// ----------------------------------------------------------------------------
// CpiAttributeAdditionalAttributes
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginCamera::CpiAttributeAdditionalAttributes(const TMPXAttribute& aCpiAttribute, RArray<TMPXAttribute>& aAttributeArray)
	{
    TRACER("CGlxCollectionPluginCamera::CpiAttributeAdditionalAttributes");
    // Only need to process KGlxMediaCollectionPluginSpecificSubTitle here as all the other attributes
    // are reading straight from resource files KGlxMediaCollectionPluginSpecificSubTitle 
    // requires a image and the Video count in the collection
    
	if (aCpiAttribute == KGlxMediaCollectionPluginSpecificSubTitle)
		{
		// need to add the usage count. but check first if it is already present
		TInt attrCount = aAttributeArray.Count();
		
		TBool found = EFalse;
		
#ifdef GLX_SUB_TITLE_REL8
		
		TBool videofound = EFalse;
		TBool imagefound = EFalse;		

		for ( TInt index = 0 ; index < attrCount ; index++)
			{
			if (aAttributeArray[index] == KMPXMediaGeneralCount)
				{
				found = ETrue;	            
				}
			else if (aAttributeArray[index] == KGlxMediaItemTypeVideo)
				{
				videofound = ETrue;	            
				}
			else if(aAttributeArray[index] == KGlxMediaItemTypeImage)	            
				{
				imagefound = ETrue;
				}
				
		if (!videofound)
			{
			aAttributeArray.Append(KGlxMediaItemTypeVideo);
			}
		if (!imagefound)	        
			{
			aAttributeArray.Append(KGlxMediaItemTypeImage);
			}
		if (!found)
			{
			aAttributeArray.Append(KMPXMediaGeneralCount);
			}
				
#else

	for ( TInt index = 0 ; index < attrCount ; index++)
			{
			if (aAttributeArray[index] == KMPXMediaGeneralCount)
				{
				found = ETrue;	            
				}			
			}			
		if (!found)
			{
			aAttributeArray.Append(KMPXMediaGeneralCount);
			}       
#endif						
		}
	}

// ----------------------------------------------------------------------------
// HandleCpiAttributeResponseL
// ----------------------------------------------------------------------------
// 
void CGlxCollectionPluginCamera::HandleCpiAttributeResponseL(CMPXMedia* aResponse, TArray<TMPXAttribute> aCpiAttributes, TArray<TGlxMediaId> /* aMediaIds */)
	{
    TRACER("CGlxCollectionPluginCamera::HandleCpiAttributeResponseL");
    
    _LIT(KResourceFile, "z:glxplugincamera.rsc");

	TInt count = aCpiAttributes.Count();
	
	for (TInt index = 0; index < count ; index++)
	    {
	    const TMPXAttribute attr = aCpiAttributes[index];
	    
	    if (attr == KGlxMediaCollectionPluginSpecificSubTitle)
	        {
	        GLX_LOG_INFO("Attribute is :Subtitle");
	        
	        if (!aResponse->IsSupported(KMPXMediaGeneralCount))
	            {
	            User::Leave(KErrNotSupported);
	            }
	                	            
            else
				{
				TInt usageCount = aResponse->ValueTObjectL<TInt>(KMPXMediaGeneralCount);
				
				HBufC* tempTitle = NULL;

#ifdef GLX_SUB_TITLE_REL8

			if (!aResponse->IsSupported(KGlxMediaItemTypeImage))
	            {
	            User::Leave(KErrNotSupported);
	            }
	            
			if (!aResponse->IsSupported(KGlxMediaItemTypeVideo))
	            {
	            User::Leave(KErrNotSupported);
	            }

				TInt videoCount = aResponse->ValueTObjectL<TInt>(KGlxMediaItemTypeVideo);

				TInt imageCount = aResponse->ValueTObjectL<TInt>(KGlxMediaItemTypeImage);

				//No Images or Videos
				if ((0 == videoCount) && (0 == imageCount ))
					{	
					GLX_LOG_INFO("ImageCount and VideoCount is 0");
									
					tempTitle = LoadLocalizedStringLC(KResourceFile, R_CAMERA_SUB_TITLE_NO_IMAGE_NO_VIDEO);
					aResponse->SetTextValueL(attr, *tempTitle);	
					}

				// 1 Image and multi/0 Videos                		
				else if (1 == imageCount) 
					{
					GLX_LOG_INFO1("ImageCount is 0 and VideoCount is %d",videoCount);
					
					tempTitle = LoadLocalizedStringLC(KResourceFile, R_CAMERA_SUB_TITLE_ONE_IMAGE_MULTI_VIDEO);

					TPtr formatString = tempTitle->Des();

					// Now create a buffer that will contain the result. needs to be length of format string plus a few extra for the number
					HBufC* title = HBufC::NewLC(formatString.Length() + 10);
					TPtr ptr = title->Des();

					StringLoader::Format(ptr, formatString, -1, videoCount);

					// Set the title in the response.	
					aResponse->SetTextValueL(attr, *title);

					CleanupStack::PopAndDestroy(title);
					}

				// Multi/0 Image and 1 Video					
				else if (1 == videoCount)
					{
					GLX_LOG_INFO1("ImageCount is %d and VideoCount is 0",imageCount);
					
					tempTitle = LoadLocalizedStringLC(KResourceFile, R_CAMERA_SUB_TITLE_MULTI_IMAGE_ONE_VIDEO);                	

					TPtr formatString = tempTitle->Des();

					// Now create a buffer that will contain the result. needs to be length of format string plus a few extra for the number
					HBufC* title = HBufC::NewLC(formatString.Length() + 10);
					TPtr ptr = title->Des();

					StringLoader::Format(ptr, formatString, -1, imageCount);

					// Set the title in the response.	
					aResponse->SetTextValueL(attr, *title);

					CleanupStack::PopAndDestroy(title);
					}

				// Multi Image and Multi Video
				else 
					{
					GLX_LOG_INFO2("ImageCount is %d,VideoCount is %d",imageCount,videoCount);
					
					tempTitle = LoadLocalizedStringLC(KResourceFile, R_CAMERA_SUB_TITLE_MULTI_IMAGE_MULTI_VIDEO);                	
					TPtr formatString = tempTitle->Des();	

					HBufC* title = HBufC::NewLC(formatString.Length() + 10);
					TPtr ptr = title->Des();

					HBufC* title1 = HBufC::NewLC(formatString.Length() + 10);
					TPtr ptr1 = title1->Des();

					StringLoader::Format(ptr, formatString, 0, imageCount);
					StringLoader::Format(ptr1, ptr, 1, videoCount);	

					// Set the title in the response.	
					aResponse->SetTextValueL(attr, *title1);

					CleanupStack::PopAndDestroy(title1);
					CleanupStack::PopAndDestroy(title);
					}	
				CleanupStack::PopAndDestroy(tempTitle);

#else
				if(0 == usageCount)
            		{
                	tempTitle = LoadLocalizedStringLC(KResourceFile, R_CAMERA_SUB_TITLE_NO_IMAGE_NO_VIDEO);                	
                	// Set the title in the response.
            		aResponse->SetTextValueL(attr, *tempTitle);  
            		CleanupStack::PopAndDestroy(tempTitle);
            		continue;                	
            		}
				// Get the format string
				else if (1 == usageCount)
                    {
                    tempTitle = LoadLocalizedStringLC(KResourceFile, R_CAMERA_SUB_TITLE_SINGLE);
                    }
                else
                    {
                    tempTitle = LoadLocalizedStringLC(KResourceFile, R_CAMERA_SUB_TITLE_MULTI);
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
	        GLX_LOG_INFO("Attribute is :MediaPopupTitle");
	        
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificNewMediaItemTitle)
	        {
	        GLX_LOG_INFO("Attribute is :NewMediaItemTitle");
	        
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificDefaultMediaTitle)
	        {
	        GLX_LOG_INFO("Attribute is :DefaultMediaTitle");
	        
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KMPXMediaGeneralTitle)
	        {
	        GLX_LOG_INFO("Attribute is :GenrealTitle");
	        
	        HBufC* title = LoadLocalizedStringLC(KResourceFile, R_CAMERA_GENERAL_TITLE);
            // Set the title in the response.
            aResponse->SetTextValueL(attr, *title);  
            CleanupStack::PopAndDestroy(title); 
	        }
	    }
	aResponse->Delete(KGlxMediaItemTypeImage);
	aResponse->Delete(KGlxMediaItemTypeVideo);
	    
	}

// ----------------------------------------------------------------------------
// IsUpdateMessageIgnoredL
// ----------------------------------------------------------------------------
// 
TBool CGlxCollectionPluginCamera::IsUpdateMessageIgnoredL(CMPXMessage& /*aMessage*/)
	{
	TRACER("CGlxCollectionPluginCamera::IsUpdateMessageIgnoredL");
	
	TBool ignore = EFalse;
	return ignore;
	}
	
// ----------------------------------------------------------------------------
// DefaultFilter
// ----------------------------------------------------------------------------
// 
TGlxFilterProperties CGlxCollectionPluginCamera::DefaultFilter(TInt aLevel)
    {
    TRACER("CGlxCollectionPluginCamera::DefaultFilter");
    
    __ASSERT_DEBUG(( (aLevel == KGlxCollectionRootLevel) || (aLevel == KGlxCollectionRootLevel+1) 
    || (aLevel == KGlxCollectionRootLevel+2) ), Panic(EGlxPanicInvalidPathLevel));
    TGlxFilterProperties filterProperties;
    
    filterProperties.iSortOrder = EGlxFilterSortOrderModifiedDate;
    filterProperties.iSortDirection = EGlxFilterSortDirectionDescending;
    
   filterProperties.iOrigin = EGlxFilterOriginCamera;

    return filterProperties;
    }	
    
// End of file
