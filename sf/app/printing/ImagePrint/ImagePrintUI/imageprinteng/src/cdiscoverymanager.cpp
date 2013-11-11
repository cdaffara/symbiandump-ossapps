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


#include <eikenv.h>
#include <bluetooth/hci/hcierrors.h>

#include "tprinter.h"
#include "cdiscoverymanager.h"
#include "ciffactory.h"
#include "cprintjobmanager.h"
#include "csettingsmanager.h"
#include "imageprint.h"
#include "cimageprint.h"
#include "clog.h"

const TInt KMMCID( 1 );

//  CONSTRUCTION
CDiscoveryManager* CDiscoveryManager::NewL(
    CIFFactory* aFactory )
    {
    CDiscoveryManager* self = CDiscoveryManager::NewLC( aFactory );
    CleanupStack::Pop();    // self

    return self;
    }

CDiscoveryManager* CDiscoveryManager::NewLC(
    CIFFactory* aFactory )
    {
    CDiscoveryManager* self =
        new ( ELeave ) CDiscoveryManager( aFactory );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// Constructor
CDiscoveryManager::CDiscoveryManager(
    CIFFactory* aFactory ) :
      iFactory( aFactory )
    {
    }

// Destruction
CDiscoveryManager::~CDiscoveryManager()
    {
    }

// 2nd phase constructor
void CDiscoveryManager::ConstructL()
    {
    }

// Returns supported protocols
TUint CDiscoveryManager::SupportedProtocols()
    {
    return iFactory->Engine()->SupportedProtocols();
    }

// Initiates device discovery
void CDiscoveryManager::StartDeviceDiscoveryL(
    MDiscoveryObserver* aNotifier,
    TUint aProtocol )
    {
    TInt discoveryError( KErrNone );
    LOG("CDiscoveryManager::StartDeviceDiscoveryL BEGIN");
    iMMCUid = -1;
    iCancelling = EFalse;
    iOnGoingDiscovery = ETrue;
    iDiscoveryObserver = aNotifier;
    TRAP_IGNORE( discoveryError = iFactory->Engine()->StartDiscoveryL( *this, aProtocol ) );
    LOG1("CDiscoveryManager::StartDeviceDiscoveryL END, error = %d", discoveryError);
	User::LeaveIfError( discoveryError );
    }

// Cancels the discovery notification
void CDiscoveryManager::CancelDeviceDiscoveryL()
    {
    if ( iOnGoingDiscovery )
        {
        User::LeaveIfError( iFactory->Engine()->CancelDiscovery() );
        iCancelling = ETrue;
        iOnGoingDiscovery = EFalse;
        }
    }

// Chooses the printer
TInt CDiscoveryManager::ChoosePrinterL(
    TInt aPrinterUid )
    {
	LOG("CDiscoveryManager::ChoosePrinterL BEGIN");
	TInt err = 0;

    // Create print job
    CPrintJobManager* printJobManager =
        static_cast<CPrintJobManager*>( iFactory->PrintJobIF() );
    iOnGoingDiscovery = EFalse;

    err = printJobManager->CreatePrintJobL( aPrinterUid );
    LOG2("CDiscoveryManager::ChoosePrinterL printjob ret: %d with printer uid: %d", err, aPrinterUid );
	if ( err == KErrNone )
		{
		if( aPrinterUid == KMMCID )
		    {
		    iMMCUid = KMMCID;    
		    }		

		// Do initialization of the settings manager once print job is
        // created and the settings can be fetched
        CSettingsManager* settings =
            static_cast<CSettingsManager*>( iFactory->SettingsIF() );
        settings->InitSettingsL( aPrinterUid, iMMCUid );

        iPrinterUid = aPrinterUid;
		}		
    
	LOG("CDiscoveryManager::ChoosePrinterL END");
	return err;
    }

// Called when the server finds the printer
void CDiscoveryManager::FoundPrinterL(
    const TPrinter& aPrinterInfo )
    {
    HBufC* name = HBufC::NewLC( aPrinterInfo.iDisplayName.Length() );
    name->Des().Copy( aPrinterInfo.iDisplayName );

    MDiscoveryObserver::TPrinterType type;
    switch ( aPrinterInfo.iProtocol )
        {
        case KImagePrint_PrinterProtocol_BPP:
            type = MDiscoveryObserver::EBPP;
            break;
        case KImagePrint_PrinterProtocol_DPOF:
            type = MDiscoveryObserver::EMMC;
            iMMCUid = aPrinterInfo.iPrinterID;
            break;
        case KImagePrint_PrinterProtocol_PictBridge:
            type = MDiscoveryObserver::EUSB;
            break;

        case KImagePrint_PrinterProtocol_OPP_Printer: //fallthrough
        case KImagePrint_PrinterProtocol_OPP_PC:
            type = MDiscoveryObserver::EOPP;
            break;
        case KImagePrint_PrinterProtocol_UPnP:
        	type = MDiscoveryObserver::EWLAN;
            break;
        default:
            type = MDiscoveryObserver::EBPP;
            break;
        }

    if( iOnGoingDiscovery != EFalse )
        {
        iDiscoveryObserver->NotifyNewPrintDeviceL(
            name, aPrinterInfo.iPrinterID,
            aPrinterInfo.iProperties & TPrinter::Cached,
            type, aPrinterInfo.iVendor );
        }
    CleanupStack::PopAndDestroy();  // name
    }

// Called to inform the status
void CDiscoveryManager::DiscoveryStatusL(
    TInt aStatus,
    TInt aErrorCode,
    TInt /*aErrorStringCode*/ )
    {
    // When Bluetooth discovery is finished, but Bluetooth is not enabled pass error code to UI.   
    if ( aErrorCode == ENoConnection )
    	{
    	if (iDiscoveryObserver)
            {
            iDiscoveryObserver->DiscoveryError( aErrorCode ); 
            }
        }
	if ( iCancelling )
        {
        // Exection continues in CancelDiscoveryL()
        if ( iDiscoveryObserver )
           {
           iDiscoveryObserver->DeviceDiscoveryCompleted();
		   }
        }
    else
        {
        if ( iOnGoingDiscovery )
            {
            if ( aStatus == EDiscovering )
                {
                iOnGoingDiscovery = EFalse;
                if ( iDiscoveryObserver )
                    {
                    iDiscoveryObserver->DeviceDiscoveryCompleted();
                    }
                }
            else if ( aStatus == EDoneDiscovery )
                {
                iOnGoingDiscovery = EFalse;
                if ( iDiscoveryObserver )
                    {
                    iDiscoveryObserver->DeviceDiscoveryCompleted();
                    }
                }
            else if ( aStatus == ECancellingDiscovery )
                {
                if ( iDiscoveryObserver )
                    {
                    iDiscoveryObserver->DiscoveryError( aErrorCode );
                    }
                }
            else
                {
                User::Leave( KErrUnknown );
                }
            }
        }
    }

// Removes cached printer from the list
void CDiscoveryManager::RemovePrinterL( const TPrinter& aPrinterInfo )
    {
    if( iOnGoingDiscovery != EFalse )
        {
        iDiscoveryObserver->RemoveCachedPrinterL( aPrinterInfo.iPrinterID );
        }
    }

TInt CDiscoveryManager::IsPictBridgeMode()
    {
    LOG("CDiscoveryManager::IsPictBridgeMode() START");
    TInt retValue = -999;
    retValue = iFactory->ImagePrintClient().IsPictBridgeMode();
    LOG1("IsPictBridgeMode() returned: %d", retValue);
    LOG("CDiscoveryManager::IsPictBridgeMode() END");
    return retValue;
    }
    
void CDiscoveryManager::RemoveDiscoveryObserver()
	{
	iOnGoingDiscovery = EFalse;
	iDiscoveryObserver = 0;
	}

//  End of File
