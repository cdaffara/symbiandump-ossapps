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

#ifndef __CIMAPLISTBASE_H__
#define __CIMAPLISTBASE_H__

#include <e32std.h>

#include "cimapcommand.h"
#include "cimapsession.h"
#include "cimaplistfolderinfo.h"

class CImapAtomParser;
class CImapAtomWalker;

/**
Provides the implementation for the sending, receiving and parsing of the IMAP LIST and LSUB commands.
@internalTechnology
@prototype
*/
class CImapListBase : public CImapCommandEx
	{
public:
	// Construction and Destruction
	~CImapListBase();
	
protected:
	CImapListBase(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArrayImapListFolderInfo& aFolderList, const CImapSettings& aImapSettings, const TDesC8& aSendMessageFormat, const TDesC8& aResponseIdentifier);
	void ConstructL(const TDesC& aReferenceName, const TDesC& aMailboxName);

private:
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	
	TParseBlockResult ParseUntaggedResponseL();
	void ParseLiteralBlockL();
	TBool ParseLineFollowingLiteralL();

	CImapCommand::TParseBlockResult ParseResponseL();
	void ProcessMailboxListL();
	void ProcessMailboxListFlagsL(CImapListFolderInfo& aFolderInfo);
	void ProcessHeirarchySeparatorL(CImapListFolderInfo& aFolderInfo);
	void ProcessMailboxL(CImapListFolderInfo& aFolderInfo);

private:
	HBufC8* iReferenceName;
	HBufC8* iMailboxName;
	RArrayImapListFolderInfo& iFolderList;
	
	const TDesC8& iSendMessageFormat;
	const TDesC8& iResponseIdentifier;
	
	CImapAtomParser* iAtomParser;
	CImapAtomWalker* iAtomWalker;
	
	enum TListParseState {
		EWaitingForResponse,	
		EWaitingForLiteral,	
		EWaitingForLine,	
		} iListParseState;

	const CImapSettings& iImapSettings;

	};

#endif // __CIMAPLISTBASE_H__
