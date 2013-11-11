/* 
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: process monitor.                 
 *
 */

#include "hsprocessmonitor.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CHsProcessMonitor::NewL()
// Two-phased constructor
// -----------------------------------------------------------------------------
CHsProcessMonitor* CHsProcessMonitor::NewL( const TInt iProcessId,
                                            MHsProcessMonitorObserver& aObserver )
    {
    CHsProcessMonitor* self = new ( ELeave ) CHsProcessMonitor( iProcessId,
                                                                aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHsProcessMonitor::~CHsProcessMonitor()
// Destructor.
// -----------------------------------------------------------------------------
//
CHsProcessMonitor::~CHsProcessMonitor()
    {
    Cancel();
    SecureProcessHandleAsClosed();    	
    iProcessId = KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CHsProcessMonitor::AttachMonitor()
// Attach monitor
// -----------------------------------------------------------------------------
//
TInt CHsProcessMonitor::AttachMonitor()
    {   
    SecureProcessHandleAsClosed();
    
    TInt error = iProcess.Open( iProcessId, EOwnerProcess );
    
    if( error == KErrNone )
        {
        iProcessHandleOpen = ETrue;
        }
    
    return error;
    }   

// -----------------------------------------------------------------------------
// CHsProcessMonitor::StartMonitor()
// Start monitor
// -----------------------------------------------------------------------------
//
TInt CHsProcessMonitor::StartMonitor()
    {
    TInt error = KErrNone;
    
    if( !iProcessHandleOpen )
        {
        error = AttachMonitor();
        }
    
    if( error == KErrNone )
        {
        iProcess.Logon( iStatus );
        SetActive();                
        }
    
    return error;
    }   

// -----------------------------------------------------------------------------
// CHsProcessMonitor::CHsProcessMonitor()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CHsProcessMonitor::CHsProcessMonitor( const TInt aProcessId,
                                      MHsProcessMonitorObserver& aObserver ) :
        CActive( EPriorityStandard ),
        iProcessId( aProcessId ),
        iObserver( aObserver ),
        iProcessHandleOpen( EFalse )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CHsProcessMonitor::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHsProcessMonitor::ConstructL()
    {    
    }

// -----------------------------------------------------------------------------
// CHsProcessMonitor::RunL
// -----------------------------------------------------------------------------
//
void CHsProcessMonitor::RunL()
    {
    iProcess.LogonCancel( iStatus );    	
    
    const TInt error = iStatus.Int();        
    
    if( error == KErrCancel )
        {
        }
    else if( error == KErrNoMemory )
        {
        User::Leave( KErrNoMemory );
        }
    else
        {
        const TExitType exitType = iProcess.ExitType();									
        if( exitType == EExitPending )
            {
            User::LeaveIfError( StartMonitor() );
            }
        else
            {
            iObserver.ProcessEnded( exitType,
                                    iProcess.ExitReason(),
                                    iProcess.ExitCategory() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHsProcessMonitor::DoCancel()
// From CActive.
// -----------------------------------------------------------------------------
//
void CHsProcessMonitor::DoCancel()
    {
    iProcess.LogonCancel( iStatus );
    }      

// -----------------------------------------------------------------------------
// CHsProcessMonitor::RunError()
// From CActive.
// -----------------------------------------------------------------------------
//
TInt CHsProcessMonitor::RunError( TInt aError )
    {    
    iObserver.ProcessMonitoringError( aError );

    // Observer will handle error. Return green light.
    return KErrNone;    
    }  

// -----------------------------------------------------------------------------
// CHsProcessMonitor::SecureProcessHandleAsClosed()
// Close process handle if open
// -----------------------------------------------------------------------------
//
void CHsProcessMonitor::SecureProcessHandleAsClosed()
    {
    if( iProcessHandleOpen )
        {
        iProcess.Close();
        iProcessHandleOpen = EFalse;
        }
    } 

// End of File
