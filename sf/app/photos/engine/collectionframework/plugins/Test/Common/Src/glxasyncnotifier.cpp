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
#include <glxlog.h>
	
// ----------------------------------------------------------------------------
// Instantiates the class to handle to Async notification
// ----------------------------------------------------------------------------
//
CGlxAsyncNotifier* CGlxAsyncNotifier::NewL(MMPXCollectionPluginObserver* aObs, CMPXMedia*& aMPXMedia, TInt aError, TCallType aCallType, MGlxAsyncCallBackObserver* aParent)
	{
	CGlxAsyncNotifier* self = new (ELeave) CGlxAsyncNotifier(aObs, aMPXMedia, aError, aCallType, aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


// ----------------------------------------------------------------------------
// Constructor. Just initialises variables
// ----------------------------------------------------------------------------
//
CGlxAsyncNotifier::CGlxAsyncNotifier(MMPXCollectionPluginObserver* aObs, CMPXMedia*& aMPXMedia, TInt aError, TCallType aCallType, MGlxAsyncCallBackObserver* aParent)
	: iObs(aObs), iMPXMedia(aMPXMedia), iError(aError), iCallType(aCallType), iParent(aParent)
	{
	
	}
	
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxAsyncNotifier::~CGlxAsyncNotifier()
	{
	CleanUp();
	}

void CGlxAsyncNotifier::ConstructL()
	{
	GLX_LOG_ENTRY_EXIT("AsyncNotifier ConstructL ");
	// Does a CMPXMedia item exist?
	// If not we need to create a dummy one
	if (NULL == iMPXMedia)
		{
		iMPXMedia = CreateDummyMPXMediaL();
		}	
	
	// Create the Async mechansim
	TCallBack asyncCallBack( StaticAsyncCallBack, this);
    iAsyncCallBack = new (ELeave) CAsyncCallBack(asyncCallBack, CActive::EPriorityStandard);
    iAsyncCallBack->CallBack();
	}

// ----------------------------------------------------------------------------
// Create a dummy CMPXMedia
// ----------------------------------------------------------------------------
//
CMPXMedia* CGlxAsyncNotifier::CreateDummyMPXMediaL()
	{
    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    supportedIds.AppendL(KMPXMediaIdContainer);
    supportedIds.AppendL(KMPXMediaIdGeneral);
    CMPXMedia *dummyEntry = CMPXMedia::NewL(supportedIds.Array());
	CleanupStack::PushL(dummyEntry);
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);

    dummyEntry->SetCObjectValueL(KMPXMediaArrayContents, array);
    dummyEntry->SetTObjectValueL(KMPXMediaArrayCount, array->Count());

    CleanupStack::PopAndDestroy(array);
    CleanupStack::Pop(dummyEntry);  
    CleanupStack::PopAndDestroy(&supportedIds);
    
    return dummyEntry;      
	}


// ----------------------------------------------------------------------------
// Static Async callback method
// ----------------------------------------------------------------------------
//
TInt CGlxAsyncNotifier::StaticAsyncCallBack(TAny* aPtr)
	{
	// Get a pointer to this class and call class method
	CGlxAsyncNotifier* self = static_cast<CGlxAsyncNotifier*>(aPtr);
	TInt result =  self->AsyncCallBack();

	// Now the request has completed. and the AsyncCallBack method has tidied up. notify parent
	// This must be last as it is expected for the parent to delete this class (self).
	self->iParent->AsyncCallBackCompleted(self);
	
	return result;
	}

// ----------------------------------------------------------------------------
// Async callback method
// ----------------------------------------------------------------------------
//
TInt CGlxAsyncNotifier::AsyncCallBack()
	{
	// Call the observer and tidy up
	switch (iCallType)
		{
		case EOpen:
	    	iObs->HandleOpen(iMPXMedia, iError);
			break;
			
		case EMedia:
	    	iObs->HandleMedia(iMPXMedia, iError);
			break;
			
		case EFindAll:
	    	iObs->HandleFindAll(iMPXMedia, iError);
			break;
		}
		
	CleanUp();
	
	return 0;
	}

// ----------------------------------------------------------------------------
// Clean everything up
// ----------------------------------------------------------------------------
//
void CGlxAsyncNotifier::CleanUp()
	{
	
	if (iAsyncCallBack)
		{
		if (iAsyncCallBack->IsActive())
			{
		    iAsyncCallBack->Cancel();
			}
	    delete iAsyncCallBack;
	    iAsyncCallBack = NULL;
		}
		
    delete iMPXMedia;		// No longer need this as the request has been cancelled
    iMPXMedia = NULL;
	}

// ----------------------------------------------------------------------------
// Cancel an oustanding Async request
// ----------------------------------------------------------------------------
void CGlxAsyncNotifier::CancelRequest()
	{
	CleanUp();
	}

