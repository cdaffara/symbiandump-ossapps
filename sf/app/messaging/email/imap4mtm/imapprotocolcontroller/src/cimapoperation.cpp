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

#include <mentact.h>
#include <msventry.h>
#include "cimapsettings.h"


#include "cimapoperation.h"

CImapOperation::CImapOperation( CImapSession*& aSession, CMsvServerEntry& aServerEntry, TInt aPriority ) :
	CMsgActive( aPriority ),
	iServerEntry( aServerEntry ),
	iSession( aSession )
	{
	}

CImapOperation::~CImapOperation()
	{
	}


/**
Sets the context if the CMsvServerEntry to the specified TMsvId
@param aId  The message server entry identifier.
*/
void CImapOperation::SetEntryL(const TMsvId aId)
	{
	User::LeaveIfError(iServerEntry.SetEntry(aId));
	}

