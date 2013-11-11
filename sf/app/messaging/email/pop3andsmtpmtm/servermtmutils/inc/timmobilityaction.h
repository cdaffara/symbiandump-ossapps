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

#ifndef __TIMMOBILITYACTION_H__
#define __TIMMOBILITYACTION_H__

/**
Enumeration for actions that can be taken when closing existing sockets
when accepting a migration to a new carrier.
*/
enum TImMobilityAction
	{
	/**
	KAcceptImmediately:
	If an operation is in progress it shall be cancelled immediately. 
	Sockets connected via the old carrier shall be closed, without 
	first completing the session with the remote server.
	Bearer mobility framework will be instructed to MigrateToNewCarrier().
	Once the new connection is active, any cancelled operation shall be
	restarted.
	*/
	KAcceptImmediately,

	/**
	KAcceptStopCurrent:
	If an operation is in progress it shall be stopped at the next
	opportunity that shall require no repeat of already completed server
	operations that have occured in the execution of the operation up
	to this time. This may result in a batch operation being "paused"
	to allow the migration before it has completed.
	Sockets connected via the old carrier shall have their sessions with
	the remote server log out, and the socket shall be closed gracefully.
	Bearer mobility framework will be instructed to MigrateToNewCarrier().
	Once the new connection is active, any "paused" operation shall be
	resumed.
	*/
	KAcceptStopCurrent,

	/**
	If an operation is in progress it shall be allowed to complete.
	Sockets connected via the old carrier shall then have their sessions
	with the remote server log out, and the socket shall be closed gracefully.
	Bearer mobility framework will be instructed to MigrateToNewCarrier().
	*/
	KAcceptCompleteCurrent
	};

/**
Enumeration describing the cause of completion of a migration handshake.
*/
enum TImMigrateComplete
	{
	KImMigrateOK,
	KImMigrateCouldNotReconnect,
	KImMigrateMobilityError
	};
	
#endif	// __TIMMOBILITYACTION_H__
