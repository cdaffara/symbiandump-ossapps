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
// obex_client.cpp [stub]
// oscarg, September 2003.
// This is a modified version of Obex.cpp. 
// Modified file for BT/IR OBEX stub
// 
//

#include <charconv.h>
//#include "debug.h"
#include "trans.h"
#include "obex.h"
#include "obexutil.h"
//#include "authentication.h"


//
// class CObex
//

CObex::CObex()
	{
	SetConnectState(EConnIdle);
	iConnector = NULL;
	iTransport = NULL;
	iCallBack = NULL;
	iChallPassword = NULL;
	iRespPassword = NULL;
	iRxChallenge = NULL;
	iCallBack = NULL;
	iChallenge = EFalse;
	iRemoteRealm = NULL;
	iRemoteUID = NULL;
	}

void CObex::ConstructL(TObexProtocolInfo& /*aObexProtocolInfoPtr*/)
	{
	}



CObex::~CObex()
	{
	DeleteTransport();
	DeleteConnector();
	iSocketServ.Close();
	//delete iAuthEngine;
	delete iChallPassword;
	delete iRespPassword;
	delete iRemoteUID;
	delete iRemoteRealm;
	delete iRxChallenge;
	
	}

EXPORT_C void CObex::RemoteAddr(TSockAddr& /*aAddr*/)
	{
	}

EXPORT_C TInt CObex::SetLocalWho(const TDesC8& aInfo)
/**
	The local who field is used to identify the local end of the OBEX session 
	when the OBEX connection is made. Set this to a unique value before 
	establishing the connection, if recognition strict peers (i.e. a peer also 
	using this who value) is required. 
**/
	{ 
	if(aInfo.Length())
		{ iLocalInfo.iWho.Copy(aInfo); return KErrNone;} 
	else
		return KErrArgument;
	}

void CObex::PrepareChallResponseL(const TDesC& /*aPassword*/)
	{
	}



TInt CObex::GenerateChallenge(TObexPacket& /*aPacket*/)
	{
	return KErrNone;
	}

void CObex::ProcessChallResponseL(const TObexInternalHeader& /*hdr*/)
	{
	}

void CObex::ProcessChallengeL(const TObexInternalHeader& /*hdr*/)
	{
	}

EXPORT_C void CObex::SetCallBack(MObexAuthChallengeHandler& aCallBack)
	{
	iCallBack = &aCallBack;
	}

void CObex::Process(TObexPacket& /*aPacket*/)
	{
	}

void CObex::Error(TInt aError) 
	{
	OnError(aError);
	}

void CObex::TransportUp(TObexConnectionInfo& /*aInfo*/)
	{
	}

void CObex::TransportDown(TBool /*aForceTransportDeletion*/)
	{
	}

void CObex::BringTransportDown()
	{
	}

void CObex::NewConnectorL(TObexProtocolInfo& /*aObexProtocolInfoPtr*/)
	{
	}

void CObex::DeleteConnector()
	{
	delete iConnector;
	iConnector = NULL;
	}

void CObex::NewTransportL(TObexConnectionInfo& /*aInfo*/)
	{
	}

void CObex::DeleteTransport()
	{
	delete iTransport;
	iTransport = NULL;
	}

void CObex::SetConnectState(TConnectState aNewState)
	{
	iConnectState = aNewState;
	}

// CObex::TSetPathInfo
EXPORT_C CObex::TSetPathInfo::TSetPathInfo() 
/**
@publishedAll
@released
*/
	{
	iFlags = 0;
	iConstants = 0;
	iNamePresent = EFalse;
	}

EXPORT_C TBool CObex::TSetPathInfo::Parent() const 
/**
@publishedAll
@released
*/
	{
	return(iFlags & KObexSetPathParent);
	}

// CObex
EXPORT_C TBool CObex::IsConnected() const 
/**
	Returns @c ETrue if this CObex is connected at an OBEX level, merely having
	a transport connected does not satisfy this condition. I.e. the two devices
	must have completed the OBEX connection request/response . All other states
	return @c EFalse. This will be unreliable if either the server blindly 
	returns the client’s who header (always reporting ETrue), or if neither
	supply "who" headers (always reporting EFalse).

@publishedAll
@released
**/
	{
	return(ConnectState() == EConnObex);
	}

