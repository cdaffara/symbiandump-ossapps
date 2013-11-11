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
* Description:  Declares CUPnPPrintingDevice class
*
*/


#ifndef CUPNPPRINTINGDEVICE_H
#define CUPNPPRINTINGDEVICE_H

#include <fbs.h>
#include <upnpmediaserverclient.h>
#include <upnpfilesharing.h>
#include <data_caging_path_literals.hrh>

#include "imageprint.h"
#include "mprotprintingdevice.h"
#include "cprintercontrolpoint.h"
#include "printmessagecodes.h"
#include "cupprintercontainer.h"
#include "cuplogger.h"
#include "upconsts.h"
#include "cupprintingjob.h"

_LIT8(KFriendlyName, "UPnPProtocol");

/// @brief The timeout to wait for first devices to be discovered
#define KPrinterDiscoveryTimeout 	TTimeIntervalMicroSeconds32(25000000)

/// @brief The timeout to wait for responses during printing 60 seconds
#define KPrinterResponseTimeout 	TTimeIntervalMicroSeconds32(60000000)
//#define KPrinterResponseTimeout 	TTimeIntervalMicroSeconds32(30000000)

/// @brief The timeout to wait for responses during printing
#define KPrinterSubmitTimeout 		TTimeIntervalMicroSeconds32(15000000)

// @brief The times the timer can expire while printing is processing
#define KPendingLimit 5

#define	KVersionMajor 0
#define	KVersionMinor 0
#define	KVersionBuild 1


enum TUPnPProtocolState
{
	EUPnPUninitialized=0,
	EUPnPDiscovery,
	EUPnPReady,
	EUPnPJobReady,
	EUPnPSubmittingJob,
	EUPnPPrintingStarted,
	EUPnPPrinting,
	EUPnPPrintingReady,
	EUPnPCancelling,
	EUPnPCancellingDiscovery
};

