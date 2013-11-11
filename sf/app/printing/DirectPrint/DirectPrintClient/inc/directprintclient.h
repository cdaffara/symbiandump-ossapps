/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/


#ifndef RDIRECTPRINTCLIENT_H
#define RDIRECTPRINTCLIENT_H

#include <e32std.h>

#include "directprintselectitem.h"

class TDirectPrintCapability;
//class TIdleGuardData;
//class TDiscoveryGuardData;
class TDirectPrintJobGuardData;

class RDirectPrintClient : public RSessionBase
	{
	public:

		/**
         *	Constructor      
         */
		IMPORT_C RDirectPrintClient();

		/**
         *	Connect to the DirectPrint server
		 *
		 *  @return KErrNone if successful, otherwise another of the system-wide error codes
         */
		IMPORT_C void ConnectL();

		/**
         *	Close the session         
         */
		IMPORT_C void Close();

		/**
         *	Count connections to the server ( all clients in all prosesses )
         *	
		 *	@param aConnections holder for connnections count 
         *  @return KErrNone if successful, otherwise another of the system-wide error codes
         */
		IMPORT_C TInt CountConnections( TInt& aConnections ) const;
		
		/**    
		 *	Inform server about foreground/background state of the client
		 *
		 *	@param aFg foreground/background flag
         *  @return KErrNone if successful, otherwise another of the system-wide error codes
         */
		IMPORT_C TInt SetForeground( TInt aFg ) const;
	
		
		/**
         *	Reserves DirectPrint engine for this client
         *	
         *  @return KErrNone if successful, KErrInUse if used by another client
         */
		IMPORT_C TInt ReserveEngine() const;

		/**
         *	Releases DirectPrint engine
         *	
         *  @return KErrNone if successful, KErrInUse if used by another client 
         */
		IMPORT_C TInt ReleaseEngine() const;
		
		/**
         *	Get protocols supported by DirectPrint engine
         *	
         *  @return OR:ed protocols or KErrInUse if used by another client 
         */
		IMPORT_C TInt SupportedProtocols() const;				
		
		/**
		 *	Gets the number of pages to be printed
		 *
		 *	@return Number of pages to print or KErrInUse if used by another client  
		 */
		IMPORT_C TInt GetNumPrintPages() const;
		
		/**
		 *	Gets the status of the printing job created
		 *
		 *	@return Printing job status or KErrInUse if used by another client
		 */
		IMPORT_C TInt GetJobStatus() const;
		
		/**
		 *	Gets the status of the printer
		 *
		 *	@param aPrinterID The exposed ID of the printer the client wants to know about.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt GetPrinterStatus( TInt aPrinterID ) const;
		
		/**
		 *	Cancel discovery
		 *
		 *	@return KErrNone or KErrInUse if used by another client
		 */
		IMPORT_C TInt CancelDiscovery() const;
		
		/**
		 *	Submits a print job already created. The actual job execution is asynchronous so this 
		 *	function returns immediately. If successful, notification of job status and job 
		 *	completion is received by the client through the MPrintEventObserver.
		 *
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt SubmitPrintJob() const;
		
		/**
		 *	Cancels the print job. This is an asynchronous command. MPrintEventObserver::PrintJobProgressEvent 
		 *	will be called with the status of TPrintJobProgressEventStatus::EDone when the cancellation 
		 *	is completed. The notification can potentially be received by the client before 
		 *	this command returns to the client.
		 *
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt CancelPrintJob() const;
		
		/**
		 *	Continues printing and / or submitting a print job.
		 *
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt ContinuePrintJob() const;
		
		/**
		 *	Removes a specified cached printer.
		 *
		 *	@param aPrinterID The exposed ID of the printer the client wants to remove.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt RemoveCachedPrinter( TInt aPrinterID ) const;
		
		/**
		 *	Gets the icon representing a given layout or template.
		 *
		 *	@param aTemplateID ID of the template / layout of interest.
		 *	@param aFbsBitmapHandle Handle to the appropriate bitmap.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt GetJobTemplateIcon( TInt aTemplateID, TInt& aFbsBitmapHandle ) const;
		
		/**
		 *	Gets the number of pages in the preview.
		 *
		 *	@return Number of pages in the preview. 
		 */
		IMPORT_C TInt GetNumPreviewPages() const;
		
		/**
		 *	Sets a print job setting.
		 *
		 *	@param aCapabilityID ID of the capability of interest.
		 *	@param aValue New capability value.
		 *	@param aAffectedCapability ID of any affected capability.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt SetJobSetting( TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability ) const;
		
		/**
		 *	Gets a setting configured for the created print job.
		 *
		 *	@param aCapabilityID ID of the capability of interest.
		 *	@param aValue Capability value.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt GetJobSetting( TInt aCapabilityID, TInt& aValue ) const;
		
		/**
		 *	Gets the detail of a certain capability of a certain printer.
		 *
		 *	@param aPrinterID The exposed ID of the printer the client wants to know about.
		 *	@param aCapabilityID ID of the capability of interest.
		 *	@param aCapability Capability information.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt GetPrinterCapability(TInt aPrinterID, TInt aCapabilityID, TDirectPrintCapability& aCapability) const;
		
		/**
		 *	Gets the IDs of the capabilities supported by the printer specified.
		 *
		 *	@param aPrinterID The exposed ID of the printer the client wants to know about.
		 *	@param aCapabilityIDs Array containing the ID's of the capabilities of the specified printer.
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt GetPrinterCapabilityIDs( TInt aPrinterID, RArray<TInt>& aCapabilityIDs ) const;
		
		/**
		 *  Registers the IdleObserver to send idle event notifications. Asynchronous function.
		 *
		 *  @param aData place holder for idle event. On function completion contains idle event data.
		 *  @param aStatus request status
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
//		IMPORT_C TInt RegisterIdleObserver( TIdleGuardData& aData, TRequestStatus& aStatus );
		
		/**
		 *  Cancel registering idle observer
		 *
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
//		IMPORT_C TInt CancelRegisterIdleObserver() const;
		
		/**
		 *  Start discovery and set request for discovery event. Asynchronous function.
		 *
		 *  @param aData place holder for discovery event. On function completion contains discovery event data.
		 *	@param aProtocols OR:ed protocol ids
		 *  @param aStatus request status
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
//		IMPORT_C TInt StartDiscovery( TDiscoveryGuardData& aData, TUint aProtocols, TRequestStatus& aStatus );
		
		/**
		 *  Set next request for discovery event. Asynchronous function.
		 *
		 *  @param aData place holder for discovery event. On function completion contains discovery event data.
		 *  @param aStatus request status
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
//		IMPORT_C TInt ContinueDiscovery( TDiscoveryGuardData& aData, TRequestStatus& aStatus );

		/**
		 *  Create print job and set request for print job event. Asynchronous function.
		 *
		 *	@param aPrinterID printer id
		 *  @param aData place holder for print job event. On function completion contains print job event data.
		 *	@param aImages array of image file names
		 *  @param aStatus request status
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt CreateJob( TInt aPrinterID, TDirectPrintJobGuardData& aData, RPointerArray<TDesC>& aImages, TRequestStatus& aStatus );
		
		/**
		 *  Set next request for print job event. Asynchronous function.
		 *
		 *  @param aData place holder for print job event. On function completion contains print job event data.
		 *  @param aStatus request status
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt ContinueCreateJob( TDirectPrintJobGuardData& aData, TRequestStatus& aStatus );
		
		/**
		 *  Cancel starting discovery
		 *
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt CancelStartDiscovery() const;
		
		/**
		 *  Cancel creating print job
		 *
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt CancelCreateJob() const;

		/**
		 *	PictBridge USB mode checker
		 *
		 *	@return TInt value more than 0, if PictBridge USB mode is on; 0 if off; negative value if error
		 */
		IMPORT_C TInt IsPictBridgeMode() const;
		
		/**
		 *  Set number of copies
		 *
		 *  @param aArray array of number of copies elements. Each array element corresponds to the array
		 *		of image file names of created print job
		 *  @return Any standard Symbian error code or error codes [ KErrInvalidSequence (-50) / KErrInvalidData (-51) ].
		 */
		IMPORT_C TInt SetNumberOfCopies( const RArray<TInt>& aArray ) const;

		IMPORT_C TInt GetProtocolNames(RSelectItemArray& aNames) const;

	public:

		/**
		 *	Version getter
		 *
		 *	@return version of the client
		 */
		TVersion Version() const;

	private:
		
		TPtr8 iDicsoveryDataPtr;
		TPtr8 iIdleDataPtr;
		TPtr8 iJobDataPtr;
		TDirectPrintCapability* iCapability;
		TPtr8 iNameDataPtr;
	};
	
	
#endif // RDIRECTPRINTCLIENT_H

//  End of File
