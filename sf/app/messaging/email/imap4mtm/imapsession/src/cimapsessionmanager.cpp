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
//

#include "cimapsettings.h"
#include <miut_err.h>		// imap server error codes
#include <utf.h>
#include <imutcon.h>
#include "cimapsessionmanager.h"
#include "cimapsession.h"
#include "ctransportmanager.h"
#include "msocketfactory.h"
#include "cimapcapabilityinfo.h"
#include "cimaplistfolderinfo.h"
#include "cimapservergreetinginfo.h"
#include "moutputstream.h"
#include "minputstream.h"
#include "cimaplogger.h"
#include "msocketconnector.h"
#include "cimapsessionconsts.h"

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	#include "cimapauthhelpers.h"
#endif
/**
Static factory constructor. Part of two phased construction.

@param aImapSettings Settings for the Imap service
@param aImapMailStore Imap mail store
@return The constructed CImapSessionManager object
*/
EXPORT_C CImapSessionManager* CImapSessionManager::NewL(CImapSettings& aImapSettings, CImapMailStore& aImapMailStore)
	{
	CImapSessionManager* self = new (ELeave) CImapSessionManager(aImapSettings, aImapMailStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Class constructor

@param aImapSettings Settings for IMAP service
@param aImapMailStore Imap mail store
*/
CImapSessionManager::CImapSessionManager(CImapSettings& aImapSettings, CImapMailStore& aImapMailStore)
	: CMsgActive(EPriorityStandard)
	, iProgressState(TImap4GenericProgress::EIdle)
	, iImapSettings(aImapSettings)
	, iImapMailStore(aImapMailStore)
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	, iCurrentAuthProfile(CImapAuthMechanismHelper::EUndefined)
#endif
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::CImapSessionManager");
	}

/**
Class destructor
*/
CImapSessionManager::~CImapSessionManager()
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::~CImapSessionManager");

	Cancel();

	delete iTransportManager;
	delete iImConnect;
	iSocketServ.Close();
	delete iHostAddr8;
	iDisconnectList.Reset();
	iSeparatorFolderList.ResetAndDestroy();
	delete iImapSession;
	}

/**
Non-trival constructor. Part of two phased construction.
*/
void CImapSessionManager::ConstructL()
	{
	// We're an active object...
	CActiveScheduler::Add(this);
	}

/**
Requests construction of a connected, logged in CImapSession object

@param aStatus Signals completion of the request.
@param aSession Place to store the session once it is fully created.
*/
EXPORT_C void CImapSessionManager::GetSessionL(TRequestStatus& aStatus, CImapSession*& aSession)
	{
	__ASSERT_DEBUG(iState == EStateIdle,
	               TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedStateForGetSession));

	__LOG_TEXT(KDefaultLog, "CImapSessionManager::GetSessionL");

	iStoreSession = &aSession;

	if (!iCreatedNetworkConnection)
		{
		CreateNetworkConnectionL();
		}
	else
		{
		CreateSocketL();
		}

	Queue(aStatus);
	}

/**
Requests logout and disconnect of a list of sessions.

@param aStatus Signals completion of the request.
@param aSessionList List of sessions to disconnect
*/
EXPORT_C void CImapSessionManager::Disconnect(TRequestStatus& aStatus, const RPointerArray<CImapSession>& aSessionList)
	{
	__ASSERT_DEBUG(iState == EStateIdle,
	               TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedStateForDisConnect));

	__ASSERT_DEBUG(iDisconnectList.Count() == 0,
	               TImapServerPanic::ImapPanic(TImapServerPanic::EDisconnectWhenDisconnectListNotEmpty1));

	// Take a copy of the list of sessions to disconnect
	iDisconnectList.Reset();
	for (TInt session(0); session < aSessionList.Count(); ++session)
		{
		__LOG_TEXT(aSessionList[session]->LogId(), "CImapSessionManager::Disconnect (async, list)");

		TInt err = iDisconnectList.Append(aSessionList[session]);
		if(err != KErrNone)
			{
			break;
			}
		}

	iProgressState = TImap4GenericProgress::EDisconnecting;
	DisconnectFirstSessionInList();

	Queue(aStatus);
	}

/**
Requests immediate disconnect of a list of sessions. The logout command is not
sent.

@param aSessionList List of sessions to disconnect
*/
EXPORT_C void CImapSessionManager::Disconnect(const RPointerArray<CImapSession>& aSessionList)
	{
	__ASSERT_DEBUG(iDisconnectList.Count() == 0,
	               TImapServerPanic::ImapPanic(TImapServerPanic::EDisconnectWhenDisconnectListNotEmpty2));

	// Take a copy of the list of sessions to disconnect
	iDisconnectList.Reset();
	for (TInt session(0); session < aSessionList.Count(); ++session)
		{
		__LOG_TEXT(aSessionList[session]->LogId(), "CImapSessionManager::Disconnect (sync, list)");

		TInt err = iDisconnectList.Append(aSessionList[session]);
		if(KErrNone != err)
            {
		    CloseSessionStreams(aSessionList[session]);
            }
		}

	ImmediateDisconnect();
	}

