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


#ifndef __CIMAPAPPEND_H__
#define __CIMAPAPPEND_H__

#include <e32std.h>
#include "cimapcommand.h"

// Forward Declaration
class CImSendMessage;
class CImapSession;

class CImapAppend;

/**
The CImapAppend class encapsulates the sending of the IMAP APPEND
command to the remote server. It  parses the response data from
the remote server.
@internalTechnology
@prototype
*/
class CImapAppend : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapAppend* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImSendMessage& aSource, const TDesC& aDestinationMailboxName, CImapSession& aParentSession);
	~CImapAppend();

private:
	CImapAppend(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImSendMessage& aSource, CImapSession& aParentSession);
	void ConstructL(const TDesC& aDestinationMailboxName);

	void SendDataCnfL();
	void SendNextMessageLineL();

	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	void Cancel();
	TBool CanCompleteFlushNow() const;

	static TInt MessageSizeCalculationStep(TAny* aSelf);
	TInt MessageSizeCalculationStepL();
	void SendAppendCommandWithSizeL();
		
	void ParseContinuationResponseL();

private:
	CImSendMessage& iSource;
	/**
	This object is used to split the message size calculation into steps: one per line of the message.
	Each step is initiated by the active scheduler when no other processing is taking place.
	*/
	CIdle* iBackgroundCalculationStepper;
	/** 
	The parent session - refereneced so that it can receive failure notification if a 
	leave occurs during MessageSizeCalculationStepL()
	*/
	CImapSession& iParentSession;
	TInt iTotalMessageSize;

	TInt iPadCount;
	HBufC8* iLineBufferForSend;
	HBufC8* iDestinationMailboxName;
	MOutputStream* iStream; // not owned
	
	/**
	Represents the sequence of steps involved in sending the append message.
	The order of the enumeration steps matches the order they are carried out.
	CanCompleteFlushNow() relies on this ordering to decide whether data has been sent yet.
	*/
	enum 
		{
		EAppendWaitingToStart,
		EAppendCalculatingSize,
		EAppendWaitingForContinuationResponse,
		EAppendSendingMessageData,
		EAppendSendingLastMessageData,
		EAppendFinishedSending
		
		} iAppendCommandSendState;
	};

#endif // __CIMAPAPPEND_H__
