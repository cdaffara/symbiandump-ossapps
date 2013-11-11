/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the CTelDMGripHandler class.
*
*/



// INCLUDE FILES

#include    <mphonedevicemodeobserver.h>
#include    <hwrmdomainpskeys.h>
#include    "cteldmgriphandler.h"
#include    "mteldmaccessory.h"
#include    "cteldmcommandhandler.h"
#include    <settingsinternalcrkeys.h>
#include    <centralrepository.h>

// MODULE DATA STRUCTURES
 
// ============================ MEMBER FUNCTIONS ===============================
 
// -----------------------------------------------------------------------------
// CTelDMGripHandler::NewL
// 
// -----------------------------------------------------------------------------
//  
CTelDMGripHandler* CTelDMGripHandler::NewL( CTelDMCommandHandler& aCommandHandler ) 
    {
    
    CTelDMGripHandler* self =
        new ( ELeave ) CTelDMGripHandler( aCommandHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CTelDMGripHandler::CTelDMGripHandler
// 
// -----------------------------------------------------------------------------
//
CTelDMGripHandler::CTelDMGripHandler( CTelDMCommandHandler& aCommandHandler ):
CActive( CActive::EPriorityStandard ),
iCommandHandler ( aCommandHandler )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CTelDMGripHandler::~CTelDMGripHandler
// 
// -----------------------------------------------------------------------------
//
CTelDMGripHandler::~CTelDMGripHandler()
    {
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CTelDMGripHandler::ConstructL
// 
// -----------------------------------------------------------------------------
//    
void CTelDMGripHandler::ConstructL()
    {    
    /// Attach to key. Start listening for changes.
    iProperty.Attach( KPSUidHWRM, KHWRMGripStatus );
    IssueRequest();                               
    }
 
// -----------------------------------------------------------------------------
// CTelDMGripHandler::HandleChange
// 
// -----------------------------------------------------------------------------
//
void CTelDMGripHandler::HandleChange()
    {
    TInt err( KErrNone );
    TInt state( KErrNone ); 
    err = RProperty::Get(
        KPSUidHWRM,
        KHWRMGripStatus,
        state );
 
    if ( err == KErrNone && state == EPSHWRMGripOpen && IsSliderCallAnswer() )
        {
        iCommandHandler.HandleCommand( CTelDMCommandHandler::EGripOpen ); 
        }
    else if ( err == KErrNone && state == EPSHWRMGripClosed && IsSliderCallEnd())
        {
        iCommandHandler.HandleCommand( CTelDMCommandHandler::EGripClose );
        }
    } 
 
 
// -----------------------------------------------------------------------------
// CTelDMGripHandler::RunL
// Cannot leave, no RunError implementation
// -----------------------------------------------------------------------------
//
void CTelDMGripHandler::RunL()
    { 
    if ( iStatus.Int() == KErrNone )
        {
        HandleChange();    
        }
    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CTelDMGripHandler::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CTelDMGripHandler::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CTelDMGripHandler::IssueRequest
// 
// -----------------------------------------------------------------------------
//
void CTelDMGripHandler::IssueRequest()
    {
    if ( !IsActive() )
        {        
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    }

// ---------------------------------------------------------
// CPhoneState::IsSliderCallAnswer
// ---------------------------------------------------------
//
TBool CTelDMGripHandler::IsSliderCallAnswer() const
    {
    TInt answerEnabled = 0;
    TBool retVal ( EFalse );
    TInt err = GetInt(          
        KCRUidTelephonySettings,
        KSettingsOpeningSlideAnswer,
        answerEnabled );
        
    if ( err == KErrNone && answerEnabled )
        {
        retVal = ETrue;            
        }
    return retVal;
    }

// ---------------------------------------------------------
// CPhoneState::IsSliderCallEnd
// ---------------------------------------------------------
//
TBool CTelDMGripHandler::IsSliderCallEnd() const
    {
    TInt endEnabled = 0;
    TBool retVal ( EFalse );
    TInt err = GetInt(          
        KCRUidTelephonySettings,
        KSettingsClosingSlideEnd,
        endEnabled );
        
    if ( err == KErrNone && endEnabled )
        {
        retVal = ETrue;            
        }
    return retVal;
    }

// ---------------------------------------------------------
// CTelDMGripHandler::GetInt
// ---------------------------------------------------------
//
TInt CTelDMGripHandler::GetInt( 
    const TUid& aUid, 
    const TUint aId,
    TInt& aValue ) const
    {
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( aUid ) );
    if ( err == KErrNone )
        {
        err = repository->Get( aId, aValue );
        delete repository;
        }
    return err;
    }
    
// End of File
