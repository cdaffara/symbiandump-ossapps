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
* Description:   Get request
*
*/



#include "glxgetrequest.h"

#include <glxlog.h>
#include <glxfilterfactory.h>
#include <glxtracer.h>


EXPORT_C CGlxGetRequest* CGlxGetRequest::NewL(const RArray<TGlxMediaId>& aMediaIdArray, 
			const RArray<TMPXAttribute>& aAttributeArray,
			const RArray<TMPXAttribute>& aCpiAttributeArray,
			const TUid& aCollectionPluginUid, 
			CMPXFilter* aFilter, 
			const TBool aDoNotNeedDataSource)
    {
    TRACER("CGlxGetRequest* CGlxGetRequest::NewL()");
	CGlxGetRequest* task = new (ELeave) CGlxGetRequest(aMediaIdArray, aAttributeArray, 
	    aCpiAttributeArray, aCollectionPluginUid, aDoNotNeedDataSource);
	CleanupStack::PushL(task);
	task->ConstructL(aFilter);
	CleanupStack::Pop(task);
	return task;
    }

CGlxGetRequest::CGlxGetRequest(const RArray<TGlxMediaId>& aMediaIdArray, 
			const RArray<TMPXAttribute>& aAttributeArray, 
			const RArray<TMPXAttribute>& aCpiAttributeArray, 
			const TUid& aCollectionPluginUid, 
			const TBool aDoNotNeedDataSource)
	: CGlxRequest(aCollectionPluginUid, aDoNotNeedDataSource), iMediaIdArray(aMediaIdArray),
	  iAttributeArray(aAttributeArray), iCpiAttributeArray(aCpiAttributeArray)
	{
    TRACER("CGlxGetRequest::CGlxGetRequest()");
	}

CGlxGetRequest::~CGlxGetRequest()
	{
    TRACER("CGlxGetRequest::~CGlxGetRequest()");
	iMediaIdArray.Close();
	iAttributeArray.Close();
	iCpiAttributeArray.Close();
	}

