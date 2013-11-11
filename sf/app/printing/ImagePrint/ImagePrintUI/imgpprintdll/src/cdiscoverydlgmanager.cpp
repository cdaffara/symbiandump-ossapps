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
#include <aknlists.h>
#include <aknPopup.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknPopupHeadingPane.h>
#include <badesca.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <AknWaitDialog.h>
#include <AknIconUtils.h>
#include <AknGlobalMsgQuery.h>      // global message query
#include <apgcli.h>                 // rapalssession
#include <centralrepository.h>      // repository
#include <aknmessagequerydialog.h>
#include <bluetooth/hci/hcierrors.h>
#include <imgprintbitmaps.mbg>
#include <upnpsettings.h>
#include <upnprunsetupcommand.h>
#include <upnpshowcommand.h>

#include "cdiscoverydlgmanager.h"
#include "imageprint.h"
#include "mdiscovery.h"
#include "mprintsettings.h"
#include "ciffactory.h"
#include "mprintsettings.h"
#include "imgpprintapputil.h"
#include "cimageprintappui.h"
#include "imageprintconsts.h"
#include "cimageprintdoc.h"
#include "cimageprintengine.h"
#include "tprinter.h"
#include "clog.h"

const TInt KArrayGranularity( 5 );
const TInt KSearchAgainUID ( -1 );

_LIT( KEmptyIconIndex, "0" );
_LIT( KCachedIconIndex, "1" );
_LIT( KBTIconIndex, "2" );
_LIT( KKioskIconIndex, "3" );
_LIT( KMMCIconIndex, "4" );
_LIT( KUSBIconIndex, "5" );
_LIT( KIconFile, "z:\\resource\\apps\\imgprintbitmaps.mif" );

const TUint KSpaceForSingleIcon( 2 ); // icon index + tab = 2 chars
const TUint KSpaceForBothIcons( 4 ); // icon index + tab + cache index + tab = 4 chars

const TInt KNumberOfIcons( 7 );
_LIT( KWLANIconIndex, "6" );

void CleanUpResetAndDestroy(TAny* aArray)
	{
	if(aArray)
		{
		CArrayPtrFlat<TDesC>* array=(CArrayPtrFlat<TDesC>*)aArray;
		array->ResetAndDestroy();
		delete array;
		}
	}
// CONSTRUCTION
CDiscoveryDlgManager* CDiscoveryDlgManager::NewL(
		CIFFactory& aIFFactory )
	{
	CDiscoveryDlgManager* self = NewLC( aIFFactory );
	CleanupStack::Pop(self);
	return self;
	}

CDiscoveryDlgManager* CDiscoveryDlgManager::NewLC(
		CIFFactory& aIFFactory )
	{
	CDiscoveryDlgManager* self =
	new (ELeave) CDiscoveryDlgManager ();
	CleanupStack::PushL(self);
	self->ConstructL( aIFFactory );
	return self;
	}

// Constructor
CDiscoveryDlgManager::CDiscoveryDlgManager()
	{
	}

// 2nd phase constructor
void CDiscoveryDlgManager::ConstructL( CIFFactory& aIFFactory )
	{
	iUidArray = new ( ELeave ) CArrayFixFlat<TInt>( KArrayGranularity );
	iBrandArray = new ( ELeave ) CArrayFixFlat<TUint>( KArrayGranularity );
	iProtocolArray = new ( ELeave ) CArrayFixFlat<TPrinterType>( KArrayGranularity );
	iListOfDeviceNames = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
	iActivePopup = EFalse;  // Popuplist is not activated
	iNameArray = new (ELeave) CDesCArrayFlat( KArrayGranularity );
	iDiscovery = aIFFactory.DiscoveryIF();
	iSettings = aIFFactory.SettingsIF();
	iSelectDeviceText = StringLoader::LoadL( R_QTN_POPUP_LIST_TITLE );
	iSearchingText = StringLoader::LoadL( R_QTN_POPUP_LIST_TITLE2 );
	iActiveTitleText.Set( iSearchingText->Des() );
	}

// Destructor
CDiscoveryDlgManager::~CDiscoveryDlgManager()
	{
	if ( iDiscovery )
		{
		iDiscovery->RemoveDiscoveryObserver();
		}
	if ( iActivePopup )
		{
		if ( iPopupList )
			{
			iPopupList->CancelPopup();
			}
		}
	delete iListBox;
	delete iUidArray;
	delete iBrandArray;
	delete iProtocolArray;
	delete iListOfDeviceNames;
	if ( iNameArray )
		{
		iNameArray->Reset();
		}
	delete iNameArray;
	delete iPrinterName;
	delete iSearchingText;
	delete iSelectDeviceText;
	}

