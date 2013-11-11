/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* 
*/

#ifndef MEMSPYASYNCTRACKER_H_
#define MEMSPYASYNCTRACKER_H_

#include <e32base.h>
#include <aknwaitdialog.h> 
#include <memspysession.h>


class CMemSpyWaitNote : public CBase, public MProgressDialogCallback
	{
public:
	
	~CMemSpyWaitNote();
	
	void StartWaitNoteL();
	
	void DialogDismissedL (TInt aButtonId);
	 
private:
	CAknWaitDialog* iWaitDialog;
	};


class CMemSpyAsyncTracker : public CActive
{	
public:

    ~CMemSpyAsyncTracker();
    
	CMemSpyAsyncTracker(RMemSpySession& aSession, void (RMemSpySession::*function)(TRequestStatus&));	
	
	void StartL();	
	
	virtual void RunL();
	
	virtual void DoCancel();
	
	virtual TInt RunError(TInt aError);
		
private:
	
	CMemSpyWaitNote* iWaitNote;
	
	void (RMemSpySession::*iFunction)(TRequestStatus&);
	
	RMemSpySession& iSession;
};







class CMemSpySwmtDumpTracker : public CMemSpyAsyncTracker
{
public:
	CMemSpySwmtDumpTracker(RMemSpySession& aSession) : 
		CMemSpyAsyncTracker(aSession, &RMemSpySession::ForceSwmtUpdate)
	{}
};




class CMemSpySwmtStartTimerTracker : public CMemSpyAsyncTracker
{
public:
	CMemSpySwmtStartTimerTracker(RMemSpySession& aSession) : 
		CMemSpyAsyncTracker( aSession, &RMemSpySession::StartSwmtTimer )
	{}
};


#endif /* MEMSPYASYNCTRACKER_H_ */
