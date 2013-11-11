/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility class for waiting idle.
*
*/


// INCLUDE FILES
#include    "ctelewaitingidle.h"
#include    "cphonerecoverysystem.h"
#include    "phonelogger.h"
#include    <startupdomainpskeys.h>


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CTeleWaitingIdle::CTeleWaitingIdle
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTeleWaitingIdle::CTeleWaitingIdle( 
                        TInt aPriority, 
                        CTeleRecoverySystem& aRecoverySystem ) : 
                        CActive( aPriority), 
                        iRecoverySystem( aRecoverySystem )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CTeleWaitingIdle::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTeleWaitingIdle* CTeleWaitingIdle::NewL( TInt aPriority, 
                                    CTeleRecoverySystem& aRecoverySystem )
    {
    CTeleWaitingIdle* self = new( ELeave ) CTeleWaitingIdle( 
                                aPriority, aRecoverySystem );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CTeleWaitingIdle::~CTeleWaitingIdle()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CTeleWaitingIdle::~CTeleWaitingIdle()
    {
    Cancel();
        
    if ( iProperty.Handle() )
        {
        iProperty.Close();
        }
    }

// -----------------------------------------------------------------------------
// CTeleWaitingIdle::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTeleWaitingIdle::DoCancel()
    {        
    if ( iProperty.Handle() )
        {
        iProperty.Cancel();
        }
    }    

// -----------------------------------------------------------------------------
// CTeleWaitingIdle::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTeleWaitingIdle::RunL()
    {
//    Phone_PRINT("WI.RunL >");
    if ( iStatus != KErrNone )
        {
        //error code is ignored, as CPeriodic. 
        return;
        }
    StartWaitingIdleL();
    }

// -----------------------------------------------------------------------------
// CTeleWaitingIdle::StartWaitingIdle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTeleWaitingIdle::StartWaitingIdleL()
    {
//    Phone_PRINT("WI.StartWaitingIdle >");
    TInt state(0);

    User::LeaveIfError( 
        iProperty.Get( 
        KPSUidStartup, 
        KPSIdlePhase1Ok, 
        state ) );
        
//    Phone_PRINTF("CTeleWaitingIdle::After - state = %d >",state);  
    if ( IsActive() )
        {
        Cancel();
        }
        
    if ( state == EIdlePhase1Ok )
        {
//        Phone_PRINT("WI.StartWaitingIdle - RecoverAllNow");
        iRecoverySystem.RecoverAllNow();
        }
    else
        {
        User::LeaveIfError( 
            iProperty.Attach( 
            KPSUidStartup, 
            KPSIdlePhase1Ok ) );
                
        if ( !IsActive() )
            {        
            iProperty.Subscribe( iStatus );
            SetActive();
            }       
        }
    }
    
//  End of File  
