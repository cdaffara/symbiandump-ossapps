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

#ifndef __MIMAPOPBACKGROUNDSYNCOBSERVER_H__
#define __MIMAPOPBACKGROUNDSYNCOBSERVER_H__

/**
Interface class for background synchronisation observer.

@internalTechnology
@prototype
*/
class MImapOpBackgroundSyncObserver
	{
public:
	virtual void BackgroundSyncComplete( TInt aError ) = 0;
	};

#endif	//__MIMAPOPBACKGROUNDSYNCOBSERVER_H__

