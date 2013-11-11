/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <StringLoader.h>
#include <AknWaitDialog.h>
#include <aknstaticnotedialog.h>
#include <eikprogi.h>
#include <aknnotewrappers.h>
#include <coecntrl.h>
#include <eikmenup.h>
#include <bautils.h>
#include <bluetooth/hci/hcierrors.h>
#include <f32file.h>
#include <imageprintapp.rsg>

#include "cimgpprintdlgmanager.h"
#include "imgpprintapputil.h"
#include "cimgpprintprogressdlg.h"
#include "clog.h"
#include "cimgpprintutils.h"
#include "mprintingobserver.h"
#include "tprinter.h"
#include "printmessagecodes.h"
#include "imageprintconsts.h"

_LIT(KPlain, "%S");

const TUint KProgressFinalValue = 100;

// Timeout for MMC InformationNote ( timeout = 5 000 000 ms -> 5 s )
const TInt KTimeOut = 5000000;  


// CONSTRUCTION
EXPORT_C CIMGPPrintDlgManager* CIMGPPrintDlgManager::NewL(
	TInt aNoc,
    MPrintJob* aPrintJob,
    TUint aNumOfPages,
    TBool aMMCPrinting,
    TUint aVendor,
    CIMGPPrintUtils* aPrintUtils,
    MPrintingObserver* aPrintingObserver )
    {
    CIMGPPrintDlgManager* self =
        CIMGPPrintDlgManager::NewLC( aNoc,
        							 aPrintJob,
                                     aNumOfPages,
                                     aMMCPrinting,
                                     aVendor,
                                     aPrintUtils,
                                     aPrintingObserver );
    CleanupStack::Pop( self );
    return self;
    }

CIMGPPrintDlgManager* CIMGPPrintDlgManager::NewLC(
	TInt aNoc,
    MPrintJob* aPrintJob,
    TUint aNumOfPages,
    TBool aMMCPrinting,
    TUint aVendor,
    CIMGPPrintUtils* aPrintUtils,
    MPrintingObserver* aPrintingObserver )
    {
    CIMGPPrintDlgManager* self =
        new ( ELeave ) CIMGPPrintDlgManager( aNoc, 
        								 aPrintJob,
                                         aNumOfPages,
                                         aMMCPrinting,
                                         aVendor,
                                         aPrintUtils,
                                         aPrintingObserver );
    self->ConstructL();
    CleanupStack::PushL( self );

    return self;
    }


// Second class constructor
void CIMGPPrintDlgManager::ConstructL()
    {
 
    }

// Default constructor
CIMGPPrintDlgManager::CIMGPPrintDlgManager(
	TInt aNoc,
    MPrintJob* aPrintJob,
    TUint aNumOfPages,
    TBool aMMCPrinting,
    TUint aVendor,
    CIMGPPrintUtils* aPrintUtils,
    MPrintingObserver* aPrintingObserver ) :
    iPrintJob( aPrintJob ),
    iNumOfPages( aNumOfPages ),
    iMMCPrinting( aMMCPrinting ),
    iVendor( aVendor )
    {
    iCancellingDialog = 0;
    // Used only in aiw printing
    if( aPrintUtils )
        {
        iPrintUtils = aPrintUtils;
        }
	if( aPrintingObserver )
        {
        iPrintingObserver = aPrintingObserver;
        }
	iNoc = aNoc;
    }

// Destructor
CIMGPPrintDlgManager::~CIMGPPrintDlgManager()
    {
	LOG("CIMGPPrintDlgManager::~CIMGPPrintDlgManager BEGIN");
    if ( iDialog )
        {
        delete iDialog;
        iDialog = 0;
        }
	LOG("CIMGPPrintDlgManager::~CIMGPPrintDlgManager END");
    }

// Starts the print job
EXPORT_C void CIMGPPrintDlgManager::StartPrintingL()
    {
	LOG("CIMGPPrintDlgManager::StartPrintingL BEGIN");
    
	iFirstNote = ETrue;
	
    TRAPD( err, iPrintJob->PrintL( this ));
    if ( err == KErrNone )
    	{
    	LOG("CIMGPPrintDlgManager::StartPrintingL printing...");
    	PrepareProgressDialogL();
    	iCancelling = EFalse;
    	iPrintingCancelled = EFalse;
    	iDialog->RunLD();
    	}
	else
		{
		LOG("CIMGPPrintDlgManager::StartPrintingL failed");
		if ( iDialog )
        	{
        	delete iDialog;
        	iDialog = 0;
        	}
    	if( iPrintUtils )
    	    {
    	    iPrintUtils->JobFinished();
    	    }
		if( iPrintingObserver )
			{
			TRAP_IGNORE( iPrintingObserver->PrintingCompletedL( EFalse ));
			}
		// Don't leave for KErrNotFound, as there's another note for it
		if ( err != KErrNotFound )
			{
			TRAP_IGNORE( IMGPPrintAppUtil::ShowErrorMsgL( err ));
			}
		}
	LOG("CIMGPPrintDlgManager::StartPrintingL END");
    }

