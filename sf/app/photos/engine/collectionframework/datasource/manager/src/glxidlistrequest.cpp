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
* Description:   Id list request
*
*/



#include "glxidlistrequest.h"
#include <glxfilterfactory.h>
#include <glxlog.h>
#include <glxtracer.h>
// ----------------------------------------------------------------------------
// CGlxIdListRequest::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxIdListRequest* CGlxIdListRequest::NewL(TGlxMediaId aContainerId, 
                       const TUid& aCollectionPluginUid, CMPXFilter* aFilter, 
                                                   CMPXCollectionPath& aPath)
	{
    TRACER("CGlxIdListRequest* CGlxIdListRequest::NewL()");
	CGlxIdListRequest* task = new (ELeave) CGlxIdListRequest(aCollectionPluginUid, aContainerId, aPath);
	CleanupStack::PushL(task);
	task->ConstructL(aFilter);
	CleanupStack::Pop(task);
	return task;
	}

// ----------------------------------------------------------------------------
// CGlxIdListRequest::CGlxIdListRequest
// ----------------------------------------------------------------------------
//
CGlxIdListRequest::CGlxIdListRequest(const TUid& aCollectionPluginUid, 
		                   TGlxMediaId aContainerId, CMPXCollectionPath& aPath)
	: CGlxRequest(aCollectionPluginUid),
	iMediaId(aContainerId),
	iPath(aPath)
	{
    TRACER("CGlxIdListRequest::CGlxIdListRequest()");
	}

CGlxIdListRequest::~CGlxIdListRequest()
	{
    TRACER("CGlxIdListRequest::~CGlxIdListRequest()");
	}
