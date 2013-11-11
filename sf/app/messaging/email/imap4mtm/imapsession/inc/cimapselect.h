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


#ifndef __CIMAPSELECT_H__
#define __CIMAPSELECT_H__

#include <e32std.h>
#include "cimapcommand.h"

// Forward Declaration
class CImapFolderInfo;

/**
The CImapSelect class encapsulates the sending of the IMAP select
command to the remote server. It will also parse the response data from
the remote server.
@internalTechnology
@prototype
*/
class CImapSelect : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapSelect* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	~CImapSelect();
	
protected:
	// Constructors need to be accessible from CImapExamine
	CImapSelect(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	TBool ParseTaggedResponseL(TInt aTagId);
	TParseBlockResult ParseUntaggedResponseL();
	
private:
	void ParseFlagsL(TDesC8& aFlags);
	void ParseUntaggedOkL();
	};
			 
#endif // __CIMAPSELECT_H__
