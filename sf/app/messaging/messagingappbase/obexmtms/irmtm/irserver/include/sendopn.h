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
// $Workfile: sendopn.h $
// $Author: Stevep $
// $Revision: 10 $
// $Date: 25/03/02 9:25 $
// 
//


#ifndef __SENDOPN_H__
#define __SENDOPN_H__

#include <obexsendop.h>

#include <msvapi.h>
#include <badesca.h> // CDesCArray
#include <obex.h>


//Panics possible during and IR send operation
enum TIrSendOperationPanic
	{
	EIrSendOperationAlreadyActive, //< The current Obex send operation is already active
	EIrSendOperationUnknownJob, //<
	EIrSendOperationUnknownSendState, //< The current value of the send state of the send operation is not found in TObexMtmProgress::TSendState
	EIrSendOperationUnexpectedTimeout //< //< Panic if send via IR has timeout unexpectedly
	};

GLDEF_C void Panic(TIrSendOperationPanic aPanic);
class CIrTimeoutTimer;


//
// CIrSendOperation 
//
class CIrServerSendOperation : public CObexServerSendOperation
/**
 * @class
 *
 * Infrared Send Operation:
 * 
 * Thin implementation over the top of CObexSendOperation. Implements InitialiseObexClientL to initialise a 
 * Obex client using Infrared as the transport mechanism. Runs on the server side.
 */
	{
public:
	
	/**
	 * Second phase constructor.
	 *
	 * @param aSendObj Reference to the object to send.
	 * @param aIrProtocolInfo Protocol info to initialise the CObexCleint
	 * @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
	 * @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
	 * @param aConnectPassword Pointer to the password for Bluetooth validation.
	 * @param aObserverRequestStatus Canonical TRequestStatus for control of active object.
	 * @param aLastSendAttempt TBool flag to check for the second send attempt and also control header sending .  EFalse sends full headers, ETrue only sends name and size.
	 * @leave Leaves if insufficient memory.
	 */

	static CIrServerSendOperation* NewL(CMsvServerEntry& aSendObj, const TObexIrProtocolInfo& aIrProtocolInfo, TInt aConnectTimeoutMicroSeconds, TInt aPutTimeoutMicroseconds, 
											const TDesC* aConnectPassword, TRequestStatus& aObserverRequestStatus, TBool aLastSendAttempt);

public: // from CObexSendOperation

	/**
	 * Initialises the Obex client to use Bluetooth as the transport mechanism with the parameters as specified in the
	 * constructor.
	 *
	 * @leave KErrXXX system wide error codes
	 */

	virtual void InitialiseObexClientL();

private:
	
	/**
	 * Constructor.
	 *
	 * @param aSendObj Reference to the object to send.
	 * @param aIrProtocolInfo Protocol info to initialise the CObexCleint
	 * @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
	 * @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
	 * @param aConnectPassword Pointer to the password for Bluetooth validation.
	 * @param aObserverRequestStatus Canonical TRequestStatus for control of active object.
	 */
	
	CIrServerSendOperation(CMsvServerEntry& aSendObj, const TObexIrProtocolInfo& aIrProtocolInfo, TInt aConnectTimeoutMicroSeconds, TInt aPutTimeoutMicroSeconds, TRequestStatus& aObserverRequestStatus, TBool aLastSendAttempt);
	
	/**
	 * Called by the base class just before the observer is completed.
	 * 
	 * @param aErrorCode thie code that the observer will be completed with
	 * @return TBool ETrue Allow the base class to delete the message.
	 * @return TBool EFalse Do NOT allow the base class to delete the message.
	 *
	 **/
	TBool CompletingObserver(TInt aErrorCode);

private:
	TObexIrProtocolInfo	iIrProtocolInfo;	//< Protocol info to initialise the CObexClient. Non-const because the CObexClient's NewL() takes a non-const reference
	};


#endif // __SENDOPN_H__
