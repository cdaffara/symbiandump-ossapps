/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CSconSyncServer implementation
*
*/


#include "sconsyncserver.h"
#include "sconsyncclientserver.h"

#include "sconsyncsession.h"
#include "debug.h"

const TInt KSConPCConnServerUid = 0x101F99F6;

// Security policy
const TUint KServerPolicyRangeCount = 2;

const TInt KServerPolicyRanges[KServerPolicyRangeCount] =
    {
    0,              // range is 0 inclusive
    EMaxService     // range is 1-KMaxTInt inclusive
    };

const TUint8 KServerPolicyElementsIndex[KServerPolicyRangeCount] =
    {
    0,                              // applies to 0th range
    CPolicyServer::ENotSupported    // applies to 1st range
    };

const CPolicyServer::TPolicyElement KServerPolicyElements[] =
    {
    { _INIT_SECURITY_POLICY_S0(KSConPCConnServerUid) /*_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData)*/, CPolicyServer::EFailClient }
    };

const CPolicyServer::TPolicy KServerPolicy =
    {
    0 /*CPolicyServer::EAlwaysPass*/, // specifies all connect attempts should pass
    KServerPolicyRangeCount,                   
    KServerPolicyRanges,
    KServerPolicyElementsIndex,
    KServerPolicyElements
    };


CSconSyncServer* CSconSyncServer::NewL()
    {
    CSconSyncServer* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CSconSyncServer* CSconSyncServer::NewLC()
    {
    CSconSyncServer* self = new(ELeave) CSconSyncServer();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CSconSyncServer::CSconSyncServer() : 
    CPolicyServer(EPriorityStandard, KServerPolicy, ESharableSessions)
    {
    }

void CSconSyncServer::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    TInt error = Start(KSconSyncServerName);

    if (error != KErrNone)
        {
        User::Panic(KSconSyncServerName, error);
        }
    TRACE_FUNC_EXIT;
    }

CSconSyncServer::~CSconSyncServer()
    {
    TRACE_FUNC;
    }

CSession2* CSconSyncServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
    {
    TRACE_FUNC;
    TVersion v(1,0,0);
    if ( !User::QueryVersionSupported(v,aVersion) )
        {
        TVersionName vname = aVersion.Name();
        LOGGER_WRITE_1("Client has wrong version: %S", &vname);
        User::Leave( KErrNotSupported );
        }
    
    return CSconSyncSession::NewL();
    }

TInt CSconSyncServer::RunError(TInt aError)
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aError: %d", aError);
    // Bad descriptor implies bad client
    if (aError == KErrBadDescriptor)
        {
        Message().Panic(KSconSyncServerName, aError);
        }
    else
        {
        Message().Complete(aError);
        }

    // Continue handling requests
    ReStart();
    TRACE_FUNC_EXIT;
    return KErrNone;
    }

void CSconSyncServer::AddSession()
    {
    ++iSessionCount;
    LOGGER_WRITE_1("CSconSyncServer::AddSession(), iSessionCount: %d", iSessionCount);
    }

void CSconSyncServer::RemoveSession()
    {
    --iSessionCount;
    LOGGER_WRITE_1("CSconSyncServer::RemoveSession(), iSessionCount: %d", iSessionCount);
    if (iSessionCount == 0)
        {
        CActiveScheduler::Stop();
        }
    }

LOCAL_C void ExeMainL()
    {
    TRACE_FUNC_ENTRY;
    // Install active scheduler
    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    CSconSyncServer::NewLC();

    User::LeaveIfError(User::RenameThread(KSconSyncServerName));
    RProcess::Rendezvous(KErrNone);

    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy();  // CSconSyncServer
    CleanupStack::PopAndDestroy();  // CActiveScheduler
    TRACE_FUNC_EXIT;
    }

GLDEF_C TInt E32Main()
    {
    TRACE_FUNC_ENTRY;
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    TRAPD(error, ExeMainL());
    __ASSERT_ALWAYS(!error, User::Panic(KSconSyncServerName, error));

    delete cleanup;
    __UHEAP_MARKEND;
    TRACE_FUNC_EXIT;
    return 0;
    }
