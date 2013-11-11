/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


//  CLASS HEADER
#include "t_glxresolutionutilitiy.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "glxresolutionutility.h"

// CONSTRUCTION
T_CGlxResolutionUtility* T_CGlxResolutionUtility::NewL()
    {
    T_CGlxResolutionUtility* self = T_CGlxResolutionUtility::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CGlxResolutionUtility* T_CGlxResolutionUtility::NewLC()
    {
    T_CGlxResolutionUtility* self = new( ELeave ) T_CGlxResolutionUtility();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CGlxResolutionUtility::~T_CGlxResolutionUtility()
    {
    }

// Default constructor
T_CGlxResolutionUtility::T_CGlxResolutionUtility()
    {
    }

// Second phase construct
void T_CGlxResolutionUtility::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS
// flag to know if gHandleResolutionChanged was called
TBool gHandleResolutionChanged = EFalse;

void T_CGlxResolutionUtility::HandleResolutionChanged ()
    {
    gHandleResolutionChanged = ETrue;
    TReal32 height = iResolutionUtility->PixelsToPoss( iSize.iHeight );
    EUNIT_ASSERT_EQUALS_DESC( height, iExpectedHeightPoss, "Height is correct");
    TReal32 width = iResolutionUtility->PixelsToPoss( iSize.iWidth );
    EUNIT_ASSERT_EQUALS_DESC( width, iExpectedWidthPoss, "Width is correct");
    EUNIT_ASSERT_EQUALS_DESC(
        iResolutionUtility->PossToPixels( height ), iSize.iHeight, "conversion back");
    EUNIT_ASSERT_EQUALS_DESC(
        iResolutionUtility->PossToPixels( width ), iSize.iWidth, "conversion back");
    }

void T_CGlxResolutionUtility::SetupL()
    {
    // reset state flag
    gHandleResolutionChanged = EFalse;
    
    iResolutionMgr      = CGlxResolutionManager::NewL();
    iResolutionUtility  = CGlxResolutionUtility::InstanceL();
    iResolutionUtility->AddObserverL(*this);
    }

void T_CGlxResolutionUtility::Teardown()
    {
    iResolutionUtility->RemoveObserver(*this);
    iResolutionUtility->Close();
    delete iResolutionMgr;
    }

void T_CGlxResolutionUtility::T_TestTvConnectedL()
    {
    iSize = TSize( 240,320 );
    // in this case we dont get the call to observer
    iResolutionMgr->SetScreenSize( iSize );
    // check that HandleResolutionChanged was not called
    EUNIT_ASSERT_DESC( !gHandleResolutionChanged, "resolution did not change" );
    gHandleResolutionChanged = EFalse; // reset flag

    iSize = TSize( 320,240 );
    iExpectedHeightPoss = 0.75;
    iExpectedWidthPoss = 1.0;
    iResolutionMgr->SetScreenSize( iSize );
    // check that HandleResolutionChanged was called
    EUNIT_ASSERT_DESC( gHandleResolutionChanged, "resolution did change" );
    gHandleResolutionChanged = EFalse; // reset flag

    iSize = TSize( 320,240 );
    iResolutionMgr->SetScreenSize( iSize );
    // check that HandleResolutionChanged was not called
    EUNIT_ASSERT_DESC( !gHandleResolutionChanged, "resolution did not change" );
    gHandleResolutionChanged = EFalse; // reset flag

    iSize = TSize( 640,640 );
    iExpectedHeightPoss = 1.0;
    iExpectedWidthPoss = 1.0;
    iResolutionMgr->SetScreenSize( iSize );
    // check that HandleResolutionChanged was called
    EUNIT_ASSERT_DESC( gHandleResolutionChanged, "resolution did change" );
    gHandleResolutionChanged = EFalse; // reset flag

    iSize = TSize( 100,500 );
    iExpectedHeightPoss = 1.0;
    iExpectedWidthPoss = 0.2;
    iResolutionMgr->SetScreenSize( iSize );
    // check that HandleResolutionChanged was called
    EUNIT_ASSERT_DESC( gHandleResolutionChanged, "resolution did change" );
    gHandleResolutionChanged = EFalse; // reset flag
    }
    

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CGlxResolutionUtility,
    "GLX Resolution Utility Test Suite.",
    "UNIT" )

EUNIT_TEST(
    "TV On",
    "tv connected",
    "TV On",
    "FUNCTIONALITY",
    SetupL, T_TestTvConnectedL, Teardown)
    

EUNIT_END_TEST_TABLE

//  END OF FILE
