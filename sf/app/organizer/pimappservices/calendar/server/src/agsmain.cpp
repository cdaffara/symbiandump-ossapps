// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "agsmain.h"

#include <bacline.h>
#include <e32property.h>

#include "agsalarm.h"
#include "agmtlsproxy.h"
#include "agmserv.h"
#include "agssess.h"
#include "agsentrymodel.h"
#include "agsfilemanager.h"
#include "agssignal.h"
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
#include "agssystemstateobserver.h"
#endif
#include <calnotification.h>

static _LIT_SECURITY_POLICY_PASS(KReadPolicy);
static _LIT_SECURITY_POLICY_S0(KWritePolicy, 0x10003a5b);

/**
 @internalComponent
 */
CAgnServUndertaker* CAgnServUndertaker::NewL()
	{
	CAgnServUndertaker* self = new(ELeave)CAgnServUndertaker();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}



// policy element constants
#define KPolicyElementReadUserData  		0
#define KPolicyElementWriteUserData  		1

const TUint KRangeCount = 4; 

const TInt KOpCodeRanges[KRangeCount] = 
	{	
	KCapabilityNone,
	KCapabilityReadUserData,
	KCapabilityWriteUserData,
	EAgnNotSupported,
	};

const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::EAlwaysPass,		
	KPolicyElementReadUserData,		
	KPolicyElementWriteUserData,	
	CPolicyServer::ENotSupported	
	};

const CPolicyServer::TPolicyElement KPolicyElements[] = 
	{
	{_INIT_SECURITY_POLICY_C1(ECapabilityReadUserData), CPolicyServer::EFailClient}, // KPolicyElementReadUserData
	{_INIT_SECURITY_POLICY_C1(ECapabilityWriteUserData), CPolicyServer::EFailClient}, // KPolicyElementWriteUserData
	};


const CPolicyServer::TPolicy KAgnSrvrPolicy =
	{
	CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
	KRangeCount,
	KOpCodeRanges,
	KElementsIndex, 	// what each range is compared to 
	KPolicyElements 	// what policies range is compared to
	};

void CAgnServUndertaker::ConstructL()
	{
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	iServer = CAgnServer::InitL(this);
	CActiveScheduler::Add(this);
	}

void CAgnServUndertaker::DoCancel()
	{
	}
void CAgnServUndertaker::RunL()
	{
	if (!iCancelled)
		{
		Deque();
		delete iServer;

		CActiveScheduler::Stop();
		}
	}


void CAgnServUndertaker::DestroyServer()
	{
	// Set the undertaker active, and make a request
	if (!IsActive())
		{
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status,KErrNone);
		}

	iCancelled = EFalse;
	}

void CAgnServUndertaker::CancelUndertaker()
	{
	iCancelled = ETrue;
	}

// CAgnAlarmServerChangeListener
//
CAgnAlarmServerChangeListener* CAgnAlarmServerChangeListener::NewL(RASCliSession& aAlarmServer, CAgnServFileMgr& aFileMgr)
    {
    CAgnAlarmServerChangeListener* self = new(ELeave) CAgnAlarmServerChangeListener(aAlarmServer, aFileMgr);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return (self);
    }

CAgnAlarmServerChangeListener::CAgnAlarmServerChangeListener(RASCliSession& aAlarmServer, CAgnServFileMgr& aFileMgr)
    :CActive(EPriorityLow), iAlarmServer(aAlarmServer), iFileMgr(aFileMgr)
    {
    CActiveScheduler::Add(this);
    }
    
void CAgnAlarmServerChangeListener::ConstructL()
    {
    SetActive();
    iAlarmServer.NotifyChange(iStatus, iAlarmId);
    }


CAgnAlarmServerChangeListener::~CAgnAlarmServerChangeListener()
    {
    Cancel();
    }

#ifdef __CAL_VERBOSE_LOGGING__
TInt CAgnAlarmServerChangeListener::RunError(TInt aError)
    {
    AgmDebug::DebugLog("CAgnAlarmServerChangeListener::RunError has error %d", aError);
    return ( KErrNone );
    }
#else
TInt CAgnAlarmServerChangeListener::RunError(TInt /*aError*/)
    {
    return ( KErrNone );
    }
#endif


void CAgnAlarmServerChangeListener::RunL()
    {  
    TInt change(iStatus.Int());
    
    if (iStatus.Int() != KErrServerTerminated)
        {
        SetActive();
        iAlarmServer.NotifyChange(iStatus, iAlarmId);
        }
    
    // Tell each file that there has been a change in the alarm server
    const TInt KFileCount(iFileMgr.Count());
    
    for (TInt i(0) ; i < KFileCount ; ++i)
        {
        iFileMgr.File(i)->Model()->Alarm().ChangeObserver().HandleAlarmServerChangeL(change);
        }
    }


void CAgnAlarmServerChangeListener::DoCancel()
    {
    if (iStatus == KRequestPending)
        {
        iAlarmServer.NotifyChangeCancel();
        }
    }



// Initialize the server - create a CAgnServer object, and an
// active scheduler

CAgnServer* CAgnServer::InitL(CAgnServUndertaker* aUndertaker)
	{
	// start server
	CAgnServer* server = CAgnServer::NewL();
	server->iUndertaker = aUndertaker;
	return server;
	}


