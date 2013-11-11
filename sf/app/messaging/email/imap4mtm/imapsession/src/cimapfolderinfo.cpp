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


#include "cimapfolderinfo.h"

#include "imappaniccodes.h"

EXPORT_C CImapFolderInfo* CImapFolderInfo::NewL()
// static method
	{
	CImapFolderInfo* self = new(ELeave)CImapFolderInfo();
	return self;
	}
	
CImapFolderInfo::~CImapFolderInfo()
	{
	delete iName;
	iExpungedMessages.Close();
	}

EXPORT_C void CImapFolderInfo::SetMsvId(TMsvId aMsvId)
	{
	iMsvId = aMsvId;
	}
EXPORT_C TMsvId CImapFolderInfo::MsvId() const
	{
	return iMsvId;
	}
	
EXPORT_C void CImapFolderInfo::SetNameL(const TDesC& aName)
	{
	HBufC* name = aName.AllocL();
	delete iName;
	iName = name;
	}
EXPORT_C const TDesC& CImapFolderInfo::Name() const
	{
	return iName ? *iName : KNullDesC();
	}

void CImapFolderInfo::SetExists(TInt aExists)
	{
	iExists = aExists;
	}
EXPORT_C TInt CImapFolderInfo::Exists() const
	{
	return iExists;
	}

EXPORT_C void CImapFolderInfo::SetRecent(TInt aRecent)
	{
	iRecent = aRecent;
	}
EXPORT_C TInt CImapFolderInfo::Recent() const
	{
	return iRecent;
	}

/**
Appends the message sequence number to the array of expunged messages,
and decrements the overall message count.
*/	
void CImapFolderInfo::AddExpungedMessageL(TUint aMessageSequenceNumber)
	{
	iExpungedMessages.AppendL(aMessageSequenceNumber);
	--iExists;
	
	__ASSERT_DEBUG(iExists >= 0, TImapServerPanic::ImapPanic(TImapServerPanic::EFolderInfoNegativeExistsCount));
	}

EXPORT_C void CImapFolderInfo::ResetExpungedMessages()
	{
	iExpungedMessages.Reset();
	}

/**
@return A read-only version of the expunged message array.
*/
EXPORT_C const RArray<TUint> CImapFolderInfo::ExpungedMessages()
	{
	return iExpungedMessages;
	}

/**
Sets to ETrue when the IMAP server has indicated that the flags of one or more messages has changed.
Resets to EFalse in order to wait for the next server notification.
@param aMessageFlagsChanged Whether the IMAP server has recently indicated that the flags of one or more messages has changed.
*/
EXPORT_C void CImapFolderInfo::SetMessageFlagsChanged(TBool aMessageFlagsChanged)
	{
	iMessageFlagsChanged = aMessageFlagsChanged;
	}
	
/**
@return Whether the IMAP server has recently indicated that the flags of one or more messages has changed.
*/
EXPORT_C TBool CImapFolderInfo::MessageFlagsChanged() const
	{
	return iMessageFlagsChanged;
	}

/**
Returns the value of iUnseen 
The meaning of this value depends upon the context in which it is being used.
In response to a SELECT command, iUnseen is the ID of the first unseen message
See section 7.1 of RFC 3501.
The CImapFolderInfo object returned by CImapSession::SelectedFolderInfo() will always 
have this meaning.
In response to a STATUS command, iUnseen is the number of unseen messages.
See section 6.3.10 of RFC 3501.
The CImapFolderInfo object populated by CImapSession::StatusL() will always 
have this meaning.
@return the value of iUnseen.
*/
void CImapFolderInfo::SetUnseen(TUint aUnseen)
	{
	iUnseen = aUnseen;
	}
EXPORT_C TUint CImapFolderInfo::Unseen() const
	{
	return iUnseen;
	}

void CImapFolderInfo::SetUidNext(TUint aUidNext)
	{
	iUidNext = aUidNext;
	}
EXPORT_C TUint CImapFolderInfo::UidNext() const
	{
	return iUidNext;
	}

void CImapFolderInfo::SetUidValidity(TUint aUidValidity)
	{
	iUidValidity = aUidValidity;
	}
EXPORT_C TUint CImapFolderInfo::UidValidity() const
	{
	return iUidValidity;
	}

void CImapFolderInfo::SetMessages(TInt aMessages)
	{
	iMessages = aMessages;
	}
	
EXPORT_C TInt CImapFolderInfo::Messages() const
	{
	return iMessages;
	}

void CImapFolderInfo::SetReadWrite(TBool aMailboxWritable)
	{
	 iMailboxWritable = aMailboxWritable;
	}
	
EXPORT_C TBool CImapFolderInfo::GetReadWrite() const
	{
	return iMailboxWritable;
	}
	
/**
Set the flag identified by aFlagId to the value of aValue.
@param aFlagId Identifies which flag to set.
@param aValue The value to apply to the flag.
*/
void CImapFolderInfo::SetFlag(TIMapMailboxFlags aFlagId, TBool aValue)
	{
	if (aValue)
		{
		iFlags |= aFlagId;
		}
	else
		{
		iFlags &= ~aFlagId;
		}
	}
	
/**
Retrieve the value of the flag identified by aFlagId.
@param aFlagId Identifies which flag to query.
@return The value of the querried flag.
*/
EXPORT_C TBool CImapFolderInfo::QueryFlag(TIMapMailboxFlags aFlagId) const
	{
	return iFlags & aFlagId;
	}