/**
Requests immediate disconnect of a session. The logout command is not sent.

@param aSession Session to disconnect
*/
EXPORT_C void CImapSessionManager::Disconnect(const CImapSession& aSession)
	{
	__ASSERT_DEBUG(iDisconnectList.Count() == 0,
	               TImapServerPanic::ImapPanic(TImapServerPanic::EDisconnectWhenDisconnectListNotEmpty3));

	__LOG_TEXT(aSession.LogId(), "CImapSessionManager::Disconnect() - START - (sync, session)");

	// Create a disconnect list with just this one session
	iDisconnectList.Reset();
	TInt err = iDisconnectList.Append(&aSession);
	
	if(KErrNone != err)
        {
	    // remove contness of aSession
	    CImapSession& session = const_cast<CImapSession&>(aSession);
	    MOutputStream* outputStream = session.OutputStream();    
        if (outputStream != NULL)
            {
            outputStream->Close();
            }
        }
	
	ImmediateDisconnect();
    __LOG_TEXT(aSession.LogId(), "CImapSessionManager::Disconnect() - END - (sync, session)");
	}

/**
Gets LastSocketActivityTimeout value for the connection

@return LastSocketActivityTimeout value
*/
EXPORT_C TUint32 CImapSessionManager::LastSocketActivityTimeout()
	{
	return iLastSocketActivityTimeout;
	}

/**
Gets progress for an asynchronous operation

@param aProgress Holds progress information
*/
EXPORT_C void CImapSessionManager::Progress(TImap4GenericProgress& aProgress)
	{
	aProgress.iOperation = TImap4GenericProgress::EConnect;
	switch(iState)
		{
	case EStateCreatingNetworkConnection:
	case EStateConnectingSocket:
	case EStateGettingLastSocketActivityTimeout:
	case EStateUpgradingSSLWrappedSocket:
	case EStateGreetingWait:
	case EStateCapabilityWait:
	case EStateStartTLSWait:
	case EStateUpgradingTLSSocket:
	case EStateSecureCapabilityWait:
	case EStateLoginWait:
	case EStateSeparatorWait:
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	case EStateAuthenticateWait:
#endif
		{
		// We are connecting a session
		aProgress.iState = TImap4GenericProgress::EConnecting;
		aProgress.iImap4SubStateProgress = TImap4GenericProgress::EIdle;

		// iMsgsDone must contain the connection stage information
		aProgress.iMsgsDone = GetConnectionStage();
		TUint32 iap = 0;
		TInt err(KErrNotFound);
		// Get the IAP value to store in iMsgsToDo
		if (iImConnect != NULL)
			{
			err = iImConnect->GetIAPValue(iap);
			}
		if (err == KErrNone)
			{
			aProgress.iMsgsToDo = iap;
			}
		else
			{
			aProgress.iMsgsToDo = err;
			}

		break;
		}

	case EStateIdle:
	default:
		{
		aProgress.iState = iProgressState;
		aProgress.iImap4SubStateProgress = TImap4GenericProgress::EIdle;
		break;
		}
		}				
	}

/**
From MSocketConnectObserver - called when a connection to the remote server
has been successfully made.

@param aInputStream Created socket's input stream API
@param aOutputStream Created socket's output stream API
*/
void CImapSessionManager::ConnectionMadeL(MInputStream& aInputStream, MOutputStream& aOutputStream)
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::ConnectionMadeL");

	iInputStream = &aInputStream;
	iOutputStream = &aOutputStream;

	// Temporarily bind to the streams for close notifications.
	// Once the session is created, it will take over the binding.
	iInputStream->Bind(*this, KDefaultLog);
	iOutputStream->Bind(*this, KDefaultLog);
	
	// Store the current bearer type in the imap settings class
	TUint32 bearerType;
	iImConnect->GetIAPBearer(bearerType);
	iImapSettings.SetCurrentBearerType(bearerType);

	CompleteSelf(KErrNone);
	}

/**
From MSocketConnectObserver - Handle the Connection Error

Notification that an error has occured in the connecting service. The error 
code will have a value of KErrCancel if the observer has stopped the connect 
service via the MSocketConnectObserver::StopConnect() API. If a problem
does occur the socket connector will suicide after handling the error.

@param aError The error code.
@return A value of KErrNone if the error has been handled or any other 
        value if its has not been handled.
*/
TInt CImapSessionManager::HandleConnectError(TInt aError)
	{
	__LOG_FORMAT((KDefaultLog, "CImapSessionManager::HandleConnectError - %d", aError));

	// We should only get this while trying to connect the socket.
	// At any other time we can ignore it.
	// We can also get this after stopping a socket connect due to a
	// cancel. However we will have already self completed and we don't
	// want to complete again. If we have completed, then the iSocketConnector
	// will have been set to NULL, so check this.
	if ((iState == EStateConnectingSocket) && (iSocketConnector != NULL))
		{
		CompleteSelf(aError);
		}

	return KErrNone;
	}

