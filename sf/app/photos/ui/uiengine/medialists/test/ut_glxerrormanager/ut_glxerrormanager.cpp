/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ut_glxerrormanager.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <mpxcollectionmessage.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediageneraldefs.h>

#include <glxmediageneraldefs.h>
//  INTERNAL INCLUDES
#include "glxmediaid.h"
#include "glxerrormanager.h"
#include "glxfetcherrorarray.h"
#include "glxmedia.h"
#include "glxcachemanager.h"
#include "glxmedialist.h"

#include <glxdrmutility.h>

void CGlxDRMUtility::Close()
    {
    delete this;
    }
    
TBool CGlxDRMUtility::ItemRightsValidityCheckL(const TDesC&, TBool)
    {
    return ETrue;
    }
    
CGlxDRMUtility* CGlxDRMUtility::InstanceL()
    {
    CGlxDRMUtility* drm = new(ELeave)CGlxDRMUtility();
    return drm;
    }
    
TSize CGlxDRMUtility::DRMThumbnailSize(TSize& aSize)
    {
    return aSize;
    }
    
CGlxDRMUtility::CGlxDRMUtility()
    {
    
    }
    
CGlxDRMUtility::~CGlxDRMUtility()
    {
    
    }


// CONSTRUCTION
ut_glxerrormanager* ut_glxerrormanager::NewL()
    {
    ut_glxerrormanager* self = ut_glxerrormanager::NewLC();
    CleanupStack::Pop();

    return self;
    }

ut_glxerrormanager* ut_glxerrormanager::NewLC()
    {
    ut_glxerrormanager* self = new( ELeave ) ut_glxerrormanager();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
ut_glxerrormanager::~ut_glxerrormanager()
    {
    }

// Default constructor
ut_glxerrormanager::ut_glxerrormanager()
    {
    }

// Second phase construct
void ut_glxerrormanager::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void ut_glxerrormanager::SetupL(  )
    {
    iItem1 = new (ELeave) CGlxMedia(TGlxMediaId(1));
    
    iItem2 = new (ELeave) CGlxMedia(TGlxMediaId(2));
    RArray<TMPXAttribute> attrSet2;
    CleanupClosePushL(attrSet2);
    attrSet2.AppendL(KMPXMediaGeneralThumbnail1);
    attrSet2.AppendL(KMPXMediaGeneralDate);
    attrSet2.AppendL(KMPXMediaGeneralSize);
    GlxErrorManager::SetAttributeErrorL(iItem2, attrSet2, KErrGeneral);
    CleanupStack::PopAndDestroy(&attrSet2);
    
    iItem3 = new (ELeave) CGlxMedia(TGlxMediaId(3));
    RArray<TMPXAttribute> attrSet3;
    CleanupClosePushL(attrSet3);
    attrSet3.AppendL(KMPXMediaGeneralThumbnail1);
    GlxErrorManager::SetAttributeErrorL(iItem3, attrSet3, KErrGeneral);
    CleanupStack::PopAndDestroy(&attrSet3);
    
    iItem4 = new (ELeave) CGlxMedia(TGlxMediaId(4));
    iItem5 = new (ELeave) CGlxMedia(TGlxMediaId(5));

    iItem6 = new (ELeave) CGlxMedia(TGlxMediaId(6));
    RArray<TMPXAttribute> attrSet6;
    CleanupClosePushL(attrSet6);
    attrSet6.AppendL(KMPXMediaGeneralThumbnail1);
    GlxErrorManager::SetAttributeErrorL(iItem6, attrSet6, KErrNotSupported);
    CleanupStack::PopAndDestroy(&attrSet6);
    }
    

void ut_glxerrormanager::Teardown(  )
    {
    delete iItem1;
    delete iItem2;
    delete iItem3;
    delete iItem4;
    delete iItem5;
    delete iItem6;
    }
    

// -----------------------------------------------------------------------------
// ***********************  WARNING ********************************************
// -----------------------------------------------------------------------------
// This test may not complete unless the EUnit timeout settings are altered
// The timeout settings must be more than:
// KGlxTemporaryErrorValidityPeriodInSeconds * 1.25
// (Currently, 60 seconds will do it)
// -----------------------------------------------------------------------------
//
void ut_glxerrormanager::T_Test1L(  )
    {
    // Check for attribute errors on null item
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(NULL, KMPXMediaGeneralThumbnail1), 
        "Null item has an error on KMPXMediaIdGeneral attributes");

    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralThumbnail1), 
        "iItem1 has an error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralDate), 
        "iItem1 has an error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralSize), 
        "iItem1 has an error on KMPXMediaGeneralSize");

    // Check for attribute errors on non errored attribute
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaGeneralTitle), 
        "iItem2 has an error on KMPXMediaGeneralTitle");

    EUNIT_ASSERT_DESC( KErrNone != GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaGeneralThumbnail1), 
        "iItem2 has no error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrNone != GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaGeneralDate), 
        "iItem2 has no error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone != GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaGeneralSize), 
        "iItem2 has no error on KMPXMediaGeneralSize");
    
    EUNIT_ASSERT_DESC( KErrNone != GlxErrorManager::HasAttributeErrorL(iItem3, KMPXMediaGeneralThumbnail1), 
        "iItem3 has no error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem3, KMPXMediaGeneralDate), 
        "iItem3 has an error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem3, KMPXMediaGeneralSize), 
        "iItem3 has an error on KMPXMediaGeneralSize");

    // Wait for half the validity period
    User::After(KGlxTemporaryErrorValidityPeriodInSeconds * 1000000 * 1.25);

    // Check for permanent error
    EUNIT_ASSERT_DESC( KErrNotSupported == GlxErrorManager::HasAttributeErrorL(iItem6, KMPXMediaGeneralThumbnail1), 
        "iItem6 has no error on KMPXMediaGeneralThumbnail1");
    }