// Called when new device is found
void CDiscoveryDlgManager::NotifyNewPrintDeviceL(
		HBufC* aText,
		TInt aUid,
		TBool aCached,
		TPrinterType aType,
		TUint aVendor )
	{
	LOG("CDiscoveryDlgManager::NotifyNewPrintDevice BEGIN");
	if( iHiddenDiscovery )
		{
		// multi-device hidden discovery (bt/wlan) handled differently
		if ( iMultiDiscovery )
			{
			LOG("CDiscoveryDlgManager::NotifyNewPrintDevice doing hidden multi-discovery");
			TInt uid = iSettings->GetDefaultPrintID();
			LOG2("CDiscoveryDlgManager::NotifyNewPrintDevice found: %d default: %d", aUid, uid);
			if ( uid == aUid )
				{
				if( iHiddenDiscoveryDialog )
					{
					TRAP_IGNORE( iHiddenDiscoveryDialog->ProcessFinishedL() );
					iHiddenDiscoveryDialog = NULL;
					}

				TInt printerErrorCode(0);
				printerErrorCode = iDiscovery->ChoosePrinterL( aUid );
				iSettings->SetCurrentPrinterProtocol( aType );
				iSettings->SetCurrentPrinterVendor( aVendor );
				iSettings->SetCurrentPrinterName( *aText );
				if( printerErrorCode == 0 )
					{
					iPrinterFound = ETrue;
					iDiscoveryCompleted = ETrue;
					delete iPrinterName;
					iPrinterName = NULL;
					// Cancel the discovery when device is found printjob is done.
					iDiscovery->CancelDeviceDiscoveryL();
					iPrinterName = aText->AllocL();
					}
				else
					{
					iPrinterFound = EFalse;
					}
				}
			}
		// USB and MMC hidden discovery only find one printer or not a printer at all
		else
			{
			if( iHiddenDiscoveryDialog )
				{
				iHiddenDiscoveryDialog->ProcessFinishedL();
				iHiddenDiscoveryDialog = NULL;
				}

			TInt printerErrorCode(0);
			printerErrorCode = iDiscovery->ChoosePrinterL( aUid );
			iSettings->SetCurrentPrinterProtocol( aType );
			iSettings->SetCurrentPrinterVendor( aVendor );            
			if( printerErrorCode == 0 )
				{
				iPrinterFound = ETrue;
				iDiscoveryCompleted = ETrue;
				delete iPrinterName;
				iPrinterName = 0;
				if( aType == MDiscoveryObserver::EMMC )
					{
					HBufC* memoryCard = 0;
					memoryCard = StringLoader::LoadLC
					( R_QTN_PRINT_MEMORY_CARD );

					iSettings->SetCurrentPrinterName( *memoryCard );
					iPrinterName = memoryCard->AllocL();                    
					CleanupStack::PopAndDestroy( memoryCard );
					}
				else
					{
					iSettings->SetCurrentPrinterName( *aText );
					iPrinterName = aText->AllocL();
					}
				}
			else
				{
				iPrinterFound = EFalse;
				}
			}
		}
	else
		{
		if ( aType == MDiscoveryObserver::EUSB )
			{
			iUsbPrinterID = aUid;
			}

		TRAPD( err, AddNewDeviceToListL( aText, aUid, aCached, aType, aVendor ) );
		if ( err )
			{
			LOG1("CDiscoveryDlgManager::NotifyNewPrintDevice device adding leaved with: %d", err );
			// Nothing to do with the error code coming from ShowErrorMsgL
			TRAP_IGNORE( IMGPPrintAppUtil::ShowErrorMsgL( err ) );
			}
		}
	LOG("CDiscoveryDlgManager::NotifyNewPrintDevice END");
	}

// Called when error is found
void CDiscoveryDlgManager::DiscoveryError(
		TInt aErrCode )
	{
	if ( aErrCode == ENoConnection )
		{		
		CancelDiscovery();
		}
	}

