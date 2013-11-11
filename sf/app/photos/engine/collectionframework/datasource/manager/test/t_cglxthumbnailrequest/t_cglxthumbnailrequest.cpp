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
#include "T_CGlxThumbnailRequest.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "glxthumbnailrequest.h"

// CONSTRUCTION
T_CGlxThumbnailRequest* T_CGlxThumbnailRequest::NewL()
    {
    T_CGlxThumbnailRequest* self = T_CGlxThumbnailRequest::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CGlxThumbnailRequest* T_CGlxThumbnailRequest::NewLC()
    {
    T_CGlxThumbnailRequest* self = new( ELeave ) T_CGlxThumbnailRequest();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CGlxThumbnailRequest::~T_CGlxThumbnailRequest()
    {
    }

// Default constructor
T_CGlxThumbnailRequest::T_CGlxThumbnailRequest()
    {
    }

// Second phase construct
void T_CGlxThumbnailRequest::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


						 
void T_CGlxThumbnailRequest::SetupL(  )
    {
    iCGlxThumbnailRequest = CGlxThumbnailRequest::NewL(TGlxMediaId(0xCAFE), 20, 
						 CGlxThumbnailRequest::EPrioritizeSpeed, ETrue, 0xC0FFEE, TUid::Uid(0));
    }
    

void T_CGlxThumbnailRequest::Teardown(  )
    {
    delete iCGlxThumbnailRequest; 
     iCGlxThumbnailRequest = NULL; 
    }
    
void T_CGlxThumbnailRequest::T_CGlxThumbnailRequest_ItemIdL(  )
    {
    EUNIT_ASSERT_EQUALS_DESC(iCGlxThumbnailRequest->ItemId(),TGlxMediaId(0xCAFE), "Item ID is 0xCAFE");
    }
    
void T_CGlxThumbnailRequest::T_CGlxThumbnailRequest_ThumbnailRequestL(  )
    {
    TGlxThumbnailRequest tnr;
    iCGlxThumbnailRequest->ThumbnailRequest(tnr);
    EUNIT_ASSERT_EQUALS_DESC(tnr.iId, TGlxMediaId(0xCAFE), "Item ID is 0xCAFE");
    EUNIT_ASSERT_EQUALS_DESC(tnr.iSizeClass, 20, "Size class is 20");
    EUNIT_ASSERT_EQUALS_DESC(tnr.iPriorityMode, CGlxThumbnailRequest::EPrioritizeSpeed, "Priority mode is CGlxThumbnailRequest::EPrioritizeSpeed");   
    }
    

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CGlxThumbnailRequest,
    "CGlxThumbnailRequest",
    "UNIT" )
    
EUNIT_TEST(
    "ItemId",
    "CGlxThumbnailRequest",
    "ItemId",
    "FUNCTIONALITY",
    SetupL, T_CGlxThumbnailRequest_ItemIdL, Teardown)
    
EUNIT_TEST(
    "ThumbnailRequest",
    "CGlxThumbnailRequest",
    "ThumbnailRequest",
    "FUNCTIONALITY",
    SetupL, T_CGlxThumbnailRequest_ThumbnailRequestL, Teardown)
    

EUNIT_END_TEST_TABLE

//  END OF FILE
