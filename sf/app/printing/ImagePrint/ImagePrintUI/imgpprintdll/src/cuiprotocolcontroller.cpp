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
#include <AknQueryDialog.h>
#include <imageprintapp.rsg>

#include "cuiprotocolcontroller.h"
#include "muimodeobserver.h"
#include "ciffactory.h"
#include "midleprinteventregisterif.h"
#include "mdiscovery.h"
#include "mprintjob.h"
#include "mprintsettings.h"
#include "mdiscoveryobserver.h"
#include "tprinter.h"
#include "imgpprintapputil.h"
#include "ccustomnotedialog.h"
#include "clog.h"

const TInt KMMCID( 1 );

// CONSTRUCTOR
EXPORT_C CUIProtocolController* CUIProtocolController::NewL(
                       MProtocolObserver* aProtocolObserver,
                       CIFFactory& aIFFactory )
    {
    CUIProtocolController* self = NewLC( aProtocolObserver,
                                         aIFFactory );
    CleanupStack::Pop( self );
    return self;
    }

CUIProtocolController* CUIProtocolController::NewLC(
                       MProtocolObserver* aProtocolObserver,
                       CIFFactory& aIFFactory )
    {
    CUIProtocolController* self = new ( ELeave ) CUIProtocolController(
                                                 aProtocolObserver,
                                                 aIFFactory );
    CleanupStack::PushL( self );
    self->ConstructL( aIFFactory );
    return self;
    }

// Default constructor
CUIProtocolController::CUIProtocolController(
                       MProtocolObserver* aProtocolObserver,
                       CIFFactory& aIFFactory ) :
                       iProtocolObserver(aProtocolObserver)
    {
    iIFFactory = &aIFFactory;
    iClassState = ENormalState;
    iDiscoveryDlgManager = NULL;
    iStartUsb = EFalse;
    iWasUsbDisconnected = EFalse;
    }

// 2nd phase constructor
void CUIProtocolController::ConstructL( CIFFactory& aIFFactory )
    {
    iIdleEventRegister = aIFFactory.IdlePrintEventRegisterIF();

    // Register to receive idle event from engine
    iIdleEventRegister->RegisterObserver( this );

    iSettings = aIFFactory.SettingsIF();
    iDiscovery = aIFFactory.DiscoveryIF();
    }

// Destructor
CUIProtocolController::~CUIProtocolController()
    {
    if(iUSBConnectionWaitNote)
    {
      delete iUSBConnectionWaitNote;	
      iUSBConnectionWaitNote = NULL;
    }
    
    // Unregister from engine
    if ( iIdleEventRegister )
    	{
    	iIdleEventRegister->UnRegisterObserver( this );  	
    	}
	}


