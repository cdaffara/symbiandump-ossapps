/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PC Connectivity server client
*
*/




#ifndef __SCONPCCONNCLIENTSERVER_H__
#define __SCONPCCONNCLIENTSERVER_H__

#include <e32base.h>

// for debugging ConML messages and capability objects 
//#define DEBUG_XML


// reasons for server panic
enum TPCConnServPanic
	{
	E_BadRequest = 1,
	E_DispatchRequest,
	E_InvalidClient,
	E_CleanupCreate,
	E_MainSchedulerError
	};
	
	
// server related constants
_LIT( KSConPCConnServerName,"SConPCConnServer" );
_LIT( KSConPCConnServerExe, "SConPCConnServer.exe" );

// client 
_LIT( KSConPCConnClientDll, "SConPCConnClient.dll" );


// Chunk
const TUint KSConPCConnChunkSize = 128;
const TUint KSConPCConnChunkMaxSize = 65536;
const TUint KSConPCConnBufferMaxSize = 65000; 

// Version number
const TUint KSConPCConnServerVersionMajor = 1;
const TUint KSConPCConnServerVersionMinor = 0;
const TUint KSConPCConnServerVersionBuild = 1;


// Function codes

enum TRequest 
{
	EPutMessage,
	EGetMessage,
	EResetMessage,
	EChunkMessage
};

#endif // __SCONPCCONNCLIENTSERVER_H__
