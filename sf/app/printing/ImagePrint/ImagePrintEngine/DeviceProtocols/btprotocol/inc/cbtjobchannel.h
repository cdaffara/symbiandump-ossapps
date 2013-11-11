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


#ifndef CBTJOBCHANNEL_H
#define CBTJOBCHANNEL_H

#include <e32base.h>
#include <f32file.h>
#include <e32math.h>

#include "tprintcapability.h"
#include "cbtchannelbase.h"
#include "cprintjob.h"

class CBtStatusChannel;
class CObexFileObject;

const TInt KProgressUpdateInterval = 1000000; 

/**
* @class 	CBtJobChannel
* @brief
*
*/
NONSHARABLE_CLASS(CBtJobChannel) : public CBtChannelBase, public MBtTimerObserver
	{
		public:

			/**  		NewL
			* @brief	Creates a new instance of XHTML file composer.
			* @return	Pointer to the instance.
			*/
			static CBtJobChannel* NewL(MBtPCObserver& aObs);

			/**  		NewLC
			* @brief	Creates a new instance of XHTML file composer and leaves it in the cleanup stack.
			* @return	Pointer to the instance.
			*/
			static CBtJobChannel* NewLC(MBtPCObserver& aObs);

			/**  		~CBtJobChannel
			* @brief	Destructor.
			*/
			IMPORT_C ~CBtJobChannel();

			/**  		GetCapabilitiesL
			* @brief Method to get printer capabilities. 
			* 
			* Actually this method gets printer attributes, but this is synchronous
			* method to get only capabilities, not all capabilities.
			*/
			void GetCapabilitiesL(CRsBtDevice& aDevice);

			/**  		CreateJob
			* @brief
			*/
			void CreateJobL(CPrintJob& aJob);

			TInt GetPercentSent();

			/**  		GetJobAttributes
			* @brief
			*/
			void GetPrinterStateL();

			/**  		GetJobAttributes
			* @brief
			*/
			void GetJobAttributesL(TInt aJobId);

	        /**
	        * Cancel a specified print job. Leaves if printer is invalid.
	        * @since Series60 3.0
	        * @param aPrinter Printer to be used.
	        * @param aJobId Print job ID of cancelled job.
	        */
	        void CancelJobL(TInt aJobId );

	        /**
	        * Cancel a specified print job. Leaves if printer is invalid.
	        * @since Series60 3.0
	        * @param aPrinter Printer to be used. 
	        * @param aJobId Print job ID of cancelled job.
	        */
			void GetEventL(TInt aJobId);

			/**  		FinishPrinting
			* @brief	Finish all printing operations started
			*/

		public: // from MBtTimerObserver
			void HandleTimeoutL();

		protected:

			/**  		ConstructL
			* @brief	Symbian second phase constructor.
			*/
			void ConstructL();

			virtual void RunL();
			virtual void DoCancel();
			virtual TInt RunError(TInt aError);

		private:

			/**  		CBtJobChannel
			* @brief	C++ constructor
			*/
			CBtJobChannel(MBtPCObserver& aObs);
			
			void ConnectL();
			void ClearConnection();
			void CleanXhtmlDirL();
			
			TInt CreateJobResponseL();
			TInt GetPrinterStateResponseL();
			TInt GetPrinterCapabilitiesResponseL();
			TInt GetJobAttributesResponseL();
			TInt CancelJobResponseL();
			TInt GetEventResponseL();

			void SendOPPL();
			void SendXhtmlDocumentL();
			
			void DoLeaveL(TInt aError);
			
			/*
			 * Removes timeout from OBEX commands.
			 * */
			void RemoveObexCmdTimeOut();
			
			/*
			 * Callback function of CPeriodic.
			 * */
			static TInt CheckProgress( TAny *aObj );
			
			/*
			 * Calculates and returns size of current print job.
			 * */
			TInt CBtJobChannel::CalculateJobSizeL();
			
			/*
			 * Updates progress bar. 
			 * */
			void UpdateProgress();
			

		private:

			TFileName8 iJobFile;
			TInt iJobFileNum;
			RArray<CImageInfo> iImageList;
			TInt iImageCount;
			TInt iFileObjectSize;

			CObexFileObject* iObexFileObject;
			
			TInt iPercents;
			
			CBtTimer* iTimer;
			CBtStatusChannel* iStatusChannel;
				
			// Periodic timer to update image sending progress.  
			CPeriodic *iUpdateProgress;
	
			TInt iJobSize;					// Size of printing job.
			TInt iProgressOfFullImages;		// Size of full images this far.
	};

#endif // CBTJOBCHANNEL_H

//  End of File

