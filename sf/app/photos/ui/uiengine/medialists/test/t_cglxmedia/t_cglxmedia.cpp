/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CGlxMedia unit tests
*
*/



//  CLASS HEADER
#include "t_cglxmedia.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <mpxattribute.h>
#include <mpxmediageneraldefs.h>

#include <glxmediageneraldefs.h>

//  INTERNAL INCLUDES
#include "glxmedia.h"

const TInt KMediaId = 1;

_LIT(KMediaTitle, "Captured");
_LIT(KMediaSubTitle, "0 items");

// CONSTRUCTION
T_CGlxMedia* T_CGlxMedia::NewL()
    {
    T_CGlxMedia* self = T_CGlxMedia::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CGlxMedia* T_CGlxMedia::NewLC()
    {
    T_CGlxMedia* self = new( ELeave ) T_CGlxMedia();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CGlxMedia::~T_CGlxMedia()
    {
    }

// Default constructor
T_CGlxMedia::T_CGlxMedia()
    {
    }

// Second phase construct
void T_CGlxMedia::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void T_CGlxMedia::SetupL(  )
    {
    iCGlxMedia = new (ELeave) CGlxMedia(TGlxMediaId(KMediaId));;
    iCGlxMedia->SetTextValueL(KMPXMediaGeneralTitle, KMediaTitle);
    iCGlxMedia->SetTextValueL(KMPXMediaGeneralSubTitle, KMediaSubTitle);
    iCGlxMedia->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);
    }

void T_CGlxMedia::Teardown(  )
    {
    delete iCGlxMedia; 
    iCGlxMedia = NULL; 
    }

void T_CGlxMedia::T_CGlxMedia_DeleteAttributeL(  )
    {
    // Delete non existing attribute
    EUNIT_PRINT(_L("Delete non existing attribute"));
    iCGlxMedia->DeleteAttribute(KMPXMediaGeneralThumbnail1);
    EUNIT_ASSERT_DESC(iCGlxMedia->Count() == 3, "3 attributes should remain");
    EUNIT_ASSERT_DESC(iCGlxMedia->IsSupported(KMPXMediaGeneralTitle), "Title attribute should remain");
    EUNIT_ASSERT_DESC(iCGlxMedia->IsSupported(KMPXMediaGeneralSubTitle), "Subtitle attribute should remain");
    EUNIT_ASSERT_DESC(iCGlxMedia->IsSupported(KGlxMediaGeneralSystemItem), "System item attribute should remain");

    // Delete existing attribute
    EUNIT_PRINT(_L("Delete existing attribute"));
    iCGlxMedia->DeleteAttribute(KMPXMediaGeneralSubTitle);
    EUNIT_ASSERT_DESC(iCGlxMedia->Count() == 2, "2 attributes should remain");
    EUNIT_ASSERT_DESC(iCGlxMedia->IsSupported(KMPXMediaGeneralTitle), "Title attribute should remain");
    EUNIT_ASSERT_DESC(!iCGlxMedia->IsSupported(KMPXMediaGeneralSubTitle), "Subtitle attribute shouldn't remain");
    EUNIT_ASSERT_DESC(iCGlxMedia->IsSupported(KGlxMediaGeneralSystemItem), "System item attribute should remain");
    }

void T_CGlxMedia::T_CGlxMedia_DeleteAttribute2_SubTest1L(  )
    {
#ifdef _DEBUG
    // Delete index less than 0
    EUNIT_PRINT(_L("Delete index less than 0"));
    EUNIT_ASSERT_PANIC_DESC(iCGlxMedia->DeleteAttribute(-1), "Images & videos", EGlxPanicIllegalArgument, "Incorrect panic");
#endif // _DEBUG
    }

void T_CGlxMedia::T_CGlxMedia_DeleteAttribute2_SubTest2L(  )
    {
#ifdef _DEBUG
    // Delete index greater than number of attributes
    EUNIT_PRINT(_L("Delete index greater than number of attributes"));
    EUNIT_ASSERT_PANIC_DESC(iCGlxMedia->DeleteAttribute(3), "Images & videos", EGlxPanicIllegalArgument, "Incorrect panic");
#endif // _DEBUG
    }

void T_CGlxMedia::T_CGlxMedia_DeleteAttribute2_SubTest3L(  )
    {
    // Delete existing attribute
    EUNIT_PRINT(_L("Delete existing attribute"));
    iCGlxMedia->DeleteAttribute(2);
    EUNIT_ASSERT_DESC(iCGlxMedia->Count() == 2, "2 attributes should remain");
    EUNIT_ASSERT_DESC(iCGlxMedia->IsSupported(KMPXMediaGeneralTitle), "Title attribute should remain");
    EUNIT_ASSERT_DESC(iCGlxMedia->IsSupported(KMPXMediaGeneralSubTitle), "Subtitle attribute should remain");
    EUNIT_ASSERT_DESC(!iCGlxMedia->IsSupported(KGlxMediaGeneralSystemItem), "System item attribute shouldn't remain");
    }
    

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CGlxMedia,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "DeleteAttribute - test1",
    "CGlxMedia",
    "DeleteAttribute - test1",
    "FUNCTIONALITY",
    SetupL, T_CGlxMedia_DeleteAttributeL, Teardown)

EUNIT_TEST(
    "DeleteAttribute2 - test2, subtest1",
    "CGlxMedia",
    "DeleteAttribute2 - test2, subtest1",
    "FUNCTIONALITY",
    SetupL, T_CGlxMedia_DeleteAttribute2_SubTest1L, Teardown)

EUNIT_TEST(
    "DeleteAttribute2 - test2, subtest2",
    "CGlxMedia",
    "DeleteAttribute2 - test2, subtest2",
    "FUNCTIONALITY",
    SetupL, T_CGlxMedia_DeleteAttribute2_SubTest2L, Teardown)

EUNIT_TEST(
    "DeleteAttribute2 - test2, subtest3",
    "CGlxMedia",
    "DeleteAttribute2 - test2, subtest3",
    "FUNCTIONALITY",
    SetupL, T_CGlxMedia_DeleteAttribute2_SubTest3L, Teardown)
    

EUNIT_END_TEST_TABLE

//  END OF FILE
