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
* Description:   Request filter.
*
*/



#include <glxlog.h>

#include "glxcommandrequest.h"
#include <glxtracer.h>

// ----------------------------------------------------------------------------
// CGlxCommandRequest::NewL
// ----------------------------------------------------------------------------
//
CGlxCommandRequest* CGlxCommandRequest::NewL(const CMPXCommand& aCommand, 
	   const TUid aCollectionPluginUid, MGlxDataSourceUpdateObserver& aObserver)
	{
    TRACER("CGlxCommandRequest* CGlxCommandRequest::NewL()");
	CGlxCommandRequest* obj = 
	             new(ELeave) CGlxCommandRequest(aCollectionPluginUid, aObserver);
	CleanupStack::PushL(obj);
	obj->ConstructL(aCommand);
	CleanupStack::Pop(obj);
	return obj;
	}

// ----------------------------------------------------------------------------
// CGlxCommandRequest::~CGlxCommandRequest
// ----------------------------------------------------------------------------
//
CGlxCommandRequest::~CGlxCommandRequest()
	{
    TRACER("CGlxCommandRequest::~CGlxCommandRequest()");
	delete iCommand;
	}

// ----------------------------------------------------------------------------
// CGlxCommandRequest::CGlxCommandRequest
// ----------------------------------------------------------------------------
//
CGlxCommandRequest::CGlxCommandRequest(TUid aCollectionPluginUid, 
		                                MGlxDataSourceUpdateObserver& aObserver)
	: CGlxRequest(aCollectionPluginUid), iDataSourceUpdateObserver(aObserver)
	{
    TRACER("CGlxCommandRequest::CGlxCommandRequest()");	
	}

// ----------------------------------------------------------------------------
// CGlxCommandRequest::ConstructL
// ----------------------------------------------------------------------------
//
void CGlxCommandRequest::ConstructL(const CMPXCommand& aCommand)
	{
    TRACER("void CGlxCommandRequest::ConstructL()");
	iCommand = CMPXCommand::NewL(aCommand);
	}
