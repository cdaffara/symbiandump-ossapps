/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DllMain file for contact adapter unit tests.
*
*/

//  EXTERNAL INCLUDES
#include <ceunittestsuite.h>
#include "cnsmldummydataprovider_test.h"
#include "cnsmldummydatastore_test.h"

/**
* UT_nsmldscontactsdataprovider.dll test suite factory function.
*/
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    CEUnitTestSuite* rootSuite = 
        CEUnitTestSuite::NewLC( _L( "Unit test for Contact adapter" ) );

    // Unit tests for dataprovider class (CNSmlDummyDataProvider)
    MEUnitTest* providerSuite = CNSmlDummyDataProvider_Test::NewL();
    CleanupStack::PushL( providerSuite );
    rootSuite->AddL( providerSuite );
    CleanupStack::Pop( providerSuite );

    // Unit tests for datastore class (CNSmlDummyDataStore)
    MEUnitTest* storeSuite = CNSmlDummyDataStore_Test::NewL();
    CleanupStack::PushL( storeSuite );
    rootSuite->AddL( storeSuite );
    CleanupStack::Pop( storeSuite );

    CleanupStack::Pop( rootSuite );

    return rootSuite;
    }
