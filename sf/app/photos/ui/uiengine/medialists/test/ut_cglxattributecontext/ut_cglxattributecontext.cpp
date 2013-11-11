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
* Description:   CGlxAttributeContext unit tests
*
*/



//  CLASS HEADER
#include "UT_CGlxAttributeContext.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>

#include <e32std.h>

#include <mpxmediageneraldefs.h>

#include <glxdrmutility.h>

//  INTERNAL INCLUDES
#include "glxattributecontext.h"

const TInt KNumberOfMedia = 9;
const TInt KFrontOffset = 2;
const TInt KRearOffset = 2;
const TInt KItemsInRange = 1 + KFrontOffset + KRearOffset;
const TInt KOneItem = 1;
const TInt KGranularityLessThanRange = 2;
const TInt KGranularityGreaterThanRange = KItemsInRange + 2;

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
UT_CGlxAttributeContext* UT_CGlxAttributeContext::NewL()
    {
    UT_CGlxAttributeContext* self = UT_CGlxAttributeContext::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CGlxAttributeContext* UT_CGlxAttributeContext::NewLC()
    {
    UT_CGlxAttributeContext* self = new( ELeave ) UT_CGlxAttributeContext();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CGlxAttributeContext::~UT_CGlxAttributeContext()
    {
    }

// Default constructor
UT_CGlxAttributeContext::UT_CGlxAttributeContext()
    {
    }

// Second phase construct
void UT_CGlxAttributeContext::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CGlxAttributeContext::SetupL(  )
    {
    iAttributeContext = CGlxDefaultAttributeContext::NewL();
    }

void UT_CGlxAttributeContext::Teardown(  )
    {
    delete iAttributeContext;
    iAttributeContext = NULL;
    }

void UT_CGlxAttributeContext::UT_CGlxAttributeContext_CGlxAttributeContextL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxAttributeContext::UT_CGlxAttributeContext_AddAttributeLL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxAttributeContext::UT_CGlxAttributeContext_RemoveAttributeL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxAttributeContext::UT_CGlxAttributeContext_AllAttributesLL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxAttributeContext::UT_CGlxAttributeContext_AttributeRequestLL(  )
    {
    // Setup test
    SetupAttributeRequestTestL();

    FetchItems_NoMediaL();
    FetchOneItem_IndexZeroL();
    FetchOneItem_IndexOneL();
    FetchOneItem_IndexTwoL();
    FetchOneItem_IndexThreeL();
    FetchOneItem_IndexFourL();
    FetchItems_GranularityLessThanRange_IndexZeroL();
    FetchItems_GranularityLessThanRange_IndexOneL();
    FetchItems_GranularityLessThanRange_IndexTwoL();
    FetchItems_GranularityLessThanRange_IndexThreeL();
    FetchItems_GranularityLessThanRange_IndexFourL();
    FetchItems_GranularityLessThanRange_IndexFiveL();
    FetchItems_GranularityGreaterThanRangeL();
    }

void UT_CGlxAttributeContext::SetupAttributeRequestTestL()
    {
    // Initial offsets, rear is 2 and front is 2
    EUNIT_PRINT(_L("Initial offsets: rear is 2 and front is 2"));
    iAttributeContext->SetRangeOffsets(KRearOffset, KFrontOffset);

    // Initial attributes to fetch
    EUNIT_PRINT(_L("Initial attributes: Title and date"));
    iAttributeContext->AddAttributeL(KMPXMediaGeneralTitle);
    iAttributeContext->AddAttributeL(KMPXMediaGeneralDate);
    }

void UT_CGlxAttributeContext::FetchItems_NoMediaL()
    {
    // Fetch items with empty media list
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(0);
    CleanupStack::PushL(mediaList);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with empty media list"));
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check no item indices returned"));
    EUNIT_ASSERT(itemIndices.Count() == 0);

    EUNIT_PRINT(_L("Check no attributes returned"));
    EUNIT_ASSERT(attributes.Count() == 0);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchOneItem_IndexZeroL()
    {
    // Fetch 1 item with focus on index 0
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 0);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 0: granularity is 1"));
    iAttributeContext->SetGranularity(KOneItem);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 1 item index is returned: Item index is 0"));
    EUNIT_ASSERT((itemIndices.Count() == KOneItem) && itemIndices[0] == 0);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 2 attributes returned: Title and date"));
    EUNIT_ASSERT((attributes.Count() == 2) && titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchOneItem_IndexOneL()
    {
    // Fetch 1 item with focus on index 1
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 1);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 1: granularity is 1"));
    iAttributeContext->SetGranularity(KOneItem);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 1 item index is returned: Item index is 1"));
    EUNIT_ASSERT((itemIndices.Count() == KOneItem) && itemIndices[0] == 1);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 2 attributes returned: Title and date"));
    EUNIT_ASSERT((attributes.Count() == 2) && titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchOneItem_IndexTwoL()
    {
    // Fetch 1 item with focus on index 2
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 2);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 2: granularity is 1"));
    iAttributeContext->SetGranularity(KOneItem);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 1 item index is returned: Item index is 2"));
    EUNIT_ASSERT((itemIndices.Count() == KOneItem) && itemIndices[0] == 2);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 1 attribute returned: Date"));
    EUNIT_ASSERT((attributes.Count() == 1) && !titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchOneItem_IndexThreeL()
    {
    // Fetch 1 item with focus on index 3
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 3);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 3: granularity is 1"));
    iAttributeContext->SetGranularity(KOneItem);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 1 item index is returned: Item index is 3"));
    EUNIT_ASSERT((itemIndices.Count() == KOneItem) && itemIndices[0] == 3);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 1 attribute returned: Title"));
    EUNIT_ASSERT((attributes.Count() == 1) && titleAttributeFound && !dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchOneItem_IndexFourL()
    {
    // Fetch 1 item with focus on index 4
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 4);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 4: granularity is 1"));
    iAttributeContext->SetGranularity(KOneItem);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 1 item index is returned: Item index is 5"));
    EUNIT_ASSERT((itemIndices.Count() == KOneItem) && itemIndices[0] == 5);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 2 attributes returned: Title and date"));
    EUNIT_ASSERT((attributes.Count() == 2) && titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchItems_GranularityLessThanRange_IndexZeroL()
    {
    // Fetch items with focus on index 0 (granularity less than range)
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 0);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 0: granularity is 2"));
    iAttributeContext->SetGranularity(KGranularityLessThanRange);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 2 item indices are returned: Item indices 0 and 1"));
    EUNIT_ASSERT((itemIndices.Count() == KGranularityLessThanRange) && itemIndices[0] == 0 && itemIndices[1] == 1);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 2 attributes returned: Title and date"));
    EUNIT_ASSERT((attributes.Count() == 2) && titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchItems_GranularityLessThanRange_IndexOneL()
    {
    // Fetch items with focus on index 1 (granularity less than range)
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 1);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 1: granularity is 2"));
    iAttributeContext->SetGranularity(KGranularityLessThanRange);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 2 item indices are returned: Item indices 1 and 2"));
    EUNIT_ASSERT((itemIndices.Count() == KGranularityLessThanRange) && itemIndices[0] == 1 && itemIndices[1] == 2);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 2 attributes returned: Title and date"));
    EUNIT_ASSERT((attributes.Count() == 2) && titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchItems_GranularityLessThanRange_IndexTwoL()
    {
    // Fetch items with focus on index 2 (granularity less than range)
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 2);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 2: granularity is 2"));
    iAttributeContext->SetGranularity(KGranularityLessThanRange);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 2 item indices are returned: Item indices 2 and 3"));
    EUNIT_ASSERT((itemIndices.Count() == KGranularityLessThanRange) && itemIndices[0] == 2 && itemIndices[1] == 3);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 2 attributes returned: Title and date"));
    EUNIT_ASSERT((attributes.Count() == 2) && titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchItems_GranularityLessThanRange_IndexThreeL()
    {
    // Fetch items with focus on index 3 (granularity less than range)
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 3);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 3: granularity is 2"));
    iAttributeContext->SetGranularity(KGranularityLessThanRange);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 2 item indices are returned: Item indices 3 and 2"));
    EUNIT_ASSERT((itemIndices.Count() == KGranularityLessThanRange) && itemIndices[0] == 3 && itemIndices[1] == 2);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 2 attributes returned: Title and date"));
    EUNIT_ASSERT((attributes.Count() == 2) && titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchItems_GranularityLessThanRange_IndexFourL()
    {
    // Fetch items with focus on index 4 (granularity less than range)
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 4);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 4: granularity is 2"));
    iAttributeContext->SetGranularity(KGranularityLessThanRange);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 2 item indices are returned: Item indices 5 and 3"));
    EUNIT_ASSERT((itemIndices.Count() == KGranularityLessThanRange) && itemIndices[0] == 5 && itemIndices[1] == 3);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 2 attributes returned: Title and date"));
    EUNIT_ASSERT((attributes.Count() == 2) && titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchItems_GranularityLessThanRange_IndexFiveL()
    {
    // Fetch items with focus on index 5 (granularity less than range)
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 5);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 5: granularity is 2"));
    iAttributeContext->SetGranularity(KGranularityLessThanRange);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 2 item indices are returned: Item indices 5 and 6"));
    EUNIT_ASSERT((itemIndices.Count() == KGranularityLessThanRange) && itemIndices[0] == 5 && itemIndices[1] == 6);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 2 attributes returned: Title and date"));
    EUNIT_ASSERT((attributes.Count() == 2) && titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

void UT_CGlxAttributeContext::FetchItems_GranularityGreaterThanRangeL()
    {
    // Fetch items with focus on index 4 (granularity greater than range)
    CGlxMediaListTest* mediaList = CGlxMediaListTest::NewL(KNumberOfMedia);
    CleanupStack::PushL(mediaList);

    mediaList->SetFocusL(NGlxListDefs::EAbsolute, 4);

    RArray<TInt> itemIndices;
    CleanupClosePushL(itemIndices);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    CMPXAttributeSpecs* attributeSpecs = NULL;

    EUNIT_PRINT(_L("Fetch items with focus on index 4: granularity is 7"));
    iAttributeContext->SetGranularity(KGranularityGreaterThanRange);
    iAttributeContext->AttributeRequestL(mediaList, itemIndices, attributes, attributeSpecs);

    EUNIT_PRINT(_L("Check 4 item indices are returned: Item indices 5, 3, 6 and 2"));
    EUNIT_ASSERT((itemIndices.Count() == 4) && itemIndices[0] == 5 && itemIndices[1] == 3 && itemIndices[2] == 6 && itemIndices[3] == 2);

    TBool titleAttributeFound = FindAttribute(attributes, KMPXMediaGeneralTitle);
    TBool dateAttributeFound = FindAttribute(attributes, KMPXMediaGeneralDate);

    EUNIT_PRINT(_L("Check 2 attributes returned: Title and date"));
    EUNIT_ASSERT((attributes.Count() == 2) && titleAttributeFound && dateAttributeFound);

    delete attributeSpecs;
    attributeSpecs = NULL;

    CleanupStack::Pop();
    attributes.Close();

    CleanupStack::Pop();
    itemIndices.Close();

    CleanupStack::PopAndDestroy();
    }

TBool UT_CGlxAttributeContext::FindAttribute(RArray<TMPXAttribute>& aAttributes, const TMPXAttribute& aAttribute)
    {
    TIdentityRelation<TMPXAttribute> match(&TMPXAttribute::Match);
    TInt index = aAttributes.Find(aAttribute, match);
    if (index == KErrNotFound)
        {
        return EFalse;
        }

    return ETrue;
    }

UT_CGlxAttributeContext::CGlxMediaListTest* UT_CGlxAttributeContext::CGlxMediaListTest::NewL(TUint aNumberOfMedia)
    {
    CGlxMediaListTest* self = new (ELeave) CGlxMediaListTest();
    CleanupStack::PushL(self);
    self->ConstructL(aNumberOfMedia);
    CleanupStack::Pop();
    return self;
    }

UT_CGlxAttributeContext::CGlxMediaListTest::~CGlxMediaListTest()
    {
    for (TInt count = 0; count < iMedias.Count(); ++count)
        {
        delete iMedias[count];
        }

    iMedias.Close();
    iItems.Close();
    iSelectedItems.Close();
    }

UT_CGlxAttributeContext::CGlxMediaListTest::CGlxMediaListTest() :
        iFocusIndex(KErrNotFound), iMedias(), iItems(), iSelectedItems()
    {
    }

void UT_CGlxAttributeContext::CGlxMediaListTest::ConstructL(TUint aNumberOfMedia)
    {
    for (TInt count = 0; count < aNumberOfMedia; ++count)
        {
        CreateMediaL(count + 1);
        }
    }

void UT_CGlxAttributeContext::CGlxMediaListTest::CreateMediaL(TUint aMediaId)
    {
    TGlxMediaId id(aMediaId);
    CGlxMedia* cGlxMedia = NULL;

    TInt modulo = (aMediaId - 1) % 5;
    if (modulo > 0)
        {
        cGlxMedia = new (ELeave) CGlxMedia(id);
        CleanupStack::PushL(cGlxMedia);

        if (modulo == 1)
            {
            // No attributes
            }
        else if (modulo == 2)
            {
            cGlxMedia->SetTextValueL(KMPXMediaGeneralTitle, _L("Title"));
            }
        else if (modulo == 3)
            {
            cGlxMedia->SetTextValueL(KMPXMediaGeneralDate, _L("Date"));
            }
        else if (modulo == 4)
            {
            cGlxMedia->SetTextValueL(KMPXMediaGeneralTitle, _L("Title"));
            cGlxMedia->SetTextValueL(KMPXMediaGeneralDate, _L("Date"));
            }

        iMedias.AppendL(cGlxMedia);
        CleanupStack::Pop();
        }

    TGlxMedia tGlxMedia(id, cGlxMedia);
    iItems.AppendL(tGlxMedia);
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CGlxAttributeContext,
    "Unit tests for CGlxAttributeContext",
    "UNIT" )
/*
EUNIT_TEST(
    "CGlxAttributeContext - test ",
    "CGlxAttributeContext",
    "CGlxAttributeContext",
    "FUNCTIONALITY",
    SetupL, UT_CGlxAttributeContext_CGlxAttributeContextL, Teardown)

EUNIT_TEST(
    "AddAttributeL - test ",
    "CGlxAttributeContext",
    "AddAttributeL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxAttributeContext_AddAttributeLL, Teardown)

EUNIT_TEST(
    "RemoveAttribute - test ",
    "CGlxAttributeContext",
    "RemoveAttribute",
    "FUNCTIONALITY",
    SetupL, UT_CGlxAttributeContext_RemoveAttributeL, Teardown)

EUNIT_TEST(
    "AllAttributesL - test ",
    "CGlxAttributeContext",
    "AllAttributesL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxAttributeContext_AllAttributesLL, Teardown)
*/
EUNIT_TEST(
    "AttributeRequestL - test ",
    "CGlxAttributeContext",
    "AttributeRequestL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxAttributeContext_AttributeRequestLL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
