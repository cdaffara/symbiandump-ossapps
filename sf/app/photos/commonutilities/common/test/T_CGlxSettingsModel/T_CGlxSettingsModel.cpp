/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
#include "T_CGlxSettingsModel.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>


//  INTERNAL INCLUDES
// class under test
#include <glxsettingsmodel.h>

// CONSTRUCTION
T_CGlxSettingsModel* T_CGlxSettingsModel::NewL()
    {
    T_CGlxSettingsModel* self = T_CGlxSettingsModel::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CGlxSettingsModel* T_CGlxSettingsModel::NewLC()
    {
    T_CGlxSettingsModel* self = new( ELeave ) T_CGlxSettingsModel();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CGlxSettingsModel::~T_CGlxSettingsModel()
    {
    }

// Default constructor
T_CGlxSettingsModel::T_CGlxSettingsModel()
    {
    }

// Second phase construct
void T_CGlxSettingsModel::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS
void T_CGlxSettingsModel::SetupL()
    {
    // do nothing
    }

void T_CGlxSettingsModel::Teardown()
    {
    // reset default values
    const TUid KCRUidGallery = { 0x2000A7B7 };
	CRepository* repository = CRepository::NewL(KCRUidGallery);
    CleanupStack::PushL(repository);
	User::LeaveIfError(repository->Reset());
	CleanupStack::PopAndDestroy(repository);
	repository = NULL;
    }

void T_CGlxSettingsModel::TestL()
    {
    const TInt KTempVal = 1;
    CGlxSettingsModel::SetValueL(KGlxSortOrderTagManager, KTempVal);
    CGlxSettingsModel::SetValueL(KGlxSortOrderTagBrowser, KTempVal);
    CGlxSettingsModel::SetValueL(KGlxIconHiddenState, KTempVal);
    EUNIT_ASSERT(KTempVal == CGlxSettingsModel::ValueL<TInt>(KGlxSortOrderTagManager));
    EUNIT_ASSERT(KTempVal == CGlxSettingsModel::ValueL<TInt>(KGlxSortOrderTagBrowser));
    EUNIT_ASSERT(KTempVal == CGlxSettingsModel::ValueL<TInt>(KGlxIconHiddenState));
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    T_CGlxSettingsModel,
    "GLX CenRep Test.",
    "Ui Utilities" )

EUNIT_TEST(
    "Test Set And Get",
    "CGlxSettingsModel",
    "Value SetValue",
    "FUNCTIONALITY",
    SetupL, TestL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
