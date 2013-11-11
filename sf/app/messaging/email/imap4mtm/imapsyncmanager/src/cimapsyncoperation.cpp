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
// CImapOperation.cpp
// 
//

#include <mentact.h>
#include "cimapsyncoperation.h"


CImapSyncOperation::CImapSyncOperation(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan, CImapSettings& aImapSettings, TInt aPriority ) :
	CMsgActive(aPriority),
	iServerEntry(aEntry),
	iSession(aSession),
	iSyncMan(aSyncMan),
	iImapSettings(aImapSettings)
	{
	}

CImapSyncOperation::~CImapSyncOperation()
	{
	}
