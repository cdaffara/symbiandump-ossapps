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
// Base class for IMAP Synchronisation Operation Classes
// 
//

#ifndef __CIMAPSYNCOPERATION_H__
#define __CIMAPSYNCOPERATION_H__

#include <e32std.h>
#include <mentact.h>

class CMsvServerEntry;
class CImapSession;
class CImapSyncManager;
class CImapSettings;

_LIT(KStarChar,"*");

/**
@internalTechnology
@prototype
*/
class CImapSyncOperation : public CMsgActive
	{
public:
	~CImapSyncOperation( );
	
protected:
	CImapSyncOperation(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan, CImapSettings& aImapSettings, TInt aPriority);

private:	
	// from CMsgActive
	virtual void DoRunL()=0;
protected:
	CMsvServerEntry& iServerEntry;
	CImapSession& iSession;
	CImapSyncManager& iSyncMan;
	CImapSettings& iImapSettings;
	};

#endif	//__CIMAPSYNCOPERATION_H__

