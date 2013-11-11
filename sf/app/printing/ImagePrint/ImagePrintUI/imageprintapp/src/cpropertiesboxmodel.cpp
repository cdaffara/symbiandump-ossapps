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


#include <akntextsettingpage.h>
#include <eiklbx.h>
#include <StringLoader.h>
#include <AknQueryValueText.h>
#include <aknpopupsettingpage.h>
#include <aknslidersettingpage.h> 
#include <bautils.h>
#include <pathinfo.h>
#include <aknmessagequerydialog.h>
#include <apgcli.h>                 // rapalssession
#include <imageprintapp.rsg>
#include <AknQueryDialog.h>
#include <driveinfo.h>

#include "imageprint.h"
#include "cprintsettings.h"
#include "cbasecapability.h"
#include "cimageprintsettingscontainer.h"
#include "cimageprintengine.h"
#include "ciffactory.h"
#include "mdiscoveryobserver.h"
#include "mdiscovery.h"
#include "clog.h"
#include "clistcapability.h"
#include "imageprintvariant.h"
#include "cpropertiesboxmodel.h"

const TInt KListItemsRequired( 2 );
const TInt KDefaultPrinterIndex( 0 );
const TInt KNumerOfCopiesIndex( 1 );

// CONSTRUCTION
CPropertiesBoxModel* CPropertiesBoxModel::NewL(
    CImagePrintEngine& aEngine,
    MPrintSettings* aSettings,
    CImagePrintSettingsContainer* aContainer )
    {
    CPropertiesBoxModel* self =
        CPropertiesBoxModel::NewLC( aEngine, aSettings, aContainer );
    CleanupStack::Pop( self );
    return self;
    }

