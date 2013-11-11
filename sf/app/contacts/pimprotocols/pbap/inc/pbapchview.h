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

#ifndef PBAPCHVIEW_H
#define PBAPCHVIEW_H

#include <e32base.h>
#include <logwrap.h>

#include "pbapfoldernodech.h"
#include "pbapchviewobserver.h"

#include "pbaplogeng.h"
#include "pbaplogengviewobserver.h"

/**
 Call History View Class
 */
class CPbapChView : public CActive, public MPbapLogEngViewObserver
	{
public:
	static CPbapChView* NewL(CPbapLogWrapper& aLogClient,
							 CFolderNodeCallHistory::THistoryType aHistoryType,
							 MPbapChViewObserver& aObserver,
							 TLogFlags aFlags);
	~CPbapChView();

	CPbapLogViewEvent* LogView();

private:
	CPbapChView(CPbapLogWrapper& aLogClient, MPbapChViewObserver& aObserver);
	void ConstructL(CFolderNodeCallHistory::THistoryType aHistoryType, TLogFlags aFlags);

	void RunL();
	void DoCancel();

	CLogFilter* CreateDirectionFilterLC(CFolderNodeCallHistory::THistoryType aHistoryType, TLogFlags aFlags);

	// from MPbapLogEngViewObserver
	virtual void PbapLogEngViewChangeEventAddedL(TLogId, TInt, TInt, TInt);
	virtual void PbapLogEngViewChangeEventChangedL(TLogId, TInt, TInt, TInt);
	virtual void PbapLogEngViewChangeEventDeletedL(TLogId, TInt, TInt, TInt);
	virtual void PbapLogEngViewChangeEventL(TUid, TInt, TInt, TInt);

private:
	CPbapLogWrapper& iLogClient;
	CLogFilterList* iLogFilterList;
	CPbapLogViewEvent* iLogView;
	TBool iRefreshView;
	
	MPbapChViewObserver& iObserver;
	};

#endif // PBAPCHVIEW_H