// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__IMAPCONNECTIONOBSERVER_H__)
#define __IMAPCONNECTIONOBSERVER_H__

/** IMAP4 connection observer events.

@see MMsvImapConnectionObserver 
@publishedAll
@released
*/
enum TImapConnectionEvent
	{
	/** Connecting to server. */
	EConnectingToServer,
	/** Synchronising folder list. */
	ESynchronisingFolderList,
	/** Synchronising inbox. */
	ESynchronisingInbox,
	/** Synchronising folders. */
	ESynchronisingFolders,
	/** Synchronisation complete. */
	ESynchronisationComplete,
	/** Disconnecting. */
	EDisconnecting,
	/** Connection completed. */
	EConnectionCompleted
	};

//	Mix-in class to allow observation of the stages of an IMAP operation
//
class MMsvImapConnectionObserver
/** Abstract base class used for IMAP4 connection observation. 

A derived class to handle IMAP4 connection events should override the HandleImapConnectionEvent() 
member function. 

Objects that implement this interface are passed as a packaged parameter 
to CImap4ClientMtm::InvokeAsyncFunctionL() when a connection command is issued.
 
@publishedAll
@released

@see TImap4Cmds
*/
	{
public:
	/** Called when a connection event occurs.
	
	@param aConnectionState Connection event */
	virtual void HandleImapConnectionEvent(TImapConnectionEvent aConnectionState)=0;
	};

#endif
