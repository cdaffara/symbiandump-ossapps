/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CUPPrinter class
*
*/


#ifndef CUPPRINTER_H
#define CUPPRINTER_H

#include <upnpservice.h>
#include <upnpdevice.h>

#include "upconsts.h"
#include "tprinter.h"
#include "imageprint.h"

/*
* @class 		CUPPrinter
* @description	This class maps the printers between protocol client and
*				UPnP printer control point.
*/

NONSHARABLE_CLASS(CUPPrinter) : public CBase
	{
		public:
			/*
			* @function 	NewL
			* @description	Constructs the CUPPrinter object
			* @param		CDevice*
			* @param		TInt Mapping id
			*/
			static CUPPrinter* NewL(CUpnpDevice& aDevice, const TInt aId);

			/*
			* @function 	NewL
			* @description	Constructs the CUAPrinter object
			* @param		TInt 	Mapping id
			* @param		TDesC& 	UPnP UId
			* @param		TDesC& 	Display name of printer
			*/
			static CUPPrinter* NewL(const TInt aId, const TDesC8& aUId, const TDesC8& aDisplayName, const TInt aVendor);


			/*
			* @function 	Destructor
			* @description	Destructor
			*/
			~CUPPrinter();
		

			/*
			* @function 	Device
			* @description	Returns a pointer to CDevice
			* @return		CDevice*
			*/
			CUpnpDevice* Device();

			/*
			* @function 	ToTPrinter
			* @description	Returns the object as TPrinter
			* @return		TPrinter
			*/
			TPrinter ToTPrinter();

			/*
			* @function 	Id
			* @description	Returns the id of the device. 
			*				NOTE: This id maps the printer requests from protocol client 
			*				to a correct UPnP printer control point's printer
			*/
			TInt Id();

			/*
			* @function 	GetUId
			* @description	Returns the UPnP uid of the device. 
			*/
			void GetUId(TDesC& aUId);

			/*
			* @function 	IsDisappeared
			* @description	Returns ETrue if the printer is reported to be disappeared
			* @return 		TBool
			*/
			TBool IsDisappeared();

			/*
			* @function 	SetDisappeared
			* @description	Sets the iDisappeared value as ETrue. Called when the printer is reported to be disappeared.
			* @param 		TBool
			*/
			void SetDisappeared(TBool aDisappeared = ETrue);
			/*
			* @function 	UId
			* @description	Returns the printers UPnP UId
			* @return 		TPtrC
			*/
			TPtrC8 UId();

			/*
			* @function 	DisplayName
			* @description	Returns the printers display name
			* @return 		TPtrC
			*/
			TPtrC8 DisplayName();

			/*
			* @function 	SetDevice
			* @description	Sets the CDevice pointer. NOTE: The actual object is never owned by this class!
			* @return 		CDevice*
			*/
			void SetDevice(CUpnpDevice* aDevice);

			/*
			* @function 	SetCached
			* @description	Sets a flag if printer is cached in cache file or not
			* @param 		TBool
			*/
			void SetCached(TBool aCached);

			/*
			* @function 	Cached
			* @description	Returns a flag if printer is cached in cache file or not
			* @return 		TBool
			*/
			TBool Cached();

			/*
			* @function 	Vendor
			* @description	Returns an enum value TPrinterVendor
			* @return 		TPrinterVendor
			*/
			TPrinter::TPrinterVendor Vendor();

			/*
			* @function 	SetVendor
			* @description	Returns an enum value TPrinterVendor
			* @return 		TPrinterVendor
			*/
			void SetVendor(const TDesC8& aManufacturer);

		protected:
		

		private:
			/*
			* @function 	Constructor
			* @description	C++ constructor
			*/
			CUPPrinter();
	
			/*
			* @function 	ConstructL
			* @description	Construction's second phase
			* @param		CDevice*	Pointer to UPnP device
			* @param		TInt 		Mapping id
			* @param		TDesC8&		UPnP UId
			* @param		TDesC8&		Display name of printer
			*/
			void ConstructL(CUpnpDevice* aDevice, const TInt aId, const TDesC8& aUId, const TDesC8& aDisplayName, const TInt aVendor);
		
		private:
		
		// @var iDevice				Pointer to UPnP device object
		CUpnpDevice* iDevice;
		
		// @var iId					Printer id for client apps
		TInt	 iId;

		// @var iUId				Printer UPnP UId
		HBufC8*	 iUId;

		// @var iDisplayName		Printer's display name (UPnP Friendly name)
		HBufC8*	 iDisplayName;
		
		// @var iDisappeared		ETrue, when a disappearing notification is received for this printer
		TBool iDisappeared;			
		
		// @var iCached				ETrue, if printer is cached in cache file.
		TBool iCached;	
		
		// @var iVendor				TPrinter::TPrinterVendor value for defining vendor
		TPrinter::TPrinterVendor iVendor;		
			
	};

#endif // CUPPRINTER_H

//  End of File
