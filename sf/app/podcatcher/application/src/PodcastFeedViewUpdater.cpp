/*
* Copyright (c) 2010 Anders Fridlund, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "PodcastFeedViewUpdater.h"
#include "PodcastFeedView.h"

CPodcastFeedViewUpdater* CPodcastFeedViewUpdater::NewL(CPodcastFeedView& aPodcastFeedView)
	{
	CPodcastFeedViewUpdater* self = new (ELeave) CPodcastFeedViewUpdater(aPodcastFeedView);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CPodcastFeedViewUpdater::StartUpdate(TInt aNbrItems)
	{
	// If there is an update in progress, stop it.
	StopUpdate();
	// Reset values to start an update from the beginning
	iNbrItems = aNbrItems;
	iNextItem = 0;
	// Queue this active object to be run once
	Call();
	}

void CPodcastFeedViewUpdater::StopUpdate()
	{
	// Cancel any outstanding request
	Cancel();
	// Reset values to zero. Not really necessary, but made for robustness
	iNbrItems = 0;
	iNextItem = 0;
	}

CPodcastFeedViewUpdater::~CPodcastFeedViewUpdater()
	{
	StopUpdate();
	Deque();
	}
        
CPodcastFeedViewUpdater::CPodcastFeedViewUpdater(CPodcastFeedView& aPodcastFeedView)
	: CAsyncOneShot(EPriorityNormal), iPodcastFeedView(aPodcastFeedView)
	{
	}

void CPodcastFeedViewUpdater::ConstructL()
	{
	}

void CPodcastFeedViewUpdater::RunL()
	{
	iPodcastFeedView.UpdateItemL(iNextItem++);
	if (iNextItem < iNbrItems)
		{
		Call();
		}
	else
		{
		StopUpdate();
		}
	}

TInt CPodcastFeedViewUpdater::RunError(TInt aError)
	{
	return aError;
	}
	
