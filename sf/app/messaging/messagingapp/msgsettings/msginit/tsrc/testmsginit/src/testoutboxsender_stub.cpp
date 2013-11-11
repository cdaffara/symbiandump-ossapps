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
#include "testoutboxsender_stub.h"
#include "debugtraces.h"

// CONSTANTS
const TInt KListBar0(0);

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
//  CTestOutboxSender::NewL
// ----------------------------------------------------
//
CTestOutboxSender* CTestOutboxSender::NewL(CMsvSession& aMsvSession)
{
    CTestOutboxSender* self = new (ELeave) CTestOutboxSender(aMsvSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// ----------------------------------------------------
//  CTestOutboxSender::CTestOutboxSender
// ----------------------------------------------------
//
CTestOutboxSender::CTestOutboxSender(CMsvSession& aMsvSession)
{
	QDEBUG_WRITE( "CTestOutboxObserver::CTestOutboxSender" );
}

// ----------------------------------------------------
//  CTestOutboxSender::~CTestOutboxSender
// ----------------------------------------------------
//
CTestOutboxSender::~CTestOutboxSender()
{
	QDEBUG_WRITE( "CTestOutboxObserver::CTestOutboxSender" );
}

// ----------------------------------------------------
//  CTestOutboxSender::ConstructL
// ----------------------------------------------------
//
void CTestOutboxSender::ConstructL()
{
	QDEBUG_WRITE( "CTestOutboxObserver::ConstructL" );
}

// ----------------------------------------------------
//  CTestOutboxSender::StartSendingL
// ----------------------------------------------------
//
void CTestOutboxSender::StartSendingL()
{
	QDEBUG_WRITE( "CTestOutboxObserver::ConstructL Enter" );
}

// ----------------------------------------------------
//  CTestOutboxSender::CancelSending
// ----------------------------------------------------
//
void CTestOutboxSender::CancelSending()
{
	QDEBUG_WRITE( "CTestOutboxObserver::CancelSending" );
}

// ----------------------------------------------------
//  CTestOutboxSender::IsSending
// ----------------------------------------------------
//
TBool CTestOutboxSender::IsSending() const
{
	QDEBUG_WRITE( "CTestOutboxObserver::IsSending" );
}

// ----------------------------------------------------
//  CTestOutboxSender::OpCompleted
// ----------------------------------------------------
//
void CTestOutboxSender::OpCompleted(CMsvSingleOpWatcher& /*aOpWatcher*/, TInt /*aCompletionCode*/)
{
	QDEBUG_WRITE( "CTestOutboxObserver::OpCompleted" );
}

// ----------------------------------------------------
//  CTestOutboxSender::CheckAndStartSendingL
// ----------------------------------------------------
//
void CTestOutboxSender::CheckAndStartSendingL(const TInt& aNetworkBars)
{
	QDEBUG_WRITE( "CTestOutboxObserver::CheckAndStartSendingL" );
}

// ----------------------------------------------------
//  CTestOutboxSender::CheckBootPhaseL
// ----------------------------------------------------
//
void CTestOutboxSender::CheckBootPhaseL()
{
	QDEBUG_WRITE( "CTestOutboxObserver::CheckBootPhaseL" );
}

// End of file


