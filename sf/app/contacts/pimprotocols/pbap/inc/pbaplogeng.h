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

#ifndef PBAPLOGENG_H
#define PBAPLOGENG_H

#include <logwrap.h>
#include <logview.h>

#include <logviewchangeobserver.h>
#include <logclientchangeobserver.h>

#include "pbaplogengviewobserver.h"

/** 
 The PBAP Log Client wrapper.

 This class allows localisation of call logging for benefit of different UI variants.
 */
NONSHARABLE_CLASS (CPbapLogWrapper) : public CBase, public MLogClientChangeObserver
	{
	// to allow access to LogClient() and Global LogEng events.
	friend class CPbapLogViewEvent;
	
public:
	IMPORT_C static CPbapLogWrapper* NewL(RFs& aFs);
	IMPORT_C ~CPbapLogWrapper();

	// From CLogWrapper
	IMPORT_C TBool ClientAvailable() const;
	
	// From CLogClient
	IMPORT_C TInt GetString(TDes &aString, TInt aId) const;	
	
protected:	
	// Only for creation of CLogViewEvent
	CLogClient& LogClient();
	
	// Register PBAP observer for MLogClientChangeObserver events
	TInt AddPbapObserver(MPbapLogEngViewObserver& aObserver);
	void RemovePbapObserver(MPbapLogEngViewObserver& aObserver);
	
private:
	CPbapLogWrapper();
	void ConstructL(RFs& aFs);

	// from MLogClientChangeObserver
	virtual void HandleLogClientChangeEventL(TUid aChangeType, TInt aParam1, TInt aParam2, TInt aParam3);

private:
	CLogWrapper*	iLogWrapper;
	CLogClient*		iLogClient; //not owned
	
	RPointerArray<MPbapLogEngViewObserver> iPbapObservers;
	};


/** 
 The PBAP Log Event View wrapper.

 This class allows localisation of call logging for benefit of different UI variants.
 */
NONSHARABLE_CLASS (CPbapLogViewEvent) : public CBase, public MLogViewChangeObserver
	{
public:
	IMPORT_C static CPbapLogViewEvent* NewL(CPbapLogWrapper& aLogWrap);
	IMPORT_C static CPbapLogViewEvent* NewL(CPbapLogWrapper& aLogWrap, MPbapLogEngViewObserver& aObserver);
	IMPORT_C ~CPbapLogViewEvent();
	
public:
	// from CLogViewEvent
	IMPORT_C TBool SetFilterL(const CLogFilterList& aFilterList, TRequestStatus& aStatus);
	IMPORT_C TBool SetFilterL(const CLogFilter& aFilter, TRequestStatus& aStatus);
	
	IMPORT_C const CLogEvent& Event() const;
	IMPORT_C TBool FirstL(TRequestStatus& aStatus);
	IMPORT_C TBool NextL(TRequestStatus& aStatus);
	IMPORT_C TInt CountL();

	IMPORT_C void Cancel();

private:
	CPbapLogViewEvent(CPbapLogWrapper& aClient);
	void ConstructL(MPbapLogEngViewObserver* aObserver = NULL);

	// from MLogViewChangeObserver
	virtual void HandleLogViewChangeEventAddedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
	virtual void HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
	virtual void HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);

private:
	CPbapLogWrapper& 	iWrapper;
	CLogViewEvent*		iViewEvent;		

	MPbapLogEngViewObserver* iPbapObserver;
	};

#endif // PBAPLOGENG_H
