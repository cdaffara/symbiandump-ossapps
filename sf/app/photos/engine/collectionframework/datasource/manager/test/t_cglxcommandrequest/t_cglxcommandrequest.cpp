/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "T_CGlxCommandRequest.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "glxcommandrequest.h"

_LIT(KSomeSetData, "Some test data");

// CONSTRUCTION
T_CGlxCommandRequest* T_CGlxCommandRequest::NewL()
    {
    T_CGlxCommandRequest* self = T_CGlxCommandRequest::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CGlxCommandRequest* T_CGlxCommandRequest::NewLC()
    {
    T_CGlxCommandRequest* self = new( ELeave ) T_CGlxCommandRequest();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CGlxCommandRequest::~T_CGlxCommandRequest()
    {
    }

// Default constructor
T_CGlxCommandRequest::T_CGlxCommandRequest()
    {
    }

// Second phase construct
void T_CGlxCommandRequest::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void T_CGlxCommandRequest::SetupL(  )
    {
    CMPXCommand* command = CMPXCommand::NewL();	
    CleanupStack::PushL(command);
    command->SetTextValueL(TMPXAttribute(1,2),KSomeSetData);
    iCGlxCommandRequest = CGlxCommandRequest::NewL(*command, TUid::Uid(0), *this);
    CleanupStack::PopAndDestroy(); // command
    }
    

void T_CGlxCommandRequest::Teardown(  )
    {
    delete iCGlxCommandRequest; 
     iCGlxCommandRequest = NULL; 
    }
    
void T_CGlxCommandRequest::T_CGlxCommandRequest_CommandL(  )
    {
	TMPXAttribute attribute(TMPXAttribute(1,2));
	const CMPXCommand& command(iCGlxCommandRequest->Command());
 	EUNIT_ASSERT_DESC(command.IsSupported(attribute), "Attribute set on command should 'be supported'");
	EUNIT_ASSERT_DESC(command.ValueText(attribute).Compare(KSomeSetData) == 0, "ValueText() for the given attribute should be identical to KSomeSetData");
    }

void T_CGlxCommandRequest::HandleMessage(CMPXMessage& aMessage)
	{
	// Don't do anything
	}

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CGlxCommandRequest,
    "CGlxCommandRequest",
    "UNIT" )
    
EUNIT_TEST(
    "CGlxCommandRequest",
    "Command",
    "Command()",
    "FUNCTIONALITY",
    SetupL, T_CGlxCommandRequest_CommandL, Teardown)
    

EUNIT_END_TEST_TABLE

//  END OF FILE