// Called when process completed
void CDiscoveryDlgManager::DeviceDiscoveryCompleted()
	{
	// // Added this to avoid multiple "Search Again" texts, because server sends DeviceDiscoveryCompleted() more than once 
	if( iDiscoveryCompleted )
		{
		return;
		}

    LOG1("[CDiscoveryDlgManager::DeviceDiscoveryCompleted]\t Before dissmissing the process/wait dialog, iDiscoveryCompleted has value of %d", iDiscoveryCompleted);

	// Searching over, flag is essential
	iSearchingCancelled = ETrue;

	iActiveTitleText.Set( iSelectDeviceText->Des() );
	// Set the title of the pop-up list
	if ( iPopupList )
		{
		TRAP_IGNORE( iPopupList->SetTitleL( iActiveTitleText ) );
		}
	LOG("CDiscoveryDlgManager::DeviceDiscoveryCompleted BEGIN");
	if( iHiddenDiscoveryDialog )
		{
		TRAP_IGNORE( iHiddenDiscoveryDialog->ProcessFinishedL() );
		iHiddenDiscoveryDialog = NULL;
		}

	iDiscoveryCompleted = ETrue;
	if ((iActivePopup) && (iPopupList))
		{
		iPopupList->Heading()->CancelAnimation();

		TRAP_IGNORE( RemoveCachedIconsL() );
		TRAP_IGNORE( AddSearchAgainTextL() );

		if ( iPopupList )
			{
			TRAP_IGNORE( iPopupList->ButtonGroupContainer()->SetCommandSetL(
					R_AVKON_SOFTKEYS_SELECT_CANCEL );
			iPopupList->ButtonGroupContainer()->DrawNow() );
			}
		}
	if ( iCancelling )
		{
		iCancelling = EFalse;
		TRAP_IGNORE( iCancellingDialog->ProcessFinishedL() );
		iCancellingDialog = NULL;
		}
	LOG("CDiscoveryDlgManager::DeviceDiscoveryCompleted END");
	}

// Displays the printer devices
TBool CDiscoveryDlgManager::StartDlgManagerL( TInt aProtocol, TBool aHidden )
	{
	LOG2("CDiscoveryDlgManager::StartDlgManagerL BEGIN: protocol %d, hidden: %d", aProtocol, aHidden);
	iDiscoveryCompleted = EFalse;
	TBool okToContinue( ETrue );
	iHiddenDiscovery = aHidden;

	// Start the device discovery with all the supported protocols
	if( aProtocol == 0 )
		{
		iSupportedProtocols = iDiscovery->SupportedProtocols();

		// If discovering WLAN, AP must be defined before continuing
		if ( iSupportedProtocols & KImagePrint_PrinterProtocol_UPnP )
			{
			LOG("CDiscoveryDlgManager::StartDlgManagerL: TryWlanDiscoveryL");
			TryDeviceDiscoveryL( iSupportedProtocols, EFalse );
			}
		// Discovery without WLAN    
		else
			{
			LOG("CDiscoveryDlgManager::StartDlgManagerL: StartDeviceDiscoveryL");
			iDiscovery->StartDeviceDiscoveryL( this, iSupportedProtocols );
			}
		}
	else
		{
		iSupportedProtocols = aProtocol;

		// If discovering WLAN
		if ( iSupportedProtocols & KImagePrint_PrinterProtocol_UPnP )
			{
			LOG("CDiscoveryDlgManager::StartDlgManagerL - discovering WLAN");
			if ( !TryDeviceDiscoveryL( aProtocol, ETrue ) )
				{
				LOG("CDiscoveryDlgManager::StartDlgManagerL - discovering WLAN failed");
				// If discovering failed, do not continue
				okToContinue = EFalse;
				return okToContinue;
				}
			}
		else
			{
			LOG("iDiscovery->StartDeviceDiscoveryL START");
			iDiscovery->StartDeviceDiscoveryL( this, aProtocol );
			} 

		LOG("iDiscovery->StartDeviceDiscoveryL END");
		}

	// BT & WLAN cases can find multiple devices which has to be handled
	if ( aProtocol & KImagePrint_PrinterProtocol_BPP ||
			aProtocol & KImagePrint_PrinterProtocol_BIP ||
			aProtocol & KImagePrint_PrinterProtocol_OPP_Printer ||
			aProtocol & KImagePrint_PrinterProtocol_OPP_PC ||
			aProtocol & KImagePrint_PrinterProtocol_UPnP )
		{
		iMultiDiscovery = ETrue;
		}

	if( iHiddenDiscovery )
		{
		okToContinue = StartHiddenDiscoveryL();
		}
	else
		{
		okToContinue = StartNormalDiscoveryL();
		}

	LOG1("CDiscoveryDlgManager::StartDlgManagerL END return: %d", okToContinue);
	return okToContinue;
	}

