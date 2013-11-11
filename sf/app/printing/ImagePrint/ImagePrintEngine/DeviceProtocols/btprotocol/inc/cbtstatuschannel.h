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
* Description:  Defines the CBtStatusChannel class.
*
*/


#ifndef CBTSTATUSCHANNEL_H
#define CBTSTATUSCHANNEL_H

#include <e32base.h>
#include <f32file.h>
#include <e32math.h>
#include <obex.h>

#include "crsbtdiscoverengine.h"
#include "tprintcapability.h"
#include "cbtchannelbase.h"
#include "mbtpcobserver.h"
#include "cbtobjectserver.h"
#include "cprintjob.h"

/**
* @class 	CBtStatusChannel
* @brief
*
*/
NONSHARABLE_CLASS( CBtStatusChannel ) : public CBtChannelBase
	{
		public:

			/**  		NewL
			* @brief	Creates a new instance of XHTML file composer.
			* @return	Pointer to the instance.
			*/
			static CBtStatusChannel* NewL(MBtPCObserver& aObs);

			/**  		NewLC
			* @brief	Creates a new instance of XHTML file composer and leaves it in the cleanup stack.
			* @return	Pointer to the instance.
			*/
			static CBtStatusChannel* NewLC(MBtPCObserver& aObs);

			/**  		~CBtStatusChannel
			* @brief	Destructor.
			*/
			~CBtStatusChannel();

			/**  		GetPrinterAttributesL
			* @brief
			*/
			TInt GetPrinterStateL();

			/**  		GetJobAttributes
			* @brief
			*/
			TInt GetJobAttributesL(TInt aJobId);

	        /**
	        * Cancel a specified print job. Leaves if printer is invalid.
	        * @since Series60 3.0
	        * @param aPrinter Printer to be used. 
	        * @param aJobId Print job ID of cancelled job.
	        */
			void GetEventL(TInt aJobId);

		protected:

			/**  		ConstructL
			* @brief	Symbian second phase constructor.
			*/
			void ConstructL();

			void RunL();
			virtual void DoCancel();
			TInt RunError(TInt aError);
			
		private:

			/**  		CBtStatusChannel
			* @brief	C++ constructor
			*/
			CBtStatusChannel(MBtPCObserver& aObs);
			
			void GetPrinterStateResponseL();
			void GetJobAttributesResponseL();
			void CancelJobResponseL();
			void GetEventResponseL();

			/**  		ConnectL
			* @brief	Creates obex connection to the aBtDevice in the iClient 
			*/
			void ConnectL();

			void DoLeaveL(TInt aError);
			
		private:

	};

#endif // CBTSTATUSCHANNEL_H

//  End of File
