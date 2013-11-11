/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ut_cglxcollectionplugintags.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES


// CONSTRUCTION
ut_cglxcollectionplugintags* ut_cglxcollectionplugintags::NewL()
    {
    ut_cglxcollectionplugintags* self = ut_cglxcollectionplugintags::NewLC();
    CleanupStack::Pop();

    return self;
    }

ut_cglxcollectionplugintags* ut_cglxcollectionplugintags::NewLC()
    {
    ut_cglxcollectionplugintags* self = new( ELeave ) ut_cglxcollectionplugintags();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
ut_cglxcollectionplugintags::~ut_cglxcollectionplugintags()
    {
    }

// Default constructor
ut_cglxcollectionplugintags::ut_cglxcollectionplugintags()
    {
    }

// Second phase construct
void ut_cglxcollectionplugintags::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void ut_cglxcollectionplugintags::SetupL(  )
    {
    iCollectionPluginTags = CGlxCollectionPluginTags :: NewL(this);
    }
    

void ut_cglxcollectionplugintags::Teardown(  )
    {
    delete iCollectionPluginTags;
    }
    

void ut_cglxcollectionplugintags::ut_cglxcollectionplugintagsL(  )
    {
    EUNIT_ASSERT_DESC( iCollectionPluginTags, "Failed to Create the collection Plugin");
    }
    

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    ut_cglxcollectionplugintags,
    "Testing Tag Collection Plugin.",
    "MODULE" )

EUNIT_TEST(
    "CGlxCollectionPluginTags",
    "CGlxCollectionPluginTags-Newl",
    "NewL",
    "Unit",
    SetupL, ut_cglxcollectionplugintagsL, Teardown)
    

EUNIT_END_TEST_TABLE

//  END OF FILE
