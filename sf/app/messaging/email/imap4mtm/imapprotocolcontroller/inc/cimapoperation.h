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

#ifndef __CIMAPOPERATION_H__
#define __CIMAPOPERATION_H__

#include <e32std.h>
#include <msvstd.h>

class CImapSession;
class CMsgActive;
class CMsvServerEntry;

/**
Base class for IMAP Operation Classes.
@internalTechnology
@prototype
*/
class CImapOperation : public CMsgActive
	{
public:
	~CImapOperation();
	
protected:
	CImapOperation(CImapSession*& aSession, CMsvServerEntry& aServerEntry, TInt aPriority);
	
	void SetEntryL(const TMsvId aId);

private:	
	// from CMsgActive
	virtual void DoRunL()=0;
protected:
	CMsvServerEntry& iServerEntry;
	CImapSession*& iSession;
	};

#endif	//__CIMAPOPERATION_H__

