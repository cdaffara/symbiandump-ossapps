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



#ifndef TEST_CALINTERIMAPI_CHECK_ATTACHMENT_H
#define TEST_CALINTERIMAPI_CHECK_ATTACHMENT_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calattachment.h>

/**
 Teststep that fetches the entry from the calendar file, gets the attachment based on the
 requirements from the user and checks the properties of the attachment and compares it with 
 the expected values.
 */
class CTestCalInterimApiCheckAttachment: public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiCheckAttachment();
	~CTestCalInterimApiCheckAttachment();
	
	TVerdict 				doTestStepL();

private:
	void 					GetAttachmentAndCheckPropertiesL(CCalEntry* aEntry);
	void		 			GetAttachmentDataFromIni(const TInt& aIndex);
	void 					CheckAttachmentPropertiesL(CCalAttachment* aCalAttachment);
	void					CheckPropertiesOfAttachmentL(CCalAttachment* aCalAttachment);
	void					CheckFileAttachmentPropertiesL(CCalAttachmentFile* aFileAttachment);

private:
	CCalEntryView*			iCalEntryView;
	TAttachment				iAttachProperties;
	TBool					iHasSize;
	TBool					iHasAttribute;
	TBool					iHasAttachType;		
	};

_LIT(KTestCalInterimApiCheckAttachment, "TestCalInterimApiCheckAttachment");

#endif 
