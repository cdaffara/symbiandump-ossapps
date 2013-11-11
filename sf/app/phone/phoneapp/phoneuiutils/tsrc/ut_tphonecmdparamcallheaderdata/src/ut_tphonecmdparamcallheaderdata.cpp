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
/*
-----------------------------------------------------------------------------
This file has been generated with EUnit Pro
http://www.sysopendigia.com/qualitykit
-----------------------------------------------------------------------------
*/
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <avkon.hrh>
#include "tphonecmdparamcallheaderdata.h"
#include "ut_tphonecmdparamcallheaderdata.h"

// - Construction -----------------------------------------------------------
UT_TPhoneCmdParamCallHeaderData* UT_TPhoneCmdParamCallHeaderData::NewL()
    {
    UT_TPhoneCmdParamCallHeaderData* self = UT_TPhoneCmdParamCallHeaderData::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

UT_TPhoneCmdParamCallHeaderData* UT_TPhoneCmdParamCallHeaderData::NewLC()
    {
    UT_TPhoneCmdParamCallHeaderData* self = new( ELeave ) UT_TPhoneCmdParamCallHeaderData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

UT_TPhoneCmdParamCallHeaderData::~UT_TPhoneCmdParamCallHeaderData()
    {
    }

UT_TPhoneCmdParamCallHeaderData::UT_TPhoneCmdParamCallHeaderData()
    {
    }

void UT_TPhoneCmdParamCallHeaderData::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------
void UT_TPhoneCmdParamCallHeaderData::SetupL(  )
    {
    }


void UT_TPhoneCmdParamCallHeaderData::Teardown(  )
    {
    }  

void UT_TPhoneCmdParamCallHeaderData::UT_TPhoneCmdParamCallHeaderData_LeaveCleanup ()
    {
    TPhoneCmdParamCallHeaderData data;
    data.SetLabelText(_L("Test3"));
    TRAP_IGNORE(LeaveFunctionL ());
    }

void UT_TPhoneCmdParamCallHeaderData::LeaveFunctionL ()
    {
    TPhoneCmdParamCallHeaderData data;
    data.SetLabelText(_L("Test2"));
    User::Leave(KErrGeneral);
    }

// - EUnit test table -------------------------------------------------------
EUNIT_BEGIN_TEST_TABLE(
    UT_TPhoneCmdParamCallHeaderData,
    "EUnit - Phone - TPhoneCmdParamCallHeaderData",
    "UNIT" )
    
EUNIT_TEST(
    "Test leave cleanup",
    "UT_TPhoneCmdParamCallHeaderData",
    "Test leave cleanup",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamCallHeaderData_LeaveCleanup, Teardown)

EUNIT_END_TEST_TABLE
