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
* Description:  Contains the CBtDiscover class definition. Bluetooth device discovery and SDP.
*
*/


#ifndef CBTDISCOVER_H
#define CBTDISCOVER_H

#include <btextnotifiers.h>
#include <btmanclient.h>
#include <obex.h>
#include <btsdp.h>

#include "e32base.h"
#include "tprinter.h"
#include "btprotocolconsts.h"

/**
  * \brief Implements Bluetooth device discovery and SDP.
  *
  * Implements Bluetooth device discovery and Service Discovery Protocol (SDP).
  * Uses the low-level Symbian interfaces for doing a Bluetooth device discovery,
  * allowing for more flexibility with the user interface. Results of the device discovery and
  * SDP request are stored in private variables until extracted by CRsBTDiscoverEngine
  * which owns the only instance of this class, and directs the asynchronous functionality.
  * \sa CRsBtDiscoverEngine
  */
NONSHARABLE_CLASS(CBtDiscover) : public CBase, public MSdpAgentNotifier
	{
		public:

			static CBtDiscover* NewL();

			virtual ~CBtDiscover();

			void Start(TRequestStatus& aStatus);
			void Stop();
			void Reset();

			TBool IsDoingSDP() const;
			TBool IsFindingDevices() const;
			TBool IsBPPSupported() const;
			TBool IsOPPSupported() const;
			
			void StopSDP();

			void GetNextDevice(TRequestStatus &aStatus);
			void GetSupportedServicesL(TRequestStatus &aStatus, TUint32 aServiceMask = -1);
			
			TBool SupportsContentType(const TDesC &aContType) const;
			void SetContentTypesL(const TPtrC8 &aString);

			void SetPrinterModelL(const TPtrC8 &aString);
			const TDesC& GetPrinterModel() const;

			void SetNameRecord(TNameRecord aNameRecord);
			TNameRecord GetNameRecord() const;

			const TDesC& GetDeviceName() const;
			TBTDeviceClass GetDeviceClass() const;
			
			TInt GetBPPPort() const;
			void SetBPPPort(TInt aPort);
			TInt GetSTSPort() const;
			void SetSTSPort(TInt aPort);
			TInt GetOPPPort() const;
			void SetOPPPort(TInt aPort);
			
			void SetVendor( TPrinter::TPrinterVendor aVendor );
			TPrinter::TPrinterVendor GetVendor() const;

		public: // From MSdpAgentNotifier

			void AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError);
			void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);
			void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue);

		private:
			CBtDiscover();
			void ConstructL();
			
			void StartSDPL();
			void AttributeRequestResultL( TSdpAttributeID aAttrID, CSdpAttrValue* aValue );
			void GetNextAttributeL(TSdpServRecordHandle aHandle);

		public:
			TSdpAttributeID iCurrentSDPAttrID;
			TUint           iCurrentServiceUUID;
			TBool 			iFoundSTS;
			TBool 			iWantedService;

		private:
			// internal flags
			// operation flags
			TBool iDoingSDP;
			TBool iFindingDevices;
			
			CSdpAgent* iAgent;

			TRequestStatus *iRequestStatus;

			CSdpSearchPattern* iSdpSearchPattern;
			CSdpAttrIdMatchList* iMatchList;

			HBufC* iContentTypes;

			RSocketServ iSocketServ;
			RHostResolver iHostResolver;

			// device data
			TNameEntry iNameEntry;
			TInquirySockAddr iSockAddr;
			HBufC* iPrinterModel;
			TPrinter::TPrinterVendor iVendor;
			TInt iBPPPort;
			TInt iSTSPort;
			TInt iOPPPort;
	};


/**
  * \brief Enumerate the values in a SDP response attribute list.
  *
  * Used to parse attributes from the structured data returned by a Bluetooth
  * Service Discovery Protocol request.
  */
class CBTAttribValVisitor : public CBase, public MSdpAttributeValueVisitor
	{
	public:
		static CBTAttribValVisitor* NewLC( CBtDiscover& aDiscover );
		~CBTAttribValVisitor();

	public: // from MSdpAttributeValueVisitor
		void VisitAttributeValueL(CSdpAttrValue& aValue, TSdpElementType aType);
		void StartListL(CSdpAttrValueList& aList);
		void EndListL();

	private:
		CBTAttribValVisitor( CBtDiscover& aDiscover );
		void CheckUuid( const TUUID& aUuid );
		void CheckUint( const TUint& aValue );
		void CheckVendor( const TDesC8& aVendor );

	private:
		CBtDiscover& iDiscover;
	};

#endif // CBTDISCOVER_H

//  End of File
