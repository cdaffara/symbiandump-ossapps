/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   test calenimporter api
*
*/



//  CLASS HEADER
#include "testcalenimporter.h"
#include "calenimptestconstants.h"

//  EXTERNAL INCLUDES
#include <CEUnitTestSuite.h>
#include <swinstapi.h>
#include <EUnitMacros.h>

// -----------------------------------------------------------------------------
// Install data 
// -----------------------------------------------------------------------------
//
/*TInt Install( const TDesC& aPackagePath )
    {
     TInt err = KErrNone;
    SwiUI::RSWInstSilentLauncher installer;
	
	err = installer.Connect();
    if(err != KErrNone)
    	{
    	EUNIT_PRINT(_L( "Connection to installer failed" ) );
    	}
    
    SwiUI::TInstallOptionsPckg options;
    options().iUpgrade = SwiUI::EPolicyAllowed;
    options().iPackageInfo = SwiUI::EPolicyAllowed;
    options().iOverwrite = SwiUI::EPolicyAllowed;
    options().iKillApp = SwiUI::EPolicyAllowed;

    err = installer.SilentInstall( aPackagePath, options );
 	if(err != KErrNone)
    	{
    	EUNIT_PRINT(_L( "Connection to installer failed" ) );
    	}
 
    installer.Close();
    
    return err; 
    }	*/

// -----------------------------------------------------------------------------
// Test suite factory function.
// -----------------------------------------------------------------------------
//
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
//  Install(KCalenImporterSisxFilePath);
    return CCalenImporterTest::NewL();
    }

//  END OF FILE






