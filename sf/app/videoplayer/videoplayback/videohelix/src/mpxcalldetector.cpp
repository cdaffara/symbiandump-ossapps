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
* Description:  
*
*/

// Version : %version: 2 %

// INCLUDE FILES
#include <ctsydomainpskeys.h>

#include "mpxcalldetector.h"
#include "mpxvideo_debug.h"


// ---------------------------------------------------------------------------
// CCallDetector::NewL
//
// ---------------------------------------------------------------------------
//
CMPXCallDetector* CMPXCallDetector::NewL( MMPXCallDetectorObserver* aObserver )
{
    MPX_ENTER_EXIT(_L("CMPXCallDetector::NewL()"));
    
    CMPXCallDetector* self = new( ELeave ) CMPXCallDetector( aObserver );        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );    
    return self;
}

// ---------------------------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------------------------
//
CMPXCallDetector::~CMPXCallDetector()
{
    Cancel();
    iTsyProperty.Close();
}

// ---------------------------------------------------------------------------
// CCallDetector::RequestNotification
// 
// ---------------------------------------------------------------------------
// 
void CMPXCallDetector::RequestNotification()
{
    MPX_ENTER_EXIT(_L("CCallDetector::RequestNotification()"));

    iTsyProperty.Subscribe( iStatus );
    SetActive();
}

// ---------------------------------------------------------------------------
// Constructor
//
// ---------------------------------------------------------------------------
//
CMPXCallDetector::CMPXCallDetector( MMPXCallDetectorObserver* aObserver )
 : CActive( EPriorityStandard ), iObserver( aObserver )
{
}

// ---------------------------------------------------------------------------
// CCallDetector::ConstructL
//
// ---------------------------------------------------------------------------
//
void CMPXCallDetector::ConstructL()
{
    MPX_ENTER_EXIT(_L("CCallDetector::ConstructL"));	
    
    User::LeaveIfError( iTsyProperty.Attach( KPSUidCtsyCallInformation, KCTsyCallState ) );
    
    CActiveScheduler::Add( this );
}

// ---------------------------------------------------------------------------
// CCallDetector::RunL
// Called when the voice line status changes.
// ---------------------------------------------------------------------------
//
void CMPXCallDetector::RunL()
{
    TInt status = iStatus.Int();
    
    RequestNotification();
    
    if ( status == KErrNone )
    {
        TInt callStatus;
        iTsyProperty.Get( callStatus );
        
        if ( callStatus != EPSCTsyCallStateNone && callStatus != EPSCTsyCallStateDisconnecting )
        {
            iObserver->CallDetectedL();
        }
    }
}

// ---------------------------------------------------------------------------
// CCallDetector::DoCancel
//
// ---------------------------------------------------------------------------
//
void CMPXCallDetector::DoCancel()
{
    iTsyProperty.Cancel();
}

// End of File
