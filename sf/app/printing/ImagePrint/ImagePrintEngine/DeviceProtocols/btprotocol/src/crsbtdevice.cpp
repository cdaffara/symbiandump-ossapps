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


#include "crsbtdevice.h"
#include "clog.h"
#include "imageprint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRsBtDevice::CRsBtDevice()
	{
	}

CRsBtDevice::~CRsBtDevice()
	{
	LOG("CRsBtDevice::~CRsBtDevice begin");
	delete iPrinterModel;
	delete iDevice;

	iCapabilities.Close();
	LOG("CRsBtDevice::~CRsBtDevice end");
	}

CRsBtDevice* CRsBtDevice::NewL(const TNameRecord &aName)
	{
	CRsBtDevice* self = CRsBtDevice::NewLC(aName);
	CleanupStack::Pop(self);
	return self;
	}

CRsBtDevice* CRsBtDevice::NewL()
	{
	CRsBtDevice* self = CRsBtDevice::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CRsBtDevice* CRsBtDevice::NewLC(const TNameRecord &aName)
	{
	CRsBtDevice* self = new (ELeave) CRsBtDevice();
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;
	}

CRsBtDevice* CRsBtDevice::NewLC()
	{
	CRsBtDevice* self = new (ELeave) CRsBtDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRsBtDevice* CRsBtDevice::NewLC(const CRsBtDevice &a)
	{
	CRsBtDevice* self = new (ELeave) CRsBtDevice();
	CleanupStack::PushL(self);
	self->ConstructL(a);
	return self;
	}

CRsBtDevice* CRsBtDevice::NewL(const CRsBtDevice &a)
	{
	CRsBtDevice* self = CRsBtDevice::NewLC(a);
	CleanupStack::Pop(self);
	return self;
	}

void CRsBtDevice::ConstructL(const TNameRecord &aName)
	{
	LOG1("CRsBtDevice::ConstructL aName.iName: %S", &(aName.iName));
	TInquirySockAddr sockAddr = (TInquirySockAddr)aName.iAddr;

	TBTDeviceClass devClass(sockAddr.MajorServiceClass(),
			sockAddr.MajorClassOfDevice(),
			sockAddr.MinorClassOfDevice());

	iDevice = CBTDevice::NewL(sockAddr.BTAddr());

	iDevice->SetDeviceClass(devClass);
	iDevice->SetFriendlyNameL(aName.iName);

	Init();

	LOG("CRsBtDevice::ConstructL end");
	}

void CRsBtDevice::ConstructL()
	{
	iDevice = CBTDevice::NewL();
	Init();
	}

void CRsBtDevice::ConstructL(const CRsBtDevice &a)
	{
	iDevice = CBTDevice::NewL();
	if(a.iDevice)
		iDevice->UpdateL(*(a.iDevice));

	iBPPPort = a.iBPPPort;
	iSTSPort = a.iSTSPort;
	iOPPPort = a.iOPPPort;
	iSupportedProtocols = a.iSupportedProtocols;
	iVendor = a.iVendor;
	iJustFound = a.iJustFound;
	iTimeDiscovered = a.iTimeDiscovered;
	iTimeLastUsed = a.iTimeLastUsed;

	iUsed = a.iUsed;

	iPrinterModel = NULL;
	if(a.iPrinterModel)
		iPrinterModel = a.iPrinterModel->AllocL();

	iInternalDeviceId = a.iInternalDeviceId;
	iDisappeared = a.iDisappeared;
	iUsedProtocol = a.iUsedProtocol;

	iCapabilities.Reset();
	for(TInt i = 0; i < a.iCapabilities.Count(); ++i)
		{

		TPrintCapability capab;
		capab.iCapabilityID = a.iCapabilities[i].iCapabilityID;
		capab.iType = a.iCapabilities[i].iType;
		capab.iLow = a.iCapabilities[i].iLow;
		capab.iHigh = a.iCapabilities[i].iHigh;
		capab.iDefaultValue = a.iCapabilities[i].iDefaultValue;
		capab.iEnumCount = a.iCapabilities[i].iEnumCount;

		for(TInt j = 0; j < a.iCapabilities[i].iEnumCount; ++j)
			{
			capab.iEnumCodes[j] = a.iCapabilities[i].iEnumCodes[j];
			}

		AddCapability(capab);
		}
	}

void CRsBtDevice::Init()
	{
	iBPPPort = 0;
	iSTSPort = 0;
	iOPPPort = 0;
	iSupportedProtocols = 0;
	iVendor = TPrinter::EVendorNone;
	iJustFound = EFalse;
	iTimeDiscovered.UniversalTime();
	iTimeLastUsed.UniversalTime();
	iPrinterModel = NULL;

	iInternalDeviceId = KErrNotFound;
	iDisappeared = ETrue;
	iUsedProtocol = 0;

	iCapabilities.Reset();
	iUsed = EFalse;
	}

/**
 * Standard method to allow object to be streamed out to a file.
 * @param aStream Stream that object will be written to, should have already been opened.
 */
void CRsBtDevice::ExternalizeL(RWriteStream& aStream) const
	{
	LOG("[CRsBtDevice::ExternalizeL]\t Begin");
	
	aStream.WriteInt32L(iInternalDeviceId);
	aStream.WriteUint32L(iBPPPort);
	aStream.WriteUint32L(iSTSPort);
	aStream.WriteUint32L(iOPPPort);
	aStream.WriteInt32L(iSupportedProtocols);
	aStream.WriteUint32L(Vendor());
	
	TUint32 temp = iTimeLastUsed.Int64();
	aStream.WriteUint32L( temp );
	temp = iTimeLastUsed.Int64() >> 32;
	aStream.WriteUint32L( temp );
	temp = iTimeDiscovered.Int64();
	aStream.WriteUint32L( temp );
	temp = iTimeDiscovered.Int64() >> 32;
	aStream.WriteUint32L( temp );
	
	
	aStream.WriteInt32L(iPrinterModel->Des().MaxLength());
	aStream << *iPrinterModel;
	
	ExternalizeCapabilitiesL(aStream);
	
	aStream << *iDevice;
	
	LOG("[CRsBtDevice::ExternalizeL]\t end");
	}

void CRsBtDevice::ExternalizeCapabilitiesL(RWriteStream& aStream) const
	{
	LOG("CRsBtDevice::ExternalizeCapabilitiesL begin");
	
	aStream.WriteInt32L(iCapabilities.Count());
	for(TInt i = 0; i < iCapabilities.Count(); ++i)
		{
		aStream.WriteInt32L(iCapabilities[i].iCapabilityID);
		aStream.WriteInt32L(iCapabilities[i].iType);
		aStream.WriteInt32L(iCapabilities[i].iDefaultValue);
		aStream.WriteInt32L(iCapabilities[i].iLow);
		aStream.WriteInt32L(iCapabilities[i].iHigh);
		aStream.WriteInt32L(iCapabilities[i].iEnumCount);
		for(TInt j = 0; j < KMaxEnumAmount; ++j)
			{
			aStream.WriteInt32L(iCapabilities[i].iEnumCodes[j]);
			}
		}
	}


/**
 * Standard method to allow object to be streamed in from a file.
 * @param aStream Stream that object will be read from, should have already been opened.
 */
void CRsBtDevice::InternalizeL(RReadStream& aStream)
	{
	LOG("[CRsBtDevice::InternalizeL]\t Begin");

	TUint timeLastHigh, timeLastLow, timeDiscHigh, timeDiscLow;
	TInt maxlen;

	iInternalDeviceId = aStream.ReadInt32L();
	iBPPPort = aStream.ReadUint32L();
	iSTSPort = aStream.ReadUint32L();
	iOPPPort = aStream.ReadUint32L();

	iSupportedProtocols = aStream.ReadInt32L();
	SetVendor(static_cast<TPrinter::TPrinterVendor>( aStream.ReadUint32L() ));

	timeLastLow = aStream.ReadUint32L();
	timeLastHigh = aStream.ReadUint32L();
	timeDiscLow = aStream.ReadUint32L();
	timeDiscHigh = aStream.ReadUint32L();

	TInt64 temp;
	temp = timeLastHigh;
	temp = temp << 32;
	iTimeLastUsed = temp | timeLastLow;
	temp = timeDiscHigh;
	temp = temp << 32;
	iTimeDiscovered = temp | timeDiscLow;

	delete iPrinterModel;
	iPrinterModel = NULL;
	maxlen = aStream.ReadInt32L();
	iPrinterModel = HBufC::NewL(aStream,maxlen);

	InternalizeCapabilitiesL(aStream);

	iDevice->InternalizeL(aStream);
	iUsed = ETrue;

	LOG1("[CRsBtDevice::InternalizeL]\t End for: %S", &FriendlyName());
	}

void CRsBtDevice::InternalizeCapabilitiesL(RReadStream& aStream)
	{
	LOG("[CRsBtDevice::InternalizeCapabilitiesL]\t begin");

	iCapabilities.Reset();

	TInt count = aStream.ReadInt32L();
	for(TInt i = 0; i < count; ++i)
		{
		TPrintCapability capability;
		capability.iCapabilityID = aStream.ReadInt32L();
		capability.iType = (TPrintCapability::ECapType)aStream.ReadInt32L();
		capability.iDefaultValue = aStream.ReadInt32L();
		capability.iLow = aStream.ReadInt32L();
		capability.iHigh = aStream.ReadInt32L();
		capability.iEnumCount = aStream.ReadInt32L();
		for(TInt j = 0; j < KMaxEnumAmount; ++j)
			{
			capability.iEnumCodes[j] = aStream.ReadInt32L();
			}
		iCapabilities.Append(capability);
		}
	}


/**
 * Create a copy of the current object.
 * \returns A pointer to the new object, the caller takes ownership of the object.
 */
CRsBtDevice* CRsBtDevice::CopyL()
	{
	CRsBtDevice* newDevice = NewLC();
	newDevice->UpdateAllL(*this);
	CleanupStack::Pop(); // newDevice
	return newDevice;
	}

/**
 * Create a copy of the current object.
 * \returns A pointer to the new object, the caller takes ownership of the object.
 */
CRsBtDevice* CRsBtDevice::CopyL(CRsBtDevice& aSource)
	{
	CRsBtDevice* newDevice = NewLC();
	newDevice->UpdateAllL(aSource);
	CleanupStack::Pop(); // newDevice
	return newDevice;
	}

/**
 * \returns Address of descriptor containing the Bluetooth friendly name of the device.
 */
const TDesC& CRsBtDevice::FriendlyName() const
	{
	return iDevice->FriendlyName();
	}

/**
 * Stores the BPP port (channel).
 * @param aPort BPP port (channel) as unsigned integer. If CRsBtDevice::SetBPPPort is not called, initialized as 0.
 */
void CRsBtDevice::SetBPPPort(TUint aPort)
	{
	iBPPPort = aPort;
	}

/**
 * Stores the status port (channel).
 * @param aPort Status port (channel) as unsigned integer. If CRsBtDevice::SetSTSPort is not called, initialized as 0.
 */
void CRsBtDevice::SetSTSPort(TUint aPort)
	{
	iSTSPort = aPort;
	}

/**
 * Stores the OPP port (channel).
 * @param aPort OPP port (channel) as unsigned integer. If CRsBtDevice::SetOPPPort is not called, initialized as 0.
 */
void CRsBtDevice::SetOPPPort(TUint aPort)
	{
	iOPPPort = aPort;
	}


/**
 * Stores the protocols that this device support.
 * @param aSupportedProtocols The Supported Protocols by this device
 */
void CRsBtDevice::SetSupportedProtocols(TUint aSupportedProtocols)
	{
	iSupportedProtocols = aSupportedProtocols;
	}

/**
 * Obtain the supported protocols by this device
 * \returns The supported protocols by this device TUint @sa KImagePrint_PrinterProtocol_BPP @sa KImagePrint_PrinterProtocol_OPP_Printer @sa KImagePrint_PrinterProtocol_OPP_PC
 */
TUint CRsBtDevice::SupportedProtocols() const
	{
	return iSupportedProtocols;
	}

/**
 * Compares the Bluetooth device address of this Bluetooth device to the address of the passed in device.
 * @param aDevice Bluetooth device to compare this object to.
 * \returns True, Bluetooth device addresses are identical, False, Bluetooth device addresses as not identical.
 */
TBool CRsBtDevice::Equals( const CRsBtDevice &aDevice ) const
	{
	return (aDevice.iDevice->BDAddr() == iDevice->BDAddr());
	}

/**
 * \returns Bluetooth device address.
 */
const TBTDevAddr& CRsBtDevice::BDAddr() const
	{
	return iDevice->BDAddr();
	}

/**
 * \returns BPP port (channel) as an unsigned integer. If CRsBtDevice::SetBPPPort is not called, initialized as 0.
 */
TUint CRsBtDevice::BPPPort() const
	{
	return iBPPPort;
	}

/**
 * \returns status port (channel) as an unsigned integer. If CRsBtDevice::SetSTSPort is not called, initialized as 0.
 */
TUint CRsBtDevice::STSPort() const
	{
	return iSTSPort;
	}

/**
 * \returns OPP port (channel) as an unsigned integer. If CRsBtDevice::SetOPPPort is not called, initialized as 0.
 */
TUint CRsBtDevice::OPPPort() const
	{
	return iOPPPort;
	}

void CRsBtDevice::SetDisappeared(TBool aDisappeared)
	{
	iDisappeared = aDisappeared;
	}

TBool CRsBtDevice::IsDisappeared() const
	{
	return iDisappeared;
	}

/**
 * First calls CRsBtDevice::UpdateDeviceAndBPP, then updates any remaining data from the current device,
 * includes whether device is saved in the device cache, the number of prints, and time stamps of
 * discovery and when last used.
 * @param aDevice Device whose data will be used to update current device.
 * \returns True, data of the current device was updated, False, nothing changed.
 */
TBool CRsBtDevice::UpdateAllL(CRsBtDevice &aDevice)
	{
	TBool modified = EFalse;

	modified = UpdateDeviceAndBPPL(aDevice);

	if (iTimeDiscovered != aDevice.iTimeDiscovered)
		{
		iTimeDiscovered = aDevice.iTimeDiscovered;
		modified = ETrue;
		}

	if (iTimeLastUsed != aDevice.iTimeLastUsed)
		{
		iTimeLastUsed = aDevice.iTimeLastUsed;
		modified = ETrue;
		}

	return modified;
	}

/**
 * First calls CRsBtDevice::UpdateDevice, then updates the current devices BPP/OPP data,
 * includes BPP/OPP supported information and BPP/OPP port.
 * @param aDevice Device whose data will be used to update current device.
 * \returns True, data of the current device was updated, False, nothing changed.
 */
TBool CRsBtDevice::UpdateDeviceAndBPPL(CRsBtDevice &aDevice)
	{
	TBool modified = EFalse;

	modified = UpdateDeviceL(aDevice);

	if ((iSupportedProtocols & aDevice.SupportedProtocols()) != iSupportedProtocols)
		{
		iSupportedProtocols = aDevice.SupportedProtocols();
		modified = ETrue;
		}

	if (iBPPPort != aDevice.BPPPort())
		{
		iBPPPort = aDevice.BPPPort();
		modified = ETrue;
		}

	if (iSTSPort != aDevice.STSPort())
		{
		iSTSPort = aDevice.STSPort();
		modified = ETrue;
		}

	if (iOPPPort != aDevice.OPPPort())
		{
		iOPPPort = aDevice.OPPPort();
		modified = ETrue;
		}

	return modified;
	}

/**
 * Update the current devices Bluetooth device data, includes device address, friendly name and device class.
 * @param aDevice Device whose data will be used to update current device.
 * \returns True, data of the current device was updated, False, nothing changed.
 */
TBool CRsBtDevice::UpdateDeviceL(CRsBtDevice &aDevice)
	{
	TBool modified = EFalse;

	if( iDevice->BDAddr() != aDevice.iDevice->BDAddr() ||
			iDevice->FriendlyName() != aDevice.iDevice->FriendlyName() ||
			! (DeviceClass() == aDevice.DeviceClass()) )
		{
		iDevice->UpdateL(*(aDevice.iDevice));
		modified = ETrue;
		}

	if( iPrinterModel )
		{
		delete iPrinterModel;
		iPrinterModel = NULL;
		modified = ETrue;
		}
	if( aDevice.iPrinterModel )
		{
		iPrinterModel = aDevice.iPrinterModel->AllocL();
		modified = ETrue;
		}
	if( Vendor() != aDevice.Vendor() )
		{
		SetVendor(aDevice.Vendor());
		modified = ETrue;
		}

	return modified;
	}

/**
 * First calls CRsBtDevice::UpdateDeviceAndBPP, then updates any remaining data from the current device,
 * includes whether device is saved in the device cache, the number of prints, and time stamps of
 * discovery and when last used.
 * @param aDevice Device whose data will be used to update current device.
 * \returns True, data of the current device was updated, False, nothing changed.
 */
TBool CRsBtDevice::UpdateUsedL(CRsBtDevice &aDevice)
	{
	TBool modified = EFalse;

	modified = UpdateDeviceAndBPPL(aDevice);

	if (iTimeDiscovered != aDevice.TimeDiscovered())
		{
		iTimeDiscovered = aDevice.TimeDiscovered();
		modified = ETrue;
		}

	if (iTimeLastUsed != aDevice.TimeLastUsed())
		{
		iTimeLastUsed = aDevice.TimeLastUsed();
		modified = ETrue;
		}

	if ( !iDisappeared )
		{
		iDisappeared = ETrue;
		modified = ETrue;
		}

	if (iUsedProtocol != (iUsedProtocol & aDevice.SupportedProtocols()) )
		{
		iUsedProtocol = aDevice.UsedProtocol();
		modified = ETrue;
		}

	return modified;
	}


/**
 * \returns The time stamp of when the device was last used.
 */
TTime CRsBtDevice::TimeLastUsed() const
	{
	return iTimeLastUsed;
	}

void CRsBtDevice::SetTimeLastUsed(TTime aTime)
	{
	iTimeLastUsed = aTime;
	}

/**
 * \returns The time stamp of when the device was first discovered.
 */
TTime CRsBtDevice::TimeDiscovered() const
	{
	return iTimeDiscovered;
	}

void CRsBtDevice::SetTimeDiscovered(TTime aTime)
	{
	iTimeDiscovered = aTime;
	}

/**
 * \returns The Bluetooth device class.
 */
TBTDeviceClass CRsBtDevice::DeviceClass() const
{
return iDevice->DeviceClass();
}

void CRsBtDevice::SetPrinterModelL(const TDesC& aModelName)
	{
	LOG1("CRsBtDevice::SetPrinterModelL aModelName: %S", &aModelName);
	HBufC* temp = aModelName.AllocL();
	TPtr ptr = temp->Des();
	ptr.UpperCase();
	delete iPrinterModel;
	iPrinterModel = temp;
	LOG("CRsBtDevice::SetPrinterModelL end");
	}

const TDesC& CRsBtDevice::GetPrinterModel() const
	{
	return *iPrinterModel;
	}

// This is not stored in the cache!
void CRsBtDevice::SetJustFound(TBool aJustFound)
	{
	iJustFound = aJustFound;
	}

// This is not stored in the cache!
TBool CRsBtDevice::IsJustFound() const
	{
	return iJustFound;
	}

TInt CRsBtDevice::DeviceId() const
	{
	return iInternalDeviceId;
	}

void CRsBtDevice::SetDeviceId(TInt aNewId)
	{
	iInternalDeviceId = aNewId;
	}

void CRsBtDevice::SetVendor( TPrinter::TPrinterVendor aVendor )
	{
	LOG1("CRsBtDevice::SetVendor aVendor: %d", aVendor);
	iVendor = aVendor;

	LOG("CRsBtDevice::SetVendor end");
	}

TPrinter::TPrinterVendor CRsBtDevice::Vendor() const
	{
	return iVendor;
	}

// -----------------------------------------------------------------------------
// CRsBtDevice::Used
// -----------------------------------------------------------------------------
TBool CRsBtDevice::IsUsed() const
	{
	return iUsed;
	}

// -----------------------------------------------------------------------------
// CRsBtDevice::SetUsed
// -----------------------------------------------------------------------------
void CRsBtDevice::SetUsed(TBool aUsed)
	{
	iUsed = aUsed;
	}

// -----------------------------------------------------------------------------
// CRsBtDevice::GetUsedProtocol
// -----------------------------------------------------------------------------
//
TInt CRsBtDevice::UsedProtocol() const
	{
	return iUsedProtocol;
	}

// -----------------------------------------------------------------------------
// CRsBtDevice::SetUsedProtocol
// -----------------------------------------------------------------------------
//
TInt CRsBtDevice::ComposeUsedProtocol(TInt aRequestedProtocols)
	{
	LOG2("CRsBtDevice::ComposeUsedProtocol]\t iRequestedProtocols: %d, supported: %d",
			aRequestedProtocols, SupportedProtocols());

	if ((aRequestedProtocols & KImagePrint_PrinterProtocol_BPP) && (SupportedProtocols() & KImagePrint_PrinterProtocol_BPP))
		{
		SetUsedProtocol(KImagePrint_PrinterProtocol_BPP);
		}
	else if ((aRequestedProtocols & KImagePrint_PrinterProtocol_OPP_Printer) && (SupportedProtocols() & KImagePrint_PrinterProtocol_OPP_Printer))
		{
		SetUsedProtocol(KImagePrint_PrinterProtocol_OPP_Printer);
		}
	else if ((aRequestedProtocols & KImagePrint_PrinterProtocol_OPP_PC) && (SupportedProtocols() & KImagePrint_PrinterProtocol_OPP_PC))
		{
		SetUsedProtocol(KImagePrint_PrinterProtocol_OPP_PC);
		}
	else
		{
		// This shouldn't happen at all, we have an error in the code
		LOG("[CRsBtDevice::ComposeUsedProtocol]\t ERROR! Protocol Mismatch.");
		SetUsedProtocol(0);
		}
	return UsedProtocol();
	}

// -----------------------------------------------------------------------------
// CRsBtDevice::SetUsedProtocol
// -----------------------------------------------------------------------------
//
void CRsBtDevice::SetUsedProtocol(TInt aUsedProtocol)
	{
	iUsedProtocol = aUsedProtocol;
	}

// -----------------------------------------------------------------------------
// CRsBtDevice::GetCapabilityIDs
// -----------------------------------------------------------------------------
//
void CRsBtDevice::GetCapabilityIDs(RArray<TInt>& aCapabilityIDs )
	{
	LOG1("[CBtPrintingDevice::GetCapabilityIDs]\t iCapabilities.Count(): %d", iCapabilities.Count());
	for (TInt i=0; i < iCapabilities.Count(); i++)
		{
		aCapabilityIDs.Append(iCapabilities[i].iCapabilityID);
		LOG2("[CBtPrintingDevice::GetCapabilityIDs]\t iCapabilities[%d]: %d", i, iCapabilities[i].iCapabilityID);
		}
	}

// -----------------------------------------------------------------------------
// CRsBtDevice::GetCapability
// -----------------------------------------------------------------------------
//
TInt CRsBtDevice::GetCapability(const TInt aCapabilityID, TPrintCapability& aCapability)
	{
	for (TInt i=0; i < iCapabilities.Count(); i++)
		{
		if (aCapabilityID == iCapabilities[i].iCapabilityID)
			{
			// copy capability to ouput param
			aCapability.iCapabilityID 	= aCapabilityID;
			aCapability.iDefaultValue 	= iCapabilities[i].iDefaultValue;
			aCapability.iType 			= iCapabilities[i].iType;
			aCapability.iEnumCount 		= 0;


			for (TInt j=0; j < iCapabilities[i].iEnumCount; j++)
				{
				aCapability.iEnumCodes[j] = iCapabilities[i].iEnumCodes[j];
				aCapability.iEnumCount++;
				}

			return KErrNone;
			}
		}

	return KErrNotSupported;
	}

//--------------------------------------------------------------------------------------------
// CRsBtDevice::SetDefaultCapability
//--------------------------------------------------------------------------------------------
TInt CRsBtDevice::SetDefaultCapability(TInt aCapabilityID, TInt aValue)
	{
	LOG2("[CBtPrintingDevice::SetDefaultCapability]\t capab: %d, value: %d", aCapabilityID, aValue);

	// Check Validity against the target capability ranges
	for (TInt c=0; c<iCapabilities.Count(); ++c)
		{
		if (iCapabilities[c].iCapabilityID == aCapabilityID)
			{
			if ((iCapabilities[c].iType == TPrintCapability::Int) ||
					(iCapabilities[c].iType == TPrintCapability::Float))
				{
				if (((aValue < iCapabilities[c].iLow) && (iCapabilities[c].iLow != -1)) ||
						((aValue > iCapabilities[c].iHigh) && (iCapabilities[c].iHigh != -1)))
					{
					LOG1("[CBtPrintingDevice::SetJobSettingL]\t capability ranges error; return %d", KErrInvalidData);
					return KErrInvalidData;
					}
				else
					{
					iCapabilities[c].iDefaultValue = aValue;
					LOG1("[CBtPrintingDevice::SetDefaultCapability]\t %d matches", iCapabilities[c].iType);
					return KErrNone;
					}

				}
			else // enum
				{
				for (TInt e = 0; e < iCapabilities[c].iEnumCount; ++e)
					{
					if (aValue == iCapabilities[c].iEnumCodes[e])
						{
						iCapabilities[c].iDefaultValue = aValue;
						LOG("[CBtPrintingDevice::SetDefaultCapability]\t enum matches");
						return KErrNone;
						}
					}
				}
			}
		}
	LOG("[CBtPrintingDevice::SetDefaultCapability]\t capab/value doesn't match");
	return KErrInvalidData;
	}

//--------------------------------------------------------------------------------------------
// CRsBtDevice::AddCapability
//--------------------------------------------------------------------------------------------
void CRsBtDevice::AddCapability(TPrintCapability& aCapability)
	{
	LOG1("[CBtPrintingDevice::AddCapability]\t %d", aCapability.iCapabilityID);

	for (TInt i=0; i < iCapabilities.Count(); i++)
		{
		if (aCapability.iCapabilityID == iCapabilities[i].iCapabilityID)
			{
			iCapabilities[i] = aCapability;
			return;
			}
		}

	iCapabilities.Append(aCapability);
	}

//--------------------------------------------------------------------------------------------
// CRsBtDevice::CapabilityIDCount
//--------------------------------------------------------------------------------------------
TInt CRsBtDevice::CapabilityCount() const
	{
	return iCapabilities.Count();
	}

//--------------------------------------------------------------------------------------------
// CRsBtDevice::GetPrinter
//--------------------------------------------------------------------------------------------
TPrinter CRsBtDevice::ToTPrinter()
	{
	LOG("[CRsBtDevice::GetTPrinter]\t");

	TPrinter printer;
	printer.iDisplayName = FriendlyName();
	if(0 == printer.iDisplayName.Length())
		{
		TBuf<20> tmpName;
		tmpName.AppendNum(DeviceId());
		printer.iDisplayName = tmpName;
		}
	printer.iPrinterID = DeviceId();
	printer.iProtocol = UsedProtocol();
	printer.iVendor = Vendor();


	// Set properties
	printer.iProperties = 0;
	if(IsUsed())
		{
		printer.iProperties |= TPrinter::Cached;
		}
	if(KImagePrint_PrinterProtocol_BPP == printer.iProtocol)
		{
		printer.iProperties |= TPrinter::SupportsPreview;
		}

	return printer;
	}

//  End of File
