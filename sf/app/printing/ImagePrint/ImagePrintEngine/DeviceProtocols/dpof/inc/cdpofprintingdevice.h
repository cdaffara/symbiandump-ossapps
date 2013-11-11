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
* Description:  Contains the CDPOFPrintingDevice class definition.
*
*/


#ifndef CDPOFPRINTINGDEVICE_H
#define CDPOFPRINTINGDEVICE_H

#include <fbs.h>
#include <AknGlobalConfirmationQuery.h>
#include <data_caging_path_literals.hrh>

#include "mprotprintingdevice.h"
#include "crsdpofengine.h"
#include "tmapping.h"
#include "cconfirmationquery.h"
#include "mconfirmationnotify.h"

//  FORWARD DECLARATIONS
class CPrinterCapabilities;
class CPrinterElement;
class CCapabilityArray;
class CPrintJob;
class TSetting;


/**
 *	@brief CDPOFPrintingDevice implements MProtPrintingDevice. This makes the class the "published" class of the DLL it is in.
 */
class CDPOFPrintingDevice : public CActive, public MProtPrintingDevice, public MDpofEngineObserver, public MConfirmationNotify
	{
		public:
			static MProtPrintingDevice* NewL();
		
			// Default constructor and destructor.
			CDPOFPrintingDevice();
			~CDPOFPrintingDevice();

		public: // From MProtPrintingDevice
			
			// General.
			void ConstructL(const TDesC& aDLLPath);
			TVersion Version();
			TUint SupportedProtocols();
			// Discovery.
			void StartDiscoveryL(MProtDiscoveryObserver& aObserver, TUint aProtocols = 0);
			TInt RemoveCachedDeviceL(TInt aDeviceID);
			void CancelDiscovery(TBool aDoCallBack = ETrue);
			// Print.
			TInt CreatePrintJobL(TInt aDeviceID, RPointerArray<TDesC>& aImages, MProtPrintEventObserver& aObserver);
			void SubmitPrintJobL();
			TInt CancelPrintJob();
			TInt ContinuePrintJobL();
			TInt GetNumPrintPages();
			TInt GetPrintJobStatus();
			TInt GetPrinterStatus(TInt aDeviceID);
			// Capabilities.
			TInt GetDeviceCapabilityIDsL(TInt aDeviceID, RArray<TInt>& aCapabilityIDs);
			TInt GetDeviceCapabilityL(TInt aDeviceID, TInt aCapabilityID, TPrintCapability& aCapability);
			TInt GetJobSetting(TInt aCapabilityID, TInt& aValue);
			TInt SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability);
			// Preview.
			TInt GetNumPreviewPages();
			TInt GetJobTemplateIconL(TInt aTemplateID, TInt& aFsBitmapHandle);
			TInt CreatePreviewImage(TInt aPageNumber);

			void RegisterIdleObserver(MProtIdleObserver * /*aObserver*/) {};
			
			public: // MBTOnNotify
			void BTOnCompleteL( TBool aResult ); 
			void ConfirmationQueryCompleteL ( TInt aError );
			
			void OverwriteExistingJobL ();
			
		
			void SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies, TInt& aErr );	

		public: // From MDpofEngineObserver

			void HandleDpofEngineEventL(const TInt aStatus, const TInt aRetCode);
		
		protected:	// From CActive
			
			void DoCancel();	
			void RunL();

		private:
			// Utils.
			// Capabilities Section.
			CCapabilityArray* LoadCapabilityL(RResourceFile& aResFile, TInt aCapID);
			CPrinterElement* FindDeviceL(TInt aPrinterID);
			TBool FindSetting(TInt aCapabilityID, TSetting& aSetting);
			TInt AddReadyRequestAndSetActive( TInt aRequest );
			TInt CheckReadyRequestAndSetActive();
			// Set MMC printer
			void SetMMCPrinterL();

		    // Confirmation to override existing autoprint file
	        void ShowConfirmationQueryL();
		
		private: // data
				
			/// Protocol Capabilities.
			CCapabilityArray* iCapabilities;
			/// Array to save all the devices discovered.
			RPointerArray<CPrinterElement> iDeviceList;
			/// Print job object.
			CPrintJob* iPrintJob;

			/// DPOF Engine.
			CRsDpofEngine *iDPOF;

			/// Array to save all the icon / template pairs supported by a protocol.
			RArray<TMapping> iMappingArray;			
		
			/// Print error.
			TInt iPrintError;
			/// Print error message code.
			TInt iPrintErrorMsgCode;
			
			RArray<TInt> iReadyRequests;			
	
	        TBool iCancelledByUser;
	   
	        CConfirmationQuery* iConfirmationQuery;
	
	        CAknGlobalConfirmationQuery* iPQ;
	        
	        HBufC* iQuestion;
	};

#endif // CDPOFPRINTINGDEVICE_H

//  End of File