TBool CDiscoveryDlgManager::StartNormalDiscoveryL()
	{
	iActiveTitleText.Set( iSearchingText->Des() );
	LOG("CDiscoveryDlgManager::StartNormalDiscoveryL BEGIN");
	TInt printerErrorCode = 0;
	TBool continueSearch( ETrue );
	TBool okToContinue( ETrue );
	iSearchingCancelled = EFalse; // ADDED
	TInt uid; // Printer uid
	// Loop until printer is selected or user decides to exit
	while ( continueSearch )
		{
		// Create pop-up list
		if ( !iPopupList )
			{
			iPopupList = CreatePopupListL();
			}

		// Show the pop-up list
		if ( iListOfDeviceNames->Count() > 0 )
			{
			iPopupList->ButtonGroupContainer()->SetCommandSetL(
					R_AVKON_SOFTKEYS_STOP
			);
			}

		// Launch the pop-up list for found devices
		LOG("CDiscoveryDlgManager::StartNormalDiscoveryL before launch popuplist");
		TBool selected = iPopupList->ExecuteLD();
		LOG1("CDiscoveryDlgManager::StartNormalDiscoveryL after popupList, selected: %d", selected);
		iPopupList = NULL;
		iActivePopup = EFalse;
		LOG("CDiscoveryDlgManager::StartNormalDiscoveryL returned from popup");
		TInt index = iListBox->CurrentItemIndex();

		// Search again was selected
		if ( selected && iUidArray->At( index ) == KSearchAgainUID )
			{
			RestartDiscoveryL();
			}
		// Device was selected
		else if ( selected )
			{
			uid = iUidArray->At( index );
			if ( uid == iUsbPrinterID )
				{
				iUSBPrinterSelected = ETrue ;
				}
			else
				{
				iUSBPrinterSelected = EFalse;
				}

			iCurrentPrinterUid = uid;
			printerErrorCode = iDiscovery->ChoosePrinterL( uid );
			if ( printerErrorCode != KErrNone )
				{
				IMGPPrintAppUtil::ShowErrorMsgL( printerErrorCode  );
				User::Leave( printerErrorCode );
				}
			else
				{
				if ( iUidArray->At( 0 ) == KSearchAgainUID )
					{
					iSettings->SetCurrentPrinterProtocol( iProtocolArray->At( index - 1 ) );
					iSettings->SetCurrentPrinterVendor( iBrandArray->At( index - 1 ) );
					iSettings->SetCurrentPrinterName( (*iNameArray)[index-1] );
					}
				else
					{
					iSettings->SetCurrentPrinterProtocol( iProtocolArray->At( index ) );
					iSettings->SetCurrentPrinterVendor( iBrandArray->At( index ));
					iSettings->SetCurrentPrinterName( (*iNameArray)[index] );
					}

				continueSearch = EFalse;
				// If there wasn't default protocol then set selected printer and protocol as default
				if ( iSettings->GetDefaultPrinterProtocol() == KErrNotFound )
					{
					LOG1("CDiscoveryDlgManager::StartNormalDiscoveryL default protocol wasn't set, setting uid: %d", uid );
					iSettings->SetDefaultPrinterID( uid );
					// Discovery has completed and 'Search again' was added as a first item
					// access protocol array with index-1
					if ( iUidArray->At( 0 ) == KSearchAgainUID )
						{
						iSettings->SetDefaultPrinterProtocol( iProtocolArray->At( index-1 ) );
						iSettings->SetDefaultPrinterNameL( (*iNameArray)[index-1] );
						}
					else
						{
						iSettings->SetDefaultPrinterProtocol( iProtocolArray->At( index ) );
						iSettings->SetDefaultPrinterNameL( (*iNameArray)[index] );
						}
					}
				else
					{
					LOG("CDiscoveryDlgManager::StartNormalDiscoveryL default protocol was already set" );
					}
				}

			TPtrC name = iListOfDeviceNames->MdcaPoint( index );
			delete iPrinterName;
			iPrinterName = NULL;
			iPrinterName = name.AllocL();
			}
		// Stop/Cancel was selected
		else
			{
			LOG("CDiscoveryDlgManager::StartNormalDiscoveryL Stop/Cancel was selected" );
			// Searching cancelled by user, cancel device discovery
			if ( !iSearchingCancelled )
				{
				LOG("CDiscoveryDlgManager::StartNormalDiscoveryL iSearchingCancelled EFalse" );
				iSearchingCancelled = ETrue;
				iDiscovery->CancelDeviceDiscoveryL();
				}

			// User cancelled the whole discovery dialog
			else
				{
				LOG("CDiscoveryDlgManager::StartNormalDiscoveryL iSearchingCancelled ETrue" );
				continueSearch = EFalse;
				okToContinue = EFalse;
				iCancelling = ETrue;

				// User cancelled the discovery dialog before all the printers were found
				if ( !iDiscoveryCompleted )
					{
					//ShowCancelWaitNoteL();
					}
				}
			}
		}
	LOG("CDiscoveryDlgManager::StartNormalDiscoveryL END");
	return okToContinue;
	}

TBool CDiscoveryDlgManager::StartHiddenDiscoveryL()
	{
	LOG("CDiscoveryDlgManager::StartHiddenDiscoveryL BEGIN");
	iHiddenDiscoveryDialog =
	new (ELeave)CAknWaitDialog(
			(REINTERPRET_CAST(CEikDialog**,&iHiddenDiscoveryDialog)));
	LOG("iHiddenDiscoveryDialog->ExecuteLD( R_HIDDEN_DIALOG ) BEGIN");
	iHiddenDiscoveryDialog->ExecuteLD( R_HIDDEN_DIALOG );
	LOG("iHiddenDiscoveryDialog->ExecuteLD( R_HIDDEN_DIALOG ) END");
	LOG1("iPrinterFound = %d", iPrinterFound);
	LOG("CDiscoveryDlgManager::StartHiddenDiscoveryL END");
	return iPrinterFound;
	}

