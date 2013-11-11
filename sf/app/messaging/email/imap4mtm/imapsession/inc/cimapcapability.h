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

#ifndef __CIMAPCAPABILITY_H__
#define __CIMAPCAPABILITY_H__

#include <e32std.h>

#include "cimapcommand.h"

// Forward Declaration
class CImapCapabilityInfo;

/**
The CImapCapability class encapsulates the sending of the IMAP capability
command to the remote server. It will also parse the response data from
the remote server. The parser API, having parsed the response data, 
will set the iCapabilityInfo variable bit filled depending on the capabilities 
returned by the remote server.
@internalTechnology
@prototype
*/
class CImapCapability : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapCapability* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapCapabilityInfo& aCapabilityInfo);
	~CImapCapability();

protected:
	CImapCapability(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapCapabilityInfo& aCapabilityInfo);
	void ConstructL();
	
	void ParseCapabilityDataL(const TDesC8& aCapabilityData);
	
private:
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	TParseBlockResult ParseUntaggedResponseL();

private:
	CImapCapabilityInfo& iCapabilityInfo;
	};
					 
#endif // __CIMAPCAPABILITY_H__
