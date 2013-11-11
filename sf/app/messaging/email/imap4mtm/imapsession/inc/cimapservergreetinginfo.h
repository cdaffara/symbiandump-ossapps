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

#ifndef __CIMAPSERVERGREETINGINFO_H__
#define __CIMAPSERVERGREETINGINFO_H__

#include <e32base.h>

/**
Represents the contents of the initial greeting that
is received when a connection is made to the server.
@internalTechnology
@prototype
*/
class CImapServerGreetingInfo : public CBase
	{
	public:
		enum TResponseTag
			{
			/**
			A server greeting response has not yet been read.
			*/
			ETagNone,
			/**
			The server connection initialised in the Non Authenticated state.
			*/
			ETagOk,
			/**
			The server connection initialised in the Authenticated state.
			*/
			ETagPreAuth,
			/**
			The server has refused the connection.
			*/
			ETagBye
			};
	
		IMPORT_C static CImapServerGreetingInfo* NewL();
		
		IMPORT_C TResponseTag ResponseTag() const;
		IMPORT_C void SetResponseTag(TResponseTag aResponseTag);
		
	private:
		CImapServerGreetingInfo();
		
	private:
		TResponseTag iResponseTag;
	};

#endif // __CIMAPSERVERGREETINGINFO_H__