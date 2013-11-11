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

#include "glxcollectionpluginbase.h"

#include <f32file.h>
#include <glxfilterfactory.h>
#include <glxlog.h>
#include <glxmediageneraldefs.h>
#include <glxthumbnailattributeinfo.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmedia.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <hbtextresolversymbian.h>
#include <glxtracer.h>

#include "glxcommandrequest.h"
#include "glxgetrequest.h"
#include "glxidlistrequest.h"
#include "glxrequest.h"
#include "glxstringcache.h"
#include "glxthumbnailrequest.h"
#include "mglxdatasource.h"


// CONSTANTS
const TGlxThumbnailRequest::TPriorityMode KGlxDefaultThumbnailPriority = TGlxThumbnailRequest::EPrioritizeQuality;
const TInt KFavoriteAlbumId = 1 ;
const TInt KCapturedAlbumId = 2 ;

// ============================ LOCAL FUNCTIONS ==============================
    
// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxCollectionPluginBase::CGlxCollectionPluginBase()
    {
    TRACER("CGlxCollectionPluginBase::CGlxCollectionPluginBase()");
    }
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxCollectionPluginBase::~CGlxCollectionPluginBase()
    {
    TRACER("CGlxCollectionPluginBase::~CGlxCollectionPluginBase()");
    if (iDataSource)
	    {
    	iDataSource->Close(*this);
	    }
	delete iPath;
	delete iStringCache;
    } 