void ut_glxerrormanager::T_Test2L(  )
    {
    RArray<TMPXAttribute> attrSet;
    CleanupClosePushL(attrSet);
    
    // Set errors one by one
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralThumbnail1), 
        "iItem1 has an error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralDate), 
        "iItem1 has an error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralSize), 
        "iItem1 has an error on KMPXMediaGeneralSize");
        
    attrSet.Close();            
    attrSet.AppendL(KMPXMediaGeneralThumbnail1);
    GlxErrorManager::SetAttributeErrorL(iItem1, attrSet, KErrGeneral);

    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralThumbnail1), 
        "iItem1 has wrong error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralDate), 
        "iItem1 has an error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralSize), 
        "iItem1 has an error on KMPXMediaGeneralSize");
        
    attrSet.Close();            
    attrSet.AppendL(KMPXMediaGeneralDate);
    GlxErrorManager::SetAttributeErrorL(iItem1, attrSet, KErrGeneral);

    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralThumbnail1), 
        "iItem1 has wrong error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralDate), 
        "iItem1 has wrong error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralSize), 
        "iItem1 has an error on KMPXMediaGeneralSize");

    attrSet.Close();            
    attrSet.AppendL(KMPXMediaGeneralSize);
    GlxErrorManager::SetAttributeErrorL(iItem1, attrSet, KErrGeneral);
    
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralThumbnail1), 
        "iItem1 has wrong error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralDate), 
        "iItem1 has wrong error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralSize), 
        "iItem1 has wrong error on KMPXMediaGeneralSize");


    // Set many errors in one go
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralThumbnail1), 
        "iItem4 has an error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralDate), 
        "iItem4 has an error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralSize), 
        "iItem4 has an error on KMPXMediaGeneralSize");

    attrSet.Close();            
    attrSet.AppendL(KMPXMediaGeneralThumbnail1);
    attrSet.AppendL(KMPXMediaGeneralDate);
    attrSet.AppendL(KMPXMediaGeneralSize);
    GlxErrorManager::SetAttributeErrorL(iItem4, attrSet, KErrGeneral);

    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralThumbnail1), 
        "iItem4 has wrong error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralDate), 
        "iItem4 has wrong error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralSize), 
        "iItem4 has wrong error on KMPXMediaGeneralSize");

    CleanupStack::PopAndDestroy(&attrSet);
    }
    
