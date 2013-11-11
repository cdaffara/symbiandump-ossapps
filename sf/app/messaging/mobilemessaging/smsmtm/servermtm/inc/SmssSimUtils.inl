// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SMSS_SIM_UTILS_INL_
#define SMSS_SIM_UTILS_INL_

#include "SMSSPAN.H"

inline const TSmsProgress& CSmsSimUtils::Progress()
	{
	return iProgress;
	}

inline TSmsSimUtilsData::TSmsSimUtilsData()
: iLastEnumerateFolder(KErrNotFound)
	{
	}

inline TBool TSmsSimUtilsData::IsPresentL(const CMsvStore& aStore) const
	{
	return aStore.IsPresentL(KSmsSimUtilsDataUid);
	}

#endif
