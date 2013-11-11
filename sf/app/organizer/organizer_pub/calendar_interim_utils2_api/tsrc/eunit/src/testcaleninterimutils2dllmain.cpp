/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   test caleninterimutils2 api
*
*/



//  CLASS HEADER
#include "testcaleninterimutils2.h"

//  EXTERNAL INCLUDES
#include <CEUnitTestSuite.h>

// -----------------------------------------------------------------------------
// CreateTestSuiteL()
// Test suite factory function.
// -----------------------------------------------------------------------------
// 
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    return CCalenInterimUtils2Test::NewL();
    }

#ifndef __SECURE_API__
// -----------------------------------------------------------------------------
// E32Dll
// Standard Symbian DLL entry point function..
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif

//  END OF FILE