/**
From MOutputStreamObserver - Indicates data has been sent successfully.
*/
void CImapSessionManager::SendDataCnf()
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::SendDataCnf");

	// No action required
	}

/**
From MOutputStreamObserver - Indicates that output stream has closed.

@param aError Error code that caused stream close.
*/
void CImapSessionManager::OutputStreamCloseInd(TInt aError)
	{
	__LOG_FORMAT((KDefaultLog, "CImapSessionManager::OutputStreamCloseInd - Error %d, State %d", aError, iState));

	// We can only get this when bound to the stream before the stream
	// binding is taken over by the session.
	__ASSERT_DEBUG(iState == EStateUpgradingSSLWrappedSocket,
	               TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedOutputStreamCloseInd));

	if (iState == EStateUpgradingSSLWrappedSocket)
		{
		iOutputStream = NULL;

		// The only reason the socket should have closed is if a genuine error
		// has occured. However if for some reason we don't get an error
		// returned we need to complete with KErrDisconnected because the
		// socket has closed and we need to tell the caller that we are not
		// going to be able to finish creating the requested session.
		if (aError >= KErrNone)
			{
			aError = KErrDisconnected;
			}

		CompleteSelf(aError);
		}
	}

/**
From MOutputStreamSecureObserver - Indicates that a secure socket request
has completed on the output stream.

@param aError Error code from completed request.
*/
void CImapSessionManager::SecureClientCnf(TInt aError)
	{
	__LOG_FORMAT((LogId(), "CImapSessionManager::SecureClientCnf - Error %d, State %d", aError, iState));

	// Should only get this when we are upgrading to a secure socket
	__ASSERT_DEBUG((iState == EStateUpgradingSSLWrappedSocket ||
	                iState == EStateUpgradingTLSSocket),
	                TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedSecureClientCnf));

	if ((iState == EStateUpgradingSSLWrappedSocket) ||
	    (iState == EStateUpgradingTLSSocket))
		{
		CompleteSelf(aError);
		}
	}

/**
From MInputStreamObserver - Indicates that the stream has received some data.
*/
void CImapSessionManager::ReceivedDataIndL(const TDesC8& /*aBuffer*/)
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::ReceivedDataIndL");
	// No action required
	}

/**
From MInputStreamObserver - Indicates that a secure socket request
has completed on the input stream.
*/
void CImapSessionManager::SecureServerCnf()
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::SecureServerCnf");
 
	// No action required
	}

/**
From MInputStreamObserver - Indicates that the input stream has been closed.

@param aError Error code that caused stream close.
*/
void CImapSessionManager::InputStreamCloseInd(TInt aError)
	{
	__LOG_FORMAT((KDefaultLog, "CImapSessionManager::InputStreamCloseInd - Error %d, State %d", aError, iState));

	// We can only get this when bound to the stream before the stream
	// binding is taken over by the session.
	__ASSERT_DEBUG(iState == EStateUpgradingSSLWrappedSocket,
	               TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedInputStreamCloseInd));

	if (iState == EStateUpgradingSSLWrappedSocket)
		{
		iInputStream = NULL;

		// The only reason the socket should have closed is if a genuine error
		// has occured. However if for some reason we don't get an error
		// returned we need to complete with KErrDisconnected because the
		// socket has closed and we need to tell the caller that we are not
		// going to be able to finish creating the requested session.
		if (aError >= KErrNone)
			{
			aError = KErrDisconnected;
			}
		
		// Complete the request if there is no error while upgrading socket to SSL/TLS.
		if(!iErrorUpgradingSSLSocket)
		   {
		   CompleteSelf(aError);	
		   }
		
		}
	}

/**
Called when asynchronous IMAP Session events occur.
Manages the various steps necessary to log in to the IMAP Service, or to
logout from it.
*/
void CImapSessionManager::DoRunL()
	{
	iCompletedSelf = EFalse;

	// If we get a positive error code such as KErrImapNO, leave now
	// which will mean that DoComplete is called with that error code.
	// This is derived from CMsgActive, so we should not get any negative
	// error codes. They will go straight to DoComplete.
	if (iStatus.Int() > KErrNone)
		{
		User::Leave(iStatus.Int());
		}

	switch(iState)
		{
		case EStateCreatingNetworkConnection:
			{
			ProcessNetworkConnectionCreatedL();
			break;
			}

		case EStateConnectingSocket:
			{
			ProcessSocketConnectedL();
			break;
			}

		case EStateUpgradingSSLWrappedSocket:
			{
			ProcessUpgradedSSLWrappedSocketL();
			break;
			}

		case EStateGreetingWait:
			{
			ProcessGreetingResponseL();
			break;
			}

		case EStateCapabilityWait:
			{
			ProcessCapabilityResponseL();
			break;
			}
	
		case EStateStartTLSWait:
			{
			ProcessStartTLSResponse();
			break;
			}

		case EStateUpgradingTLSSocket:
			{
			ProcessUpgradedTLSSocketL();
			break;
			}

		case EStateSecureCapabilityWait:
			{
			ProcessSecureCapabilityResponseL();
			break;
			}

		case EStateLoginWait:
			{
			ProcessLoginResponseL();
			break;
			}
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		case EStateAuthenticateWait:
			{
			ProcessAuthenticateResponseL();
			break;
			}	
#endif
		case EStateSeparatorWait:
			{
			ProcessSeparatorResponseL();
			break;
			}

		case EStateLogoutWait:
			{
			ProcessLogoutResponse();
			break;
			}

		case EStateDisconnectingSocket:
			{
			ProcessSocketDisconnected();
			break;
			}

		case EStateGettingLastSocketActivityTimeout:
		default:
			{
			__ASSERT_DEBUG(ETrue, TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedStateForDoRunL));
			break;
			}
		}
	}

