// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __EVENTHANDLER_H__
#define __EVENTHANDLER_H__

class MImapTestEventHandler
	{
public:
	virtual void HandleEvent(TInt aEventCode)=0;
	};
	
#endif //__EVENTHANDLER_H__
	