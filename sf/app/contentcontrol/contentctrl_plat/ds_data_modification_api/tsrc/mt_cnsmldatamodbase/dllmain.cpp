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
* Description:  DllMain file for data mod unit tests.
*
*/

// INCLUDE FILES
#include <CEUnitTestSuite.h>
#include "mt_cnsmldatamodbase.h"

/**
 * mt_cnsmldatamodbase.dll test suite factory function.
 */

EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    CEUnitTestSuite* rootSuite = CEUnitTestSuite::NewLC(_L("DataModBase Module Tests"));

     // Note that NewLC leaves the object in the cleanupstack.
     rootSuite->AddL( MT_CNSmlDataModBase::NewLC() );
     CleanupStack::Pop( ); // MT_CNSmlDataModBase
     
     CleanupStack::Pop( rootSuite );
     return rootSuite;
    }

// End of file