/**
Cancels the current outstanding asynchronous operation.

The operation will only be cancelled here. Any cleanup is deferred to the
DoComplete method which will be called next as this is a CMsgActive.
*/
void CImapSessionManager::DoCancel()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::DoCancel");

	switch(iState)
		{
		case EStateCreatingNetworkConnection:
			{
			iImConnect->Cancel();
			break;
			}

		case EStateConnectingSocket:
			{
			// We will call StopConnect() on the socket connector.
			// This is supposed to call back to HandleConnectError with
			// KErrCancel once it completes. However it appears that this
			// does not always happen, so it is safer to self complete here
			// with KErrCancel.
			// Just in case the HandleConnectError() routine does get called
			// we will set the iSocketConnector to NULL to indicate that the
			// request has been cancelled
			MSocketConnector* socketConnector(iSocketConnector);
			iSocketConnector = NULL;
			socketConnector->StopConnect();
			CompleteSelf(KErrCancel);
			break;
			}

		case EStateGreetingWait:
		case EStateCapabilityWait:
		case EStateStartTLSWait:
		case EStateSecureCapabilityWait:
		case EStateLoginWait:
		case EStateSeparatorWait:
		case EStateLogoutWait:
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		case EStateAuthenticateWait:
#endif
			{
			iImapSession->Cancel();
			break;
			}

		case EStateUpgradingSSLWrappedSocket:
		case EStateUpgradingTLSSocket:
		case EStateDisconnectingSocket:
			{
			// There is no specific cancel for any of these states.
			// Just self complete with KErrCancel.
			CompleteSelf(KErrCancel);
			break;
			}

		case EStateGettingLastSocketActivityTimeout:
		default:
			{
			__ASSERT_DEBUG(ETrue, TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedStateForDoCancel));
			CompleteSelf(KErrCancel);
			break;
			}
		}

	// Call base class version to notify parent class.
	CMsgActive::DoCancel();
	}

/**
The current outstanding asynchronous operation has completed.
Perform any necessary cleanup.

@param aStatus Status of completed operation.
*/
void CImapSessionManager::DoComplete(TInt& aStatus)
	{
	__LOG_FORMAT((LogId(), "CImapSessionManager::DoComplete - Error %d, State %d", aStatus, iState));

	iCompletedSelf = EFalse;
	
	// If we have completed with an error code, we may want to change it to a
	// more specific one based on the state we are in.
	if (aStatus != KErrNone)
		{
		ConvertError(aStatus);
		}

	// If we have completed with an error, handle any required cleanup
	if (aStatus != KErrNone)
		{
		switch (iState)
			{
			case EStateCreatingNetworkConnection:
				{
				delete iImConnect;
				iImConnect = NULL;
				break;
				}

			case EStateConnectingSocket:
				{
				// Nothing required.
				break;
				}

			case EStateGettingLastSocketActivityTimeout:
			case EStateUpgradingSSLWrappedSocket:
				{
				// Need to close the streams as we have not yet passed them
				// on to the session.
				// Closing the output stream will close the input stream too.
				if (iOutputStream != NULL)
					{
					// Since there was a error while upgrading socket to SSL/TLS.
					iErrorUpgradingSSLSocket = ETrue;
					iOutputStream->Close();
					}
				break;
				}

			case EStateUpgradingTLSSocket:
			case EStateGreetingWait:
			case EStateCapabilityWait:
			case EStateStartTLSWait:
			case EStateSecureCapabilityWait:
			case EStateLoginWait:
			case EStateSeparatorWait:
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
			case EStateAuthenticateWait:
#endif
				{
				// At this stage, the session has been created so we need
				// to close the streams and then delete the session.
				CloseSessionStreams(iImapSession);
				delete iImapSession;
				iImapSession = NULL;
				break;
				}

			case EStateLogoutWait:
			case EStateDisconnectingSocket:
				{
				// Immediately disconnect all the sessions remaining in the disconnect
				// list. This is because the protocol controller has no way of knowing
				// which ones are disconnected and which ones are not, so it is safer
				// just to disconnect them all.
				ImmediateDisconnect();
				break;
				}

			default:
				{
				__ASSERT_DEBUG(ETrue, TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedStateForDoCompleteWithError));
				break;
				}
			}
		iProgressState = TImap4GenericProgress::EDisconnected;
		}
	else
		{
		iProgressState = TImap4GenericProgress::EIdle;
		}

	// If we have completed successfully, and we are performing a connect,
	// store the completed session information in the place requested by the
	// caller.
	// Note that iImapSession will only be non NULL for a successful connection
	// attempt.
	if (iImapSession != NULL)
		{
		*iStoreSession = iImapSession;
		iImapSession = NULL;
		}

	delete iHostAddr8;
	iHostAddr8 = NULL;

	iInputStream = NULL;
	iOutputStream = NULL;
	iSocketConnector = NULL;

	iState = EStateIdle;
	}

