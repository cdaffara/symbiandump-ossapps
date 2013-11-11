// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PBAPCLIENTSERVER_H
#define PBAPCLIENTSERVER_H

#include <e32base.h>

/**
 Pbap Client and Server Utilities
*/

// server name
_LIT(KPbapServerName,"!PBAP");			// Server object name
_LIT(KPbapServerImg,"btaccesshost");	// Server image name (.exe implied)


const TUid KPbapServerUid={0x102825b2};

// A version must be specified when creating a session with the server
const TUint KPbapServerMajorVersionNumber=1;
const TUint KPbapServerMinorVersionNumber=0;
const TUint KPbapServerBuildVersionNumber=0;


// opcodes used in message passing between client and server
enum TPbapServerRequest
	{
	EPbapStartListening,
	EPbapStopListening,
	EPbapSetPassword,
	EPbapDbgMarkHeap,
	EPbapDbgCheckHeap,
	EPbapDbgMarkEnd,
	EPbapDbgFailNext
	};


#endif //PBAPCLIENTSERVER_H
