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
#include "tphonecmdparamglobalnote.h"
#include "ut_tphonecmdparamglobalnote.h"
#include "phoneconstants.h"

// - Construction -----------------------------------------------------------
UT_TPhoneCmdParamGlobalNote* UT_TPhoneCmdParamGlobalNote::NewL()
    {
    UT_TPhoneCmdParamGlobalNote* self = UT_TPhoneCmdParamGlobalNote::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

UT_TPhoneCmdParamGlobalNote* UT_TPhoneCmdParamGlobalNote::NewLC()
    {
    UT_TPhoneCmdParamGlobalNote* self = new( ELeave ) UT_TPhoneCmdParamGlobalNote();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

UT_TPhoneCmdParamGlobalNote::~UT_TPhoneCmdParamGlobalNote()
    {
    
    }

UT_TPhoneCmdParamGlobalNote::UT_TPhoneCmdParamGlobalNote()
    {
    }

void UT_TPhoneCmdParamGlobalNote::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------
void UT_TPhoneCmdParamGlobalNote::SetupL(  )
    {
    
    }


void UT_TPhoneCmdParamGlobalNote::Teardown(  )
    {

    }

void UT_TPhoneCmdParamGlobalNote::ut_TimeoutL()
    {
    TPhoneCmdParamGlobalNote globalNote;
    TInt timeout = globalNote.Timeout();
    EUNIT_ASSERT(timeout == 0);
    }

void UT_TPhoneCmdParamGlobalNote::ut_SetTimeoutL()
{
    TPhoneCmdParamGlobalNote globalNote;
    globalNote.SetTimeout(KPhoneNoteNoTimeout);
    EUNIT_ASSERT(globalNote.Timeout() == KPhoneNoteNoTimeout);
}

EUNIT_BEGIN_TEST_TABLE(
    UT_TPhoneCmdParamGlobalNote,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "",
    "UT_TPhoneCmdParamGlobalNote",
    "Timeout",
    "FUNCTIONALITY",
    SetupL, ut_TimeoutL, Teardown)

EUNIT_TEST(
    "",
    "UT_TPhoneCmdParamGlobalNote",
    "SetTimeout",
    "FUNCTIONALITY",
    SetupL, ut_SetTimeoutL, Teardown)

EUNIT_END_TEST_TABLE