void CIMGPPrintDlgManager::PrepareProgressDialogL()
	{
	LOG("CIMGPPrintDlgManager::PrepareProgressDialogL START");
	HBufC* txt;
    if ( !iMMCPrinting )
        {
        txt = StringLoader::LoadLC(
            R_QTN_PRINT_PROGRESS_NOTE_TITLE );
        }
    else
        {
        txt = StringLoader::LoadLC(
            R_QTN_COPY_PROGRESS_NOTE_TITLE);
        }
  
	TBuf<128> buf;
	buf.Format( KPlain, txt );
	CleanupStack::PopAndDestroy( txt );

    iDialog = new ( ELeave ) CAknProgressDialog(
   	    (REINTERPRET_CAST( CEikDialog**, &iDialog )), EFalse );
	iDialog->PrepareLC( R_PRINT_PROGRESS_PLAIN_PROG_NOTE );
	
    CEikProgressInfo* info = iDialog->GetProgressInfoL();
    info->SetFinalValue( KProgressFinalValue );
    iDialog->SetTextL( buf );
    iDialog->SetCallback( this );
    LOG("CIMGPPrintDlgManager::PrepareProgressDialogL END");
	}


// Called to update the print progress
void CIMGPPrintDlgManager::PrintProgress(
    TUint aComplete )
    {
    TInt err = KErrNone;
    TRAP( err, PrintProgressL( aComplete ) );
    if ( err != KErrNone )
        {
        TRAP_IGNORE( IMGPPrintAppUtil::ShowErrorMsgL( err ) );
        }
    }

// Called to update the print progress
void CIMGPPrintDlgManager::PrintProgressL(
    TUint aComplete )
    {
    LOG1( "CIMGPPrintDlgManager::PrintProgressL START with: %d", aComplete );
	if( !iCancelling )
		{
		if ( !iDialog )
        	{
        	LOG("CIMGPPrintDlgManager::PrintProgressL: creating dialog again");
			PrepareProgressDialogL();
    		iDialog->RunLD();
        	}
    	CEikProgressInfo* info = iDialog->GetProgressInfoL();
    	info->SetAndDraw( aComplete );
		}
    LOG("CIMGPPrintDlgManager::PrintProgressL END");
    }

// Called when the print job is finished
void CIMGPPrintDlgManager::JobFinished()
    {
			
	LOG("CIMGPPrintDlgManager::JobFinished BEGIN");
    if ( iDialog )
        {
        delete iDialog;
        iDialog = 0;
        }    
    if ( iMMCPrinting && !iCancelling )
        {
		LOG("CIMGPPrintDlgManager::JobFinished mmc note");
        TRAP_IGNORE( ShowMMCPrintingCompletedDlgL() );
 		LOG("CIMGPPrintDlgManager::JobFinished mmc note done");
        }
    if ( iCancelling )
		{
		if(iCancellingDialog)
			{
		TRAP_IGNORE( iCancellingDialog->ProcessFinishedL() );
		iCancellingDialog = 0;
		iCancelling = EFalse;
			}
		}
	else
		{
	    if( iPrintUtils )
	        {
			LOG("CIMGPPrintDlgManager::JobFinished iPrintUtils");
	        iPrintUtils->JobFinished();
			LOG("CIMGPPrintDlgManager::JobFinished iPrintUtils done");
	        }
		if( iPrintingObserver )
			{
			TRAP_IGNORE( iPrintingObserver->PrintingCompletedL( EFalse ));
			}
		}
	LOG("CIMGPPrintDlgManager::JobFinished END");
    }

// Manages print job errors
void CIMGPPrintDlgManager::JobError(
    TInt aErrCode, TInt aErrorStringCode )
    {
    LOG2("CIMGPPrintDlgManager::JobError BEGIN, error = %d, stringcode = %d", aErrCode, aErrorStringCode );
    
    if( iMMCPrinting && aErrCode == ECancellingNoMessage ) 
        {
       	if ( iDialog )
    	    {
    	    delete iDialog;
    	    iDialog = 0;
    	    }
       return;
       }
    
    if ( iMMCPrinting && !iCancelling )
        {
        TRAP_IGNORE( IMGPPrintAppUtil::ShowErrorNoteL( R_NOTE_IMAGEPRINT_MMCCOPYFAIL ) );
        // Nothing to do if the displaying of error note fails
        // Just need to make sure that this method never leaves
        }
	if ( iCancelling )
		{
        LOG( "CIMGPPrintDlgManager::JobError ---> iCancellingDialog->ProcessFinishedL()" );
		TRAP_IGNORE( iCancellingDialog->ProcessFinishedL() );
		iCancellingDialog = 0;
		iCancelling = EFalse;
		}
	else
		{
    	// Delete dialog
    	if ( iDialog )
    	    {
    	    delete iDialog;
    	    iDialog = 0;
    	    }
    	if( iPrintUtils &&
		    aErrCode != KHCIErrorBase-EPageTimedOut &&
		    aErrorStringCode != EObexConnectError )
    	    {
    	    iPrintUtils->JobFinished();
    	    }
		if( iPrintingObserver )
			{
			TRAP_IGNORE( iPrintingObserver->PrintingCompletedL( EFalse ));
			}
		}

     /* Error note is shown to user in normal cases, but not when
      * printing is done to MMC, canceled, done via quickprint, or
      * BT timeout happens, as these cases are handled in other parts
      * of code
      */
    if ( !iMMCPrinting &&
         aErrCode != ECancelling &&
         ( !iPrintUtils ||
		   ( aErrCode != KHCIErrorBase-EPageTimedOut &&
		     aErrorStringCode != EObexConnectError )))
        {
        TRAP_IGNORE( IMGPPrintAppUtil::ShowErrorMsgL( aErrCode, aErrorStringCode ));
        // Nothing to do to handle err
        }
	LOG("CIMGPPrintDlgManager::JobError END");
    }

