// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __CIMAPSESSIONMANAGER_H__
#define __CIMAPSESSIONMANAGER_H__

#include <e32std.h>
#include <es_sock.h>
#include <msvstd.h>
#include <mentact.h>
#include "msocketconnectobserver.h"
#include "moutputstreamobserver.h"
#include "moutputstreamsecureobserver.h"
#include "minputstreamobserver.h"
#include "cimaplistfolderinfo.h"
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	#include "cimapauthhelpers.h"					
#endif


// Forward Declarations
class CImapSession;
class CImapSettings;
class CImapMailStore;
class CTransportManager;
class MInputStream;
class MOutputStream;
class TImap4GenericProgress;
class CImapListFolderInfo;
class CImConnect;
class MSocketConnector;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	class CImapCapabilityInfo;
	class CImapAuthMechanismHelper;
#endif

/**
Provides a mechanism for creating connected, logged in CImapSession objects,
and for disconnecting them.
The objects are created on behalf of the requesting class, which owns the
created CImapSession objects immediately upon creation.
@internalTechnology
@prototype
*/
class CImapSessionManager : public CMsgActive,
		public MSocketConnectObserver,
		public MOutputStreamObserver,
		public MOutputStreamSecureObserver,
		public MInputStreamObserver
	{
public:
	IMPORT_C static CImapSessionManager* NewL(CImapSettings& aImapSettings, CImapMailStore& aImapMailStore);
	~CImapSessionManager();

	// Get a connected, logged in session. The passed session pointer
	// should not be used unless the routine completes with a status of
	// KErrNone.
	IMPORT_C void GetSessionL(TRequestStatus& aStatus, CImapSession*& aSession);
	// Logout and disconnect a list of sessions
	IMPORT_C void Disconnect(TRequestStatus& aStatus, const RPointerArray<CImapSession>& sessionList);
	// Immediately disconnect a list of sessions
	IMPORT_C void Disconnect(const RPointerArray<CImapSession>& sessionList);
	// Immediately disconnect a single session
	IMPORT_C void Disconnect(const CImapSession& aSession);
	// Get LastSocketActivityTimeout value from the connection
	IMPORT_C TUint32 LastSocketActivityTimeout();
	// Get progress information for current operation
	IMPORT_C void Progress(TImap4GenericProgress& aProgress);
	// Identifies if an RConnection is available
	IMPORT_C TBool HasConnection();
	// Get the RConnection used by all the sessions
	IMPORT_C RConnection& GetConnectionL();
	// Gets the access point ID in use for the connection to the server
	IMPORT_C TInt GetAccessPointIdForConnection(TUint32& aAccessPointId) const;
	// Close the current network connection
 	IMPORT_C void CloseNetworkConnection();

private:
	// Construction routines
	CImapSessionManager(CImapSettings& aImapSettings, CImapMailStore& aImapMailStore);
	void ConstructL();

	// from MSocketConnectObserver
	virtual void ConnectionMadeL(MInputStream& aInputStream, MOutputStream& aOutputStream);
	virtual TInt HandleConnectError(TInt aError);

	// from MOutputStreamObserver
	virtual void SendDataCnf();
	virtual void OutputStreamCloseInd(TInt aError);

	// from MOutputStreamSecureObserver
	virtual void SecureClientCnf(TInt aError);

	// from MInputStreamObserver
	virtual void ReceivedDataIndL(const TDesC8& aBuffer);
	virtual void SecureServerCnf();
	virtual void InputStreamCloseInd(TInt aError);

	// from CMsgActive
	virtual void DoRunL();
	virtual void DoCancel();
	virtual void DoComplete(TInt& aStatus);

	// State handlers for state machine
	void ProcessNetworkConnectionCreatedL();
	void ProcessSocketConnectedL();
	void ProcessUpgradedSSLWrappedSocketL();
	void ProcessGreetingResponseL();
	void ProcessCapabilityResponseL();
	void ProcessStartTLSResponse();
	void ProcessUpgradedTLSSocketL();
	void ProcessSecureCapabilityResponseL();
	void ProcessLoginResponseL();
	void ProcessSeparatorResponseL();
	void ProcessLogoutResponse();
	void ProcessSocketDisconnected();
	void CreateSecureSocket();
	void StartSessionL();
	void CreateNetworkConnectionL();
	void CreateSocketL();
	void SendLoginL();
	void GetHierarchySeparatorL();
	void DisconnectFirstSessionInList();
	void DisconnectSocket();
	void ImmediateDisconnect();
	void CloseSessionStreams(CImapSession* aSession);
	TInt GetConnectionStage();
	void ConvertError(TInt& aError);
	void CompleteSelf(TInt aError);
	TInt LogId();
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	void ProcessAuthenticateResponseL();
	void SelectAuthExtensionProfileL(const CImapCapabilityInfo& aCapabilityInfo);
	void SendAuthenticateL(CImapAuthMechanismHelper::TImapAuthProfileFlag aCurrentAuthProfile);
#endif
	
private:

/**
IMAP Session Manager States
@internalComponent
@prototype
*/
enum TImapSessionManagerState
	{
	EStateIdle,
	EStateCreatingNetworkConnection,
	EStateConnectingSocket,
	EStateGettingLastSocketActivityTimeout,
	EStateUpgradingSSLWrappedSocket,
	EStateGreetingWait,
	EStateCapabilityWait,
	EStateStartTLSWait,
	EStateUpgradingTLSSocket,
	EStateSecureCapabilityWait,
	EStateLoginWait,
	EStateSeparatorWait,
	EStateLogoutWait,
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	EStateAuthenticateWait,
#endif
	EStateDisconnectingSocket

	};

	// Session Manager State
	TImapSessionManagerState iState;

	// Progress state
	TImap4GenericProgress::TImap4GenericProgressState iProgressState;

	// Service settings, passed on construction:
	CImapSettings& iImapSettings;
	
	// Mail Store, passed on construction:
	CImapMailStore& iImapMailStore;
	
	// created on construction:
	CTransportManager* iTransportManager;

	// The Imap session being created
	CImapSession* iImapSession;

	// Pointer to the session store of the session create caller.
	// This is updated to point to the newly created session, after the
	// session has been fully created.
	CImapSession** iStoreSession;

	// Host address
	HBufC8* iHostAddr8;

	// Session input and output streams
	MInputStream* iInputStream;
	MOutputStream* iOutputStream;

	// List of sessions to disconnect. Sessions to be disconnected
	// are not owned by session manager so are not deleted.
	RPointerArray<CImapSession> iDisconnectList;

	// Socket server
	RSocketServ iSocketServ;
	// Handles RConnection used by sockets
	CImConnect* iImConnect;
	// Flag to indicate if network connection is created yet
	TBool iCreatedNetworkConnection;

	// Stores result of calling list command to get separator value
	RArrayImapListFolderInfo iSeparatorFolderList;

	// LastSocketActivityTimeout value read from CImConnect	
	TUint32 iLastSocketActivityTimeout;

	// Socket connector used to create connection.
	// Not owned by this class	
	MSocketConnector* iSocketConnector;
	
	// Flag to indicate that we have just completed ourself.
	// We use callbacks from the transport handler to receive error
	// notifications and then self complete with those error codes. As it is
	// possible to receive more than one callback at a time, we use this flag
	// to ensure we don't try to self complete more than once.
	TBool iCompletedSelf;
	
	// Flag to indicate if there was an error while  upgrading the socket to SSL/TLS.
	TBool iErrorUpgradingSSLSocket;
	
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	CImapAuthMechanismHelper::TImapAuthProfileFlag iCurrentAuthProfile;
#endif
	};

#endif // __CIMAPSESSIONMANAGER_H__
