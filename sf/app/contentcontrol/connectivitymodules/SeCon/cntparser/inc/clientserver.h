/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contact Parser Server definitions
*
*/


#ifndef __CLIENTSERVER_H__
#define __CLIENTSERVER_H__
#include <e32base.h>

// server name
_LIT(KCntParserName,"Contacts Parser");

// A version must be specified when creating a session with the server

const TUint KCntParserMajorVersionNumber=0;
const TUint KCntParserMinorVersionNumber=1;
const TUint KCntParserBuildVersionNumber=1;

// opcodes used in message passing between client and server
enum TCntParserRqst
	{
	ECntParserServerCancel,
	ECntParserServerClose,
	ECntParserServerCreateIrMCL2,
	ECntParserServerTotalNumOfContacts,
	ECntParserServerDisconnect,
	ECntParserServerCntParsed,
	ECntParserServerCntSaved,
	ECntParserServerCancelAll
	};

#endif