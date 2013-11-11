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

#ifndef __TMESSAGEFLAGINFO_H__
#define __TMESSAGEFLAGINFO_H__

#include <e32std.h>


/**
Represents the flags of a server-side message 
@internalTechnology
@prototype
*/
class TMessageFlagInfo
	{
public:
	enum TImapMessageFlags {
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
					/**
					Message is "recently" arrived in this mailbox - can be affected by other sessions
					*/
					ERecent = 0x20
					};

	TMessageFlagInfo();

	void SetFlag(TImapMessageFlags aFlagId, TBool aValue);
	IMPORT_C TBool QueryFlag(TImapMessageFlags aFlagId);
	
	TPtrC8 ParseFlagsL(const TDesC8& aFlagsData);
	TInt SetMessageUid(const TDesC8& aMessageUid);
	IMPORT_C TUint MessageUid() const;

private:	
	TUint iMessageUid; 
	
	/**
	Bitwise combinaton of TImapMessageFlags.
	*/
	TUint8 iFlags;
	};

typedef RArray<TMessageFlagInfo> RArrayMessageFlagInfo;

#endif // __TMESSAGEFLAGINFO_H__
