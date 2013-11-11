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

#ifndef PBAPCHVIEWOBSERVER_H
#define PBAPCHVIEWOBSERVER_H

#include <e32def.h>

class MPbapChViewObserver
	{
public:
	virtual void CallHistoryViewReady(TInt aError) = 0;
	virtual void CallHistoryChangeNotification(TBool aViewReady = ETrue) = 0;
	};

#endif // PBAPCHVIEWOBSERVER_H