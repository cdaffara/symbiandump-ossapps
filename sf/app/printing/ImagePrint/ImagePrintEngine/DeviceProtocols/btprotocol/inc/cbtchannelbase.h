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
* Description:  Defines the CBtChannelBase class.
*
*/


#ifndef CBTCHANNELBASE_H
#define CBTCHANNELBASE_H

#include <e32base.h>
#include <f32file.h>
#include <e32math.h>
#include <obexbase.h> 
#include <btnotif.h>
#include <avkon.hrh>

#include "crsbtdiscoverengine.h"
#include "tprintcapability.h"
#include "mbtpcobserver.h"
#include "cbtobjectserver.h"
#include "cprintjob.h"
#include "mconfirmationnotify.h"
#include "cbtpasswordquery.h"

class CBtSoapCodec;
class CObexBufObject;
class CObexClient;
class CObexFileObject;

/**
* @class 	CBtChannelBase
* @brief	A base class to implement CObexClient use.
*
*			This class implements the common methods used for connecting and
*			communicating with device via CObexClient using either BPP job based
*			printing or OPP simple push.
*/
NONSHARABLE_CLASS(CBtChannelBase) : public CActive, public MObexAuthChallengeHandler, public MConfirmationNotify
	{
		public:

			/**  		~CBtChannelBase
			* @brief	Destructor.
			*/
			virtual ~CBtChannelBase();

			/**  		StartL
			* @brief	Synchronized method to start communication with device.
			*			Method clears former connections and connects to the device given.
			* @param	aDevice Device to connect
			*/
			virtual void StartL( CRsBtDevice& aDevice );

			/**  		Stop
			* @brief	Clears connection.
			*			Device is disconnected and the channel (CObexClient) is deleted.
			*/
			virtual void Stop();

			/**  		IsConnected
			* @brief	Checks wether the channel is connected or not.
			* @return	ETrue if connected, EFalse otherway.
			*/
			virtual TBool IsConnected();

			/**  		Disconnect
			* @brief	Method to disconnect channel.
			*/
			virtual void Disconnect();

			/**  		FinishPrinting
			* @brief	Finishes all printing operations started and stops the connection.
			*/
			virtual void FinishPrinting();

			/**  		SetDevice
			* @brief	Sets the default device.
			* @param	aDevice Device to set as default
			*/
			void SetDevice( CRsBtDevice& aDevice );

			/**  		Device
			* @brief	Gets the default device.
			* @return	Reference to default device.
			*/
			const CRsBtDevice& Device() const;

			/**  		JobId
			* @brief	Gets the default job id.
			* @param	aId Job id to set as default.
			*/
			void SetJobId( TInt aId );

			/**  		JobId
			* @brief	Gets the default job id.
			* @return	Default job id.
			*/
			TInt JobId() const;

       	public:
		    /**  		GetUserPasswordL
		    * @brief	Gets the Obex password.
			* @return	Contain the Realm specified by the unit forcing the Authentication.
			*/
		    virtual void GetUserPasswordL( const TDesC &aRealm );
		    
		    void PasswordQueryCompleteL ( TObexPasskeyBuf aResultPckg );
		
		protected:

			/**  		CBtChannelBase
			* @brief	C++ constructor
			* @param	Observer to call back about received feed back or occured errors.
			*/
			CBtChannelBase(MBtPCObserver& aObs);

			/**  		ConstructL
			* @brief	Symbian second phase constructor.
			*/
			void ConstructL();

			// ===============================
			// From CActive.
			// ===============================
			virtual void DoCancel();
			virtual TInt RunError(TInt aError) = 0;

			/**  		ConnectL
			* @brief	Pure virtual method for CObexClient connection.
			*/
			virtual void ConnectL() = 0;

			/**  		TControlState
			* @brief	Operation states in derived classes.
			*/
			enum TControlState
			{
				EStateNone = 0,
				
            	EStateConnecting,
            	EStateDisconnecting,
				EStateGettingCapabilities,
				EStateGettingPrinterState, 
				EStateCreatingJob, 

				EStateCancellingJob,
				EStateSendingDocument,
				EStateSendingXhtml,
				EStateSendingOPP,
				EStateGettingJobAttributes,

				EStateGetEvent,
				EStateGettingEvent,
				EStateFinish
			};


			/**  		GetSoapResponseL
			* @brief	Gets Soap response and parses the action name
			* @return	Soap action ID (TBtSoapActions)
			*/
			virtual TInt GetSoapResponseL();

			/**  		SendSoapRequestL
			* @brief	Operates Soap Action
			*/
			virtual void SendSoapRequestL(const TInt aRequest, const TInt aJobId = KErrNotFound);
			/// @brief	Overwrites SendSoapRequestL
			virtual void SendSoapRequestL(const TInt aRequest, TRequestStatus& aStatus,
								  		  const TInt aJobId = KErrNotFound);

			/**  		CompleteRequest
			* @brief	Completes pending request
			*/
			void CompleteRequest(TControlState aStateCompleted, TInt aError = KErrNone);

			/**  		Activate
			* @brief	Activates object if not already active
			*/
			virtual void Activate();

			/**  		ClearConnection
			* @brief	Disconnects and deletes the connection.
			*			In case of pending request, calls Abort for CObexClient.
			*/
			virtual void ClearConnection();

			/**  		WaitL
			* @brief	Sets the object to wait request to complete if not already waiting.
			*/
			void WaitL();

			/**  		StopWaiting
			* @brief	Stops the object waiting request to complete if waiting.
			*/
			void StopWaiting();
			
			/**  		DeleteChannel
			* @brief	Deletes the channel and initializes it.
			*/
			void DeleteChannel();

		protected:

			/** @var MBtPCObserver& iObs
			 *  Observer to call back about received feed back or occured errors. */
			MBtPCObserver& iObs;

			/** @var TControlState iState
			 *  Internal container for current operation state. */
			TControlState iState;

			/** @var TInt iJobId
			 *  Current job id. */
			TInt iJobId;

			/** @var CObexClient* iChannel
			 *  Channel to communicate with device over bluetooth. */
			CObexClient* iChannel;

			/** @var CBtSoapCodec* iSoapCodec
			 *  Helper class to codec and manage SOAP messages. */
			CBtSoapCodec* iSoapCodec;

			/** @var CObexBufObject* iObexBufObject
			 *  OBEX data object, type of 'buffer'. */
			CObexBufObject* iObexBufObject;

			/** @var CObexNullObject* iObexNullObject
			 *  Empty OBEX object for connecting. */
			CObexNullObject* iObexNullObject;

			/** @var CBufBase* iObexBody
			 *  Body of the OBEX data object. */
			CBufBase* iObexBody;

			/** @var CRsBtDevice* iDevice
			 *  The device to communicate with. */
			CRsBtDevice* iDevice;

			/** @var CActiveSchedulerWait* iWait
			 *  Active scheduler wait object for nested scheduler loops. */
			CActiveSchedulerWait* iWait;

			/** @var TInt iResponseError
			 *  Member to maintain errors occured during phases of synchronized calls. */
			TInt iResponseError;

			/** @var TBool iStop
			 *  Flag to indicate the processing shall or is about to stop. */
			TBool iStop;
			
			CBtPasswordQuery* iPasswordQuery;
			
		    TBTObexPasskey iPckg;		// Input parameters		
	};

#endif // CBTCHANNELBASE_H

//  End of File
