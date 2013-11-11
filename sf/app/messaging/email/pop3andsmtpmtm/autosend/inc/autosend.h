// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AUTOSEND_H__
#define __AUTOSEND_H__

#if !defined(_MSG_NO_LOGGING) || defined(_DEBUG)
#define AUTOSEND_LOGGING
#endif
const TInt KDefaultPriority = CActive::EPriorityStandard;

_LIT(KMsvWinsAutoSendThreadName, "AutoSend");

class CClientMtmRegistry;

TInt Execute(const TMsvId aDestinationId);
void DoExecuteL(const TMsvId aDestinationId);

class TDummyMsvSessionObserver : public MMsvSessionObserver
	{
public:
	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny *aArg1, TAny *aArg2, TAny *aArg3);
	};

/**
@internalComponent
@released
*/
class CAutoSendProgressTimer : public CTimer
	{
public:
	static CAutoSendProgressTimer* NewL(CMsvOperation& aOperation, const TTimeIntervalMicroSeconds32& aPollInterval);
	void Start();
	~CAutoSendProgressTimer();
	
private:
	CAutoSendProgressTimer(CMsvOperation& aOperation, const TTimeIntervalMicroSeconds32& aPollInterval);
	void RunL();
	CMsvOperation& iOperation;
	TTimeIntervalMicroSeconds32 iPollInterval;
	};

/**
@internalComponent
@released
*/
class CImAutoSend : public MMsvSessionObserver, CActive
	{
public:	
	static CImAutoSend* StartLC(const TMsvId aDestinationId);
	~CImAutoSend();

	void HandleSessionEventL(MMsvSessionObserver::TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) {};
	TBool IsSendActive() {return IsActive();}

private:

	CImAutoSend(const TMsvId aDestinationId);
	void ConstructL();

	void DoCancel();
	void RunL();

	void CallMtmL();

#ifdef AUTOSEND_LOGGING
	void FLog(const TDesC& buf);
	void FLog(TRefByValue<const TDesC> aFormat, ...);
#endif

private:
	CMsvSession*			iSession;
	CMsvOperation*			iOperation;
	CAutoSendProgressTimer*	iTimer;
	TMsvId					iDestinationId;
	CActiveSchedulerWait	iActiveSchedulerWait;
	};

#endif
