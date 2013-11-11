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
//


#ifndef __AGSMAIN_H__
#define __AGSMAIN_H__

#include <f32file.h>
#include <asclisession.h>
#include "agsbackuprestoreagent.h"

class CAgnTlsProxy;
class RFs;
class CAgnServerSession;
class CAgnServer;

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
class CCAgnSystemStateObserver;
#endif

// Set server priority
const TInt KServerPriority = 0;  // EStandard Priority

const TUid KUidAgnGeneralPrefs=			{0x10000235};
const TUid KUidAgnModel=				{0x100000f1};
const TUid KUidAgnPrintSetup=			{0x1000010D};
const TUid KUidAgnApp=					{0x10003A5C};

enum TAgnServerMode
	{
	ETransientServer,
	ENonTransientServer
	};

/**
Agenda Server - Server Close Timer:
An active timer class adding delay before shutting down the agenda server to prevent a cold reboot
*/
class CAgnServUndertaker : public CActive
	{
public:
	static CAgnServUndertaker* NewL();
	void ConstructL();
	void DestroyServer();
	void CancelUndertaker();
private:
	inline CAgnServUndertaker(TInt aPriority = EPriorityStandard) : CActive(aPriority){};
	void DoCancel();
	void RunL();
private:
	TBool	iCancelled;	// set this to cancel the undertaker after it has been started
public:
	CAgnServer* iServer;
	};

// CAgnAlarmServerChangeListener
//

class CAgnAlarmServerChangeListener : public CActive
    {
public:
    static CAgnAlarmServerChangeListener* NewL(RASCliSession& iAlarmServer, CAgnServFileMgr& iFileMgr);
    ~CAgnAlarmServerChangeListener();
    
private:
    CAgnAlarmServerChangeListener(RASCliSession& iAlarmServer, CAgnServFileMgr& iFileMgr);
    void ConstructL();
   
private:
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    
private:
    RASCliSession& iAlarmServer;
    CAgnServFileMgr& iFileMgr;
    
    // not used
    TAlarmId iAlarmId;
    };

// CAgnServer

class CAgnServer : public CPolicyServer
	{
public:
	static CAgnServer* NewL();
	void ConstructL();
	~CAgnServer();

	static CAgnServer* InitL(CAgnServUndertaker* aUndertaker);

	virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	inline CAgnServFileMgr* FileMgr();
	void SessionClosed();
	void SessionAdded();
	RFs* FsSession();
	void FetchSessionsL(RPointerArray<CAgnServerSession>& aSessions);

	TAgnServerMode ServerMode();
	CAgnTlsProxy* TimeZoneConverter();
	void ShutdownServer();
	const CAgnServBackupRestoreAgent& BackupRestoreAgent() const;
	RASCliSession& AlarmServer();
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	const CCAgnSystemStateObserver& SystemStateObserver() const;
#endif
protected:
	CAgnServer(TInt aPriority);
private:
	CAgnServFileMgr* iFileMgr;
	CAgnServBackupRestoreAgent* iBackupRestoreAgent;
	RFs iFs;
	TInt iSessionCount;
	CAgnServUndertaker* iUndertaker;	// used to clear itself up
	CAgnTlsProxy* iProxy;
	TAgnServerMode iServerMode;
    RASCliSession  iAlarmServer;
    CAgnAlarmServerChangeListener* iAlarmServerChangeListener;

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	CCAgnSystemStateObserver* iSystemStateObserver;
#endif
	};

// inline functions

CAgnServFileMgr* CAgnServer::FileMgr() {return iFileMgr;}

#endif
