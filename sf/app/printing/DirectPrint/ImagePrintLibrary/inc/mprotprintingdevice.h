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
* Description:  Contains the following interfaces: MProtPrintingDevice, 
* 				 MProtIdleObserver, MProtPrintEventObserver and MProtDiscoveryObserver.
*
*/


#ifndef MPROTPRINTINGDEVICE_H
#define MPROTPRINTINGDEVICE_H

#include <e32base.h>

#include "tprinter.h"
#include "directprintcapability.h"
#include "tidleevent.h"

const TInt DEVICE_ID_FIELD_MASK = 0x03FFFFFF;	// most significant bits: 0000 0011 rest is 1's
const TInt PROTOCOL_ID_FIELD_MASK = 0xFC000000;	// most significant bits: 1111 1100
const TInt PROTOCOL_ID_CLEAR_MASK = 0x0000003F;	// less siginificant bits: 0011 1111
const TInt PROTOCOL_ID_BIT_COUNT = 6;


/**
 *	@brief Interface to be implemented by a class observing the discovery events of a class implementing MProtPrintingDevice.
 *
 *	Interface used by a Protocol implementing "MProtPrintingDevice" to report back any device found or not found and
 *	the general status of the discovery process. This should be implemented by the class using "MProtPrintingDevice"; in
 *	this case, it's already implemented in a class in the Image Print Library.
 */
class MProtDiscoveryObserver
	{
	public:
		virtual void FoundDeviceL( TPrinter& aDeviceInfo ) = 0;
		virtual void RemoveDeviceL( TPrinter& aDeviceInfo ) = 0;
		
		/**
		 *  @brief 
		 *  @param aStatus ( EDiscovering | ECancellingDiscovery | EDoneDiscovery | EDiscoveryFinished )
		 *                   EDiscovering           = currently discovering
		 *                   ECancellingDiscovery   = cancel request on discovery
		 *                   EDiscoveryFinished     = on discovery time-out   
		 *  @param aErrorCode
		 *  @param aErrorStringCode
		 */
		virtual void DiscoveryStatusL( TInt aStatus,
										TInt aErrorCode,
										TInt aErrorStringCode = KErrNone ) = 0;
	};

/**
 *	@brief Interface to be implemented by a class observing the printing events of a class implementing MProtPrintingDevice.
 *
 *	Interface used by a Protocol implementing "MProtPrintingDevice" to report back the printing progress, any error or
 *	the printer status. This should be implemented by the class using "MProtPrintingDevice"; in this case, it's already
 *  implemented in a class in the Image Print Library.
 *  These methods should be used only when actual printing is ongoing
 */
class MProtPrintEventObserver
	{
	public:
		/**
		 *	@brief Method to inform observer about printing proggress.
		 *
		 *	@param aStatus 			Defines the printing status defined in ImagePrint.h.
		 *	@param aPercent 		Percents of job readiness.
		 *	@param aJobStateCode	Job's state defined in PrintMessageCodes.h.
		 */
		virtual void PrintJobProgressEvent( TInt aStatus, 
										TInt aPercent, 
										TInt aJobStateCode ) = 0;
										
		/**
		 *	@brief Method to inform observer about error occured during printing.
		 *
		 *	@param aErrorCode 		Defines the error code that is symbian wide or defined in PrintMessageCodes.h.
		 *	@param aErrorStringCode	Reserved for future use.
		 */
		virtual void PrintJobErrorEvent( TInt aErrorCode, 
										TInt aErrorStringCode = KErrNone ) = 0;
										
		/**
		 *	@brief Method to inform observer about error concerning the printing device.
		 *
		 *	@param aErrorCode 		Defines the error code that is symbian wide or defined in PrintMessageCodes.h.
		 *	@param aErrorStringCode	Reserved for future use.
		 */
		virtual void PrinterStatusEvent( TInt aErrorCode, 
										TInt aErrorStringCode = KErrNone ) = 0;

		// DEPRECATED - DO NOT USE
		virtual void PreviewImageEvent( TInt aFsBitmapHandle ) = 0;

		// DEPRECATED - DO NOT USE
		virtual void ShowMessageL( TInt aMsgLine1Code,
										TInt aMsgLine2Code ) = 0;

		// DEPRECATED - DO NOT USE
		virtual TBool AskYesNoQuestionL( TInt aMsgLine1Code,
										TInt aMsgLine2Code ) = 0;

		// DEPRECATED - DO NOT USE
		virtual const TDesC& AskForInputL( TInt aMsgLine1Code,
										TInt aMsgLine2Code ) = 0;
	};

