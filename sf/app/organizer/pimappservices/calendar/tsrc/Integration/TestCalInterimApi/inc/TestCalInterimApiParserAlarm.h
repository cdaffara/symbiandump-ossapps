/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TEST_CALINTERIMAPI_PARSER_ALARM_H__
#define __TEST_CALINTERIMAPI_PARSER_ALARM_H__

#include "TestCalInterimApiParserBase.h"

#include <calalarm.h>

/**
 * Parser that parses alarm related commands from the data file.
 */
class CTestCalInterimApiParserAlarm : public CTestCalInterimApiParserBase
	{
public:

	static CTestCalInterimApiParserAlarm*	NewLC(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback);
	static CTestCalInterimApiParserAlarm*	NewL (const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback);
	~CTestCalInterimApiParserAlarm();
	
	virtual TInt		ParseL(const TDesC& aCommand, const TDesC& aData);

private:

	void 				ConstructL(const TDesC& aData);
	CTestCalInterimApiParserAlarm(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback);
	
	void				ProcessAlarmSoundNameL(const TDesC& aData);
	void				ProcessDisposition(const TDesC& aData);
	void				ProcessMimeTypeL(const TDesC& aData);
	void				ProcessContentL(const TDesC& aData);
	
private:
	CCalEntry&						iEntry;
	
	CCalAlarm*						iCalAlarm;
	HBufC8*							iAlarmContent;
	HBufC8*							iMimeType;
	CCalContent::TDisposition		iDisposition;
			
	TBool							iHasAlarmSoundName;
	TBool							iHasDisposition;
	TBool							iHasMimeType;
	TBool							iHasContent;
	};

#endif