/*
* @class 		CUPnPPrintingDevice
* @description	Implements the MProtPrintingDevice and MPCPObserver interfaces 
*				to integrate Image Print and UPnP stack.
*
*/
class CUPnPPrintingDevice : public CBase, public MPCPObserver, public MProtPrintingDevice
	{
		public:
		
			/*
			* @function		CUPnPPrintingDevice
			* @description	C++ constructor
			*/
			CUPnPPrintingDevice();

			/*
			* @function		NewProtocolL
			* @description	Called from Main.cpp to construct a new MProtPrintingDevice instance
			*/
			static MProtPrintingDevice* NewL();

			/*
			* @function		~CUPnPPrintingDevice
			* @description	Destructor.
			*/
			~CUPnPPrintingDevice();
			
			/*
			* @function		DiscoveryTimeoutCBL
			* @description	This static callback function is called by iPrinterTimer when the 
			*				discovery timer event occurs.
			* @param		
			* @return		
			*/
			static TBool DiscoveryTimeoutCBL(TAny *aWho);
			
			/*
			* @function		StatusTimeoutCBL
			* @description	This static callback function is called by iPrinterTimer when the 
			*				status timer event occurs.
			* @param		
			* @return		
			*/
			static TBool StatusTimeoutCBL(TAny *aWho);

			/*
			* @function		SubmitTimeoutCBL
			* @description	This static callback function is called by iPrinterTimer when the 
			*				submit timer event occurs.
			* @param		TAny
			* @return		TInt
			*/
			static TInt SubmitTimeoutCBL(TAny *aWho);

			
			

			// ===============================
			// From MProtPrintingDevice.
			// ===============================
			// General
			virtual void ConstructL(const TDesC& aDLLPath);
			virtual TVersion Version();
			virtual TUint SupportedProtocols();
			// Discovery.
			virtual void StartDiscoveryL(MProtDiscoveryObserver& aObserver, TUint aProtocols = 0);
			virtual TInt RemoveCachedDeviceL(TInt aDeviceID);
			virtual void CancelDiscovery(TBool aDoCallBack = ETrue);
			// Print.
			virtual TInt CreatePrintJobL(TInt aDeviceID, RPointerArray<TDesC>& aImages, MProtPrintEventObserver& aObserver);
			virtual void SubmitPrintJobL();
			virtual TInt CancelPrintJob();
			virtual TInt ContinuePrintJobL();
			virtual TInt GetNumPrintPages();
			virtual TInt GetPrintJobStatus();
			virtual TInt GetPrinterStatus(TInt aDeviceID);
			// Capabilities.
			virtual TInt GetDeviceCapabilityIDsL(TInt aDeviceID, RArray<TInt>& aCapabilityIDs);
			virtual TInt GetDeviceCapabilityL(TInt aDeviceID, TInt aCapabilityID, TPrintCapability& aCapability);
			virtual TInt GetJobSetting(TInt aCapabilityID, TInt& aValue);
			virtual TInt SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability);
			// Preview.
			virtual TInt GetNumPreviewPages();
			virtual TInt GetJobTemplateIconL(TInt aTemplateID, TInt& aFsBitmapHandle);
			virtual TInt CreatePreviewImage(TInt aPageNumber);

			virtual void RegisterIdleObserver(MProtIdleObserver* aObserver);
		
			virtual void SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies, TInt& aErr );

			// ===============================
			// From MPCPObserver
			// ===============================
			virtual void CreateJobResponse( CUpnpAction* aAction, TInt aErrorCode );
			virtual void CancelJobResponse( CUpnpAction* aAction, TInt aErrorCode );
			virtual void GetPrinterAttributesResponse( CUpnpAction* aAction, TInt aErrorCode );
			virtual void GetJobAttributesResponse( CUpnpAction* aAction, TInt aErrorCode );
			virtual void GetMarginsResponse( CUpnpAction* aAction, TInt aErrorCode );
	        virtual void GetMediaListResponse( CUpnpAction* aAction, TInt aErrorCode );
			virtual void StateUpdatedResponse( const TDesC8& aPrinterState, 
                                           const TDesC8& aPrinterStateReasons, 
                                           const TDesC8& aJobIdList,
	                                       const TDesC8& aJobEndState, 
                                           const TDesC8& aJobMediaSheetsCompleted, 
                                           const TDesC8& aJobAbortState,
                                           const TDesC8& aContentCompleteList );
			virtual void DeviceDiscoveredL( CUpnpDevice* aDevice );
			virtual void DeviceDisappeared( CUpnpDevice* aDevice );
			// state events
			virtual void PrinterStateUpdated( const TDesC8& aEvent );
			virtual void PrinterStateReasonsUpdated( const TDesC8& aEvent );
			virtual void JobIdListUpdated( const TDesC8& aEvent );
			virtual void JobEndStateUpdated( const TDesC8& aEvent );
			virtual void JobMediaSheetsCompletedUpdated( const TDesC8& aEvent );
			virtual void ContentCompleteListUpdated( const TDesC8& aEvent );
			virtual void JobAbortStateUpdated( const TDesC8& aEvent );

		protected:
		

		private:

			/*
			* @function 	StartDiscoveryInternalL
			* @description	Non-leaving interface for MProtPrintingDevice method StartDiscoveryL
			*				for ImagePrint compliance. Informs observer about leaves by call back method.
			* @param 		TInt protocol identifier bitmap
			*/
			void StartDiscoveryInternalL(TUint aProtocol);

			/*
			* @function 	RestartControlPointL
			* @description	Restarts the printer control point
			*/
			void RestartControlPointL();

			/*
			* @function 	SendPrinterListL
			* @description	Returns the device list to MProtDiscoveryObserver object
			* @param		TBool
			*/
			void SendPrinterListL(TBool aSync = ETrue);

			/*
			* @function 	DoDiscoveryTimeoutL
			* @description	Ends the first device discovery session. This function is called
			*				by the static callback function DiscoveryTimeoutCBL.
			* @param		TBool aDoCallBack If ETrue then protocol client is notified.
			* @return 		TBool
			*/
			TBool DoDiscoveryTimeoutL(TBool aDoCallBack);

			/*
			* @function 	DoSubmitTimeoutL
			* @description	This function is executed after submit time out expires.
			* @return 		TInt
			*/
			void DoSubmitTimeoutL();

			/*
			* @function 	SubmitJobToPcpL
			* @description	Submits the printing job to PCP
			* @param		CDevice& Reference to the printer to be used in printing
			* @param		TInt The ID of the printer to be used in printing
			*/
			void SubmitJobToPcpL(CUpnpDevice& aPrinter, TInt aPrinterID);

			/*
			* @function 	SubmitJobToPcpL
			* @description	Submits the printing job to PCP
			* @param		TInt The ID of the printer to be used in printing
			*/
			void SubmitJobToPcpL(TInt aDeviceID);

			/*
			* @function 	DoStatusTimeoutL
			* @description	Handles time out event in printer communication.
			* @return 		TBool
			*/
			TBool DoStatusTimeoutL();

			/*
			* @function 	InitJobL
			* @description	Initializes the printing jobs to job container
			* @param		TInt 			Printer for printing jobs
			* @param		RPointerArray	File paths for files to be printed
			*/
			void InitJobL(TInt aDeviceId, RPointerArray<TDesC>& aImages);

			/*
			* @function 	StartPrintingL
			* @description	Sends the printing jobs to the UPnP Printer Control Point
			*/
			void StartPrintingL();

			/*
			* @function 	DoCancelPrinting
			* @description	Cancels the ongoing printing job. Returns KErrInvalidSequence if state is not 
			*				EUPnPSubmittingJob, EUPnPPrintingStarted or EUPnPPrinting.
			* @return 		TInt	System wide error code
			*/
			TInt DoCancelPrinting();
			
			/*
			* @function 	UpdatePrintingStatus
			* @description	Updates the status of printing jobs and informs the printing observer about progress and events
			* @param		const TDesC8& aPrinterState
			*/
			void UpdatePrintingStatus(const TUPnPState aJobState);
		
			
			/*
			* @function		FinishPrinting
			* @description	Executes the basic operations to be done when printing is done.
			*/
			void FinishPrinting();

			/*
			* @function		FinishPrinting
			* @description	Overloads FinishPrinting and informs observer about occurred error.
			* @param		TInt Symbian wide error code
			* @param		TInt ImagePrint specific (error) message code
			*/
			void FinishPrinting(TInt aErrorCode, TInt aErrorMessage);

			/*
			* @function		ResetTimer
			* @description	Resets the timer and initializes timer related flags.
			*/
			void ResetTimer();			

			/*
			* @function		StartTimer
			* @description	Starts the timer and resets it first.
			* @param		TTimeIntervalMicroSeconds32	Delay for the timer
			* @param		TTimeIntervalMicroSeconds32 Interval for timer to expire
			* @param		TCallBack					Call back function when timer expires
			*/
			void StartTimer(TTimeIntervalMicroSeconds32 aDelay,TTimeIntervalMicroSeconds32 anInterval,TCallBack aCallBack);			

			/*
			* @function		GetPrinter
			* @description	Returns a pointer to the current printer.
			* @return		CDevice* Current printer or NULL
			*/
			CUpnpDevice* GetPrinter();
			
		private:
			// @var iPrinterControl		Control point to UPnP
			CPrinterControlPoint* iPrinterControl;
			
			// @var iPrinterContainer	Container for printers
			CUPPrinterContainer* iPrinterContainer;
			
			// @var iPrintingJob		Printing job
			CUPPrintingJob* iPrintingJob;
			
			// @var iPrinterTimer		Array of discovered printers
			CPeriodic* iPrinterTimer;
			
			// @var iPrintFactory		XHTML-print file factory
			CUPPrintFactory* iPrintFactory;
			
			// @var iPrinterState		State information of the object
			TUPnPProtocolState iState;

			// @var iPrintersFound		Indicates if there are printers found
			TBool iPrintersFound;

			// @var iStatusTimeout		This variable is set to ETrue when a status time out occurs
			TBool iStatusTimeout;

			// @var iFbsBitmap			(Dummy) bitmap for icon template
			CFbsBitmap* iFbsBitmap;
			
			// @var iPendingCount		Counter to avoid too long pending while printing
			TInt iPendingCount;
			
			// @var iFriendlyName		In Home network configuration defined name for the device
			TBuf8<256> iFriendlyName;
			
			// @var iTriedReadCacheAndFail   To check that never trying read cache file more than one time, if we don't have it.
			TBool iTriedReadCacheAndFail;
			
	};

#endif // CUPNPPRINTINGDEVICE_H

//  End of File
