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
#include <glxpluginall.rsg>
#include <StringLoader.h>
#include <glxtracer.h>

#include "glxcollectionpluginall.h"
#include "glxcollectionpluginall.hrh"
#include "glxmediageneraldefs.h"
#include "glxcollectiongeneraldefs.h"
#include "glxlog.h"


#include "glxpanic.h"
#include "mglxdatasource.h"
#include "glxrequest.h"
#include "glxidlistrequest.h"
#include "glxfilterfactory.h"

/**
 * @internal reviewed 13/06/2007 by Alex Birkett
 */

// CONSTANTS

// ============================ LOCAL FUNCTIONS ==============================
    
// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginAll* CGlxCollectionPluginAll::NewL(TAny* aObs)
    {
    CGlxCollectionPluginAll* self = new (ELeave) CGlxCollectionPluginAll(
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
CGlxCollectionPluginAll::~CGlxCollectionPluginAll()
    {
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginAll::CGlxCollectionPluginAll(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginAll::ConstructL()
    {
    iDataSource = MGlxDataSource::OpenDataSourceL(KGlxDefaultDataSourceUid, *this);
	}

void CGlxCollectionPluginAll::CpiAttributeAdditionalAttributes(const TMPXAttribute& aCpiAttribute, RArray<TMPXAttribute>& aAttributeArray)
	{
    TRACER("CGlxCollectionPluginAll::CpiAttributeAdditionalAttributes()");
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

void CGlxCollectionPluginAll::HandleCpiAttributeResponseL(CMPXMedia* aResponse, TArray<TMPXAttribute> aCpiAttributes, TArray<TGlxMediaId> /* aMediaIds */)
	{
    TRACER("CGlxCollectionPluginAll::HandleCpiAttributeResponseL");
    _LIT(KResourceFile, "z:glxpluginall.rsc");
    
	TInt count = aCpiAttributes.Count();
	
	for (TInt index = 0; index < count ; index++)
	    {
	    const TMPXAttribute attr = aCpiAttributes[index];
	    
	    if (attr == KGlxMediaCollectionPluginSpecificSubTitle)
	        {
	        if (!aResponse->IsSupported(KMPXMediaGeneralCount))
	            {
	            User::Leave(KErrNotSupported);
	            }
            else
                {
                TInt usageCount = aResponse->ValueTObjectL<TInt>(KMPXMediaGeneralCount);
                HBufC* tempTitle = NULL;
                
                if(0 == usageCount)
                	{
                	tempTitle = LoadLocalizedStringLC(KResourceFile, R_ALL_ITEM_SUB_TITLE_EMPTY);                	
                	// Set the title in the response.
            		aResponse->SetTextValueL(attr, *tempTitle);  
            		CleanupStack::PopAndDestroy(tempTitle);
            		continue;            
                	}
                // Get the format string
                else  if (1 == usageCount)
                    {
                    tempTitle = LoadLocalizedStringLC(KResourceFile, R_ALL_SUB_TITLE_SINGLE);
                    }
                else
                    {
                    tempTitle = LoadLocalizedStringLC(KResourceFile, R_ALL_SUB_TITLE_MULTI);
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
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle)
	        {
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificNewMediaItemTitle)
	        {
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificDefaultMediaTitle)
	        {
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KMPXMediaGeneralTitle)
	        {
	        HBufC* title = LoadLocalizedStringLC(KResourceFile, R_ALL_GENERAL_TITLE);
            // Set the title in the response.
            aResponse->SetTextValueL(attr, *title);  
            CleanupStack::PopAndDestroy(title); 
	        }
	    }
	}

TBool CGlxCollectionPluginAll::IsUpdateMessageIgnoredL(CMPXMessage& /*aMessage*/)
	{
	TBool ignore = EFalse;
	return ignore;
	}
	
// End of file
