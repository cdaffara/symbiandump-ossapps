// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// obex_server.cpp [stub]
// oscarg, September 2003.
// This is a modified version of Obex_Server.cpp. 
// Modified file for BT/IR OBEX stub
// 
//


#include <charconv.h>
#include <utf.h>
#include "debug.h"
#include "trans.h"
#include "obex.h"
#include "obexutil.h"
#ifdef TRANSPORT_IRDA
	#include "irdattp.h"
#endif //TRANSPORT_IRDA
#include "authentication.h"



//
// class CObexServer
//

CObexServer::CObexServer()
	{
	iCurrentOperation = EOpIdle;
	iEnabled = EFalse;
	iOwner = NULL;
	iSpecObject = NULL;
    //iObexProtocolInfoPtr = aObexProtocolInfoPtr;

	//the connectionID is fixed at the moment
	ResetConnectionID();
	SetConnectionID(0xc30fa596);
	iTargetChecking = EIfPresent;
	}

void CObexServer::ConstructL(TObexProtocolInfo& aObexProtocolInfoPtr)
	{
	CObex::ConstructL(aObexProtocolInfoPtr);
	iSpecObject = CObexNullObject::NewL();

	}
	

EXPORT_C TInt CObexServer::SetPutFinalResponseHeaders(CObexHeaderSet* aHeaderSet)
   {
   aHeaderSet = NULL;
   return 0;
   } 
    

CObexServer::~CObexServer()
	{
	FLOG(_L("CObexServer Destructor\r\n"));
	Stop();
	delete iSpecObject;
	}

void CObexServer::ResetConnectionID()
	{
	iConnectionID = 0;
	iConnectionIdSet = EFalse;
	}

void CObexServer::SetConnectionID(TUint32 aConnectionID)
	{
	iConnectionID = aConnectionID;
	iConnectionIdSet = ETrue;
	}

TUint32 CObexServer::ConnectionID()
	{
	return (iConnectionID);
	}

TInt CObexServer::PrepareFinalChallResponse(TObexPacket& /*aPacket*/, TConnectState& /*aNextState*/)
	{
	return KErrNone;
	}


EXPORT_C void CObexServer::UserPasswordL(const TDesC& aPassword)
/** A call back from the the service with the password required for use with generating 
the challenge response. 

@param aPassword Password 

@publishedAll
@released
*/
	{
	//now have a password, get a nonce, and get it hashed then reply
	if (ConnectState() == EWaitForUserInput)
		{
		FLOG(_L("CObexServer::UserPasswordL\r\n"));
		PrepareChallResponseL( aPassword);
		FLOG(_L("UserPasswordL - PrepareChallResponse Success\r\n"));

		TObexInternalHeader hdr;
		hdr.Set(TObexInternalHeader::EAuthResponse, (const_cast<TUint8*> (iOutgoingChallResp.Ptr())), iOutgoingChallResp.Size());
		if(iTransport->SendPacket().InsertData(hdr))
			{
			FLOG(_L("UserPasswordL Inserting EAuthResponse Header\r\n"));

			SetConnectState(EConnObex); //all finished
			iTransport->SendPacket().SetFinal();
			iTransport->Send();
			//inform the client that the connection was succesfull
			iOwner->ObexConnectIndication(iRemoteInfo, TPtr8(NULL, 0));
			}
		else
			{
			User::Leave(KErrGeneral);
			}
		}
	}


TInt CObexServer::AddConnectionIDHeader(TObexPacket& /*aPacket*/)
	{
	return KErrNone;
	}


EXPORT_C  CObexServer* CObexServer::NewL(TObexProtocolInfo& aObexProtocolInfoPtr)
/** Allocates and constructs a new OBEX server object.

The passed protocol information object specifies what transport protocol to 
use. For IrDA, pass a TObexIrProtocolInfo; for Bluetooth, pass a TObexBluetoothProtocolInfo.

@param aObexProtocolInfoPtr Protocol information
@return New OBEX server object 

@publishedAll
@released
*/
	{
	CObexServer* self = new(ELeave) CObexServer();
	CleanupStack::PushL(self);
	self->ConstructL(aObexProtocolInfoPtr);
	CleanupStack::Pop();
	return(self);
	}


EXPORT_C TInt CObexServer::Start(MObexServerNotify* /*aOwner*/)
/** Starts the server, specifying a notification interface.

If the server is already started, no state changes occur (i.e. any connections/operations 
in progress are not interrupted), but the notifications will be sent to aOwner. 
This allows "child" servers to take over ownership of existing connections. 

Details of this function behaviour depend on the transport specified when 
constructed: in general a listener socket is created, its port number registered 
as appropriate, and an accept queued.

@param aOwner Server notification interface
@return System wide error code 

@publishedAll
@released
*/
	{
	return KErrNone;
	}

EXPORT_C void CObexServer::Stop()
/** Disconnects any transfer in progress and disables further connections. 

@publishedAll
@released
*/
	{
	}

TInt CObexServer::AcceptConnection()
	{
	return KErrNone;
	}