void CDiscoveryDlgManager::DialogDismissedL( TInt /*aButtonId*/ )
	{
	LOG("CDiscoveryDlgManager::DialogDismissedL BEGIN");
	// This is called only if hidden discovery is cancelled
	// start cancelling operation and put cancelling note on.
	iPrinterFound = EFalse;
	iCancelling = ETrue;
	iDiscovery->CancelDeviceDiscoveryL();
	// User cancelled the discovery dialog before all the printers were found
	if ( !iDiscoveryCompleted )
		{
		ShowCancelWaitNoteL();
		}
	LOG("CDiscoveryDlgManager::DialogDismissedL END");
	}

void CDiscoveryDlgManager::ShowCancelWaitNoteL()
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
	}

// Creates and initializees the popup list
CAknPopupList* CDiscoveryDlgManager::CreatePopupListL()
	{
	LOG("CDiscoveryDlgManager::CreatePopupListL BEGIN");
	if ( iListBox )
		{
		delete iListBox;
		iListBox = 0;
		}
	// Creates list box for the pop-up list
	iListBox = new ( ELeave ) CAknSingleGraphicPopupMenuStyleListBox;//CAknSingleGraphicPopupMenuStyleListBox;

	// Create and initialize the popup list
	iPopupList = CAknPopupList::NewL(
			iListBox,
			/*R_AVKON_SOFTKEYS_CANCEL,*/
			R_AVKON_SOFTKEYS_STOP,
			AknPopupLayouts::EDynMenuWindow );

	// Set the title of the pop-up list
	iPopupList->SetTitleL( iActiveTitleText );

	// Initialize list box
	iListBox->ConstructL( iPopupList,
			EAknListBoxSelectionList ); // EAknListBoxMenuList
	iListBox->CreateScrollBarFrameL( ETrue );
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
			CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	HBufC* text = StringLoader::LoadLC( R_QTN_PRINT_NO_PRINTERS );
	iListBox->View()->SetListEmptyTextL( *text );
	CleanupStack::PopAndDestroy( text );

	//Array of icons to show in the pop-up list
	CArrayPtrFlat< CGulIcon >* icons =
	new( ELeave ) CArrayPtrFlat< CGulIcon >( KNumberOfIcons );

	//Push to ClenupStack. Array and all of items added to it
	//are deleted if leave occurs
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy, icons));
	icons->SetReserveL( KNumberOfIcons );

	CFbsBitmap* bitmap = NULL;
	CFbsBitmap* mask = NULL;

	bitmap = new( ELeave ) CFbsBitmap();
	User::LeaveIfError( bitmap->Create( TSize(0,0), EColor256 ) );
	CleanupStack::PushL( bitmap );
	CGulIcon* icon = CGulIcon::NewL( bitmap );
	CleanupStack::Pop( bitmap );
	CleanupStack::PushL( icon );
	icons->AppendL( icon );
	CleanupStack::Pop( icon );

	AknIconUtils::CreateIconLC( bitmap, mask, KIconFile,
			EMbmImgprintbitmapsQgn_indi_print_cached,
			EMbmImgprintbitmapsQgn_indi_print_cached_mask );
	icon = CGulIcon::NewL(bitmap,mask);
	CleanupStack::Pop( 2 ); // bitmap, mask
	CleanupStack::PushL( icon );
	icons->AppendL( icon );
	CleanupStack::Pop( icon );

	AknIconUtils::CreateIconLC( bitmap, mask, KIconFile,
			EMbmImgprintbitmapsQgn_prop_print_bt,
			EMbmImgprintbitmapsQgn_prop_print_bt_mask );
	icon = CGulIcon::NewL( bitmap, mask );
	CleanupStack::Pop( 2 ); // bitmap, mask
	CleanupStack::PushL( icon );
	icons->AppendL( icon );
	CleanupStack::Pop( icon );

	AknIconUtils::CreateIconLC( bitmap, mask, KIconFile,
			EMbmImgprintbitmapsQgn_prop_print_kiosk,
			EMbmImgprintbitmapsQgn_prop_print_kiosk_mask );
	icon = CGulIcon::NewL( bitmap, mask );
	CleanupStack::Pop( 2 ); // bitmap, mask
	CleanupStack::PushL( icon );
	icons->AppendL( icon );
	CleanupStack::Pop( icon );

	AknIconUtils::CreateIconLC( bitmap, mask, KIconFile,
			EMbmImgprintbitmapsQgn_prop_print_mmc,
			EMbmImgprintbitmapsQgn_prop_print_mmc_mask );
	icon = CGulIcon::NewL( bitmap, mask );
	CleanupStack::Pop( 2 ); // bitmap, mask
	CleanupStack::PushL( icon );
	icons->AppendL( icon );
	CleanupStack::Pop( icon );

	AknIconUtils::CreateIconLC( bitmap, mask, KIconFile,
			EMbmImgprintbitmapsQgn_prop_print_usb,
			EMbmImgprintbitmapsQgn_prop_print_usb_mask );
	icon = CGulIcon::NewL( bitmap, mask );
	CleanupStack::Pop( 2 ); // bitmap, mask
	CleanupStack::PushL( icon );
	icons->AppendL( icon );
	CleanupStack::Pop( icon );

	AknIconUtils::CreateIconLC( bitmap, mask, KIconFile,
			EMbmImgprintbitmapsQgn_prop_print_wlan,
			EMbmImgprintbitmapsQgn_prop_print_wlan_mask );
	icon = CGulIcon::NewL( bitmap, mask );
	CleanupStack::Pop( 2 ); // bitmap, mask
	CleanupStack::PushL( icon );
	icons->AppendL( icon );
	CleanupStack::Pop( icon );

	CleanupStack::Pop( icons );
	iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( icons );
	iListBox->Model()->SetItemTextArray( iListOfDeviceNames );
	iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

	// Set the animation
	iPopupList->Heading()->SetHeaderAnimationL( R_IMAGEPRINT_ANIMATION );
	iActivePopup = ETrue;

	LOG("CDiscoveryDlgManager::CreatePopupListL END");
	return iPopupList;
	}