void ut_glxerrormanager::T_Test3L(  )
    {
    // Check for errors on null item
    EUNIT_ASSERT_DESC( !GlxErrorManager::HasError(NULL), 
        "Null item has some errors attached");

    EUNIT_ASSERT_DESC( EFalse == GlxErrorManager::HasError(iItem1), 
        "iItem1 has some errors attached");
    EUNIT_ASSERT_DESC( ETrue == GlxErrorManager::HasError(iItem2), 
        "iItem2 has no errors attached");
    EUNIT_ASSERT_DESC( ETrue == GlxErrorManager::HasError(iItem3), 
        "iItem3 has no errors attached");
    EUNIT_ASSERT_DESC( EFalse == GlxErrorManager::HasError(iItem4), 
        "iItem4 has some errors attached");
    EUNIT_ASSERT_DESC( EFalse == GlxErrorManager::HasError(iItem5), 
        "iItem5 has some errors attached");

    // Check for permanent errors
    EUNIT_ASSERT_DESC( GlxErrorManager::HasError(iItem6), 
        "iItem6 has no errors attached");
    }
    
void ut_glxerrormanager::T_Test4L(  )
    {
    // Check returned error array attribute
    TMPXAttribute errorAttribute = GlxErrorManager::ErrorAttribute();

    // HACK - error attribute is private, so these should reflect the values used in the error manager implementation
    EUNIT_ASSERT_DESC(errorAttribute.ContentId() == 0x200071AC && errorAttribute.AttributeId() == 0x01, "Incorrect error attribute");
    }
    