/**
State handler called when network connection has been created.
*/
void CImapSessionManager::ProcessNetworkConnectionCreatedL()
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::ProcessNetworkConnectionCreatedL");

	iCreatedNetworkConnection = ETrue;
	CreateSocketL();
	}

/**
State handler called when socket has been connected.
*/
void CImapSessionManager::ProcessSocketConnectedL()
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::ProcessSocketConnectedL");

	// Change state just in case the call to GetLastSocketActivityTimeout
	// returns an error and we leave. This allows the subsequent DoComplete
	// to clean up the connection
	iState = EStateGettingLastSocketActivityTimeout;
	User::LeaveIfError(iImConnect->GetLastSocketActivityTimeout(iLastSocketActivityTimeout));

	if (iImapSettings.SSLWrapper())
		{
		iState = EStateUpgradingSSLWrappedSocket;
		iErrorUpgradingSSLSocket = EFalse;
		CreateSecureSocket();
		}
	else
		{
		StartSessionL();
		}
	}

/**
State handler called when a socket has been upgraded to SSL.
*/
void CImapSessionManager::ProcessUpgradedSSLWrappedSocketL()
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::ProcessUpgradedSSLWrappedSocketL");

	StartSessionL();
	}

/**
State handler called when a greeting response has been received.
*/
void CImapSessionManager::ProcessGreetingResponseL()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::ProcessGreetingResponseL");

	const CImapServerGreetingInfo& greetingInfo = iImapSession->ServerGreetingInfo();

	switch (greetingInfo.ResponseTag())
		{
		case CImapServerGreetingInfo::ETagOk:
			{
			__LOG_TEXT(LogId(), "CImapSessionManager::Greeting response OK");

			// Request capability information from server
			iImapSession->CapabilityL(iStatus);
			iState=EStateCapabilityWait;
			SetActive();
			break;
			}

		case CImapServerGreetingInfo::ETagPreAuth:
			{
			__LOG_TEXT(LogId(), "CImapSessionManager::Greeting response PreAuth");

			// Server has pre authenticated us. Just request hierarchy
			// separator and complete.
			GetHierarchySeparatorL();
			break;
			}

		case CImapServerGreetingInfo::ETagBye:
			{
			__LOG_TEXT(LogId(), "CImapSessionManager::Greeting response Bye");

			// Server is currently busy. Complete with an error
			Complete(KErrImapServerBusy);
			break;
			}
 
		default:
			{
			__ASSERT_DEBUG(ETrue, TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedServerGreetingResponse));
			Complete(KErrCorrupt);
			break;
			}
		}
	}

/**
State handler called when a capability response has been received.
*/
void CImapSessionManager::ProcessCapabilityResponseL()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::ProcessCapabilityResponseL");

	const CImapCapabilityInfo& capabilityInfo = iImapSession->CapabilityInfo();

	// Check that a valid IMAP revision number has been received
	if (!capabilityInfo.QueryFlag(CImapCapabilityInfo::EVersion4Rev1))
		{
		__LOG_TEXT(LogId(), "CImapSessionManager - Capability not IMAP4Rev1");

		Complete(KErrImapServerVersion);
		return;
		}

	// Check if we must use a secure socket connection
	if (iImapSettings.SecureSockets())
		{
		__LOG_TEXT(LogId(), "CImapSessionManager - Use secure socket");

		// We need a secure socket connection, so check that the server
		// supports TLS
		if (capabilityInfo.QueryFlag(CImapCapabilityInfo::EStartTls))
			{
			iImapSession->StartTlsL(iStatus);
			iState=EStateStartTLSWait;
			SetActive();
			}
		else
			{
			__LOG_TEXT(LogId(), "CImapSessionManager - Server does not support TLS");

			// Server does not support TLS, so complete with an error
			Complete(KErrImapServerNoSecurity);
			}
		}
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	else if (iImapSettings.IMAP4Auth())
		{
		SelectAuthExtensionProfileL(capabilityInfo);
		}
