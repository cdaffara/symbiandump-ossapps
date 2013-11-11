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
#include "T_CGlxIdListRequest.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "glxidlistrequest.h"

// CONSTRUCTION
T_CGlxIdListRequest* T_CGlxIdListRequest::NewL()
    {
    T_CGlxIdListRequest* self = T_CGlxIdListRequest::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CGlxIdListRequest* T_CGlxIdListRequest::NewLC()
    {
    T_CGlxIdListRequest* self = new( ELeave ) T_CGlxIdListRequest();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CGlxIdListRequest::~T_CGlxIdListRequest()
    {
    }

// Default constructor
T_CGlxIdListRequest::T_CGlxIdListRequest()
    {
    }

// Second phase construct
void T_CGlxIdListRequest::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void T_CGlxIdListRequest::SetupL(  )
    {
    iCGlxIdListRequest = CGlxIdListRequest::NewL(TGlxMediaId(123), TUid::Uid(0));
    }
    

void T_CGlxIdListRequest::Teardown(  )
    {
    delete iCGlxIdListRequest; 
     iCGlxIdListRequest = NULL; 
    }
    
void T_CGlxIdListRequest::T_CGlxIdListRequest_ContainerIdL()
    {
    EUNIT_ASSERT_EQUALS_DESC(iCGlxIdListRequest->ContainerId(), TGlxMediaId(123), "MediaID is 123");
    }
    
//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CGlxIdListRequest,
    "CGlxIdListRequest",
    "UNIT" )
    
EUNIT_TEST(
    "ContainerId",
    "CGlxIdListRequest",
    "ContainerId",
    "FUNCTIONALITY",
    SetupL, T_CGlxIdListRequest_ContainerIdL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
