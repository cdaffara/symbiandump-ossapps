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
// $Workfile: btsendop.h $
// $Author: Stevep $
// $Revision: 8 $
// $Date: 25/03/02 8:56 $
// 
//

#ifndef __BTSENDOP_H__
#define __BTSENDOP_H__

#include <obex.h>	//TObexBluetoothProtocolInfo
#include <obexsendop.h>
#include "../../../obexmtm/obexutil/include/ObexSdpUtils.h" //MObexSdpUtilsObserver

//
// CBtServerSendOperation 
//

class CBtServerSendOperation : public CObexServerSendOperation, public MObexSdpUtilsObserver
/**
class CBtServerSendOperation

Bluetooth Send Operation:

Thin implementation over the top of CObexSendOperation. Implements InitialiseObexClientL to initialise a Obex client
using Bluetooth as the transport mechanism. Runs on the server side.

@internalTechnology
@released
*/
	{
public:

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

	static CBtServerSendOperation* NewL(CMsvServerEntry& aSendObj, const TObexBluetoothProtocolInfo& aBtProtocolInfo, TInt aConnectTimeoutMicroSeconds, TInt aPutTimeoutMicroseconds, const TDesC* aConnectPassword, TRequestStatus& aObserverRequestStatus);

	/**
	 * Destructor
	 **/
	virtual ~CBtServerSendOperation();

public: // from CObexSendOperation

	/**
	 * Initialises the Obex client to use Bluetooth as the transport mechanism with the parameters as specified in the
	 * constructor.
	 *
	 * @leave KErrXXX System-wide error codes 
	 */

	virtual void InitialiseObexClientL();
	
	/**
	 * Fills in the iBtProtocolInfo structure with the results of the SDP Lookup.
	 */	
	virtual void SecondPhaseObexClientInitL();

	/**
	 * Operations to perform before attempting a connection.
	 * As multiple connection attempts can be made, it is necessary for this
	 * routine to ensure it can handle being called multiple times.
	 */
	virtual void PreConnectOperations();

	/**
	 * Operations to perform after attempting a connection.
	 * As multiple connection attempts can be made, it is necessary for this
	 * routine to ensure it can handle being called multiple times.
	 */
	virtual void PostConnectOperations();

	/**
	 * Operations to perform before attempting to send a set of objects.
	 */
	virtual void PreSendOperations();

	/**
	 * Operations to perform after attempting to send a set of objects.
	 */
	virtual void PostSendOperations();

	/**
	 * Cancel
	 */	
	void DoCancel();

private:
	
	/**
	 * Constructor.
	 *
	 * @param aSendObj Reference to the object to send.
	 * @param aBtProtocolInfo Protocol info to initialise the CObexCleint
	 * @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
	 * @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
	 * @param aConnectPassword Pointer to the password for Bluetooth validation.
	 * @param aObserverRequestStatus Canonical TRequestStatus for control of active object.
	 */	

	CBtServerSendOperation(CMsvServerEntry& aSendObj, const TObexBluetoothProtocolInfo& aBtProtocolInfo, TInt aConnectTimeoutMicroSeconds, TInt aPutTimeoutMicroSeconds, TRequestStatus& aObserverRequestStatus);
	
	/**
	 * SDP Query result, parameters have the following meaning:
	 *
	 * @param aError If this is anything but KErrNone then the query has not completed correctly and all other parameters are meaningless.
	 * @param aPortNumber - The RFCOMM port number that the remote device supports OBEX on
	 * @param aObexObjectPushProfileSupported - ETrue if the remote device supports the Bluetooth OBEX Push Profile
	 * @param aObexObjectPushProfileVersion - Version of Bluetooth OBEX Push Profile supported (only valid if aObexObjectPushProfileSupported == ETrue)
	 * @param aSupportedFormats - Structure containing a list of the OBEX Object formats supported by the remote device
	 */

	virtual void RemoteBtObexQueryResult(TInt aError, 
								 TInt aPortNumber, 
								 TBool aObexObjectPushProfileSupported, 
								 TInt aObexObjectPushProfileVersion, 
								 TObexSupportedFormatsList aSupportedFormats);

	/**
	 * Allow the link to go into low power modes
	 */
	void AllowLowPowerModes();

	/**
	 * Prevent the link from going into low power modes
	 */
	void PreventLowPowerModes();

private:
	TObexBluetoothProtocolInfo iBtProtocolInfo;	//< Protocol info to initialise the CObexClient. Non-const because the CObexClient's NewL() takes a non-const reference
	CObexSdpUtils *iSdpUtils; //< Used to perform SDP lookup query
	TInt	iBTObexPortNumber; //< Bluetooth Obex port number
	RSocketServ iSocketServ;
	RBTPhysicalLinkAdapter iLinkAdapter;
	TBool iPreventingLowPowerModes;
	};

#endif // __BTSENDOP_H__
