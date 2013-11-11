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
#include <hal.h>
#include <apgcli.h>
#include <s32file.h>
#include <AiwGenericParam.h>
#include <e32property.h>
#include <featmgr.h>
#include <caf/content.h>

#include "cimageprintappui.h"
#include "imageprint.h"
#include "cimageprintsettingsview.h"
#include "cimgpviewemptyview.h"
#include "cdiscoverydlgmanager.h"
#include "cimageprintdoc.h"
#include "printapputil.h"
#include "imageprintapp.hrh"
#include "mprintsettings.h"
#include "cimgpprintdlgmanager.h"
#include "clog.h"
#include "cimageprintengine.h"
#include "ciffactory.h"
#include "mdiscovery.h"
#include "cimgpprintutils.h"
#include "uimodefactory.h"
#include "muimode.h"
#include "muimodeobserver.h"
#include "caiwprintpreviewprovider.h"

using namespace ContentAccess;

_LIT( KEngineRsc, "imageprintengine.rsc" );
_LIT8( KJpegFileType, "image/jpeg" );

// For navipane changes
_LIT( KLine, " - " );
const TInt KLineLength(3);
const TInt32 KUid (0x101FFA84);

// Second class constructor
void CImagePrintAppUi::ConstructL()
    {
	LOG("CImagePrintAppUi::ConstructL BEGIN");
	// Add EAknSingleClickCompatible to BaseConstructL function parameter for single click event 
    BaseConstructL( EAknEnableSkin | EAppOrientationAutomatic | EAknEnableMSK | EAknSingleClickCompatible );	
  	
    FeatureManager::InitializeLibL();

	// get the best matching resource for the current locale
	TFileName file(NULL);	

	file.Append( KDC_RESOURCE_FILES_DIR );
	file.Append( KEngineRsc );
	BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), file );
    iResourceOffset = iEikonEnv->AddResourceFileL( file );

    iDoc = static_cast<CImagePrintDoc*>( CAknAppUi::Document() );

	TRAPD( err, iDoc->InitializeEngineL() );
	if ( err == KErrInUse )
		{
		LOG("Too many connections. Exiting...");
		CAknErrorNote* errornote = new (ELeave) CAknErrorNote( ETrue );
		HBufC* buf = StringLoader::LoadLC( R_QTN_PRINT_CONCURRENT_NOTE );
		errornote->ExecuteLD( *buf );
		CleanupStack::PopAndDestroy( buf );
		Exit();
		}
	else
		{
		User::LeaveIfError( err );
		}

    if ( iDoc->Engine()->InterfaceFactory().IsApplicationAlreadyInUse() )
    	{
    	LOG("Too many connections. Exiting...");
		CAknErrorNote* errornote = new (ELeave) CAknErrorNote( ETrue );
		HBufC* buf = StringLoader::LoadLC( R_QTN_PRINT_CONCURRENT_NOTE );
		errornote->ExecuteLD( *buf );
		CleanupStack::PopAndDestroy( buf );
		Exit();
    	}

    iAiwState = ENotAiwCall;
    TInt r = iAiwCall.Attach(KPrintPropertyCat,EPrintProperty,EOwnerProcess);
    if (r == KErrNone)
        {
        iAiwCall.Get(KPrintPropertyCat,EPrintProperty,iAiwState);
        }

	TBool aiw = EFalse;
	if ( iAiwState == EAiwPrintCall )
        {
		// Set the aiw call RProperty key back to default value so that
		// if application for some reason crashes the value is not left for AIW mode.
		iAiwCall.Set( KPrintPropertyCat,EPrintProperty, ENotAiwCall );
        SetImageArrayL(GetImagesFromAiwProviderL());
        aiw = ETrue;
        }


    iUIMode = UIModeFactory::CreateL( aiw, this, iDoc->Engine()->InterfaceFactory() );
    
	LOG("CImagePrintAppUi::ConstructL CImagePrintSettingsView");
    CImagePrintSettingsView* settingsView = CImagePrintSettingsView::NewL(
        iDoc->PrintSettings(), iDoc->PrintJob(), this, iUIMode );
    CleanupStack::PushL( settingsView );
	AddViewL( settingsView );
    CleanupStack::Pop( settingsView );

	LOG("CImagePrintAppUi::ConstructL CIMGPViewEmptyView");
    CIMGPViewEmptyView* emptyView = CIMGPViewEmptyView::NewL();
    CleanupStack::PushL( emptyView );
	AddViewL( emptyView );
    CleanupStack::Pop( emptyView);

    SetDefaultViewL( *emptyView );
    // default printer search in aiw mode must be done via
    // CIdle callback for UI to construct itself properly
    iIdleSearch = CIdle::NewL( CActive::EPriorityStandard );
	iIdleSearch->Start( TCallBack( SearchDefaultPrinterCbL, this ));

    // Default value for printer UID before printer is selected.
    iPrinterUID = -1;

    iCoeEnv->RootWin().EnableScreenChangeEvents();

    iApplicationStarting = ETrue;
    iAlbumCall = KErrNotFound;
	LOG("CImagePrintAppUi::ConstructL END");
    }

