// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_SMSDELIVER_H__
#define __T_SMSDELIVER_H__

#include <e32base.h>
#include <msvapi.h>
#include <es_sock.h>
#include <logwrap.h>

class CSmsTestUtils;
class CMsvEntry;
class CSmsClientMtm;
class CMsvSession;
class CSmsSettings;
class CSmsHeader;
class CTestUtilsWatcherStarter;
class CTestDeliverBase;

class CTestEngine : public CBase
					
	{
public:		// methods

	static CTestEngine* NewL();
	virtual ~CTestEngine();
	void DoTestsL();
	
private:	// methods

	CTestEngine();
	void ConstructL();
	
	void RunTestCaseL(CTestDeliverBase& aTestCase);
	
	TBool WatchersAlreadyRunningL();
	void StartWatcherL();

private:	// attributes

	CSmsTestUtils*	iTestUtils;
	CTestUtilsWatcherStarter* iWatchers;
	RSocketServ iSocketServ;
	RSocket iSocket;
	};
	
class CTestDeliverBase : public CActive,
						 public MMsvEntryObserver
	{
public:

	virtual ~CTestDeliverBase();
	void Start();
	
private:	// methods from CActive

	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
private:	// methods from MMsvEntryObserver

	virtual void HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

protected:

	CTestDeliverBase(CSmsTestUtils& aTestUtils);
	void ConstructL();

	void CompleteSelf();
	void ClearFoldersL();
	void TestComplete();
	void CreateMessageL();
	void SendMessageL();
	
	CMsvSession& Session();
	CMsvEntry& MsvEntry();
	CSmsClientMtm& Client();
	CSmsSettings& ServiceSettings();
	
protected:

	CSmsTestUtils&	iTestUtils;
	CMsvEntry*		iEntryForObserver;
	CMsvOperation*	iOperation;
	TMsvId			iMessageId;

private:

	virtual void ChangeServiceSettingsL();
	virtual void CheckCreatedMessageL();
	virtual TMsvId GetObserverEntry();

	virtual void SetRecipientsL(CSmsHeader& aHeader) =0;
	virtual void CheckSentMessageL() =0;
	virtual void CheckDeliveredMessageL() =0;
	virtual const TDesC& TestName() =0;

private:

	enum TDeliverTestState
		{
		EClearFolders,
		EChangeServiceSettings,
		ECreateMessage,
		ECheckCreatedMessage,
		ESendMessage,
		ECheckSentMessage,
		EPendingDeliveryReport,
		EDone
		};			
	
private:
	
	TDeliverTestState	iState;
	};

class CTestDeliverOneRecipientOk : public CTestDeliverBase
	{
public:

	static CTestDeliverOneRecipientOk* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverOneRecipientOk();
	
private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();

private:

	CTestDeliverOneRecipientOk(CSmsTestUtils& aTestUtils);
	};
	
class CTestDeliverOneRecipientFail : public CTestDeliverBase
	{
public:

	static CTestDeliverOneRecipientFail* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverOneRecipientFail();
	
private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();
	
private:

	CTestDeliverOneRecipientFail(CSmsTestUtils& aTestUtils);
	};
	
class CTestDeliverMultiRecipientsOk_1 : public CTestDeliverBase
	{
public:

	static CTestDeliverMultiRecipientsOk_1* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverMultiRecipientsOk_1();
	
private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();

private:

	CTestDeliverMultiRecipientsOk_1(CSmsTestUtils& aTestUtils);

private:
	
	TInt	iCheckCount;	
	};
	
class CTestDeliverMultiRecipientsOk_2 : public CTestDeliverBase
	{
public:

	static CTestDeliverMultiRecipientsOk_2* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverMultiRecipientsOk_2();
	
private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();

private:

	CTestDeliverMultiRecipientsOk_2(CSmsTestUtils& aTestUtils);
	};
	
class CTestDeliverMultiRecipientsOk_3 : public CTestDeliverBase
	{
public:

	static CTestDeliverMultiRecipientsOk_3* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverMultiRecipientsOk_3();
	
private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();

private:

	CTestDeliverMultiRecipientsOk_3(CSmsTestUtils& aTestUtils);

private:
	
	TInt	iCheckCount;	
	};
	
class CTestDeliverNoMatching : public  CTestDeliverBase
	{
public:

	static CTestDeliverNoMatching* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverNoMatching();
	
private:	// methods from MMsvEntryObserver

	virtual void HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	
private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual void ChangeServiceSettingsL();
	virtual TMsvId GetObserverEntry();
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();

private:

	CTestDeliverNoMatching(CSmsTestUtils& aTestUtils);

private:

	TLogId	iLogId;
	};

class CTestDeliverNoStatusReports : public  CTestDeliverBase
	{
public:

	static CTestDeliverNoStatusReports* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverNoStatusReports();

private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual void ChangeServiceSettingsL();
	virtual TMsvId GetObserverEntry();
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();
	
private:

	CTestDeliverNoStatusReports(CSmsTestUtils& aTestUtils);
	};

class CTestDeliverNotSent : public  CTestDeliverBase
	{
public:

	static CTestDeliverNotSent* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverNotSent();

private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual TMsvId GetObserverEntry();
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();

private:

	CTestDeliverNotSent(CSmsTestUtils& aTestUtils);
	};
	
class CTestDeliverMultiRecipientsFail_1 : public CTestDeliverBase
	{
public:

	static CTestDeliverMultiRecipientsFail_1* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverMultiRecipientsFail_1();
	
private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();

private:

	CTestDeliverMultiRecipientsFail_1(CSmsTestUtils& aTestUtils);

private:
	
	TInt	iCheckCount;	
	};
	
class CTestDeliverMultiRecipientsMixed_1 : public CTestDeliverBase
	{
public:

	static CTestDeliverMultiRecipientsMixed_1* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverMultiRecipientsMixed_1();
	
private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();

private:

	CTestDeliverMultiRecipientsMixed_1(CSmsTestUtils& aTestUtils);

private:
	
	TInt	iCheckCount;	
	};

class CTestDeliverMultiRecipientsMixed_2 : public CTestDeliverBase
	{
public:

	static CTestDeliverMultiRecipientsMixed_2* NewL(CSmsTestUtils& aTestUtils);
	virtual ~CTestDeliverMultiRecipientsMixed_2();

private:	// methods from CTestDeliverBase

	virtual void SetRecipientsL(CSmsHeader& aHeader);
	virtual void CheckSentMessageL();
	virtual void CheckDeliveredMessageL();
	virtual const TDesC& TestName();

private:

	CTestDeliverMultiRecipientsMixed_2(CSmsTestUtils& aTestUtils);

private:

	TInt	iCheckCount;
	};

#endif	// __T_SMSDELIVER_H__
