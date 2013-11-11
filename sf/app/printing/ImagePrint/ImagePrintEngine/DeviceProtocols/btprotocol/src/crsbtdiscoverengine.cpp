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


#include "crsbtdiscoverengine.h"
#include "cbtdiscover.h"
#include "crsbtdevice.h"
#include "rsutils.h"
#include "cbtdevicecontainer.h"
#include "btprotocolconsts.h"
#include "clog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRsBtDiscoverEngine* CRsBtDiscoverEngine::NewL(MBtDiscoveryObserver& aObs)
{
	CRsBtDiscoverEngine* self = new (ELeave) CRsBtDiscoverEngine(aObs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}


CRsBtDiscoverEngine::CRsBtDiscoverEngine(MBtDiscoveryObserver& aObs)
	: CActive(CActive::EPriorityStandard),
	iState(EFindingDevices),
	iObserver(aObs),
	iDevice(NULL),
	iRequestedProtocols(0)
{
		CActiveScheduler::Add(this);
}

/**
 * @brief Second phase constructor.
 *
 * @param aFolderToUser Folder where the device cache will be read/stored.
 */
void CRsBtDiscoverEngine::ConstructL()
	{
	iDiscover = CBtDiscover::NewL();
	iDelayFindDevices.CreateLocal();
	iTimeout = CBtTimer::NewL( *this );
	}

/**
 * @brief Destructor.
 */
CRsBtDiscoverEngine::~CRsBtDiscoverEngine()
	{
	LOG("CRsBtDiscoverEngine::~CRsBtDiscoverEngine begin");
	Cancel();
	delete iDiscover;
	ResetDevice();
	iFoundBeforeArray.Close();
	iDelayFindDevices.Close();
	delete iTimeout;
	LOG("CRsBtDiscoverEngine::~CRsBtDiscoverEngine end");
	}

/**
 * @brief Cancels asynchronous tasks of this active object. Terminates the Device and Service Discovery.
 */
void CRsBtDiscoverEngine::DoCancel()
	{
	LOG("CRsBtDiscoverEngine::DoCancel begin");
	ResetDevice();
	iTimeout->Cancel();
	iDiscover->Stop();
	iDiscover->StopSDP();
	iDelayFindDevices.Cancel();
	iFoundBeforeArray.Reset();
	LOG("CRsBtDiscoverEngine::DoCancel end");
	}

/**
 * @brief Called by the Active Scheduler when an asynchronous call finish.
 */
void CRsBtDiscoverEngine::RunL()
	{
	LOG1("CRsBtDiscoverEngine::RunL iStatus.Int(): %d", iStatus.Int());
	LOG1("CRsBtDiscoverEngine::RunL iState: %d", iState);

	iTimeout->Cancel();

	if( iStatus != KErrNone )
		{
		switch( iState )
			{
			case EDelayFindDevices:
				LOG1("[CRsBtDiscoverEngine::RunL]\t Try to find again... (error %d)", iStatus.Int());
				break;
			case EFindingDevices:
				if( iStatus == (KHCIErrorBase - ECommandDisallowed) )
					{
					LOG("[CRsBtDiscoverEngine::RunL]\t Retry discovery");
					iState = EDelayFindDevices;
					iDelayFindDevices.Cancel();
					iDelayFindDevices.After(iStatus,500000);
					SetActive();
					break;
					}
				else if (iStatus == KErrHostResNoMoreResults)
					{
					LOG("[CRsBtDiscoverEngine::RunL]\t No more devices, all found.");
					ResetDevice();
					//Make call to callback function informing that all is done
					iObserver.HandleDiscoveryEvent(MBtDiscoveryObserver::EDoneFindingPrinters, KErrNone);
					break;
					}
			default:
				LOG1("[CRsBtDiscoverEngine::RunL]\t Leave error %d", iStatus.Int());
				User::Leave(iStatus.Int());
				break;
			}
		}
	else
		{
			switch( iState )
				{
				case EFindingDevices:
					if( CheckBtDevice() )
						{
						TNameRecord name = iDiscover->GetNameRecord();
						TBTDevAddr addr = ((TBTSockAddr)name.iAddr).BTAddr();
						TInt nPos = iFoundBeforeArray.Find( addr );

						if( nPos == KErrNotFound )
							{
							// found new printer, add to list and check service capabilities
							User::LeaveIfError( iFoundBeforeArray.Append( addr ) );
							ServiceDiscoveryL();
							break;
							}
						}
						ContinueDiscovery();
					break;
				case EGettingServices:
					if( ComposeDeviceL() )
						{
						LOG1("CRsBtDiscoverEngine::RunL wanted device: %S", &(iDiscover->GetDeviceName()));
						//Make call to callback function informing that new device has been found.
						iObserver.HandleDiscoveryEvent(MBtDiscoveryObserver::EFoundBtPrinter, iStatus.Int());
						}
					//And search for more devices
					ContinueDiscovery();
					break;

				case EDelayFindDevices:
					StartDiscovery();
					break;
				default:
					break;
			}
		}

	LOG("CRsBtDiscoverEngine::RunL end");
	}

TInt CRsBtDiscoverEngine::RunError(TInt aError)
	{
	LOG1("CRsBtDiscoverEngine::RunError aError: %d", aError);
	Stop();
	
	//Make call to callback function informing that there was an error.
	iObserver.HandleDiscoveryEvent(MBtDiscoveryObserver::EErrorDuringDiscover,iStatus.Int());
	LOG("CRsBtDiscoverEngine::RunError end");
	return KErrNone;
	}

/**
 * @brief Starts the asynchronous discovery of devices.
 *
 * Starts the asynchronous discovery of devices, it will first report
 * the devices that are stored on the cache from a previous run.
 * If no devices were returned from the cache, it will start the
 * Bluetooth discovery process inmediatly.
 *
 * @param aRequestedProtocols The requested protocols.
 */
void CRsBtDiscoverEngine::Start(TUint aRequestedProtocols)
	{
	LOG1("[CRsBtDiscoverEngine::StartL]\t aRequestedProtocols: %d", aRequestedProtocols);
	iRequestedProtocols = aRequestedProtocols;
	iFoundBeforeArray.Reset();

	if(!iDiscover->IsFindingDevices() && !iDiscover->IsDoingSDP())
		{
		LOG("[CRsBtDiscoverEngine::StartL]\t inside IF clause");
		Cancel();
		ResetDevice();

		StartDiscovery();
		}

	LOG("[CRsBtDiscoverEngine::StartL]\t exit");
	}

/**
 * @brief Stops the asynchronous discovery of devices.
 *
 * Starts the asynchronous discovery of devices, it will first report
 * the devices that are stored on the cache from a previous run.
 * If no devices were returned from the cache, it will start the
 * Bluetooth discovery process inmediatly.
 *
 * @param aRequestedProtocols The requested protocols.
 */
void CRsBtDiscoverEngine::Stop()
	{
	LOG("[CRsBtDiscoverEngine::Stop]\t");
	
	if(IsActive())
		Cancel();
	else
		DoCancel();
	
	LOG("[CRsBtDiscoverEngine::Stop]\t exit");
	}

/**
 * @brief Start the asynchronous Bluetooth discovery process.
 *
 * This function calls the method CBtDiscover::FindDevicesL() which
 * will cause RunL() with status EFindingDevices to be called when a
 * device is discovered. All bluetooth devices will be find, and RunL()
 * will determine if it might be a device that we can use.
 *
 * @sa FindMoreDevices()
 */
void CRsBtDiscoverEngine::StartDiscovery()
	{
	LOG("[CRsBtDiscoverEngine::StartDiscovery]\t begin");
	iState = EFindingDevices;
	iDiscover->Start( iStatus );

	LOG1( "[CRsBtDiscoverEngine::StartDiscovery]\t iStatus.Int(): 0x%X", iStatus.Int() );
	if( iStatus == KErrNotSupported )
	{
		// cleanup if error
		iDiscover->Stop();
		TRequestStatus *tmpStat = &iStatus;
		User::RequestComplete(tmpStat, KErrHostResNoMoreResults);
	}
	else
	{
		iTimeout->Cancel();
		iTimeout->Start(KDiscoveryDelay);
	}
	SetActive();
	LOG("[CRsBtDiscoverEngine::StartDiscovery]\t end");
	}

/**
 * @brief Find the next device.
 *
 * It the current state is ESendingCached this will call
 * ReportCachedDeviceL() again to fetch the next cached device.
 *
 * If the state is ERemovingStale it will call
 * RemoveStalePrintersL() again to remove the next stale device.
 *
 * Otherwise, it will call CBtDiscover::FindMoreDevices() to
 * discover another device.
 */
void CRsBtDiscoverEngine::ContinueDiscovery()
	{
	LOG1("[CRsBtDiscoverEngine::ContinueDiscoveryL]\t begin iState: %d", iState);

	Cancel();

	iState = EFindingDevices;
	iDiscover->GetNextDevice(iStatus);
	iTimeout->Cancel();
	iTimeout->Start(KDiscoveryDelay);
	SetActive();

	}

void CRsBtDiscoverEngine::HandleTimeoutL()
	{
	LOG("[CRsBtDiscoverEngine::HandleTimeoutL]\t begin");

	Stop();
	if (iState == EGettingServices && IsActive())
	{
		LOG("[CRsBtDiscoverEngine::StopServiceDiscovery]\t complete iStatus IS IT OK???");
		TRequestStatus *tmpStat = &iStatus;
		User::RequestComplete(tmpStat, KErrNone);
	}
	iObserver.HandleDiscoveryEvent(MBtDiscoveryObserver::EDoneFindingPrinters, KErrNone);

	LOG("[CRsBtDiscoverEngine::HandleTimeoutL]\t end");
	}

/**
 * @brief Determine if the BT Device could be a device we need.
 *
 * When any BT Device is discovered RunL() will be called, and it
 * will use this function to do a first check on the device to see
 * if it is a device that we could use, or if it is a device that
 * is totally unrelated to printing and we can ignore rigth away.
 *
 * Currently we are checking if the device is either a Rendering
 * device or a Computer device. If it is a computer it should support
 * Object Transfers to be useful.
 *
 * With this first check we can't be 100% sure if this is a device
 * that we can use. If it is a potentially useful device we will need
 * to do a Service Discovery (SDP) check later.
 *
 * @return ETrue if this is an appropiate BT Device, EFalse if this device must be ignored.
 *
 * @sa ServiceDiscoveryL()
 */
TBool CRsBtDiscoverEngine::CheckBtDevice()
	{
	// Algorithm to understand Device / Service Class bits

	//1. Are "Rendering"0x40000 and "Object Transfer"0x100000 bits set in the Major Service
	//   Class field? If not, check if it is a Computer and Supports Object Transfer.
	//2. If it is a Rendering device and the Major Device Class is "Imaging"0x600, is the "Printer"0x80 bit set in
	//   the Minor Device Class field? If not, ignore this Bluetooth device.
	//3. If the device was a Computer and it supports Object Transfer, check that
	//   the MinorDeviceClass Field is any of  Desktop | Laptop | Server.

	// http://www.bluetooth.org/assigned-numbers/baseband.htm
	// http://www.bluetooth.org/assigned-numbers/sdp.htm

	TUint32 devClass = iDiscover->GetDeviceClass().DeviceClass();
	LOG1("[CRsBtDiscoverEngine::CheckBtDevice]\t devClass: %d", devClass);

	TBool potentialDevice = EFalse;
	
	if( !(devClass & KBTMjrSvcClassObjectTransfer_V2) )
	{
		LOG("[CRsBtDiscoverEngine::CheckBtDevice]\t !KBTMjrSvcClassObjectTransfer_V2 -> return");
		return potentialDevice;
	}
	
	if (devClass & KBTMjrSvcClassRendering_V2)
	{
		if( (devClass & KBTMjrDevClassImaging_V2) && !(devClass & KBTMinDevClassPrinter_V2) )
		{
			// Other imaging device
			LOG("[CRsBtDiscoverEngine::CheckBtDevice]\t other imaging device");
			potentialDevice = EFalse;
		}
		else
		{
			// Possibly a BPP printer,
			LOG("[CRsBtDiscoverEngine::CheckBtDevice]\t KBTMjrSvcClassRendering_V2 and KBTMjrSvcClassObjectTransfer_V2");
			potentialDevice = ETrue;
		}
	}
	else if(devClass & KBTMjrDevClassComputer)
	{
		// Note: If it is a Rendering Device, it could support BPP or OPP or both. So we will need to do a SDP to know.
		// On the other hand, if the Device is of Class Computer, we can check here if we want the device or not
		// by checking if iRequestedProtocols contains KImagePrint_PrinterProtocol_OPP_PC. If not, then we don't want
		// any Computer Devices.

		
		if ( (iRequestedProtocols & KImagePrint_PrinterProtocol_OPP_PC) &&
		     ( (devClass & KBTMinDevClassDesktop) || 
		       (devClass & KBTMinDevClassServer) || 
		       (devClass & KBTMinDevClassLaptop) ) )
		{
			LOG("[CRsBtDiscoverEngine::CheckBtDevice] OPP Computers were requested and this is a computer");
			potentialDevice = ETrue;
		}
	}
	
	LOG1("[CRsBtDiscoverEngine::CheckBtDevice]\t potentialDevice: %d", potentialDevice);
	return potentialDevice;
	}

/**
 * @brief Start SDP (Service Discovery Protocol) on a device.
 *
 * Start SDP (Service Discovery Protocol) on a device to make sure that
 * it supports the services needed by this system. The Services that
 * we are looking for are DirectPrinting (BPP) and OBEX Object Push (OPP).
 *
 * This method will only call CBtDiscover::VerifyServiceSupportL() which
 * is an asynchronous call. RunL() will be called back when the results are
 * ready.
 */
void CRsBtDiscoverEngine::ServiceDiscoveryL()
	{
		iDiscover->GetSupportedServicesL(iStatus, KBtMaskObexLevel);
		iState = EGettingServices;
		SetActive();
	}

void CRsBtDiscoverEngine::StopServiceDiscovery()
{
	LOG1("[CRsBtDiscoverEngine::StopServiceDiscovery]\t iState %d",iState);
	if (iState == EGettingServices && IsActive())
	{
		LOG("[CRsBtDiscoverEngine::StopServiceDiscovery]\t complete iStatus IS IT OK???");
		TRequestStatus *tmpStat = &iStatus;
		User::RequestComplete(tmpStat, KErrNone);
	}
	iDiscover->StopSDP();
}

/**
 * @brief Compose the device.
 *
 * When a Service Discovery has finished, we need to see
 * if the services that we want were found on the device.
 * This function will also tell if this device supports a
 * protocol that was requested.
 *
 * If a device was determined to support BPP and/or OPP it 
 * supports a protocol this library can manage.
 *
 * For example:
 * If the libray is requesting devices that support
 * the protocol KImagePrint_PrinterProtocol_OPP_Printer and the
 * current device supports only KImagePrint_PrinterProtocol_BPP
 * then this device will be handled because this library
 * can manage BPP Printers. But this device will not be used because
 * the library is currently requesting only the OPP_Printer protocol.
 * In this case ComposeDeviceL() will return EFalse, indicating that
 * we won't use this device.
 *
 * @return ETrue if this is a device that we can use, EFalse if this device cannot be used.
 */
TBool CRsBtDiscoverEngine::ComposeDeviceL()
	{
	LOG("[CRsBtDiscoverEngine::ComposeDeviceL]\t begin");

	TBool wantedDevice( EFalse );
	TNameRecord name = iDiscover->GetNameRecord();
	ResetDevice();
	CRsBtDevice *device = CRsBtDevice::NewLC( name );

	TUint supportedProtocols( 0 );
	if( iDiscover->IsBPPSupported() )
		{
		LOG("[CRsBtDiscoverEngine::ComposeDeviceL]\t BPP supported");
		// We indicate that this device does support this protocol
		supportedProtocols = KImagePrint_PrinterProtocol_BPP;
		// But we still need to see if that protocol was requested.
		if( iRequestedProtocols & KImagePrint_PrinterProtocol_BPP )
			{
			device->SetBPPPort( iDiscover->GetBPPPort() );
			device->SetSTSPort( iDiscover->GetSTSPort() );
			wantedDevice = ETrue;
			}
		}

	if( iDiscover->IsOPPSupported() )
		{
		// It must be either a Computer or a Printer, but NOT both!
		TBTDeviceClass deviceClass = device->DeviceClass();

		if( deviceClass.DeviceClass() & KBTMjrDevClassComputer )
			{
			LOG("[CRsBtDiscoverEngine::ComposeDeviceL]\t OPP computer");
			supportedProtocols |= KImagePrint_PrinterProtocol_OPP_PC;
			if( iRequestedProtocols & KImagePrint_PrinterProtocol_OPP_PC )
				{
				device->SetOPPPort(iDiscover->GetOPPPort());
				wantedDevice = ETrue;
				}
			}
		else
			{
			LOG("[CRsBtDiscoverEngine::ComposeDeviceL]\t OPP printer(?)");
			supportedProtocols |= KImagePrint_PrinterProtocol_OPP_Printer;
			if( iRequestedProtocols & KImagePrint_PrinterProtocol_OPP_Printer )
				{
				device->SetOPPPort(iDiscover->GetOPPPort());
				wantedDevice = ETrue;
				}
			}
		}

	// Store if wanted device
	if( wantedDevice )
		{
		LOG1("[CRsBtDiscoverEngine::ComposeDeviceL]\t iRequestedProtocols: %d", iRequestedProtocols);
		LOG1("[CRsBtDiscoverEngine::ComposeDeviceL]\t supportedProtocols: %d", supportedProtocols);
		device->SetSupportedProtocols(supportedProtocols);
		device->SetPrinterModelL(iDiscover->GetPrinterModel());
		device->SetVendor(iDiscover->GetVendor());
		device->SetJustFound(ETrue);

		iDevice = device;
		CleanupStack::Pop(device);
		device = NULL;
		LOG1("[CRsBtDiscoverEngine::ComposeDeviceL]\t iDevice: %d", iDevice);
		}
	else
		{
		LOG("[CRsBtDiscoverEngine::ComposeDeviceL]\t NOT wanted device, don't store");
		CleanupStack::PopAndDestroy(device);
		}

	// We have now added this device to the cache, along with the protocols
	// that it supports. However we wont report it if it does not support
	// the specific protocol that was requested.

	LOG1("[CRsBtDiscoverEngine::ComposeDeviceL]\t end with: %d", wantedDevice);
	return wantedDevice;
	}

/**
 * @brief Obtain the current device.
 *
 * @param[out] aSequence Will obtain the position the current device.
 *
 * @return A reference to the current CRsBtDevice.
 */
CRsBtDevice& CRsBtDiscoverEngine::GetDevice()
	{
	return *iDevice;
	}

void CRsBtDiscoverEngine::ResetDevice()
{
	if(iDevice)
		{
		delete iDevice;
		iDevice = NULL;
		}
}

//  End of File

