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
* Description:  Declares CUPPrinterContainer class
*
*/


#ifndef CUPPRINTERCONTAINER_H
#define CUPPRINTERCONTAINER_H

#include <upnpdevice.h>
#include <f32file.h>

#include "tprinter.h"
#include "cupprinter.h"
#include "imageprint.h"

/*
* @class 		CUPPrinterContainer
* @description	This class contains an array of discovered devices
*				and methods for handling it. It also converts CDevice objects to 
*				TPrinter, where TPrinter's id is the index 
*/

NONSHARABLE_CLASS(CUPPrinterContainer) : public CBase
	{
		public:
			/*
			* @function 	NewL
			* @description	Constructs the CUPPrinterContainer object
			*/
			static CUPPrinterContainer* NewL();

			/*
			* @function 	Destructor
			* @description	Releases the resources allocated by this object
			*/
			~CUPPrinterContainer();
			
			/*
			* @function 	PrinterCount
			* @description	Returns the number of printers in iPrinters.
			* @return 		TInt
			*/
			TInt PrinterCount();
			
			/*
			* @function 	AddPrinterL
			* @description	Adds a device to the device array. Called when
			*				a device is discovered.
			* @param		CDevice*& Reference to the pointer of the device
			* @param		TInt& The new id will be set to this value
			* @return		TInt Printer id of the added device
			*/
			void AddPrinterL(CUpnpDevice& aDevice, TInt& aId);

			/*
			* @function 	RemovePrinter
			* @description	Removes a printer from the array. Called when
			*				a device is disappeared.
			* @param		CDevice& aDevice
			* @return 		TInt Symbian error code
			*/
			TInt RemovePrinter(CUpnpDevice& aDevice);
			
			/*
			* @function 	RemovePrinter
			* @description	Removes a printer from the array. Called when
			*				a device is disappeared.
			* @param		TInt aIndex
			* @return 		TInt Symbian error code
			*/
			TInt RemovePrinter(TInt aIndex);
			
			/*
			* @function 	PrinterIndex
			* @description	Returns the index number of the device in discovered
			*				printers array. If printer is not found -1 is returned.
			*				This function compares the Uuid's of the CDevices.
			* @param		CDevice 
			* @return		TInt
			*/
			TInt PrinterIndex(CUpnpDevice& aDevice);

			/*
			* @function 	PrinterIndex
			* @description	Returns the index number of the device in discovered
			*				printers array. If printer is not found -1 is returned.
			*				This function searches for matching CUPPrinter id's.
			* @param		CDevice* 
			* @return		TInt
			*/
			TInt PrinterIndex(TInt aPrinterId);

			/*
			* @function 	PrinterDisappeared
			* @description	Sets a printer as disappeared in printer array.
			* @param		CDevice
			*/
			void PrinterDisappeared(CUpnpDevice& aDevice);

			/*
			* @function 	SyncPrinterArray
			* @description	Removes disappeared devices from iPrinters array.
			*/
			void SyncPrinterArray();

			/*
			* @function 	ReadCacheL
			* @description	Reads printer information from cache file into printer array
			*/
			void ReadCacheL();

			/*
			* @function 	UpdateCacheL
			* @description	Updates the cache file with available printers
			*/
			void UpdateCacheL();

			/*
			* @function 	CacheCurrentL
			* @description	Updates the current device in cache file
			*/
			void CacheCurrentL();

			/*
			* @function 	RemoveCachedDeviceL
			* @description	Removes the given device in cache file
			* @param		TInt Id of the device to remove
			*/
			TInt RemoveCachedDeviceL(TInt aDeviceID);
			
			/*
			* @function 	UpdateCacheL
			* @description	Updates the cache file with available printers
			* @param		TInt Id of the device to add in cache
			*/
			void UpdateCacheL(TInt aDeviceID);

			/*
			* @function 	ToTPrinter
			* @description	Returns a printer object as TPrinter by index.
			* @param		CDevice*& aDevice
			* @return		TPrinter
			*/
			TPrinter ToTPrinter(CUpnpDevice& aDevice);

			/*
			* @function 	ToTPrinter
			* @description	Returns a printer object as TPrinter by index.
			* @param		TInt aIndex
			* @return		TPrinter
			*/
			TPrinter ToTPrinter(TInt aIndex);


			/*
			* @function 	GetDisappearedPrinters
			* @description	Initializes a pointer array with pointers to all disappeared printers
			* @param		RPointerArray	Out param
			*/
			void GetDisappearedPrinters(RPointerArray<CUPPrinter>& aArray);

			/*
			* @function 	Printer
			* @description	Returns a pointer to CUPPrinter in printer array.
			* @param		CDevice& aDevice
			* @return		CUPPrinter*
			*/
			CUPPrinter* Printer(CUpnpDevice& aDevice);

			/*
			* @function 	Printer
			* @description	Returns a pointer to CUPPrinter in printer array.
			* @param		TInt aId
			* @return		CUPPrinter*
			*/
			CUPPrinter* Printer(TInt aId);

		protected:
		

		private:
			/*
			* @function 	Constructor
			* @description	C++ constructor
			*/
			CUPPrinterContainer();
	
		
			/*
			* @function 	ConstructL
			* @description	Construction's second phase
			*/
			void ConstructL();

			/*
			* @function 	ToCDevice
			* @description	Returns a pointer to CUPPrinter's CDevice in printer array.
			* @param		TInt aIndex
			* @return		CDevice*
			*/
			CUpnpDevice* ToCDevice(TInt aIndex);

			/*
			* @function 	GenerateId
			* @description	Generated an unique id number for a printer
			* @return		TInt Generated Id number
			*/
			TInt GenerateId();

			/*
			* @function 	AddCachedPrinterL
			* @description	Adds a cached printer to printer array
			* @param		TInt 	aId
			* @param		TDesC&	aUPnPUId
			* @param		TDesC&	aDisplayName
			*/
			void AddCachedPrinterL(TInt aId, TDesC8& aUPnPUId, TDesC8& aDisplayName, const TInt aVendor);

			/*
			* @function 	ReadCacheFileL
			* @description	Reads the cached printers in iCacheBuffer
			*				NOTE: Creates the iCacheBuffer
			*/
			void ReadCacheFileL();

			/*
			* @function 	FindCachedPrinterL
			* @description	Returns requested printer data from the cache file and the start 
			*				and end position of the printer data in file
			* @param		TInt 	ID of the printer to find
			* @param		TInt&	Start position of the data in file
			* @param		TInt&	End position of the data in file
			* @return		TPtrC8	Printer data in buffer pointer
			*/
			TPtrC8 FindCachedPrinterL(TInt aId, TInt& aStartPos, TInt& aEndPos);
		
			/*
			* @function 	AddCachedPrinterL
			* @description	Adds a cached printer to printer array
			* @param		TInt 	aId
			* @param		TDesC&	aUPnPUId
			* @param		TDesC&	aDisplayName
			*/
			CUPPrinter* PrinterByIndex(TInt aIndex);

		
		private:
			// @var iPrinterArray	Discovered devices
			RPointerArray<CUPPrinter> iPrinters;

			// @var iFsSession		File server session
			RFs iFsSession;


			//@var iIdCounter		Temprary id generation solution
			TInt iIdCounter;

			//@var iCacheBuffer			The content of the cached file.
			HBufC8* iCacheBuffer;
	};

#endif // CUPPRINTERCONTAINER_H

//  End of File