EXPORT_C TBool CUIProtocolController::SearchDefaultPrinterL()
    {
    LOG("CUIProtocolController::SearchDefaultPrinterL START");
    TBool retValue = EFalse;
    TInt defaultProtocol = -1;
	TBool usbOnlyMode = EFalse;
	
    // Get supported protocols from the engine
	TInt supportedProtocols = iIFFactory->DiscoveryIF()->SupportedProtocols();

	// If PictBridge is only supported protocol, set it as default
	if( !(supportedProtocols & KImagePrint_PrinterProtocol_BPP)  && 
    	!(supportedProtocols & KImagePrint_PrinterProtocol_DPOF) && 
    	!(supportedProtocols & KImagePrint_PrinterProtocol_UPnP) && 
     	(supportedProtocols & KImagePrint_PrinterProtocol_PictBridge) )
		{
		LOG("CUIProtocolController::SearchDefaultPrinterL: USB only mode detected");
		defaultProtocol = MDiscoveryObserver::EUSB;
		usbOnlyMode = ETrue;
		}
    // If usb is connected and PictBridge selected it will be used
    else if( iDiscovery->IsPictBridgeMode() > 0 )
        {
        defaultProtocol = MDiscoveryObserver::EUSB;
        }
    else // check default printer.
        {
        LOG("iSettings->GetDefaultPrinterProtocol()");
        defaultProtocol = iSettings->GetDefaultPrinterProtocol();
        LOG1("defaultProtocol = %d",defaultProtocol);
        }

   retValue = SetDefaultPrinterL( defaultProtocol );
    
   // If there is no default printer or default printer not available
   // start settings view with no printer information (but not in usb only mode)
   if( retValue == EFalse && usbOnlyMode == EFalse )
        {
        retValue = ETrue;
        iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::ENOPRINTER );
        iSettings->SetDefaultPrinterProtocol( MDiscoveryObserver::ENOPRINTER );
        HBufC* noPrinterText = StringLoader::LoadLC( R_PRINT_NO_PRINTER ); 
        iSettings->SetCurrentPrinterName( *noPrinterText );
        iSettings->SetDefaultPrinterNameL( *noPrinterText );
        CleanupStack::PopAndDestroy ( noPrinterText ); //noPrinterText        
        }

   if( iStartUsb )
        {
        iStartUsb = EFalse;
        defaultProtocol = MDiscoveryObserver::EUSB;
        retValue = SearchUSBPrinterL();
        if( retValue )
            {
            iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::EUSB );
               
            // In USB case it also must be checked whether default printer is
            // set at this point, and if not, set USB as default. This is because
            // in normal cases usb overrides the default protocol without 
            // changing itself to default.
            TInt realDefault = iSettings->GetDefaultPrinterProtocol();
            if ( realDefault == KErrNotFound )
                {
                iSettings->SetDefaultPrinterProtocol( MDiscoveryObserver::EUSB );
                iSettings->SetDefaultPrinterNameL( *iSettings->GetCurrentPrinterName() );                	
                }
            }
        }

    LOG("CUIProtocolController::SearchDefaultPrinterL END");
    return retValue;
    }

TBool CUIProtocolController::SetDefaultPrinterL( TInt aDefaultProtocol )
	{
	 TBool retValue = EFalse;
	 switch( aDefaultProtocol )
	        {
	        case MDiscoveryObserver::EUSB:
	            {
	            retValue = SearchUSBPrinterL();
	            if( retValue )
	                {
	                iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::EUSB );
	                iSettings->SetDefaultPrinterProtocol( MDiscoveryObserver::EUSB );			
	                iSettings->SetDefaultPrinterNameL( *iSettings->GetCurrentPrinterName() );    
	                }
	            return retValue;
	            }
	        case MDiscoveryObserver::EBPP:
	            {
	            retValue = SearchDefaultBTPrinterL();
	            if( retValue )
	                {
	                iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::EBPP );
	                }
	            return retValue;
	            }
	        case MDiscoveryObserver::EOPP:            
	            {
	            retValue = SearchDefaultBTPrinterL();
	            if( retValue )
	                {
	                iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::EOPP );
	                }
	            return retValue;
	            }
	        case MDiscoveryObserver::EMMC:
	            {
	           retValue = SearchMMCPrinterL();
	            if( retValue )
	                {
	                iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::EMMC );
	                }
	            return retValue;
	            }
	        case MDiscoveryObserver::EWLAN:
	            {
	            retValue = SearchDefaultUPnPPrinterL();
	            if( retValue )
	                {
	                iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::EWLAN );
	                }
	            return retValue;
	            }

	        default:
	            {
	            return retValue;
	            }
	        }
	}

