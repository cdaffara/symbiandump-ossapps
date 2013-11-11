// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALINSTANCEIMPL_H__
#define __CALINSTANCEIMPL_H__

#include "caltime.h"
#include "calinstance.h"

class CCalEntry;

/** Implementation class for a CCalInstance.

@internalComponent
*/
NONSHARABLE_CLASS(CCalInstanceImpl) : public CBase
	{
public:
	static CCalInstanceImpl* NewL(CCalEntry* aEntry, const TCalTime& iTime);
	~CCalInstanceImpl();
	
	CCalEntry& Entry() const;
	TCalTime Time() const;
	TCalTime StartTimeL() const;
	TCalTime EndTimeL() const;
	TCalInstanceId InstanceIdL() const;
	
private:
	CCalInstanceImpl(CCalEntry* aEntry, const TCalTime& aTime);
	
private:
	CCalEntry& iCalEntry;
	TCalTime iCalTime;
	};

#endif // __CALINSTANCEIMPL_H__
