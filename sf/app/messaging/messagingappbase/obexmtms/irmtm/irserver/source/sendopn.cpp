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
// $Workfile: sendopn.cpp $
// $Author: Stevep $
// $Revision: 8 $
// $Date: 25/03/02 9:25 $
// 
//

//class includes
#include "sendopn.h"

//system includes
#include <msvids.h>
#include <ircmtm.h> //KUidMsgTypeIr

//user includes
#include "irmsgtypeuid.h" //KUidMsgTypeIr



CIrServerSendOperation* CIrServerSendOperation::NewL
                                                (CMsvServerEntry& aSendObj,	const TObexIrProtocolInfo& aIrProtocolInfo,
                                                 TInt aConnectTimeoutMicroSeconds, TInt aPutTimeoutMicroseconds, 
                                                 const TDesC* aConnectPassword, TRequestStatus& aObserverRequestStatus,
                                                 TBool aLastSendAttempt)
/**
 * Second phase constructor.
 *
 * @param aSendObj Reference to the object to send.
 * @param aIrProtocolInfo Protocol info to initialise the CObexCleint
 * @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
 * @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
 * @param aConnectPassword Pointer to the password for Bluetooth validation.
 * @param aObserverRequestStatus Canonical TRequestStatus for control of active object.
 * @param aLastSendAttempt TBool flag to check for the second send attempt and also control header sending.  EFalse sends full headers, ETrue sends only name and size.
 * @leave Leaves if insufficient memory.
 */
	{
	CIrServerSendOperation* self=new(ELeave) CIrServerSendOperation
	                                         (aSendObj, aIrProtocolInfo, aConnectTimeoutMicroSeconds,
	                                          aPutTimeoutMicroseconds, aObserverRequestStatus, aLastSendAttempt);
	CleanupStack::PushL(self);
	self->ConstructL(aConnectPassword);
	CleanupStack::Pop(); //self
	return self;
	}

CIrServerSendOperation::CIrServerSendOperation(CMsvServerEntry& aSendObj, const TObexIrProtocolInfo& aIrProtocolInfo,
                                               TInt aConnectTimeoutMicroSeconds, TInt aPutTimeoutMicroSeconds,
                                               TRequestStatus& aObserverRequestStatus, TBool aLastSendAttempt)
	:CObexServerSendOperation(KUidMsgTypeIrUID, aSendObj, aConnectTimeoutMicroSeconds, aPutTimeoutMicroSeconds,
	                          aObserverRequestStatus,aLastSendAttempt),
		iIrProtocolInfo(aIrProtocolInfo)	//default bitwise copy constructor should suffice
/**
 * Constructor.
 *
 * @param aSendObj Reference to the object to send.
 * @param aIrProtocolInfo Protocol info to initialise the CObexCleint
 * @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
 * @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
 * @param aConnectPassword Pointer to the password for Bluetooth validation.
 * @param aObserverRequestStatus Canonical TRequestStatus for control of active object.
 * @param aLastSendAttempt TBool flag to check for the second send attempt and also control header sending.  EFalse sends full headers, ETrue sends only name and
 */
	{
	}

void CIrServerSendOperation::InitialiseObexClientL()
/**
 * Initialises the Obex client to use Bluetooth as the transport mechanism with the parameters as specified in the
 * constructor.
 *
 * @leave KErrXXX system wide error codes
 */

	{
	// Initialise Obex client object.
	iObexClient = CObexClient::NewL(iIrProtocolInfo);
	}

TBool CIrServerSendOperation::CompletingObserver(TInt aErrorCode)
	{
	// Check if the send operation has completed successfully or 
    // user cancelled the request or if it is the second send attempt . 
    // If so , we will not be attempting a retry.
    if((aErrorCode == KErrNone) || (aErrorCode == KErrCancel ) || (iLastSendAttempt))
		{
		return ETrue;
		}
	return EFalse;
	}

