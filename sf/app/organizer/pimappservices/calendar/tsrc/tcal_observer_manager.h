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

#ifndef __TCAL_OBSERVER_MANAGER_H__
#define __TCAL_OBSERVER_MANAGER_H__

#include "tcal_observer.h"

class CTestAppManager : public CTestApp, public MCalChangeCallBack2
	{
public:
	static CTestAppManager* NewLC();
	~CTestAppManager();
	
	void RunTestL();
private:
	void ConstructL();
	
private:
	static TInt StopWaitingForNotification(TAny*);
	void DoStopWaitingForNotification();
	
	void NextTestL();
	void TestFetchEntryByGuidL(TCalLocalUid localId);
	
private: // from MCalChangeCallBack2
	void CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems);

private:
	RThread				iThread;
	TBool				iNotified;
	TInt				iNumChangesFound;
	TBool				iExpectingNotification;
	};
	
#endif // __TCAL_OBSERVER_MANAGER_H__
