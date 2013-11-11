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
* Description:   Thumbnail request
*
*/



#include "glxthumbnailrequest.h"
#include <glxlog.h>
#include <glxtracer.h>

EXPORT_C CGlxThumbnailRequest* CGlxThumbnailRequest::NewL(TGlxThumbnailRequest aThumbnailRequest,
					const TUid& aCollectionPluginUid, const TInt attributeId, CMPXFilter* aFilter)
	{
    TRACER("CGlxThumbnailRequest* CGlxThumbnailRequest::NewL()");
	CGlxThumbnailRequest* tnRequest = new (ELeave) CGlxThumbnailRequest(aThumbnailRequest, aCollectionPluginUid, attributeId);
	CleanupStack::PushL(tnRequest);
	tnRequest->ConstructL(aFilter);
	CleanupStack::Pop(tnRequest);
	return tnRequest;
	}

CGlxThumbnailRequest::CGlxThumbnailRequest(TGlxThumbnailRequest aThumbnailRequest,
					const TUid& aCollectionPluginUid, const TInt attributeId)
: CGlxRequest(aCollectionPluginUid), iThumbnailRequest(aThumbnailRequest), iAttributeId(attributeId)
	{
    TRACER("CGlxThumbnailRequest::CGlxThumbnailRequest()");
	}

CGlxThumbnailRequest::~CGlxThumbnailRequest()
	{
    TRACER("CGlxThumbnailRequest::~CGlxThumbnailRequest()");
	delete iInfo;
	}

EXPORT_C const TGlxMediaId& CGlxThumbnailRequest::ItemId() const
	{
    TRACER("TGlxMediaId& CGlxThumbnailRequest::ItemId()");
	return iThumbnailRequest.iId;
	}
	
EXPORT_C void CGlxThumbnailRequest::ThumbnailRequest(TGlxThumbnailRequest& aThumbnailRequest) const
	{
    TRACER("void CGlxThumbnailRequest::ThumbnailRequest()");
	aThumbnailRequest = iThumbnailRequest;
	}

// -----------------------------------------------------------------------------
// AttributeId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CGlxThumbnailRequest::AttributeId() const
    {
    TRACER("TUint CGlxThumbnailRequest::AttributeId()");
    return iAttributeId;
    }

EXPORT_C void CGlxThumbnailRequest::SetThumbnailInfo(CGlxtnFileInfo* aInfo)
    {
    TRACER("void CGlxThumbnailRequest::SetThumbnailInfo()");
    iInfo = aInfo;
    }

EXPORT_C CGlxtnFileInfo* CGlxThumbnailRequest::ThumbnailInfo() const
    {
    TRACER("CGlxtnFileInfo* CGlxThumbnailRequest::ThumbnailInfo()");
    return (iInfo);
    }