// aiw search callback
TInt CImagePrintAppUi::SearchDefaultPrinterCbL( TAny* aObj )
	{
	LOG("CImagePrintAppUi::SearchDefaultPrinterCbL begin");
	CImagePrintAppUi* obj = static_cast<CImagePrintAppUi*>( aObj );
	TRAPD( err, obj->iUIMode->SearchDefaultPrinterL() );
	if ( err != KErrNone )
		{
		LOG1( "CImagePrintAppUi::SearchDefaultPrinterCbL search failed, err: %d", err );
		
		if ( err == KErrDisconnected )
		    {
			HBufC* buf = StringLoader::LoadLC( R_NOTE_DISCONNECT_PRINT_ERROR );
    		CAknErrorNote* errornote = new CAknErrorNote( ETrue );       
    		errornote->ExecuteLD( *buf );	        	
    		CleanupStack::PopAndDestroy( buf );
		    }

        // Closes program
		obj->Exit();
		}
	LOG("CImagePrintAppUi::SearchDefaultPrinterCbL end");
	return EFalse;
	}

// Destructor
CImagePrintAppUi::~CImagePrintAppUi()
    {
	FeatureManager::UnInitializeLib();
    delete iNaviDecorator;

    iEikonEnv->DeleteResourceFile( iResourceOffset );
    delete iPrintDlgManager;
    delete iUSBConnectionWaitNote;

	if( iIdleSearch )
		{
		iIdleSearch->Cancel();
		}
	
	delete iUIMode;
	delete iIdleSearch;

    iAiwCall.Close();
    }

// Handles global user events
void CImagePrintAppUi::HandleCommandL(
    TInt aCommand )
    {
    LOG1( "CImagePrintAppUi::HandleCommandL BEGIN: %d", aCommand );
    switch ( aCommand )
        {
        case EAknCmdExit:
        case EAknSoftkeyExit:
            {
            Exit();
            break;
            }
        case EEikCmdExit:
        	{
        	// Cancel current printjob because this command may have come from MMC removal
        	// cancelling may fail in engine if mmc writing was in critical point, so TRAP
        	TRAP_IGNORE( iDoc->PrintJob()->CancelL() );
        	if ( iAiwState == EAiwPrintCall )
        		{
	            User::Exit( EAiwPrintCall );
        		}
        	else
        		{
        		Exit();
        		}
            break;
        	}

        case EAknSoftkeyBack:
            {       
            break;
            }

        case ESelectMarked:
            {
			if( iValidImages )
            	{
				iUIMode->SearchDefaultPrinterL();
				}
			else
				{
           		CAknErrorNote* errornote = new (ELeave) CAknErrorNote( ETrue );
				HBufC* buf = StringLoader::LoadLC( R_QTN_PRINT_SELECTNEW_NOTE );
				errornote->ExecuteLD( *buf );
				CleanupStack::PopAndDestroy( buf );
				}
			break;
            }
		default:
            break;
        }
	LOG( "CImagePrintAppUi::HandleCommandL END" );        
    }

void CImagePrintAppUi::LaunchHelpApplicationL( const TDesC& aHelpText )
    {
    CArrayFix< TCoeHelpContext >* cntx = new( ELeave ) CArrayFixFlat< TCoeHelpContext >( 1 );
    CleanupStack::PushL( cntx );

    //make uid
    TUid uid;
    uid.iUid = ( KUid );

    cntx->AppendL( TCoeHelpContext( uid, aHelpText ) );
    CleanupStack::Pop( cntx );

    //and launch help - takes ownership of context array
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), cntx );
    }