#endif	
	else 
		{
		if (capabilityInfo.QueryFlag(CImapCapabilityInfo::ELoginDisabled))
			{
			__LOG_TEXT(LogId(), "CImapSessionManager - Login disabled");

			// The server does not support insecure logins, so complete with an error
			Complete(KErrImapServerLoginDisabled);
			}	
		else
			{
			// Just do plain login
			SendLoginL();	
			} 
		}
	}

/**
State handler called when the Start TLS response has been received.
*/
void CImapSessionManager::ProcessStartTLSResponse()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::ProcessCapabilityResponseL");

	iState = EStateUpgradingTLSSocket;
	CreateSecureSocket();
	}

/**
State handler called when a socket has been upgraded to TLS.
*/
void CImapSessionManager::ProcessUpgradedTLSSocketL()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::ProcessUpgradedTLSSocketL");

	iImapSession->CapabilityL(iStatus);
	iState=EStateSecureCapabilityWait;
	SetActive();
	}

/**
State handler called when a capability response has been received after
secure connection setup.
*/
void CImapSessionManager::ProcessSecureCapabilityResponseL()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::ProcessSecureCapabilityResponseL");

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	const CImapCapabilityInfo& capabilityInfo = iImapSession->CapabilityInfo();
	if (iImapSettings.IMAP4Auth())
		{
		SelectAuthExtensionProfileL(capabilityInfo);
		}
	else
#endif 
		{
		SendLoginL();	
		}
	}

/**
State handler called when a login response has been received.
*/
void CImapSessionManager::ProcessLoginResponseL()
	{
	__LOG_ON(LogId(), ETrue);
	__LOG_TEXT(LogId(), "CImapSessionManager::ProcessLoginResponseL");

	GetHierarchySeparatorL();
	}

/**
State handler called when a separator response has been received.
*/
void CImapSessionManager::ProcessSeparatorResponseL()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::ProcessSeparatorResponseL");

	if ((iSeparatorFolderList.Count() > 0) &&
	    (iSeparatorFolderList[0]->iHierarchySeperator != '\0'))
		{
		iImapSettings.SetPathSeparatorL(iSeparatorFolderList[0]->iHierarchySeperator);
		}

	iSeparatorFolderList.ResetAndDestroy();

	Complete(KErrNone);
	}

/**
State handler called when a logout response has been received.
*/
void CImapSessionManager::ProcessLogoutResponse()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::ProcessLogoutResponse");

	DisconnectSocket();
	}

/**
State handler called when a socket has been disconnected.
*/
void CImapSessionManager::ProcessSocketDisconnected()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::ProcessSocketDisconnected");

	iDisconnectList.Remove(0);
	DisconnectFirstSessionInList();
	}

/**
Start creation of a network connection.
*/
void CImapSessionManager::CreateNetworkConnectionL()
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::CreateNetworkConnectionL");
	
	if (iSocketServ.Handle() == 0)
		{
		User::LeaveIfError(iSocketServ.Connect());
		}

	if (iImConnect == NULL)
		{
		iImConnect = CImConnect::NewL(iImapSettings.IAPPreferences(), iSocketServ);
		}

	iImConnect->StartL(iStatus);
	iState = EStateCreatingNetworkConnection;
	SetActive();
	}

/**
Gets the server address and password from the CImapSettings class and
establishes a socket to the server.
*/
void CImapSessionManager::CreateSocketL()
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::CreateSocket");

	if (iTransportManager == NULL)
		{
		iTransportManager = CTransportManager::NewL(iSocketServ.Handle(), iImConnect->GetConnection());
		}

	TPtrC hostAddress = iImapSettings.ServerAddress();
	TInt hostPort     = iImapSettings.Port();

	iHostAddr8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(hostAddress);	
	TPtr8 addr8ptr = iHostAddr8->Des();

	iSocketConnector = &(iTransportManager->SocketFactory().ConnectL(*this, addr8ptr, hostPort));

	iState = EStateConnectingSocket;
	iStatus = KRequestPending;
	SetActive();
	}

/**
Start creation of a secure socket.
*/
void CImapSessionManager::CreateSecureSocket()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::CreateSecureSocket");

	iOutputStream->BindSecure(*this);

	TPtrC8 domainName;
	if (iImapSettings.TlsSslDomain().Length() > 0)
		{
		domainName.Set(iImapSettings.TlsSslDomain());
		}
	else
		{
		domainName.Set(*iHostAddr8);
		}

	iOutputStream->SecureClientReq(domainName);

	iStatus = KRequestPending;
	SetActive();
	}

/**
Create a new session, and wait for the greeting response from the IMAP server.
*/
void CImapSessionManager::StartSessionL()
	{
	__LOG_TEXT(KDefaultLog, "CImapSessionManager::StartSessionL");

	// Create the IMAP Session. 
	iImapSession = CImapSession::NewL(iImapSettings, iImapMailStore, *iInputStream, *iOutputStream);

	// Connected, request consumption of Server Greeting
	iState = EStateGreetingWait;
	iImapSession->ReadServerGreetingL(iStatus);
	SetActive();
	}

