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
* Description: Data Source Task Class.
*
*/


#include "glxdatasourcetask.h"

#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <glxtracer.h>

#include "glxrequest.h"
#include "glxidlistrequest.h"
#include "glxdatasource.h"
#include "glxlog.h"

EXPORT_C CGlxDataSourceTask::~CGlxDataSourceTask()
	{
    TRACER("EXPORT_C CGlxDataSourceTask::~CGlxDataSourceTask()");
	delete iRequest;
	delete iResponse;
	}

EXPORT_C CGlxDataSourceTask::CGlxDataSourceTask(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource)
    : iRequest(aRequest), iDataSource(aDataSource), iObserver(&aObserver)
    {
    TRACER("EXPORT_C CGlxDataSourceTask::CGlxDataSourceTask()");
    }

TBool CGlxDataSourceTask::ObserverMatch(MGlxDataSourceRequestObserver& aObserver)
	{
    TRACER("TBool CGlxDataSourceTask::ObserverMatch(MGlxDataSourceRequestObserver& aObserver)");
	if(&aObserver == iObserver)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
	
EXPORT_C void CGlxDataSourceTask::ListToMediaL(const RArray<TGlxMediaId>& aList)
	{
    TRACER("void CGlxDataSourceTask::ListToMediaL(const RArray<TGlxMediaId>& aList)");    
	RArray<TMPXItemId> mpxIds;
	CleanupClosePushL(mpxIds);
 
	for (TInt i = 0; i < aList.Count(); i++)
		{
		mpxIds.AppendL(aList[i].Value());
		}
    if (dynamic_cast<CGlxIdListRequest*>(iRequest))
        {
        CGlxIdListRequest* request = static_cast<CGlxIdListRequest*>(iRequest);
        request->CollectionPath().AppendL(mpxIds.Array());
        }

    CleanupStack::PopAndDestroy(&mpxIds);
	}
	
EXPORT_C void CGlxDataSourceTask::HandleRequestComplete(TInt aError)
	{
    TRACER("void CGlxDataSourceTask::HandleRequestComplete(TInt aError)");
    GLX_DEBUG2("Entering CGlxDataSourceTask::HandleRequestComplete() error %d", aError);
	if( !iCancelled && iResponse && iObserver )
		{
		iObserver->HandleResponse(iResponse, iRequest, aError);
	    iDataSource->HandleTaskComplete(this);
		}
	}

// ----------------------------------------------------------------------------
// CreateResponseL()
///@todo Remove this method
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxDataSourceTask::CreateResponseL()
	{
    TRACER("void CGlxDataSourceTask::CreateResponseL()");
	iResponse = CMPXMedia::NewL();
	}

TBool CGlxDataSourceTask::IsDataSourceNeeded() const
    {
    TRACER("TBool CGlxDataSourceTask::IsDataSourceNeeded()");
    return !iRequest->DoNotNeedDataSource();
    }
