/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*       CStartUpMonitor implementation file
*
*/


// INCLUDE FILES
#include <startupdomainpskeys.h>
#include "simscnumberdetector.h"
#include "startupmonitor.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CStartUpMonitor::CStartUpMonitor
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CStartUpMonitor::CStartUpMonitor( CMsgSimOperation* aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {    
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CStartUpMonitor::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CStartUpMonitor::ConstructL()
    {
  
    User::LeaveIfError( iStartupProperty.Attach( KPSUidStartup, KPSGlobalSystemState ) );
    // Complete self
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
  
    }

// ---------------------------------------------------------
// CStartUpMonitor::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CStartUpMonitor* CStartUpMonitor::NewL( CMsgSimOperation* aWatcher )
    {
    
    CStartUpMonitor* self = new ( ELeave )
        CStartUpMonitor( aWatcher );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CStartUpMonitor::~CStartUpMonitor
//
// Destructor
// ---------------------------------------------------------
//
CStartUpMonitor::~CStartUpMonitor()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CStartUpMonitor::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CStartUpMonitor::DoCancel()
    {
    iStartupProperty.Cancel();
    }

// ---------------------------------------------------------
// CStartUpMonitor::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CStartUpMonitor::RunL()
    {

    TInt startupState = 0;
    iStartupProperty.Get( startupState );

    if ( startupState == ESwStateNormalRfOn ||
        startupState == ESwStateNormalRfOff ||
        startupState == ESwStateNormalBTSap )
        {
        iWatcher->HandleStartupReadyL();       
        }
    else
        {
        iStatus = KRequestPending;
        iStartupProperty.Subscribe( iStatus );
        SetActive();
        }

    }

//  End of File  