// -----------------------------------------------------------------------------
// ***********************  WARNING ********************************************
// -----------------------------------------------------------------------------
// This test may not complete unless the EUnit timeout settings are altered
// The timeout settings must be more than:
// KGlxTemporaryErrorValidityPeriodInSeconds * 1.25
// (Currently, 60 seconds will do it)
// -----------------------------------------------------------------------------
//
void ut_glxerrormanager::T_Test5L(  )
    {
    RArray<TMPXAttribute> attrSet;
    CleanupClosePushL(attrSet);

    // Clean unused errors on iItem3
    attrSet.AppendL(KMPXMediaGeneralTitle);
    GlxErrorManager::ClearExpiredAndUnusedErrorsL(*iItem3, attrSet);

    // HasError should now report false for iItem3
    EUNIT_ASSERT_DESC( !GlxErrorManager::HasError(iItem3), 
        "iItem3 has unused errors attached");

    attrSet.Reset();
    attrSet.AppendL(KMPXMediaGeneralThumbnail1);
    GlxErrorManager::SetAttributeErrorL(iItem1, attrSet, KErrGeneral);
    
    // Wait for half the validity period
    User::After(KGlxTemporaryErrorValidityPeriodInSeconds * 1000000 / 2);
    
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralThumbnail1), 
        "iItem1 has wrong error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralDate), 
        "iItem1 has an error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralSize), 
        "iItem1 has an error on KMPXMediaGeneralSize");

    attrSet.Reset();
    attrSet.AppendL(KMPXMediaGeneralDate);
    GlxErrorManager::SetAttributeErrorL(iItem1, attrSet, KErrGeneral);
        
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralThumbnail1), 
        "iItem1 has wrong error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralDate), 
        "iItem1 has wrong error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralSize), 
        "iItem1 has an error on KMPXMediaGeneralSize");

    // Wait for another .75 of the validity period
    User::After(KGlxTemporaryErrorValidityPeriodInSeconds * 1000000 * 3 / 4);

    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralThumbnail1), 
        "iItem1 has an unexpired error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralDate), 
        "iItem1 has expired error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralSize), 
        "iItem1 has an error on KMPXMediaGeneralSize");

    // HasError should report true for iItem1, since there is still an outstanding error
    EUNIT_ASSERT_DESC( GlxErrorManager::HasError(iItem1), 
        "iItem1 has no errors attached");

    // HasError should still report true for iItem2, even though all the errors have expired
    EUNIT_ASSERT_DESC( GlxErrorManager::HasError(iItem2), 
        "iItem2 has no errors attached");
    
    // But checking HasAttributeErrorL shouldn't find any
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaGeneralThumbnail1), 
        "iItem2 has an error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaGeneralDate), 
        "iItem2 has an error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaGeneralSize), 
        "iItem2 has an error on KMPXMediaGeneralSize");

    // Now we clean iItem1 and iItem2
    attrSet.Reset();            
    attrSet.AppendL(KMPXMediaGeneralThumbnail1);
    attrSet.AppendL(KMPXMediaGeneralDate);
    attrSet.AppendL(KMPXMediaGeneralSize);

    GlxErrorManager::ClearExpiredAndUnusedErrorsL(*iItem1, attrSet);
    GlxErrorManager::ClearExpiredAndUnusedErrorsL(*iItem2, attrSet);
       
    // HasError should report true for iItem1, since there is still an outstanding error
    EUNIT_ASSERT_DESC( GlxErrorManager::HasError(iItem1), 
        "iItem1 has no errors attached");

    // HasError should now report false for iItem2
    EUNIT_ASSERT_DESC( !GlxErrorManager::HasError(iItem2), 
        "iItem2 has uncleared errors attached");

    // Check all errors
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralThumbnail1), 
        "iItem1 has an error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralDate), 
        "iItem1 has no error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaGeneralSize), 
        "iItem1 has an error on KMPXMediaGeneralSize");
    
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaGeneralThumbnail1), 
        "iItem2 has an error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaGeneralDate), 
        "iItem2 has an error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaGeneralSize), 
        "iItem2 has an error on KMPXMediaGeneralSize");

    // Clean unused errors on iItem1
    attrSet.Reset();
    attrSet.AppendL(KMPXMediaGeneralTitle);
    GlxErrorManager::ClearExpiredAndUnusedErrorsL(*iItem1, attrSet);

    // HasError should now report false for iItem1
    EUNIT_ASSERT_DESC( !GlxErrorManager::HasError(iItem1), 
        "iItem1 has unused errors attached");


    // Now try ClearExpiredAndUnusedErrorsL with a smaller set of attributes in use
    attrSet.Reset();            
    attrSet.AppendL(KMPXMediaGeneralThumbnail1);
    attrSet.AppendL(KMPXMediaGeneralDate);
    attrSet.AppendL(KMPXMediaGeneralSize);
    GlxErrorManager::SetAttributeErrorL(iItem4, attrSet, KErrGeneral);

    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralThumbnail1), 
        "iItem4 has wrong error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralDate), 
        "iItem4 has wrong error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralSize), 
        "iItem4 has wrong error on KMPXMediaGeneralSize");

    attrSet.Reset();            
    attrSet.AppendL(KMPXMediaGeneralSize);
    GlxErrorManager::ClearExpiredAndUnusedErrorsL(*iItem4, attrSet);

    // HasError should report true for iItem4, since there is still an error
    EUNIT_ASSERT_DESC( GlxErrorManager::HasError(iItem4), 
        "iItem4 has no errors attached");

    // Only the KMPXMediaGeneralSize error should remain
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralThumbnail1), 
        "iItem4 has an error on KMPXMediaGeneralThumbnail1");
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralDate), 
        "iItem4 has an error on KMPXMediaGeneralDate");
    EUNIT_ASSERT_DESC( KErrGeneral == GlxErrorManager::HasAttributeErrorL(iItem4, KMPXMediaGeneralSize), 
        "iItem4 has wrong error on KMPXMediaGeneralSize");

    // Clear expired and unused errors on item with no errors
    EUNIT_ASSERT_DESC( !GlxErrorManager::HasError(iItem5), 
        "iItem5 has some errors attached");

    attrSet.Reset();            
    attrSet.AppendL(KMPXMediaGeneralThumbnail1);
    GlxErrorManager::ClearExpiredAndUnusedErrorsL(*iItem5, attrSet);

    EUNIT_ASSERT_DESC( !GlxErrorManager::HasError(iItem5), 
        "iItem5 has some errors attached");

    // Clear errors on item with permanent errors
    attrSet.Reset();
    attrSet.AppendL(KMPXMediaGeneralThumbnail1);
    GlxErrorManager::ClearExpiredAndUnusedErrorsL(*iItem6, attrSet);

    EUNIT_ASSERT_DESC( GlxErrorManager::HasError(iItem6), 
        "iItem6 has no errors attached");

    EUNIT_ASSERT_DESC( KErrNotSupported == GlxErrorManager::HasAttributeErrorL(iItem6, KMPXMediaGeneralThumbnail1), 
        "iItem6 has wrong error on KMPXMediaGeneralThumbnail1");

    // Clear unused errors which are permanent
    attrSet.Reset();            
    attrSet.AppendL(KMPXMediaGeneralTitle);
    GlxErrorManager::ClearExpiredAndUnusedErrorsL(*iItem6, attrSet);

    EUNIT_ASSERT_DESC( !GlxErrorManager::HasError(iItem6), 
        "iItem6 has some errors attached");

    CleanupStack::PopAndDestroy(&attrSet);
    }