EXPORT_C TBool CObex::IsStrictPeer() const 
/**	
	Returns true if the "who" header specified in the server’s connect response 
	matched that of the client’s connect request, and both had a length greater 
	than 0 (i.e. both specified a "who" field). Undefined if @c IsConnected() 
	== @c EFalse. 

@publishedAll
@released
**/
	{
	return((iLocalInfo.iWho.Length()>0) && (iLocalInfo.iWho==iRemoteInfo.iWho));
	}

EXPORT_C const TObexConnectInfo& CObex::LocalInfo() const 
/**
	Use this member to gain access to (and alter, if necessary) the 
	CObex::TConnectInfo structure which will be sent to the OBEX peer as part 
	of the connection process. Only alter the contents of this having read and 
	understood the purpose of the fields, as defined in the OBEX spec. Altering 
	this structure after a connection has been made will have no effect on the
	current session, but will be used for future connection attempts.

@publishedAll
@released
**/
	{
	return(iLocalInfo);
	}


EXPORT_C const TObexConnectInfo& CObex::RemoteInfo() const 
/**
	Use this member to read the details of the remote machine’s connection 
	information, as specified by it in during OBEX connection. This data can 
	not be altered, as this serves no purpose.
	The content of this structure is undefined when @c IsConnected () == @c 
	EFalse. 

@publishedAll
@released
**/
	{
	return(iRemoteInfo);
	}

EXPORT_C CObex::TConnectState CObex::ConnectState() const 
	{
	return (iConnectState);
	};


/**
@ fn virtual void MObexServerNotify::ErrorIndication (TInt aError) =0;
This function will be called when some form of OBEX protocol error occurs, the 
passed parameter aError is a standard EPOC error code which describes the 
nature of this error. It is only be called for fatal errors, in that the OBEX 
connection will be brought down and then the error indication passed. Any 
error which does not bring the connection down will not be indicated, for 
example the server issuing a semantically valid, but unrecognised command.
	
@publishedAll
@released
**/

/** @fn virtual void MObexServerNotify::TransportUpIndication () =0;
Called when the underlying socket transport connection is made from a remote 
client to the server. Note that this does not, however, indicate an OBEX 
connection has been successfully established. Define any low-level connection 
policy here. Also a good place to set up the LocalConnectInfo, if any 
non-defaults are required.
	
@publishedAll
@released
**/

/** @fn virtual void MObexServerNotify::TransportDownIndication () =0;
Override this to provide processing when the transport connection is dropped 
(by either party), whether the OBEX connection was gracefully disconnected or 
not. This is the definitive place for disconnection processing; it will always 
be called on disconnection.
	
@publishedAll
@released
**/

/** @fn virtual TInt MObexServerNotify::ObexConnectIndication  (const CObex::TConnectInfo& aRemoteInfo, const TDesC8& aInfo) =0;
Override this function to provide extra processing when an OBEX connection is 
made from a remote client.
@param aRemoteInfo	holds the connection information supplied by that
					remote machine.
@param aInfo		holds further information about the requested
					connection (reserved).
@return				EPOC error indicating the success of the connection.
	
@publishedAll
@released
**/

/** fn virtual void MObexServerNotify::ObexDisconnectIndication (const TDesC8& aInfo) =0;
Override this to provide extra processing on a (graceful) OBEX disconnection 
by the client.
@param aInfo	Contains information about the disconnection (reserved).
@note This indication will not be raised if the remote machine simply 
drops the transport connection. However, ErrorIndication () will be called if 
the disconnection is unexpected/ invalid (i.e. another operation was in 
progress at the time). In all cases, TransportDownIndication () will be 
called.
	
@publishedAll
@released
**/

/** @fn virtual CObexBaseObject* MObexServerNotify::PutRequestIndication () =0;
Called on receipt of the first packet of a (valid) put request. It is called 
before any parsing of the packet is performed, simply to establish whether 
this server is interested in receiving objects at all.
@return a pointer to a CObexBaseObject derived object, which the object 
being put will be parsed into.
@return NULL	ERespForbidden will be returned to the client.
@note If the returned object cannot be initialised for receiving (e.g. a 
tempory file could not be created for CObexFileObjects) 
ERespInternalError will be returned to the client.
	
@publishedAll
@released
**/

