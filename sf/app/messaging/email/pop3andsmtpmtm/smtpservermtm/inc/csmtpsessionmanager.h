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

#ifndef __CSMTPSESSIONMANAGER_H__
#define __CSMTPSESSIONMANAGER_H__

#include <mentact.h>
#include <msvstd.h>
#include <es_sock.h>

class CMsvServerEntry;
class CSmtpSettings;
class CImSmtpSession;
class TRequestStatus;
class TImSmtpProgress;
class CImConnect;
class CImMobilityManager;

/**
Provides a mechanism for creating logged in SMTP sessions and for
disconnecting them.

@internalTechnology
@released
*/
NONSHARABLE_CLASS(CSmtpSessionManager) : public CMsgActive
	{
public:
	static CSmtpSessionManager* NewL(CImMobilityManager* aMobilityManager, TMsvId aServiceId);
	~CSmtpSessionManager();

	void GetSessionL(CMsvServerEntry& aServerEntry, CSmtpSettings& aSettings,
                   CImSmtpSession*& aSession, TRequestStatus& aClientStatus);
	void DeleteSession(CImSmtpSession& aSession, TRequestStatus& aClientStatus);
	void ConnectionProgress(TImSmtpProgress& aProgress);
	TBool IsSessionConnected();
	TBool IsConnectionStarted();
	TInt GetAccessPointIdForConnection(TUint32& aAccessPointId) const;

private:
	enum TState
		{
		EStateIdle,
		EStateCreatingConnection,
		EStateCreatingSession,
		EStateStoringSession,
		EStateQuittingSession
		};

private:
	// Construction routines
	CSmtpSessionManager(CImMobilityManager* aMobilityManager, TMsvId aServiceId);
	void ConstructL();

	virtual void DoRunL();
	virtual void DoCancel();
	virtual void DoComplete(TInt& aStatus);

	void CreateConnectionL();
	void RegisterConnectionL();
	void CreateSessionL();
	void StoreSessionL();
	void QuitSessionL();

private:
	// Mobility manager. Not owned by this class.
	// Will be null if bearer mobility not supported
	CImMobilityManager* iMobilityManager;

	// SMTP service ID
	TMsvId iServiceId;

	// SMTP service entry. Not owned by this class
	CMsvServerEntry* iServerEntry;

	// SMTP settings. Not owned by this class
	CSmtpSettings* iSettings;

	// Session we are operating on
	CImSmtpSession* iSession;

	// Stores the created session for passing back to caller
	CImSmtpSession** iStoreSession;

	// Network connection
	CImConnect* iConnect;

	// Socket server
	RSocketServ iServ;

	// State machine state
	TState iState;
	};

#endif // __CSMTPSESSIONMANAGER_H__
