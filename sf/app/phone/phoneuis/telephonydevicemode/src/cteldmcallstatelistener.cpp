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
* Description:  Implementation of the CTelDMCallStateListener class.
*
*/



// INCLUDE FILES
#include    "cteldmcallstatelistener.h"
#include    "mteldmcallstateobserver.h"
#include    "mteldmaccessory.h"

#include    <ctsydomainpskeys.h>

// MODULE DATA STRUCTURES
 
// ============================ MEMBER FUNCTIONS ===============================
 
// -----------------------------------------------------------------------------
// CTelDMCallStateListener::NewL
// 
// -----------------------------------------------------------------------------
//  
CTelDMCallStateListener* CTelDMCallStateListener::NewL() 
    {
    
    CTelDMCallStateListener* self =
        new ( ELeave ) CTelDMCallStateListener;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CTelDMCallStateListener::CTelDMCallStateListener
// 
// -----------------------------------------------------------------------------
//
CTelDMCallStateListener::CTelDMCallStateListener():
CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CTelDMCallStateListener::~CTelDMCallStateListener
// 
// -----------------------------------------------------------------------------
//
CTelDMCallStateListener::~CTelDMCallStateListener()
    {
    Cancel();
    iProperty.Close();
    iObserverArray.Reset();
    iObserverArray.Close();
    }

// -----------------------------------------------------------------------------
// CTelDMCallStateListener::ConstructL
// 
// -----------------------------------------------------------------------------
//    
void CTelDMCallStateListener::ConstructL()
    {
    /// Attach to key. Start listening call state changes.
    iProperty.Attach( KPSUidCtsyCallInformation, KCTsyCallState );
    IssueRequest();                               
      
    }
 
// -----------------------------------------------------------------------------
// CTelDMCallStateListener::HandleChange
// 
// -----------------------------------------------------------------------------
//
void CTelDMCallStateListener::HandleChange()
    {
    TInt err( KErrNone );
    iCallState = KErrNone;
    err = RProperty::Get( KPSUidCtsyCallInformation, 
                            KCTsyCallState,
                            iCallState );
    if ( err == KErrNone )
        {                   
        for ( TInt i = 0; i < iObserverArray.Count(); i++ ) 
            {
            TRAP_IGNORE( iObserverArray[ i ]->CallStateChangedL(iCallState) );
            }
        }
    } 
  
// -----------------------------------------------------------------------------
// CTelDMCallStateListener::RunL
// Cannot leave, no RunError implementation
// -----------------------------------------------------------------------------
//
void CTelDMCallStateListener::RunL()
    { 
    if ( iStatus.Int() == KErrNone )
        {
        HandleChange();    
        }
    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CTelDMCallStateListener::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CTelDMCallStateListener::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CTelDMCallStateListener::IssueRequest
// 
// -----------------------------------------------------------------------------
//
void CTelDMCallStateListener::IssueRequest()
    {
    if ( !IsActive() )
        {        
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    }
    
// -----------------------------------------------------------------------------
// CTelDMCallStateListener::AddObserver
// 
// -----------------------------------------------------------------------------
void CTelDMCallStateListener::AddObserverL( MTelDMCallStateObserver& 
                                                            aCallStateObserver )
    {
    if ( iObserverArray.Find( &aCallStateObserver ) != KErrNone )
        {
        User::LeaveIfError( iObserverArray.Append( &aCallStateObserver ) );
        }
    }

 
// End of File