void ut_glxerrormanager::T_Test6L(  )
    {
    // Check for attribute errors on null item
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(NULL, KMPXMediaIdGeneral), 
        "Null item has an error on KMPXMediaIdGeneral attributes");
    
    // Check for KMPXMediaIdGeneral attribute errors on item with no errors
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem1, KMPXMediaIdGeneral), 
        "iItem1 has an error on KMPXMediaIdGeneral attributes");

    // Check for KGlxMediaIdGeneral attribute errors on item with errors
    EUNIT_ASSERT_DESC( KErrNone == GlxErrorManager::HasAttributeErrorL(iItem2, KGlxMediaIdGeneral), 
        "iItem2 has an error on KGlxMediaIdGeneral attributes");

    // Check for KMPXMediaIdGeneral attribute errors on item with errors
    EUNIT_ASSERT_DESC( KErrNone != GlxErrorManager::HasAttributeErrorL(iItem2, KMPXMediaIdGeneral), 
        "iItem2 has no error on KMPXMediaIdGeneral attributes");    

    // Check for KMPXMediaIdGeneral attribute errors on item with permanent errors
    EUNIT_ASSERT_DESC( KErrNone != GlxErrorManager::HasAttributeErrorL(iItem6, KMPXMediaIdGeneral), 
        "iItem6 has no error on KMPXMediaIdGeneral attributes");    
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    ut_glxerrormanager,
    "Testing GlxErrorManager utility class.",
    "UNIT" )

EUNIT_TEST(
    "HasAttributeError_1L - Test1",
    "GlxErrorManager",
    "HasAttributeError_1L",
    "FUNCTIONALITY",
    SetupL, T_Test1L, Teardown)
    
EUNIT_TEST(
    "SetAttributeErrorL - Test2",
    "GlxErrorManager",
    "SetAttributeErrorL",
    "FUNCTIONALITY",
    SetupL, T_Test2L, Teardown)
    
EUNIT_TEST(
    "HasError - Test3",
    "GlxErrorManager",
    "HasError",
    "FUNCTIONALITY",
    SetupL, T_Test3L, Teardown)
    
EUNIT_TEST(
    "ErrorAttribute - Test4",
    "GlxErrorManager",
    "ErrorAttribute",
    "FUNCTIONALITY",
    SetupL, T_Test4L, Teardown)
    
EUNIT_TEST(
    "ClearExpiredAndUnusedErrorsL - Test5",
    "GlxErrorManager",
    "ClearExpiredAndUnusedErrorsL",
    "FUNCTIONALITY",
    SetupL, T_Test5L, Teardown)

EUNIT_TEST(
    "HasAttributeError_2L - Test6",
    "GlxErrorManager",
    "HasAttributeError_2L",
    "FUNCTIONALITY",
    SetupL, T_Test6L, Teardown)
    

EUNIT_END_TEST_TABLE

//  END OF FILE