// Manages print job status events
void CIMGPPrintDlgManager::JobStatusEvent( TInt aErrCode, TInt aErrorStringCode )
    {
    LOG2("CIMGPPrintDlgManager::JobStatusEvent = %d, stringcode = %d", aErrCode, aErrorStringCode );
    // If printer is disconnected cancel printing else
    // just show the error message
    if ( aErrCode == KErrDisconnected )
        {
        TRAP_IGNORE( IMGPPrintAppUtil::ShowErrorMsgL( KErrDisconnected, aErrorStringCode ));
        if ( iDialog )
            {
            // This ends the printing
            delete iDialog;
            iDialog = 0;
            }
        }
    else
        {
        TRAP_IGNORE( IMGPPrintAppUtil::ShowErrorMsgL( aErrCode, aErrorStringCode ));
        }
	LOG("CIMGPPrintDlgManager::JobStatusEvent END");
    }

// Shows printing completed dlg
void CIMGPPrintDlgManager::ShowMMCPrintingCompletedDlgL()
    {
	LOG("CIMGPPrintDlgManager::ShowMMCPrintingCompletedDlgL BEGIN");
    
    RPointerArray<TDesC> allImages;
	CleanupClosePushL( allImages );
    iPrintJob->GetPrintJobL( allImages );
    TInt imagesCount = allImages.Count();
    CleanupStack::PopAndDestroy( &allImages );
    HBufC* buf = NULL;

    // Blocks possibility to show note when exiting image print. 
    if (iFirstNote)
    	{    
    	if ( iNoc == 1 && imagesCount == 1 )
	        {
	        buf = StringLoader::LoadLC( R_SETTINGS_IMAGEPRINT_MMC_PRINT_NOTE_ONE_PICTURE ); //r_settings_imageprint_mmc_print_note_one_picture
	        }
	    else
	        {
	        buf = StringLoader::LoadLC( R_SETTINGS_IMAGEPRINT_MMC_PRINT_NOTE );
	        }
	    	
	       	CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
		    dlg->SetTimeout( (CAknNoteDialog::TTimeout) KTimeOut );        
		    dlg->ExecuteLD( *buf );
		    CleanupStack::PopAndDestroy( buf );
		    
		iFirstNote = EFalse;    
	    
    	} 
	
	    LOG("CIMGPPrintDlgManager::ShowMMCPrintingCompletedDlgL END");
    }

// Called when dialog gets dismissed
void CIMGPPrintDlgManager::DialogDismissedL(
    TInt aButtonId )
    {
	LOG("CIMGPPrintDlgManager::DialogDismissedL BEGIN");
    // If user pressed cancel, cancel printing
    LOG1( "CIMGPPrintDlgManager::DialogDismissedL iPrintingCancelled: %d", iPrintingCancelled );
    if ( ( aButtonId == EAknSoftkeyCancel ) && ( iPrintingCancelled == EFalse ) )
        {
		LOG("CIMGPPrintDlgManager::DialogDismissedL aButtonId == EAknSoftkeyCancel");
        iDialog = 0;
		iCancelling = ETrue;
		
		// Printing cancelled
		iPrintingCancelled = ETrue;

        TRAPD( err, iPrintJob->CancelL() );
        if( err == KErrNone )
        	{
			ShowCancelWaitNoteL();
        	}
		else
			{
			iCancelling = EFalse;
			}        	
        }

	LOG("CIMGPPrintDlgManager::DialogDismissedL END");
    }

// Displays the cancel wait note
void CIMGPPrintDlgManager::ShowCancelWaitNoteL()
	{
	iCancellingDialog  =
		new ( ELeave ) CAknWaitDialog(
			(REINTERPRET_CAST(CEikDialog**,&iCancellingDialog)));
    iCancellingDialog->PrepareLC(R_CANCEL_PRINTING_DIALOG);
    HBufC* text = StringLoader::LoadLC( R_QTN_PRINT_PROGRESS_PROMPT_CANCEL );
    iCancellingDialog->SetTextL( *text );
    CleanupStack::PopAndDestroy(text);

    iCancellingDialog->SetTone( CAknNoteDialog::ENoTone );
    iCancellingDialog->RunLD();

    if( iPrintUtils )
        {
        iPrintUtils->JobFinished();
        }
	if( iPrintingObserver )
		{
		iPrintingObserver->PrintingCompletedL( ETrue );
		}
	}

//  End of File
