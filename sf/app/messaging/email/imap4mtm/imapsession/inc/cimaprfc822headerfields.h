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

#ifndef __CIMAPRFC822HEADERFIELDS_H__
#define __CIMAPRFC822HEADERFIELDS_H__

#include <msvstd.h>
#include "cimapheaderfields.h"

/**
Stores the RFC822 header information that is returned by a BODY[HEADER.FIELDS(...)] request.
The data is stored as a simple array of heap descriptors, one for each field type.
@internalTechnology
@prototype
*/
NONSHARABLE_CLASS(CImapRfc822HeaderFields) : public CImapHeaderFields
	{
public:
	/**
	Provides an index into the field value array
	*/
	enum THeaderFields
		{
		EImapFrom,
		EImapReplyTo,
		EImapTo,
		EImapCc,
		EImapBcc,
		EImapSubject,
		EImapDate,
		EImapReceived,
		EImapMessageId,
		EImapXMailer,
		EImapPriority,
		EImapXPriority,
		EImapXMSMailPriority,
		EImapPrecedence,
		EImapImportance,
		EImapReturnReceiptTo,
		EImapXReturnReceiptTo,
		EImapDispositionNotificationTo,
		EImapDispositionNotificationOptions,

		EImapCountFields
		};

	static CImapRfc822HeaderFields* NewL();
	~CImapRfc822HeaderFields();

	IMPORT_C const TDesC8& FieldValue(THeaderFields aFieldId);
	IMPORT_C TBool FieldExists(THeaderFields aFieldId);
	IMPORT_C TMsvPriority PriorityL();
	IMPORT_C TTime Date();
	IMPORT_C const TDesC8& ReturnReceiptField();

private:
	CImapRfc822HeaderFields();
	void ConstructL();
	TInt Match(const TDesC8& aFieldName, TBool& aNeedToStripSpaces);
	};

#endif // __CIMAPRFC822HEADERFIELDS_H__
