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


#ifndef __CIMAPFETCHBODYRESPONSE_H__
#define __CIMAPFETCHBODYRESPONSE_H__

#include <e32base.h>

#include "tmessageflaginfo.h"

class CImapMimeHeaderFields;

/**
Represents the results of a FETCH BODY request.
@internalTechnology
@prototype
*/
class CImapFetchBodyResponse : public CBase
	{
public:
	IMPORT_C static CImapFetchBodyResponse* NewL();
	~CImapFetchBodyResponse();
	
	IMPORT_C CImapMimeHeaderFields* HeaderFields();
	IMPORT_C TMessageFlagInfo& MessageFlagInfo();

	// non-exported public methods
	void SetHeaderFields(CImapMimeHeaderFields* aHeaderFields);
	void SetMessageFlagInfo(const TMessageFlagInfo& aMessageFlagInfo);

private:	
	/**
	Stores any message flags information recieved for this response.
	*/
	TMessageFlagInfo iMessageFlagInfo;
	/**
	Stores MIME header fields information.  May be NULL.
	*/
	CImapMimeHeaderFields* iHeaderFields;
	};

#endif // __CIMAPFETCHBODYRESPONSE_H__
