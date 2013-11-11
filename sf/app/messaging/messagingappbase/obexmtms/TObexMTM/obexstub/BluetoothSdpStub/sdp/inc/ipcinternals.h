// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Constants and types used for the messages between the sdp client
// code and sdp server code.
// 
//


#ifndef IPCINTERNALS_H
#define IPCINTERNALS_H


/**
Provides server startup parameters.
**/
class TSdpServerStart
	{
public:
	TSdpServerStart(TRequestStatus& aStatus);
	TPtrC AsCommand() const;
	TSdpServerStart();
	TInt GetCommand();
	void SignalL();
public:
	TThreadId iId;
	TRequestStatus* iStatus;
	};

enum TSdpServerRequest
	{
	ESdpCreateDatabaseSubSession,
	ESdpCreateAgentSubSession,
	ESdpCloseSubSession,
	ESdpResourceCountMarkStart,
	ESdpResourceCountMarkEnd,
	ESdpResourceCount,
	ESdpCancelRequest,
	ESdpServiceSearch,
	ESdpAttributeSearch,
	ESdpServiceAttributeSearch,
	ESdpDatabaseCreateServiceRecord,
	ESdpDatabaseUpdateServiceRecordHandle,
	ESdpDatabaseUpdateAttribute,
	ESdpDatabaseDeleteAttribute,
	ESdpDatabaseDeleteServiceRecord
	};

// server name
_LIT(KSdpServerName,"Sdp");
#ifdef __WINS__
_LIT(KSdpServerImg,"Sdp");	//<DLL/EXE name
#else
_LIT(KSdpServerImg,"z:\\system\\libs\\Sdp.exe");	//<DLL/EXE name
#endif
//const TUid KSdpServerUid3={0x100069cc};
const TUid KSdpServerUid3={0x10009220};
#ifdef __WINS__
const TInt KSdpServerStackSize=0x2000;			//  8KB
const TInt KSdpServerInitHeapSize=0x1000;		//  4KB
const TInt KSdpServerMaxHeapSize=0x1000000;		// 16MB
#endif

// A version must be specified when creating a session with the server
const TUint KSdpServerMajorVersionNumber=0;
const TUint KSdpServerMinorVersionNumber=1;
const TUint KSdpServerBuildVersionNumber=1;

#endif