/**
Send a login command to the IMAP server.
*/
void CImapSessionManager::SendLoginL()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::SendLoginL");
	
	// Turn off logging so that user name and password are hidden
	__LOG_ON(LogId(), EFalse);

	iImapSession->LoginL(iStatus, iImapSettings.LoginName(), iImapSettings.Password());
	iState = EStateLoginWait;
	SetActive();
	}

/**
Request the hierarchy separator from the IMAP server by sending a list command
with no folders specified.
*/
void CImapSessionManager::GetHierarchySeparatorL()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::GetHierarchySeparatorL");

	// Always update the path separator on connection.
	iSeparatorFolderList.ResetAndDestroy();
	iImapSession->ListL(iStatus, KNullDesC(), KNullDesC(), iSeparatorFolderList);
	iState = EStateSeparatorWait;
	SetActive();
	}

/**
We are disconnecting a list of sessions. If there are any left in the list,
send a logout command for the first one.
*/
void CImapSessionManager::DisconnectFirstSessionInList()
	{
	if (iDisconnectList.Count() == 0)
		{
		iDisconnectList.Reset();
		Complete(KErrNone);
		return;
		}

	__LOG_TEXT(LogId(), "CImapSessionManager::Logout session");

	iState = EStateLogoutWait;
	TRAPD(err, iDisconnectList[0]->LogoutL(iStatus));

	if (err == KErrNone)
		{
		SetActive();
		}
	else
		{
		DisconnectSocket();
		}
	}

/**
Disconnect a socket.
*/
void CImapSessionManager::DisconnectSocket()
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::DisconnectSocket");

	// Just close the streams as this will delete the socket to.
	CloseSessionStreams(iDisconnectList[0]);

	iState = EStateDisconnectingSocket;

	// Complete ourselves to keep the state machine moving
	iStatus = KRequestPending;
	CompleteSelf(KErrNone);
	SetActive();
	}

/**
Immediately disconnect all the sessions in the disconnect list.
No logout command is sent. We just disconnect the socket.
*/
void CImapSessionManager::ImmediateDisconnect()
	{
	while (iDisconnectList.Count() > 0)
		{
		__LOG_TEXT(LogId(), "CImapSessionManager::ImmediateDisconnect");

		// Just close the streams as this will delete the socket to.
		CloseSessionStreams(iDisconnectList[0]);
		iDisconnectList.Remove(0);
		}

	iDisconnectList.Reset();
	}

/**
Close the session streams. This will also delete the streams and their
associated socket.
*/
void CImapSessionManager::CloseSessionStreams(CImapSession* aSession)
	{
	__LOG_TEXT(aSession->LogId(), "CImapSessionManager::CloseSessionStreams");

	// Closing the output stream will close the input stream too.
	MOutputStream* outputStream = aSession->OutputStream();

	if (outputStream != NULL)
		{
		outputStream->Close();
		}
	}

/**
Gets the connection stage information from the connection.

@return Connection stage or error code.
*/
TInt CImapSessionManager::GetConnectionStage()
	{
	if (iImConnect)
		{
		TNifProgress progress;
		TInt err = iImConnect->Progress(progress);
		return (err == KErrNone) ? (progress.iStage) : (err);
		
		}

	return KErrNotFound;
	}

/**
Converts an error code to one that is more meaningful for the state we are in.

@param aError Error code to convert.
*/
void CImapSessionManager::ConvertError(TInt& aError)
	{
	if (aError <= KErrNone)
		{
		return;
		}

	switch (iState)
		{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)			

		case EStateCapabilityWait:
			{
			__LOG_TEXT(LogId(), "CImapSessionManager - Convert error to bad  authenticate");
			aError = KErrImapAuthenticationFailed;
			break;
			}
#endif
		case EStateLoginWait:
			{
			__LOG_TEXT(LogId(), "CImapSessionManager - Convert error to bad logon");
			aError = KErrImapBadLogon;
			break;
			}
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)			
		case EStateAuthenticateWait:
			{
			__LOG_TEXT(LogId(), "CImapSessionManager - Convert error to bad logon");
			aError = KErrImapBadLogon;
			break;
			}
#endif
		case EStateStartTLSWait:
			{
			__LOG_TEXT(LogId(), "CImapSessionManager - Convert error to TLS negotiate failed");
			aError = KErrImapTLSNegotiateFailed;
			break;
			}

		case EStateLogoutWait:
		case EStateDisconnectingSocket:
			{
			// Do nothing
			break;
			}

		default:
			{
			__LOG_TEXT(LogId(), "CImapSessionManager - Convert error to Imap connect error");
			// This is a postive error code (defined in cimapsession, thus allowing further
			// handling in the protocol controller. Specifically, this is used in the case
			// of initial connection failure where brearer mobility is configured.
			aError = KErrImapConnectError;
			break;
			}
		}
	}

