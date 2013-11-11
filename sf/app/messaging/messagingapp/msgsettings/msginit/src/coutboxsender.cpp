/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:   Implements class
 *
 */

// INCLUDE FILES

#include <CoreApplicationUIsSDKCRKeys.h>
#include <msvuids.h>                 // Entry Uids
#include "coutboxsendoperation.h"
#include "coutboxsender.h"
#include "debugtraces.h"

// CONSTANTS
const TInt KListBar0(0);

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
//  COutboxSender::NewL
// ----------------------------------------------------
//
COutboxSender* COutboxSender::NewL(CMsvSession& aMsvSession)
{
    COutboxSender* self = new (ELeave) COutboxSender(aMsvSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// ----------------------------------------------------
//  COutboxSender::COutboxSender
// ----------------------------------------------------
//
COutboxSender::COutboxSender(CMsvSession& aMsvSession) :
 iMsvSession(aMsvSession)
{
}

// ----------------------------------------------------
//  COutboxSender::~COutboxSender
// ----------------------------------------------------
//
COutboxSender::~COutboxSender()
{
    // Stop and delete the sending operation
    delete iRunningOperation;

    // Check if notification is set
    if (iFlags & EUserSettingsNotifierSet) 
    {
        // Remove the notification from list
        if (iNotifyHandler) 
        {
            iNotifyHandler->StopListening();
            delete iNotifyHandler;
        }

        delete iSession;
    }

}

// ----------------------------------------------------
//  COutboxSender::ConstructL
// ----------------------------------------------------
//
void COutboxSender::ConstructL()
{
    iNetworkBars = KListBar0;
    iSession = CRepository::NewL(KCRUidCoreApplicationUIs);
    iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession, CCenRepNotifyHandler::EIntKey,
        KCoreAppUIsNetworkConnectionAllowed);
    iNotifyHandler->StartListeningL();

    // Turn flag on for possible error handling cases
    iFlags |= EUserSettingsNotifierSet;

    CheckBootPhaseL();
}

// ----------------------------------------------------
//  COutboxSender::StartSendingL
// ----------------------------------------------------
//
void COutboxSender::StartSendingL()
{
    // Check if the sending is already in progress
    if (!IsSending()) 
    {
        // Create instance of Single Operation Watcher
        CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL(*this);

        // Push to cleanup stack while creating sending operation
        CleanupStack::PushL(singleOpWatcher);
        CMsvOperation* op = COutboxSendOperation::NewL(iMsvSession, singleOpWatcher->iStatus);
        CleanupStack::Pop(singleOpWatcher);

        // Set operation
        singleOpWatcher->SetOperation(op); // takes immediately ownership
        iRunningOperation = singleOpWatcher;
    }
}

// ----------------------------------------------------
//  COutboxSender::CancelSending
// ----------------------------------------------------
//
void COutboxSender::CancelSending()
{
    // Remove the running operation
    delete iRunningOperation;
    iRunningOperation = NULL;
}

// ----------------------------------------------------
//  COutboxSender::IsSending
// ----------------------------------------------------
//
TBool COutboxSender::IsSending() const
{
    return (iRunningOperation != NULL);
}

// ----------------------------------------------------
//  COutboxSender::OpCompleted
// ----------------------------------------------------
//
void COutboxSender::OpCompleted(CMsvSingleOpWatcher& /*aOpWatcher*/, TInt /*aCompletionCode*/)
{
    delete iRunningOperation;
    iRunningOperation = NULL;
}

// ----------------------------------------------------
//  COutboxSender::HandleNotifyInt
// ----------------------------------------------------
//
void COutboxSender::HandleNotifyInt(const TUint32 aID, const TInt aNewValue)
{
    QDEBUG_WRITE(("COutboxSender::HandleNotifyInt"));
    // Check if key is for offline-connecton
    if (aID == KCoreAppUIsNetworkConnectionAllowed) 
    {
        QDEBUG_WRITE(("COutboxSender::HandleNotifyInt KCoreAppUIsNetworkConnectionAllowed"));
        // Check if connection is established
        if (aNewValue == ECoreAppUIsNetworkConnectionAllowed) 
        {
            QDEBUG_WRITE(("COutboxSender::HandleNotifyInt ECoreAppUIsNetworkConnectionAllowed 1"));
            // Phone switched on again!
            iFlags |= EOffllineSendingNeeded;

            QDEBUG_WRITE_FORMAT(("COutboxSender::HandleNotifyInt iNetworkBars = %d"), iNetworkBars );
            if (iNetworkBars > KListBar0) 
            {
                QDEBUG_WRITE_FORMAT(("COutboxSender::HandleNotifyInt sending now, iNetworkBars = %d"), iNetworkBars );
                TRAP_IGNORE( StartSendingL() );
            }
        }
        else 
        {
            QDEBUG_WRITE("COutboxSender::HandleNotifyInt ECoreAppUIsNetworkConnectionAllowed 0");
            // Clear flag
            iFlags &= ~EOffllineSendingNeeded;
            // Stop sending
            CancelSending();
            // Set the coverage to 0 in case it didn't come from network in time
            iNetworkBars = KListBar0;
        }
    }
}

// ----------------------------------------------------
//  COutboxSender::HandleNotifyGeneric
// ----------------------------------------------------
//
void COutboxSender::HandleNotifyGeneric(const TUint32 /*aID*/)
{
    //NO OPERATION
}

// ----------------------------------------------------
//  COutboxSender::HandleNotifyError
// ----------------------------------------------------
//
void COutboxSender::HandleNotifyError(const TUint32 /*aID*/, const TInt /*aError*/,
    CCenRepNotifyHandler* /*aHandler*/)
{
    //NO OPERATION
}

// ----------------------------------------------------
//  COutboxSender::CheckAndStartSendingL
// ----------------------------------------------------
//
void COutboxSender::CheckAndStartSendingL(const TInt& aNetworkBars)
{
    QDEBUG_WRITE_FORMAT("COutboxSender::CheckAndStartSendingL aNetworkBars = ", aNetworkBars );
    iNetworkBars = aNetworkBars;
    // Check if sending is needed and network is available
    if (aNetworkBars > KListBar0) 
    {
        QDEBUG_WRITE_FORMAT("COutboxSender::CheckAndStartSendingL sending now, iNetworkBars ", iNetworkBars );
        // Start sending
        StartSendingL();
    }
    else 
    {
        QDEBUG_WRITE_FORMAT("COutboxSender::CheckAndStartSendingL not sending, iNetworkBars = ", iNetworkBars );
    }
}

// ----------------------------------------------------
//  COutboxSender::CheckBootPhaseL
// ----------------------------------------------------
//
void COutboxSender::CheckBootPhaseL()
{
    TInt connection;

    TInt err = iSession->Get(KCoreAppUIsNetworkConnectionAllowed, connection);

    if (err == KErrNone) 
    {
        // Check if connection is established
        if (connection) 
        {
            QDEBUG_WRITE("COutboxSender::CheckBootPhaseL KGSNetworkConnectionAllowed 1");
            // Phone switched on again!            
            iFlags |= EOffllineSendingNeeded;

            if (iNetworkBars > KListBar0) 
            {
                QDEBUG_WRITE_FORMAT("COutboxSender::CheckBootPhaseL sending now, iNetworkBars = ", iNetworkBars );
                StartSendingL();
            }
        }
    }
    else 
    {
        QDEBUG_WRITE("COutboxSender::CheckBootPhaseL Cannot access shared data");
    }
}

// End of file