// Adds new device to list, updates popup list
void CDiscoveryDlgManager::AddNewDeviceToListL(
		HBufC* aText,
		TInt aUid,
		TBool aCached,
		TPrinterType aType,
		TUint aVendor )
	{
	LOG("CDiscoveryDlgManager::AddNewDeviceToListL BEGIN");
	HBufC* buf;

	// if the device is memory card
	if( aType == MDiscoveryObserver::EMMC )
		{
		HBufC* stringholdercopy = StringLoader::LoadLC
		( R_QTN_PRINT_MEMORY_CARD );
		buf = HBufC::NewLC( stringholdercopy->Size() + KSpaceForSingleIcon );
		TPtr ptr = buf->Des();
		ptr.Append( KMMCIconIndex );
		ptr.Append( KTabChar );
		ptr.Append( stringholdercopy->Des() );

		// Add new device into array
		iListOfDeviceNames->InsertL( 0, *buf );
		iNameArray->InsertL( 0, *stringholdercopy );
		CleanupStack::PopAndDestroy( 2, stringholdercopy ); //buf, stringholdercopy
		}
	else // any other device
		{
		if ( aCached )
			{
			buf = HBufC::NewLC( aText->Size() + KSpaceForBothIcons );
			}
		else
			{
			buf = HBufC::NewLC( aText->Size() + KSpaceForSingleIcon );
			}
		TPtr ptr = buf->Des();
		if( aType == MDiscoveryObserver::EBPP )
			{
			ptr.Append( KBTIconIndex );
			}
		else if( aType == MDiscoveryObserver::EOPP )
			{
			ptr.Append( KKioskIconIndex );
			}
		else if( aType == MDiscoveryObserver::EUSB )
			{
			ptr.Append( KUSBIconIndex );
			}
		else if( aType == MDiscoveryObserver::EWLAN )
			{
			ptr.Append( KWLANIconIndex );
			}
		else
			{
			ptr.Append( KEmptyIconIndex );
			}
		ptr.Append( KTabChar );        	
		ptr.Append( aText->Des() );

		if ( aCached )
			{
			ptr.Append( KTabChar );  
			ptr.Append( KCachedIconIndex );
			}

		// Add new device into array
		iListOfDeviceNames->InsertL( 0, *buf );
		CleanupStack::PopAndDestroy( buf );
		}

	// Add Uid of the device into array
	iUidArray->InsertL( 0, aUid );

	aVendor = TPrinter::EVendorNone;

	iBrandArray->InsertL( 0, aVendor );
	iProtocolArray->InsertL( 0, aType );
	if( aType != MDiscoveryObserver::EMMC )
		{
		iNameArray->InsertL( 0, *aText );
		}

	if ( iActivePopup )
		{
		LOG("CDiscoveryDlgManager::AddNewDeviceToListL popup update");
		iListBox->HandleItemAdditionL();
		iPopupList->DrawNow();

		if ( iListOfDeviceNames->MdcaCount() == 1 )
			{
			// First item to appear, change CBA
			iPopupList->ButtonGroupContainer()->SetCommandSetL(
					R_AVKON_SOFTKEYS_STOP
			);
			iPopupList->ButtonGroupContainer()->DrawNow();
			}
		}
	LOG("CDiscoveryDlgManager::AddNewDeviceToListL END");
	}

