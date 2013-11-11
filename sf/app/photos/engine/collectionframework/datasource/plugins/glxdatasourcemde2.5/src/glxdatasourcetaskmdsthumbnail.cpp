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
* Description:   
*
*/



#include "glxdatasourcetaskmdsthumbnail.h"

#include <caf/content.h>
#include <glxcollectionplugincamera.hrh>
#include <glxcollectionplugindownloads.hrh>
#include <glxcollectionpluginmonths.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionplugintags.hrh>
#include <glxerrors.h>
#include <glxfilterfactory.h>
#include <glxgetrequest.h>
#include <glxidlistrequest.h>
#include <glxlog.h>
#include <glxmediacollectioninternaldefs.h>
#include <glxmediageneraldefs.h>
#include <glxrequest.h>
#include <glxthumbnailattributeinfo.h>
#include <glxthumbnailrequest.h>
#include <glxtnfileinfo.h>
#include <glxtracer.h>
#include <lbsposition.h>
#include <mdeconstants.h>
#include <mdelogiccondition.h>
#include <mdeobject.h>
#include <mdeobjectdef.h>
#include <mdeobjectquery.h>
#include <mdepropertydef.h>
#include <mderelationdef.h>
#include <mdesession.h>
#include <mdetextproperty.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h> 
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>

#include "glxdatasourcemds.h"
#include "glxdatasourcemds.hrh"

// CONSTANTS

_LIT(KPropertyDefNameLastModifiedDate, "LastModifiedDate");
_LIT(KPropertyDefNameDRM, "DRM");
_LIT(KPropertyDefNameItemType, "ItemType");
//_LIT(KPropertyDefNameLastModifiedDate, "LastModifiedDate");
_LIT(KPropertyDefNameSize, "Size");
_LIT(KUnsupportedItemType,  "video/x-pn-realvideo");
_LIT(KUnsupportedItemType1, "video/x-pn-multirate-realvideo");
_LIT(KUnsupportedItemType2, "video/vnd.rn-realvideo");
_LIT(KUnsupportedItemType3, "video/x-ms-wmv");

// ----------------------------------------------------------------------------
//  IsUnsupportedL
//  Tests to see if an item type is unsupported
// ----------------------------------------------------------------------------
//  
TBool IsUnsupportedL(CMdESession& aSession, TItemId aItemId)
	{
    TRACER("IsUnsupportedL()");
	TBool isUnsupported = EFalse;
    CMdEObject* item = aSession.GetObjectL(aItemId);
    
    if (!item)
    	{
    	User::Leave(KErrNotFound);
    	}
    
    CleanupStack::PushL(item);
    CMdEProperty* itemType = NULL;
    
    CMdEPropertyDef& itemTypeProperty = item->Def().GetPropertyDefL(KPropertyDefNameItemType);
    
    TInt itemTypeIndex = item->Property(itemTypeProperty, itemType);
    if (itemTypeIndex > KErrNotFound)
    	{
    	const TDesC& des = static_cast< CMdETextProperty *>(itemType)->Value();
    	if (des.Compare(KUnsupportedItemType) == 0 || 
    	    des.Compare(KUnsupportedItemType1) == 0 || 
    	    des.Compare(KUnsupportedItemType2) == 0 ||
    	    des.Compare(KUnsupportedItemType3) == 0)
    		{
    		// The item is unsupported
    		isUnsupported = ETrue;
    		}
    	}
    CleanupStack::PopAndDestroy(item);

    return isUnsupported;
	}
// ----------------------------------------------------------------------------
//  Constructor
// ----------------------------------------------------------------------------
// 	
CGlxDataSourceTaskMdeThumbnail:: CGlxDataSourceTaskMdeThumbnail(CGlxThumbnailRequest* aRequest, 
		                       MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource)
    : CGlxDataSourceTaskMde(aRequest, aObserver, aDataSource)
    {
    TRACER("CGlxDataSourceTaskMdeThumbnail::CGlxDataSourceTaskMdeThumbnail()")
    iFilterProperties = TGlxFilterFactory::ExtractAttributes(iRequest->Filter());
    }

// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------
//  	
CGlxDataSourceTaskMdeThumbnail::~CGlxDataSourceTaskMdeThumbnail()
    {
    TRACER("CGlxDataSourceTaskMdeThumbnail::~CGlxDataSourceTaskMdeThumbnail()")    
    DataSource()->CancelFetchThumbnail();
    delete iTnFileInfo;
    iTnFileInfo = NULL;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::ExecuteRequest
// ----------------------------------------------------------------------------
//          
void CGlxDataSourceTaskMdeThumbnail::ExecuteRequestL()
    {
    TRACER("CGlxDataSourceTaskMdeThumbnail::ExecuteRequestL()") 
#ifdef _DEBUG
    iStartTime.HomeTime(); 
#endif
    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
    
    TGlxThumbnailRequest tnReq;
    request->ThumbnailRequest(tnReq);
    GLX_DEBUG4("*** CGlxDataSourceTaskMdeThumbnail::ExecuteRequestL() Id=%d, TN Size w(%d) h(%d) ***", tnReq.iId.Value(),
    						tnReq.iSizeClass.iWidth, tnReq.iSizeClass.iHeight);
    GLX_DEBUG2("CGlxDataSourceTaskMdeThumbnail::ExecuteRequestL() iPriorityMode=%d", tnReq.iPriorityMode);
	
	if(request->ThumbnailInfo())
		{
		if (tnReq.iSizeClass.iWidth > 0 && tnReq.iSizeClass.iHeight > 0 )
			{
			DataSource()->FetchThumbnailL(iRequest, *this);
			}
		else
			{
			ThumbnailFetchComplete(KErrNone, ETrue);
			}
		}
	else
		{
	   	FetchFileInfoL();
		}
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::HandleRequestComplete
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMdeThumbnail::HandleRequestComplete(TInt aError)
	{
    TRACER("void CGlxDataSourceTaskMdeThumbnail::HandleRequestComplete()");
    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
    if(request && !(aError == KErrNone || aError == KErrNoMemory))
    	{
    	// An error has occured.
    	TBool isUnsupported = EFalse;
    	// If the item type is unsupported the we want to display the default
    	// icon rather than the corrupt icon.
    	TRAPD(err, isUnsupported = IsUnsupportedL(DataSource()->Session(), 
    			                          TItemId(request->ItemId().Value())));
    	if (isUnsupported)
    		{
    		aError = KErrNotSupported;
    		}
    	else if (err != KErrNone)
    		{
    		aError = err;
    		}
    	}
	CGlxDataSourceTask::HandleRequestComplete(aError);
	}

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeThumbnail::DoHandleQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeThumbnail::DoHandleQueryCompletedL(CMdEQuery& /*aQuery*/)
    {
    TRACER("CGlxDataSourceTaskMdeThumbnail::DoHandleQueryCompletedL()")
    if (EContainerFirstItemQuery == iQueryTypes[0])
        {
        TRAPD(err, DoHandleContainerFirstItemQueryCompletedL());
        
        if (err != KErrNone)
            {
            CompleteThumbnailRequest(err);
            }
        }
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::HandleThumbnailFetchCompleteL
// ----------------------------------------------------------------------------
//   
void CGlxDataSourceTaskMdeThumbnail::HandleThumbnailFetchCompleteL(const TGlxMediaId& aId,
        TGlxThumbnailQuality aQuality)
    {
    TRACER("CGlxDataSourceTaskMdeThumbnail::HandleThumbnailFetchCompleteL()")
    __ASSERT_DEBUG(dynamic_cast<CGlxThumbnailRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxThumbnailRequest* req = static_cast<CGlxThumbnailRequest*>(iRequest);
    GLX_DEBUG2("*** CGlxDataSourceTaskMdeThumbnail::HandleThumbnailFetchCompleteL() Id=%d ***", req->ItemId().Value());
#ifdef _DEBUG
    iStopTime.HomeTime(); 
    GLX_DEBUG2("CGlxDataSourceTaskMdeThumbnail:HandleThumbnailFetchCompleteL() took %d us",
                     (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif    
    delete iResponse;
    iResponse = NULL;
    iResponse = CMPXMedia::NewL();
    CGlxThumbnailAttribute* tnAttribute = new (ELeave) CGlxThumbnailAttribute;
    CleanupStack::PushL(tnAttribute);
    TGlxThumbnailRequest tnRequest;
    req->ThumbnailRequest(tnRequest); 
    TSize size(tnRequest.iSizeClass);
    tnAttribute->iDimensions = size;
    tnAttribute->iCroppingRect = tnRequest.iCroppingRect;
    tnAttribute->iThumbnailQuality = aQuality;
    GLX_DEBUG2("CGlxDataSourceTaskMdeThumbnail::HandleThumbnailFetchCompleteL() aQuality=%d", aQuality);

    TUint attributeId = req->AttributeId();
    if ( GlxIsFullThumbnailAttribute(attributeId) )
        {
        TBool quality = (EGlxThumbnailQualityHigh == aQuality);
        attributeId = GlxFullThumbnailAttributeId(quality,
                                                size.iWidth, size.iHeight);
        }

    iResponse->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, aId.Value());
    iResponse->SetNoNewLCObjectL(
                TMPXAttribute(KGlxMediaIdThumbnail, attributeId), tnAttribute);
    CleanupStack::PopAndDestroy(tnAttribute);
    }

void CGlxDataSourceTaskMdeThumbnail::ThumbnailFetchComplete(TInt aError, 
                                                            TBool aQuality)
	{
    TRACER("CGlxDataSourceTaskMdeThumbnail::ThumbnailFetchComplete(TNM)")
    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
    TGlxThumbnailRequest tnRequest;
    request->ThumbnailRequest(tnRequest); 
    TInt err = aError;
    if(!err)
        {
        TGlxThumbnailQuality tnQuality = EGlxThumbnailQualityHigh;
        if (!aQuality)
            {
            tnQuality = EGlxThumbnailQualityLow;
            }
        TRAP(err, HandleThumbnailFetchCompleteL(tnRequest.iId, tnQuality));
        }
    HandleRequestComplete(err);
	}

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::CompleteFetchFileInfoL
// ----------------------------------------------------------------------------
//  	    
void CGlxDataSourceTaskMdeThumbnail::CompleteFetchFileInfoL(CMdEObject* aItem)
    {
    TRACER("CGlxDataSourceTaskMdeThumbnail::CompleteFetchFileInfoL()")
    TParsePtrC parser(aItem->Uri());
    iTnFileInfo->SetFilePathL(parser.FullName());

    CMdEProperty* size;
    CMdEPropertyDef& sizeProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameSize);
    TInt sizeIndex = aItem->Property(sizeProperty, size); 
    if( KErrNotFound == sizeIndex )
        {
        iTnFileInfo->iFileSize = 0;
        }
    else
        {
        iTnFileInfo->iFileSize = size->Uint32ValueL();
        }

    CMdEProperty* lastModifiedDateProperty;
    CMdEPropertyDef& lastModifiedDatePropertyDef = aItem->Def().GetPropertyDefL(
            KPropertyDefNameLastModifiedDate);

#ifdef _DEBUG
    TInt index = // This variable is only required for the assert debug below. If it is not wrapped in an  
                 // #ifdef _DEBUG, it will cause a warning in non debug builds.
#endif    
    aItem->Property(lastModifiedDatePropertyDef, lastModifiedDateProperty);
    __ASSERT_DEBUG(index > KErrNotFound, Panic(EGlxPanicRequiredPropertyNotFound));
    
    iTnFileInfo->iFileTime = static_cast<CMdETimeProperty*>(lastModifiedDateProperty)->Value();
    
    CGlxDataSource::TItemType itemType = DataSource()->ItemType(aItem);
    iTnFileInfo->iIsVideo = ( CGlxDataSource::EItemTypeVideo == itemType );
    
    CMdEProperty* drmProtected;
    CMdEPropertyDef* drmProtectedProperty = NULL;
    
    ///@todo AB test
    TRAP_IGNORE(drmProtectedProperty = &aItem->Def().GetPropertyDefL(KPropertyDefNameDRM)); 
    TInt drmProtectedIndex = KErrNotFound;
    
    if (drmProtectedProperty)
    	{
    	drmProtectedIndex= aItem->Property(*drmProtectedProperty, drmProtected);
    	}
    
    TInt err = KErrNone;
    if( KErrNotFound == drmProtectedIndex)
        {
        // should be present for all normally harvested images
        // so this should only be where we didn't pre-fetch the attributes
        // i.e. background thumbnail generation
        // so we get status from CAF to avoid forcing second stage harvest
        TRAP(err, 
            ContentAccess::CContent* content = ContentAccess::CContent::NewLC(
                    iTnFileInfo->FilePath());
            content->GetAttribute(ContentAccess::EIsProtected, iTnFileInfo->iIsProtected);
            CleanupStack::PopAndDestroy(content);
            );
        }
    else
        {
        iTnFileInfo->iIsProtected =  static_cast<CMdEBoolProperty*>(drmProtected)->Value();
        }

    CompleteThumbnailRequest(err);
    iTnFileInfo = NULL;
    iTnRequestStatus = NULL;
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeThumbnail::DoHandleContainerFirstItemQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeThumbnail::DoHandleContainerFirstItemQueryCompletedL()
    {
    TRACER("CGlxDataSourceTaskMdeThumbnail::DoHandleContainerFirstItemQueryCompletedL()")
    if( !iQueries[0]->Count() )
        {
        User::Leave(KErrGlxEmptyContainer); // leave with a specific Error code so UI can display empty folder icon
        }
    
    CMdEObject* item = static_cast<CMdEObject*>(iQueries[0]->TakeOwnershipOfResult(0));
    if(!item)
        {
        User::Leave(KErrNotFound);
        }
    
    CleanupStack::PushL(item);
    CompleteFetchFileInfoL(item);
    CleanupStack::PopAndDestroy(item);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeThumbnail::CompleteThumbnailRequest
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeThumbnail::CompleteThumbnailRequest(TInt aError)
	{
    TRACER("CGlxDataSourceTaskMdeThumbnail::CompleteThumbnailRequest(TInt aError)")
    GLX_DEBUG2("CGlxDataSourceTaskMdeThumbnail::CompleteThumbnailRequest  aError=%d", aError);
	if (aError != KErrNone)
		{
		ThumbnailFetchComplete(aError, EFalse);
		}
	else
		{
	    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
        TGlxThumbnailRequest tnReq;
	   	request->ThumbnailRequest(tnReq);

		if (tnReq.iSizeClass.iWidth > 0 && tnReq.iSizeClass.iHeight > 0 )
			{
		    request->SetThumbnailInfo(iTnFileInfo);
		    //This function is called number of times as a callback ,
            //hence not trapping the leaving function which costs time and memory.
            //Ignoring this for code scanner warnings - Leaving functions called in non-leaving functions.
			DataSource()->FetchThumbnailL(iRequest, *this);
			}
		else
			{
		    GLX_LOG_INFO("CGlxDataSourceTaskMdeThumbnail::CompleteThumbnailRequest KErrArgument");
			ThumbnailFetchComplete(KErrArgument, EFalse);
			}
		}
	}

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::FetchFileInfoL
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMdeThumbnail::FetchFileInfoL()
    {
    TRACER("CGlxDataSourceTaskMdeThumbnail::FetchFileInfoL()")
    __ASSERT_DEBUG(dynamic_cast<CGlxThumbnailRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
    CMdEObject* item = DataSource()->Session().GetObjectL(request->ItemId().Value());
    if(!item)
        {
        User::Leave(KErrNotFound);
        }
        
    CleanupStack::PushL(item);
    iTnFileInfo = new (ELeave) CGlxtnFileInfo;
    
    CGlxDataSource::TContainerType containerType = DataSource()->ContainerType(item);
    if( CGlxDataSource::EContainerTypeNotAContainer != containerType )
        {
        iTnFileInfo->iTemporary = ETrue;
        
        TGlxMediaId container = request->ItemId();
        CMdEObjectDef* objectDef = &item->Def();
        
        /// @todo: use default filter so we can ensure we always get correct first item if filters change
        iFilterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
        iFilterProperties.iSortDirection = EGlxFilterSortDirectionOverrideToDescendingIfDate;
        iFilterProperties.iLastCaptureDate = ETrue;
        // for 10.1 we need first image thumbnail which is not DRM
        iFilterProperties.iNoDRM = ETrue;
        iFilterProperties.iItemType = EGlxFilterImage;

        if( CGlxDataSource::EContainerTypeMonth == containerType )
            {
            iFilterProperties.iOrigin = EGlxFilterOriginAll;
            AddMonthFilterL(item, iFilterProperties);
            container = TGlxMediaId(KGlxCollectionRootId);
            objectDef = &DataSource()->ObjectDef();
            }
            
        DoQueryL(*objectDef, ETrue, EContainerFirstItemQuery, 
                EQueryResultModeItem, container);        
        }
    else
        {
        CompleteFetchFileInfoL(item);
        }

    CleanupStack::PopAndDestroy(item);
    }
