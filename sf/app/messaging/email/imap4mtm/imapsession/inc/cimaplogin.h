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

#ifndef __CIMAPLOGIN_H__
#define __CIMAPLOGIN_H__

#include <e32std.h>
#include "cimapcommand.h"
#include "cimapcapability.h"


/**
The CImapLogin class encapsulates the sending of the IMAP LOGIN
command to the remote server. Some servers might also send back CAPABILITY response
along with the LOGIN response. The class will parse the response data from the remote server.
@internalTechnology
@prototype
*/
class CImapLogin : public CImapCapability
	{
public:
	// Construction and Destruction
	static CImapLogin* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC8& aUserName, const TDesC8& aPassword, CImapCapabilityInfo& aCapabilityInfo);
	~CImapLogin();
	
private:
	CImapLogin(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapCapabilityInfo& aCapabilityInfo);
	void ConstructL(const TDesC8& aUserName, const TDesC8& aPassword);	
	
	void CheckLiteralInUserNameAndPassword();
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	void ParseContinuationResponseL();
	
	TBool ParseTaggedResponseL(TInt aTagId);	
	
private:
	HBufC8* iUserName; 
	HBufC8* iPassword;	
	TBool iLiteralUsername;
	TBool iLiteralPassword;
	MOutputStream* iStream;
	TBool iSentUserNameLength;
	};
			 
#endif // __CIMAPLOGIN_H__