// Returns the name of the chosen printer
TInt CDiscoveryDlgManager::PrinterUid() const
	{
	return iCurrentPrinterUid;
	}

// Returns the name of the chosen printer
const TDesC& CDiscoveryDlgManager::PrinterName() const
	{
	LOG("CDiscoveryDlgManager::PrinterName() BEGIN");
	if ( !iPrinterName )
		{
		return KNullDesC();
		}
	else
		{
		return *iPrinterName;
		}
	}
	
	TBool CDiscoveryDlgManager::USBPrinterSelected() const
	{
	return iUSBPrinterSelected;
	}

// Resets the popup list and starts the device discovery again
void CDiscoveryDlgManager::RestartDiscoveryL()
	{
	iSearchingCancelled = EFalse;

	iListOfDeviceNames->Reset();
	iUidArray->Reset();
	iBrandArray->Reset();
	iProtocolArray->Reset();
	iActiveTitleText.Set( iSearchingText->Des() );
	iPopupList = CreatePopupListL();
	iDiscoveryCompleted = EFalse;
	iDiscovery->StartDeviceDiscoveryL( this, iSupportedProtocols );
	}

// Removes cached printer from the list
void CDiscoveryDlgManager::RemoveCachedPrinterL(
		TInt aUid )
	{
	TInt i( 0 );
	TBool found( EFalse );
	for ( i = 0; i < iUidArray->Count() && !found; i++ )
		{
		if ( aUid == iUidArray->At( i ) )
			{
			TInt curIndex = iListBox->CurrentItemIndex();
			if ( curIndex >= i &&
					iListOfDeviceNames->MdcaCount() > 1 && curIndex > 0 )
				{
				iListBox->SetCurrentItemIndexAndDraw( curIndex - 1 );
				}

			iUidArray->Delete( i );
			iBrandArray->Delete( i );
			iProtocolArray->Delete( i );
			iNameArray->Delete( i );
			iListOfDeviceNames->Delete( i );
			iListBox->HandleItemRemovalL();
			iPopupList->DrawNow();

			if ( iListOfDeviceNames->MdcaCount() == 0 )
				{
				// Zero items, change softkeys
				if ( iActivePopup )
					{
					iPopupList->ButtonGroupContainer()->SetCommandSetL(
							R_AVKON_SOFTKEYS_CANCEL );
					iPopupList->ButtonGroupContainer()->DrawNow();
					}
				}
			found = ETrue;
			}
		}
	}

// Removes cached printer icons from the list
void CDiscoveryDlgManager::RemoveCachedIconsL()
	{
	TInt counter = iListOfDeviceNames->MdcaCount();
	TChar KTabChar( 0x0009 );
	for ( TInt i = 0 ; i < counter; i++ )
		{
		TPtrC tmp( iListOfDeviceNames->MdcaPoint( i ) );
		//locate the rightmost tab
		TInt tabPosition = tmp.LocateReverse( KTabChar );
		if ( tabPosition != KErrNotFound &&
				tabPosition > 1 ) //don't remove the pre-text tab
			{
			iListOfDeviceNames->InsertL( i, tmp.Left( tabPosition ) );
			iListOfDeviceNames->Delete( i + 1 );
			}
		}
	iListBox->HandleItemAdditionL();
	iPopupList->DrawNow();
	}

// Adds serarch again text to list
void CDiscoveryDlgManager::AddSearchAgainTextL()
	{
	HBufC* str = StringLoader::LoadLC
	( R_QTN_SEARCH_AGAIN );
	HBufC* buf = HBufC::NewLC( str->Size() + KSpaceForSingleIcon );
	TPtr ptr = buf->Des();
	ptr.Append( KEmptyIconIndex );
	ptr.Append( KTabChar );
	ptr.Append( str->Des() );

	iListOfDeviceNames->InsertL( 0, *buf );

	CleanupStack::PopAndDestroy( 2, str ); //buf, str

	iUidArray->InsertL( 0, KSearchAgainUID );
	iListBox->HandleItemAdditionL();
	iPopupList->DrawNow();
	}

//--------------------------------------------------------------------------------------------
//
// CDiscoveryDlgManager::WlanAccessPointExists
//
//--------------------------------------------------------------------------------------------
TBool CDiscoveryDlgManager::WlanAccessPointExistsL()
	{
	LOG("[CDiscoveryDlgManager::WlanAccessPointExists] BEGIN");
	TBool accessPointExists( EFalse );
        CUpnpShowCommand* upnpShowCommand = NULL;
        TRAPD ( error, upnpShowCommand = CUpnpShowCommand::NewL() );   
        if ( error != KErrNone )
        	{
        	LOG1("[CDiscoveryDlgManager::WlanAccessPointExists] CUpnpShowCommand::NewL leaves, with: %d", error);
        	return accessPointExists;
        	}
        CleanupStack::PushL( upnpShowCommand );    
        accessPointExists = upnpShowCommand->IsAvailableL();
        CleanupStack::PopAndDestroy(); // upnpShowCommand
		LOG1("[CDiscoveryDlgManager::WlanAccessPointExists]\t END, AccessPointExists: %d", accessPointExists);
        return accessPointExists;  
	}

