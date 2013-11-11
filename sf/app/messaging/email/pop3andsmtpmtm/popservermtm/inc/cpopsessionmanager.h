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

#ifndef __CPOPSESSIONMANAGER_H__
#define __CPOPSESSIONMANAGER_H__

#include <mentact.h>
#include <es_sock.h>

class CImPop3Settings;
class CImIAPPreferences;
class TRequestStatus;
class CImConnect;
class CImPop3Session;
class TPop3Progress;

/**
Provides a mechanism for creating logged in POP sessions and for
disconnecting them.
@internalTechnology
@released
*/
NONSHARABLE_CLASS(CPopSessionManager) : public CMsgActive
	{
public:
	static CPopSessionManager* NewL();
	~CPopSessionManager();

	void GetSessionL(CImPop3Settings& aSettings, CImIAPPreferences& aIAPPrefs,
	                 CImPop3Session*& aSession, TRequestStatus& aStatus);
	void DeleteSession(CImPop3Session& aSession, TRequestStatus& aStatus);
	void ConnectionProgress(TPop3Progress& aProgress);
	TBool HasConnection();
	RConnection& GetConnection();
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
	CPopSessionManager();
	void ConstructL();

	virtual void DoRunL();
	virtual void DoCancel();
	virtual void DoComplete(TInt& aStatus);

	void CreateConnectionL();
	void CreateSessionL();
	void StoreSession();
	void QuitSession();

private:

	// Pop settings. Not owned by this class
	CImPop3Settings* iSettings;

	// IAP preferences. Not owned by this class
	CImIAPPreferences* iIAPPrefs;

	// Session we are operating on
	CImPop3Session* iSession;

	// Stores the created session for passing back to caller
	CImPop3Session** iStoreSession;

	// Network connection
	CImConnect* iConnect;

	// Socket server
	RSocketServ iServ;

	// State machine state
	TState iState;
	};

#endif // __CPOPSESSIONMANAGER_H__
