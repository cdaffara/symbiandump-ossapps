/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Allows a synchronous wait on a operation
 *
 */

// INCLUDE FILES
#include <aknenv.h>
#include "MuiuOperationWait.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CMuiuOperationWait::NewLC
//
// ---------------------------------------------------------
//
EXPORT_C CMuiuOperationWait* CMuiuOperationWait::NewLC( TInt aPriority )
    {
    CMuiuOperationWait* self = new ( ELeave ) CMuiuOperationWait( aPriority );
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------
// CMuiuOperationWait::CMuiuOperationWait
//
// ---------------------------------------------------------
//
CMuiuOperationWait::CMuiuOperationWait( TInt aPriority )
: CActive( aPriority )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CMuiuOperationWait::~CMuiuOperationWait
//
// ---------------------------------------------------------
//
EXPORT_C CMuiuOperationWait::~CMuiuOperationWait()
    {
    Cancel();
    }


// ---------------------------------------------------------
// CMuiuOperationWait::Start
//
// ---------------------------------------------------------
//
EXPORT_C void CMuiuOperationWait::Start()
    {
    SetActive();
    iWait.Start();
    }


// ---------------------------------------------------------
// CMuiuOperationWait::RunL
//
// ---------------------------------------------------------
//
void CMuiuOperationWait::RunL()
    {
    CAknEnv::StopSchedulerWaitWithBusyMessage( iWait );
    }


// ---------------------------------------------------------
// CMuiuOperationWait::DoCancel
//
// ---------------------------------------------------------
//
void CMuiuOperationWait::DoCancel()
    {
    if( iStatus == KRequestPending )
        {
        TRequestStatus* s=&iStatus;
        User::RequestComplete( s, KErrCancel );
        }
    CAknEnv::StopSchedulerWaitWithBusyMessage( iWait );
    }

// End of file
