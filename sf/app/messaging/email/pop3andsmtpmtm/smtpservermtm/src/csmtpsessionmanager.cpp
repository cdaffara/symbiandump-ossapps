// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "csmtpsessionmanager.h"
#include "csmtpsettings.h"
#include "imutcon.h"
#include "IMSMSEND.H"
#include "IMSM.H"
#include "SMTSUTIL.H"
#include "cimmobilitymanager.h"

/**
Factory constructor

@param aMobilityManager Mobility manager (null if mobility not supported)
@return Constructed session manager
*/
CSmtpSessionManager* CSmtpSessionManager::NewL(CImMobilityManager* aMobilityManager, TMsvId aServiceId)
	{
	CSmtpSessionManager* self = new (ELeave) CSmtpSessionManager(aMobilityManager, aServiceId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor

@param aMobilityManager Mobility manager (null if mobility not supported)
*/
CSmtpSessionManager::CSmtpSessionManager(CImMobilityManager* aMobilityManager, TMsvId aServiceId) :
 CMsgActive(KImSmtpSessionPriority),
 iMobilityManager(aMobilityManager),
 iServiceId(aServiceId)
	{
	}

/**
Second phase constructor
*/
void CSmtpSessionManager::ConstructL()
	{
	User::LeaveIfError(iServ.Connect());
	CActiveScheduler::Add(this);
	}

/**
Destructor
*/
CSmtpSessionManager::~CSmtpSessionManager()
	{
	Cancel();
	delete iConnect;
	iServ.Close();
	}

/**
Gets a new logged in SMTP session.
The caller passes a reference to a session pointer which they should initially
set to NULL. If the client status is completed with KErrNone, then their pointer
will have been updated to point to the new session. If the client status completes
with an error then their pointer will not be updated.

@param aServerEntry SMTP service entry
@param aSettings SMTP settings
@param aSession Used to store session to pass back to caller
@param aClientStatus Signals completion of the request
*/
void CSmtpSessionManager::GetSessionL(CMsvServerEntry& aServerEntry,
                                      CSmtpSettings& aSettings,
                                      CImSmtpSession*& aSession,
                                      TRequestStatus& aClientStatus)
	{
	iServerEntry = &aServerEntry;
	iSettings = &aSettings;
	iStoreSession = &aSession;

	CreateConnectionL();

	Queue(aClientStatus);
	}

/**
Deletes a session.

@param aSession The session to delete. This routine takes immediate ownsership
                of the session.
@param aClientStatus Signals completion of the request
*/
void CSmtpSessionManager::DeleteSession(CImSmtpSession& aSession,
                                        TRequestStatus& aClientStatus)
	{
	iSession = &aSession;

	Queue(aClientStatus);

	TRAPD(err, QuitSessionL());

	// If we failed to quit from the server we should just delete the
	// session anyway as that will drop the connection.
	if (err != KErrNone)
		{
		delete iSession;
		iSession = NULL;
		TRequestStatus* status = &aClientStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

/**
Gets the progress of a connection

@param aProgress Stores the progress information
*/
void CSmtpSessionManager::ConnectionProgress(TImSmtpProgress& aProgress)
	{
	if (iSession)
		{
		aProgress.iSendFileProgress = iSession->FileProgress();
		aProgress.SetMsgNo(iSession->GetConnectionStage());
		aProgress.SetConnectionIAP(iSession->GetConnectionIAP());
		}
	else
		{
		TUint32 iap = 0;
		TInt iapErr = KErrNotFound;
	
		aProgress.iSendFileProgress.iSessionState = EConnectingToSmtp;
		aProgress.iSendFileProgress.iBytesSent = 0;
		aProgress.iSendFileProgress.iBytesToSend = 0;

		if (iConnect)
			{
			TNifProgress progress;
			TInt err = iConnect->Progress(progress);
			if (err == KErrNone)
				{
				aProgress.SetMsgNo(progress.iStage);
				}
			else
				{
				aProgress.SetMsgNo(err);
				}

			iapErr = iConnect->GetIAPValue(iap);
			}
		else
			{
			aProgress.SetMsgNo(KErrNotFound);
			}

		if (iapErr == KErrNone)
			{
			aProgress.SetConnectionIAP(iap);
			}
		else
			{
			aProgress.SetConnectionIAP(iapErr);
			}
		}
	}

/**
Returns whether the session is connected

@return Flag indicating if session is connected
*/
TBool CSmtpSessionManager::IsSessionConnected()
	{
	if (iSession)
		{
		return iSession->IsConnected();
		}

	return EFalse;
	}

/**
Returns whether the network connection has been started

@return Flag indicating if network connection has been started
*/
TBool CSmtpSessionManager::IsConnectionStarted()
	{
	if (iConnect)
		{
		return ETrue;
		}

	return EFalse;
	}


/**
Called when asynchronous event completes.
*/
void CSmtpSessionManager::DoRunL()
	{
	switch (iState)
		{
		case EStateCreatingConnection:
			{
			RegisterConnectionL();
			CreateSessionL();
			break;
			}
		
		case EStateCreatingSession:
			{
			StoreSessionL();
			break;
			}

		case EStateQuittingSession:
			{
			delete iSession;
			iSession = NULL;
			break;
			}

		case EStateStoringSession:
		default:
			{
			gPanic(EImsmSessionManagerInvalidState);
			break;
			}
		}
	}

/**
Cancel an outstanding asynchronous request.
*/
void CSmtpSessionManager::DoCancel()
	{
	if (iState == EStateCreatingConnection)
		{
		iConnect->Cancel();
		}
	else if (iSession)
		{
		iSession->Cancel();
		}

	CMsgActive::DoCancel();
	}

/**
The clients outstanding asynchronous request has been completed.

@param aStatus Status of completed operation.
*/
void CSmtpSessionManager::DoComplete(TInt& aStatus)
	{
	if (aStatus != KErrNone)
		{
		delete iSession;
		iSession = NULL;

		if (iState == EStateCreatingConnection)
			{
			delete iConnect;
			iConnect = NULL;
			}
		}
	else
		{
		iSession = NULL;
		}
	}

/**
Creates the network connection if it has not already been created.
*/
void CSmtpSessionManager::CreateConnectionL()
	{
	if (!iConnect)
		{
		iState = EStateCreatingConnection;

		iConnect = CImConnect::NewL(iSettings->IapPrefs(), iServ);
		iConnect->StartL(iStatus);
		SetActive();
		}
	else
		{
		CreateSessionL();
		}
	}

/**
Registers the network connection with the bearer mobility manager (if defined)
*/
void CSmtpSessionManager::RegisterConnectionL()
	{
	if (iMobilityManager)
		{
		iMobilityManager->SetConnection(iConnect->GetConnection());
		}
	}

/**
Starts the creation of a session.
*/
void CSmtpSessionManager::CreateSessionL()
	{
	iState = EStateCreatingSession;

	iSession = CImSmtpSession::NewL(*iServerEntry, *iSettings, iServ, *iConnect, iServiceId);
	iSession->ConnectL(iStatus);
	SetActive();
	}

/**
Stores the created session via the reference passed by the client
*/
void CSmtpSessionManager::StoreSessionL()
	{
	iState = EStateStoringSession;
	*iStoreSession = iSession;

	TUint32 iap;
	TInt iapErr = iConnect->GetIAPValue(iap);

	if (iapErr == KErrNone)
		{
		iSettings->SetIapL(iap);
		}
	else
		{
		iSettings->SetIapL(KDefaultSettingsIap);
		}
	}

/**
Quit from the server
*/
void CSmtpSessionManager::QuitSessionL()
	{
	iState = EStateQuittingSession;
	iSession->QuitL(iStatus);
	SetActive();
	}

/**
Gets the access point ID in use for the connection to the server

@param aAccessPointId On return stores the access point ID value

@return KErrNone if successful, or a system wide error code
*/
TInt CSmtpSessionManager::GetAccessPointIdForConnection(TUint32& aAccessPointId) const
	{
	if (iConnect)
		{
		return iConnect->GetIAPValue(aAccessPointId);
		}

	return KErrNotFound;
	}
