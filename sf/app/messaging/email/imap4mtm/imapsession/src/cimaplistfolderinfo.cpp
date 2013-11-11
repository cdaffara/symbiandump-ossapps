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


#include "cimaplistfolderinfo.h"


CImapListFolderInfo* CImapListFolderInfo::NewL()
// static method
	{
	CImapListFolderInfo* self = new(ELeave)CImapListFolderInfo;
	// no ConstructL()
	return self;
	}
	
// Empty private constuctor forces the use of NewL()
CImapListFolderInfo::CImapListFolderInfo()
	{}
	
CImapListFolderInfo::~CImapListFolderInfo()
	{
	delete iFolderName;
	}

/**
Takes ownership of the supplied folder name
@param aFolderName the folder name that will be owned by this object.
*/
void CImapListFolderInfo::SetFolderNameL(HBufC* aFolderName)
	{
	delete iFolderName;
	iFolderName = aFolderName;
	}
	
EXPORT_C TPtrC CImapListFolderInfo::FolderName() const
	{
	return iFolderName ? *iFolderName : KNullDesC();
	}

/**
Set the flag identified by aFlagId to the value of aValue.
@param aFlagId Identifies which flag to set.
@param aValue The value to apply to the flag.
*/
void CImapListFolderInfo::SetFlag(TIMapListFlags aFlagId, TBool aValue)
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
EXPORT_C TBool CImapListFolderInfo::QueryFlag(TIMapListFlags aFlagId)
	{
	return iFlags & aFlagId;
	}

/**
Perfoms comparison between two CImapListFolderInfo objects by ordering their folder names.
@return whether aFirst is alphabetically before or after aSecond
@param aFirst the first CImapListFolderInfo object to compare
@param aSecond the second CImapListFolderInfo object to compare
*/
TInt CImapListFolderInfo::CompareByFolderName(const CImapListFolderInfo& aFirst, const CImapListFolderInfo& aSecond)
// static method
	{
	// This is a case-senstive, as servers are allowed to treat non-inbox mailboxes as case-sensitive (although they may choose not to).
	return aFirst.FolderName().Compare(aSecond.FolderName());
	}
