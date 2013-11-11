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
* Description:  Declares CUPPrintingJob class
*
*/


#ifndef CUPPRINTINGJOB_H
#define CUPPRINTINGJOB_H

#include <e32base.h>
#include <upnpitem.h>
#include <upnpmediaserverclient.h>

#include "upconsts.h"
#include "tprintcapability.h"
#include "cupprintfactory.h"
#include "cupjobstate.h"

class CUPPrinter;
class CUPFileSharingActive;

enum TUpnpContainerType
{
	EImageAndVideo = 0
};


#define KMaxIdLength 		10

/*
* @class 		CUPPrintingJob
* @description	This class contains the necessary printing 
*				job information between UPnPStack and MProtPrintingDevice
*/
NONSHARABLE_CLASS(CUPPrintingJob) : public CBase
	{
		public:
			/*
			* @function 	NewL
			* @description	Constructs the CUPPrintingJob object. 
			* 				NOTE: The ownership of aFileFactory does not change here!
			* @param		aPrinter		Selected printer for the job
			* @param		aImages			Array of images to be printed
			* @param		aFileFactory	Pointer to XHTML-print file factory
			* @param		aUserName		User name got from central repository
			*/
			static CUPPrintingJob* NewL(CUPPrinter* aPrinter, RArray<TFileName>& aImages, CUPPrintFactory* aFileFactory, const TDesC8& aUserName);
			
			/*
			* @function 	Destructor
			* @description	Destroys the object and frees the memory allocated by this object
			*/
			~CUPPrintingJob();
		
			/*
			* @function 	SetSessionId
			* @description	Initializes the session id used for printer communication with this job
			* @param		TInt aId
			*/
			void SetSessionId(TInt aId);

			/*
			* @function 	SessionId
			* @description	Returns the session id
			* @return 		TInt
			*/
			TInt SessionId();
			
			/*
			* @function 	SetJobId
			* @description	Initializes the job id
			* @param		TDesC& aId
			*/
			void SetJobId(const TDesC8& aId);

			/*
			* @function 	GetJobId
			* @description	Returns the job id as const TDes8&
			* @param 		const TDes8&
			*/
			void GetJobId(TDes8& aId);
			
			/*
			* @function 	GetJobName
			* @description	Returns the file path
			* @param 		TDes8&
			*/
			void GetJobName(TDes8& aJobName);

			/*
			* @function 	ImageCount
			* @description	Returns the number of images in job.
			* @return 		TInt
			*/
			TInt ImageCount();

			/*
			* @function 	Images
			* @description	Returns job's images as CImageInfo
			* @param		RArray<CImageInfo> Output parameter
			*/
			void Images(RArray<CImageInfo>& aImages);

			/*
			* @function 	PrinterId
			* @description	Returns the printer id
			* @param		TInt
			*/
			TInt PrinterId();
			
			/*
			* @function 	ShareFileL
			* @description	Shares the images via UPnP Media Server and destroys object CUPnPItem.
			* @param		CUPnPItem*	UPnP Item to share
			*/
			void ShareFileL(CUpnpItem* aItemToShare);

			/*
			* @function 	ShareImagesL
			* @description	Shares the images via UPnP Media Server
			*/
			void ShareImagesL();
			
			/*
			* @function 	UnShareImages
			* @description	Removes the sharing of all images from UPnP Media Server
			* @return 		TInt	System wide error code
			*/
			TInt UnShareImages();
			
			/*
			* @function 	UnShareImages
			* @description	Removes the sharing of file from UPnP Media Server
			* @param 		TInt	UPnP item ID
			* @return 		TInt	System wide error code
			*/
			TInt UnShareFile(TInt aItemId);
			
			/*
			* @function 	GetUserName
			* @description	Returns the user name
			* @param 		TPtrC8&
			*/
			void GetUserName(TPtrC8& aDocFormat);
		
			/*
			* @function 	SetNumsOfCopies
			* @description	Sets the number of copies for certain image.
			* @param 		TInt aCapabilityId
			* @param		TInt aValue
			* @return		TInt System wide error code
			*/
			TInt SetNumsOfCopies( const RArray<TInt>& aNumsOfCopies);
			
			/*
			* @function 	XhtmlPrintFile
			* @description	Returns the file path of generated XHTML-Print file
			* @param 		TDes8&
			*/
			void GetPrintFileL(TDes8& aPrintFile);

			/*
			* @function 	Sheets
			* @description	Returns the total number of sheets the job requires
			* @return 		TInt
			*/
			TInt Sheets();
			
			/*
			* @function 	Progress
			* @description	Returns the progress percentage of the job
			* @return 		TInt
			*/
			TInt Progress();
			
			/*
			* @function 	JobState
			* @description	Returns the pointer to the job state maintainer
			* @return 		CUPJobState
			*/
			CUPJobState* JobState();

			/*
			* @function 	JobStateReason
			* @description	Returns the reason for the job's state
			* @return 		TInt
			*/
			TInt JobStateReason();

	        /**
	    	* StateUpdatedResponse callback function DEPRECATED
	        * @since Series60 3.1
			* @param	TDesC8& New printer state
			* @param	TDesC8& Reason for the printer state		
			* @param	TDesC8& List of job ids pending at the printer queue		
			* @param	TDesC8& Current job ID
			* @param	TDesC8& Number of the printed sheets
	        */
			TUPnPState UpdateJobState(const TDesC8& aPrinterState, 
									  const TDesC8& aReason, 	
									  const TDesC8& aJobList, 
									  const TDesC8& aJobId, 
									  const TDesC8& aJobMediaSheetsCompleted);
									  
			/*
			* @function		PrinterStateUpdated
			* @description	Returns the state of the job after printer state updated 
			* @param		TDesC8& New printer state
			* @return		TUPnPState
			*/
			TUPnPState PrinterStateUpdated( const TDesC8& aEvent );
			
			/*
			* @function		PrinterStateReasonsUpdated
			* @description	Returns the state of the job after printer state reason updated 
			* @param		TDesC8& Reason for the printer state		
			* @return		TUPnPState
			*/
			TUPnPState PrinterStateReasonsUpdated( const TDesC8& aEvent );
			
			/*
			* @function		JobIdListUpdated
			* @description	Returns the state of the job after job ID list updated 
			* @param		TDesC8& List of job ids pending at the printer queue		
			* @return		TUPnPState
			*/
			TUPnPState JobIdListUpdated( const TDesC8& aEvent );

			/*
			* @function		JobEndStateUpdatedL
			* @description	Returns the state of the job after job end state updated 
			* @param		TDesC8& The end state of last printed job. Format: 
			*						<id>,<name>,<originator>,<completed sheets>,<state>
			* @return		TUPnPState
			*/
			TUPnPState JobEndStateUpdatedL( const TDesC8& aEvent );

			/*
			* @function		JobMediaSheetsCompletedUpdated
			* @description	Returns the state of the job after media sheets completed list updated 
			* @param		TDesC8& Number of the printed sheets
			* @return		TUPnPState
			*/
			TUPnPState JobMediaSheetsCompletedUpdated( const TDesC8& aEvent );

			/*
			* @function		JobMediaSheetsCompletedUpdated
			* @description	Returns the state of the job after media sheets completed list updated 
			* @param		TDesC8& ID of the current job
			* @param		TDesC8& Number of the printed sheets
			* @return		TUPnPState
			*/
			TUPnPState JobMediaSheetsCompletedUpdated(const TDesC8& aId, const TDesC8& aEvent );

			/*
			* @function		JobAbortStateUpdatedL
			* @description	Returns the state of the job after job abort state updated 
			* @param		TDesC8& The abort state of last aborted job. Format: 
			*						<id>,<name>,<originator>,<completed sheets>,aborted,<reason>
			* @return		TUPnPState
			*/
			TUPnPState JobAbortStateUpdatedL( const TDesC8& aEvent );

			/*
			* @function		ContentCompleteListUpdated
			* @description	Returns the state of the job after content completed list updated 
			* @param		TDesC8& Number of the sheets completedly received in printer
			* @return		TUPnPState
			*/
			TUPnPState ContentCompleteListUpdated( const TDesC8& aEvent );


	
		protected:
			/*
			* @function 	ConstructL
			* @description	Construction's second phase
			* @param		aPriner		Selected printer for the job
			* @param		aImages		Array of images to be printed
			*/
			void ConstructL(CUPPrinter* aPrinter, RArray<TFileName>& aImages, const TDesC8& aUserName);

		private:

			/*
			* @function 	CUPPrintingJob
			* @description	C++ constructor
			* @param		aPrinterId	Id of the printer to be used for this job.
			*
			*/
			CUPPrintingJob(CUPPrintFactory* aFileFactory);
	
			/*
			* @function 	UPnPItemLC
			* @description	Returns the image's UPnP item for file sharing
			* @param		TInt	Index of the image, which UPnP item is requested 
			* @param		TDesC8&	Name of the file used as base for UPnPItem
			* @param		TInt	On return contains the UPnP item ID
			* @return		CUPnPItem*
			*/
			CUpnpItem* UPnPItemLC(TInt aItemId, const TDesC8& aFileName, const TDesC8& aObjectType, const TDesC8& aMimeType);

		private:
		
		//@var iSessionId			Session id used in sending this job to printer
		TInt iSessionId;
	
		//@var iJobId				Id for printing job received from printer control point
		TBuf8<KMaxIdLength> iJobId;
		
		//@var iPrinterId			Defines the printer to be used. Has to be a valid index of CUAPrinterContainer.
		TInt iPrinterId;
		
		//@var iUserName			Name of the user (phone model)
		TPtrC8 iUserName;

		//@var iImages				Job's images
		RArray<CImageInfo> iImages;
		
		//@var iPrintFactory	Object for creating XHTML-Print file
		CUPPrintFactory* iPrintFactory;
		
		//@var iJobState			Object to maintain job's state
		CUPJobState* iJobState;
		
		//@var iSheets				Number of sheets to print
		TInt iSheets;
		
		//@var iXhtmlFileShareId			ID for sharing xhtml file
		TInt iXhtmlFileShareId;

		//@var iFileSharing			File sharing resource
		CUPFileSharingActive* iFileSharing;

		//@var iMediaServerClient			Media server to map the file sharing resource
		RUpnpMediaServerClient iMediaServerClient;
		
		//@var iiMediaServerAddress   media server's ip and port address.
		TFileName8 iMediaServerAddress;
		
		//@var iOrgMediaServerStatus   mediaserver's running status before we use it.
		TInt iOrgMediaServerStatus;
	};

#endif // CUPPRINTINGJOB_H

//  End of File