CAgnServer::CAgnServer(TInt aPriority)
 : CPolicyServer(aPriority, KAgnSrvrPolicy, ESharableSessions), iServerMode(ETransientServer)
	{
	}


// Create & start a new Agenda server
CAgnServer* CAgnServer::NewL()
	{
	CAgnServer* self = new (ELeave) CAgnServer(KServerPriority);
	CleanupStack::PushL(self);
	self->ConstructL();



	CleanupStack::Pop();		// self

	return self;
	}

CAgnTlsProxy* CAgnServer::TimeZoneConverter()
	{
	return iProxy;
	}

// Create a file manager and connect to the file server
// Rename the server process to "AgendaServer"
// Set up the log file, if logging is required
// Start the server
//
void CAgnServer::ConstructL()
	{
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	_LIT(KCmdLineArgNonTransientServer, "-nontransient");
	const TInt count = args->Count();
	for (TInt i = 1; i < count; i++)
		{
		if (args->Arg(i).CompareF(KCmdLineArgNonTransientServer) == 0)
			{
			iServerMode = ENonTransientServer;
			break;
			}
		}
	CleanupStack::PopAndDestroy(args);	
	
	iProxy = CAgnTlsProxy::CreateL(CAgnTlsProxy::TAgnTlsTzRulesType_Server);
			
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError( iFs.ShareProtected() );
	User::LeaveIfError(iAlarmServer.Connect());
	iFileMgr = CAgnServFileMgr::NewL(iFs, *this);
	iFileMgr->CreatePermanentDataL();
	iBackupRestoreAgent = CAgnServBackupRestoreAgent::NewL(*iFileMgr);
	iBackupRestoreAgent->Start();
	iAlarmServerChangeListener = CAgnAlarmServerChangeListener::NewL(iAlarmServer, *iFileMgr);


#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	TRAP_IGNORE(iSystemStateObserver = CCAgnSystemStateObserver::NewL(*iFileMgr));
#endif
	User::LeaveIfError(RThread().RenameMe(KAgendaServerName));

	StartL(KAgendaServerName);

	TInt err = RProperty::Define(KCalPubSubCategory, ECalPubSubTodoNotification, RProperty::EByteArray, KReadPolicy, KWritePolicy);   
	if (err != KErrNone && err != KErrAlreadyExists)
	    {
	    User::Leave(err);
	    }
	    
	err = RProperty::Define(KCalPubSubCategory, ECalPubSubEventNotification, RProperty::EByteArray, KReadPolicy, KWritePolicy);   
	if (err != KErrNone && err != KErrAlreadyExists)
	    {
	    User::Leave(err);
	    }
	}


CAgnServer::~CAgnServer()
	{
	delete iAlarmServerChangeListener;
	
	iAlarmServer.Close();
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	delete iSystemStateObserver;
#endif
	delete iBackupRestoreAgent;
	delete iFileMgr;
	iFs.Close();
	

	__ASSERT_DEBUG(iProxy != NULL, User::Invariant());
	CAgnTlsProxy::Release(NULL);
	}


// Create a new server session

CSession2* CAgnServer::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage*/) const
	{
	TVersion ver(KAgnServerMajorVersion, KAgnServerMinorVersion, KAgnServerBuildVersion);
	if (!User::QueryVersionSupported(ver,aVersion))
		User::Leave(KErrNotSupported);

	// Call this just in case a previous session caused the undertaker to run
	iUndertaker->CancelUndertaker();

	CAgnServer* nonConstPtr = const_cast<CAgnServer*>(this);
	return CAgnServerSession::NewL(*nonConstPtr);
	}


void CAgnServer::SessionClosed()
	{
	// Called when a session exits
	// If this is the last session for the server, then close down the active object
	iSessionCount--;

	ShutdownServer();
	}

// Try to shutdown the server making sure that no sessions are connected to it, its in transient mode and no file delay timers are running
void CAgnServer::ShutdownServer()
	{
	if (iSessionCount == 0 && (iServerMode == ETransientServer) && !(iFileMgr->FileCloseTimersRunning()))
		{
		// Destroy the server
		iUndertaker->DestroyServer();
		}
	}

void CAgnServer::SessionAdded()
	{
	iSessionCount++;
	}

RFs* CAgnServer::FsSession()
	{
	return &iFs;
	}

void CAgnServer::FetchSessionsL(RPointerArray<CAgnServerSession>& aSessions)
  	{
  	iSessionIter.SetToFirst();
  	
  	while (iSessionIter != NULL)
  		{
  		aSessions.AppendL(static_cast<CAgnServerSession*>(iSessionIter++));
  		}
  	}

TAgnServerMode CAgnServer::ServerMode()
	{ 
	return iServerMode; 
	}
	
const CAgnServBackupRestoreAgent& CAgnServer::BackupRestoreAgent() const
	{
	return *iBackupRestoreAgent;	
	}

RASCliSession& CAgnServer::AlarmServer()
    {
    return iAlarmServer;    
    }

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
const CCAgnSystemStateObserver& CAgnServer::SystemStateObserver() const
    {
    return *iSystemStateObserver;
    }
#endif
