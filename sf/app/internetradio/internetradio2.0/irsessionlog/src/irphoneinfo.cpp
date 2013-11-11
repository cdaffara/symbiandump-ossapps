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
* Description:
*
*/


#include "irphoneinfo.h"
#include "mirphoneinfoobserver.h"

// ---------------------------------------------------------------------------
// Function: NewL
// ---------------------------------------------------------------------------
//
CPhoneInfo* CPhoneInfo::NewL(MIRPhoneInfoObserver* aObserver)
    {
    CPhoneInfo* self = new(ELeave) CPhoneInfo(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Function: ~CPhoneInfo
// default destructor
// ---------------------------------------------------------------------------
//
CPhoneInfo::~CPhoneInfo()
    {
    Cancel();
    delete iTelephony;
    iTelephony = NULL;
    }

// ---------------------------------------------------------------------------
// Function: StartUpdatingL
// ---------------------------------------------------------------------------
//
void CPhoneInfo::StartUpdating()
    {
    // Retrieves the model information and unique identification of the mobile device.
    if( !IsActive() && iTelephony )
        {    
        iTelephony->GetPhoneId( iStatus, iPhoneIdV1Pckg );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// Function: CPhoneInfo
// default constructor
// ---------------------------------------------------------------------------
//
CPhoneInfo::CPhoneInfo( MIRPhoneInfoObserver* aObserver )
    : CActive( EPriorityLow ),
      iPhoneIdV1Pckg( iPhoneIdV1 ),
      iTelephony( NULL ),
      iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// Function: ConstructL
// Two phase constructor
// ---------------------------------------------------------------------------
//
void CPhoneInfo::ConstructL()
    {
    iTelephony = CTelephony::NewL();
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// Function: RunL
// From CActive
// ---------------------------------------------------------------------------
//
void CPhoneInfo::RunL()
    {
    if( NULL != iObserver )
        {
        if( KErrNone == iStatus.Int() )
            {
            TRAP_IGNORE( iObserver->ImeiUpdatedL(iPhoneIdV1.iSerialNumber) );
            }
        else
            {
            TRAP_IGNORE( iObserver->ImeiUpdatedL(KNullDesC) );
            }
        }
    }

// ---------------------------------------------------------------------------
// Function: DoCancel
// From CActive
// ---------------------------------------------------------------------------
//
void CPhoneInfo::DoCancel()
    {
    // Cancels an outstanding asynchronous request.
    if( IsActive() && iTelephony )
        {    
        iTelephony->CancelAsync( CTelephony::EGetPhoneIdCancel );
        }
    }
