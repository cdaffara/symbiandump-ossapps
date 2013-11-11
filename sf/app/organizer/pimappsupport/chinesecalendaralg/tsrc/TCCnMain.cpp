// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <ecom/ecom.h>
#include "ConvertDates.h"

_LIT(KTestName,"Basic Calcon tests.");
LOCAL_D RTest test(KTestName);

void mainL();

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();

	TRAPD(error,mainL());
	test(KErrNone == error);
	REComSession::FinalClose(); 

	delete cleanupStack;
	__UHEAP_MARKEND;
	return KErrNone;
    }

/**
@SYMTestCaseID PIM-TCCNMAIN-0001
 */
	
void mainL()
    {
	CConvertDates* convDates = CConvertDates::NewL();
	CleanupStack::PushL(convDates);

	convDates->iTest(convDates->IntegrityTestL());

 	TInt err=KErrNone;
 	TRAP(err,convDates->ConvertL(1));
 	convDates->iTest(err == KErrNone);
	TRAP(err,convDates->ConvertL(2));
 	convDates->iTest(err == KErrNone);

	convDates->iTest(convDates->CompareL());
	convDates->iTest.Start(_L("@SYMTestCaseID PIM-TCCNMAIN-0001"));
	convDates->iTest.End();
	convDates->iTest.Close();
	
	CleanupStack::PopAndDestroy(convDates);
	}
