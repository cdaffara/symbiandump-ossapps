/*
* ============================================================================
*  Name        : CWPSaver.cpp
*  Part of     : Provisioning / ProvisioningBC
*  Description : Helper class for saving Provisioning settings. Provides a progress note.
*  Version     : %version: 3 % << Don't touch! Updated by Synergy at check-out.
*
*  Copyright © 2002-2006 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
* ============================================================================
*/

// INCLUDE FILES
#include <commdb.h>
#include <CWPEngine.h>
#include <CWPAdapter.h>

#include "CWPSaver.h"
#include <HbProgressDialog>
#include <HbAction>

// CONSTANTS
const TInt KMaxWaitTime = 2000000;
const TInt KRetryCount = 5;

// CLASS DECLARATION

// ========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// C++ default constructor.
// ----------------------------------------------------------------------------
CWPSaver::CWPSaver( CWPEngine& aEngine, TBool aSetAsDefault )
                  : CActive( EPriorityStandard ),
                    iEngine( aEngine ), 
                    iSetAsDefault( aSetAsDefault ),
                    iCurrentItem( 0 ),
                    iResult( KErrNone )
    {
    }

// ----------------------------------------------------------------------------
// CWPSaver::PrepareLC
// ----------------------------------------------------------------------------
//
void CWPSaver::PrepareLC()
    {
    // Assume ownership of this.
    CleanupStack::PushL( this );
    iProgress = new HbProgressDialog(HbProgressDialog::WaitDialog);
    iProgress->setText(hbTrId("txt_deviceupdate_dblist_config_set_val_saving"));
    iProgress->clearActions();   
    iProgress->show();

 }

// ----------------------------------------------------------------------------
// CWPSaver::ExecuteLD
// ----------------------------------------------------------------------------
//
TInt CWPSaver::ExecuteLD( TInt& aNumSaved, TBufC<256>& Value )
    {
    PrepareLC();

    // Add us to active scheduler and make sure RunL() gets called.
    CActiveScheduler::Add( this );
    CompleteRequest();
    iWait.Start();

    // Progress note has been finished/cancelled. Cache the result
    // and delete this.
    TInt result( iResult );
    aNumSaved = iCurrentItem;
    Value = iValue;
    CleanupStack::PopAndDestroy(); // this

    return result;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CWPSaver::~CWPSaver()
    {
    Cancel();
  }

// ----------------------------------------------------------------------------
// CWPSaver::DoCancel
// ----------------------------------------------------------------------------
//
void CWPSaver::DoCancel()
    {
    }

// ----------------------------------------------------------------------------
// CWPSaver::RunL
// ----------------------------------------------------------------------------
//
void CWPSaver::RunL()
    {
    // Choose whether to save or set as default
    TInt err( KErrNone );
    if( iSetAsDefault )
        {
        TRAP( err, 
            if( iEngine.CanSetAsDefault( iCurrentItem ) )
                {
                iEngine.SetAsDefaultL( iCurrentItem );
                } );
        }
    else
        {
        TRAP(err, QT_TRYCATCH_LEAVING( iEngine.SaveL( iCurrentItem ) )); 
        }
    if( err != KErrNone )
        {
        // For all other errors, pass them through.
        iValue = iEngine.SummaryText(iCurrentItem);
        delete iProgress;
        iProgress = NULL;
        User::LeaveIfError( err );
        }
    // Succesful save, so reset retry count
    iRetryCount = 0;

    // Normal progress
    if( iCurrentItem == iEngine.ItemCount()-1 )
        {
        //iProgress->cancel();
        //iProgress->close();
        delete iProgress;
        iProgress = NULL;
        iWait.AsyncStop();       
        }
    else
        {
        //CEikProgressInfo* progressInfo = iDialog->GetProgressInfoL();
        iCurrentItem++;
        //progressInfo->SetAndDraw(iCurrentItem);
        CompleteRequest();
        }
    }

// ----------------------------------------------------------------------------
// CWPSaver::RunError
// ----------------------------------------------------------------------------
//
TInt CWPSaver::RunError( TInt aError )
    {
    // There was a leave in RunL(). Store the error and
    // stop the dialog.
    iResult = aError;
    iWait.AsyncStop();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CWPSaver::CompleteRequest
// ----------------------------------------------------------------------------
//
void CWPSaver::CompleteRequest()
    {
    // Schedule an immediate complete. Make sure that there
    // is no timer alive first
    Cancel();
    //iRetryTimer->Cancel();

    SetActive();
    TRequestStatus* sp = &iStatus;
    User::RequestComplete( sp, KErrNone );
    }

