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


#ifndef CDISCOVERYDLGMANAGER_H
#define CDISCOVERYDLGMANAGER_H

#include <e32std.h>
#include <aknPopup.h>
#include <aknlists.h>
#include <f32file.h>
#include <AknProgressDialog.h>

#include "mdiscoveryobserver.h"
#include "mdiscovery.h"

class CAknWaitDialog;
class CImagePrintAppUi;
class MPrintSettings;
class CIFFactory;

// CONSTANTS
const TInt KMaxPrinterNameLength( 84 );

//  CLASS DEFINITION
/**
 *
 * CDiscoveryDlgManager is a class that is responsible of displaying the pop-up
 * list at the beginning containing the available medias/devices to print to.
 *
 */
class CDiscoveryDlgManager
    : public CBase,
      public MDiscoveryObserver,
	  public MProgressDialogCallback
    {
    public:     // Constructors and destructors

        /**
         *  Two-phase constructor
         *
         *  @param aDiscovery   Discovery IF
         *
         *  @return Initialized instance of the class
         */
		static CDiscoveryDlgManager* NewL( CIFFactory& aIFFactory );
		static CDiscoveryDlgManager* NewLC( CIFFactory& aIFFactory);

        /**
         *  Destructor
         */
        virtual ~CDiscoveryDlgManager();

	protected:  // Constructors and destructors

		/**
		 *  Default constructor
         *  @param aDiscovery   Discovery IF
		 */
		CDiscoveryDlgManager();

        /**
         *  2nd phase constructor
         */
		void ConstructL( CIFFactory& aIFFactory );

	public:		// Methods derived from MDiscoveryObserver

		void NotifyNewPrintDeviceL( HBufC* aText, TInt aUid, TBool aCached,
                                   TPrinterType aType, TUint aVendor );
		void DiscoveryError( TInt aErrCode );
		void DeviceDiscoveryCompleted();
        void RemoveCachedPrinterL( TInt aUid ) ;

	public: // from MProgressDialogCallback

		void DialogDismissedL( TInt aButtonId );	
	
	public: // New methods

		/**
		 *  Starts the dialog manager, displays pop-up window where
         *  the found printers are placed
         *
         *  @param aProtocol Specific protocol to searh for. ( all
         *                   available as default )
         *
         *  @param aHidden True will not launch popup list. This will also
         *                 select the found printer to be the printer in use.
         *                 ( EFalse as default ). Note that this can only be used
         *                 with specific protocol and with BT first found printer
         *                 will be selected.
         *
         *
         *  @return ETrue = printer has been selected succesfully
         *          EFalse = not
		 */
		TBool StartDlgManagerL( TInt aProtocol = 0, TBool aHidden = EFalse);

		TInt PrinterUid() const;
        /**
         *  Returns the chosen printer's name
         */
        const TDesC& PrinterName() const;
		/*
		* Was the selected printer USB printer
		*/
		TBool USBPrinterSelected() const;
		
		/*
		*  Canceling discovery and close device popuplist.
		*/
		void CancelDiscovery();

	private:    // New methods

        /**
         *  Creates the pop-up list around the instance's list box
         *
         *  @return Initialized instance of the pop-up list
         */
		CAknPopupList* CreatePopupListL();

        /**
         *  Adds new device to list, updates popup list
         *
         *  @param aText    Textual description of the new print device
		 *  @param aUid     Uid that is associated with the print device
         *  @param aCached  Is printer cached or not?
         *  @param aType    Printer type
         *  @param aVendor	Printer brand
         */
        void AddNewDeviceToListL( HBufC* aText, TInt aUid, TBool aCached,
                                  TPrinterType aType, TUint aVendor );

        /**
         *  Show "restart discovery" query and start the discovery again
         *  if it is so desired
         */
        void RestartDiscoveryL();

        /**
         *  Removes cached icons from the list
         */
        void RemoveCachedIconsL();

        /**
         *  Adds "search again" text to pop-up list
         */
        void AddSearchAgainTextL();

		/**
		* Pop up a wait note during cancellation operation
		*/
		void ShowCancelWaitNoteL();

        TBool StartNormalDiscoveryL();

        TBool StartHiddenDiscoveryL();
        
        /*
		* @function 	AccessPointExistsL
		* @description	Checks if access point is defined
		* @return 		TBool
		*/
		TBool WlanAccessPointExistsL();
		
		/*
		* @function 	TryWlanDiscoveryL
		* @description	Try WLAN discovery 
		*               Checks if AP is defined
		                if not, launches WLAN wizard
		* @return 		TBool
		*/
		TBool TryDeviceDiscoveryL( TUint aProtocols, TBool aSingleDiscovery );

	private:    // Data

		// Listbox for popup list
		CAknSingleGraphicPopupMenuStyleListBox* iListBox;

		// The popup list
		CAknPopupList* iPopupList;

		// Array for profile names
		CDesCArrayFlat* iListOfDeviceNames;

        // Pointer to engine discovery interface
		MDiscovery* iDiscovery;

		// True if popup window is active
		TBool iActivePopup;

        // Array of printer UIDs
		CArrayFixFlat<TInt>* iUidArray;

        // Array of printer Protocols
		CArrayFixFlat<TPrinterType>* iProtocolArray;

        // Array of printer brands
		CArrayFixFlat<TUint>* iBrandArray;

        // Selected printer's name
        HBufC* iPrinterName;

        // Supported protocols
        TUint iSupportedProtocols;

		// Cancellation wait dialog
		CAknWaitDialog* iCancellingDialog;

		// Cancelling state
		TBool iCancelling;

		// Discovery status
		TBool iDiscoveryCompleted;

        // USB printer id
        TInt iUsbPrinterID;

		TBool iUSBPrinterSelected;

        // Hidden discovery flag
        TBool iHiddenDiscovery;

        // Printer found flag
        TBool iPrinterFound;

        // Searching cancelled flag
        TBool iSearchingCancelled;

		// Wait dialog shown while doing hidden discovery
		CAknWaitDialog* iHiddenDiscoveryDialog;

		// Array to store printer names
		CDesCArray* iNameArray;

		// Indicates whether discovery searches multiple devices (bt/wlan cases) or not
		TBool iMultiDiscovery;

		TInt iCurrentPrinterUid;

		// Interface for settings
        MPrintSettings* iSettings;

        //Contais "Searching" title text for popup dialog
   	    HBufC* iSearchingText;

        //Contais "Select device" title text for popup dialog
        HBufC* iSelectDeviceText;

        //Pointer to active title text
        TPtrC iActiveTitleText;
        
    };

#endif  // CDISCOVERYDLGMANAGER_H

//  End of File