CPropertiesBoxModel* CPropertiesBoxModel::NewLC(
    CImagePrintEngine& aEngine,
    MPrintSettings* aSettings,
    CImagePrintSettingsContainer* aContainer )
    {
    CPropertiesBoxModel* self =
        new ( ELeave ) CPropertiesBoxModel( aEngine, aSettings, aContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Constructor
CPropertiesBoxModel::CPropertiesBoxModel(
    CImagePrintEngine& aEngine,
    MPrintSettings* aSettings,
    CImagePrintSettingsContainer* aContainer ) :
    iEngine( aEngine ),
    iSettingsIF( aSettings ),
    iContainer( aContainer )
    {
    }

// Destructor
CPropertiesBoxModel::~CPropertiesBoxModel()
    {
    delete iPrintSettings;
    delete iDefaultPrinter;
    delete iNumberOfCopiesText; 
    delete iMMC;
    delete iPictBridge;
    }

// 2nd phase constructor
void CPropertiesBoxModel::ConstructL()
    {
    InitSettingsL();
    iDefaultPrinter = StringLoader::LoadL( R_PRINT_SELECTED_PRINTER );
    iNumberOfCopiesText = StringLoader::LoadL( R_QTN_PRINT_SETTINGS_NOC );
    iMMC = StringLoader::LoadL( R_PRINT_MEMORY_CARD );
    iPictBridge = StringLoader::LoadL( R_QTN_PRINT_SETTINGS_DEFAULT_USB );
    iNumberOfCopiesValue = 1;
    }

void CPropertiesBoxModel::InitSettingsL()
    {
    delete iPrintSettings;
    iPrintSettings = NULL;
    iPrintSettings = iSettingsIF->PrinterSettingsL();
    iCount = iPrintSettings->iCapabilities.Count();
    TInt i( 0 );
    TInt counter = iPrintSettings->iCapabilities.Count();
    // If current protocol is MMC remove one item, because there is no layouts for mmc. 
    if ( MDiscoveryObserver::EMMC == iSettingsIF->GetCurrentPrinterProtocol() )
    	{
    		iCount--;
    	}
      
    for ( i = 0; i < counter; i++ )
        {
        if ( iPrintSettings->iCapabilities[i]->iTitle.CompareF( KNullDesC ) == 0 )
            {
            // Remove non-title capability items as those won't be displayed
            iCount--;
            }
        }
    
    // Add always first two for default printer item and for Number of copies item
    iCount = iCount + KListItemsRequired;
    }

// Returns the number of listbox items
TInt CPropertiesBoxModel::MdcaCount() const
    {
    LOG1("CPropertiesBoxModel::MdcaCount: %d", iCount );
    return iCount;
    }

// Creates the text string for the list box item and returns it
TPtrC CPropertiesBoxModel::MdcaPoint( TInt aIndex ) const
    {
    LOG1("CPropertiesBoxModel::MdcaPoint START index: %d", aIndex );
	
	switch ( aIndex )
		{
		case KDefaultPrinterIndex:
		    {
		    SetDefaultPrinterItem();
			break;
		    }
		case KNumerOfCopiesIndex:
		    {
			SetNumberOfCopiesItem();
			break;
		    }
		default:
            {
            SetOtherCapability( aIndex );
			break;
            }    
		}
	
	LOG1("CPropertiesBoxModel::MdcaPoint END, iBuf: %S", &iBuf );        
    return iBuf;
    }

void CPropertiesBoxModel::SetDefaultPrinterItem() const
    {
    LOG("CPropertiesBoxModel::SetDefaultPrinterItem START");

    iBuf.Copy( KTabChar );
    iBuf.Append( *iDefaultPrinter );
    iBuf.Append( KTabChar );
    iBuf.Append( KTabChar );
    if( iSettingsIF->GetDefaultPrinterProtocol() ==
        MDiscoveryObserver::EMMC ) // mmc
        {
        iBuf.Append( *iMMC );
        }
    else if( iSettingsIF->GetDefaultPrinterProtocol() ==
             MDiscoveryObserver::EUSB ) // pictbridge
        {
        iBuf.Append( *iPictBridge );
        }
    else
        {
        // No can do if this fails. Not leaving function.
        TRAP_IGNORE(
            HBufC* defaultPrinterName =
                iSettingsIF->GetDefaultPrinterNameLC();
            iBuf.Append( *defaultPrinterName );
            CleanupStack::PopAndDestroy( defaultPrinterName );
            );
        }
    
    LOG("CPropertiesBoxModel::SetDefaultPrinterItem END");	
    }

void CPropertiesBoxModel::SetNumberOfCopiesItem() const
    {
    LOG("CPropertiesBoxModel::SetNumberOfCopiesItem START");	

    iBuf.Copy( KTabChar );
    iBuf.Append( *iNumberOfCopiesText );
    iBuf.Append( KTabChar );
    iBuf.Append( KTabChar );
    iBuf.AppendNum( iNumberOfCopiesValue );

    LOG("CPropertiesBoxModel::SetNumberOfCopiesItem END");	
    }

void CPropertiesBoxModel::SetOtherCapability( const TInt &aIndex ) const
    {
    LOG("CPropertiesBoxModel::SetOtherCapability START");	
    
    iBuf.Zero();
    HBufC* buf = NULL;

    // Get the text for the list box
    TInt position = CapabilityArrayIndex( aIndex - KListItemsRequired );
    TRAPD( err,
       buf = iPrintSettings->iCapabilities[position]->ListBoxTextL() );
	if ( !err && buf )
        {
        iBuf = buf->Des();
        delete buf;
        }

    LOG("CPropertiesBoxModel::SetOtherCapability END");	
    }

// Handles the list box (enter) events by launching the pop-up list
void CPropertiesBoxModel::HandleListBoxEventL(
    CEikListBox* aListBox,
    TListBoxEvent aEventType )
    {
    LOG("CPropertiesBoxModel::HandleListBoxEventL START");
    //Change to EEventItemSingleClicked for Single click event
    if ( (aEventType == EEventEnterKeyPressed || aEventType == EEventItemSingleClicked ) &&
         aListBox->CurrentItemIndex() != KErrNotFound )
        {
        if( aListBox->CurrentItemIndex() == 0 )
            {
            // launch dialog query for selecting default printer this returns
            // protocol id as defined in DiscoveryObserver.h or -1 if failed.
            TInt protocol = LaunchDefaultPrinterListL();
            if( protocol >= 0 )
                {
                iContainer->DoDiscoveryL( protocol );
                InitSettingsL();
                iContainer->UpdateListBox();
                iContainer->DrawNow();
                }
            }
        
         if( aListBox->CurrentItemIndex() == 1 )
            {
            LaunchNumberOfCopiesDialogL();
           	iContainer->UpdateListBox();
            iContainer->DrawNow();
            }
 	
         if ( aListBox->CurrentItemIndex() > 1 )
            {
            TInt selIndex = CapabilityArrayIndex( aListBox->CurrentItemIndex() - KListItemsRequired );
            TBool valueChanged = iPrintSettings->iCapabilities[selIndex]->LaunchPopupListL();

            if( valueChanged )
                {
                SaveCurrentSelectionL( *(iPrintSettings->iCapabilities[ selIndex ]), aListBox );
                InitSettingsL();
                iContainer->UpdateListBox();
                iContainer->DrawNow();                    
                }
            }
        }

	LOG("CPropertiesBoxModel::HandleListBoxEventL END");        
    }

// Saves selection to engine
void CPropertiesBoxModel::SaveCurrentSelectionL(
                          CBaseCapability &aCapab,
                          CEikListBox* aListBox )
    {
    LOG("CPropertiesBoxModel::SaveCurrentSelectionL START");    
    
    TInt initList = iSettingsIF->SetPrinterSettingL( aCapab );
    if( initList == 0 )
        {
        CPrintSettings* tempSettings = iSettingsIF->PrinterSettingsL();
        delete iPrintSettings;
        iPrintSettings = tempSettings;

        // Sets the active page to be the first one, if settings have truly
        // changed
        iSettingsIF->SetActivePageL( 1 );

        aListBox->HandleItemRemovalL();
        aListBox->HandleItemAdditionL();
        }
	
	LOG("CPropertiesBoxModel::SaveCurrentSelectionL END");            
    }

// Returns capability array index based on the list box index
TUint CPropertiesBoxModel::CapabilityArrayIndex(
    TInt aListBoxIndex ) const
    {
	LOG("CPropertiesBoxModel::CapabilityArrayIndex START");    
    
    // Skip capabilities with empty title as they won't be placed to list box
    TInt found( EFalse );
    TInt i( 0 );
    
    while ( !found )
        {
        if ( iPrintSettings->iCapabilities[i]->
             iTitle.CompareF( KNullDesC ) != 0  )
            {
            aListBoxIndex--;
            }

        if ( aListBoxIndex >= 0 )
            {
            i++;
            }
        else
            {
            found = ETrue;
            }
        }
	
	LOG("CPropertiesBoxModel::CapabilityArrayIndex END");    
    return TUint( i );
    }


void CPropertiesBoxModel::LaunchNumberOfCopiesDialogL()
    {
	LOG("CPropertiesBoxModel::LaunchNumberOfCopiesDialogL START");       
	CAknNumberQueryDialog* dlg = new( ELeave ) CAknNumberQueryDialog ( iNumberOfCopiesValue );
	dlg->PrepareLC( R_NOC_QUERY);
	dlg->RunLD();
    
    // After selecting copies, NaviText is updated.
    iContainer->ChangeStatusPaneTextL( iNumberOfCopiesValue ); 
	                                                           
    // Set value for settings
    SetNumberOfCopiesValueL();	
	
	LOG("CPropertiesBoxModel::LaunchNumberOfCopiesDialogL END");       
    }


void CPropertiesBoxModel::SetNumberOfCopiesValueL()
    {
    RArray<TInt> numberOfCopiesArray;
    CleanupClosePushL( numberOfCopiesArray );
    iSettingsIF->NumberOfCopies( numberOfCopiesArray );
   
    // Images selected in print job
    TInt imagesCount = iEngine.FileArray().Count(); 
    if(0 != imagesCount)
        {
        // Get number of copies for current image
        if( numberOfCopiesArray.Count() != imagesCount )
            {
            numberOfCopiesArray.Reset();
            for( TInt i=0; i < imagesCount; ++i )
                {
                numberOfCopiesArray.Append( 1 );
                }
            }
    
        TInt oldNOC( numberOfCopiesArray[ 0 ] );
    
        // if new value was entered
        if (iNumberOfCopiesValue != oldNOC)
            {
            // Set Number of copies value for each image.
            // Value is same for all images in IP 6.2
            for ( TInt i=0; i<imagesCount; i++ )
                {
                numberOfCopiesArray[ i ] = iNumberOfCopiesValue;
                }
            
            iSettingsIF->SetNumberOfCopiesL( numberOfCopiesArray );
            }
        }
    CleanupStack::PopAndDestroy( &numberOfCopiesArray ); 
    }

TInt CPropertiesBoxModel::LaunchDefaultPrinterListL()
    {
    const TInt expectedNumberOfProtocols( 4 );
    
    // Array for list item descriptors.
    CDesCArrayFlat* protocolNameArray = 
        new( ELeave ) CDesCArrayFlat( expectedNumberOfProtocols );
    CleanupStack::PushL( protocolNameArray );

    // Map from list item index to protocol.
    RArray<TInt> indexToProtocolMap( expectedNumberOfProtocols );
    CleanupClosePushL( indexToProtocolMap );

    // Get supported protocols from the engine
    TInt supportedProtocols =
        iEngine.InterfaceFactory().DiscoveryIF()->SupportedProtocols();

    // Read list item descriptors, append them in array and update map.

    // Bluetooth printer
    if( supportedProtocols & KImagePrint_PrinterProtocol_BPP )
        {
        protocolNameArray->AppendL( CCoeEnv::Static()->AllocReadResourceLC(
            R_QTN_PRINT_SETTINGS_DEFAULT_BT )->Des() );
        indexToProtocolMap.AppendL( MDiscoveryObserver::EBPP );
        CleanupStack::PopAndDestroy();	// protocolNameArray
        }

    // Memory card printer
    if( supportedProtocols & KImagePrint_PrinterProtocol_DPOF )
        {
        RFs fs;
    	CleanupClosePushL( fs ); 
    	if( fs.Connect() == KErrNone )
        	{
			TInt driveId (-1);
			TFileName path;
			User::LeaveIfError( DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRemovableMassStorage, driveId ) );
			User::LeaveIfError( PathInfo::GetRootPath( path, driveId ) );

    		if ( BaflUtils::PathExists( fs, path ))
    			{
		        protocolNameArray->AppendL( CCoeEnv::Static()->AllocReadResourceLC(
	            R_QTN_PRINT_MEMORY_CARD )->Des() );
		        indexToProtocolMap.AppendL( MDiscoveryObserver::EMMC );
		        CleanupStack::PopAndDestroy();	// protocolNameArray
    			}
    		
        	CleanupStack::PopAndDestroy( &fs );
        	}
        }

    // USB printers
    if( supportedProtocols & KImagePrint_PrinterProtocol_PictBridge )
        {
        protocolNameArray->AppendL( CCoeEnv::Static()->AllocReadResourceLC(
            R_QTN_PRINT_SETTINGS_DEFAULT_USB )->Des() );
        indexToProtocolMap.AppendL( MDiscoveryObserver::EUSB );
        CleanupStack::PopAndDestroy();	// protocolNameArray
        }

    // WLAN printers 
    if ( supportedProtocols & KImagePrint_PrinterProtocol_UPnP )
        {
        protocolNameArray->AppendL( CCoeEnv::Static()->AllocReadResourceLC(
        R_QTN_PRINT_SETTINGS_DEFAULT_WLAN )->Des() );
        indexToProtocolMap.AppendL( MDiscoveryObserver::EWLAN );
        CleanupStack::PopAndDestroy();	// protocolNameArray
        }
    
    // Prepare arrays for dialog
    CAknQueryValueTextArray* queryTextArray = CAknQueryValueTextArray::NewL();
    CleanupStack::PushL( queryTextArray );
    queryTextArray->SetArray( *protocolNameArray );

    CAknQueryValueText* queryValue = CAknQueryValueText::NewL();
    CleanupStack::PushL( queryValue );
    queryValue->SetArrayL( queryTextArray );

    // Determine which is the default protocol using settings interface.
    // Here defaultProtocol is protocol id as defined in MDiscoveryObserver.h.
    TInt defaultProtocol = iSettingsIF->GetDefaultPrinterProtocol();

    // Then do lookup in indexToProtocolMap to find correct index for this
    // protocol id. Note that after this defaultProtocol is an index.
    defaultProtocol = indexToProtocolMap.Find( defaultProtocol );
    if ( defaultProtocol != KErrNotFound )
    	{
	    queryValue->SetCurrentValueIndex( defaultProtocol );
    	}
    else
    	{
   	    // Default protocol not available, get current protocol
   	    // This is for example for the case where MMC is default,
   	    // but removed from phone.
   	    TInt currentProtocol = iSettingsIF->GetCurrentPrinterProtocol();
	    currentProtocol = indexToProtocolMap.Find( currentProtocol );
	    if ( currentProtocol != KErrNotFound )
	    	{
			queryValue->SetCurrentValueIndex( currentProtocol );
	    	}
    	}

    CAknPopupSettingPage* page = new ( ELeave ) CAknPopupSettingPage(
                iDefaultPrinter,
                EAknSettingPageNoOrdinalDisplayed,
                EAknCtPopupSettingList,
                R_EMPTY_PROTOCOL_SETTING_LIST,
                R_SETTING_GENERIC_PROTOCOL_PAGE,
                *queryValue );
    
    CleanupStack::PushL( page );
    page->ConstructL();
    CleanupStack::Pop( page );

    // Execute dialog and check that it returns properly. If so get the selected
    // index and use it to protocol map to get name of the selected protocol.
    TInt selectedProtocolId( -1 );
    if( page->ExecuteLD() )
        {
        selectedProtocolId =
            indexToProtocolMap[ queryValue->CurrentValueIndex() ];
        }

    // CleanupStack contains 4 items:
    // protocolNameArray, indexToProtocolMap (close), queryTextArray and
    // queryArray.
    const TInt numberOfItemsInCleanupStack( 4 );
    CleanupStack::PopAndDestroy( numberOfItemsInCleanupStack, protocolNameArray );
    return selectedProtocolId;
    }

TInt CPropertiesBoxModel::NumberOfCopies()
	{
	return iNumberOfCopiesValue;
	}

//  End of File
