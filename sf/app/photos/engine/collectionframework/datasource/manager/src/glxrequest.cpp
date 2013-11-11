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
* Description:   request.
*
*/



#include "glxrequest.h"
#include <glxfilterfactory.h>
#include <glxlog.h>
#include <glxtracer.h>

EXPORT_C CGlxRequest::~CGlxRequest()
	{
    TRACER("EXPORT_C CGlxRequest::~CGlxRequest()");
	delete iFilter;
	}

CGlxRequest::CGlxRequest(const TUid& aCollectionPluginUid, const TBool aDoNotNeedDataSource)
	: iCollectionPluginUid(aCollectionPluginUid),
	iDoNotNeedDataSource(aDoNotNeedDataSource)
	{
    TRACER("CGlxRequest::CGlxRequest()");
	}

EXPORT_C TUid CGlxRequest::CollectionPluginUid() const
	{
    TRACER("TUid CGlxRequest::CollectionPluginUid()");
	return iCollectionPluginUid;
	}

EXPORT_C CMPXFilter* CGlxRequest::Filter() const
    {
    TRACER("CMPXFilter* CGlxRequest::Filter()");
    return iFilter;
    }
    
void CGlxRequest::ConstructL(const CMPXFilter* aFilter)
	{
    TRACER("void CGlxRequest::ConstructL(const CMPXFilter* aFilter)");
	if (aFilter)
	    {
	    // Take a copy of the filter as we do not own it.
    	iFilter = CMPXFilter::NewL(*aFilter);
	    }
	else
	    {
	    // No filter supplied, so create a default filter;
	    iFilter = TGlxFilterFactory::CreateAlphabeticSortFilterL();
	    }
	}
	
