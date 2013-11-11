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


#include <aknViewAppUi.h>
#include <aknnavi.h>
#include <akntitle.h>
#include <avkon.hrh>
#include <imageprintapp.rsg>
#include <eikmenup.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <wlaninternalpskeys.h>
#include <e32property.h>

#include "cimageprintsettingsview.h"
#include "cimageprintsettingscontainer.h"
#include "cimageprintsettingsview.h"
#include "cimageprintappui.h"
#include "cimageprintdoc.h"
#include "cimgpprintdlgmanager.h"
#include "mprintsettings.h"
#include "mprintjob.h"
#include "imageprintapp.hrh"
#include "cimageprintengine.h"
#include "muimode.h"
#include "ciffactory.h"
#include "mdiscoveryobserver.h"

_LIT( KImagePrintAppHelpSettings, "IMGPRNAPP_HLP_SETTINGS" );

// CONSTRUCTION
CImagePrintSettingsView* CImagePrintSettingsView::NewL(
    MPrintSettings* aSettings,
    MPrintJob* aPrintJob,
    CImagePrintAppUi* aAppUi,
    MUIMode* aUIMode )
    {
    CImagePrintSettingsView* self = CImagePrintSettingsView::NewLC(
        aSettings, aPrintJob, aAppUi, aUIMode );
    CleanupStack::Pop();	// self

    return self;
    }

CImagePrintSettingsView* CImagePrintSettingsView::NewLC( 
    MPrintSettings* aSettings,
    MPrintJob* aPrintJob,
    CImagePrintAppUi* aAppUi,
    MUIMode* aUIMode )
    {
    CImagePrintSettingsView* self = 
        new ( ELeave ) CImagePrintSettingsView( aPrintJob, aAppUi, aUIMode  );    
    CleanupStack::PushL( self );
    self->ConstructL( aSettings);

    return self;
    }

// 2nd phase constructor
void CImagePrintSettingsView::ConstructL(
    MPrintSettings* aSettings )
    {    
       
    iSettings = aSettings;    
    TUint vendor = iSettings->GetCurrentPrinterVendor();
    iPrintDlgManager = CIMGPPrintDlgManager::NewL( 1,
        iPrintJob, aSettings->NumOfPagesL(), 
        aSettings->IsMMC(), vendor );
    BaseConstructL( R_SETTINGS_VIEW );    
    }

// Default constructor
CImagePrintSettingsView::CImagePrintSettingsView( 
    MPrintJob* aPrintJob, CImagePrintAppUi* aAppUi, MUIMode* aUIMode ) 
      : CAknView(),
        iPrintJob( aPrintJob ),
        iAppUi( aAppUi ),
        iUIMode(aUIMode)
    {
    }

// Default destructor
CImagePrintSettingsView::~CImagePrintSettingsView()
    {
    delete iPrintDlgManager;
    DestroyContainer();
    }

// Returns IDs
TUid CImagePrintSettingsView::Id() const
    {
    return KImageSettingsViewId;
    }

// Handles commands coming to this viwe
void CImagePrintSettingsView::HandleCommandL(
    TInt aCommand )
    {
    switch ( aCommand )
        {
        case ESettingsChangeItem:
            {
            iContainer->EditItemL();
            break;
            }

        case ESettingsPrint:
            {
            StartPrintingL();
            break;
            }

        case ESettingsExit:  
            {
            // Exit caller also
            AppUi()->HandleCommandL( EEikCmdExit );          
            break;
            }

        case EAknSoftkeyBack:
            {
            // Exit to gallery
            AppUi()->HandleCommandL( EAknCmdExit );
            break;
            }

        case ECmdPrintHelp:
            {
            iAppUi->LaunchHelpApplicationL( KImagePrintAppHelpSettings );
            break;
            }

        default:            
            AppUi()->HandleCommandL( aCommand );
            break;
        }
    }

// Activates the view, creates the control container
void CImagePrintSettingsView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/)
    {
    // Change the status pane text
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    sp->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( 
        sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) ); 
    tp->SetTextL( iEikonEnv->AllocReadResourceLC( 
        R_TXT_IMAGEPRINT_TITLE_PRINTSETTINGS )->Des() );
    CleanupStack::PopAndDestroy();  // tp
    sp->DrawNow();
    
    if ( !iContainer )
        {
        iContainer = CImagePrintSettingsContainer::NewL( 
            iSettings, this, AppUi()->ClientRect(), iAppUi, iUIMode );        
        AppUi()->AddToStackL( *this, iContainer );
		}      
	}

// Deactivates view, deletes the control
void CImagePrintSettingsView::DoDeactivate()
    {
    DestroyContainer();
    }

// Destroys control container
void CImagePrintSettingsView::DestroyContainer()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    iContainer = 0;
    }

// Starts the printing sequence
void CImagePrintSettingsView::StartPrintingL()
    {
    
    // First of all check that WLAN is on, in case WLAN is current printing protocol.
    if ( iSettings->GetCurrentPrinterProtocol() == MDiscoveryObserver::EWLAN )
    	{
		HandlePossibleLinkDisconnectionL();   
    	}
    
    TUint vendor = iSettings->GetCurrentPrinterVendor();
    
    // Default number of copies value = 1, so keep it, if iSettings doesn't return deviant value.
    TInt copiesBuffer = 1;
    iSettings->NumberOfCopies( copiesBuffer, 0 );

    delete iPrintDlgManager;
    iPrintDlgManager = 0;
    iPrintDlgManager = CIMGPPrintDlgManager::NewL( copiesBuffer,
    iPrintJob, iSettings->NumOfPagesL(), 
    iSettings->IsMMC(), vendor );
    
    CIFFactory& factory = iAppUi->GetDocument()->Engine()->InterfaceFactory();
    factory.SettingsIF()->UpdateNumberOfCopiesToServer();
     
    iPrintDlgManager->StartPrintingL();
    }

void CImagePrintSettingsView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    // Hide command <exit> if application is started from active idle 
    // and it is in landscape orientation
    if( aResourceId == R_PROFILE_LIST_VIEW_MENU ) 
        {
		// If there is no default printer selected, remove print option from menu to avoid incorrect print sequency. 
        TInt currentProtocol = iSettings->GetCurrentPrinterProtocol();
        if ( currentProtocol == MDiscoveryObserver::ENOPRINTER )
        	{
        	aMenuPane->DeleteMenuItem( ESettingsPrint );
        	}          
        }
    }

void CImagePrintSettingsView::HandleForegroundEventL( TBool aForeground )
    {
    if ( aForeground )
        {
        ViewScreenDeviceChangedL();
        }
    }

void CImagePrintSettingsView::HandlePossibleLinkDisconnectionL()	
	{
	TInt wlanState;
	RProperty::Get( KPSUidWlan, KPSWlanIndicator, wlanState );

	if ( wlanState == EPSWlanIndicatorNone )
      	{         
      	// Show connection to printer lost -note and exit application.
		HBufC* buf = StringLoader::LoadLC( R_NOTE_DISCONNECT_PRINT_ERROR );
		CAknErrorNote* errornote = new CAknErrorNote( ETrue );       
		errornote->ExecuteLD( *buf );	        	
		CleanupStack::PopAndDestroy( buf );
	    	
		AppUi()->HandleCommandL( EEikCmdExit );  
		}
	}


//  End of File
