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

#include "tmessageflaginfo.h"

#include "cimapcommand.h"
#include "cimapsessionconsts.h"

/**
Default constructor - initialises its data members to zero.
Note this is not a CBase class, so this does not happen automatically.
*/
TMessageFlagInfo::TMessageFlagInfo()
	: iMessageUid(0), iFlags(0)
	{}

/**
Set the flag identified by aFlagId to the value of aValue.
@param aFlagId Identifies which flag to set.
@param aValue The value to apply to the flag.
*/
void TMessageFlagInfo::SetFlag(TImapMessageFlags aFlagId, TBool aValue)
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
EXPORT_C TBool TMessageFlagInfo::QueryFlag(TImapMessageFlags aFlagId)
	{
	return iFlags & aFlagId;
	}

/**
Parse the flags data and update to iFlags.
@param aFlagsData contains list of flags to be parsed.
@return The remaining portion of the aFlagsData parameter that was not parsed.
*/
TPtrC8 TMessageFlagInfo::ParseFlagsL(const TDesC8& aFlagsData)
	{
	// formal definition of the FLAGS data item of a FETCH FLAGS is
	//
	// msg-att-dynamic = "FLAGS" SP "(" [flag-fetch *(SP flag-fetch)] ")"
	//
	// flag-fetch = flag / "\Recent"
	// flag = "\Answered" / "\Flagged" / "\Deleted" / "\Seen" / "\Draft" / flag-keyword / flag-extension
	// flag-extension = "\" atom
	// flag-keyword = atom
	//
	//

	TPtrC8 parseData(aFlagsData);
	
	TInt start = aFlagsData.Locate('(');
	TInt end = aFlagsData.Locate(')');

	// Reset the flags, prior to setting them.
	iFlags = 0;

	// Now set them.
	if ((start >= 0) && (end > start))
		{
		RDesParts flags;
		CleanupClosePushL(flags);
		
		// lose the brackets
		parseData.Set(aFlagsData.Mid(start+1, end-start-1));
		
		CImapCommand::GetDelimitedPartsL(' ', parseData, flags);
		TInt flagsCount = flags.Count();
		for (TInt i = 0; i < flagsCount; ++i)
			{
			TPtrC8 flag = flags[i];

			if(flag.CompareF(KImapTxtFlagDeleted) == 0)
				{
				SetFlag(TMessageFlagInfo::EDeleted, ETrue);
				}
			else if (flag.CompareF(KImapTxtFlagSeen) == 0)
				{
				SetFlag(TMessageFlagInfo::ESeen, ETrue);
				}
			else if(flag.CompareF(KImapTxtFlagFlagged) == 0)
				{
				SetFlag(TMessageFlagInfo::EFlagged, ETrue);			
				}
			else if(flag.CompareF(KImapTxtFlagAnswered) == 0)
				{
				SetFlag(TMessageFlagInfo::EAnswered, ETrue);
				}
			else if(flag.CompareF(KImapTxtFlagDraft) == 0)
				{
				SetFlag(TMessageFlagInfo::EDraft, ETrue);
				}
			else if(flag.CompareF(KImapTxtFlagRecent) == 0)
				{
				SetFlag(TMessageFlagInfo::ERecent, ETrue);	
				}
			}

		// return the remainder.
		// if there is a space after the closing bracket, 
		// then remainder should start after the space.
		if (aFlagsData.Length() > end + 1)
			{
			if (aFlagsData[end+1] == ' ')
				{
				++end;
				}
			}		
		
		if (aFlagsData.Length() > end)
			{
			parseData.Set(aFlagsData.Mid(end + 1));
			}
		else
			{
			parseData.Set(aFlagsData.Right(0));
			}
			
		CleanupStack::PopAndDestroy(&flags);
		}

	return parseData;
	}

/**
Sets the Message UID by converting the given string value into an integer.
@param aMessageUid string containing the UID value.
@return KErrNone if the coversion was successful.  Otherwise a system-wide error code will be given.
*/
TInt TMessageFlagInfo::SetMessageUid(const TDesC8& aMessageUid)
	{
	TLex8 lex(aMessageUid);
	return lex.Val(iMessageUid);
	}

/**
@return The UID for this message
*/
EXPORT_C TUint TMessageFlagInfo::MessageUid() const
	{
	return iMessageUid;
	}
