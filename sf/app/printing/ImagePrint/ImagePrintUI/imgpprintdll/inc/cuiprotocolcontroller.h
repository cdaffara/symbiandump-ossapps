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


#ifndef CUIPROTOCOLCONTROLLER_H
#define CUIPROTOCOLCONTROLLER_H

#include <e32def.h>
#include <e32base.h>
#include <AknProgressDialog.h>
#include <usbman.h>

#include "cdiscoverydlgmanager.h"
#include "mprotocolobserver.h"
#include "midleprinteventobserver.h"
#include "imageprint.h"

class CIFFactory;
class MIdlePrintEventRegisterIF;
class MDiscovery;
class MPrintSettings;
class CCustomNoteDialog;

const TUint KAllBTProtocols =	KImagePrint_PrinterProtocol_BPP |
								KImagePrint_PrinterProtocol_BIP |
								KImagePrint_PrinterProtocol_OPP_Printer |
								KImagePrint_PrinterProtocol_OPP_PC;

//  CLASS DEFINITION
/**
 *
 * Class searches printers(protocols) and creates print job for
 * selected printer. Class also works as an idle observer.
 *
 */
class CUIProtocolController:
    public CBase,
    public MIdlePrintEventObserver,
    public MProgressDialogCallback
    {
    public: // Constructors and destructors

        /**
         *  Standard two-phase constructors
         *
         *  @param aProtocolObserver Protocol observer
         *  @param aIFFactory Interface factory class
         *
         *  @return Initialized instance of the class
         */
        IMPORT_C static CUIProtocolController* NewL( MProtocolObserver* aProtocolObserver,
                                            CIFFactory& aIFFactory );
        static CUIProtocolController* NewLC( MProtocolObserver* aProtocolObserver,
                                             CIFFactory& aIFFactory );
        /**
         * Destructor.
         */
        virtual ~CUIProtocolController();

    private:

        /**
         * Default constructor.
         *  @param aProtocolObserver Protocol observer
         *  @param aIFFactory Interface factory class
         */
        CUIProtocolController( MProtocolObserver* aProtocolObserver,
                               CIFFactory& aIFFactory );

     	/**
         *  Second phase constructor
         *
         *  @param aIFFactory Interface factory class
         */
     	void ConstructL( CIFFactory& aIFFactory );

    public: // Methods derived from MIdlePrintEventObserver
        void UsbPrinterDisconnected();
        void UsbPrinterConnected();
    	void UsbStateChanged( TImgPrintUSBState aState, TInt aError );
		void ReadyToContinue();
		void AsynchronousServerLeave( TInt aError );

    public: // Methods derived from MProgressDialogCallback
        void DialogDismissedL(TInt /*aButtonId*/ );

    public: // New methods

        /**
         * Searches default printer and takes it into use. If default
         * printer is not found full searcg is performed.
         *
         * @return ETrue if printer is succesfully taken into use
         *         EFalse in all other cases.
         */
        IMPORT_C TBool SearchDefaultPrinterL();

        /**
         * Searches printer with a given protocol, takes it into use
         * and sets the given protocol as a default one.
         *
         * @param aProtocol Protocol to be searched for.
         * @return ETrue if printer is succesfully taken into use
         *         EFalse in all other cases.
         */
        IMPORT_C TBool ChangeDefaultPrinterL( TInt aProtocol );

        /**
         * Searches for all available printers.
         * PrintJob is created after discovery with the selected
         * printer.
         *
         * @return ETrue if printer has been taken into use.
         *         EFalse in all other cases.
         */
        TBool FullSearchL();

    private: // New methods

    	TBool SetDefaultPrinterL( TInt aDefaultProtocol );
    	
        /**
         * Searches the usb printer. Method is synchronous.
         * PrintJob is created after discovery has been done.
         *
         * @return ETrue if PictBridge printer has been taken into use.
         *         EFalse in all other cases.
         */
        TBool SearchUSBPrinterL();

        /**
         * Takes the default BT printer into use.
         * PrintJob is created without discovery.
         *
         * @return ETrue if default BT printer has been taken into use.
         *         EFalse in all other cases.
         */
        TBool SearchDefaultBTPrinterL();

        /**
         * Searches for all available BT printers.
         * PrintJob is created after discovery with the selected
         * printer.
         *
         * @return ETrue if BT printer has been taken into use.
         *         EFalse in all other cases.
         */
        TBool SearchBTPrinterL();

        /**
         * Searches for all available UPnP(WLAN) printers.
         * PrintJob is created after discovery with the selected
         * printer.
         *
         * @return ETrue if UPnP printer has been taken into use.
         *         EFalse in all other cases.
         */
        TBool SearchUPnPPrinterL();

        /**
         * Takes the default UPnP(WLAN) printer into use
         * via hidden discovery
         *
         * @return ETrue if default UPnP(WLAN) printer has been taken into use.
         *         EFalse in all other cases.
         */
        TBool SearchDefaultUPnPPrinterL();

        /**
         * Takes the MMC into use.
         * PrintJob is created without discovery.
         *
         * @return ETrue if MMC has been taken into use.
         *         EFalse in all other cases.
         */
        TBool SearchMMCPrinterL();

        /**
         * Is Usb cable connected AND in PictBridge mode
         * @return ETrue if usb connected and in right mode, EFalse if not.
         */
        TBool IsUsbCableConnected();

        /**
         * Displays Connect USB cable -note.
         */
        void ShowConnectUsbCableNoteL();

    private:    // Enumerations

        // Different states of device discovery
        enum TClassState
            {
            ENormalState,
            EDiscoveryOngoing,
            EUSBDisconnectedDuringDiscovery
            };

    private: // Data

    	// Observer
    	MProtocolObserver* iProtocolObserver;

    	// Interface for registering idle observer
    	MIdlePrintEventRegisterIF* iIdleEventRegister;

    	// Interface for device discovery
    	MDiscovery* iDiscovery;

    	// Interface for settings
        MPrintSettings* iSettings;

        // Interface factory
        CIFFactory* iIFFactory;

        // USB connect note
        CCustomNoteDialog* iUSBConnectionWaitNote;

        // Usb manager
		RUsb iUsbManager;

		// Device state
        TUsbDeviceState iDeviceState;

        // Class state related to usb and discovery
        TInt iClassState;
        
        // Discovery dialog manager in case when search all protocols
        CDiscoveryDlgManager* iDiscoveryDlgManager;
        
        // tells that usb is connect during all protocols search. 
        TBool iStartUsb;
        
        // indicates that usb connection has terminated at least once
        TBool iWasUsbDisconnected;
    };

#endif  // CUIPROTOCOLCONTROLLER_H

//  End of File
