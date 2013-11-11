// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CIMAPSYNCDOWNLOADRULES_H__
#define __CIMAPSYNCDOWNLOADRULES_H__

#include <e32std.h>
#include <imapset.h>

/**
Maximum number of download rules that can be stored

@publishedPartner
@released
*/
const TInt KMaxImapSyncDownloadRules = 32;

/**
Stores the download rules to be used during the IMAP synchronisation phase.

Each rule consists of a bearer types field, and a set of get mail options.
The bearer types are defined by the TCommDbBearer enumeration and it is
possible to specify multiple bearer types for one rule by performing a
bitwise OR operation on the values from the enumeration and passing the
resulting value to the various APIs. The get mail options are defined by
the TImImap4GetPartialMailInfo structure.

There are two sets of download rules, one set for the inbox and one set for
other folders. Generally each of the APIs takes a flag to indicate which set
to operate upon.

@publishedPartner
@released
*/
class CImapSyncDownloadRules : public CBase
	{
public:
	/**
	Defines the type of folder (inbox or other folder)
	*/
	enum TRulesType
		{
		EInboxRulesType,
		EFolderRulesType
		};

public:
	IMPORT_C static CImapSyncDownloadRules* NewL();
	IMPORT_C static CImapSyncDownloadRules* NewLC();
	~CImapSyncDownloadRules();

	IMPORT_C CImapSyncDownloadRules* CopyL();
	IMPORT_C CImapSyncDownloadRules* CopyLC();
	IMPORT_C TInt AddRuleL(TRulesType aType, TInt aPos, TUint32 aBearerTypes, const TImImap4GetPartialMailInfo& aMailInfo);
	IMPORT_C TBool DeleteRuleL(TRulesType aType, TInt aPos);
	IMPORT_C TInt DeleteBearerTypesL(TRulesType aType, TUint32 aBearerTypes, TBool& aRuleRemoved);
	IMPORT_C TBool GetRuleL(TRulesType aType, TInt aPos, TUint32& aBearerTypes, TImImap4GetPartialMailInfo& aMailInfo) const;
	IMPORT_C TInt GetMailInfoL(TRulesType aType, TUint32 aBearerTypes, TImImap4GetPartialMailInfo& aMailInfo) const;
	IMPORT_C void Reset();

private:
	CImapSyncDownloadRules();
	void ConstructL();

private:
	struct TImapSyncDownloadRules
		{
		TUint32 iBearerTypes;
		TImImap4GetPartialMailInfo iMailInfo;
		};

private:
	RArray<TImapSyncDownloadRules> iRulesList[2];
	};

#endif //__CIMAPSYNCDOWNLOADRULES_H__
