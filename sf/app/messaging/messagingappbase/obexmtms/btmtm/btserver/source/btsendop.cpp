// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// $Workfile: btsendop.cpp $
// $Author: Stevep $
// $Revision: 10 $
// $Date: 25/03/02 9:32 $
// 
//

//class include
#include "btsendop.h"

//system includes
#include <btmsgtypeuid.h>	//KUidMsgTypeBt
#include "btmtmcmds.h"	//EBtMtmCmdSend
//#include <bttypes.h>

//user includes
//#include "btcmtm.h"	//EBtcCmdSend
//#include "btmsgtypeuid.h" //KUidMsgTypeBt

const TInt KPacketBufferSize=20000; // Packet buffer size

CBtServerSendOperation* CBtServerSendOperation::NewL(CMsvServerEntry& aSendObj,	const TObexBluetoothProtocolInfo& aBtProtocolInfo, TInt aConnectTimeoutMicroSeconds, TInt aPutTimeoutMicroseconds, const TDesC* aConnectPassword, TRequestStatus& aObserverRequestStatus)
/**
 * Second phase constructor.
 *
 * @param aSendObj Reference to the object to send.
 * @param aBtProtocolInfo Protocol info to initialise the CObexCleint
 * @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
 * @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
 * @param aConnectPassword Pointer to the password for Bluetooth validation.
 * @param aObserverRequestStatus Canonical TRequestStatus for control of active object.
 * @leave KErrXXX System-wide error codes
 */
	{
	CBtServerSendOperation* self=new(ELeave) CBtServerSendOperation(aSendObj, aBtProtocolInfo, aConnectTimeoutMicroSeconds, aPutTimeoutMicroseconds, aObserverRequestStatus);
	CleanupStack::PushL(self);
	self->ConstructL(aConnectPassword);
	CleanupStack::Pop(); //self
	return self;
	}

CBtServerSendOperation::CBtServerSendOperation(CMsvServerEntry& aSendObj, const TObexBluetoothProtocolInfo& aBtProtocolInfo, TInt aConnectTimeoutMicroSeconds, TInt aPutTimeoutMicroSeconds, TRequestStatus& aObserverRequestStatus)
	:CObexServerSendOperation(KUidMsgTypeBt, aSendObj, aConnectTimeoutMicroSeconds, aPutTimeoutMicroSeconds, aObserverRequestStatus),
		iBtProtocolInfo(aBtProtocolInfo),iBTObexPortNumber(0)	//default bitwise copy constructor should suffice
/**
 * Constructor.
 *
 * @param aSendObj Reference to the object to send.
 * @param aBtProtocolInfo Protocol info to initialise the CObexCleint
 * @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
 * @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
 * @param aConnectPassword Pointer to the password for Bluetooth validation.
 * @param aTargetID Unsigned Int32 containing target ID if present.
 * @param aSendTargetID TBool for flag to send target ID or not.
 * @param aObserverRequestStatus Canonical TRequestStatus for control of active object.
 */
	{
	}

CBtServerSendOperation::~CBtServerSendOperation()
	{
	delete iSdpUtils;

	iLinkAdapter.Close();
	iSocketServ.Close();
	}

void CBtServerSendOperation::InitialiseObexClientL()
/**
 * Initialises the Obex client to use Bluetooth as the transport mechanism with the parameters as specified in the
 * constructor.
 *
 * @leave KErrXXX System-wide error codes 
 */

	{
	//Prevent the link from going into low power modes before the SDP query takes place. 
	PreventLowPowerModes(); 
	
	// trigger SDP Lookup
	iSdpUtils = CObexSdpUtils::NewL(*this);
	
	User::LeaveIfError(iSdpUtils != NULL);

	TBTDevAddr devAddr = iBtProtocolInfo.iAddr.BTAddr();
	iSdpUtils->RemoteBtObexQueryL(devAddr);

	// tell the obex server module we're async
	iAsyncInit = ETrue;
	}




