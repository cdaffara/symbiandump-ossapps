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

#ifndef __TCAL_OBSERVER_MODIFIER_H__
#define __TCAL_OBSERVER_MODIFIER_H__

#include "tcal_observer.h"

class CTestAppModifier : public CTestApp
	{
public:
	static CTestAppModifier* NewL();
	~CTestAppModifier();

public: 
	void RunTestL();

private:
	void StoreEntryLD(CCalEntry* aEntry, TBool aInRange = ETrue);
	void StoreEntryLC(CCalEntry* aEntry, TBool aInRange = ETrue);
	CCalEntry* CreateEntryLC(CCalEntry::TType aType, TTime aTime);
	void ClearEntryL();
	void UpdateEntriesL(TBool aInRange = ETrue);
	
	void SyncPoint(TBool aReset = ETrue);
	
	void NextTestL();
	
private:
	void ConstructL();
	
private:
	CDesC8Array*		iUidArray;
	TInt				iGuidNum;
	TInt				iNumEntriesToDelete;
	};
	
#endif // __TCAL_OBSERVER_MODIFIER_H__
