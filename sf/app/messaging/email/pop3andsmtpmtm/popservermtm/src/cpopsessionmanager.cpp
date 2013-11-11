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

#include "cpopsessionmanager.h"
#include <pop3set.h>
#include <iapprefs.h>
#include "imutcon.h"
#include <pops.h>
#include "POPS.PAN"

GLREF_C void Panic(TPopsPanic aPanic);

/**
Factory constructor

@return Constructed session manager
*/
CPopSessionManager* CPopSessionManager::NewL()
	{
	CPopSessionManager* self = new (ELeave) CPopSessionManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor
*/
CPopSessionManager::CPopSessionManager() :
 CMsgActive(KImPopSessionPriority)
	{
	}

/**
Second phase constructor
*/
void CPopSessionManager::ConstructL()
	{
	User::LeaveIfError(iServ.Connect());
	CActiveScheduler::Add(this);
	}

/**
Destructor
*/
CPopSessionManager::~CPopSessionManager()
	{
	Cancel();
	delete iConnect;
	iServ.Close();
	}

/**
Gets a new logged in POP session.
The caller passes a reference to a session pointer which they should initially
set to NULL. If the client status is completed with KErrNone, then their pointer
will have been updated to point to the new session. If the client status completes
with an error then their pointer will not be updated.

@param aSettings POP settings
@param aIAPPrefs IAP preferences
@param aSession Used to store session to pass back to caller
@param aClientStatus Signals completion of the request
*/
void CPopSessionManager::GetSessionL(CImPop3Settings& aSettings,
												 CImIAPPreferences& aIAPPrefs,
												 CImPop3Session*& aSession,
												 TRequestStatus& aClientStatus)
	{
	iSettings = &aSettings;
	iIAPPrefs = &aIAPPrefs;
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
void CPopSessionManager::DeleteSession(CImPop3Session& aSession,
                                       TRequestStatus& aClientStatus)
	{
	iSession = &aSession;
	QuitSession();
	Queue(aClientStatus);
	}

/**
Gets the progress of a connection

@param aProgress Stores the progress information
*/
void CPopSessionManager::ConnectionProgress(TPop3Progress& aProgress)
	{
	TUint32 iap = 0;
	TInt iapErr = KErrNotFound;

	if (iSession)
		{
		aProgress.iTotalMsgs = iSession->TextServerSession()->GetConnectionStage();

		iapErr = iSession->TextServerSession()->GetIAPValue(iap);
		}
	else if (iConnect)
		{
		TNifProgress progress;
		TInt err = iConnect->Progress(progress);
		if (err == KErrNone)
			{
			aProgress.iTotalMsgs = progress.iStage;
			}
		else
			{
			aProgress.iTotalMsgs = err;
			}

		iapErr = iConnect->GetIAPValue(iap);
		}
	else
		{
		aProgress.iTotalMsgs = KErrNotFound;
		}

	if (iapErr == KErrNone)
		{
		aProgress.iTotalBytes = iap;
		}
	else
		{
		aProgress.iTotalBytes = iapErr;
		}		
	}

/**
Called when asynchronous event completes.
*/
void CPopSessionManager::DoRunL()
	{
	switch (iState)
		{
		case EStateCreatingConnection:
			{
			CreateSessionL();
			break;
			}
		
		case EStateCreatingSession:
			{
			StoreSession();
			break;
			}

		case EStateQuittingSession:
			{
			// We have quit successfully, so we can delete the session now.
			delete iSession;
			iSession = NULL;
			break;
			}

		case EStateStoringSession:
		default:
			{
			Panic(EPopSessionManagerInvalidState);
			break;
			}
		}
	}

/**
Cancel an outstanding asynchronous request.
*/
void CPopSessionManager::DoCancel()
	{
	if (iState == EStateCreatingConnection)
		{
		iConnect->Cancel();
		}
	else if (iSession)
		{
		iSession->Cancel();
		}

	// Tell base class to finish up.
	CMsgActive::DoCancel();
	}

/**
The clients outstanding asynchronous request has been completed.

@param aStatus Status of completed operation.
*/
void CPopSessionManager::DoComplete(TInt& aStatus)
	{
	if (aStatus != KErrNone)
		{
		// If an error occured or the client cancelled the request then
		// perform all the cleanup.
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
void CPopSessionManager::CreateConnectionL()
	{
	iState = EStateCreatingConnection;

	if (iConnect)
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	else
		{
		iConnect = CImConnect::NewL(*iIAPPrefs, iServ);
		iConnect->StartL(iStatus);
		}
	SetActive();
	}

/**
Returns ETrue if the session manager has access to an RConnection

@return TBool
*/
TBool CPopSessionManager::HasConnection()
	{
	if (iConnect)
		{
		return ETrue;
		}
	return EFalse;
	}

/**
Requests an already connected RConnection from the Mobility Manager
and returns the RConnection, this method is called by the POP Server MTM.
*/
RConnection& CPopSessionManager::GetConnection()
	{
	// Get the Connection from CImConnect and pass on to the Server MTM
	return iConnect->GetConnection();
	}

/**
Starts the creation of a session.
*/
void CPopSessionManager::CreateSessionL()
	{
	iState = EStateCreatingSession;

	iSession = CImPop3Session::NewL(iServ, *iConnect);
	iSession->ConnectL(iSettings, *iIAPPrefs, iStatus);
	SetActive();
	}

/**
Stores the created session via the reference passed by the client
*/
void CPopSessionManager::StoreSession()
	{
	iState = EStateStoringSession;
	*iStoreSession = iSession;
	}

/**
Quit from the server
*/
void CPopSessionManager::QuitSession()
	{
	iState = EStateQuittingSession;
	iSession->Quit(iStatus);
	SetActive();
	}

/**
Gets the access point ID in use for the connection to the server

@param aAccessPointId On return stores the access point ID value

@return KErrNone if successful, or a system wide error code
*/
TInt CPopSessionManager::GetAccessPointIdForConnection(TUint32& aAccessPointId) const
	{
	if (iConnect)
		{
		return iConnect->GetIAPValue(aAccessPointId);
		}

	return KErrNotFound;
	}
