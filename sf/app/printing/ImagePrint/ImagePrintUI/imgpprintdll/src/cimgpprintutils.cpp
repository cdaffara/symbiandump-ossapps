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
#include <imageprintengine.rsg>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <aknstaticnotedialog.h>
#include <MGFetch.h>
#include <eikspane.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <imageconversion.h>
#include <StringLoader.h>
#include <avkon.hrh>
#include <bautils.h>
#include <hlplch.h>
#include <e32std.h>
#include <coehelp.h>
#include <s32file.h>
#include <AiwGenericParam.h>
#include <e32property.h>
#include <bluetooth/hci/hcierrors.h>
#include <pathinfo.h>
#include <driveinfo.h>

#include "cimgpprintutils.h"
#include "imageprint.h"
#include "cimageprintsettingsview.h"
#include "mdiscoveryobserver.h"
#include "cimageprintdoc.h"
#include "imgpprintapputil.h"
#include "imageprintapp.hrh"
#include "mprintsettings.h"
#include "cimgpprintdlgmanager.h"
#include "clog.h"
#include "cimageprintengine.h"
#include "ciffactory.h"
#include "cuiprotocolcontroller.h"
#include "imageprintconsts.h"
#include "printmessagecodes.h"
#include "mprinteventregisterif.h"

#ifndef __WINS__
_LIT( KEngineRsc, "imageprintengine.rsc" );
_LIT( KImagePrintAppRsc, "imageprintapp.rsc" );
#else
_LIT( KEngineRsc, "imageprintengine.rsc" );
_LIT( KImagePrintAppRsc, "imageprintapp.rsc" );
#endif

EXPORT_C CIMGPPrintUtils* CIMGPPrintUtils::NewL( MAiwObserver* aAiwObserver )
    {
    CIMGPPrintUtils* self = NewLC( aAiwObserver );
    CleanupStack::Pop(self);
    return self;
    }

CIMGPPrintUtils* CIMGPPrintUtils::NewLC( MAiwObserver* aAiwObserver )
    {
    CIMGPPrintUtils* self =
                new (ELeave) CIMGPPrintUtils;
    CleanupStack::PushL(self);
    self->ConstructL( aAiwObserver );
    return self;
    }
// Second class constructor
void CIMGPPrintUtils::ConstructL( MAiwObserver* aAiwObserver )
    {
	iAiwObserver = aAiwObserver;

#ifndef __WINS__
    TFileName file;
    TParse parse;
    parse.Set( file, NULL, NULL );
    file.Append( KDC_APP_RESOURCE_DIR );
    file.Append( KImagePrintAppRsc );
    TFileName file2;
    parse.Set( file2, NULL, NULL );
    file2.Append( KDC_RESOURCE_FILES_DIR );
    file2.Append( KEngineRsc );
#else
    TFileName file;
    file.Append( KDC_APP_RESOURCE_DIR );
    file.Append( KImagePrintAppRsc);
    TFileName file2;
    file2.Append( KDC_RESOURCE_FILES_DIR );
    file2.Append( KEngineRsc );
#endif
	// get the best matching resource for the current locale
	BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), file );
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL( file );

	// get the best matching resource for the current locale
	BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), file2 );
    iResourceOffset2 = CCoeEnv::Static()->AddResourceFileL( file2 );

	TRAPD( err, iEngine = CImagePrintEngine::NewL( 0 ));
	if ( err == KErrInUse )
		{
        HBufC* buf = StringLoader::LoadLC( R_QTN_PRINT_CONCURRENT_NOTE );
        CAknErrorNote* errornote = new (ELeave) CAknErrorNote( ETrue );
        errornote->ExecuteLD( *buf );
		CleanupStack::PopAndDestroy( buf );
		User::Leave( KErrInUse );
		}
	else
		{
		User::LeaveIfError( err );
		}

	iSettings = iEngine->InterfaceFactory().SettingsIF();

    if ( iEngine->InterfaceFactory().IsApplicationAlreadyInUse() )
		{
        HBufC* buf = StringLoader::LoadLC( R_QTN_PRINT_CONCURRENT_NOTE );
        CAknErrorNote* errornote = new (ELeave) CAknErrorNote( ETrue );
        errornote->ExecuteLD( *buf );
		CleanupStack::PopAndDestroy( buf );
		User::Leave( KErrInUse );
		}

    // Default value for printer UID before printer is selected.
    iPrinterUID = -1;

    iIdle = CIdle::NewL( CActive::EPriorityIdle );
    }

// Destructor
CIMGPPrintUtils::~CIMGPPrintUtils()
    {
    CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
    CCoeEnv::Static()->DeleteResourceFile( iResourceOffset2 );

	if ( iEngine )
		{
		iEngine->InterfaceFactory().PrintEventRegisterIF()->UnRegisterObserver( this );
		}

    if (iPrintDlgManager)
        {
        delete iPrintDlgManager;
        }
	delete iProtocolController;
    delete iEngine;
    delete iIdle;
    }

// Set the printer uid in the UI side
void CIMGPPrintUtils::SetPrinterUID( TInt aPrinterUID )
    {
    iPrinterUID = aPrinterUID;
    }

