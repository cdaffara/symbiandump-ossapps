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

#ifndef __IMAPSTATUS_H__
#define __IMAPSTATUS_H__

#include <e32std.h>
#include "cimapcommand.h"
#include "cimapfolderinfo.h"
#include "cimapatomparser.h"

/**
The CImapStatus class encapsulates the sending of the IMAP STATUS
command to the remote server. It will also parse the response data from the remote server.
@internalTechnology
@prototype
*/
class CImapStatus : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapStatus* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aMailboxName, const TDesC8& aStatusDataItemNames, CImapFolderInfo& aFolderInfo);
	~CImapStatus();
	
private:
	CImapStatus(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapFolderInfo& aFolderInfo);
	void ConstructL(const TDesC& aMailboxName, const TDesC8& aStatusDataItemNames);	
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);	
	
	TParseBlockResult ParseUntaggedResponseL();
	void ParseLiteralBlockL();
	TBool ParseLineFollowingLiteralL();
	
	void ParseStatusResponseL();
	void ParseStatusAttributeL(const TDesC8& aAttribute, const TDesC8& aValue);
	
private:
	// Data for send
	HBufC8* 		iMailboxName;
	HBufC8* 		iStatusDataItemNames;
	TInt 		iLiteralSize;
	CImapFolderInfo& iFolderInfo;
	CImapAtomParser* iAtomParser;
	};
			 
#endif // __IMAPSTATUS_H__