/**
Self complete with specified error code.

@param aError Error code to complete with.
*/
void CImapSessionManager::CompleteSelf(TInt aError)
	{
	// We use callbacks from the transport handler to receive error
	// notifications and then self complete with those error codes. As it is
	// possible to receive more than one callback at a time, we have to make
	// sure we don't try to self complete more than once.
	if (!iCompletedSelf)
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, aError);
		iCompletedSelf = ETrue;
		}
	}

/**
Gets the log ID to use for debug logging.

@return Log ID
*/
TInt CImapSessionManager::LogId()
	{
#ifdef __IMAP_LOGGING
	if (iImapSession)
		{
		return iImapSession->LogId();
		}
	else if (iDisconnectList.Count() > 0)
		{
		return iDisconnectList[0]->LogId();
		}
#endif //__IMAP_LOGGING
	return KDefaultLog;
	}

/**
Returns ETrue if the session manager has access to an RConnection

@return TBool
*/
EXPORT_C TBool CImapSessionManager::HasConnection()
	{
	if (iImConnect)
		{
		return ETrue;
		}

	return EFalse;
	}

/**
Gets the RConnection used for all the sessions

@return RConnection reference
*/
EXPORT_C RConnection& CImapSessionManager::GetConnectionL()
	{
	__ASSERT_DEBUG(iImConnect,
	               TImapServerPanic::ImapPanic(TImapServerPanic::ERConnectionNotDefined));

	if (!iImConnect)
		{
		User::Leave(KErrNotFound);		
		}

	return iImConnect->GetConnection();
	}

/**
Gets the access point ID in use for the connection to the server

@param aAccessPointId On return stores the access point ID value

@return KErrNone if successful, or a system wide error code
*/
EXPORT_C TInt CImapSessionManager::GetAccessPointIdForConnection(TUint32& aAccessPointId) const
	{
	if (iImConnect)
		{
		return iImConnect->GetIAPValue(aAccessPointId);
		}

	return KErrNotFound;
	}
	
/**
Close the current network connection since all the sessions will be closed before this
*/
EXPORT_C void CImapSessionManager::CloseNetworkConnection()
 	{
 	iCreatedNetworkConnection = EFalse;	
	delete iTransportManager;
	iTransportManager = NULL;
 	delete iImConnect;
	iImConnect = NULL ;
 	iSocketServ.Close();
 	}

/**
Send a Authenticate  command to the IMAP server.
@param aCurrentAuthProfile
*/
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)

void CImapSessionManager::SendAuthenticateL(CImapAuthMechanismHelper::TImapAuthProfileFlag aCurrentAuthProfile) 
	{
	__LOG_TEXT(LogId(), "CImapSessionManager::SendAuthenticateL");
	
	iImapSession->AuthenticateL(iStatus,aCurrentAuthProfile);

	iState =  EStateAuthenticateWait;
	SetActive();
	}

/**
State handler called when a authenticate response has been received.
*/
void CImapSessionManager::ProcessAuthenticateResponseL()
	{
	__LOG_ON(LogId(), ETrue);
	__LOG_TEXT(LogId(), "CImapSessionManager::ProcessLoginResponseL");

	GetHierarchySeparatorL();
	}

/**
Selects  Authentication extension profile depending on authentication and fallback flags
@param aCapabilityInfo
*/	
void CImapSessionManager::SelectAuthExtensionProfileL(const CImapCapabilityInfo& aCapabilityInfo)
	{
	if (aCapabilityInfo.QueryFlag(CImapCapabilityInfo::EAuthCramMd5))	
		{
		__LOG_TEXT(LogId(), "CImapSessionManager - ECramMD5");
		iCurrentAuthProfile = CImapAuthMechanismHelper::ECramMD5;
		SendAuthenticateL(iCurrentAuthProfile);
		}
	else if(iImapSettings.FallBack())
    	{
      	if (aCapabilityInfo.QueryFlag(CImapCapabilityInfo::EAuthPlain))
      		{
      		__LOG_TEXT(LogId(), "CImapSessionManager - EAuthPlain");
      		iCurrentAuthProfile=CImapAuthMechanismHelper::EPlain;
			SendAuthenticateL(iCurrentAuthProfile);
      		}
		else if (aCapabilityInfo.QueryFlag(CImapCapabilityInfo::EAuthLogin))
			{
			__LOG_TEXT(LogId(), "CImapSessionManager - ELogin");
			iCurrentAuthProfile = CImapAuthMechanismHelper::ELogin;
			SendAuthenticateL(iCurrentAuthProfile);
			}
    	else
			{
			__LOG_TEXT(LogId(), "CImapSessionManager - ELogin");
			SendLoginL();	
			}
		}
	else 
		{
		TInt status = 1;
		Complete(status);
		}
	}
#endif