//--------------------------------------------------------------------------------------------
//
// CDiscoveryDlgManager::TryDeviceDiscoveryL()
//
//--------------------------------------------------------------------------------------------
TBool CDiscoveryDlgManager::TryDeviceDiscoveryL( TUint aProtocols, TBool aSingleDiscovery )
	{
	LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL");

	// WLAN access point not defined 	
	if ( !WlanAccessPointExistsL() )
		{          
		LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - AP not defined, launch WLAN wizard query");

		// Launch WLAN wizard query
		LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - CAknMessageQueryDialog");
		CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog;

		// User pressed OK
		if ( dlg->ExecuteLD( R_LAUNCH_WLAN_WIZARD_QUERY ) )
			{
			LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - Launch WLAN wizard");

			// Launch WLAN wizard
			CUpnpRunSetupCommand* runSetupCommand = CUpnpRunSetupCommand::NewL();
			CleanupStack::PushL(runSetupCommand);
			runSetupCommand->RunSetupL();
			CleanupStack::PopAndDestroy(runSetupCommand);


			// If access point defined by the WLAN wizard
			if ( WlanAccessPointExistsL() )
				{
				LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - AP defined in WLAN wizard");

				if ( aSingleDiscovery )
					{
					// Start discovery
					// Continues to StartDeviceDiscoveryL
					LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - Start discovery with single protocol");
					iDiscovery->StartDeviceDiscoveryL( this, aProtocols );
					}
				else
					{
					// Start discovery
					// Continues to StartDeviceDiscoveryL
					LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - Start discovery with all prots");
					iDiscovery->StartDeviceDiscoveryL( this, iSupportedProtocols );
					}
				}

			// Access point not defined by the WLAN wizard
			else
				{
				// Thus, WLAN discovery not supported
				iSupportedProtocols = iSupportedProtocols - KImagePrint_PrinterProtocol_UPnP;

				if ( aSingleDiscovery )
					{
					// Do nothing
					// WLAN not supported
					LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - Launch WLAN wizard canceled");
					return EFalse;
					}
				else
					{
					LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - Start discovery without WLAN");
					iDiscovery->StartDeviceDiscoveryL( this, iSupportedProtocols );
					}

				iSupportedProtocols = iSupportedProtocols - KImagePrint_PrinterProtocol_UPnP;
				LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - Start discovery without WLAN");
				iDiscovery->StartDeviceDiscoveryL( this, iSupportedProtocols );
				}
			}

		// User pressed cancel
		else
			{  
			LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - User pressed cancel");

			// WLAN discovery not supported
			iSupportedProtocols = iSupportedProtocols - KImagePrint_PrinterProtocol_UPnP; 

			if ( aSingleDiscovery )
				{
				// Do nothing
				// WLAN not supported
				LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - Launch WLAN wizard canceled");
				return EFalse;
				}
			else
				{
				LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - Start discovery without WLAN");
				iDiscovery->StartDeviceDiscoveryL( this, iSupportedProtocols );
				}  
			}

		return ETrue;
		}

	// If WLAN access point is defined
	else 
		{
		LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - AP is defined");

		// Start discovery
		// Continues to StartDeviceDiscoveryL

		if ( aSingleDiscovery )
			{
			LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - Start WLAN discovery");
			iDiscovery->StartDeviceDiscoveryL( this, aProtocols );
			}
		else
			{
			LOG("[CUPnPPrintingDevice]\t TryDeviceDiscoveryL - Start discovery with all prots");
			iDiscovery->StartDeviceDiscoveryL( this, iSupportedProtocols ); 
			}

		return ETrue;
		}
	}

//--------------------------------------------------------------------------------------------
//
// CDiscoveryDlgManager::CancelDiscovery()
//
//--------------------------------------------------------------------------------------------
void CDiscoveryDlgManager::CancelDiscovery()
	{
	LOG("CDiscoveryDlgManager::CancelDiscovery begin");
	if(iPopupList)
		{
		LOG("CDiscoveryDlgManager::CancelDiscovery canceling popup device list.");
		iSearchingCancelled = ETrue;

		TInt error ( KErrNone );
		TRAP( error, iDiscovery->CancelDeviceDiscoveryL());


		iPopupList->CancelPopup();
		}
	LOG("CDiscoveryDlgManager::CancelDiscovery end");
	}

//  End of File