// ----------------------------------------------------------------------------
// Executes a command on the selected collection
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionPluginBase::CommandL(TMPXCollectionCommand /*aCmd*/, TInt /*aArg*/)
    {
    TRACER("void CGlxCollectionPluginBase::CommandL(TMPXCollectionCommand /*aCmd*/, TInt /*aArg*/)");
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// Executes a command on the selected collection
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionPluginBase::CommandL(CMPXCommand& aCmd)
    {
    TRACER("void CGlxCollectionPluginBase::CommandL(CMPXCommand& aCmd)");
    CGlxCommandRequest* request = CGlxCommandRequest::NewL(aCmd, Uid(), *this);
    iDataSource->DataSourceRequestL(request, *this);
    }

// ----------------------------------------------------------------------------
// Navigates to the given path
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionPluginBase::OpenL(const CMPXCollectionPath& aPath,
                   const TArray<TMPXAttribute>& aAttrs,
                   CMPXFilter* aFilter)
    {
    TRACER("void CGlxCollectionPluginBase::OpenL()");
    iPath = CMPXCollectionPath::NewL(aPath);
    // When a collection is opened for browsing, 
    // there are two queries executed with similar filter. 
    // First query to open the collection from list / cloud view.
    // Second one from grid view construction. To improve the grid opening
    // performance, the first query will be completed with empty Id list.
    TBool openRequest = EFalse;
    for (TInt index = 0; index < aAttrs.Count(); index++)
        {
        const TMPXAttribute attr = aAttrs[index];

        if (attr == KGlxFilterGeneralNavigationalStateOnly)
            {
            openRequest = ETrue;
            }
        }

    if ((aFilter && aFilter->IsSupported(
            KGlxFilterGeneralNavigationalStateOnly)) || openRequest)
        {
        RArray<TMPXItemId> mpxIds;
        CleanupClosePushL(mpxIds);
        iPath->AppendL(mpxIds.Array());
        iObs->HandleOpen(iPath, KErrNone);
        CleanupStack::PopAndDestroy(&mpxIds);
        delete iPath;
        iPath = NULL;
        return;
        }

    TGlxMediaId targetId(aPath.Id());
    if (aPath.Levels() == KGlxCollectionRootLevel)
        {
        targetId = TGlxMediaId(KGlxCollectionRootId);
        }
    TInt level = aPath.Levels() + 1; // filter applies to new level we are retrieving
    CMPXFilter* filter = TGlxFilterFactory::CreateCombinedFilterL(DefaultFilter(level), aFilter, EFalse);
    CleanupStack::PushL(filter);
    CGlxIdListRequest* request = CGlxIdListRequest::NewL(targetId, Uid(), filter, *iPath);
    CleanupStack::PopAndDestroy(filter);
    iDataSource->DataSourceRequestL(request, *this);
    }

// ----------------------------------------------------------------------------
// Extended properties of the current file (async)
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionPluginBase::MediaL(const CMPXCollectionPath& aPath, 
                        const TArray< TMPXAttribute >& aAttrs,
                        const TArray<TCapability>& aCaps,
                        CMPXAttributeSpecs* aSpecs)
    {
    TRACER("void CGlxCollectionPluginBase::MediaL()");
	__ASSERT_DEBUG(aPath.Count(), Panic(EGlxPanicMediaRequestedWithEmptyPath));
	
	TInt bitmapId = 0;
    
    TInt count = aAttrs.Count();
    for( TInt i = 0; i < count; ++i )
        {
        if( KGlxMediaIdThumbnail == aAttrs[i].ContentId() )
            {
            bitmapId = aAttrs[i].AttributeId();
            }
        }
        
    CGlxRequest* request = NULL;
    if(aSpecs && bitmapId)
    	{
	    TBool clientHasDRM = EFalse;
	    for ( TInt i = 0; i < aCaps.Count(); ++i)
	    	{
	    	if(aCaps[i] == ECapabilityDRM)
	    		{
	    		clientHasDRM = ETrue;
	    		break;
	    		}
	    	}
	    TSize size(0,0);
	    TGlxThumbnailRequest::TPriorityMode priority = KGlxDefaultThumbnailPriority;
	    TGlxThumbnailFilter filter = EGlxThumbnailFilterNone;
	    TInt bitmapHandle = 0;
	    TRect cropRect;
		const TMPXAttributeData sizeAttrib = { KGlxMediaIdThumbnail , KGlxAttribSpecThumbnailSize };
		const TMPXAttributeData priorityAttrib = { KGlxMediaIdThumbnail , KGlxAttribSpecThumbnailQualityOverSpeed };
		const TMPXAttributeData bitmapAttrib = { KGlxMediaIdThumbnail , KGlxAttribSpecThumbnailBitmapHandle };
		const TMPXAttributeData cropAttrib = { KGlxMediaIdThumbnail , KGlxAttribSpecThumbnailCroppingRect };
		const TMPXAttributeData filterAttrib = { KGlxMediaIdThumbnail , KGlxAttribSpecThumbnailFiltering };

	    if(aSpecs->IsSupported(sizeAttrib))
	    	{
	    	size = aSpecs->ValueTObjectL<TSize>(sizeAttrib);
	    	}
	    if(aSpecs->IsSupported(priorityAttrib))
	    	{
	    	priority = aSpecs->ValueTObjectL<TBool>(priorityAttrib) ? TGlxThumbnailRequest::EPrioritizeQuality : TGlxThumbnailRequest::EPrioritizeSpeed;
	    	}
	    if(aSpecs->IsSupported(bitmapAttrib))
	    	{
	    	bitmapHandle = aSpecs->ValueTObjectL<TInt>(bitmapAttrib);
	    	}
	    if(aSpecs->IsSupported(cropAttrib))
	    	{
	    	cropRect = aSpecs->ValueTObjectL<TRect>(cropAttrib);
	    	}
	    if(aSpecs->IsSupported(filterAttrib))
	    	{
	    	filter = aSpecs->ValueTObjectL<TGlxThumbnailFilter>(filterAttrib);
	    	}
	    
	    	
	    TGlxThumbnailRequest tnRequest(TGlxMediaId(aPath.Id()), size, priority, bitmapHandle, clientHasDRM, cropRect, filter);
        CMPXFilter* defFilter = TGlxFilterFactory::CreateCombinedFilterL(DefaultFilter(aPath.Levels()), NULL, EFalse);
        CleanupStack::PushL(defFilter);
	    request = CGlxThumbnailRequest::NewL(tnRequest, Uid(), bitmapId, defFilter);
        CleanupStack::PopAndDestroy(defFilter);
        
        CleanupStack::PushL(request);
        
 	    CGlxtnFileInfo* tnFileInfo = new (ELeave) CGlxtnFileInfo;
	    CleanupStack::PushL(tnFileInfo);
	    
        TInt countInfo(0);
	    
	    if(aSpecs->IsSupported(KMPXMediaGeneralSize))
	    	{
	    	tnFileInfo->iFileSize = aSpecs->ValueTObjectL<TUint>(KMPXMediaGeneralSize);
	    	countInfo++;
	    	}
	    if(aSpecs->IsSupported(KGlxMediaGeneralLastModifiedDate))
	    	{
	    	tnFileInfo->iFileTime = aSpecs->ValueTObjectL<TInt64>(KGlxMediaGeneralLastModifiedDate);
	    	countInfo++;
	    	}
	    if(aSpecs->IsSupported(KMPXMediaDrmProtected))
	    	{
	    	tnFileInfo->iIsProtected = aSpecs->ValueTObjectL<TBool>(KMPXMediaDrmProtected);
	    	countInfo++;
	    	}
	    if(aSpecs->IsSupported(KMPXMediaGeneralUri))
	    	{
	    	tnFileInfo->SetFilePathL(aSpecs->ValueText(KMPXMediaGeneralUri));
	    	countInfo++;
	    	}
	    if(aSpecs->IsSupported(KMPXMediaGeneralCategory))
	    	{
	    	TMPXGeneralCategory category = aSpecs->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory);
	    	if( EMPXVideo == category )
	    	    {
    	    	tnFileInfo->iIsVideo = ETrue;
    	    	countInfo++;
	    	    }
	    	else if( EMPXImage == category )
	    	    {
    	    	tnFileInfo->iIsVideo = EFalse;
    	    	countInfo++;
	    	    }
	    	}
	    	
	    const TInt KGlxRequiredCountInfo = 5;
        CGlxThumbnailRequest* requestTn = static_cast<CGlxThumbnailRequest*>(request);
        if( KGlxRequiredCountInfo == countInfo )
            {
            requestTn->SetThumbnailInfo(tnFileInfo);
            CleanupStack::Pop(tnFileInfo);
            }
        else
            {
            CleanupStack::PopAndDestroy(tnFileInfo);
            }
        
        CleanupStack::Pop(request);
    	}
	else
		{
		request = GetRequestFactoryL(aPath, aAttrs);
		}
    iDataSource->DataSourceRequestL(request, *this);
		
    }
        
