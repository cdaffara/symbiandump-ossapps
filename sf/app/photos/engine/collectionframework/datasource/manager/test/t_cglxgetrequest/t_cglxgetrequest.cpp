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
#include "T_CGlxGetRequest.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "glxgetrequest.h"

// CONSTRUCTION
T_CGlxGetRequest* T_CGlxGetRequest::NewL()
    {
    T_CGlxGetRequest* self = T_CGlxGetRequest::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CGlxGetRequest* T_CGlxGetRequest::NewLC()
    {
    T_CGlxGetRequest* self = new( ELeave ) T_CGlxGetRequest();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CGlxGetRequest::~T_CGlxGetRequest()
    {
    }

// Default constructor
T_CGlxGetRequest::T_CGlxGetRequest()
    {
    }

// Second phase construct
void T_CGlxGetRequest::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void T_CGlxGetRequest::SetupL(  )
    {
    iCGlxGetRequest = NULL;
    RArray<TGlxMediaId> mediaIds;
    RArray<TMPXAttribute> attributeArray;
    CleanupClosePushL(mediaIds);
    CleanupClosePushL(attributeArray);
    mediaIds.AppendL(TGlxMediaId(0xC0FFEE));
    attributeArray.AppendL(TMPXAttribute(0xFEEDBABE,0xC0CAC01A));
    iCGlxGetRequest = new (ELeave) CGlxGetRequest(mediaIds, attributeArray, TUid::Uid(0));
    CleanupStack::Pop(2); //mediaIds, attributeArray
    }
    

void T_CGlxGetRequest::Teardown(  )
    {
    delete iCGlxGetRequest; 
     iCGlxGetRequest = NULL; 
    }
    
void T_CGlxGetRequest::T_CGlxGetRequest_MediaIdsL(  )
    {
 	TArray<TGlxMediaId> mediaIds = iCGlxGetRequest->MediaIds();
    EUNIT_ASSERT_EQUALS_DESC(mediaIds.Count(), 1 ,"MediaIds array count is 1");
    TGlxMediaId mediaId = mediaIds[0];
    EUNIT_ASSERT_EQUALS_DESC( mediaId.Value(), 0xC0FFEE, "MediaIds value is 0xC0FFEE");   
    }
    
void T_CGlxGetRequest::T_CGlxGetRequest_AttributesL(  )
    {
  	TArray<TMPXAttribute> attributeArray = iCGlxGetRequest->Attributes();
  	EUNIT_ASSERT_EQUALS_DESC(attributeArray.Count(), 1 ,"Attribue array count is 1");
    TMPXAttribute attribute = attributeArray[0];
    EUNIT_ASSERT_EQUALS_DESC(attribute.ContentId(), 0xFEEDBABE  ,"Attribue content id is 0xFEEDBABE");
    EUNIT_ASSERT_EQUALS_DESC(attribute.AttributeId(), 0xC0CAC01A  ,"Attribue attribute id is 0xC0CAC01A");
    }
    
//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CGlxGetRequest,
    "GlxGetRequest",
    "UNIT" )
    
EUNIT_TEST(
    "MediaIds",
    "CGlxGetRequest",
    "MediaIds",
    "FUNCTIONALITY",
    SetupL, T_CGlxGetRequest_MediaIdsL, Teardown)
    
EUNIT_TEST(
    "Attributes",
    "CGlxGetRequest",
    "Attributes",
    "FUNCTIONALITY",
    SetupL, T_CGlxGetRequest_AttributesL, Teardown)
    

EUNIT_END_TEST_TABLE

//  END OF FILE
