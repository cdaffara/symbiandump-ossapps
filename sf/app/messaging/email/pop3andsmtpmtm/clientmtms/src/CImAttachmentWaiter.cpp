// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <mmsvattachmentmanager.h>
#include "CImAttachmentWaiter.h"
#include "MIUTMSG.H"

/**
Allocates & Creates a new CImAttachmentWaiter object.
@return  A pointer to the newly created object
*/
EXPORT_C CImAttachmentWaiter* CImAttachmentWaiter::NewL()
	{
	CImAttachmentWaiter* self = new(ELeave) CImAttachmentWaiter();
	return self;
	}
	
/**
Adds the attachment waiter active object to the active scheduler.
*/
CImAttachmentWaiter::CImAttachmentWaiter()
	: CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

/**
Cancels the outstanding request.
Frees all resources owned by the object prior to its destruction
*/
EXPORT_C CImAttachmentWaiter::~CImAttachmentWaiter()
	{
	Cancel();
	if (iDeleteMessage)
 		{		
 		delete iMailMessage;
 		iMailMessage = NULL;
 		}
	}	

/**
Issues a waiting request for adding an attachment .

@param aStatus		The TRequestStatus to complete when request has completed.
@param aMailMessage	The CImEmailMessage pointer to which the attachment is to be added
@param aDeleteMessage	TBool indicating if the CImEmailMessage needs to be deleted when CImAttachmentWaiter is Reset or  destroyed
@leave KErrInUse
*/
EXPORT_C void CImAttachmentWaiter::StartWaitingL(TRequestStatus& aStatus, CImEmailMessage* aMailMessage, TBool aDeleteMessage)
	{
	if( IsActive() )
		{
		aMailMessage->AttachmentManager().CancelRequest();
		User::Leave(KErrInUse);
		}
	
	aStatus = KRequestPending;
	iReportStatus = &aStatus;
	iMailMessage = aMailMessage;
	iDeleteMessage = aDeleteMessage;
	SetActive();
	}

/**
Resets the values. 
*/
void CImAttachmentWaiter::Reset()
	{
	iReportStatus = NULL;
	if (iDeleteMessage)
 		{		
 		delete iMailMessage;
 		iMailMessage = NULL;
 		}
	}

void CImAttachmentWaiter::RunL()
	{
	User::RequestComplete(iReportStatus, iStatus.Int());
	Reset();
	}
	
/**
It cancels an outstanding request.
*/
void CImAttachmentWaiter::DoCancel()
	{
	iMailMessage->Cancel();
	User::RequestComplete(iReportStatus, KErrCancel);
	Reset();
	}