// Set the image array
void CImagePrintAppUi::SetImageArrayL( CDesCArrayFlat* aFileArray )
    {
    iFiles = aFileArray;

    // Sets default navipane text with no copies.     
    SetNavipaneTextL( 1 );            
        
    iDoc->SetImageArrayL( aFileArray );
    }

// Get the image array
CDesCArrayFlat* CImagePrintAppUi::GetImageArray()
    {
    return iFiles;
    }

// Get the document class
CImagePrintDoc* CImagePrintAppUi::GetDocument()
    {
    return iDoc;
    }

TInt CImagePrintAppUi::GetAlbumCall()
    {
    return iAlbumCall;
    }

void CImagePrintAppUi::SetAlbumCall(TInt aAlbumCall)
    {
    iAlbumCall = aAlbumCall;
    }

void CImagePrintAppUi::SetValidImagesStatus( TBool aStatus )
    {
    iValidImages = aStatus;
    }

void CImagePrintAppUi::ChangeViewL( TInt aView )
    {
    switch( aView )
        {	
        case MUIModeObserver::SettingsView:
            {
            ActivateLocalViewL( KImageSettingsViewId );
            break;
            }
        }
    }

void CImagePrintAppUi::ExitApplication()
    {
    Exit();
    }


TInt CImagePrintAppUi::GetAiwState()
    {
    return iAiwState;
    }

TBool CImagePrintAppUi::AiwState()
    {
    TBool retValue = EFalse;
    if( iAiwState == EAiwPrintCall )
        {
        retValue = ETrue;
        }
    return retValue;
    }


// Files from AIW customer
CDesCArrayFlat* CImagePrintAppUi::GetImagesFromAiwProviderL()
    {
	LOG("TBool CImagePrintAppUi::GetImagesFromAiwProvider() BEGIN");

    RFs fileServer;
    User::LeaveIfError(fileServer.Connect());
    CleanupClosePushL( fileServer );
    RFileReadStream readStream;
    
    TFileName paramFile;
    CIMGPPrintUtils::GetDataFilePathL( paramFile );
    
    User::LeaveIfError( readStream.Open(fileServer, paramFile, EFileRead) );
    readStream.PushL();
    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC(readStream);

    CDesCArrayFlat* imageFiles = new (ELeave) CDesCArrayFlat(5);

	TBool noOtherFileTypes( EFalse );
    TBool showFilesremovedNote ( EFalse );

    TInt index( 0 );
	const TAiwGenericParam* param = paramList->FindFirst(index,
			EGenericParamFile,
            EVariantTypeDesC);
	while ( index != KErrNotFound )
		{
		TFileName filename( param->Value().AsDes() );
		// CheckMIMEType needs to be changed so that we can distinguish between
		// a) file not found b) file was incorrect MIME-type
		noOtherFileTypes = CIMGPPrintUtils::CheckMIMEType( KJpegFileType, filename );
		if ( noOtherFileTypes )
			{
			// check drm protection
			CContent* content = NULL;
			content = CContent::NewLC( filename );

			TInt isDrmProtected( 0 );
			TInt err = content->GetAttribute( EIsProtected, isDrmProtected );
			CleanupStack::PopAndDestroy(); // content
   		if ( err || isDrmProtected )
    			{
    			LOG("CImagePrintAppUi::GetImagesFromAiwProvider(): found drm file!");
    			noOtherFileTypes = EFalse;
    			}
			}

		if ( noOtherFileTypes )
			{
			imageFiles->AppendL( filename );
			}
        else
            {
            showFilesremovedNote = ETrue;
		    }

		param = paramList->FindNext(index,
			EGenericParamFile,
            EVariantTypeDesC);
		}

    if ( showFilesremovedNote )
        {
        CAknErrorNote* errornote = new (ELeave) CAknErrorNote( ETrue );
		    HBufC* buf = StringLoader::LoadLC( R_QTN_PRINT_WRONGTYPE_NOTE );
		    errornote->ExecuteLD( *buf );
		    CleanupStack::PopAndDestroy( buf );
        }
    
    // If only one image is selected to print and it's corrupted, exit from application.
    if ( showFilesremovedNote && ( imageFiles->Count() == KErrNone ))
    	{
  		CAknErrorNote* errornote = new (ELeave) CAknErrorNote( ETrue );
		HBufC* buf = StringLoader::LoadLC( R_QTN_PRINT_SELECTNEW_NOTE );
		errornote->ExecuteLD( *buf );
		CleanupStack::PopAndDestroy( buf );
		Exit();
    	}

    CleanupStack::PopAndDestroy(paramList); //paramList

    LOG("TBool CImagePrintAppUi::GetImagesFromAiwProvider() END");

    readStream.Close();
    readStream.Pop();
    CleanupStack::PopAndDestroy( &fileServer );
    return imageFiles;
    }

