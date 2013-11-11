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
* Description:  Contains the CRsBtDevice class definition.
*
*/


#ifndef CRSBTDEVICE_H
#define CRSBTDEVICE_H

#include <e32base.h>
#include <bt_sock.h>
#include <btdevice.h>

#include "tprinter.h"
#include "btprotocolconsts.h"
#include "tprintcapability.h"

/**
  * @brief Stores information on Bluetooth device.
  *
  * Contains information related to a Bluetooth device, such as a printer. This includes Bluetooth
  * information like device ID and device class, BPP and OPP information like port (channel), number of prints made,
  * and time stamps of discovery and last use.
  */
NONSHARABLE_CLASS( CRsBtDevice ): public CBase
	{
		public:
			TBool UpdateDeviceL(CRsBtDevice &aDevice);
			TBool UpdateDeviceAndBPPL(CRsBtDevice& aDevice);
			TBool UpdateAllL(CRsBtDevice &aDevice);
			TBool UpdateUsedL(CRsBtDevice &aDevice);

			void SetDisappeared(TBool aDisappeared);
			TBool IsDisappeared() const;

			void SetSupportedProtocols(TUint aSupportedProtocols);
			TUint SupportedProtocols() const;

			void SetBPPPort(TUint aPort);
			TUint BPPPort() const;

			void SetSTSPort(TUint aPort);
			TUint STSPort() const;

			void SetOPPPort(TUint aPort);
			TUint OPPPort() const;

			void SetTimeLastUsed(TTime aTime);
			TTime TimeLastUsed() const;

			void SetTimeDiscovered(TTime aTime);
			TTime TimeDiscovered() const;

			const TBTDevAddr& BDAddr() const;
			TBTDeviceClass DeviceClass() const;

			TBool Equals( const CRsBtDevice &aDevice ) const;
			CRsBtDevice* CopyL();
			CRsBtDevice* CopyL(CRsBtDevice& aSource);

			static CRsBtDevice* NewLC(const CRsBtDevice &a);
			static CRsBtDevice* NewLC(const TNameRecord &aName);
			static CRsBtDevice* NewLC();
			static CRsBtDevice* NewL(const CRsBtDevice &a);
			static CRsBtDevice* NewL(const TNameRecord &aName);
			static CRsBtDevice* NewL();

			virtual ~CRsBtDevice();

			void ExternalizeL(RWriteStream& aStream) const;
			void InternalizeL(RReadStream& aStream);

			const TDesC& FriendlyName() const;

			void SetPrinterModelL(const TDesC& aModelName);
			const TDesC& GetPrinterModel() const;

			void SetJustFound(TBool aJustFound);
			TBool IsJustFound() const;

			TInt DeviceId() const;
			void SetDeviceId(TInt aNewId);
			
			void SetVendor( TPrinter::TPrinterVendor aVendor );
			TPrinter::TPrinterVendor Vendor() const;

			void AddCapability(TPrintCapability& aCapability);
			void GetCapabilityIDs(RArray<TInt>& aCapabilityIDs );
			TInt GetCapability(const TInt aCapabilityID, TPrintCapability& aCapability);
			TInt SetDefaultCapability(TInt aCapabilityID, TInt aValue);
			TInt CapabilityCount() const;

			TPrinter ToTPrinter();
			TBool IsUsed() const;
			void SetUsed(TBool aUsed);
			
			TInt UsedProtocol() const;
			TInt ComposeUsedProtocol(TInt aRequestedProtocols);
			void SetUsedProtocol(TInt aProtocol);

		private:
			CRsBtDevice();
			// copy constructor
			CRsBtDevice(const CRsBtDevice &a);
			
			void ConstructL(const TNameRecord &aName);
			void ConstructL(const CRsBtDevice &a);
			void ConstructL();
			void Init();
			void ExternalizeCapabilitiesL(RWriteStream& aStream) const;
			void InternalizeCapabilitiesL(RReadStream& aStream);

		private:
			TTime iTimeLastUsed;
			TTime iTimeDiscovered;
			TUint iBPPPort;
			TUint iSTSPort;
			TUint iOPPPort;
			TUint iSupportedProtocols;
			TPrinter::TPrinterVendor iVendor;
			CBTDevice* iDevice;
			HBufC *iPrinterModel;
			TBool iJustFound; 
			TInt iInternalDeviceId;

			TBool iUsed;			
			TBool iDisappeared;			

			TInt		iUsedProtocol;
			RArray<TPrintCapability> iCapabilities;


	};

#endif // CRSBTDEVICE_H

//  End of File