EXPORT_C void CObexServer::SetChallengeL(const TDesC& aPassword)
/** Sets a password required to access the server.

When a password is set, a client must specify it to access the server.

@param aPassword Password 

@publishedAll
@released
*/
	{
	FLOG(_L("CObexServer::SetChallengeL\r\n"));

	delete iChallPassword;
	iChallPassword = NULL;
	iChallPassword = HBufC8::NewL(aPassword.Length());
	TPtr8 ptr = iChallPassword->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr, aPassword);
	iChallenge = ETrue;
	}

EXPORT_C void CObexServer::ResetChallenge()
/** Resets the password.

After this call, a client does not need to provide a password to access the 
server. 

@publishedAll
@released
*/
	{
	FLOG(_L("CObexServer::ResetChallengeL\r\n"));
	delete iChallPassword;
	iChallPassword = NULL;
	iChallenge = EFalse;
	}
EXPORT_C void CObexServer::SetTargetChecking(TTargetChecking aChecking)
/**
	@publishedAll
	
	Specifies target header checking behaviour.
	
	Supports three behaviours---never check, always check, and check only if a target
	header has been sent.  The default behaviour is to only check when a target header
	has been sent.
	
	No checking allows a form of multiplexing to be used, where one server object may
	respond to multiple target headers.  The behaviour desired by the client can be
	determined by examining the target header specified in the Connect.
	
	@param aChecking The desired level of target header checking.
 */
	{
	iTargetChecking = aChecking;
	}


//ConnectionID and Who headers are Mandatory if the Target header was used 
//in the connection from
TInt CObexServer::PrepareConnectPacket(TObexPacket& /*aPacket*/)
	{
	return KErrNone;
	}

void CObexServer::CheckTarget(TConnectState& aNextState, TInt& aRetVal)
	{
	FLOG(_L("Local Who:\r\n"));
	FTRACE(FHex(iLocalInfo.iWho));
	FLOG(_L("Target:\r\n"));
	FTRACE(FHex(iRemoteInfo.iTargetHeader));
	
	// Workaround for bug with PocketPC 2002---if target header is sixteen bytes of zeros, connect anyway.
	_LIT8(KZeroTarget, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");

	// Allow connection iff:
	//    PocketPC attempting to connect to inbox (Sixteen bytes of zeros), when no LocalWho
	// or Target header matches LocalWho (includes Inbox connections)
	
	if (!(	// Negate as block below discards connection
		    ((iLocalInfo.iWho == KNullDesC8) && (iRemoteInfo.iTargetHeader == KZeroTarget))
		 || ( iLocalInfo.iWho == iRemoteInfo.iTargetHeader)
	   ))
		{
		FLOG(_L("ParseConnectPacket ETarget header doesn't match local iWho, dropping link\r\n"));
		aNextState = EConnTransport;
		aRetVal = ERespNotFound;
		}
	}


//if the Target header is sent then it must match the local iWho field
TInt CObexServer::ParseConnectPacket(TObexPacket& /*aPacket*/)
	{
	return KErrNone;
	}

TBool CObexServer::CheckForConnectionID(CObexBaseObject& /*aObject*/)
	{
	return ETrue;
	}

void CObexServer::OnPacketReceive(TObexPacket& /*aPacket*/)
	{
	}

void CObexServer::OnError(TInt aError)
	{
	FTRACE(FPrint(_L("OnError aError: %d iCurrentOperation: %d"), aError, iCurrentOperation));

	if (aError == KErrDisconnected)
		{
		if ((iCurrentOperation != EOpDisconnect) && (iConnectState > EConnTransport))
			{
			 //extended error for IrObex,("peer device aborted data transmission/obex sending") 
			iOwner->ErrorIndication(KErrIrObexServerPutPeerAborted);
			}
		}
	else
		{
		iOwner->ErrorIndication(aError);
		}
	}

void CObexServer::OnTransportUp()
	{
	iTargetReceived = EFalse;

	iOwner->TransportUpIndication();
	}

/**
 * Tell the MObexServerNotify observer the transport is down and listen
 * for another connection.
 */
void CObexServer::OnTransportDown()
	{// Cancel Anything waiting. Restart the accepter	
	iOwner->TransportDownIndication();
	TInt err = AcceptConnection();
	if(err != KErrNone)
		Error(err);
	}
	
	
// CObexServer
EXPORT_C TBool CObexServer::IsStarted() 
/**
	@return ETrue	server has had a Start issued, and as is available to 
					accept connections
	@return EFalse	otherwise

@publishedAll
@released
**/
	{
	return iEnabled;
	}

EXPORT_C CObex::TOperation CObexServer::CurrentOperation() const 
/**
	Returns the operation currently being performed by the remote client, or 
	EOpIdle if between operations. Note that there is no implication of whether 
	the server is currently connected; EOpIdle will be returned regardless of 
	connection state, if no operation is currently being performed. Use 
	CObex::IsConnected () to find connection staus.

@publishedAll
@released
**/
	{
	return (iCurrentOperation);
	}