// ----------------------------------------------------------------------------
// Cancel outstanding request
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionPluginBase::CancelRequest()
    {
    TRACER("void CGlxCollectionPluginBase::CancelRequest()");
    iDataSource->CancelRequest(*this);
    }

// ----------------------------------------------------------------------------
// Adds an item to the collection
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionPluginBase::AddL(const CMPXMedia& /*aNewMedia*/)
    {
    TRACER("void CGlxCollectionPluginBase::AddL()");
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// Remove an item or items under a group from the collection
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionPluginBase::RemoveL(const CMPXCollectionPath& /*aPath*/)
    {
    TRACER("void CGlxCollectionPluginBase::RemoveL()");
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// Remove an item or items under a group from the collection
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionPluginBase::RemoveL(const CMPXMedia& /*aMedia*/)
    {
    TRACER("void CGlxCollectionPluginBase::RemoveL()");
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// Sets/updates the media for the item
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionPluginBase::SetL(const CMPXMedia& /*aMedia*/)
    {
    TRACER("void CGlxCollectionPluginBase::SetL(const CMPXMedia& /*aMedia*/)");
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionPluginBase::FindAllL(const CMPXMedia& /*aCriteria*/, const TArray<TMPXAttribute>& /*aAttrs*/)
    {
    TRACER("void CGlxCollectionPluginBase::FindAllL()");
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
EXPORT_C CMPXMedia* CGlxCollectionPluginBase::FindAllSyncL(const CMPXMedia& /*aCriteria*/,
                                    const TArray<TMPXAttribute>& /*aAttrs*/)
	{
    TRACER("CMPXMedia* CGlxCollectionPluginBase::FindAllSyncL()");
    User::Leave(KErrNotSupported);
    return NULL;
	}

    
// ----------------------------------------------------------------------------
// GetCapabilities
// ----------------------------------------------------------------------------
//
EXPORT_C TCollectionCapability CGlxCollectionPluginBase::GetCapabilities()
	{
    TRACER("TCollectionCapability CGlxCollectionPluginBase::GetCapabilities()");
	return 0;
	}

EXPORT_C void CGlxCollectionPluginBase::HandleResponse(CMPXMedia* aResponse, CGlxRequest* aRequest, const TInt& aError)
	{
    TRACER("void CGlxCollectionPluginBase::HandleResponse()");
    GLX_DEBUG2("CGlxCollectionPluginBase::HandleResponse()() aError=%d", aError);
    if (dynamic_cast<CGlxIdListRequest*>(aRequest))
	// iRequest is a CGlxIdListRequest
	    {
      iObs->HandleOpen(iPath, aError);
      delete iPath;
      iPath = NULL;
	    }
	else if (dynamic_cast<CGlxThumbnailRequest*>(aRequest))
	// iRequest is a CGlxThumbnailRequest
        {
        //when IAD update / sisx installation of S60 TNM is done and
        //photos asks for the thumbnail, the thumbnail manager returns an 
        //error KErrServerTerminated(-15)/KErrDied(-13) for which the MPX recreates
        //the collection plugins and reconnects to the MPX server and goes into
        //a invalid state. After this, the error is not propogated to the Medialist
        //and no further requests are processed and it keeps returning KErrNotReady(-18)
        //So, as a HACK we are changing the error code to KErrCompletion(-17), 
        //for which the MPX collection does not take any action and propogates the error
        //to the medialist. This error(KErrCompletion) is treated as a temporary error
        //and the thumbnail request is made again from the medialist for which the
        //S60 TNM returns the thumbnail properly.
        iObs->HandleMedia(aResponse, ((aError == KErrServerTerminated
                || aError == KErrDied) ? KErrCompletion : aError));
        }
	else if (dynamic_cast<CGlxGetRequest*>(aRequest))
	// iRequest is a CGlxGetRequest
        {
		TInt error = aError;
        CGlxGetRequest* getRequest = static_cast<CGlxGetRequest*>(aRequest);
		TBool b = getRequest->CpiAttributesSet();
        if(b  && ( KErrNone == error ))
        	{
        	TRAP(error, HandleCpiAttributeResponseL(aResponse, getRequest->CpiAttributes(), getRequest->MediaIds().Array()));
        	}
		iObs->HandleMedia(aResponse, error); 
	    }
	else if (dynamic_cast<CGlxCommandRequest*>(aRequest))
	// iRequest is a CGlxCommandRequest
        {
		iObs->HandleCommandComplete(aResponse, aError); 
	    }
	else
		{
		// Other request types not handled
		Panic(EGlxPanicLogicError); 
		}
	}

CGlxRequest* CGlxCollectionPluginBase::GetRequestFactoryL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs)
	{
    TRACER("CGlxRequest* CGlxCollectionPluginBase::GetRequestFactoryL()");
	TBool doNotNeedDataSource = EFalse;
	RArray<TGlxMediaId> mediaIdArray;
	
	CleanupClosePushL(mediaIdArray);
	
	if( aPath.Levels() == KGlxCollectionRootLevel)
		{
		__ASSERT_DEBUG(Uid().iUid == (TUint32)aPath.Id(), Panic(EGlxPanicIllegalArgument));
		mediaIdArray.AppendL(TGlxMediaId(KGlxCollectionRootId));
		doNotNeedDataSource = ETrue;
		}
	else
		{
		// Add the media Ids to the iMediaIdArray
		TArray<TInt> selection = aPath.Selection();
		TInt selectedCount = selection.Count();
		if (selectedCount)
		// Items are selected; We require the attributes for the selected items.
			{
			for (TInt i = 0; i < selectedCount; i++)
				{
				const TMPXItemId& itemId = aPath.IdOfIndex(selection[i]);
				if(itemId.operator ==(KCapturedAlbumId))//captured
				    {
                    mediaIdArray.Insert(TGlxMediaId(itemId),0);
				    }
				else if(itemId.operator ==(KFavoriteAlbumId))//favourites
				    {
                    mediaIdArray.Insert(TGlxMediaId(itemId),0);
				    }
				else
				    {
                    mediaIdArray.AppendL(TGlxMediaId(itemId));
				    }
				}
			}
		else // No items are selected; We will use the current item
			{
			mediaIdArray.AppendL(TGlxMediaId(aPath.Id()));
			}
		
		}
	
	// Add the attributes to the attributeArray
	RArray<TMPXAttribute> attributeArray;
	CleanupClosePushL(attributeArray);
	RArray<TMPXAttribute> cpiAttributeArray;
	CleanupClosePushL(cpiAttributeArray);
	
	for (TInt i = 0; i < aAttrs.Count(); i++)
		{
        if( KGlxMediaIdCollectionPluginSpecific == aAttrs[i].ContentId())
        	{
			cpiAttributeArray.AppendL(aAttrs[i]);
			CpiAttributeAdditionalAttributes(aAttrs[i], attributeArray);  
			doNotNeedDataSource = EFalse;   	
        	}
        else 
        	{
        	if ( aAttrs[i] == KMPXMediaGeneralCount )
        	    {
        	    doNotNeedDataSource = EFalse;
        	    }
			attributeArray.AppendL(aAttrs[i]);	       	
        	}
		}
	
    CMPXFilter* filter = TGlxFilterFactory::CreateCombinedFilterL(DefaultFilter(aPath.Levels()), NULL, EFalse);
    CleanupStack::PushL(filter);
	CGlxRequest* request = CGlxGetRequest::NewL(mediaIdArray, attributeArray, cpiAttributeArray, Uid(), filter, doNotNeedDataSource); // Array contents ownership transferred
    CleanupStack::PopAndDestroy(filter);
	CleanupStack::Pop(&cpiAttributeArray);
	CleanupStack::Pop(&attributeArray);
	CleanupStack::Pop(&mediaIdArray);
	return request;
	}	


 EXPORT_C void CGlxCollectionPluginBase::HandleMessage(CMPXMessage& aMessage)
	{
    TRACER("void CGlxCollectionPluginBase::HandleMessage(CMPXMessage& aMessage)");
	aMessage.SetTObjectValueL<TUid>(KMPXMessageCollectionId, Uid());
	iObs->HandleMessage(&aMessage, KErrNone);
	}

EXPORT_C TBool CGlxCollectionPluginBase::IsUpdateMessageIgnored(CMPXMessage& /*aMessage*/)
	{
    TRACER("TBool CGlxCollectionPluginBase::IsUpdateMessageIgnored(CMPXMessage& /*aMessage*/)");
	// Default implementation ignores all update messages
	return ETrue;
	}

EXPORT_C void CGlxCollectionPluginBase::CpiAttributeAdditionalAttributes(const TMPXAttribute& /*aCpiAttribute*/, RArray<TMPXAttribute>& /*aAttributeArray*/)
	{
    TRACER("void CGlxCollectionPluginBase::CpiAttributeAdditionalAttributes()");
	// Default implementation does not require additional attributes	
	}

EXPORT_C void CGlxCollectionPluginBase::HandleCpiAttributeResponseL(CMPXMedia* /*aResponse*/, TArray<TMPXAttribute> /*aCpiAttributes*/, TArray<TGlxMediaId> /* aMediaIds */)
	{
    TRACER("void CGlxCollectionPluginBase::HandleCpiAttributeResponseL()");
	// Base class does not support CPI specific attributes, deriving class must override.
	User::Leave(KErrNotSupported);
	}

EXPORT_C HBufC* CGlxCollectionPluginBase::LoadLocalizedStringLC(const TDesC& aResourceFile, const TInt aResourceId)
    {
	TRACER("void CGlxCollectionPluginBase::LoadLocalizedStringLC()");
	
	if (!iStringCache)
        {
        iStringCache = CGlxStringCache::NewL();
        }        
	return (iStringCache->LoadLocalizedStringLC(aResourceFile,aResourceId));
    }
    

EXPORT_C TGlxFilterProperties CGlxCollectionPluginBase::DefaultFilter(TInt /*aLevel*/)
    {
    TRACER("TGlxFilterProperties CGlxCollectionPluginBase::DefaultFilter(TInt /*aLevel*/)");
    TGlxFilterProperties filterProperties;
    filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
    filterProperties.iSortDirection = EGlxFilterSortDirectionDescending;
    
    return filterProperties;
    }
    
EXPORT_C void CGlxCollectionPluginBase::initializeTextResolverSymbian()
    {
    TRACER("CGlxCollectionPluginBase::initializeTextResolverSymbian");
    _LIT(KSomeFile, "photos_");
    _LIT(KPath, "z:\\resource\\qt\\translations");      
     HbTextResolverSymbian::Init(KSomeFile, KPath);
    }

EXPORT_C HBufC* CGlxCollectionPluginBase::LoadLocalizedStringLC(const TDesC& aStringName)
    {
     TRACER("void CGlxCollectionPluginBase::LoadLocalizedStringLC()");
     return HbTextResolverSymbian::LoadLC(aStringName); 
    }

// End of file
