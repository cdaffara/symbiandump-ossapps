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

#ifndef __TEST_CALINTERIMAPI_LAST_MODIFIED_H__
#define __TEST_CALINTERIMAPI_LAST_MODIFIED_H__

#include "TestCalInterimApiImportExport.h"
#include <calentryview.h>
#include <caliterator.h>

/** constants */
const TInt KTimeDelay = 3;

/** CTestCalInterimApiLastModified */
_LIT(KTestCalInterimApiLastModified,"TestCalInterimApiLastModified");
_LIT(KUidEntry,						"uid");
_LIT(KExpectedLastModified,			"lastmodified");

// Class CTestCalInterimApiLastModified - This class fetched all entries from database, and verifies the Last Modified Date/Time with
// the last modified date/time from ini file.
class CTestCalInterimApiLastModified : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiLastModified();
	~CTestCalInterimApiLastModified();
	TVerdict doTestStepL();

private:
	TPtrC iPtrEntryUid;
	TPtrC iPtrExpectedLastModified;
	};

#endif //__TEST_CALINTERIMAPI_LAST_MODIFIED_H__

