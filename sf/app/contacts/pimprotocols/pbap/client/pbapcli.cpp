// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Bluetooth Phone Book Access Profile (PBAP) client side API implementation. This file
// implements the methods used to start and stop the PBAP server listening for 
// phone book client equipment (PCE) requests. 
// 
//

/**
 @file
 @publishedPartner
 @released
*/
#include "pbapcli.h"
#include "clientserver.h"


/**
 Start the server process which lives in its own executable and rendezvous with it.

 @return  KErrNone if successful, or an error code if not.
 */
static TInt StartServer()
	{
	const TUidType serverUid(KNullUid, KNullUid, KPbapServerUid);

	RProcess server;
	TInt r=server.Create(KPbapServerImg,KNullDesC,serverUid);

	if (r==KErrNone)
		{
		TRequestStatus stat;
		server.Rendezvous(stat);
		if (stat!=KRequestPending)
			{
			server.Kill(0);		// abort startup
			}
		else
			{
			server.Resume();	// logon OK - start the server
			}	
		User::WaitForRequest(stat);		// wait for start or death

		// we can't use the 'exit reason' if the server panicked as this
		// is the panic 'reason' and may be '0' which cannot be distinguished
		// from KErrNone
		r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();	
	    server.Close(); 
		}
	return r;
	}


/**
 Standard constructor.
 */
EXPORT_C RPbapSession::RPbapSession()
	{
	}


/**
 Connects the client to the Bluetooth PBAP Server. 

 This must be used before any of the other methods listed in this API
 section. The first client to call this method will cause the initiation of
 the PBAP server within its own executable process. The PBAP Server will
 only allow at most one session to be connected at any one time and any
 subsequent attempts to Connect() to the server will result in KErrInUse 
 being returned.

 Once a connection to the server has completed successfully Start() must be
 called to allow PCE requests.

 @return  KErrNone if successful, a system-wide error code if not. 
 @capability None
 */
EXPORT_C TInt RPbapSession::Connect()
	{
	TInt err = CreateSession(KPbapServerName, Version());

	// we will get an error if the server has not been started yet so check
	if ((err == KErrNotFound) || (err == KErrServerTerminated))
		{
		err = StartServer();
		if ((err == KErrNone) || (err == KErrAlreadyExists))
			{
			// attempt to create the session again
			err = CreateSession(KPbapServerName, Version());
			}
		}
		
	return err;
	}


/**
 Get the PBAP server version.
 */
TVersion RPbapSession::Version() const
	{
	return TVersion(KPbapServerMajorVersionNumber,
					KPbapServerMinorVersionNumber,
					KPbapServerBuildVersionNumber);
	}


/**
 Start listening for PCE requests (via Obex over Bluetooth) in the PBAP server.
 If this request is successful sessions with a PCE can be established

 @return  KErrNone if successful, or an error code if not.
 
 @capability LocalServices
 @capability ReadUserData
 */
EXPORT_C TInt RPbapSession::Start()
	{
	return SendReceive(EPbapStartListening);
	}


/**
 Stop listening for PCE requests to the PBAP server.
 Once this function is called all PCE requests will fail until the next
 successful Start() call. Calling this function during a request
 will result in an Obex disconnect being reported to the client.
 
 @capability LocalServices
 @capability ReadUserData
 */	
EXPORT_C void RPbapSession::Stop()
	{
	SendReceive(EPbapStopListening);
	}


/**
  Sets the password required to access the PBAP server, this can be done 
  before or after a call to RPbapSession::Start.
  When a password is set, a PCE must specify it to access the server. To 
  remove the need for the password, the PBAP server must be stopped and
  re-started.

 @return  KErrNone if successful, or an error code if not.

 @capability LocalServices
 @capability ReadUserData
 */
EXPORT_C TInt RPbapSession::SetPassword(const TDesC& aPassword)
	{
	TIpcArgs args(&aPassword);
	return SendReceive(EPbapSetPassword, args);
	}


/**
 Set a heap mark in the PBAP server thread.
 
 @capability None
 */
EXPORT_C TInt RPbapSession::__DbgMarkHeap()
	{
	return DEBUG_SEND_RECIEVE(EPbapDbgMarkHeap, TIpcArgs(TIpcArgs::ENothing));
	}


/**
 Performs a heap mark check in the PBAP Server thread.
 
 @param aCount  The number of heap cells expected to be allocated at
                the current nest level.
 @capability None
 */
EXPORT_C TInt RPbapSession::__DbgCheckHeap(TInt aCount)
	{
	TIpcArgs args(aCount);
	return DEBUG_SEND_RECIEVE(EPbapDbgCheckHeap, args);
	}


/**
 Perfom a heap mark end check in the PBAP server thread.
 
 @param aCount  The number of heap cells expected to remain allocated
                at the current nest level.
 @capability None
 */
EXPORT_C TInt RPbapSession::__DbgMarkEnd(TInt aCount)
	{
	TIpcArgs args(aCount);
	return DEBUG_SEND_RECIEVE(EPbapDbgMarkEnd, args);
	}


/**
 Set a heap fail next condition in the PBAP server thread.
 
 @param aCount  Determines when the allocation will fail. 
 @capability None
 */
EXPORT_C TInt RPbapSession::__DbgFailNext(TInt aCount)
	{
	TIpcArgs args(aCount);
	return DEBUG_SEND_RECIEVE(EPbapDbgFailNext, args);
	}
