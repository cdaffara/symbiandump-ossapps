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
// imapopsyncsubs.h
// CImapOpSyncSubs header
// 
//

#if !defined(__IMAPOPSYNCSUBS_H__)
#define __IMAPOPSYNCSUBS_H__

#include <mentact.h>
#include <msventry.h>
#include "cimapsyncoperation.h"

// Forward declarations
class CImapSession;
class CImapSyncManager;
class CImapSettings;

class CImapOpSyncSubs : public CImapSyncOperation
/**
@internalComponent
@prototype
*/
	{
public:
	static CImapOpSyncSubs* NewL(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan,CImapSettings& aImapSettings);
	static CImapOpSyncSubs* NewLC(CMsvServerEntry& aEntry, CImapSession& aSession,CImapSyncManager& aSyncMan,CImapSettings& aImapSettings);
	void SynchroniseSubscriptionsL(TRequestStatus& aStatus);
	~CImapOpSyncSubs();
	
private:
	CImapOpSyncSubs(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan,CImapSettings& aImapSettings);
	void ConstructL();

	void SetEntryL(const TMsvId aId);
	void ChangeEntryL(const TMsvEntry& aEntry);
	void GetChildrenL(CMsvEntrySelection& aSelection);
	void ResetSubscriptionFlagsL(const TMsvId aFolder);

	void DoRunL();
	void DoCancel();
	void DoComplete(TInt& aStatus);
	void CompleteSelf();
private:
	TMsvId iServiceId;
	CImapSyncManager::TSynchroniseState iNextSyncAction;
	RArrayImapListFolderInfo iImapListFolderInfo;

	};

#endif // __IMAPOPSYNCSUBS_H__
