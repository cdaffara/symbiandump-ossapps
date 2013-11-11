// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __IMUTCON_H__
#define __IMUTCON_H__

#include <mentact.h>
#include <iapprefs.h>
#include <es_sock.h>
#include <in_sock.h>
#include <commdbconnpref.h>

class CStoreableOverrideSettings;
class CImTextServerSession;

// simulate failures if this is set
#ifdef _DEBUG
#define __IMSK_SIMULATION
#endif

class CImConnect : public CActive
/**
@internalTechnology
@released
*/
	{
public:
	// text session is used to log
	static CImConnect* NewL(const CImIAPPreferences& aIAPPreferences, CImTextServerSession &aTextSession);
	IMPORT_C static CImConnect* NewL(const CImIAPPreferences& aIAPPreferences, RSocketServ& aSocketServerHandle);

	~CImConnect();

	IMPORT_C void StartL(TRequestStatus& aStatus);

/**
	Intended Usage	:	Returns the IAP value
	@since			8.0
	@return			Returns the IAP value
	*/
	IMPORT_C TInt GetIAPValue(TUint32& aIap);

/**
	Intended Usage	:	On return, the unique name of the RConnection.
	@since			9.1
	@return			KErrNone if succesful, or another of the system-wide error codes. 
	*/	
	IMPORT_C TInt GetRConnectionName(TName &aName);

/**
	@fn				GetIAPBearer(TUint32& aBearer)
	Intended Usage	:	Returns the IAP bearer
	@since			8.0
	@param			aBearer is a return argument containing the bearer if it exists
	@return			Returns KErrNone, KErrNotFound or KErrBadHandle
	*/
	IMPORT_C TInt GetIAPBearer(TUint32& aBearer);

/**
	@fn				GetConnection()
	Intended Usage	:	Returns RConnection
	@since			8.0
	@return			Returns RConnection
	*/
	IMPORT_C RConnection& GetConnection();

/**
	Intended Usage	:	Returns progress of the connection
	@since			8.0
	@param			On return, progress information
	@return			Returns KErrNone or KErrBadHandle
	*/
	IMPORT_C TInt Progress(TNifProgress& aProgress);

/**
	@fn				GetLastSocketActivityTimeout(TUint32& aTimeout)
	Intended Usage	:	Returns the last socket activity timeout value
	@since			9.1
	@param			aTimeout is a return argument containing the timeout if it was found
	@post				aTimeout will be filled with the timeout value
	@return			Returns KErrNone, KErrNotFound or KErrBadHandle
	*/
	IMPORT_C TInt GetLastSocketActivityTimeout(TUint32& aTimeout);

#ifdef __IMSK_SIMULATION
	// the code will ignore the first aIAPsToFail iaps in the preferences passed in.
	// if this is more than the number of iaps then it will fail with KErrEtelModemNotDetected
	void SetIAPsToFail(TInt aIAPsToFail);
#endif

/**
	@fn				SecondaryStartL(CImTextServerSession* aPrimaryTextServerSession)
	Intended Usage	:	Attaches the existing RConnection.
	@since			9.2
	@param			aPrimaryTextServerSession is set on secondary session	
	*/
	void SecondaryStartL(CImTextServerSession* aPrimaryTextServerSession);

protected:
	// From CActive;
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	CImConnect(const CImIAPPreferences& aIAPPreferences, CImTextServerSession* aTextSession, RSocketServ* aSocketServerHandle);
	// checks to see if we are online, if we are not online call ConnectL
	void CheckAndConnectL();
	void ConnectL();
	void ConnectWithSNAP();
	void ConnectWithIAPListL();

private:
	const CImIAPPreferences& iIAPPreferences;
	RConnection	iConn;
	TCommDbMultiConnPref iPrefs;
	TInt iAttempt;
	TRequestStatus* iReportStatus;
	
	// Text server session. Session is not owned by this class.
	// Pointer can be NULL which indicates that our class was
	// instantiated with a socket server handle instead of a
	// reference to a text server session.
	CImTextServerSession* iTextSession;

	// Socket server handle. Handle is not owned by this object.
	// Pointer can be NULL which indicates that handle must be
	// fetched from iTextSession text server session class.
	RSocketServ* iSocketServerHandle;

	CStoreableOverrideSettings* iOverrides;

#ifdef __IMSK_SIMULATION
	// the code will ignore the first iIAPsToFail iaps in the preferences passed in.
	// if this is more than the number of iaps then it will fail with KErrEtelModemNotDetected
	TInt iIAPsToFail;
#endif
	};

#endif

