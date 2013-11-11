/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPEAccessoryModeMonitor class 
*
*/



// INCLUDE FILES
#include "cpeaccessorymodemonitor.h"
#include <talogger.h>
   
// CONSTANTS
 
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEAccessoryModeMonitor::CPEAccessoryModeMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEAccessoryModeMonitor::CPEAccessoryModeMonitor():
    CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPEAccessoryModeMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEAccessoryModeMonitor* CPEAccessoryModeMonitor::NewL()
    {
    CPEAccessoryModeMonitor* self = 
        new (ELeave) CPEAccessoryModeMonitor();

    CleanupStack::PushL( self );
    CleanupStack::Pop( self );

    return self;
    }
    
// Destructor
CPEAccessoryModeMonitor::~CPEAccessoryModeMonitor()
    {
    Cancel();
    iMode.CloseSubSession();
    iAccServer.Disconnect();
    }

// -----------------------------------------------------------------------------
// CPEAccessoryModeMonitor::AccessoryServer
// -----------------------------------------------------------------------------
//
RAccessoryServer CPEAccessoryModeMonitor::AccessoryServer()
    {
    if ( !iAccConn )
        {
        TInt error = iAccServer.Connect();
        if ( error == KErrNone )
            {
            iAccConn = ETrue;
            }
        }
    return iAccServer;
    }

// -----------------------------------------------------------------------------
// CPEAccessoryModeMonitor::AccessoryMode
// -----------------------------------------------------------------------------
//
TInt CPEAccessoryModeMonitor::AccessoryMode()
    {

    if ( !iAccSess )
        {
        if ( !iAccConn )
            {
            AccessoryServer();    
            }
        if ( iAccConn )
            {
            TInt error = iMode.CreateSubSession( iAccServer );
            if ( error == KErrNone )
                {
                iAccSess = ETrue;    
                }
            }
        }
    if ( iAccSess && !IsActive() )
        {
        iMode.GetAccessoryMode( iPolAccessoryMode );
        iAccessoryMode = iPolAccessoryMode.iAccessoryMode;    
        iAudioOutputStatus = iPolAccessoryMode.iAudioOutputStatus;
        Start();        
        }
    TEFLOGSTRING2( KTAINT, 
        "CPEAccessoryModeMonitor.AccessoryMode< Mode: %d ", iAccessoryMode );        
    return iAccessoryMode;           
    }
    
// -----------------------------------------------------------------------------
// CPEAccessoryModeMonitor::Status
// -----------------------------------------------------------------------------
//
 
TBool CPEAccessoryModeMonitor::Status()
    {
    if ( !iAccSess )
        {
        AccessoryMode();
        }
    return iAudioOutputStatus;
    }    
// -----------------------------------------------------------------------------
// CPEAccessoryModeMonitor::DoCancel
//
// -----------------------------------------------------------------------------
//
void CPEAccessoryModeMonitor::DoCancel()
    {
    iMode.CancelNotifyAccessoryModeChanged();
    }

// -----------------------------------------------------------------------------
// CPEAccessoryModeMonitor::RunL
// Receives event on accessory mode change.
//
// -----------------------------------------------------------------------------
//
void CPEAccessoryModeMonitor::RunL()
    {
    TEFLOGSTRING2(KTAINT, "CPEAccessoryModeMonitor::RunL %d", iStatus.Int() );
    
    switch ( iStatus.Int() )
        {
        case KErrNone:
            {
            iAccessoryMode = iPolAccessoryMode.iAccessoryMode;    
            iAudioOutputStatus = iPolAccessoryMode.iAudioOutputStatus;
            // Issue request again if needed.
            Start();
            }
            break;
        case KErrCancel:
        case KErrNotSupported:
            break;
        default:
            // Issue request again if needed.
             Start();
            break;
        }
     }


// -----------------------------------------------------------------------------
// CPEAccessoryModeMonitor::Start
// 
//
// -----------------------------------------------------------------------------
//     
void CPEAccessoryModeMonitor::Start()
    {
    TEFLOGSTRING(KTAINT, "CPEAccessoryModeMonitor::Start" );
    if ( !IsActive() )
        {
        iMode.NotifyAccessoryModeChanged( iStatus, iPolAccessoryMode );
        SetActive();                          
        }    
    }
 
//  End of File  
