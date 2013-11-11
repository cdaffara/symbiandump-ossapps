/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     Attachment Waiter for MMS Client MTM and UI.
*
*/



// INCLUDE FILES
#include <mmsvattachmentmanager.h>
#include <msvstore.h>

#include "mmsattachmentwaiter.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
EXPORT_C CMmsAttachmentWaiter* CMmsAttachmentWaiter::NewL()
    {
    CMmsAttachmentWaiter* self = new(ELeave) CMmsAttachmentWaiter();
    return self;
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
CMmsAttachmentWaiter::CMmsAttachmentWaiter()
    : CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
CMmsAttachmentWaiter::~CMmsAttachmentWaiter()
    {
    Cancel();
    delete iStore;
    }    

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
EXPORT_C void CMmsAttachmentWaiter::StartWaitingL(
    TRequestStatus& aStatus, CMsvStore* aStore, MMsvAttachmentManager* aAttachmentManager)
    {
    if( IsActive() )
        User::Leave(KErrInUse);
    // We cannot set our own status to pending state because the component
    // we are giving our status to must do it
    // However, we must set our client's status to pending
    aStatus = KRequestPending;
    iReportStatus = &aStatus;
    iStore = aStore;
    iAttachmentManager = aAttachmentManager;
    SetActive();
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsAttachmentWaiter::Reset()
    {
    iReportStatus = NULL;
    iAttachmentManager = NULL;
    delete iStore;
    iStore = NULL;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsAttachmentWaiter::RunL()
    {
    User::LeaveIfError(iStatus.Int());
    iStore->CommitL();
    User::RequestComplete(iReportStatus, KErrNone);
    Reset();
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMmsAttachmentWaiter::DoCancel()
    {
    iAttachmentManager->CancelRequest();
    User::RequestComplete(iReportStatus, KErrCancel);
    Reset();
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TInt CMmsAttachmentWaiter::RunError(TInt aError)
    {
    User::RequestComplete(iReportStatus, aError);
    Reset();
    return KErrNone;
    }
