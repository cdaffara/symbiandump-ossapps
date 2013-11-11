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
#include "UT_TPhoneCmdParamQuery.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <avkon.hrh>
#include "PhoneUI.pan"
#include "TPhoneCmdParamQuery.h"

_LIT( KDataText, "datatext" );

// - Construction -----------------------------------------------------------
UT_TPhoneCmdParamQuery* UT_TPhoneCmdParamQuery::NewL()
    {
    UT_TPhoneCmdParamQuery* self = UT_TPhoneCmdParamQuery::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

UT_TPhoneCmdParamQuery* UT_TPhoneCmdParamQuery::NewLC()
    {
    UT_TPhoneCmdParamQuery* self = new( ELeave ) UT_TPhoneCmdParamQuery();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

UT_TPhoneCmdParamQuery::~UT_TPhoneCmdParamQuery()
    {
    iDataText.Close();
    }

UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery()
    {
    }

void UT_TPhoneCmdParamQuery::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iDataText.Assign( KDataText().AllocL() );
    }

// - Test methods -----------------------------------------------------------
void UT_TPhoneCmdParamQuery::SetupL(  )
    {
    TPhoneCmdParamQuery query;
    iTPhoneCmdParamQuery.SetQueryResourceId( 1 );
    query.SetDataText( &iDataText );
    iTPhoneCmdParamQuery = query; 
    }


void UT_TPhoneCmdParamQuery::Teardown(  )
    {

    }


void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_SetQueryResourceIdL(  )
    {
    // iTPhoneCmdParamQuery.SetQueryResourceId( <add parameters here > );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_QueryResourceIdL(  )
    {
    iTPhoneCmdParamQuery.QueryResourceId( );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_SetQueryPromptL(  )
    {
    // iTPhoneCmdParamQuery.SetQueryPrompt( <add parameters here > );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_QueryPromptL(  )
    {
    iTPhoneCmdParamQuery.QueryPrompt( );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_SetDefaultCbaL(  )
    {
    // iTPhoneCmdParamQuery.SetDefaultCba( <add parameters here > );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_DefaultCbaL(  )
    {
    iTPhoneCmdParamQuery.DefaultCba( );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_SetContentCbaL(  )
    {
    // iTPhoneCmdParamQuery.SetContentCba( <add parameters here > );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_ContentCbaL(  )
    {
    iTPhoneCmdParamQuery.ContentCba( );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_SetDataTextL(  )
    {
    // iTPhoneCmdParamQuery.SetDataText( <add parameters here > );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_DataTextL(  )
    {
    iTPhoneCmdParamQuery.DataText( );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_SetQueryTypeL(  )
    {
    // iTPhoneCmdParamQuery.SetQueryType( <add parameters here > );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_QueryTypeL(  )
    {
    iTPhoneCmdParamQuery.QueryType( );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_SetSendKeyEnabledL(  )
    {
    // iTPhoneCmdParamQuery.SetSendKeyEnabled( <add parameters here > );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_SendKeyEnabledL(  )
    {
    iTPhoneCmdParamQuery.SendKeyEnabled( );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_SetCommandParamIdL(  )
    {
    // iTPhoneCmdParamQuery.SetCommandParamId( <add parameters here > );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_SetTimeOutL(  )
    {
    //iTPhoneCmdParamQuery.SetTimeOut( <add parameters here > );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void UT_TPhoneCmdParamQuery::UT_TPhoneCmdParamQuery_TimeOutL(  )
    {
    iTPhoneCmdParamQuery.TimeOut( );
     EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }


void UT_TPhoneCmdParamQuery::T_SetToneL(  )
    {
    const TInt KValidToneId1 = 0;
    const TInt KValidToneId2 = 100;
    iTPhoneCmdParamQuery.SetTone( KValidToneId1 );
    iTPhoneCmdParamQuery.SetTone( KValidToneId2 );
    
    const TInt KInvalidToneId = -1;
    EUNIT_ASSERT_PANIC( iTPhoneCmdParamQuery.SetTone( KInvalidToneId ),
        "Common phone", EPhoneUtilsInvalidArgument );
    }


void UT_TPhoneCmdParamQuery::T_ToneL(  )
    {
    const TInt KValidToneId = 100;
    EUNIT_ASSERT_EQUALS( 0, iTPhoneCmdParamQuery.Tone() );
    
    iTPhoneCmdParamQuery.SetTone( KValidToneId );
    EUNIT_ASSERT_EQUALS( KValidToneId, iTPhoneCmdParamQuery.Tone() );
    }


void UT_TPhoneCmdParamQuery::T_SetCbaCommandMappingL(  )
    {
    for ( TInt i = 0; i < KMaxNumberOfCbaCommandMappings; i++ )
        {
        iTPhoneCmdParamQuery.SetCbaCommandMapping( i, i );
        }
    
    // try updating existing mapping
    iTPhoneCmdParamQuery.SetCbaCommandMapping( 0, 0 );
    
    // try to set more mappings than allowed
    const TInt KCustomCommandForYes = 100;
    EUNIT_ASSERT_PANIC( iTPhoneCmdParamQuery.
            SetCbaCommandMapping( EAknSoftkeyYes, KCustomCommandForYes ),
        "Common phone", EPhoneUtilsIndexOutOfBounds );
    }


void UT_TPhoneCmdParamQuery::T_CbaCommandMappingL()
    {
    const TInt KCustomCommandForYes = 100;
    iTPhoneCmdParamQuery.SetCbaCommandMapping( 
        EAknSoftkeyYes, KCustomCommandForYes );
    EUNIT_ASSERT_EQUALS( KCustomCommandForYes, 
        iTPhoneCmdParamQuery.CbaCommandMapping( EAknSoftkeyYes ) );
    }


void UT_TPhoneCmdParamQuery::T_SetCustomCommandForTimeOutL()
    {
    const TInt KCustomCommandForTimeOut1 = 100;
    const TInt KCustomCommandForTimeOut2 = 100;
    iTPhoneCmdParamQuery.SetCustomCommandForTimeOut( KCustomCommandForTimeOut1 );
    iTPhoneCmdParamQuery.SetCustomCommandForTimeOut( KCustomCommandForTimeOut2 );
    
    TInt command = 0;
    TInt result = iTPhoneCmdParamQuery.GetCustomCommandForTimeOut( command );
    EUNIT_ASSERT_EQUALS( KErrNone, result );
    EUNIT_ASSERT_EQUALS( KCustomCommandForTimeOut2, command );
    }


void UT_TPhoneCmdParamQuery::T_GetCustomCommandForTimeOutL()
    {
    TInt command = 0;
    TInt result = iTPhoneCmdParamQuery.GetCustomCommandForTimeOut( command );
    EUNIT_ASSERT_EQUALS( KErrNotFound, result );
    EUNIT_ASSERT_EQUALS( 0, command );
    }


void UT_TPhoneCmdParamQuery::T_AssignmentOperatorL()
    {
    for ( TInt i = 0; i < KMaxNumberOfCbaCommandMappings; i++ )
        {
        iTPhoneCmdParamQuery.SetCbaCommandMapping( i, i );
        }
    iTPhoneCmdParamQuery.SetCustomCommandForTimeOut( 0 );
    
    TPhoneCmdParamQuery query;
    query = iTPhoneCmdParamQuery;
    EUNIT_ASSERT_EQUALS( query.ParamId(), iTPhoneCmdParamQuery.ParamId() );
    EUNIT_ASSERT_EQUALS( query.QueryResourceId(), iTPhoneCmdParamQuery.QueryResourceId() );
    EUNIT_ASSERT_EQUALS( query.QueryPrompt(), iTPhoneCmdParamQuery.QueryPrompt() );
    EUNIT_ASSERT_EQUALS( query.DefaultCba(), iTPhoneCmdParamQuery.DefaultCba() );
    EUNIT_ASSERT_EQUALS( query.ContentCba(), iTPhoneCmdParamQuery.ContentCba() );
    EUNIT_ASSERT_EQUALS( query.DataText(), iTPhoneCmdParamQuery.DataText() );
    EUNIT_ASSERT_EQUALS( query.QueryType(), iTPhoneCmdParamQuery.QueryType() );
    EUNIT_ASSERT_EQUALS( query.SendKeyEnabled(), iTPhoneCmdParamQuery.SendKeyEnabled() );
    EUNIT_ASSERT_EQUALS( query.TimeOut(), iTPhoneCmdParamQuery.TimeOut() );
    EUNIT_ASSERT_EQUALS( query.Tone(), iTPhoneCmdParamQuery.Tone() );
    EUNIT_ASSERT_EQUALS( query.ItemTextArrayForListQuery(), 
                iTPhoneCmdParamQuery.ItemTextArrayForListQuery() );
    EUNIT_ASSERT_EQUALS( query.QueryCommandObserver(), 
                iTPhoneCmdParamQuery.QueryCommandObserver() );
        
    TInt timetOutCmd1 = 1;
    TInt timetOutCmd2 = 2;
    TInt result = query.GetCustomCommandForTimeOut( timetOutCmd1 );
    EUNIT_ASSERT_EQUALS( KErrNone, result );
    result = iTPhoneCmdParamQuery.GetCustomCommandForTimeOut( timetOutCmd2 );
    EUNIT_ASSERT_EQUALS( KErrNone, result );
    EUNIT_ASSERT_EQUALS( timetOutCmd1, timetOutCmd2 );
    
    for ( TInt i = 0; i <= KMaxNumberOfCbaCommandMappings; i++ )
        {
        EUNIT_ASSERT_EQUALS( query.CbaCommandMapping( i ), i ); 
        }
    }


void UT_TPhoneCmdParamQuery::T_CopyConstructorL()
    {
    for ( TInt i = 0; i < KMaxNumberOfCbaCommandMappings; i++ )
        {
        iTPhoneCmdParamQuery.SetCbaCommandMapping( i, i );
        }
    iTPhoneCmdParamQuery.SetCustomCommandForTimeOut( 0 );
    
    TPhoneCmdParamQuery query( iTPhoneCmdParamQuery );
    EUNIT_ASSERT_EQUALS( query.ParamId(), iTPhoneCmdParamQuery.ParamId() );
    EUNIT_ASSERT_EQUALS( query.QueryResourceId(), iTPhoneCmdParamQuery.QueryResourceId() );
    EUNIT_ASSERT_EQUALS( query.QueryPrompt(), iTPhoneCmdParamQuery.QueryPrompt() );
    EUNIT_ASSERT_EQUALS( query.DefaultCba(), iTPhoneCmdParamQuery.DefaultCba() );
    EUNIT_ASSERT_EQUALS( query.ContentCba(), iTPhoneCmdParamQuery.ContentCba() );
    EUNIT_ASSERT_EQUALS( query.DataText(), iTPhoneCmdParamQuery.DataText() );
    EUNIT_ASSERT_EQUALS( query.QueryType(), iTPhoneCmdParamQuery.QueryType() );
    EUNIT_ASSERT_EQUALS( query.SendKeyEnabled(), iTPhoneCmdParamQuery.SendKeyEnabled() );
    EUNIT_ASSERT_EQUALS( query.TimeOut(), iTPhoneCmdParamQuery.TimeOut() );
    EUNIT_ASSERT_EQUALS( query.Tone(), iTPhoneCmdParamQuery.Tone() );
    EUNIT_ASSERT_EQUALS( query.ItemTextArrayForListQuery(), 
            iTPhoneCmdParamQuery.ItemTextArrayForListQuery() );
    EUNIT_ASSERT_EQUALS( query.QueryCommandObserver(), 
            iTPhoneCmdParamQuery.QueryCommandObserver() );
    
    
    TInt timetOutCmd1 = 1;
    TInt timetOutCmd2 = 2;
    TInt result = query.GetCustomCommandForTimeOut( timetOutCmd1 );
    EUNIT_ASSERT_EQUALS( KErrNone, result );
    result = iTPhoneCmdParamQuery.GetCustomCommandForTimeOut( timetOutCmd2 );
    EUNIT_ASSERT_EQUALS( KErrNone, result );
    EUNIT_ASSERT_EQUALS( timetOutCmd1, timetOutCmd2 );
    
    for ( TInt i = 0; i <= KMaxNumberOfCbaCommandMappings; i++ )
        {
        EUNIT_ASSERT_EQUALS( query.CbaCommandMapping( i ), i ); 
        }
    }

// - EUnit test table -------------------------------------------------------
EUNIT_BEGIN_TEST_TABLE(
    UT_TPhoneCmdParamQuery,
    "Add test suite description here.",
    "UNIT" )
/*
EUNIT_TEST(
    "SetQueryResourceId - test0",
    "TPhoneCmdParamQuery",
    "SetQueryResourceId - test0",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_SetQueryResourceIdL, Teardown)
    
EUNIT_TEST(
    "QueryResourceId - test1",
    "TPhoneCmdParamQuery",
    "QueryResourceId - test1",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_QueryResourceIdL, Teardown)
    
EUNIT_TEST(
    "SetQueryPrompt - test2",
    "TPhoneCmdParamQuery",
    "SetQueryPrompt - test2",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_SetQueryPromptL, Teardown)
    
EUNIT_TEST(
    "QueryPrompt - test3",
    "TPhoneCmdParamQuery",
    "QueryPrompt - test3",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_QueryPromptL, Teardown)
    
EUNIT_TEST(
    "SetDefaultCba - test4",
    "TPhoneCmdParamQuery",
    "SetDefaultCba - test4",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_SetDefaultCbaL, Teardown)
    
EUNIT_TEST(
    "DefaultCba - test5",
    "TPhoneCmdParamQuery",
    "DefaultCba - test5",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_DefaultCbaL, Teardown)
    
EUNIT_TEST(
    "SetContentCba - test6",
    "TPhoneCmdParamQuery",
    "SetContentCba - test6",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_SetContentCbaL, Teardown)
    
EUNIT_TEST(
    "ContentCba - test7",
    "TPhoneCmdParamQuery",
    "ContentCba - test7",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_ContentCbaL, Teardown)
    
EUNIT_TEST(
    "SetDataText - test8",
    "TPhoneCmdParamQuery",
    "SetDataText - test8",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_SetDataTextL, Teardown)
    
EUNIT_TEST(
    "DataText - test9",
    "TPhoneCmdParamQuery",
    "DataText - test9",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_DataTextL, Teardown)
    
EUNIT_TEST(
    "SetQueryType - test10",
    "TPhoneCmdParamQuery",
    "SetQueryType - test10",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_SetQueryTypeL, Teardown)
    
EUNIT_TEST(
    "QueryType - test11",
    "TPhoneCmdParamQuery",
    "QueryType - test11",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_QueryTypeL, Teardown)
    
EUNIT_TEST(
    "SetSendKeyEnabled - test12",
    "TPhoneCmdParamQuery",
    "SetSendKeyEnabled - test12",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_SetSendKeyEnabledL, Teardown)
    
EUNIT_TEST(
    "SendKeyEnabled - test13",
    "TPhoneCmdParamQuery",
    "SendKeyEnabled - test13",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_SendKeyEnabledL, Teardown)
    
EUNIT_TEST(
    "SetCommandParamId - test14",
    "TPhoneCmdParamQuery",
    "SetCommandParamId - test14",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_SetCommandParamIdL, Teardown)
    
EUNIT_TEST(
    "SetTimeOut - test15",
    "TPhoneCmdParamQuery",
    "SetTimeOut - test15",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_SetTimeOutL, Teardown)
    
EUNIT_TEST(
    "TimeOut - test16",
    "TPhoneCmdParamQuery",
    "TimeOut - test16",
    "FUNCTIONALITY",
    SetupL, UT_TPhoneCmdParamQuery_TimeOutL, Teardown)
*/
EUNIT_TEST(
    "SetTone - test17",
    "TPhoneCmdParamQuery",
    "SetTone - test17",
    "FUNCTIONALITY",
    SetupL, T_SetToneL, Teardown)
    
EUNIT_TEST(
    "Tone - test18",
    "TPhoneCmdParamQuery",
    "Tone - test18",
    "FUNCTIONALITY",
    SetupL, T_ToneL, Teardown)

EUNIT_TEST(
    "SetCbaCommandMapping",
    "TPhoneCmdParamQuery",
    "SetCbaCommandMapping",
    "FUNCTIONALITY",
    SetupL, T_SetCbaCommandMappingL, Teardown)
    
EUNIT_TEST(
    "CbaCommandMapping",
    "TPhoneCmdParamQuery",
    "CbaCommandMapping",
    "FUNCTIONALITY",
    SetupL, T_CbaCommandMappingL, Teardown)

EUNIT_TEST(
    "SetCustomCommandForTimeOut",
    "TPhoneCmdParamQuery",
    "SetCustomCommandForTimeOut",
    "FUNCTIONALITY",
    SetupL, T_SetCustomCommandForTimeOutL, Teardown)
    
EUNIT_TEST(
    "GetCustomCommandForTimeOut",
    "TPhoneCmdParamQuery",
    "GetCustomCommandForTimeOut",
    "FUNCTIONALITY",
    SetupL, T_GetCustomCommandForTimeOutL, Teardown)

EUNIT_TEST(
    "Tests assignment operator.",
    "TPhoneCmdParamQuery",
    "operator=",
    "FUNCTIONALITY",
    SetupL, T_AssignmentOperatorL, Teardown)

EUNIT_TEST(
    "Tests copy constructor.",
    "TPhoneCmdParamQuery",
    "",
    "FUNCTIONALITY",
    SetupL, T_CopyConstructorL, Teardown)

EUNIT_END_TEST_TABLE