void CBtServerSendOperation::RemoteBtObexQueryResult(TInt aError, TInt aPortNumber, TBool /*aObexObjectPushProfileSupported*/, 
									 TInt /*aObexObjectPushProfileVersion*/, TObexSupportedFormatsList /*aSupportedFormats*/)
	/**
	 * SDP Query result, parameters have the following meaning:
	 *
	 * @param aError If this is anything but KErrNone then the query has not completed correctly and all other parameters are meaningless.
	 * @param aPortNumber - The RFCOMM port number that the remote device supports OBEX on
	 * @param aObexObjectPushProfileSupported - ETrue if the remote device supports the Bluetooth OBEX Push Profile
	 * @param aObexObjectPushProfileVersion - Version of Bluetooth OBEX Push Profile supported (only valid if aObexObjectPushProfileSupported == ETrue)
	 * @param aSupportedFormats - Structure containing a list of the OBEX Object formats supported by the remote device
	 */
	{

	if(aError)
		{
		// finish the async init at this point
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, aError);
		}
	else
		{
		// remember the port number
		iBTObexPortNumber = aPortNumber;

		// finish the async init at this point
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

void CBtServerSendOperation::SecondPhaseObexClientInitL()
/**
 * Fills in the iBtProtocolInfo structure with the results of the SDP Lookup.
 */
	{
	delete iSdpUtils;
	iSdpUtils = NULL;

	// use the port number to complete initialisation
	TRfcommSockAddr& sockAddr = iBtProtocolInfo.iAddr;
	sockAddr.SetPort(iBTObexPortNumber);

	// setting of receive & transmit packet buffer size.
	TObexProtocolPolicy obexProtocolPolicy;
	obexProtocolPolicy.SetReceiveMtu(KPacketBufferSize);
	obexProtocolPolicy.SetTransmitMtu(KPacketBufferSize);

	// Initialise Obex client object.
	iObexClient = CObexClient::NewL(iBtProtocolInfo,obexProtocolPolicy); 
	}

void CBtServerSendOperation::PreConnectOperations()
	{
	}

void CBtServerSendOperation::PostConnectOperations()
	{
	// If the connection attempt has failed and we are not going to retry the connection,
	// make sure that the link is allowed to go into low power modes.

	const TDesC8* progressDes = NULL;
 	TRAPD(error, progressDes = &ProgressL());

	if (error == KErrNone)
		{
		TPckgBuf<TObexMtmProgress> package;
		package.Copy(*progressDes);

		if (package().iSendState == TObexMtmProgress::ESendError)
			{
			AllowLowPowerModes();
			}
		}
	}

void CBtServerSendOperation::PreSendOperations()
	{
	// Try to ensure the link does not go into a low power mode.
	// Note that we may already be preventing low power modes as a result of attempting
	// to do this prior to the connection setup. In this case, calling it again does
	// nothing.
	// Note that as the obex connection is setup, this should not fail. If for some reason
	// it does, this would result in a slower file transfer.
	PreventLowPowerModes();
	}

void CBtServerSendOperation::PostSendOperations()
	{
	// Allow the link to go into low power modes
	AllowLowPowerModes();
	}

void CBtServerSendOperation::DoCancel()
	{
	// As the transfer is being cancelled, allow the link to go into low power modes
	AllowLowPowerModes();

	delete iSdpUtils;
	iSdpUtils = NULL;

	CObexServerSendOperation::DoCancel();
	}

void CBtServerSendOperation::AllowLowPowerModes()
	{
	// Allow the link to go into low power modes
	if (iPreventingLowPowerModes)
		{
		iLinkAdapter.AllowLowPowerModes(EAnyLowPowerMode);
		iPreventingLowPowerModes = EFalse;
		}

	iLinkAdapter.Close();
	iSocketServ.Close();
	}

void CBtServerSendOperation::PreventLowPowerModes()
	{
	// Prevent the link from going into low power modes. This means the connect and send
	// operations will be much quicker.

	if (iPreventingLowPowerModes)
		{
		return;
		}

	// Connect to the socket server
	if (!iSocketServ.Handle())
		{
		if (iSocketServ.Connect() != KErrNone)
			{
			return;
			}
		}

	// Open the bluetooth link adapter
	if (!iLinkAdapter.IsOpen())
		{
		TBTDevAddr devAddr = iBtProtocolInfo.iAddr.BTAddr();
			
		if (iLinkAdapter.Open(iSocketServ, devAddr) != KErrNone)
			{
			return;
			}
		}

	if (iLinkAdapter.PreventLowPowerModes(EAnyLowPowerMode) == KErrNone)
		{
		iPreventingLowPowerModes = ETrue;
		}
	}