/**
 *  @brief Interface to be implemented by CImagePrint. It is used to report
 *  anything related to status changes in the handling protocol.
 *  @sa MProtPrintingDevice::RegisterIdleObserver
 */
class MProtIdleObserver
	{
	public:
		/** Called by a protocol to notify CImagePrint of events. The meaning of the event and associated error and mesage codes
		 *  is protocol dependant. This should be used only when actual printing is not ongoing.
		 */
		virtual void StatusEvent( const TEvent &aEvent,
										TInt aError,
										TInt aMsgCode ) = 0;
	};

/**
 *	@brief Interface to be implemented in the Polymorphic DLLs.
 *
 *	Interface to be implemented by any printing protocol that wants to be added to the Image Print library, so it can
 *	be seen by the library and used in a transparent way. This implementation should be encapsulated within a Polymorphic
 *	DLL and put in a specific directory.
 */
class MProtPrintingDevice
	{
	public:
		// General.
		/**
		 *	@brief Part of the standard construction pattern.
		 *	@param aDLLPath Path where the DLL was found so it can know where it is located and use that information
		 *	when looking for resource files and such. The path includes the DLL itself.
		 */
		virtual void ConstructL( const TDesC& aDLLPath ) = 0;
		/**
		 *	@brief Reports the Version of the DLL.
		 *	@return A TVersion object containing the version of the protocol.
		 */
		virtual TVersion Version() = 0;
		/**
		 *	@brief Returns the Protocols supported by the DLL. It can be any of the KImagePrint_PrinterProtocol* constants.
		 *	@return An OR'ed value with the supported protocols information.
		 */
		virtual TUint SupportedProtocols() = 0;
		// Discovery.
		/**
		 *	@brief Starts the discovery process.
		 *
		 *	This process is to find whatever printing device is available nearby. After this call server waits
		 *  DiscoveryStatusL from MProtDiscoveryObserver to be called with value aStatus = EDiscoveryFinished
		 *  after the discovey is finished.
		 *	@param aObserver The discovery observer.
		 *	@param aProtocol Used to indicate what protocols to search for. 0 = all, 1 = BPP, 2 = BIP, 4 = OPP, 8 = DPOF, 16 = PB.
		 *	@return Any standard Symbian error code.
		 */
		virtual void StartDiscoveryL( MProtDiscoveryObserver& aObserver,
										TUint aProtocol = 0 ) = 0;
		/**
		 *	@brief Removes a specified cached printer.
		 *	@param[in] aDeviceID The ID of the printer the client wants to remove.
		 *	@return Any standard Symbian error code or ( KErrInvalidSequence and KErrInvalidData ).
		 */
		virtual TInt RemoveCachedDeviceL( TInt aDeviceID ) = 0;
		
		/**
		 *	@brief Cancels the discovery process.
		 *  The caller expects to get notification of cancelled discovery via 'DiscoveryStatusL' with value 'EDiscoveryCancelling'
		 *	@param[in] aDoCallBack Flag to indicate whether to call back the observer or not. Default = True.
		 */
		virtual void CancelDiscovery( TBool aDoCallBack = ETrue ) = 0;

		// Print.
		/**
		 *	@brief Creates a print job in the specified printer.
		 *	@param[in] aDeviceID The ID of the printer where the client wishes to create the print job.
		 *	@param aImages Array containing the full names of the files to print.
		 *	@param aObserver The print event observer.
		 *	@return Any standard Symbian error code or ( KErrInvalidSequence and KErrInvalidData ).
		 */
		virtual TInt CreatePrintJobL( TInt aDeviceID,
										RPointerArray<TDesC>& aImages,
										MProtPrintEventObserver& aObserver ) = 0;
		/**
		 *	@brief Submits a print job already created.
		 */
		virtual void SubmitPrintJobL() = 0;
		/**
		 *	@brief Cancels the print job.
		 */
		virtual TInt CancelPrintJob() = 0;
		/**
		 *	@brief Continues printing and / or submitting a print job. Implementation may vary from protocol to protocol.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		virtual TInt ContinuePrintJobL() = 0;
		/**
		 *	@brief Gets the number of pages to be printed.
		 *	@return Number of pages to print.
		 */
		virtual TInt GetNumPrintPages() = 0;
		/**
		 *  DEPRECATED - DO NOT USE
		 *	@brief Gets the status of the printing job created.
		 *	@return Printing job status.
		 */
		virtual TInt GetPrintJobStatus() = 0;
		/**
		 *  DEPRECATED - DO NOT USE
		 *	@brief Gets the status of the printer.
		 *	@param[in] aDeviceID The ID of the printer the client wants to know about.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		virtual TInt GetPrinterStatus( TInt aDeviceID ) = 0;
		// Capabilities.
		/**
		 *	@brief Gets the IDs of the capabilities supported by the printer specified.
		 *	@param[in] aDeviceID The ID of the printer the client wants to know about.
		 *	@param[out] aCapabilityIDs Array containing the ID's of the capabilities of the specified printer.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		virtual TInt GetDeviceCapabilityIDsL( TInt aDeviceID,
										RArray<TInt>& aCapabilityIDs ) = 0;
		/**
		 *	@brief Gets the detail of a certain capability of a certain printer.
		 *	@param[in] aDeviceID The exposed ID of the printer the client wants to know about.
		 *	@param[in] aCapabilityID ID of the capability of interest.
		 *	@param[out] aCapability Capability information.
		 * @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		virtual TInt GetDeviceCapabilityL( TInt aDeviceID,
										TInt aCapabilityID,
										TDirectPrintCapability& aCapability ) = 0;
		/**
		 *	@brief Gets a setting (i.e., papersize selected) from an existing job.
		 *	@param[in] aCapabilityID ID of the capability of interest.
		 *	@param[out] aValue Capability value.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		virtual TInt GetJobSetting( TInt aCapabilityID,
										TInt& aValue ) = 0;
		/**
		 *	@brief Sets a print job setting.
		 *	@param[in] aCapabilityID ID of the capability of interest.
		 *	@param[in] aValue New capability value.
		 *	@param[out] aAffectedCapability ID of any affected capability.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		virtual TInt SetJobSettingL( TInt aCapabilityID,
										TInt aValue,
										TInt& aAffectedCapability ) = 0;
		// Preview.
		/**
		 *  DEPRECATED - DO NOT USE
		 *	@brief Gets the number of pages in the preview.
		 *	@return Number of pages in the preview.
		 */
		virtual TInt GetNumPreviewPages() = 0;
		/**
		 *  DEPRECATED - DO NOT USE
		 *	@brief Gets the icon representing a given layout or template.
		 *	@param[in] aTemplateID ID of the template / layout of interest.
		 *	@param[out] aFsBitmapHandle Handle to the appropriate bitmap.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		virtual TInt GetJobTemplateIconL( TInt aTemplateID,
										TInt& aFsBitmapHandle ) = 0;
		/**
		 *  DEPRECATED - DO NOT USE
		 *	@brief Creates a preview image.
		 *	@param[in] aPageNumber Page number to create preview image of.
				 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		virtual TInt CreatePreviewImage( TInt aPageNumber ) = 0;

		/**
		 *	@brief Destructor.
		 */
		virtual ~MProtPrintingDevice() {}

		
		/**
		 *  @brief Registers the IdleObserver to send idle event notifications
		 *  @param A pointer to an object implementing MIdleObserver interface that will be notified of events
		 *  @note In order to stop sending notifications, send NULL as the parameter.
		 */
		virtual void RegisterIdleObserver( MProtIdleObserver *aObserver ) = 0;


		/**
		 *  @brief Number of copies setter. Must be called after the print job is created but before
		 *		submission. Amount of array elements must be the same with the amount of image files
		 *		in the print job. Array elements must be greater than 0.
		 *  @param aNumsOfCopies number of copies array
		 *  @param aErr contains error code on return
		 */
		virtual void SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies,
										TInt& aErr ) = 0;

	public:
		TUid iDtor_ID_Key;
	protected:
		/// Protocol API observer.
		MProtDiscoveryObserver* iDiscoveryObserver;
		/// Protocol API observer.
		MProtPrintEventObserver* iPrintEventObserver;
	};

#endif // MPROTPRINTINGDEVICE_H

//  End of File
