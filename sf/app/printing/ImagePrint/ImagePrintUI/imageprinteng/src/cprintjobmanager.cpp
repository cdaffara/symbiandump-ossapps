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


#include <imageprintapp.rsg>
#include <StringLoader.h>
#include <aknnotewrappers.h>

#include "cprintjobmanager.h"
#include "cprinteventcatcher.h"
#include "csettingsmanager.h"
#include "mprintjobobserver.h"
#include "imageprint.h"
#include "cimageprintengine.h"
#include "crealfactory.h"
#include "clog.h"
#include "printcapabilitycodes.h"
#include "printmessagecodes.h"
#include "mprintsettings.h"
#include "cimageprint.h"
#include "mdiscoveryobserver.h"

const TInt KCancelRetries( 2 );
// CONSTRUCTION
CPrintJobManager* CPrintJobManager::NewL(
    CRealFactory* aFactory,
    CImagePrintEngine* aDLLEngine )
    {
    CPrintJobManager* self = CPrintJobManager::NewLC(
        aFactory, aDLLEngine );
    CleanupStack::Pop();    // self

    return self;
    }

CPrintJobManager* CPrintJobManager::NewLC(
    CRealFactory* aFactory,
    CImagePrintEngine* aDLLEngine )
    {
    CPrintJobManager* self = new ( ELeave ) CPrintJobManager(
        aFactory, aDLLEngine );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// Constructor
CPrintJobManager::CPrintJobManager(
    CRealFactory* aFactory,
    CImagePrintEngine* aDLLEngine ) :
      iFactory( aFactory ),
      iDLLEngine( aDLLEngine )
    {
    }

// Destructor
CPrintJobManager::~CPrintJobManager()
    {
    LOG("CPrintJobManager::~CPrintJobManager BEGIN");

    iImages.ResetAndDestroy();
    iImages.Close();
    
    // Cancel current print job at exit
    if(IsPrinting())
    {		
    	TRAP_IGNORE( CancelL() );    	
    }

    LOG("CPrintJobManager::~CPrintJobManager END");
    }

// Second phase constructor
void CPrintJobManager::ConstructL()
    {
    }

// Creates a new print job
TInt CPrintJobManager::CreatePrintJobL(
    TInt aPrinterId )
    {
	LOG("CPrintJobManager::CreatePrintJobL BEGIN");
	iPrintingOnGoing = EFalse;
    // Creates a print job
    MPrintEventObserver* notifier = iFactory->PrintEventObserverIF();
    iPrinterUid = aPrinterId;

    TInt printJobError = iFactory->Engine()->CreatePrintJobL(
        aPrinterId, iDLLEngine->FileArray(), *notifier );
        
   iImages.ResetAndDestroy();
   
   for ( TInt i(0) ; i < iDLLEngine->FileArray().Count() ; i++ )
        {
        HBufC* image = iDLLEngine->FileArray()[i]->AllocLC();
        iImages.AppendL( image );
        CleanupStack::Pop( image );
        }

	LOG1( "CPrintJobManager::CreatePrintJobL END, err: %d", printJobError );
    return printJobError;
    }

// Submits the created print job
void CPrintJobManager::PrintL(
    MPrintJobObserver* aObserver )
    {
	LOG("CPrintJobManager::PrintL BEGIN");
    TInt getSettings;
    iPrintingOnGoing = ETrue;

    //Get current template UID from settings manager
    TInt uid = iFactory->SettingsIF()->TemplateUid();

    LOG1("Print done with id: %d", uid);

    //Set the UID to Image Print Engine.
    TInt err = iFactory->Engine()->SetJobSettingL(
               EPrintCapabLayout, uid, getSettings );
	LOG1("CPrintJobManager::PrintL SetJobSettingL err: %d", err);

	if ( err == KErrNotFound )
		{
		HBufC* buf = StringLoader::LoadLC( R_QTN_PRINT_NOT_FOUND_ERROR );
		CAknErrorNote* errornote = new (ELeave) CAknErrorNote( ETrue );
		CleanupStack::PushL( errornote );
		errornote->ExecuteLD( *buf );
		CleanupStack::PopAndDestroy( errornote );
		CleanupStack::PopAndDestroy( buf );
		}
	if ( err != KErrNone )
		{
		iPrintingOnGoing = EFalse;
	   	User::Leave( err );
		}
    err = iFactory->Engine()->SubmitPrintJobL();
	LOG1("CPrintJobManager::PrintL SubmitPrintJobL err: %d", err);
	if ( err == KErrNotFound )
		{
		CAknErrorNote* errornote = new (ELeave) CAknErrorNote( ETrue );
		HBufC* buf = StringLoader::LoadLC( R_QTN_PRINT_NOT_FOUND_ERROR );
		errornote->ExecuteLD( *buf );
		CleanupStack::PopAndDestroy( buf );
		}
	if ( err != KErrNone )
		{
		iPrintingOnGoing = EFalse;
	   	User::Leave( err );
		}
    iObserver = aObserver;
	LOG("CPrintJobManager::PrintL END");
    }

// Cancels printing
void CPrintJobManager::CancelL()
    {
    LOG("CPrintJobManager::CancelL BEGIN");
    if ( IsPrinting() )
    	{
	    iCancelling = ETrue;
	    TInt err = iFactory->Engine()->CancelPrintJob();
	    if ( err )
	        {
	        LOG1("CPrintJobManager::CancelL failed err: %d", err);
	        if ( err == EPbStatusErrorReasonHardwarePrinterUnavailable )
	        	{
	        	//Printer was not ready, retry after a while a couple of times
	        	for( TInt i=0; i<KCancelRetries; i++ )
	        		{
	        		// If cancellation doesn't work as expected. Add here Periodic timer.
	        		err = iFactory->Engine()->CancelPrintJob();
	        		if ( err == KErrNone )
	        			{
	        			break;
	        			}
	        		else
	        			{
	        			LOG1("CPrintJobManager::CancelL failed err: %d", err);
	        			}
	        		}
	        	}
	        else
	        	{
		        iCancelling = EFalse;
		        User::LeaveIfError( err );	        	
	        	}
	        }
		// Cancellation is returned to UI after PrintJobProgress returns completion status
	    iPrintingOnGoing = EFalse;
	    }
	   else
	   	{
	   	if(iObserver)
	   		{
	   		iObserver->JobFinished(); 
	   		}
	   	}
	    
	LOG("CPrintJobManager::CancelL END");
    }
    
// Returns the images from current print job.
void CPrintJobManager::GetPrintJobL( RPointerArray<TDesC>& aImages )
    {
    for ( TInt i(0); i < iImages.Count(); i++ )
        {
        aImages.AppendL( iImages[i] );
        }
    }

// Returns the status of the printing
TBool CPrintJobManager::IsPrinting() const
    {
    return iPrintingOnGoing;
    }

// Called by engine for progress notifications
void CPrintJobManager::PrintJobProgress(
    TInt aStatus,
    TInt aPercentCompletion,
    TInt aJobStateCode )
    {
    if ( aStatus == EActive )
        {
        iPrintingOnGoing = ETrue;  
        iObserver->PrintProgress( aPercentCompletion );
        }
    else
        {
        if ( aJobStateCode == ECancelling && aStatus == EDone )
        	{
        	iCancelling = EFalse;
        	iPrintingOnGoing = EFalse;
        	iObserver->JobError( aJobStateCode, KErrNone );
        	}
        else if ( iCancelling && aStatus == EDone)
			{
	        iCancelling = EFalse;
			iObserver->JobFinished();
	        }
        else if ( aJobStateCode == ECancellingNoMessage && aStatus == EDone )
			{
	  	    iObserver->JobError( aJobStateCode, KErrNone );
	        }
	    else
	        {
	        iObserver->JobFinished();
	        iPrintingOnGoing = EFalse;
	        }
        }
    }

// Called by engine to notify print errors
void CPrintJobManager::PrintJobError(
    TInt aError, TInt aErrorStringCode )
	{
	LOG2("[CPrintJobManager::PrintJobError] PrintJobError: %d, errorstring: %d", aError, aErrorStringCode);
	if( iPrintingOnGoing )
		{
		if ( iObserver )
			{        
			LOG("CPrintJobManager::PrintJobError Printing is going, send error to UI.");
			iObserver->JobError( aError, aErrorStringCode );
			}    	
		}
	// ERKZ-7JDFZ8 - Canceling Job hangs for ever
	// If WLAN printer is turned off while printing UI receives KErrHostUnreach. 
	// If error is not handled, printing continues. 
	if ( aError == KErrHostUnreach )
		{
		if ( iObserver )
			{ 
			LOG("CPrintJobManager::PrintJobError KErrHostUnreach");
			iObserver->JobError( KErrHostUnreach, aErrorStringCode );
			}
		}
	iPrintingOnGoing = EFalse;
	// show Pictbridge fatal errors and warnings always
	if ( iFactory )
		{
		if( iFactory->SettingsIF()->GetCurrentPrinterProtocol() == MDiscoveryObserver::EUSB )
			{
			if ( iObserver )
				{ 
				LOG("CPrintJobManager::PrintJobError sending PictBridge error to UI...");
				iObserver->JobError( aError, aErrorStringCode );
				}
			}
		}
	}

// Called by engine to notify printer status (errors)
void CPrintJobManager::PrinterStatus(
    TInt aError,
    TInt aErrorStringCode )
    {
	iObserver->JobStatusEvent( aError, aErrorStringCode );
    }

//  End of File
