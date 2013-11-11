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
* Description:   test agnversit2 api
*
*/



//  CLASS HEADER
#include "testversit2.h"
#include "testagnversit2.h"

//  EXTERNAL INCLUDES
#include <CEUnitTestSuite.h>
#include <swinstapi.h>
#include <EUnitMacros.h>

//_LIT(KAgnversit2SisxFilePath,"E://testing//data//unittest//agnversit2//input//agnversit2.sisx");

// -----------------------------------------------------------------------------
// SetDateTimeLocale
// -----------------------------------------------------------------------------
//
void SetDateTimeLocale()    
    {
    
    TLocale locale;
    locale.SetTimeFormat( ETime24 );
    locale.SetStartOfWeek( EMonday );
    locale.SetDeviceTime( TLocale::EDeviceUserTime );

    _LIT( KMySeparators, ".:" );
    const TChar sep01( KMySeparators()[0] );
    const TChar sep02( KMySeparators()[1] );
    const TChar empty( NULL );

    locale.SetDateSeparator( empty, 0 );
    locale.SetDateSeparator( sep01, 1 );
    locale.SetDateSeparator( sep01, 2 );
    locale.SetDateSeparator( empty, 3 );

    locale.SetTimeSeparator( empty, 0 );
    locale.SetTimeSeparator( sep02, 1 );
    locale.SetTimeSeparator( sep01, 2 );
    locale.SetTimeSeparator( empty, 3 );

    locale.Set();

    TInt year( 2007 );
    TInt day( 10 );
    TInt hour( 21 );
    TInt min( 45 );
    TInt sec( 30 );
    const TDateTime dt( year, TMonth( EMay ), day-1, hour, min, sec, 0 );
    User::SetUTCTime( TTime( dt ) );    // UTC is better, in case this module
    // may run anywhere in the world.
    }

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
  //  SetDateTimeLocale();
     
    CEUnitTestSuite* rootSuite = CEUnitTestSuite::NewLC(_L("Wizard generated"));
/*    
    rootSuite->AddL( T_AgnVersit2::NewLC() );
    CleanupStack::Pop();
*/    
    rootSuite->AddL( CCVersit2Test::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( CCAgnVersit2Test::NewLC() );
    CleanupStack::Pop();
    
    CleanupStack::Pop(); // rootSuite
    
//	Install(KAgnversit2SisxFilePath);
	
    return rootSuite;
    }

//  END OF FILE






