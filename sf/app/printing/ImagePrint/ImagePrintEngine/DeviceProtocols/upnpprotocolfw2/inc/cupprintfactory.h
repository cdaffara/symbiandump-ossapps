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
* Description:  Declares CUPPrintFactory class
*
*/


#ifndef CUPPRINTFACTORY_H
#define CUPPRINTFACTORY_H

#include <e32base.h>
#include <f32file.h>
#include <upnpdevice.h>

#include "cxhtmlfilecomposer.h"
#include "cimageinfo.h"
#include "printcapabilitycodes.h"
#include "tprintcapability.h"
#include "upconsts.h"

/*
* @class 		CUPPrintFactory
* @description	This class maps the printers between protocol client and
*				UPnP printer control point.
*/
NONSHARABLE_CLASS(CUPPrintFactory) : public CBase
	{
		public:
			/*
			* @function 	NewL
			* @description	Constructs the object
			*/
			static CUPPrintFactory* NewL();

			/*
			* @function 	Destructor
			* @description	Destructor
			*/
			~CUPPrintFactory();

			/*
			* @function		CreateXhtmlFileL
			* @description 	Creates XHTML-Print template folder for printing job.
			* @param		aJobURIs			Arrays of images' HTTP addresses to be printed
			* @param		aLayout				Defines the print layout
			* @param		aPaperSize			Paper size
			* @param		aQuality			Print quality
			* @param		aXhtmlPrintFile		Output parameter, which contains the filepath of created template file
			* @param		TInt& 				Returns the number of pages to print
			*/
			void CreateXhtmlFileL(const RArray<CImageInfo>& aJobURIs, const TInt aLayout,
								  const TInt aPaperSize, const TInt aQuality,
								  TDes& aXhtmlPrintFile, TInt& aPageCount);

			/*
			* @function 	DeleteXhtmlFilesL
			* @description	Deletes all XHTML-print files files
			*/
			void DeleteXhtmlFilesL();

			/*
			* @function 	FileCount
			* @description	Returns the number of the files printed at this session.
			*				Number is same as in the latest filename.
			* @return		TInt
			*/
			TInt FileCount();

			/*
			* @function 	GetCapabilityIDs
			* @description	Returns the capability IDs of supported capabilities
			* @param		aCapabilityIDs	Output parameter
			*/
			void GetCapabilityIDs(RArray<TInt>& aCapabilityIDs);

			/*
			* @function 	GetCapability
			* @description	Returns the default value of a capability
			* @param 		aCapabilityID	Id of the capability
			* @param		aCapability		Output parameter
			* @return		System wide error code
			*/
			TInt GetCapability(const TInt aCapabilityID, TPrintCapability& aCapability);

			/*
			* @function 	GetPrintSetting
			* @description	Returns the requested capability value.
			*				If capability id is invalid KErrArgument is returned.
			* @param		TInt	aCapabilityID
			* @param		TInt	aValue
			* @return 		TInt	Error code
			*/
			TInt GetPrintSetting(TInt aCapabilityID, TInt& aCapability);

			/*
			* @function 	SetPrintSetting
			* @description	Changes the value of an existing print setting.
			*				If setting is not supported KErrNotSupported is returned.
			* @param 		TInt aCapabilityId
			* @param		TInt aValue
			* @return		TInt System wide error code
			*/
			TInt SetPrintSetting(TInt aCapabilityID, TInt aValue);

		protected:


		private:
			/*
			* @function 	Constructor
			* @description	C++ constructor
			*/
			CUPPrintFactory();

			/*
			* @function 	ConstructL
			* @description	Construction's second phase
			*/
			void ConstructL();

			/*
			* @function 	InitCapabilities
			* @description	Initializes the supported capabilities array
			*/
			void InitCapabilities();

			/*
			* @function 	GetDefaultSettingsL
			* @description	Reads default printing settings from the file.
			* @param		TInt& returns layout value
			* @param		TInt& returns size value
			* @param		TInt& returns quality value
			*/
			void GetDefaultSettingsL(TInt& aLayout, TInt& aSize, TInt& aQuality);

			/*
			* @function 	SetDefaultSettingsL
			* @description	Sets the currently stored default printing settings in file.
			*/
			void SetDefaultSettingsL();

		private:
			//@var	iFileCount	Counter used in XHTML-print file naming
			TInt iFileCount;

			//@var iCapabilities	Capabilities/ settings supported by XHTML-print file factory
			RArray<TPrintCapability> iCapabilities;

			//@var iFileSession			File session for writing job settings in file.
			RFs iFileSession;
	};

#endif // CUPPRINTFACTORY_H

//  End of File
