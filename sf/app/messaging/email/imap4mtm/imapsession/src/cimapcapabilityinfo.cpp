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


#include "cimapcapabilityinfo.h"

/**
Public construction
*/
EXPORT_C CImapCapabilityInfo* CImapCapabilityInfo::NewL()
// static method
	{
	return new(ELeave)CImapCapabilityInfo;
	}
	
/**
Private constructor
*/
CImapCapabilityInfo::CImapCapabilityInfo()
	{}
	
/**
Destructor
*/
CImapCapabilityInfo::~CImapCapabilityInfo()
	{}

/**
Set the flag identified by aFlagId to the value of aValue.
@param aFlagId Identifies which flag to set.
@param aValue The value to apply to the flag.
*/
EXPORT_C void CImapCapabilityInfo::SetFlag(TImapCapabilityFlags aFlagId, TBool aValue)
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
EXPORT_C TBool CImapCapabilityInfo::QueryFlag(const TImapCapabilityFlags aFlagId) const
	{
	return iFlags & aFlagId;
	}

/**
This method is called by the system when it recieves updated capability information,
prior to updating this object with new data.
The method unsets all flags and removes the AuthType.
*/
void CImapCapabilityInfo::Reset()
	{
	iFlags = 0;
	}
