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

#include "cimapservergreetinginfo.h"


EXPORT_C CImapServerGreetingInfo* CImapServerGreetingInfo::NewL()
// static method
	{
	return new(ELeave)CImapServerGreetingInfo();
	}

EXPORT_C CImapServerGreetingInfo::TResponseTag CImapServerGreetingInfo::ResponseTag() const
	{
	return iResponseTag;
	}
	
EXPORT_C void CImapServerGreetingInfo::SetResponseTag(TResponseTag aResponseTag)
	{
	iResponseTag = aResponseTag;
	}

CImapServerGreetingInfo::CImapServerGreetingInfo()
	{}
