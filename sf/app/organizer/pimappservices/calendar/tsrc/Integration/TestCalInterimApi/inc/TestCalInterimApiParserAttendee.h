/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TEST_CALINTERIMAPI_PARSER_ATTENDEE_H__
#define __TEST_CALINTERIMAPI_PARSER_ATTENDEE_H__

#include "TestCalInterimApiParserBase.h"
#include <calentry.h>
#include <caluser.h>

class CCalEntry;
class CCalAttendee;

class CTestCalInterimApiParserUser : public CTestCalInterimApiParserBase
	{
public:
	static CTestCalInterimApiParserUser* 	NewLC(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallBack);
	static CTestCalInterimApiParserUser* 	NewL (const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallBack);
	~CTestCalInterimApiParserUser();
	
	//	CTestCalInterimParserBase implementation
	virtual TInt 		ParseL(const TDesC& aCommand, const TDesC& aData);
	
protected:
	CTestCalInterimApiParserUser(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback);
	void 				ConstructL(const TDesC& aData);
	void 				ProcessCommonNameL(const TDesC& aData);	
	void 				ProcessSentByL(const TDesC& aData);
	
private:
	void 				SetOrganizerL();

protected:
	CCalEntry&			iEntry;	
	HBufC*				iAddress;
	HBufC*				iSentByAddress;
	HBufC*				iCommonName;

private:
	TBool				iHasSentBy;
	TBool				iHasCommonName;
	};

 
class CTestCalInterimApiParserAttendee : public CTestCalInterimApiParserUser
	{
public:
	static CTestCalInterimApiParserAttendee*	NewLC(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback);
	static CTestCalInterimApiParserAttendee*	NewL(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback);
	~CTestCalInterimApiParserAttendee();

	//	CTestCalInterimParserBase implementation
	virtual TInt		ParseL(const TDesC& aCommand, const TDesC& aData);

private:
	CTestCalInterimApiParserAttendee(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback);
	
	void				ProcessRoleL(const TDesC& aData);
	void				ProcessAttendeeStatusL(const TDesC& aData);
	void				ProcessResponseRequestedL(const TDesC& aData);
	
	CCalAttendee* 		CreateAttendeeLC();
	void				GetAttendeeInfoL(CCalAttendee* aAttendee);

private:
	CCalAttendee::TCalRole		iCalRole;
	CCalAttendee::TCalStatus	iCalStatus;
	TBool						iRsvp;
	
	TBool						iHasRole;
	TBool						iHasAttendeeStatus;
	TBool						iHasRsvp;
	};

#endif
