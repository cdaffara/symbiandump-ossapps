// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CIRCANCELOBSERVER_H__
#define __CIRCANCELOBSERVER_H__

#include <e32std.h>
#include <msvapi.h>

/**
This class is a CMsvOperation used to clean up entries for operations that have been queued.
The IR server mtm only takes ownership for entries once the operation has started so this object is provided 
as a wrapper to to the send operation to own entries for queued IR operations. The entries are only cleaned
up if the operation has not started and only when the operation is explicitly cancelled by the user.
@internalComponent
*/
class CIRCancelObserver : public CMsvOperation
	{
public:
	static CIRCancelObserver* NewL(TRequestStatus& aObserverRequestStatus, CMsvSession& aSession, TMsvId aEntry);
	virtual ~CIRCancelObserver();

	TRequestStatus& Status();
	void SetOperation(CMsvOperation* aMsvOperation);
	
	// from CMsvOperation
	virtual const TDesC8& ProgressL();
	virtual const TDesC8& FinalProgress();

private:
	CIRCancelObserver(TRequestStatus& aObserverRequestStatus, CMsvSession& aSession, TMsvId aEntry);
	void CleanupEntryL() const;
	void CleanupEntry() const;

	// from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	/** The entry id for the entry of the IR operation.*/
	TMsvId iEntryId;
	
	/** The send operation that this class owns and encapsulates.*/
	CMsvOperation* iMsvOperation;

	/** Flag to indicate if the operation has started.*/
	TBool iOperationStarted;
	};

#endif // __CIRCANCELOBSERVER_H__
