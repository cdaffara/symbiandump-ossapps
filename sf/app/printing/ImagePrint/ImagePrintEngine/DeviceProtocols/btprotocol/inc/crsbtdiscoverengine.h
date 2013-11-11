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
* Description:  Contains the CRsBtDiscoverEngine class definition and its observer
* 	 		     definition (MBtDiscoveryObserver).
*
*/


#ifndef CRSBTDISCOVERENGINE_H
#define CRSBTDISCOVERENGINE_H

#include <e32base.h>
#include <f32file.h>
#include <bttypes.h>

#include "imageprint.h"
#include "cbttimer.h"

class CBtDiscover;
class CRsBtDevice;
class CBtDeviceContainer;

#define KMaxDaysForCachedDevice 30


/**
  * \brief Observer interface for device discovery or SDP events.
  *
  * The class which owns the CRsBtDiscoverEngine object implements this interface to get
  * asynchronous notifications of Bluetooth device discovery or Service Discovery Protocol
  * events.
  */
class MBtDiscoveryObserver
	{
		public:
			// the values in this enum are subject to change as need crops up
			// the granularity of feedback should be enough to meet UI needs

			enum TEvent
			{
				EFoundBtPrinter,
				ERemoveBtPrinter,
				EDoneFindingPrinters,
				EErrorDuringDiscover,
				EErrorFindingDevice,
				EErrorVerifyingBt
			};
		public:
			virtual void HandleDiscoveryEvent(TInt aEvent,TInt aErrorCode)=0;
	};

/**
  * \brief Brief Description
  *
  * Detailed Description
  */
NONSHARABLE_CLASS( CRsBtDiscoverEngine ): public CActive, public MBtTimerObserver
	{
		public:
			CRsBtDevice& GetDevice();
			~CRsBtDiscoverEngine();
			
			static CRsBtDiscoverEngine* NewL(MBtDiscoveryObserver& aObs);
			void Start(TUint aRequestedProtocols);
			void Stop();
			
		protected:	// from CActive
			void DoCancel();
			void RunL();
			TInt RunError(TInt aError);

		protected:	// from MBtTimerObserver
			void HandleTimeoutL();

		private:

			CRsBtDiscoverEngine(MBtDiscoveryObserver& aObs);
			void ConstructL();

			void StartDiscovery();
			void ContinueDiscovery();
			
			TBool ComposeDeviceL();
			void ResetDevice();
			
			TBool CheckBtDevice();
			
			void ServiceDiscoveryL();
			void StopServiceDiscovery();
		
		private:

			enum TState
				{
				EFindingDevices,
				EGettingServices,
				EDelayFindDevices
				};

		private:
		
			/// State we are currently on ESendingCached, EFindingDevices, ERemovingStale, EVerifyServices, EDelayFindDevices.
			TState iState;
			/// This Object implements the Bluetooth Discovery system calls.
			CBtDiscover *iDiscover;
			/// Discovery Events observer.
			MBtDiscoveryObserver& iObserver;
			/// Indicates the position of the current device.
			CRsBtDevice* iDevice;
			
			RArray<TBTDevAddr> iFoundBeforeArray;
			RTimer iDelayFindDevices;
			/// Bitmask of the protocols requested for printing.
			TUint  iRequestedProtocols;
			CBtTimer* iTimeout;
	};

#endif // CRSBTDISCOVERENGINE_H

//  End of File
