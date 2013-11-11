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


#ifndef __CIMAPFOLDERINFO_H__
#define __CIMAPFOLDERINFO_H__

#include <e32base.h>
#include <msvstd.h>

/**
Stores the folder information that is returned by an IMAP server in its responses.
@internalTechnology
@prototype
*/
class CImapFolderInfo : public CBase
	{
public:
	enum TIMapMailboxFlags {
					/** 
					Message has been read
					*/
					ESeen = 0x1,
					/**
					Message has been answered
					*/
					EAnswered = 0x2, 
					/**
					Message is "flagged" for urgent/special attention
					*/
					EFlagged = 0x4,
					/**
					Message is "deleted" for removal by later EXPUNGE
					*/
					EDeleted = 0x8,
					/**
					Message has not completed composition (marked as a draft).
					*/
					EDraft = 0x10,
					};

	IMPORT_C static CImapFolderInfo* NewL();
	
	~CImapFolderInfo();

	IMPORT_C void SetMsvId(TMsvId aMsvId);
	IMPORT_C TMsvId MsvId() const;
	
	IMPORT_C void SetNameL(const TDesC& aName);
	IMPORT_C const TDesC& Name() const;
	
	void SetFlag(TIMapMailboxFlags aFlagId, TBool aValue);
	IMPORT_C TBool QueryFlag(TIMapMailboxFlags aFlagId) const;
	
	void SetExists(TInt aExists);
	IMPORT_C TInt Exists() const;
	
	IMPORT_C void SetRecent(TInt aRecent);
	IMPORT_C TInt Recent() const;

	void AddExpungedMessageL(TUint aMessageSequenceNumber);
	IMPORT_C void ResetExpungedMessages();
	IMPORT_C const RArray<TUint> ExpungedMessages();
	
	IMPORT_C void SetMessageFlagsChanged(TBool aMessageFlagsChanged);
	IMPORT_C TBool MessageFlagsChanged() const;	
	
	void SetUnseen(TUint aUnseen);
	IMPORT_C TUint Unseen() const;
	
	void SetUidNext(TUint aUidNext);
	IMPORT_C TUint UidNext() const;
	
	void SetUidValidity(TUint aUidValidity);
	IMPORT_C TUint UidValidity() const;		
	
	void SetMessages(TInt aMessages);
	IMPORT_C TInt Messages() const;

	void SetReadWrite(TBool aMailboxWritable);
	IMPORT_C TBool GetReadWrite() const;
private:
	TMsvId	iMsvId;

	HBufC*	iName;
	TUint8	iFlags;
	TInt	iExists;
	TInt	iRecent;
	RArray<TUint> iExpungedMessages;
	/**
	This is set to ETrue whenever an unsolicited FETCH response has been received.
	When ETrue, it means that one or more of the messages in the currently selected
	mailbox has recently had its flag's changed.
	*/
	TBool	iMessageFlagsChanged;
	
	TBool 	iMailboxWritable;
	/**
	The meaning of iUnseen depends upon the context in which it is being used.
	In response to a SELECT command, iUnseen is the ID of the first unseen message
	See section 7.1 of RFC 3501.
	The CImapFolderInfo object returned by CImapSession::SelectedFolderInfo() will always 
	have this meaning.
	In response to a STATUS command, iUnseen is the number of unseen messages.
	See section 6.3.10 of RFC 3501.
	The CImapFolderInfo object populated by CImapSession::StatusL() will always 
	have this meaning.
	*/
	TUint	iUnseen;
	TInt 	iMessages;
	TUint	iUidNext;
	TUint	iUidValidity;
	};
#endif // __CIMAPFOLDERINFO_H__
