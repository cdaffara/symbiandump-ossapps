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
* Description:   CGlxFetchErrorArray unit tests
*
*/



//  CLASS HEADER
#include "t_cglxfetcherrorarray.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <mpxmediageneraldefs.h>

#include <glxmediageneraldefs.h>

//  INTERNAL INCLUDES
#include "glxfetcherrorarray.h"
#include "glxfetcherror.h"

const TInt KTestMediaIdGeneral = 1;

// CONSTRUCTION
T_CGlxFetchErrorArray* T_CGlxFetchErrorArray::NewL()
    {
    T_CGlxFetchErrorArray* self = T_CGlxFetchErrorArray::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CGlxFetchErrorArray* T_CGlxFetchErrorArray::NewLC()
    {
    T_CGlxFetchErrorArray* self = new( ELeave ) T_CGlxFetchErrorArray();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CGlxFetchErrorArray::~T_CGlxFetchErrorArray()
    {
    }

// Default constructor
T_CGlxFetchErrorArray::T_CGlxFetchErrorArray()
    {
    }

// Second phase construct
void T_CGlxFetchErrorArray::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void T_CGlxFetchErrorArray::SetupL(  )
    {
    EUNIT_PRINT(_L("Initial setup"));

    EUNIT_PRINT(_L("ErrorArray1 has 2 errors"));
    iCGlxFetchErrorArray1 = new (ELeave) CGlxFetchErrorArray();

    TGlxFetchError error1(KMPXMediaGeneralSize, KErrNotSupported);
    iCGlxFetchErrorArray1->AddErrorL(error1);

    TGlxFetchError error2(KGlxMediaGeneralSystemItem, KErrNotFound);
    iCGlxFetchErrorArray1->AddErrorL(error2);

    EUNIT_PRINT(_L("ErrorArray2 has 0 errors"));
    iCGlxFetchErrorArray2 = new (ELeave) CGlxFetchErrorArray();
    }
    

void T_CGlxFetchErrorArray::Teardown(  )
    {
    delete iCGlxFetchErrorArray1; 
    iCGlxFetchErrorArray1 = NULL; 

    delete iCGlxFetchErrorArray2; 
    iCGlxFetchErrorArray2 = NULL; 
    }
    

void T_CGlxFetchErrorArray::T_CGlxFetchErrorArray_ErrorCountL(  )
    {
    // Check ErrorArray1 has 2 errors
    EUNIT_PRINT(_L("Check ErrorArray1 has 2 errors"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray1->ErrorCount() == 2, "Incorrect error count");

    // Check ErrorArray2 has 0 errors
    EUNIT_PRINT(_L("Check ErrorArray2 has 0 errors"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray2->ErrorCount() == 0, "Incorrect error count");
    }
    
void T_CGlxFetchErrorArray::T_CGlxFetchErrorArray_ErrorL(  )
    {
    // Obtain error at index 0
    EUNIT_PRINT(_L("Obtain error at ErrorArray1 index 0"));
    const TGlxFetchError& error1 = iCGlxFetchErrorArray1->Error(0);

    EUNIT_PRINT(_L("Check error at ErrorArray1 index 0"));
    EUNIT_ASSERT_DESC(error1.iAttr == KMPXMediaGeneralSize && error1.iError == KErrNotSupported, "Incorrect error returned");

    // Obtain error at index 1
    EUNIT_PRINT(_L("Obtain error at ErrorArray1 index 1"));
    const TGlxFetchError& error2 = iCGlxFetchErrorArray1->Error(1);

    EUNIT_PRINT(_L("Check error at ErrorArray1 index 1"));
    EUNIT_ASSERT_DESC(error2.iAttr == KGlxMediaGeneralSystemItem && error2.iError == KErrNotFound, "Incorrect error returned");
    }
    
void T_CGlxFetchErrorArray::T_CGlxFetchErrorArray_AddErrorLL(  )
    {
    // Add existing error
    EUNIT_PRINT(_L("Add existing error to ErrorArray1"));
    TGlxFetchError error1(KMPXMediaGeneralSize, KErrGeneral);
    iCGlxFetchErrorArray1->AddErrorL(error1);

    EUNIT_PRINT(_L("Check ErrorArray1 still has 2 errors"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray1->ErrorCount() == 2, "Incorrect error count");

    const TGlxFetchError& storedError1 = iCGlxFetchErrorArray1->Error(0);
    EUNIT_ASSERT_DESC(storedError1.iAttr == KMPXMediaGeneralSize && storedError1.iError == KErrGeneral, "Incorrect error returned");

    // Add new error
    EUNIT_PRINT(_L("Add new error to ErrorArray2"));
    TGlxFetchError error2(KMPXMediaGeneralDate, KErrCorrupt);
    iCGlxFetchErrorArray2->AddErrorL(error2);

    EUNIT_PRINT(_L("Check ErrorArray2 has 1 new error"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray2->ErrorCount() == 1, "Incorrect error count");

    const TGlxFetchError& storedError2 = iCGlxFetchErrorArray2->Error(0);
    EUNIT_ASSERT_DESC(storedError2.iAttr == KMPXMediaGeneralDate && storedError2.iError == KErrCorrupt, "Incorrect error returned");
    }
    
void T_CGlxFetchErrorArray::T_CGlxFetchErrorArray_RemoveL(  )
    {
    // Remove error at index 1
    EUNIT_PRINT(_L("Remove error at ErrorArray1 index 1"));
    iCGlxFetchErrorArray1->Remove(1);

    EUNIT_PRINT(_L("Check ErrorArray1 still has 1 error"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray1->ErrorCount() == 1, "Incorrect error count");

    const TGlxFetchError& storedError1 = iCGlxFetchErrorArray1->Error(0);
    EUNIT_ASSERT_DESC(storedError1.iAttr == KMPXMediaGeneralSize && storedError1.iError == KErrNotSupported, "Incorrect error returned");
    }
    
void T_CGlxFetchErrorArray::T_CGlxFetchErrorArray_FindErrorL(  )
    {
    // Find existing error
    EUNIT_PRINT(_L("Find existing error in ErrorArray1"));

    EUNIT_PRINT(_L("Check correct ErrorArray1 index returned"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray1->FindError(KMPXMediaGeneralSize) == 0, "Incorrect index for error");

    // Find non-existing error
    EUNIT_PRINT(_L("Find non-existing error in ErrorArray1"));

    EUNIT_PRINT(_L("Check KErrNotFound returned"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray1->FindError(KMPXMediaGeneralTitle) == KErrNotFound, "Index should be KErrNotFound");

    // Find non-existing error on empty ErrorArray2
    EUNIT_PRINT(_L("Find non-existing error in empty ErrorArray2"));

    EUNIT_PRINT(_L("Check KErrNotFound returned"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray2->FindError(KMPXMediaGeneralSize) == KErrNotFound, "Index should be KErrNotFound");
    }
    
void T_CGlxFetchErrorArray::T_CGlxFetchErrorArray_FindError2L(  )
    {
    // Find KMPXMediaIdGeneral errors
    EUNIT_PRINT(_L("Find KMPXMediaIdGeneral errors in ErrorArray1"));

    EUNIT_PRINT(_L("Check correct ErrorArray1 index returned"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray1->FindError(KMPXMediaIdGeneral) == 0, "Incorrect index for error");

    // Find KGlxMediaIdGeneral errors
    EUNIT_PRINT(_L("Find KGlxMediaIdGeneral errors in ErrorArray1"));

    EUNIT_PRINT(_L("Check correct ErrorArray1 index returned"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray1->FindError(KGlxMediaIdGeneral) == 1, "Incorrect index for error");

    // Find non-existing error
    EUNIT_PRINT(_L("Find non-existing error in ErrorArray1"));

    EUNIT_PRINT(_L("Check KErrNotFound returned"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray1->FindError(KTestMediaIdGeneral) == KErrNotFound, "Index should be KErrNotFound");

    // Find non-existing error on empty ErrorArray2
    EUNIT_PRINT(_L("Find non-existing error on empty ErrorArray2"));

    EUNIT_PRINT(_L("Check KErrNotFound returned"));
    EUNIT_ASSERT_DESC(iCGlxFetchErrorArray2->FindError(KTestMediaIdGeneral) == KErrNotFound, "Index should be KErrNotFound");
    }
    

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CGlxFetchErrorArray,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "ErrorCount - test1",
    "CGlxFetchErrorArray",
    "ErrorCount - test1",
    "FUNCTIONALITY",
    SetupL, T_CGlxFetchErrorArray_ErrorCountL, Teardown)
    
EUNIT_TEST(
    "Error - test2",
    "CGlxFetchErrorArray",
    "Error - test2",
    "FUNCTIONALITY",
    SetupL, T_CGlxFetchErrorArray_ErrorL, Teardown)
    
EUNIT_TEST(
    "AddErrorL - test3",
    "CGlxFetchErrorArray",
    "AddErrorL - test3",
    "FUNCTIONALITY",
    SetupL, T_CGlxFetchErrorArray_AddErrorLL, Teardown)
    
EUNIT_TEST(
    "Remove - test4",
    "CGlxFetchErrorArray",
    "Remove - test4",
    "FUNCTIONALITY",
    SetupL, T_CGlxFetchErrorArray_RemoveL, Teardown)
    
EUNIT_TEST(
    "FindError - test5",
    "CGlxFetchErrorArray",
    "FindError - test5",
    "FUNCTIONALITY",
    SetupL, T_CGlxFetchErrorArray_FindErrorL, Teardown)
    
EUNIT_TEST(
    "FindError - test6",
    "CGlxFetchErrorArray",
    "FindError - test6",
    "FUNCTIONALITY",
    SetupL, T_CGlxFetchErrorArray_FindError2L, Teardown)
    

EUNIT_END_TEST_TABLE

//  END OF FILE
