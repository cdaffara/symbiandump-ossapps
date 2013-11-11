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
* Description:  Declares CPrinterControlPoint class
*
*/


#ifndef CPRINTERCONTROLPOINT_H
#define CPRINTERCONTROLPOINT_H

#include <upnpcontrolpoint.h>

#include "mpcpobserver.h"
#include "cjobstateevents.h"
#include "upconsts.h"

class CUpnpDevice;
class CUpnpAction;
class CUpnpHttpMessage;

enum TPrintService
{
	EServiceNone = 0,
	EPrintBasic,
	EPrintEnhanced
};

// CLASS DECLARATION

/**
*  @brief Declares the CPrinterControlPoint
*
*  Prior using the CPrinterControlPoint class, client application has to create 
*  the network connection for the selected IAP. If connection is not up and 
*  running, the CDispatcherEngine class construction will fail. 
*
*  The Printer Control Point starts XHTML folder sharing and printer search when it's constructed.
*
*  See UPnP Print Design document for instuctions.
*
*  @since Series60 2.0
*/
class CPrinterControlPoint : public CUpnpControlPoint//, public MUPnPSecurityObserver
    {
    public: // Constructors and destructors

        /**
        * Create a CPrinterControlPoint object using two phase construction. 
        * @param aPCPObserver
        * @param aFriendlyName 		Specifies the friendly name of the control point.
        * @param aIapId 			IAP to use.
        * @result pointer to created CPrinterControlPoint object.
        */
        IMPORT_C static CPrinterControlPoint* NewL( MPCPObserver& aPCPObserver);

        /**
        * Destructor.
        */
        IMPORT_C ~CPrinterControlPoint();

	public: // From MUPnPSecurityObserver
        /**
        * Function informs when file sharing is ready
        * @since Series 60 3.0        
        */

    public: 
    
        /**
        * Create a print job to the printer. The XHTML-print files must be in folder defined by KUPnPDataPath.
        * @since Series60 3.0
        * @param aPrinter Printer to be used.
        * @param aPrintFile XHTML-Print file's name WITHOUT path (NOTE: path for files defined in KUPnPDataPath)
        * @param aCopies Number of copies of the document to be printed for the job.
        * @param aSides Specifies how pages are to be imposed upon the sides of a selected medium for the job.
        * @param aNumberUp Indicates the number of pages to impose upon a single side.
        * @param aOrientationRequest Indicates the desired orientation for printed pages.
        * @param aMediaSize Identifies the medium size name and dimensions.
        * @param aMediaType Identtifies the medium type.
        * @param aPrintQuality Specifies the print quality requested for the job.
        * @param aPrintBasic Specifies the service to use for printing.
        */
        IMPORT_C TInt CreateJobL( CUpnpDevice* aPrinter, 
                                  const TDesC8& aPrintFile,
                                  const TDesC8& aUserName,
                                  const TDesC8& aCopies, 
                                  const TDesC8& aSides, 
                                  const TDesC8& aNumberUp,
                                  const TDesC8& aOrientationRequest, 
                                  const TDesC8& aMediaSize, 
                                  const TDesC8& aMediaType, 
                                  const TDesC8& aPrintQuality,
                                  TBool aPrintBasic = EFalse );

        /**
        * Cancel a specified print job. Leaves if printer is invalid.
        * @since Series60 3.0
        * @param aPrinter Printer to be used. 
        * @param aJobId Print job ID of cancelled job.
        */
        IMPORT_C void CancelJobL( CUpnpDevice* aPrinter, const TDesC8& aJobId );

        /**
        * Cancel a specified print job. Leaves if printer is invalid.
        * @since Series60 3.0
        * @param aSessionId Print job ID of cancelled job.
        */
        IMPORT_C void CancelJobL( const TInt aSessionId );

        /**
        * This function is used to query printer attributes.
        * @since Series60 3.0
        * @param aPrinter Printer to be used.
        */
        IMPORT_C void GetPrinterAttributesL( CUpnpDevice* aPrinter );

        /**
        * This function is used to query job attributes of specified print job.
        * @since Series60 3.
        * @param aPrinter Printer to be used.
        * @param aJobId Print job ID.
        */
        IMPORT_C void GetJobAttributesL( CUpnpDevice* aPrinter, const TDesC8& aJobId );

        /**
        * This function is used to query margin information from printers supporting PrintEnhanced:1.
        * If printer supports only PrintBasic:1, a function leaves with KErrNotSupported.
        *
        * @since Series60 3.0
        * @param aPrinter Printer to be used.
        * @param aMediaSize
        * @param aMediaType
        */
        IMPORT_C void GetMarginsL( CUpnpDevice* aPrinter, const TDesC8& aMediaSize, const TDesC8& aMediaType );

        /**
        * Allows a client to determine a valid combinations of MediaType and MediaSize that the printer supports.
        * Action is supported only in PrintEnhanced:1.
        * If printer supports only PrintBasic:1, a function leaves with KErrNotSupported.
        *
        * @since Series60 3.0
        * @param aPrinter Printer to be used.
        * @param aMediaSize
        * @param aMediaType
        */
        IMPORT_C void GetMediaListL( CUpnpDevice* aPrinter, const TDesC8& aMediaSize, const TDesC8& aMediaType );


        /**
        * Informs the control point that printing has finished and the event unsubscription can be done.
        * @since Series60 3.
        * @param aPrinter Printer to be used.
        * @param aJobId Print job ID.
        */
        IMPORT_C void PrintingFinishedL( CUpnpDevice* aPrinter );

    
    	// From CControlPoint

        /**
        * StateUpdatedL
        * @since Series60 3.0
        * @param aService the service state updated
        */
        IMPORT_C void StateUpdatedL( CUpnpService* aService );

        /**
        * DeviceDiscoveredL
        * @since Series60 3.0
        * @param aDevice the device found
        */        
        IMPORT_C void DeviceDiscoveredL( CUpnpDevice* aDevice );

        /**
        * DeviceDisappearedL
        * @since Series60 3.0
        * @param aDevice
        */
        IMPORT_C void DeviceDisappearedL( CUpnpDevice* aDevice );

        /**
        * ActionResponseReceivedL
        * @since Series60 3.0
        * @param aAction the action received
        */
        IMPORT_C void ActionResponseReceivedL( CUpnpAction* aAction );

        /**
        * HttpResponseReceivedL
        * @since Series60 3.0
        * @param aMessage the HTTP response message
        */
        IMPORT_C void HttpResponseReceivedL( CUpnpHttpMessage* aMessage );

    private: 
	
        /**
        * First phase of the two phased construction.
        * @param aPCPObserver
        * @param aFriendlyName Specifies the friendly name of the control point.
        */
        CPrinterControlPoint( MPCPObserver& aPCPOBserver );

        /**
        * Runs base classes ConstructL, starts XHTML folder sharing and starts
        * printer search.
        *
        */
        void ConstructL( );

        /**
        * Get service from specified device.
        * @since Series60 3.0
        * @param aPrinter Printer to be used.
        * @param aService Service type
        * @return Pointer to the resolved service
        */
        CUpnpService* GetServiceFromDevice( CUpnpDevice* aPrinter, TPrintService& aService );
        
        /*
        * Function for parsing remote host address from Data Sink URL
        * @param aDataSinkUrl
        * @return Address
        */
        const TPtrC8 DataSinkUrlAddress( const TDesC8& aDataSinkUrl );

        /*
        * Function for parsing path from Data Sink URL
        * @param aDataSinkUrl
        * @return Path
        */
        const TPtrC8 DataSinkUrlPath( const TDesC8& aDataSinkUrl );

        /*
        * Function for parsing port from Data Sink URL
        * @param aDataSinkUrl
        * @return Port number
        */
        TInt DataSinkUrlPort( const TDesC8& aDataSinkUrl );

        /*
        * Sends the CancelJob action to the printer
        * @param aService	Print service, from where the action will be created
        * @param aJobId		Id of the job to be cancelled
        */
		void DoCancelJobL(CUpnpService& aService, const TDesC8& aJobId);
		
		
        /*
        * Function for removing tags (if exists) from actions received from UPnP
        * @param aString	Descriptor to remove tags
        * @return Pointer to the descriptor without tags
        */
		TPtrC8 RemoveTags( const TDesC8& aString);
		
        /*
        * Function for writing the log of job state events
        * @param aService
        */
		void JobStateLog(CUpnpService* aService);

		
    private: // Data
	
        //@var iPCPObserver			PCP observer for callbacks, not owned
	    MPCPObserver& iPCPObserver;

    	//@var iCancelledJobSIDs 	Contains session ids for jobs to be cancelled
		RArray<TInt> iCancelledJobSIDs;
        
        //@var iJobStateEvents		Job state event maintainer	
        CJobStateEvents* iJobStateEvents;
        
};

#endif // CPRINTERCONTROLPOINT_H

// End Of File