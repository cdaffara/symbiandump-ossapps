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


#ifndef CBTPRINTINGDEVICE_H
#define CBTPRINTINGDEVICE_H

#include "mprotprintingdevice.h"
#include "mbtpcobserver.h"
#include "imageprint.h"
#include "btprotocolconsts.h"
#include "crsbtdiscoverengine.h"
#include "crsbtdevice.h"
#include "cbtprintercontroller.h"
#include "cprintjob.h"

static const TUid KBtProtocolUid = { 0x01007779 };

/**
 *	@brief CBtPrintingDevice implements MProtPrintingDevice. This makes the class the "published" class of the DLL it is in.
 */
class CBtPrintingDevice : public CActive, public MProtPrintingDevice, public MBtPCObserver
	{
		public:

			static MProtPrintingDevice* NewL();

			// Default constructor and destructor.
			CBtPrintingDevice();
			~CBtPrintingDevice();

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

			/**
			 * @brief Submit a job previously created with CreatePrintJobL()
			 */
			void SubmitPrintJobL();

			/**
			 * @brief Cancels and ongoing print job.
			 *
			 * @return KErrNone if successful, KErrInvalidSequence if no printing job was previously submitted.
			 */
			TInt CancelPrintJob();
			TInt ContinuePrintJobL();
			TInt GetNumPrintPages();
			TInt GetPrintJobStatus();
			TInt GetPrinterStatus(TInt aDeviceID);
			// Capabilities.
			TInt GetDeviceCapabilityIDsL(TInt aDeviceID, RArray<TInt>& aCapabilityIDs);
			TInt GetDeviceCapabilityL(TInt aDeviceID, TInt aCapabilityID, TPrintCapability& aCapability);
			TInt GetJobSetting(TInt aCapabilityID, TInt& aValue);

			/**
			 * @brief Sets the value of a single capability.
			 *
			 * Sets the value of a single capability. The value is passed as a TInt.
			 * For Float capabilities it is the numerator.
			 *
			 * Changes in certain capability values can affect other capabilities,
			 * such as a change in paper size changes the available templates.
			 * When this occurs, the affected capabilities are returned in the
			 * aAffectedCapability field. If there are any, the client should get
			 * these capabilities again.
			 *
			 * @param[in] aCapabilityID Id of the capability to be setted.
			 * @param[in] aValue Value to set the capability to.
			 * @param[out] aAffectedCapability 0=None, else CapabilityID
			 *
			 * @return KErrNone if successful, KErrInvalidSequence if job is not active, 
			 *         KErrInvalidData if setting request is invalid (either capability ID or value). 
			 *	       Otherwise another of the system-wide error codes.
			 */
			TInt SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability);
			
			// Preview.
			TInt GetNumPreviewPages();

			/**
			 * @brief Creates a CFbsBitmap object and fills it with the preview bitmap.
			 *
			 * @param[in] aTemplateID Layout Capability ID of the icon desired.
			 * @param[out] aFsBitmapHandle
			 *
			 * @return KErrNone, or another of the system-wide error codes.
			 */
			TInt GetJobTemplateIconL(TInt aTemplateID, TInt& aFsBitmapHandle);

			/**
			 * Creates a preview image in a CFbsBitmap object. PreviewImageEvent() is called on the PrintEventObserver.
			 *
			 * @param aPageNumber Currently unused
			 *
			 * @return KErrNone if successful, KErrInvalidData if the Preview Image could not be loaded, or another of the system-wide error codes.
			 */
			TInt CreatePreviewImage(TInt aPageNumber);

			void RegisterIdleObserver(MProtIdleObserver * /*aObserver*/) {};

			void SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies, TInt& aErr );

			void CreateJobResponse( TInt aJobId, TInt aErrorCode );

			void CancelJobResponse(TInt aErrorCode );

			void GetPrinterStateResponse(TInt aState, 
										 TInt aStateReasons, 
										 TInt aOperationStatus, TInt aDeviceId);

			void GetEventResponse(TInt aJobId, 
								  TInt aJobState, 	
								  TInt aPrinterState, 
								  TInt aStateReasons, 
								  TInt aOperationStatus);								 

			void GetJobAttributesResponse( TInt  aJobState, 
										  const TDesC8&  aJobName,
										  const TDesC8&  aOriginator,
										  TInt aSheetsCompleted, 
										  TInt aPendingJobs,
										  TInt aErrorCode );
												  
			void DeviceDiscovered( CRsBtDevice& aDevice );
			void DeviceDisappeared( CRsBtDevice& aDevice );
			void DiscoveryError(TInt aError);
			
			/*
			 * Updates OPP printing progress bar. 
			 * */
			void OPPSendProgress( TInt aProgress );

		public: //own
			static TBool HandleTimeout(TAny *aWho);

		private:
		
			virtual void RunL();
			virtual void DoCancel();
			virtual TInt RunError(TInt aError);

			void GetUsedDevicesL();

			void InitJobL(CRsBtDevice *aPrinter, RPointerArray<TDesC>& aImages);

			TBool InformObserver(TInt aUsedProtocol);

			void GetCapabilities(CRsBtDevice& aPrinter, TBool aForcedGet = EFalse);
			void GetDefaultCapabilities(RArray<TPrintCapability>& aCapabilities);

			void FoundDeviceL(CRsBtDevice& aDevice);
			void UpdateCacheList();
			
			TBool DoHandleTimeout();

			void ResetTimer();
			void StartTimer(TTimeIntervalMicroSeconds32 aDelay,
							TTimeIntervalMicroSeconds32 anInterval,
							TCallBack aCallBack);

			CRsBtDevice* FindDevice(TInt aPrinterID);

			TInt AddBtDeviceL(CRsBtDevice& aDevice);

			void FinishPrinting(TInt aErrorCode);

			void ManagePrintingState(TInt aJobId, TInt aJobState, TInt aPrinterState,
					  					 TInt aStateReasons, TInt aOperationStatus);
					  					 
			void GetJobAttributesL();
			void GetPrinterStateL();
			void CancelJobL();
			void Deactivate();
			// updates the printing proggress if increased and resets the pending counter if so.
			TInt UpdateProggress();
			void GetLocalBTNameL(TDes8& aName);
			void OPPJobRollBack();
						
		private:
		
			enum TDeviceState
			{
				ENone,
				EReady,
				EGetPrinterState,
				EGetJobAttributes,
				EFinish,
				EDiscovering,
				EJobProgress,
				ECancelJob
			};
		
			/// State operating.
			TDeviceState iState;

			/// Print job object.
			CPrintJob* iPrintingJob;

			CBtPrinterController* iPrinterController;
			CBtDeviceContainer* iDeviceContainer;

			/// Requested Protocols for Discovery
			TUint  iRequestedProtocols;

			/// Print error.
			TInt iPrintError;
			/// Print error message code.
			TInt iPrintErrorMsgCode;
			
			TUint iPendingCount;
			
			CPeriodic* iPrinterTimer;
			
			TInt iProggress;
			
			// used BlueTooth profile  
			TUint iUsedBtProfile;
			
			
	};

#endif // CBTPRINTINGDEVICE_H

//  End of File
