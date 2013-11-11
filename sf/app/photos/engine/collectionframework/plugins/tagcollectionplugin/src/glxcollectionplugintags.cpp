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
* Description:   Tag collection plugin implementation
*
*/



// INCLUDE FILES
#include "glxcollectionplugintags.h"
#include "glxcollectionplugintags.hrh"

 
#include <mpxmediageneraldefs.h>        //KMPXMediaIdGeneral
#include <mpxmediacontainerdefs.h>      //Attributes for KMPXMediaIdContainer
#include <StringLoader.h>
#include <glxplugintags.rsg>
#include <mglxdatasource.h>         //MGlxDataSource
#include <glxlog.h>                 //For Logging
#include <glxtracer.h>

_LIT(KResourceFile, "z:glxplugintags.rsc");
/**
 * @internal reviewed 14/06/2007 by Alex Birkett
 */

// ----------------------------------------------------------------------------
// Two-Phased Constructor
// ----------------------------------------------------------------------------

CGlxCollectionPluginTags* CGlxCollectionPluginTags::NewL(TAny* aObs)
    {    
    TRACER("CGlxCollectionPluginTags::NewL");

    CGlxCollectionPluginTags* self = new (ELeave) CGlxCollectionPluginTags(
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
 CGlxCollectionPluginTags::~CGlxCollectionPluginTags()
    {
    //Do Nothing
    }
 	
 	
// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
void CGlxCollectionPluginTags::ConstructL()
    {            
    TRACER("CGlxCollectionPluginTags::ConstructL");

    // iDataSource - @owner is CGlxCollectionPluginBase
    iDataSource = MGlxDataSource::OpenDataSourceL(KGlxDefaultDataSourceUid, *this);     
    }
    
    
// ----------------------------------------------------------------------------
//  Constructor
// ----------------------------------------------------------------------------

CGlxCollectionPluginTags::CGlxCollectionPluginTags(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }


// ----------------------------------------------------------------------------
// Add Additional attributes
// ----------------------------------------------------------------------------

void CGlxCollectionPluginTags::CpiAttributeAdditionalAttributes(
				const TMPXAttribute& aCpiAttribute, RArray<TMPXAttribute>& aAttributeArray)
	{
    TRACER("CGlxCollectionPluginTags::CpiAttributeAdditionalAttributes");  
    // Only need to process KGlxMediaCollectionPluginSpecificSubTitle here as all
    // the others are reading straight from resource files
    // KGlxMediaCollectionPluginSpecificSubTitle requires a usage count
	if (aCpiAttribute == KGlxMediaCollectionPluginSpecificSubTitle)
	    {
	    // need to add the usage count. but check first if it is already present
	    TInt attrCount = aAttributeArray.Count();
	    TBool found = EFalse;
	    
	    GLX_DEBUG1("CGlxCollectionPluginTags::CpiAttributeAdditionalAttributes-AttributesCount");  
	    
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
// Modify the response
// ----------------------------------------------------------------------------

void CGlxCollectionPluginTags::HandleCpiAttributeResponseL(CMPXMedia* aResponse, 
				TArray<TMPXAttribute> aCpiAttributes, TArray<TGlxMediaId> aMediaIds)
	{
    TRACER("CGlxCollectionPluginTags::HandleCpiAttributeResponseL");  
    const TInt mediaIdCount = aMediaIds.Count();
    
    switch (mediaIdCount)
        {
    case 0:
            User::Leave(KErrNotSupported);
            break;
        
    case 1:
            GLX_LOG_INFO("CGlxCollectionPluginTags::HandleCpiAttributeResponseL-MediaID");  
    	
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
                HandleCpiAttributeResponseL((*mediaArray)[index], aCpiAttributes, 
                						aMediaIds[index]);
                }
            aResponse->SetCObjectValueL(KMPXMediaArrayContents, mediaArray);        
            CleanupStack::PopAndDestroy(mediaArray);
            }
        break;
        }
	}
	
	
// ----------------------------------------------------------------------------
// Modify the response
// ----------------------------------------------------------------------------	

void CGlxCollectionPluginTags::HandleCpiAttributeResponseL(CMPXMedia* aResponse,
			 TArray<TMPXAttribute> aCpiAttributes, TGlxMediaId aMediaId)
    {
    
    TRACER("CGlxCollectionPluginTags::HandleCpiAttributeResponseL-Enter");  

    const TInt attribCount = aCpiAttributes.Count();
	
    GLX_LOG_INFO("CGlxCollectionPluginTags::HandleCpiAttributeResponseL-Attributes created");
	
	for (TInt index = 0; index < attribCount ; index++)
	    {
	    const TMPXAttribute attr = aCpiAttributes[index];
	    
	    if (attr == KGlxMediaCollectionPluginSpecificSubTitle)
	        {
            GLX_LOG_INFO("CGlxCollectionPluginTags::HandleCpiAttributeResponseL-Subtitle");
	        
	        if (!aResponse->IsSupported(KMPXMediaGeneralCount))
	            {
	            User::Leave(KErrNotSupported);
	            }
            else
                {
                TInt usageCount = aResponse->ValueTObjectL<TInt>(KMPXMediaGeneralCount);
                HBufC* tempTitle = NULL;
                
                if (TGlxMediaId(KGlxCollectionRootId) == aMediaId)
                	{
                	if(0 == usageCount)
                		{
                    	tempTitle = LoadLocalizedStringLC(KResourceFile, 
                    			R_TAG_SUB_TITLE_NO_ITEMS);                    	
                    	// Set the title in the response.
                		aResponse->SetTextValueL(attr, *tempTitle);  
                		CleanupStack::PopAndDestroy(tempTitle);
                		continue;  
                		}
                	else if (1 == usageCount)
                    	{
                    	tempTitle = LoadLocalizedStringLC(KResourceFile,
                   			 R_TAG_SUB_TITLE_SINGLE);
                        aResponse->SetTextValueL(attr, *tempTitle);  
                        CleanupStack::PopAndDestroy(tempTitle);
                        continue;
                    	}
                    else 
                    	{
                    	tempTitle = LoadLocalizedStringLC(KResourceFile,
                   			 R_TAG_SUB_TITLE_MULTI);
                    	}
                	}
                else
                	{
                	if(0 == usageCount)
                		{
                    	tempTitle = LoadLocalizedStringLC(KResourceFile, 
                    			R_TAG_SUB_TITLE_NO_ITEMS);                    	
                    	// Set the title in the response.
                		aResponse->SetTextValueL(attr, *tempTitle);  
                		CleanupStack::PopAndDestroy(tempTitle);
                		continue;  
                		}
                	else if (1 == usageCount)
                		{
                		tempTitle = LoadLocalizedStringLC(KResourceFile,
                   			 R_TAG_ITEM_SUB_TITLE_SINGLE);
                        aResponse->SetTextValueL(attr, *tempTitle);  
                        CleanupStack::PopAndDestroy(tempTitle);
                        continue;
                		}
                    else
                        {
                        tempTitle = LoadLocalizedStringLC(KResourceFile,
                        			 R_TAG_ITEM_SUB_TITLE_MULTI);
                        }
                    }
                TPtr formatString = tempTitle->Des();
                
                // Now create a buffer that will contain the result. needs to be length 
                //of format string plus a few extra for the number
                HBufC* title = HBufC::NewLC(formatString.Length() + 10);
                TPtr ptr = title->Des();
                StringLoader::Format(ptr, formatString, -1, usageCount);
                
                // Set the title in the response.
                aResponse->SetTextValueL(attr, *title);    

                CleanupStack::PopAndDestroy(title);
                CleanupStack::PopAndDestroy(tempTitle);
                }
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle)
	        {
            GLX_LOG_INFO("CGlxCollectionPluginTags::HandleCpiAttributeResponseL-Popup Title");
	        
	        HBufC* title = LoadLocalizedStringLC(KResourceFile, R_TAG_POPUP_TITLE);
            aResponse->SetTextValueL(attr, *title);  
            CleanupStack::PopAndDestroy(title);
            
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificNewMediaItemTitle)
	        {
            GLX_LOG_INFO("CGlxCollectionPluginTags::HandleCpiAttributeResponseL-Mediaitem");
	        
	        HBufC* title = LoadLocalizedStringLC(KResourceFile, R_TAG_ITEM_TITLE);
            aResponse->SetTextValueL(attr, *title);  
            CleanupStack::PopAndDestroy(title);
            
	        }
	        
	    else if (attr == KGlxMediaCollectionPluginSpecificDefaultMediaTitle)
	        {
            GLX_LOG_INFO("CGlxCollectionPluginTags::HandleCpiAttributeResponseL-Defualt mediatitle");
	        
	        HBufC* title = LoadLocalizedStringLC(KResourceFile, R_TAG_DEFAULT_TITLE);
            aResponse->SetTextValueL(attr, *title);  
            CleanupStack::PopAndDestroy(title);
            
	        }
	    else if (attr == KMPXMediaGeneralTitle)
	        {
            GLX_LOG_INFO("CGlxCollectionPluginTags::HandleCpiAttributeResponseL-Attr Title");
	        
	        HBufC* title = LoadLocalizedStringLC(KResourceFile, R_TAG_GENERAL_TITLE);
            aResponse->SetTextValueL(attr, *title);  
            CleanupStack::PopAndDestroy(title); 
	        }
	    }
    }

// ----------------------------------------------------------------------------
// Ignore the Updated Message
// ----------------------------------------------------------------------------	
TBool CGlxCollectionPluginTags::IsUpdateMessageIgnored(CMPXMessage& /*aMessage*/)
	{
	TRACER("CGlxCollectionPluginTags::IsUpdateMessageIgnored");  
	TBool ignore = EFalse;
	return ignore;
	}

// ----------------------------------------------------------------------------
// Set the filter Properties
// ----------------------------------------------------------------------------	

TGlxFilterProperties CGlxCollectionPluginTags::DefaultFilter(TInt aLevel)
    {
    TRACER("CGlxCollectionPluginTags::DefaultFilter"); 
    __ASSERT_DEBUG(( (aLevel == KGlxCollectionRootLevel) || (aLevel == KGlxCollectionTagLevel) 
                || (aLevel == KGlxCollectionTagContentsLevel) || (aLevel == KGlxCollectionTagFSContentsLevel)),
                 Panic(EGlxPanicInvalidPathLevel));
    TGlxFilterProperties filterProperties;
    filterProperties.iSortDirection = EGlxFilterSortDirectionAscending;
    switch(aLevel)
        {
        case KGlxCollectionRootLevel:
        case KGlxCollectionTagLevel: 
            {
            filterProperties.iMinCount = 1;
            filterProperties.iSortOrder = EGlxFilterSortOrderAlphabetical;
            filterProperties.iItemType = EGlxFilterTag;

            break;
            }
        case KGlxCollectionTagContentsLevel:
	        {
        	filterProperties.iSortDirection = EGlxFilterSortDirectionDescending;
			filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
            break;
	        }
        case KGlxCollectionTagFSContentsLevel:
            {
            filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
            break;
            }
        }
    return filterProperties;
    }
    
//End of File
