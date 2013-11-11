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

#include <e32test.h>
#include "ConvertDates.h"

// Type definitions
#define UNUSED_VAR(a) a = a

void mainL();

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();

	TRAPD(error,mainL());
	UNUSED_VAR(error); //Used to supress build warnings

	delete cleanupStack;
	__UHEAP_MARKEND;
	return KErrNone;
    }

void mainL()
    {
	CConvertDates* ConvDates = CConvertDates::NewL();
	CleanupStack::PushL(ConvDates);

	ConvDates->ConvertL();
	CleanupStack::PopAndDestroy();  //delete ConvDates;
	}