// Get the printer uid in the UI side
TInt CIMGPPrintUtils::GetPrinterUID()
    {
    return iPrinterUID;
    }

CImagePrintEngine& CIMGPPrintUtils::Engine() const
	{
	return *iEngine;
	}

EXPORT_C TBool CIMGPPrintUtils::CheckMIMEType( const TDesC8& aMimeType,
											   const TFileName& aFileName )
    {
    TBool retVal(ETrue);
    TBuf8<128> mimeType;
    mimeType.Zero();

    RFs fServer;
    TInt res = fServer.Connect();
    if ( res != KErrNone )
    	{
    	return EFalse;
    	}

    TRAPD( err, CImageDecoder::GetMimeTypeFileL(fServer, aFileName, mimeType) );
    fServer.Close();
    if ( err != KErrNone )
        {
        retVal = EFalse;
        }
    else if ( aMimeType.Length() > 0 )
        {
        // Check that MIME-type matches, otherwise don't use the image
        if ( mimeType.CompareF( aMimeType ) != 0 )
            {
            retVal = EFalse;
            }
        }
    return retVal;
    }


// Print
EXPORT_C void CIMGPPrintUtils::StartPrintingL( CDesCArrayFlat* aImages )
    {
    iEngine->InterfaceFactory().PrintEventRegisterIF()->RegisterObserver( this );
    iEngine->SetImageArrayL(aImages);
    iProtocolController = CUIProtocolController::NewL( this, iEngine->InterfaceFactory() );
    TBool printerFound = iProtocolController->SearchDefaultPrinterL();
    iSettings->UpdateNumberOfCopiesToServer();
    if( printerFound )
        {
        LOG( "CIMGPPrintUtils::StartPrintingL default printer found, printing" );
        PrintL();
        }
    else
        {
        LOG( "CIMGPPrintUtils::StartPrintingL default printer NOT found, leaving" );
        User::Leave( KErrCancel );
        }
    }

void CIMGPPrintUtils::PrintL()
    {
	LOG( "CIMGPPrintUtils::PrintL Start" );
    TInt prot = iSettings->GetCurrentPrinterProtocol();
    TUint vendor = iSettings->GetCurrentPrinterVendor();
    TBool mmc = EFalse;
    if( prot == MDiscoveryObserver::EMMC )
        {
        mmc = ETrue;
        }
    else
        {
        mmc = EFalse;
        }

    delete iPrintDlgManager;
    iPrintDlgManager = 0;
    iPrintDlgManager = CIMGPPrintDlgManager::NewL( 1,
    		iEngine->InterfaceFactory().PrintJobIF(), 1, mmc, vendor, this );
    iPrintDlgManager->StartPrintingL();
    LOG( "CIMGPPrintUtils::PrintL End" );
    }

void CIMGPPrintUtils::JobFinished()
    {
    iAiwObserver->JobFinished();
    }

void CIMGPPrintUtils::UsbCableDisconnected()
    {
    iAiwObserver->JobFinished();
    }

void CIMGPPrintUtils::RestartPrintingL()
	{
	TBool printerFound = iProtocolController->FullSearchL();
    if( printerFound )
        {
        LOG( "CIMGPPrintUtils::RestartPrinting: printer selected, printing" );
	    iSettings->UpdateNumberOfCopiesToServer();
        PrintL();
        }
    else
        {
        LOG( "CIMGPPrintUtils::RestartPrinting: printer not selected, leaving" );
        iAiwObserver->JobFinished();
        }
	}

void CIMGPPrintUtils::PrintJobProgress( TInt /*aStatus*/,
        					   TInt /*aPercentCompletion*/,
            				   TInt /*aJobStateCode*/ )
	{
	// No implementation
	}

void CIMGPPrintUtils::PrintJobError( TInt aError, TInt aErrorStringCode )
	{
	if ( aError == KHCIErrorBase-EPageTimedOut &&
		 aErrorStringCode == EObexConnectError )
		{
		//If full discovery is not wanted, uncomment next two lines for error msg & quit
		//TRAP_IGNORE( IMGPPrintAppUtil::ShowErrorMsgL( aError, aErrorStringCode ));
		iIdle->Cancel();
		iIdle->Start( TCallBack( IdleCallbackL, this ));
		}
	}

TInt CIMGPPrintUtils::IdleCallbackL( TAny* aObj )
	{
	CIMGPPrintUtils* obj = static_cast<CIMGPPrintUtils*>( aObj );
	obj->RestartPrintingL();
	return EFalse;
	}

void CIMGPPrintUtils::PrinterStatus( TInt /*aError*/, TInt /*aErrorStringCode*/ )
	{
	// No implementation
	}

EXPORT_C void CIMGPPrintUtils::GetDataFilePathL( TFileName& aFileName )
	{
	TInt phoneMemoryNumber( 0 );    
	User::LeaveIfError( DriveInfo::GetDefaultDrive(
			DriveInfo::EDefaultPhoneMemory, phoneMemoryNumber ) );

	TDriveUnit unit( phoneMemoryNumber );

	aFileName.Append( unit.Name() );
	aFileName.Append( KParamFile );
	}


//  End of File
