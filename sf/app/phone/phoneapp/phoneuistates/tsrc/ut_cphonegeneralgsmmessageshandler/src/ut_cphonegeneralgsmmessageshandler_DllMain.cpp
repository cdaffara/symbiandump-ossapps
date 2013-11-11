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
* Description: 
*
*/
//  EXTERNAL INCLUDES
#include <CEUnitTestSuite.h>
#include "ut_cphonegeneralgsmmessageshandler.h"

/**
* ut_cphonegeneralgsmmessageshandler
*/
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    CEUnitTestSuite* rootSuite = CEUnitTestSuite::NewLC(_L("ut_cphonegeneralgsmmessageshandler"));

    // Note that NewLC leaves the object in the cleanupstack.
    rootSuite->AddL( ut_cphonegeneralgsmmessageshandler::NewLC() );
    CleanupStack::Pop();

    CleanupStack::Pop( rootSuite );

    return rootSuite;
    }