// Changes default printer
EXPORT_C TBool CUIProtocolController::ChangeDefaultPrinterL( TInt aProtocol )
    {
    LOG("CUIProtocolController::ChangeDefaultPrinterL START");
    TBool retValue;
	TInt previousDefaultProtocol = iSettings->GetDefaultPrinterProtocol();
	iSettings->SetDefaultPrinterProtocol( KErrNotFound );
	iClassState = EDiscoveryOngoing;
	
    switch( aProtocol )
        {
        case MDiscoveryObserver::EUSB:
            {
            retValue = SearchUSBPrinterL();
            if( retValue )
                {
                iSettings->SetDefaultPrinterProtocol( MDiscoveryObserver::EUSB );
                iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::EUSB );
                iSettings->SetDefaultPrinterNameL( *iSettings->GetCurrentPrinterName() );
                }
            break;
            }

        case MDiscoveryObserver::EBPP:
            {
            retValue = SearchBTPrinterL();
            if( retValue )
                {
                iSettings->SetDefaultPrinterProtocol( MDiscoveryObserver::EBPP );
                iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::EBPP );
                }
            break;
            }

        case MDiscoveryObserver::EMMC:
            {
            retValue = SearchMMCPrinterL();
            if( retValue )
                {
                iSettings->SetDefaultPrinterProtocol( MDiscoveryObserver::EMMC );
                iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::EMMC );
                }
            break;
            }

        case MDiscoveryObserver::EWLAN:
            {
            retValue = SearchUPnPPrinterL();
            if( retValue )
                {
                iSettings->SetDefaultPrinterProtocol( MDiscoveryObserver::EWLAN );
                iSettings->SetCurrentPrinterProtocol( MDiscoveryObserver::EWLAN );
                }

            break;
            }

        default:
            {
            // protocol not supported
            retValue = EFalse;
            break;
            }
        }

	if ( retValue == EFalse )
		{
		iSettings->SetDefaultPrinterProtocol( previousDefaultProtocol );
	    LOG1("CUIProtocolController::ChangeDefaultPrinterL iClassState == %d", iClassState);
		if ( iClassState == EUSBDisconnectedDuringDiscovery )
			{
			iClassState = ENormalState;
			UsbPrinterDisconnected();
			}
		else if( MDiscoveryObserver::EUSB == previousDefaultProtocol && iWasUsbDisconnected )
			{
		    LOG("CUIProtocolController::ChangeDefaultPrinterL nothing selected, but usb previously disconnected, discoverying");
			SearchDefaultPrinterL();
			}
		}
	iClassState = ENormalState;
    LOG("CUIProtocolController::ChangeDefaultPrinterL END");
    return retValue;
    }

TBool CUIProtocolController::SearchUSBPrinterL()
    {
    LOG("CUIProtocolController::SearchUSBPrinterL START");
    TBool retValue = EFalse;

    // if the cable is not physically connected, show connect usb cable note
    if( !IsUsbCableConnected() )
        {
        ShowConnectUsbCableNoteL();
        
        // Active scheduler is used here to make this method synchronous
        CActiveScheduler::Start();
        }
    // If usb cable is connected and pictbridge mode
    if( iDiscovery->IsPictBridgeMode() > 0 )
        {
        CDiscoveryDlgManager* discoveryDlgManager =
                              CDiscoveryDlgManager::NewLC( *iIFFactory );
        retValue = discoveryDlgManager->StartDlgManagerL(
                            KImagePrint_PrinterProtocol_PictBridge, ETrue );
        CleanupStack::PopAndDestroy( discoveryDlgManager );
        discoveryDlgManager = NULL;
        }
	LOG("CUIProtocolController::SearchUSBPrinterL END");
    return retValue;
    }

// Takes BT printer in use
TBool CUIProtocolController::SearchDefaultBTPrinterL()
    {
    LOG("CUIProtocolController::SearchDefaultBTPrinterL START");
    TBool retValue = EFalse;

    CDiscoveryDlgManager* discoveryDlgManager =
                          CDiscoveryDlgManager::NewLC( *iIFFactory );
    retValue = discoveryDlgManager->StartDlgManagerL( KAllBTProtocols, ETrue );
    CleanupStack::PopAndDestroy( discoveryDlgManager );
    discoveryDlgManager = NULL;
	LOG("CUIProtocolController::SearchDefaultBTPrinterL END");
    return retValue;
    }

