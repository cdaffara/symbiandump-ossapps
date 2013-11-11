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

#ifndef __MIMAPSESSIONOBSERVER_H__
#define __MIMAPSESSIONOBSERVER_H__

/**
Provides an API to allow the session to provide callback notifications to its
parent.

@internalComponent
@released
*/
class MImapSessionObserver
	{
public:
	/**
	Provides notification of a failure that occurs on the session while the
	parent does not have an outstanding asynchronous operation running on it.
	For example, this could be used to notify of an error that occurs during
	the cancel and idle operation.
	*/
	virtual void NonCompletedFailure()=0;
	};

#endif	//__MIMAPSESSIONOBSERVER_H__
