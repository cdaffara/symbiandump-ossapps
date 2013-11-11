/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Album collection plugin definition
*
*/



#include "glxcollectionplugintestbase.h"

#include <mpxcollectionpath.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxcmn.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>

// ----------------------------------------------------------------------------
// Creates an Async notification
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginTestBase::AsyncNotifyL(CMPXMedia*& aMPXMedia, TInt aError, CGlxAsyncNotifier::TCallType aCallType)
	{
		ASSERT(0 == iAsyncNotifier);
		iAsyncNotifier = CGlxAsyncNotifier::NewL(iObs, aMPXMedia, aError, aCallType, this);
	}

// ----------------------------------------------------------------------------
// Creates an Async notification
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginTestBase::AsyncNotifyL(TInt aError, CGlxAsyncNotifier::TCallType aCallType)
	{
		ASSERT(0 == iAsyncNotifier);
		ASSERT(0 == iDummyMedia);
		iAsyncNotifier = CGlxAsyncNotifier::NewL(iObs, iDummyMedia, aError, aCallType, this);
	}

// ----------------------------------------------------------------------------
// Cancels any outstanding async request
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginTestBase::CancelRequest()
	{
	if (iAsyncNotifier)
		{
		iAsyncNotifier->CancelRequest();
		}
	}
	
// ----------------------------------------------------------------------------
// This method get called when the Async request has completed. Now we can tidy
// up.
// ----------------------------------------------------------------------------
//
void  CGlxCollectionPluginTestBase::AsyncCallBackCompleted(CGlxAsyncNotifier* aPtr)
	{
	ASSERT(aPtr == iAsyncNotifier);	// Just to check that we are deleting what we expect.
	// Async function is now complete so delete the Async Notifier
	delete iAsyncNotifier;
	iAsyncNotifier = NULL;
	}

	