// Takes mmc in use
TBool CUIProtocolController::SearchMMCPrinterL()
    {
    LOG("CUIProtocolController::SearchMMCPrinterL START");
    TBool retValue = EFalse;

    TInt err = iDiscovery->ChoosePrinterL( KMMCID );
    if( err == KErrNone)
        {
        LOG("Print job done succesfully with MMC DEFAULT PRINTER");
        // Set current printer name to be mmc
        HBufC* mmcText = StringLoader::LoadLC
                                  ( R_QTN_PRINT_MEMORY_CARD );
        iSettings->SetCurrentPrinterName( *mmcText );
        // Set vendor to none
        iSettings->SetCurrentPrinterVendor( TPrinter::EVendorNone );

        CleanupStack::PopAndDestroy(); //mmmcText
        retValue = ETrue;
        }
    LOG("CUIProtocolController::SearchMMCPrinterL END");
    return retValue;
    }

// Searches WLAN printers
TBool CUIProtocolController::SearchUPnPPrinterL()
    {
    LOG("CUIProtocolController::SearchUPnPPrinterL START");
    TBool retValue = EFalse;
    CDiscoveryDlgManager* discoveryDlgManager =
                          CDiscoveryDlgManager::NewLC( *iIFFactory );
    retValue = discoveryDlgManager->StartDlgManagerL( KImagePrint_PrinterProtocol_UPnP );
    CleanupStack::PopAndDestroy( discoveryDlgManager );
    discoveryDlgManager = NULL;
    LOG("CUIProtocolController::SearchUPnPPrinterL END");
    return retValue;
    }

// Takes WLAN printer in use
TBool CUIProtocolController::SearchDefaultUPnPPrinterL()
    {
    LOG("CUIProtocolController::SearchDefaultUPnPPrinterL START");
    TBool retValue = EFalse;

    CDiscoveryDlgManager* discoveryDlgManager =
                          CDiscoveryDlgManager::NewLC( *iIFFactory );
    retValue = discoveryDlgManager->StartDlgManagerL( KImagePrint_PrinterProtocol_UPnP, ETrue );
    CleanupStack::PopAndDestroy( discoveryDlgManager );
    discoveryDlgManager = NULL;
	LOG("CUIProtocolController::SearchDefaultUPnPPrinterL END");
    return retValue;
    }

TBool CUIProtocolController::SearchBTPrinterL()
    {
    LOG("CUIProtocolController::SearchBTPrinterL START");
    TBool retValue = EFalse;
    CDiscoveryDlgManager* discoveryDlgManager =
                          CDiscoveryDlgManager::NewLC( *iIFFactory );
    retValue = discoveryDlgManager->StartDlgManagerL( KAllBTProtocols );
    CleanupStack::PopAndDestroy( discoveryDlgManager );
    discoveryDlgManager = NULL;
    LOG("CUIProtocolController::SearchBTPrinterL END");
    return retValue;
    }

// Searches all supported protocols
TBool CUIProtocolController::FullSearchL()
    {
    LOG("CUIProtocolController::FullSearchL START");
    TBool retValue = EFalse;
    iDiscoveryDlgManager = CDiscoveryDlgManager::NewL( *iIFFactory );
    LOG("CUIProtocolController::FullSearchL before Fullsearch");
    retValue = iDiscoveryDlgManager->StartDlgManagerL();
    LOG("CUIProtocolController::FullSearchL after FullSearch");
    delete iDiscoveryDlgManager;
    iDiscoveryDlgManager = NULL;
    LOG("CUIProtocolController::FullSearchL END");
    return retValue;
    }

// Is the usb cable connected to phone or not
TBool CUIProtocolController::IsUsbCableConnected()
    {
    LOG("CUIProtocolController::IsUsbCableConnected START");
    TInt err = iUsbManager.Connect();
    if ( err == KErrNone )
    	{
	    iUsbManager.GetDeviceState( iDeviceState );
	    iUsbManager.Close();	
    	}
    else
    	{
	    LOG("CUIProtocolController::IsUsbCableConnected iUsbManager.Connect() FAIL");	
    	}
    LOG("CUIProtocolController::IsUsbCableConnected END");
    return ( iDeviceState != EUsbDeviceStateUndefined );
    }