TBool CImagePrintAppUi::OrientationPortrait()
    {
    TBool retValue = ETrue;
    TSize screenSize = CCoeEnv::Static()->ScreenDevice()->SizeInPixels();
    if( screenSize.iWidth == 416 ||
    	screenSize.iWidth == 320 )
        {
        retValue = EFalse;
        }
    return retValue;
    }

TBool CImagePrintAppUi::QVGAMode()
    {
    TBool retValue = EFalse;
    TSize screenSize = CCoeEnv::Static()->ScreenDevice()->SizeInPixels();
    if( screenSize.iWidth == 240 ||
    	screenSize.iWidth == 320 )
        {
        retValue = ETrue;
        }
    return retValue;
    }

// Sets NaviPane text
void CImagePrintAppUi::SetNavipaneTextL( const TInt aCopiesOfImage, HBufC* aPaperSize )
	{	
	// Navipane image count will be sum of all images on current print job.
	// E.g. If 2 images selected and copies value is 2, navi value will be: 2 x 2 = 4.
	TInt printCount = aCopiesOfImage * ( iFiles->MdcaCount() );
    
    // Buffer to store image or images -string.
	HBufC* naviText;
    
    // If only one image to print navipane text will be: Image
    if ( printCount == 1 )
    	{    	
    	naviText = StringLoader::LoadLC( R_PRINTJOB_IMAGE );
		}
    // If more than one image to print navipane text will be: Images
	else 
		{
		naviText = StringLoader::LoadLC( R_PRINTJOB_IMAGES, printCount );
		}  
	
    TInt strLength = naviText->Length();
    
    CEikStatusPane* sp =
        iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* np =
        static_cast<CAknNavigationControlContainer*>(
            sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    delete iNaviDecorator;
    iNaviDecorator = NULL;

    // If paper size needs to be added to the NaviPane, more space is needed
    if( aPaperSize )
        {
        strLength += aPaperSize->Length() + KLineLength;
        }
    // Clear the printer name string from all formatting tabs & numbers
	HBufC* stringholder = HBufC::NewLC( strLength );
	*stringholder = *naviText;
	TPtr ptr = stringholder->Des();
	TInt pos = stringholder->Find( KTabChar );
	if ( pos == 1 ) // 1st possible icon formatting tab is at pos 1
		{
		ptr.Delete( 0, 2 ); // delete the first icon tab and the number previous to it
		pos = stringholder->Find( KTabChar );
		}
	// 2nd possible iconformatting tab is 2nd last char (len - 2), don't delete it here  		
	while ( pos != KErrNotFound &&
		    pos != stringholder->Length() - 2 )
		{
		ptr.Delete( pos, 1); // delete all middle tabs
		pos = stringholder->Find( KTabChar );
		}
	// 2nd possible iconformatting tab is 2nd last char
	if ( pos != KErrNotFound &&
		 pos == stringholder->Length() - 2 )
		{
		// delete the 2nd icon formatting tab and the number next to it
		ptr.Delete( stringholder->Length() - 2, 2 );
		}

    // Add paper size and '-', in the beginning of the text
    if( aPaperSize )
	    {
        TPtr ptr2 = aPaperSize->Des();
        ptr.Insert(0, KLine);
        ptr.Insert(0, ptr2);
	    }

    iNaviDecorator = np->CreateNavigationLabelL( *stringholder );
    CleanupStack::PopAndDestroy( stringholder );
    np->PushL( *iNaviDecorator );
    sp->DrawNow();
    
    CleanupStack::PopAndDestroy( naviText );
    }

TUid CImagePrintAppUi::GetSettingsViewUid()
    {
    return KImageSettingsViewId;
    }

// End of File