/** @fn virtual TInt MObexServerNotify::PutPacketIndication () =0;
Called on receipt of every packet of an OBEX ‘put’ operation. It will always 
be preceded by a PutRequestIndicatoin(). The object returned by the request 
indication will updated to reflect all the information received concerning the 
object which the client is sending, from packets up to and including the 
current one. 
@return	KErrNone	instructs the server to allow the client to continue the 
					put operation.
@return -			any other value the operation is cancelled with an 
					appropriate OBEX server response code.
Use this function to provide periodic user notification on the progress of the 
transfer (noting that the Length attribute may take an arbitrary number of 
packets to become non-zero, and the constraints on its accuracy). Due to the 
nature of OBEX operations, where any header attribute can be sent at any point 
in the transfer, this is also a good place to marshal the details of a 
received object, and possibly prompt for action on the received object (e.g. 
renaming on filename clashes).
@note It is worth noting that this function could be hit quite heavily, in 
proportion to the size of the object transferred. Therefore more consideration 
should be given to the speed of execution of this function than that of the 
other indications.
	
@publishedAll
@released
**/

/** @fn virtual TInt MObexServerNotify::PutCompleteIndication () =0;
Called after the final put packet has been successfully received and parsed.
@return	KErrNone	instructs the remote client that the put completed successfully.
@return -			any other value the operation is cancelled with an 
					appropriate OBEX server response code.
@note Note that there will not necessarily be a call to this function 
corresponding to each PutRequestIndication; if an error occurs while the put 
is being carried out ErrorIndication will be called instead.
@since 6.1 Before 6.1, the return type was void.
	
@publishedAll
@released
**/

/** @fn virtual CObexBaseObject* MObexServerNotify::GetRequestIndication (CObexBaseObject* aRequestedObject) =0;
Called when a full get request has been received from the client. 
aRequestedObject holds all the details about the object the remote client has 
requested. Use this function to analyse the clients request, and return a 
pointer to the object to be returned to the client, or NULL to return no 
object. If NULL is returned, the server will send ERespForbidden to the 
client, if an error occurs in returning the object, ERespInternalError is 
returned, otherwise the returned object is sent back to the client.
	
@publishedAll
@released
**/

/** @fn virtual TInt MObexServerNotify::GetPacketIndication () =0;
Called for every packet of get reply sent by the server back to the client. 
That is, it is only called while an object is being sent to the client, not 
while the client is providing its initial specification for the object it 
requires. Return KErrNone to continue sending the object, or some other error 
code to cancel the operation.
Use this function to provide user notification on the object being sent, and 
its progress. As with PutPacketIndication, it is worth bearing in mind that 
this packet might get called quite heavily, especially for large objects and 
small OBEX packet sizes, so the speed of execution here is important.
	
@publishedAll
@released
**/

/** @fn virtual TInt MObexServerNotify::GetCompleteIndication () =0;
Called when the final packet of the object has been returned to the client. 
@return	KErrNone	instructs the remote client that the get completed successfully.
@return -			any other value the operation is cancelled with an 
					appropriate OBEX server response code.
@note Like PutCompleteIndication, this function might not be 
called for each GetRequestIndication, if the operation is interrupted by an 
error.
@since 6.1 Before 6.1, the return type was void.
	
@publishedAll
@released
**/

/** @fn virtual TInt MObexServerNotify::SetPathIndication (const CObex::TSetPathInfo& aPathInfo, const TDesC8& aInfo) =0;
Indicates a SETPATH command has been received by the server.
@param aPathInfo	Will contain the flags, constants, and 
					name (if present) for this Setpath
@param aInfo		Not currently used: will be == KNullDesC

@return				An EPOC error code indicating the success of the setpath.

	
@publishedAll
@released
**/

/** @fn virtual void MObexServerNotify::AbortIndication () =0;
Called when an abort packet is received from the client. An OBEX abort command 
simply cancels the current operation, and does not necessarily have to cause 
the connection to be dropped. On return, a ERespSuccess packet is sent to the 
client. 
	
@publishedAll
@released
**/

/** @fn virtual void MObexAuthChallengeHandler::GetUserPasswordL(const TDesC& aRealm) =0;
Indicates that the server/client has received an Authentication challenge. The challenge
can only be responded to once a password has been retrieved by some means and passed
back to the calling class. The password is passed back via 
CObex::UserPasswordL( const TDesC& aPassword)
@param aRealm	this will contain the Realm specified by the unit forcing
				the Authentication, if no UserID was supplied then this
				parameter will be of zero length. 
	
@publishedAll
@released
**/


//
//
// DLL entry point
//
// The reason code is of no interest here and it is ignored
//

GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return KErrNone;
	}