void CUIProtocolController::ShowConnectUsbCableNoteL()
    {
    LOG("CUIProtocolController::ShowConnectUsbCableNote START");
    iUSBConnectionWaitNote = new (ELeave) CCustomNoteDialog(
	    reinterpret_cast< CEikDialog** > (&iUSBConnectionWaitNote) );
    iUSBConnectionWaitNote->SetCallback( this );
    iUSBConnectionWaitNote->ExecuteLD(R_CONNECT_USB);
    LOG("CUIProtocolController::ShowConnectUsbCableNote END");
    }

// MProgressDialogCallback method
void CUIProtocolController::DialogDismissedL(TInt /*aButtonId*/ )
    {
    LOG("CUIProtocolController::DialogDismissedL START");
    CActiveScheduler::Stop();
    LOG("CUIProtocolController::DialogDismissedL END");
    }

//Idle observer method
void CUIProtocolController::UsbPrinterDisconnected()
    {
    LOG("CUIProtocolController::UsbPrinterDisconnected START");
    if( iSettings->GetCurrentPrinterProtocol() == MDiscoveryObserver::EUSB )
        {
        LOG1("CUIProtocolController::UsbPrinterConnected iClassState is %d", iClassState);
        if ( iClassState == EDiscoveryOngoing || iClassState == EUSBDisconnectedDuringDiscovery )
        	{
        	iClassState = EUSBDisconnectedDuringDiscovery;
        	iWasUsbDisconnected = ETrue;
            LOG("CUIProtocolController::UsbPrinterConnected iClassState is now EUSBDisconnectedDuringDiscovery");
        	}
        else
        	{
	        iProtocolObserver->UsbCableDisconnected();        	
        	}
        }
    LOG("CUIProtocolController::UsbPrinterDisconnected END");
    }

//Idle observer method
void CUIProtocolController::UsbPrinterConnected()
    {
    LOG("CUIProtocolController::UsbPrinterConnected START");
    if( EUSBDisconnectedDuringDiscovery == iClassState )
    	{
    	iClassState = EDiscoveryOngoing;
        LOG("CUIProtocolController::UsbPrinterConnected iClassState is now EDiscoveryOngoing");
    	}

    if( iUSBConnectionWaitNote )    // Remove wait note and start discovery
        {
        delete iUSBConnectionWaitNote;
        iUSBConnectionWaitNote = NULL;
        }
    else
        {
        if(iDiscoveryDlgManager)
            {
            iStartUsb = ETrue;
            iDiscoveryDlgManager->CancelDiscovery();
            }
        }
    LOG("CUIProtocolController::UsbPrinterConnected END");
    }

//Idle observer method
void CUIProtocolController::UsbStateChanged( TImgPrintUSBState aState, TInt aError )
    {
    LOG("CUIProtocolController::UsbStateChanged START");
    if ( aState == ENOUSBEvent )
    	{
	    TRAP_IGNORE( IMGPPrintAppUtil::ShowErrorMsgL( aError ) );	
    	}
    LOG("CUIProtocolController::UsbStateChanged END");
    }

//Idle observer method
void CUIProtocolController::ReadyToContinue()
    {
    LOG("CUIProtocolController::ReadyToContinue START");
    LOG("CUIProtocolController::ReadyToContinue END");
    }

void CUIProtocolController::AsynchronousServerLeave( TInt /*aError*/ )
	{
	LOG("CUIProtocolController::AsynchronousServerLeave START");

	// Cancel discovery in case of server leave. No can do if Cancel leaves.
	LOG("CUIProtocolController::AsynchronousServerLeave CancelDeviceDiscoveryL()");
	TRAP_IGNORE( iIFFactory->DiscoveryIF()->CancelDeviceDiscoveryL() );

	// Cancel printing in case of server leave. No can do if Cancel leaves.
	TRAP_IGNORE( iIFFactory->PrintJobIF()->CancelL() );
    LOG("CUIProtocolController::AsynchronousServerLeave END");
	}

//  End of File
