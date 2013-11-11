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
* Description:  Defines the CBtPrinterController class.
*
*/


#ifndef CBTPRINTERCONTROLLER_H
#define CBTPRINTERCONTROLLER_H

#include <e32base.h>
#include <f32file.h>
#include <e32math.h>

#include "crsbtdiscoverengine.h"
#include "tprintcapability.h"
#include "mbtpcobserver.h"
#include "cbtobjectserver.h"
#include "cprintjob.h"
#include "cbtonquery.h"
#include "mbtonnotify.h"

class CBtStatusChannel;
class CBtJobChannel;
class CBtSoapCodec;
class CObexBufObject;
class CObexClient;
class CObexFileObject;

NONSHARABLE_CLASS( CBtPrinterController ) : public CBase,
                                            public MBtDiscoveryObserver,
                                            public MBtObjectServerObserver,
                                            public MBTOnNotify
	{
		public:

			/**  		NewL
			* @brief	Creates a new instance of XHTML file composer.
			* @return	Pointer to the instance.
			*/
			static CBtPrinterController* NewL(MBtPCObserver& aObs);

			/**  		NewLC
			* @brief	Creates a new instance of XHTML file composer and leaves it in the cleanup stack.
			* @return	Pointer to the instance.
			*/
			static CBtPrinterController* NewLC(MBtPCObserver& aObs);

			void DeviceDiscovered();
			void DeviceDisappeared();

			/**  		~CBtPrinterController
			* @brief	Destructor.
			*/
			IMPORT_C ~CBtPrinterController();

			TInt GetPercentSent();
			void CancelDiscovery();
			void Stop();
			TBool IsConnected();

			/**  		GetCapabilitiesL
			* @brief Method to get printer capabilities.
			*
			* Actually this method gets printer attributes, but this is synchronous
			* method to get only capabilities, not all capabilities.
			*/
			void GetCapabilitiesL(CRsBtDevice& aDevice);

			/**  		GetPrinterStateL
			* @brief
			*/
			void GetPrinterStateL(CRsBtDevice& aDevice);

			/**  		CreateJob
			* @brief
			*/
			void CreateJobL(CRsBtDevice& aDevice, CPrintJob& aJob);

			/**  		GetJobAttributes
			* @brief
			*/
			void GetJobAttributesL(CRsBtDevice& aDevice, TInt aJobId);

	        /**
	        * Cancel a specified print job. Leaves if printer is invalid.
	        * @since Series60 3.0
	        * @param aPrinter Printer to be used.
	        * @param aJobId Print job ID of cancelled job.
	        */
	        void CancelJobL( CRsBtDevice& aDevice, TInt aJobId );

			/**  		GetEventL
			* @brief
			*/
			void GetEventL(TInt aJobId);

			/**  		StartDiscoveryL
			* @brief	Starts discovering devices
			*/
			void StartDiscoveryL(TUint aProtocol);

			/**  		FinishPrinting
			* @brief	Finish all printing operations started
			*/
			void FinishPrinting();

		public:	// From MRsBtDiscoverEngineObserver

			/**
			 * @brief Get asynchronous notifications of Bluetooth device discovery or Service Discovery Protocol events.
			 *
			 * Possible values for aEvent are of type MRsBtDiscoverEngineObserver::TEvent:
			 * enum TEvent
			 * {
			 *    EFoundBtPrinter,
			 *    ERemoveBtPrinter,
			 *    EDoneFindingPrinters,
			 *    EErrorDuringDiscover,
			 *    EErrorFindingDevice,
			 *    EErrorVerifyingBt
			 * };
			 *
			 * @param aEvent Event that occurred, of type MRsBtDiscoverEngineObserver::TEvent.
			 * @param aErrorCode If the event is an error, error code is sent here.
			 */
			void HandleDiscoveryEvent(TInt aEvent, TInt aErrorCode);

		public: // MBtObexServerObserver

			void HandleObjectServerEvent(TInt aStatus, TInt aInfo = 0);

		public: // MBTOnNotify

		    void BTOnCompleteL( TBool aResult ); 
		
		protected:

			/**  		ConstructL
			* @brief	Symbian second phase constructor.
			*/
			void ConstructL();

		private:

			/**  		CBtPrinterController
			* @brief	C++ constructor
			*/
			CBtPrinterController(MBtPCObserver& aObs);

			void InitJobVariables();

			/**  		ConnectedL
			* @brief	Creates obex connection to the aBtDevice in the iClient
			*/
			void ClearConnections();
			void EstablishObjectChannelL( CRsBtDevice& aDevice );

		private:

			// @var iFinish
			TBool iFinish;

			RArray<CImageInfo> iImageList;
			TInt iJobId;

			MBtPCObserver& iObs;

			CBtJobChannel* iJobChannel;
			CBtObjectServer* iObjectChannel;

			CRsBtDiscoverEngine* iBtDiscoverEngine;

			CRsBtDevice* iDevice;

			TInt iPercents;
			TInt iImageCount;
			TInt iSentImages;
			TInt iServerError;

	        CBtOnQuery* iQuery;
	        TUint iProtocol;

	
	};

#endif // CBTPRINTERCONTROLLER_H

//  End of File

