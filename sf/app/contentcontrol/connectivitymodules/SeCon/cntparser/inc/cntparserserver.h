/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCntParserServer header
*
*/


#ifndef __CNTPARSERSERVER_H__
#define __CNTPARSERSERVER_H__

#include <e32base.h>
#include <e32svr.h>

class CCntParserCollector;
class CContactDatabase;

_LIT(KTxtServer,"CntParserServerExe.exe");

// needed for creating server thread.
const TUint KDefaultHeapSize=0x10000;

// reasons for server panic
enum TCntParserServerPanic
{
	EBadRequest,
	EBadDescriptor,
	EMainSchedulerError,
	ESvrCreateServer,
	ESvrStartServer,
	ECreateTrapCleanup,
	ENotImplementedYet,
};


//**********************************
//CCntParserServer
//**********************************
class CCntParserServer : public CServer2
{
	public:
	IMPORT_C static CCntParserServer* NewL();
	IMPORT_C virtual ~CCntParserServer();
	static TInt RunServer();
	static void RunServerL();
	void IncreaseSessionCountL();
	void DecreaseSessionCount();
	void Stop();
	
	TInt CreateIrMCL2PhoneBookL(const TDesC& path, TInt aMaxNumberOfContacts,TBool aForced);
	void PrepareCollectorL();
	TInt ContactsParsed();
	void CancelPhoneBookRequest();
	void CancelAllPhoneBookRequests();
	TInt TotalNumOfContacts();
	TInt ContactsSaved();
	void Disconnect();
	void CompletePhoneBookRequests(TInt aErr);
	void SetRequests(TInt aRequests);

	protected:
	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMsg) const;


	protected:
	CCntParserServer();


	private:
	TInt	iSessionCount;
	TInt	iRequests;	//How many PB.VCF requests are ongoing, if 0 collector is canceled

	CCntParserCollector* iCollector;
	CContactDatabase* iCurrentDatabase;
};

//**********************************
//global functions
//**********************************

// function to panic the server
GLREF_C void PanicServer(TCntParserServerPanic aPanic);


#endif