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



#ifndef TEST_CALINTERIMAPI_DELETE_ATTACHMENT_H
#define TEST_CALINTERIMAPI_DELETE_ATTACHMENT_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calentry.h>
#include <calattachment.h>

/**
This test step fetches the entry from the calendar file and deletes the attachment
from the parent/ child entry 
*/

class CCalEntryView;
class CTestCalInterimApiDeleteAttachment : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiDeleteAttachment();
	~CTestCalInterimApiDeleteAttachment();

	virtual TVerdict	doTestStepL();

private:
	void			DeleteAttachmentAndStoreEntryL(CCalEntry* aCalEntry);
	void			OOMTestsForDeleteAttachmentL(CCalEntry* aCalEntry, CCalAttachment* aCalAttachment);
		
private:
	CCalEntryView*	iCalEntryView;
	TBool			iAllocTest;
	};

_LIT(KTestCalInterimApiDeleteAttachment,"TestCalInterimApiDeleteAttachment");

#endif
