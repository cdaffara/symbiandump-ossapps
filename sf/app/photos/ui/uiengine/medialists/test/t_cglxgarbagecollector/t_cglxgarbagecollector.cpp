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
* Description:   CGlxGarbageCollector unit tests
*
*/



//  CLASS HEADER
#include "t_cglxgarbagecollector.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <mpxattribute.h>
#include <mpxmediageneraldefs.h>

#include <glxmediageneraldefs.h>

//  INTERNAL INCLUDES
#include "glxerrormanager.h"
#include "glxfetcherrorarray.h"
#include "glxgarbagecollector.h"
#include "glxmedia.h"

#include "t_cachetesthelpers.h"

T_CGlxGarbageCollector* T_CGlxGarbageCollector::iGarbageCollectorTest = NULL;

const TInt KIdSpaceId1 = 1;
const TInt KIdSpaceId2 = 2;
const TInt KIdSpaceId3 = 3;

const TInt KCache1Media1Id = 1;
const TInt KCache1Media2Id = 2;
const TInt KCache1Media3Id = 3;

const TInt KCache2Media1Id = 1;
const TInt KCache2Media2Id = 2;

const TInt KCache3Media1Id = 1;
const TInt KCache3Media2Id = 2;
const TInt KCache3Media3Id = 3;
const TInt KCache3Media4Id = 4;
const TInt KCache3Media5Id = 5;
const TInt KCache3Media6Id = 6;
const TInt KCache3Media7Id = 7;

_LIT(KCache1Media1Title, "Captured");
//_LIT(KCache1Media2Title, "Albums");
//_LIT(KCache1Media3Title, "All");

_LIT(KCache1Media1SubTitle, "0 items");
//_LIT(KCache1Media2SubTitle, "0 albums");
//_LIT(KCache1Media3SubTitle, "7 items");

_LIT(KCache2Media1Title, "Favourites");
_LIT(KCache2Media2Title, "Print later");

_LIT(KCache2Media1SubTitle, "0 items");
_LIT(KCache2Media2SubTitle, "0 items");

_LIT(KCache3Media1Title, "Spring");
//_LIT(KCache3Media2Title, "Summer");
_LIT(KCache3Media3Title, "Autumn");
_LIT(KCache3Media4Title, "Winter");
_LIT(KCache3Media5Title, "Sun");
//_LIT(KCache3Media6Title, "Moon");
//_LIT(KCache3Media7Title, "Stars");

const TInt KCache3Media1Size = 8;
const TInt KCache3Media2Size = 16;
const TInt KCache3Media3Size = 32;
//const TInt KCache3Media4Size = 64;
//const TInt KCache3Media5Size = 128;
//const TInt KCache3Media6Size = 256;
//const TInt KCache3Media7Size = 512;

// This value must be the same as in the garbage collector
const TInt KItemsScannedPerGCRound = 10;

const TInt KErrorExpiryPeriod = 30;
const TInt KTimerCallbackPeriod1 = 5;
const TInt KTimerCallbackPeriod2 = 35;
const TInt KMicrosecondsInSeconds = 1000000;

// These values are for CleanupCaches test
const TInt KItemsToScan = 10;
const TInt KCache1ItemCount = 3;
const TInt KCache2ItemCount = 2;
const TInt KCache3ItemCount = 7;

// CONSTRUCTION
T_CGlxGarbageCollector* T_CGlxGarbageCollector::NewL()
    {
    T_CGlxGarbageCollector* self = T_CGlxGarbageCollector::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CGlxGarbageCollector* T_CGlxGarbageCollector::NewLC()
    {
    T_CGlxGarbageCollector* self = new( ELeave ) T_CGlxGarbageCollector();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CGlxGarbageCollector::~T_CGlxGarbageCollector()
    {
    }

// Default constructor
T_CGlxGarbageCollector::T_CGlxGarbageCollector()
    {
    }

// Second phase construct
void T_CGlxGarbageCollector::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void T_CGlxGarbageCollector::SetupL(  )
    {
    SetupCachesL();

    iCGlxGarbageCollector = CGlxGarbageCollector::NewL(iCaches);

    // Set this variable to 'this' in the tests if they need to know
    // if idlecallbacks have been called on garbage collector
    iGarbageCollectorTest = NULL;
    }
    

void T_CGlxGarbageCollector::Teardown(  )
    {
    iCaches.ResetAndDestroy();

    delete iCGlxGarbageCollector;
    iCGlxGarbageCollector = NULL;
    }
    

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_Cleanup_SubTest1L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title and Subtitle attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    EUNIT_PRINT(_L("System Item attribute wanted by mediauser2"));
    attributes.Reset();
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser2 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser2);

    EUNIT_PRINT(_L("Media1 in cache2 used by mediauser2"));
    CGlxCache* cache2 = iCaches[1];
    AddMediaUserToMediaL(*cache2, 0, *mediaUser2);

    EUNIT_PRINT(_L("Thumbnail attribute wanted by mediauser3"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralThumbnail1);

    CGlxMediaUserTest* mediaUser3 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser3);

    EUNIT_PRINT(_L("Title attribute wanted by mediauser4"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralTitle);

    CGlxMediaUserTest* mediaUser4 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser4);

    EUNIT_PRINT(_L("All media in cache3 used by mediauser3 and mediauser4"));
    CGlxCache* cache3 = iCaches[2];
    AddMediaUserToCacheMediasL(*cache3, *mediaUser3);
    AddMediaUserToCacheMediasL(*cache3, *mediaUser4);

    // Cleanup
    EUNIT_PRINT(_L("Cleanup"));
    iCGlxGarbageCollector->Cleanup();

    // Timer callback will stop the wait loop we are about to start
    CTimerTest* timer = CTimerTest::NewL(this);
    CleanupStack::PushL(timer);

    timer->StartTimer(KTimerCallbackPeriod1);

    // Start the wait loop
    iWait.Start();

    // Wait loop stopped
    CleanupStack::PopAndDestroy(timer);

    // Garbage collector should have cleaned up once
    EUNIT_PRINT(_L("Check remaining media in cache1"));
    EUNIT_ASSERT_DESC(cache1->Count() == 3, "Remaining media should be 3");

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    CheckCache1MediaAttributes(*media1Cache1, 2, ETrue, ETrue, EFalse);
    CheckCache1MediaErrors(*media1Cache1, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    CheckCache1MediaAttributes(*media2Cache1, 1, EFalse, EFalse, EFalse, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, ETrue, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache1"));
    CGlxMedia* media3Cache1 = cache1Helper.ItemPool()[2];
    CheckCache1MediaAttributes(*media3Cache1, 1, EFalse, EFalse, EFalse, ETrue);
    CheckCache1MediaErrors(*media3Cache1, ETrue, ETrue, EFalse, EFalse);

    EUNIT_PRINT(_L("Check remaining media in cache2"));
    EUNIT_ASSERT_DESC(cache2->Count() == 1, "Remaining media should be 1");

    // Create helper class
    T_CacheTestHelpers cache2Helper(cache2);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache2"));
    CGlxMedia* media1Cache2 = cache2Helper.ItemPool()[0];
    CheckCache2MediaAttributes(*media1Cache2, 1, EFalse, EFalse, ETrue);
    CheckCache2MediaErrors(*media1Cache2, EFalse);

    EUNIT_PRINT(_L("Check remaining media in cache3"));
    EUNIT_ASSERT_DESC(cache3->Count() == 4, "Remaining media should be 4");

    // Create helper class
    T_CacheTestHelpers cache3Helper(cache3);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache3"));
    CGlxMedia* media1Cache3 = cache3Helper.ItemPool()[0];
    CheckCache3MediaAttributes(*media1Cache3, 2, ETrue, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media1Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache3"));
    CGlxMedia* media2Cache3 = cache3Helper.ItemPool()[1];
    CheckCache3MediaAttributes(*media2Cache3, 1, EFalse, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media2Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache3"));
    CGlxMedia* media3Cache3 = cache3Helper.ItemPool()[2];
    CheckCache3MediaAttributes(*media3Cache3, 1, EFalse, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media3Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media4 in cache3"));
    CGlxMedia* media4Cache3 = cache3Helper.ItemPool()[3];
    CheckCache3MediaAttributes(*media4Cache3, 2, EFalse, ETrue, EFalse, EFalse, ETrue);
    CheckCache3MediaErrors(*media4Cache3, ETrue, ETrue, EFalse, EFalse, EFalse);

    CleanupStack::PopAndDestroy(mediaUser4);
    CleanupStack::PopAndDestroy(mediaUser3);
    CleanupStack::PopAndDestroy(mediaUser2);
    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_Cleanup_SubTest2L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title and Subtitle attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    EUNIT_PRINT(_L("System Item attribute wanted by mediauser2"));
    attributes.Reset();
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser2 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser2);

    EUNIT_PRINT(_L("Media1 in cache2 used by mediauser2"));
    CGlxCache* cache2 = iCaches[1];
    AddMediaUserToMediaL(*cache2, 0, *mediaUser2);

    EUNIT_PRINT(_L("Thumbnail attribute wanted by mediauser3"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralThumbnail1);

    CGlxMediaUserTest* mediaUser3 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser3);

    EUNIT_PRINT(_L("Title attribute wanted by mediauser4"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralTitle);

    CGlxMediaUserTest* mediaUser4 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser4);

    EUNIT_PRINT(_L("All media in cache3 used by mediauser3 and mediauser4"));
    CGlxCache* cache3 = iCaches[2];
    AddMediaUserToCacheMediasL(*cache3, *mediaUser3);
    AddMediaUserToCacheMediasL(*cache3, *mediaUser4);

    // Cleanup
    EUNIT_PRINT(_L("Cleanup"));
    iCGlxGarbageCollector->Cleanup();

    iGarbageCollectorTest = this;

    // Timer callback will stop the wait loop we are about to start
    CTimerTest* timer = CTimerTest::NewL(this);
    CleanupStack::PushL(timer);

    timer->StartTimer(KTimerCallbackPeriod1);

    // Start the wait loop
    iWait.Start();

    // Wait loop stopped
    CleanupStack::PopAndDestroy(timer);

    // Garbage collector should have cleaned up twice
    // Second round caused by updated media
    EUNIT_PRINT(_L("Check remaining media in cache1"));
    EUNIT_ASSERT_DESC(cache1->Count() == 3, "Remaining media should be 3");

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    CheckCache1MediaAttributes(*media1Cache1, 2, ETrue, ETrue, EFalse);
    CheckCache1MediaErrors(*media1Cache1, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    CheckCache1MediaAttributes(*media2Cache1, 1, EFalse, EFalse, EFalse, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, ETrue, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache1"));
    CGlxMedia* media3Cache1 = cache1Helper.ItemPool()[2];
    CheckCache1MediaAttributes(*media3Cache1, 1, EFalse, EFalse, EFalse, ETrue);
    CheckCache1MediaErrors(*media3Cache1, ETrue, ETrue, EFalse, EFalse);

    EUNIT_PRINT(_L("Check remaining media in cache2"));
    EUNIT_ASSERT_DESC(cache2->Count() == 1, "Remaining media should be 1");

    // Create helper class
    T_CacheTestHelpers cache2Helper(cache2);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache2"));
    CGlxMedia* media1Cache2 = cache2Helper.ItemPool()[0];
    CheckCache2MediaAttributes(*media1Cache2, 1, EFalse, EFalse, ETrue);
    CheckCache2MediaErrors(*media1Cache2, EFalse);

    EUNIT_PRINT(_L("Check remaining media in cache3"));
    EUNIT_ASSERT_DESC(cache3->Count() == 4, "Remaining media should be 4");

    // Create helper class
    T_CacheTestHelpers cache3Helper(cache3);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache3"));
    CGlxMedia* media1Cache3 = cache3Helper.ItemPool()[0];
    CheckCache3MediaAttributes(*media1Cache3, 2, ETrue, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media1Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache3"));
    CGlxMedia* media2Cache3 = cache3Helper.ItemPool()[1];
    CheckCache3MediaAttributes(*media2Cache3, 1, EFalse, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media2Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache3"));
    CGlxMedia* media3Cache3 = cache3Helper.ItemPool()[2];
    CheckCache3MediaAttributes(*media3Cache3, 1, EFalse, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media3Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media4 in cache3"));
    CGlxMedia* media4Cache3 = cache3Helper.ItemPool()[3];
    CheckCache3MediaAttributes(*media4Cache3, 2, EFalse, ETrue, EFalse, EFalse, ETrue);
    CheckCache3MediaErrors(*media4Cache3, ETrue, ETrue, EFalse, EFalse, EFalse);

    CleanupStack::PopAndDestroy(mediaUser4);
    CleanupStack::PopAndDestroy(mediaUser3);
    CleanupStack::PopAndDestroy(mediaUser2);
    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_Cleanup_SubTest3L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title and Subtitle attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    EUNIT_PRINT(_L("System Item attribute wanted by mediauser2"));
    attributes.Reset();
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser2 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser2);

    EUNIT_PRINT(_L("Media1 in cache2 used by mediauser2"));
    CGlxCache* cache2 = iCaches[1];
    AddMediaUserToMediaL(*cache2, 0, *mediaUser2);

    EUNIT_PRINT(_L("Thumbnail attribute wanted by mediauser3"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralThumbnail1);

    CGlxMediaUserTest* mediaUser3 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser3);

    EUNIT_PRINT(_L("Title attribute wanted by mediauser4"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralTitle);

    CGlxMediaUserTest* mediaUser4 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser4);

    EUNIT_PRINT(_L("All media in cache3 used by mediauser3 and mediauser4"));
    CGlxCache* cache3 = iCaches[2];
    AddMediaUserToCacheMediasL(*cache3, *mediaUser3);
    AddMediaUserToCacheMediasL(*cache3, *mediaUser4);

    // Cleanup
    EUNIT_PRINT(_L("Cleanup"));
    iCGlxGarbageCollector->Cleanup();

    // Timer callback will stop the wait loop we are about to start
    CTimerTest* timer = CTimerTest::NewL(this);
    CleanupStack::PushL(timer);

    timer->StartTimer(KTimerCallbackPeriod1);

    // Start the wait loop
    iWait.Start();

    // Wait loop stopped
    // Update media
    UpdateMediaL();

    // Timer callback will stop the wait loop we are about to start
    timer->StartTimer(KTimerCallbackPeriod1);

    // Start the wait loop
    iWait.Start();

    // Wait loop stopped
    CleanupStack::PopAndDestroy(timer);

    // Garbage collector should have cleaned up twice
    // Second round caused by updated media
    EUNIT_PRINT(_L("Check remaining media in cache1"));
    EUNIT_ASSERT_DESC(cache1->Count() == 3, "Remaining media should be 3");

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    CheckCache1MediaAttributes(*media1Cache1, 2, ETrue, ETrue, EFalse);
    CheckCache1MediaErrors(*media1Cache1, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    CheckCache1MediaAttributes(*media2Cache1, 1, EFalse, EFalse, EFalse, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, ETrue, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache1"));
    CGlxMedia* media3Cache1 = cache1Helper.ItemPool()[2];
    CheckCache1MediaAttributes(*media3Cache1, 1, EFalse, EFalse, EFalse, ETrue);
    CheckCache1MediaErrors(*media3Cache1, ETrue, ETrue, EFalse, EFalse);

    EUNIT_PRINT(_L("Check remaining media in cache2"));
    EUNIT_ASSERT_DESC(cache2->Count() == 1, "Remaining media should be 1");

    // Create helper class
    T_CacheTestHelpers cache2Helper(cache2);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache2"));
    CGlxMedia* media1Cache2 = cache2Helper.ItemPool()[0];
    CheckCache2MediaAttributes(*media1Cache2, 1, EFalse, EFalse, ETrue);
    CheckCache2MediaErrors(*media1Cache2, EFalse);

    EUNIT_PRINT(_L("Check remaining media in cache3"));
    EUNIT_ASSERT_DESC(cache3->Count() == 4, "Remaining media should be 4");

    // Create helper class
    T_CacheTestHelpers cache3Helper(cache3);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache3"));
    CGlxMedia* media1Cache3 = cache3Helper.ItemPool()[0];
    CheckCache3MediaAttributes(*media1Cache3, 2, ETrue, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media1Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache3"));
    CGlxMedia* media2Cache3 = cache3Helper.ItemPool()[1];
    CheckCache3MediaAttributes(*media2Cache3, 1, EFalse, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media2Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache3"));
    CGlxMedia* media3Cache3 = cache3Helper.ItemPool()[2];
    CheckCache3MediaAttributes(*media3Cache3, 1, EFalse, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media3Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media4 in cache3"));
    CGlxMedia* media4Cache3 = cache3Helper.ItemPool()[3];
    CheckCache3MediaAttributes(*media4Cache3, 2, EFalse, ETrue, EFalse, EFalse, ETrue);
    CheckCache3MediaErrors(*media4Cache3, ETrue, ETrue, EFalse, EFalse, EFalse);

    CleanupStack::PopAndDestroy(mediaUser4);
    CleanupStack::PopAndDestroy(mediaUser3);
    CleanupStack::PopAndDestroy(mediaUser2);
    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

// This test takes longer than 30 seconds
// Therefore, EUnit timeout should be set to greater than this
void T_CGlxGarbageCollector::T_CGlxGarbageCollector_Cleanup_SubTest4L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title and Subtitle attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    EUNIT_PRINT(_L("System Item attribute wanted by mediauser2"));
    attributes.Reset();
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser2 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser2);

    EUNIT_PRINT(_L("Media1 in cache2 used by mediauser2"));
    CGlxCache* cache2 = iCaches[1];
    AddMediaUserToMediaL(*cache2, 0, *mediaUser2);

    EUNIT_PRINT(_L("Thumbnail attribute wanted by mediauser3"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralThumbnail1);

    CGlxMediaUserTest* mediaUser3 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser3);

    EUNIT_PRINT(_L("Title attribute wanted by mediauser4"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralTitle);

    CGlxMediaUserTest* mediaUser4 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser4);

    EUNIT_PRINT(_L("All media in cache3 used by mediauser3 and mediauser4"));
    CGlxCache* cache3 = iCaches[2];
    AddMediaUserToCacheMediasL(*cache3, *mediaUser3);
    AddMediaUserToCacheMediasL(*cache3, *mediaUser4);

    // Cleanup
    EUNIT_PRINT(_L("Cleanup"));
    iCGlxGarbageCollector->Cleanup();

    // Timer callback will stop the wait loop we are about to start
    CTimerTest* timer = CTimerTest::NewL(this);
    CleanupStack::PushL(timer);

    timer->StartTimer(KTimerCallbackPeriod2);

    // Start the wait loop
    iWait.Start();

    // Wait loop stopped
    // Cleanup again
    EUNIT_PRINT(_L("Cleanup again"));
    iCGlxGarbageCollector->Cleanup();

    // Timer callback will stop the wait loop we are about to start
    timer->StartTimer(KTimerCallbackPeriod1);

    // Start the wait loop
    iWait.Start();

    // Wait loop stopped
    CleanupStack::PopAndDestroy(timer);

    EUNIT_PRINT(_L("Check remaining media in cache1"));
    EUNIT_ASSERT_DESC(cache1->Count() == 3, "Remaining media should be 3");

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Garbage collector should have cleaned up twice
    // Second round caused by temporary error timer callback
    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    CheckCache1MediaAttributes(*media1Cache1, 2, ETrue, ETrue, EFalse);
    CheckCache1MediaErrors(*media1Cache1, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    CheckCache1MediaAttributes(*media2Cache1, 1, EFalse, EFalse, EFalse, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, EFalse, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache1"));
    CGlxMedia* media3Cache1 = cache1Helper.ItemPool()[2];
    CheckCache1MediaAttributes(*media3Cache1, 1, EFalse, EFalse, EFalse, ETrue);
    CheckCache1MediaErrors(*media3Cache1, ETrue, ETrue, EFalse, EFalse);

    EUNIT_PRINT(_L("Check remaining media in cache2"));
    EUNIT_ASSERT_DESC(cache2->Count() == 1, "Remaining media should be 1");

    // Create helper class
    T_CacheTestHelpers cache2Helper(cache2);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache2"));
    CGlxMedia* media1Cache2 = cache2Helper.ItemPool()[0];
    CheckCache2MediaAttributes(*media1Cache2, 1, EFalse, EFalse, ETrue);
    CheckCache2MediaErrors(*media1Cache2, EFalse);

    EUNIT_PRINT(_L("Check remaining media in cache3"));
    EUNIT_ASSERT_DESC(cache3->Count() == 4, "Remaining media should be 4");

    // Create helper class
    T_CacheTestHelpers cache3Helper(cache3);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache3"));
    CGlxMedia* media1Cache3 = cache3Helper.ItemPool()[0];
    CheckCache3MediaAttributes(*media1Cache3, 2, ETrue, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media1Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache3"));
    CGlxMedia* media2Cache3 = cache3Helper.ItemPool()[1];
    CheckCache3MediaAttributes(*media2Cache3, 1, EFalse, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media2Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache3"));
    CGlxMedia* media3Cache3 = cache3Helper.ItemPool()[2];
    CheckCache3MediaAttributes(*media3Cache3, 1, EFalse, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media3Cache3, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media4 in cache3"));
    CGlxMedia* media4Cache3 = cache3Helper.ItemPool()[3];
    CheckCache3MediaAttributes(*media4Cache3, 1, EFalse, ETrue, EFalse, EFalse);
    CheckCache3MediaErrors(*media4Cache3, EFalse);

    CleanupStack::PopAndDestroy(mediaUser4);
    CleanupStack::PopAndDestroy(mediaUser3);
    CleanupStack::PopAndDestroy(mediaUser2);
    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupCachesL(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title and Subtitle attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    EUNIT_PRINT(_L("System Item attribute wanted by mediauser2"));
    attributes.Reset();
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser2 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser2);

    EUNIT_PRINT(_L("Media1 in cache2 used by mediauser2"));
    CGlxCache* cache2 = iCaches[1];
    AddMediaUserToMediaL(*cache2, 0, *mediaUser2);

    EUNIT_PRINT(_L("Thumbnail attribute wanted by mediauser3"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralThumbnail1);

    CGlxMediaUserTest* mediaUser3 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser3);

    EUNIT_PRINT(_L("Title attribute wanted by mediauser4"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralTitle);

    CGlxMediaUserTest* mediaUser4 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser4);

    EUNIT_PRINT(_L("All media in cache3 used by mediauser3 and mediauser4"));
    CGlxCache* cache3 = iCaches[2];
    AddMediaUserToCacheMediasL(*cache3, *mediaUser3);
    AddMediaUserToCacheMediasL(*cache3, *mediaUser4);

    // Create helper class
    T_CacheTestHelpers cache2Helper(cache2);

    // Keep original cache pointers
    CGlxMedia* oldMedia1Cache2 = cache2Helper.ItemPool()[0];

    // Create helper class
    T_CacheTestHelpers cache3Helper(cache3);

    // Keep original cache pointers
    CGlxMedia* oldMedia1Cache3 = cache3Helper.ItemPool()[0];
    CGlxMedia* oldMedia3Cache3 = cache3Helper.ItemPool()[2];
    CGlxMedia* oldMedia4Cache3 = cache3Helper.ItemPool()[3];
    CGlxMedia* oldMedia5Cache3 = cache3Helper.ItemPool()[4];
    CGlxMedia* oldMedia6Cache3 = cache3Helper.ItemPool()[5];
    CGlxMedia* oldMedia7Cache3 = cache3Helper.ItemPool()[6];

    // Cleanup all caches
    EUNIT_PRINT(_L("Cleanup all caches"));
    TBool scannedAllCaches = iCGlxGarbageCollector->CleanupCaches();

    // The tests from hereon in are based on KItemsToScan being 10
    // Tests only work on this basis

    // The tests from hereon in are based on KItemsScannedPerGCRound
    // being the same value contained in the garbage collector
    if (KItemsScannedPerGCRound <= KItemsToScan)
        {
        EUNIT_PRINT(_L("Check whether all caches scanned"));
        EUNIT_ASSERT_DESC(!scannedAllCaches, "Some caches shouldn't have been scanned");
        }

    if (KCache1ItemCount < KItemsScannedPerGCRound)
        {
        EUNIT_PRINT(_L("Check remaining media in cache1"));
        EUNIT_ASSERT_DESC(cache1->Count() == 3, "Remaining media should be 3");

        // Create helper class
        T_CacheTestHelpers cache1Helper(cache1);

        EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
        CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
        CheckCache1MediaAttributes(*media1Cache1, 2, ETrue, ETrue, EFalse);
        CheckCache1MediaErrors(*media1Cache1, EFalse);

        EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
        CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
        CheckCache1MediaAttributes(*media2Cache1, 1, EFalse, EFalse, EFalse, ETrue);
        CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, ETrue, EFalse);

        EUNIT_PRINT(_L("Check remaining attributes for media3 in cache1"));
        CGlxMedia* media3Cache1 = cache1Helper.ItemPool()[2];
        CheckCache1MediaAttributes(*media3Cache1, 1, EFalse, EFalse, EFalse, ETrue);
        CheckCache1MediaErrors(*media3Cache1, ETrue, ETrue, EFalse, EFalse);

        if (KCache1ItemCount + KCache2ItemCount < KItemsScannedPerGCRound)
            {
            EUNIT_PRINT(_L("Check remaining media in cache2"));
            EUNIT_ASSERT_DESC(cache2->Count() == 1, "Remaining media should be 1");

            CGlxMedia* media1Cache2 = cache2Helper.ItemPool()[0];
            EUNIT_ASSERT_DESC(media1Cache2 == oldMedia1Cache2, "New media1 in cache2 should be old media1");

            EUNIT_PRINT(_L("Check remaining attributes for media1 in cache2"));
            CheckCache2MediaAttributes(*media1Cache2, 1, EFalse, EFalse, ETrue);
            CheckCache2MediaErrors(*media1Cache2, EFalse);

            if (KItemsScannedPerGCRound == KItemsToScan)
                {
                EUNIT_PRINT(_L("Check remaining media in cache3"));
                EUNIT_ASSERT_DESC(cache3->Count() == 6, "Remaining media should be 6");

                CGlxMedia* media1Cache3 = cache3Helper.ItemPool()[0];
                CGlxMedia* media2Cache3 = cache3Helper.ItemPool()[1];
                CGlxMedia* media3Cache3 = cache3Helper.ItemPool()[2];
                CGlxMedia* media4Cache3 = cache3Helper.ItemPool()[3];
                CGlxMedia* media5Cache3 = cache3Helper.ItemPool()[4];
                CGlxMedia* media6Cache3 = cache3Helper.ItemPool()[5];
                EUNIT_ASSERT_DESC(media1Cache3 == oldMedia1Cache3, "New media1 in cache3 should be old media1");
                EUNIT_ASSERT_DESC(media2Cache3 == oldMedia3Cache3, "New media2 in cache3 should be old media3");
                EUNIT_ASSERT_DESC(media3Cache3 == oldMedia4Cache3, "New media3 in cache3 should be old media4");
                EUNIT_ASSERT_DESC(media4Cache3 == oldMedia5Cache3, "New media4 in cache3 should be old media5");
                EUNIT_ASSERT_DESC(media5Cache3 == oldMedia6Cache3, "New media5 in cache3 should be old media6");
                EUNIT_ASSERT_DESC(media6Cache3 == oldMedia7Cache3, "New media6 in cache3 should be old media7");

                EUNIT_PRINT(_L("Check remaining attributes for media1 in cache3"));
                CheckCache3MediaAttributes(*media1Cache3, 2, ETrue, ETrue, EFalse, EFalse);
                CheckCache3MediaErrors(*media1Cache3, EFalse);

                EUNIT_PRINT(_L("Check remaining attributes for media2 in cache3"));
                CheckCache3MediaAttributes(*media2Cache3, 1, EFalse, ETrue, EFalse, EFalse);
                CheckCache3MediaErrors(*media2Cache3, EFalse);

                EUNIT_PRINT(_L("Check remaining attributes for media3 in cache3"));
                CheckCache3MediaAttributes(*media3Cache3, 1, EFalse, ETrue, EFalse, EFalse);
                CheckCache3MediaErrors(*media3Cache3, EFalse);

                EUNIT_PRINT(_L("Check remaining attributes for media4 in cache3"));
                CheckCache3MediaAttributes(*media4Cache3, 2, EFalse, ETrue, EFalse, EFalse, ETrue);
                CheckCache3MediaErrors(*media4Cache3, ETrue, ETrue, EFalse, EFalse, EFalse);

                EUNIT_PRINT(_L("Check remaining attributes for media5 in cache3"));
                CheckCache3MediaAttributes(*media5Cache3, 1, EFalse, EFalse, EFalse, EFalse, ETrue);
                CheckCache3MediaErrors(*media5Cache3, ETrue, EFalse, EFalse, EFalse, EFalse);

                EUNIT_PRINT(_L("Check remaining attributes for media6 in cache3"));
                CheckCache3MediaAttributes(*media6Cache3, 1, EFalse, EFalse, EFalse, EFalse, ETrue);
                CheckCache3MediaErrors(*media6Cache3, ETrue, EFalse, EFalse, EFalse, ETrue);
                }
            }
        }

    CleanupStack::PopAndDestroy(mediaUser4);
    CleanupStack::PopAndDestroy(mediaUser3);
    CleanupStack::PopAndDestroy(mediaUser2);
    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupCache_SubTest1L(  )
    {
    // Cleanup cache1, scan for 0 items
    CGlxCache* cache1 = iCaches[0];
    EUNIT_PRINT(_L("Cleanup cache1, scan for 0 items"));
    TInt remainingScan = iCGlxGarbageCollector->CleanupCache(*cache1, 0);

    EUNIT_PRINT(_L("Check remaining scan count is 0"));
    EUNIT_ASSERT_DESC(remainingScan == 0, "Remaining scan count should be 0");

    EUNIT_PRINT(_L("Check remaining media in cache1"));
    EUNIT_ASSERT_DESC(cache1->Count() == 3, "Remaining media should be 3");

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    CheckCache1MediaAttributes(*media1Cache1, 3, ETrue, ETrue, ETrue);
    CheckCache1MediaErrors(*media1Cache1, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    CheckCache1MediaAttributes(*media2Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, ETrue, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache1"));
    CGlxMedia* media3Cache1 = cache1Helper.ItemPool()[2];
    CheckCache1MediaAttributes(*media3Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media3Cache1, ETrue, ETrue, EFalse, EFalse);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupCache_SubTest2L(  )
    {
    // Create helper class
    CGlxCache* cache1 = iCaches[0];
    T_CacheTestHelpers cache1Helper(cache1);

    // Keep original cache pointers
    CGlxMedia* oldMedia2Cache1 = cache1Helper.ItemPool()[1];
    CGlxMedia* oldMedia3Cache1 = cache1Helper.ItemPool()[2];

    // No media users
    // Cleanup cache1, scan for 1 item
    EUNIT_PRINT(_L("Cleanup cache1, scan for 1 item"));
    TInt remainingScan = iCGlxGarbageCollector->CleanupCache(*cache1, 1);

    EUNIT_PRINT(_L("Check remaining scan count is 0"));
    EUNIT_ASSERT_DESC(remainingScan == 0, "Remaining scan count should be 0");

    EUNIT_PRINT(_L("Check remaining media in cache1"));
    EUNIT_ASSERT_DESC(cache1->Count() == 2, "Remaining media should be 2");

    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    EUNIT_ASSERT_DESC(media1Cache1 == oldMedia2Cache1, "New media1 in cache1 should be old media2");
    EUNIT_ASSERT_DESC(media2Cache1 == oldMedia3Cache1, "New media2 in cache1 should be old media3");

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media1Cache1, ETrue, ETrue, ETrue, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, EFalse, EFalse);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupCache_SubTest3L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title, Subtitle and System item attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Cleanup cache1, scan for 1 item
    EUNIT_PRINT(_L("Cleanup cache1, scan for 1 item"));
    TInt remainingScan = iCGlxGarbageCollector->CleanupCache(*cache1, 1);

    EUNIT_PRINT(_L("Check remaining scan count is 0"));
    EUNIT_ASSERT_DESC(remainingScan == 0, "Remaining scan count should be 0");

    EUNIT_PRINT(_L("Check remaining media in cache1"));
    EUNIT_ASSERT_DESC(cache1->Count() == 3, "Remaining media should be 3");

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    CheckCache1MediaAttributes(*media1Cache1, 3, ETrue, ETrue, ETrue);
    CheckCache1MediaErrors(*media1Cache1, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    CheckCache1MediaAttributes(*media2Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, ETrue, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache1"));
    CGlxMedia* media3Cache1 = cache1Helper.ItemPool()[2];
    CheckCache1MediaAttributes(*media3Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media3Cache1, ETrue, ETrue, EFalse, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupCache_SubTest4L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("No attributes wanted by mediauser1"));
    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Keep original cache pointers
    CGlxMedia* oldMedia2Cache1 = cache1Helper.ItemPool()[1];
    CGlxMedia* oldMedia3Cache1 = cache1Helper.ItemPool()[2];

    // Cleanup cache1, scan for 1 item
    EUNIT_PRINT(_L("Cleanup cache1, scan for 1 item"));
    TInt remainingScan = iCGlxGarbageCollector->CleanupCache(*cache1, 1);

    EUNIT_PRINT(_L("Check remaining scan count is 0"));
    EUNIT_ASSERT_DESC(remainingScan == 0, "Remaining scan count should be 0");

    EUNIT_PRINT(_L("Check remaining media in cache1"));
    EUNIT_ASSERT_DESC(cache1->Count() == 2, "Remaining media should be 2");

    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    EUNIT_ASSERT_DESC(media1Cache1 == oldMedia2Cache1, "New media1 in cache1 should be old media2");
    EUNIT_ASSERT_DESC(media2Cache1 == oldMedia3Cache1, "New media2 in cache1 should be old media3");

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media1Cache1, ETrue, ETrue, ETrue, EFalse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, EFalse, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupCache_SubTest5L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("No attributes wanted by mediauser1"));
    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("Media2 in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToMediaL(*cache1, 1, *mediaUser1);

    EUNIT_PRINT(_L("Subtitle and System item attributes wanted by mediauser2"));
    attributes.AppendL(KMPXMediaGeneralSubTitle);
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser2 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser2);

    EUNIT_PRINT(_L("Media3 in cache1 used by mediauser2"));
    AddMediaUserToMediaL(*cache1, 2, *mediaUser2);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Keep original cache pointers
    CGlxMedia* oldMedia3Cache1 = cache1Helper.ItemPool()[2];

    // Cleanup cache1, scan for 4 items
    EUNIT_PRINT(_L("Cleanup cache1, scan for 4 items"));
    TInt remainingScan = iCGlxGarbageCollector->CleanupCache(*cache1, 4);

    EUNIT_PRINT(_L("Check remaining scan count is 1"));
    EUNIT_ASSERT_DESC(remainingScan == 1, "Remaining scan count should be 1");

    EUNIT_PRINT(_L("Check remaining media in cache1"));
    EUNIT_ASSERT_DESC(cache1->Count() == 1, "Remaining media should be 1");

    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    EUNIT_ASSERT_DESC(media1Cache1 == oldMedia3Cache1, "New media1 in cache1 should be old media3");

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 1, EFalse, EFalse, ETrue, EFalse);
    CheckCache1MediaErrors(*media1Cache1, EFalse);

    CleanupStack::PopAndDestroy(mediaUser2);
    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest1L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title, Subtitle and System item attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media1 in cache1
    EUNIT_PRINT(_L("Cleanup media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    iCGlxGarbageCollector->CleanupMediaL(*media1Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 3, ETrue, ETrue, ETrue);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest2L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("System item attribute wanted by mediauser1"));
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media1 in cache1
    EUNIT_PRINT(_L("Cleanup media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    iCGlxGarbageCollector->CleanupMediaL(*media1Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 1, EFalse, EFalse, ETrue);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest3L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Thumbnail attribute wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralThumbnail1);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media1 in cache1
    EUNIT_PRINT(_L("Cleanup media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    iCGlxGarbageCollector->CleanupMediaL(*media1Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 0, EFalse, EFalse, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest4L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("No attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralThumbnail1);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media1 in cache1
    EUNIT_PRINT(_L("Cleanup media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    iCGlxGarbageCollector->CleanupMediaL(*media1Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 0, EFalse, EFalse, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest5L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title, Subtitle and System item attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media2 in cache1
    EUNIT_PRINT(_L("Cleanup media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->CleanupMediaL(*media2Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, ETrue, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest6L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("System item attribute wanted by mediauser1"));
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media2 in cache1
    EUNIT_PRINT(_L("Cleanup media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->CleanupMediaL(*media2Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 1, EFalse, EFalse, ETrue, EFalse);
    CheckCache1MediaErrors(*media2Cache1, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest7L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title and Subtitle attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media2 in cache1
    EUNIT_PRINT(_L("Cleanup media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->CleanupMediaL(*media2Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 1, EFalse, EFalse, EFalse, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, ETrue, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest8L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title and System item attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media2 in cache1
    EUNIT_PRINT(_L("Cleanup media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->CleanupMediaL(*media2Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media2Cache1, ETrue, ETrue, EFalse, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest9L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Thumbnail attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralThumbnail1);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media2 in cache1
    EUNIT_PRINT(_L("Cleanup media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->CleanupMediaL(*media2Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 0, EFalse, EFalse, EFalse, EFalse);
    CheckCache1MediaErrors(*media2Cache1, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest10L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("No attributes wanted by mediauser1"));
    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media2 in cache1
    EUNIT_PRINT(_L("Cleanup media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->CleanupMediaL(*media2Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 0, EFalse, EFalse, EFalse, EFalse);
    CheckCache1MediaErrors(*media2Cache1, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest11L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title, Subtitle and System item attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media2 in cache1
    EUNIT_PRINT(_L("Cleanup media3 in cache1"));
    CGlxMedia* media3Cache1 = cache1Helper.ItemPool()[2];
    iCGlxGarbageCollector->CleanupMediaL(*media3Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media3 in cache1"));
    CheckCache1MediaAttributes(*media3Cache1, 2, EFalse, EFalse, ETrue, ETrue);
    CheckCache1MediaErrors(*media3Cache1, ETrue, ETrue, EFalse, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_CleanupMediaL_SubTest12L(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("No attributes wanted by mediauser1"));
    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    // Cleanup media2 in cache1
    EUNIT_PRINT(_L("Cleanup media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->CleanupMediaL(*media2Cache1);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 0, EFalse, EFalse, EFalse, EFalse);
    CheckCache1MediaErrors(*media2Cache1, EFalse);

    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_GetAttributesInUseLL(  )
    {
    // Create media users
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    EUNIT_PRINT(_L("Title, Subtitle and System item attributes wanted by mediauser1"));
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralSubTitle);
    attributes.AppendL(KGlxMediaGeneralSystemItem);

    CGlxMediaUserTest* mediaUser1 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser1);

    EUNIT_PRINT(_L("All media in cache1 used by mediauser1"));
    CGlxCache* cache1 = iCaches[0];
    AddMediaUserToCacheMediasL(*cache1, *mediaUser1);

    EUNIT_PRINT(_L("Thumbnail attribute wanted by mediauser2"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralThumbnail1);

    CGlxMediaUserTest* mediaUser2 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser2);

    EUNIT_PRINT(_L("Title, Date and Size attributes wanted by mediauser3"));
    attributes.Reset();
    attributes.AppendL(KMPXMediaGeneralTitle);
    attributes.AppendL(KMPXMediaGeneralDate);
    attributes.AppendL(KMPXMediaGeneralSize);

    CGlxMediaUserTest* mediaUser3 = CGlxMediaUserTest::NewL(attributes);
    CleanupStack::PushL(mediaUser3);

    EUNIT_PRINT(_L("All media in cache3 used by mediauser2 and mediauser3"));
    CGlxCache* cache3 = iCaches[2];
    AddMediaUserToCacheMediasL(*cache3, *mediaUser2);
    AddMediaUserToCacheMediasL(*cache3, *mediaUser3);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    RArray<TMPXAttribute> attributesInUse;
    CleanupClosePushL(attributesInUse);

    // Obtain attributes in use for media1 in cache1
    EUNIT_PRINT(_L("Obtain attributes in use for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    iCGlxGarbageCollector->GetAttributesInUseL(*media1Cache1, attributesInUse);

    EUNIT_PRINT(_L("Check attributes in use for media1 in cache1"));
    EUNIT_ASSERT_DESC(attributesInUse.Count() == 3, "Three attributes should be in use");
    EUNIT_ASSERT_DESC(attributesInUse.Find(KMPXMediaGeneralTitle, TMPXAttribute::Match) != KErrNotFound, "Title attribute should be in use");
    EUNIT_ASSERT_DESC(attributesInUse.Find(KMPXMediaGeneralSubTitle, TMPXAttribute::Match) != KErrNotFound, "Subtitle attribute should be in use");
    EUNIT_ASSERT_DESC(attributesInUse.Find(KGlxMediaGeneralSystemItem, TMPXAttribute::Match) != KErrNotFound, "System item attribute should be in use");

    // Create helper class
    T_CacheTestHelpers cache3Helper(cache3);

    attributesInUse.Reset();

    // Obtain attributes in use for media1 in cache3
    EUNIT_PRINT(_L("Obtain attributes in use for media1 in cache3"));
    CGlxMedia* media1Cache3 = cache3Helper.ItemPool()[0];
    iCGlxGarbageCollector->GetAttributesInUseL(*media1Cache3, attributesInUse);

    EUNIT_PRINT(_L("Check attributes in use for media1 in cache3"));
    EUNIT_ASSERT_DESC(attributesInUse.Count() == 4, "Four attributes should be in use");
    EUNIT_ASSERT_DESC(attributesInUse.Find(KMPXMediaGeneralThumbnail1, TMPXAttribute::Match) != KErrNotFound, "Thumbnail attribute should be in use");
    EUNIT_ASSERT_DESC(attributesInUse.Find(KMPXMediaGeneralTitle, TMPXAttribute::Match) != KErrNotFound, "Title attribute should be in use");
    EUNIT_ASSERT_DESC(attributesInUse.Find(KMPXMediaGeneralDate, TMPXAttribute::Match) != KErrNotFound, "Date attribute should be in use");
    EUNIT_ASSERT_DESC(attributesInUse.Find(KMPXMediaGeneralSize, TMPXAttribute::Match) != KErrNotFound, "Size attribute should be in use");

    CleanupStack::PopAndDestroy(&attributesInUse);
    CleanupStack::PopAndDestroy(mediaUser3);
    CleanupStack::PopAndDestroy(mediaUser2);
    CleanupStack::PopAndDestroy(mediaUser1);
    CleanupStack::PopAndDestroy(&attributes);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest1L(  )
    {
    EUNIT_PRINT(_L("Title, Subtitle and System item attributes in use for media1 in cache1"));
    RArray<TMPXAttribute> attributesInUse;
    CleanupClosePushL(attributesInUse);

    attributesInUse.AppendL(KMPXMediaGeneralTitle);
    attributesInUse.AppendL(KMPXMediaGeneralSubTitle);
    attributesInUse.AppendL(KGlxMediaGeneralSystemItem);
    attributesInUse.AppendL(GlxErrorManager::ErrorAttribute());

    // Create helper class
    T_CacheTestHelpers cache1Helper(iCaches[0]);

    // Delete attributes not in use for media1 in cache1
    EUNIT_PRINT(_L("Delete attributes not in use for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    iCGlxGarbageCollector->DeleteOtherAttributes(*media1Cache1, attributesInUse);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 3, ETrue, ETrue, ETrue);

    CleanupStack::PopAndDestroy(&attributesInUse);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest2L(  )
    {
    EUNIT_PRINT(_L("Title and Subtitle attributes in use for media1 in cache1"));
    RArray<TMPXAttribute> attributesInUse;
    CleanupClosePushL(attributesInUse);

    attributesInUse.AppendL(KMPXMediaGeneralTitle);
    attributesInUse.AppendL(KMPXMediaGeneralSubTitle);
    attributesInUse.AppendL(GlxErrorManager::ErrorAttribute());

    // Create helper class
    T_CacheTestHelpers cache1Helper(iCaches[0]);

    // Delete attributes not in use for media1 in cache1
    EUNIT_PRINT(_L("Delete attributes not in use for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    iCGlxGarbageCollector->DeleteOtherAttributes(*media1Cache1, attributesInUse);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 2, ETrue, ETrue, EFalse);

    CleanupStack::PopAndDestroy(&attributesInUse);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest3L(  )
    {
    EUNIT_PRINT(_L("Thumbnail attribute in use for media1 in cache1"));
    RArray<TMPXAttribute> attributesInUse;
    CleanupClosePushL(attributesInUse);

    attributesInUse.AppendL(KMPXMediaGeneralThumbnail1);
    attributesInUse.AppendL(GlxErrorManager::ErrorAttribute());

    // Create helper class
    T_CacheTestHelpers cache1Helper(iCaches[0]);

    // Delete attributes not in use for media1 in cache1
    EUNIT_PRINT(_L("Delete attributes not in use for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    iCGlxGarbageCollector->DeleteOtherAttributes(*media1Cache1, attributesInUse);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 0, EFalse, EFalse, EFalse);

    CleanupStack::PopAndDestroy(&attributesInUse);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest4L(  )
    {
    EUNIT_PRINT(_L("No attributes in use for media1 in cache1"));
    RArray<TMPXAttribute> attributesInUse;
    CleanupClosePushL(attributesInUse);

    attributesInUse.AppendL(GlxErrorManager::ErrorAttribute());

    // Create helper class
    T_CacheTestHelpers cache1Helper(iCaches[0]);

    // Delete attributes not in use for media1 in cache1
    EUNIT_PRINT(_L("Delete attributes not in use for media1 in cache1"));
    CGlxMedia* media1Cache1 = cache1Helper.ItemPool()[0];
    iCGlxGarbageCollector->DeleteOtherAttributes(*media1Cache1, attributesInUse);

    EUNIT_PRINT(_L("Check remaining attributes for media1 in cache1"));
    CheckCache1MediaAttributes(*media1Cache1, 0, EFalse, EFalse, EFalse);

    CleanupStack::PopAndDestroy(&attributesInUse);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest5L(  )
    {
    EUNIT_PRINT(_L("Title, Subtitle and System item attributes in use for media2 in cache1"));
    RArray<TMPXAttribute> attributesInUse;
    CleanupClosePushL(attributesInUse);

    attributesInUse.AppendL(KMPXMediaGeneralTitle);
    attributesInUse.AppendL(KMPXMediaGeneralSubTitle);
    attributesInUse.AppendL(KGlxMediaGeneralSystemItem);
    attributesInUse.AppendL(GlxErrorManager::ErrorAttribute());

    // Create helper class
    T_CacheTestHelpers cache1Helper(iCaches[0]);

    // Delete attributes not in use for media2 in cache1
    EUNIT_PRINT(_L("Delete attributes not in use for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->DeleteOtherAttributes(*media2Cache1, attributesInUse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 2, EFalse, EFalse, ETrue, ETrue);

    CleanupStack::PopAndDestroy(&attributesInUse);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest6L(  )
    {
    EUNIT_PRINT(_L("System item attribute in use for media2 in cache1"));
    RArray<TMPXAttribute> attributesInUse;
    CleanupClosePushL(attributesInUse);

    attributesInUse.AppendL(KGlxMediaGeneralSystemItem);
    attributesInUse.AppendL(GlxErrorManager::ErrorAttribute());

    // Create helper class
    T_CacheTestHelpers cache1Helper(iCaches[0]);

    // Delete attributes not in use for media2 in cache1
    EUNIT_PRINT(_L("Delete attributes not in use for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->DeleteOtherAttributes(*media2Cache1, attributesInUse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 2, EFalse, EFalse, ETrue, ETrue);

    CleanupStack::PopAndDestroy(&attributesInUse);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest7L(  )
    {
    EUNIT_PRINT(_L("Title and Subtitle attributes in use for media2 in cache1"));
    RArray<TMPXAttribute> attributesInUse;
    CleanupClosePushL(attributesInUse);

    attributesInUse.AppendL(KMPXMediaGeneralTitle);
    attributesInUse.AppendL(KMPXMediaGeneralSubTitle);
    attributesInUse.AppendL(GlxErrorManager::ErrorAttribute());

    // Create helper class
    T_CacheTestHelpers cache1Helper(iCaches[0]);

    // Delete attributes not in use for media2 in cache1
    EUNIT_PRINT(_L("Delete attributes not in use for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->DeleteOtherAttributes(*media2Cache1, attributesInUse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 1, EFalse, EFalse, EFalse, ETrue);

    CleanupStack::PopAndDestroy(&attributesInUse);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest8L(  )
    {
    EUNIT_PRINT(_L("Thumbnail attribute in use for media2 in cache1"));
    RArray<TMPXAttribute> attributesInUse;
    CleanupClosePushL(attributesInUse);

    attributesInUse.AppendL(KMPXMediaGeneralThumbnail1);
    attributesInUse.AppendL(GlxErrorManager::ErrorAttribute());

    // Create helper class
    T_CacheTestHelpers cache1Helper(iCaches[0]);

    // Delete attributes not in use for media2 in cache1
    EUNIT_PRINT(_L("Delete attributes not in use for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->DeleteOtherAttributes(*media2Cache1, attributesInUse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 1, EFalse, EFalse, EFalse, ETrue);

    CleanupStack::PopAndDestroy(&attributesInUse);
    }

void T_CGlxGarbageCollector::T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest9L(  )
    {
    EUNIT_PRINT(_L("No attributes in use for media2 in cache1"));
    RArray<TMPXAttribute> attributesInUse;
    CleanupClosePushL(attributesInUse);

    attributesInUse.AppendL(GlxErrorManager::ErrorAttribute());

    // Create helper class
    T_CacheTestHelpers cache1Helper(iCaches[0]);

    // Delete attributes not in use for media2 in cache1
    EUNIT_PRINT(_L("Delete attributes not in use for media2 in cache1"));
    CGlxMedia* media2Cache1 = cache1Helper.ItemPool()[1];
    iCGlxGarbageCollector->DeleteOtherAttributes(*media2Cache1, attributesInUse);

    EUNIT_PRINT(_L("Check remaining attributes for media2 in cache1"));
    CheckCache1MediaAttributes(*media2Cache1, 1, EFalse, EFalse, EFalse, ETrue);

    CleanupStack::PopAndDestroy(&attributesInUse);
    }

void T_CGlxGarbageCollector::TimerCallback()
    {
    // Stop wait loop
    iWait.AsyncStop();
    }

void T_CGlxGarbageCollector::IdleCallbackCalledL()
    {
    ++iIdleCallbackCount;

    // After first idle callback called on garbage collector,
    // Update media in cache
    if (iIdleCallbackCount == 1)
        {
        UpdateMediaL();
        }
    }

void T_CGlxGarbageCollector::UpdateMediaL()
    {
    // Create helper class
    T_CacheTestHelpers cache1Helper(iCaches[0]);

    TTime time;
    time.UniversalTime();

    // Add attributes to media1 in cache1 whilst garbage collector is cleaning up
    CGlxMedia* cache1Media1 = cache1Helper.ItemPool()[0];
    cache1Media1->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);

    CGlxFetchErrorArray* cache1Media1ErrorArray = new (ELeave) CGlxFetchErrorArray();
    CleanupStack::PushL(cache1Media1ErrorArray);

    TGlxFetchError cache1Media1Error1(KMPXMediaGeneralCount, KErrNotFound);
    cache1Media1ErrorArray->AddErrorL(cache1Media1Error1);

    cache1Media1->SetCObjectValueL(GlxErrorManager::ErrorAttribute(), cache1Media1ErrorArray);

    CleanupStack::Pop(cache1Media1ErrorArray);

    // Cleanup again
    EUNIT_PRINT(_L("Cleanup again"));
    iCGlxGarbageCollector->Cleanup();
    }

void T_CGlxGarbageCollector::SetupCachesL()
    {
    // Create cache1
    CGlxCache* cache1 = new (ELeave) CGlxCache(TGlxIdSpaceId(KIdSpaceId1));
    CleanupStack::PushL(cache1);
    iCaches.AppendL(cache1);
    CleanupStack::Pop(cache1);

    // Create helper class
    T_CacheTestHelpers cache1Helper(cache1);

    TTime time;
    time.UniversalTime();

    // Create media1
    CGlxMedia* cache1Media1 = new (ELeave) CGlxMedia(TGlxMediaId(KCache1Media1Id));
    CleanupStack::PushL(cache1Media1);

    cache1Media1->SetTextValueL(KMPXMediaGeneralTitle, KCache1Media1Title);
    cache1Media1->SetTextValueL(KMPXMediaGeneralSubTitle, KCache1Media1SubTitle);
    cache1Media1->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);

    cache1Helper.ItemPool().AppendL(cache1Media1);

    CleanupStack::Pop(cache1Media1);

    // Create media2
    CGlxMedia* cache1Media2 = new (ELeave) CGlxMedia(TGlxMediaId(KCache1Media2Id));
    CleanupStack::PushL(cache1Media2);

    cache1Media2->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);

    CGlxFetchErrorArray* cache1Media2ErrorArray = new (ELeave) CGlxFetchErrorArray();
    CleanupStack::PushL(cache1Media2ErrorArray);

    time.UniversalTime();

    TGlxFetchError cache1Media2Error1(KMPXMediaGeneralTitle, KErrNotSupported);
    cache1Media2ErrorArray->AddErrorL(cache1Media2Error1);

    TGlxFetchError cache1Media2Error2(KMPXMediaGeneralSubTitle, KErrNotFound);
    cache1Media2ErrorArray->AddErrorL(cache1Media2Error2);

    cache1Media2->SetCObjectValueL(GlxErrorManager::ErrorAttribute(), cache1Media2ErrorArray);

    CleanupStack::Pop(cache1Media2ErrorArray);

    cache1Helper.ItemPool().AppendL(cache1Media2);

    CleanupStack::Pop(cache1Media2);

    // Create media3
    CGlxMedia* cache1Media3 = new (ELeave) CGlxMedia(TGlxMediaId(KCache1Media3Id));
    CleanupStack::PushL(cache1Media3);

    cache1Media3->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);

    CGlxFetchErrorArray* cache1Media3ErrorArray = new (ELeave) CGlxFetchErrorArray();
    CleanupStack::PushL(cache1Media3ErrorArray);

    time.UniversalTime();
    time -= TTimeIntervalSeconds(KErrorExpiryPeriod);

    TGlxFetchError cache1Media3Error1(KMPXMediaGeneralTitle, KErrNotSupported);
    cache1Media3Error1.iTimestamp = time.DateTime();
    cache1Media3ErrorArray->AddErrorL(cache1Media3Error1);

    TGlxFetchError cache1Media3Error2(KMPXMediaGeneralSubTitle, KErrNotFound);
    cache1Media3Error2.iTimestamp = time.DateTime();
    cache1Media3ErrorArray->AddErrorL(cache1Media3Error2);

    cache1Media3->SetCObjectValueL(GlxErrorManager::ErrorAttribute(), cache1Media3ErrorArray);

    CleanupStack::Pop(cache1Media3ErrorArray);

    cache1Helper.ItemPool().AppendL(cache1Media3);

    CleanupStack::Pop(cache1Media3);

    // Create cache2
    CGlxCache* cache2 = new (ELeave) CGlxCache(TGlxIdSpaceId(KIdSpaceId2));
    CleanupStack::PushL(cache2);
    iCaches.AppendL(cache2);
    CleanupStack::Pop(cache2);

    // Create helper class
    T_CacheTestHelpers cache2Helper(cache2);

    // Create media1
    CGlxMedia* cache2Media1 = new (ELeave) CGlxMedia(TGlxMediaId(KCache2Media1Id));
    CleanupStack::PushL(cache2Media1);

    cache2Media1->SetTextValueL(KMPXMediaGeneralTitle, KCache2Media1Title);
    cache2Media1->SetTextValueL(KMPXMediaGeneralSubTitle, KCache2Media1SubTitle);
    cache2Media1->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, ETrue);

    cache2Helper.ItemPool().AppendL(cache2Media1);

    CleanupStack::Pop(cache2Media1);

    // Create media2
    CGlxMedia* cache2Media2 = new (ELeave) CGlxMedia(TGlxMediaId(KCache2Media2Id));
    CleanupStack::PushL(cache2Media2);

    cache2Media2->SetTextValueL(KMPXMediaGeneralTitle, KCache2Media2Title);
    cache2Media2->SetTextValueL(KMPXMediaGeneralSubTitle, KCache2Media2SubTitle);
    cache2Media2->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);

    cache2Helper.ItemPool().AppendL(cache2Media2);

    CleanupStack::Pop(cache2Media2);

    // Create cache3
    CGlxCache* cache3 = new (ELeave) CGlxCache(TGlxIdSpaceId(KIdSpaceId3));
    CleanupStack::PushL(cache3);
    iCaches.AppendL(cache3);
    CleanupStack::Pop(cache3);

    // Create helper class
    T_CacheTestHelpers cache3Helper(cache3);

    // Create media1
    CGlxMedia* cache3Media1 = new (ELeave) CGlxMedia(TGlxMediaId(KCache3Media1Id));
    CleanupStack::PushL(cache3Media1);

    time.UniversalTime();

    cache3Media1->SetTObjectValueL<TInt>(KMPXMediaGeneralThumbnail1, KCache3Media1Id);
    cache3Media1->SetTextValueL(KMPXMediaGeneralTitle, KCache3Media1Title);
    cache3Media1->SetTObjectValueL<TInt>(KMPXMediaGeneralDate, time.Int64());
    cache3Media1->SetTObjectValueL<TInt>(KMPXMediaGeneralSize, KCache3Media1Size);

    cache3Helper.ItemPool().AppendL(cache3Media1);

    CleanupStack::Pop(cache3Media1);

    // Create media2
    CGlxMedia* cache3Media2 = new (ELeave) CGlxMedia(TGlxMediaId(KCache3Media2Id));
    CleanupStack::PushL(cache3Media2);

    cache3Media2->SetTObjectValueL<TInt>(KMPXMediaGeneralSize, KCache3Media2Size);

    cache3Helper.ItemPool().AppendL(cache3Media2);

    CleanupStack::Pop(cache3Media2);

    // Create media3
    CGlxMedia* cache3Media3 = new (ELeave) CGlxMedia(TGlxMediaId(KCache3Media3Id));
    CleanupStack::PushL(cache3Media3);

    cache3Media3->SetTextValueL(KMPXMediaGeneralTitle, KCache3Media3Title);
    cache3Media3->SetTObjectValueL<TInt>(KMPXMediaGeneralSize, KCache3Media3Size);

    cache3Helper.ItemPool().AppendL(cache3Media3);

    CleanupStack::Pop(cache3Media3);

    // Create media4
    CGlxMedia* cache3Media4 = new (ELeave) CGlxMedia(TGlxMediaId(KCache3Media4Id));
    CleanupStack::PushL(cache3Media4);

    cache3Media4->SetTextValueL(KMPXMediaGeneralTitle, KCache3Media4Title);
    cache3Media4->SetTObjectValueL<TInt>(KMPXMediaGeneralDate, time.Int64());

    CGlxFetchErrorArray* cache3Media4ErrorArray = new (ELeave) CGlxFetchErrorArray();
    CleanupStack::PushL(cache3Media4ErrorArray);

    time.UniversalTime();
    time -= TTimeIntervalSeconds(KErrorExpiryPeriod);

    TGlxFetchError cache3Media4Error1(KMPXMediaGeneralThumbnail1, KErrNotFound);
    cache3Media4Error1.iTimestamp = time.DateTime();
    cache3Media4ErrorArray->AddErrorL(cache3Media4Error1);

    TGlxFetchError cache3Media4Error2(KMPXMediaGeneralSize, KErrCorrupt);
    cache3Media4Error2.iTimestamp = time.DateTime();
    cache3Media4ErrorArray->AddErrorL(cache3Media4Error2);

    cache3Media4->SetCObjectValueL(GlxErrorManager::ErrorAttribute(), cache3Media4ErrorArray);

    CleanupStack::Pop(cache3Media4ErrorArray);

    cache3Helper.ItemPool().AppendL(cache3Media4);

    CleanupStack::Pop(cache3Media4);

    // Create media5
    CGlxMedia* cache3Media5 = new (ELeave) CGlxMedia(TGlxMediaId(KCache3Media5Id));
    CleanupStack::PushL(cache3Media5);

    time.UniversalTime();

    cache3Media5->SetTextValueL(KMPXMediaGeneralTitle, KCache3Media5Title);
    cache3Media5->SetTObjectValueL<TInt>(KMPXMediaGeneralDate, time.Int64());

    CGlxFetchErrorArray* cache3Media5ErrorArray = new (ELeave) CGlxFetchErrorArray();
    CleanupStack::PushL(cache3Media5ErrorArray);

    TGlxFetchError cache3Media5Error1(KMPXMediaGeneralThumbnail1, KErrNotFound);
    cache3Media5Error1.iTimestamp = time.DateTime();
    cache3Media5ErrorArray->AddErrorL(cache3Media5Error1);

    TGlxFetchError cache3Media5Error2(KMPXMediaGeneralSize, KErrCorrupt);
    cache3Media5Error2.iTimestamp = time.DateTime();
    cache3Media5ErrorArray->AddErrorL(cache3Media5Error2);

    cache3Media5->SetCObjectValueL(GlxErrorManager::ErrorAttribute(), cache3Media5ErrorArray);

    CleanupStack::Pop(cache3Media5ErrorArray);

    cache3Helper.ItemPool().AppendL(cache3Media5);

    CleanupStack::Pop(cache3Media5);

    // Create media6
    CGlxMedia* cache3Media6 = new (ELeave) CGlxMedia(TGlxMediaId(KCache3Media6Id));
    CleanupStack::PushL(cache3Media6);

    CGlxFetchErrorArray* cache3Media6ErrorArray = new (ELeave) CGlxFetchErrorArray();
    CleanupStack::PushL(cache3Media6ErrorArray);

    time.UniversalTime();
    time -= TTimeIntervalSeconds(KErrorExpiryPeriod);

    TGlxFetchError cache3Media6Error1(KMPXMediaGeneralSize, KErrCorrupt);
    cache3Media6Error1.iTimestamp = time.DateTime();
    cache3Media6ErrorArray->AddErrorL(cache3Media6Error1);

    cache3Media6->SetCObjectValueL(GlxErrorManager::ErrorAttribute(), cache3Media6ErrorArray);

    CleanupStack::Pop(cache3Media6ErrorArray);

    cache3Helper.ItemPool().AppendL(cache3Media6);

    CleanupStack::Pop(cache3Media6);

    // Create media7
    CGlxMedia* cache3Media7 = new (ELeave) CGlxMedia(TGlxMediaId(KCache3Media7Id));
    CleanupStack::PushL(cache3Media7);

    CGlxFetchErrorArray* cache3Media7ErrorArray = new (ELeave) CGlxFetchErrorArray();
    CleanupStack::PushL(cache3Media7ErrorArray);

    time.UniversalTime();

    TGlxFetchError cache3Media7Error1(KMPXMediaGeneralSize, KErrCorrupt);
    cache3Media7Error1.iTimestamp = time.DateTime();
    cache3Media7ErrorArray->AddErrorL(cache3Media7Error1);

    cache3Media7->SetCObjectValueL(GlxErrorManager::ErrorAttribute(), cache3Media7ErrorArray);

    CleanupStack::Pop(cache3Media7ErrorArray);

    cache3Helper.ItemPool().AppendL(cache3Media7);

    CleanupStack::Pop(cache3Media7);
    }

void T_CGlxGarbageCollector::AddMediaUserToCacheMediasL(CGlxCache& aCache, 
        MGlxMediaUser& aMediaUser)
    {
    // Create helper class
    T_CacheTestHelpers cacheHelper(&aCache);

    TInt cacheCount = aCache.Count();
    for (TInt i = 0; i < cacheCount; ++i)
        {
        CGlxMedia* media = cacheHelper.ItemPool()[i];

        TInt userCount = media->UserCount();

        media->ReserveUsersL(userCount + 1);
        media->AddUser(aMediaUser, KErrNotFound);
        }
    }

void T_CGlxGarbageCollector::RemoveMediaUserFromCacheMedias(CGlxCache& aCache, 
        MGlxMediaUser& aMediaUser)
    {
    // Create helper class
    T_CacheTestHelpers cacheHelper(&aCache);

    TInt cacheCount = aCache.Count();
    for (TInt i = 0; i < cacheCount; ++i)
        {
        CGlxMedia* media = cacheHelper.ItemPool()[i];
        media->RemoveUser(aMediaUser);
        }
    }

void T_CGlxGarbageCollector::AddMediaUserToMediaL(CGlxCache& aCache, TInt aIndex, 
        MGlxMediaUser& aMediaUser)
    {
    // Create helper class
    T_CacheTestHelpers cacheHelper(&aCache);

    CGlxMedia* media = cacheHelper.ItemPool()[aIndex];

    TInt userCount = media->UserCount();

    media->ReserveUsersL(userCount + 1);
    media->AddUser(aMediaUser, KErrNotFound);
    }

void T_CGlxGarbageCollector::RemoveMediaUserFromMedia(CGlxCache& aCache, TInt aIndex, 
        MGlxMediaUser& aMediaUser)
    {
    // Create helper class
    T_CacheTestHelpers cacheHelper(&aCache);

    CGlxMedia* media = cacheHelper.ItemPool()[aIndex];
    media->RemoveUser(aMediaUser);
    }

void T_CGlxGarbageCollector::CheckCache1MediaAttributes(const CGlxMedia& aMedia,
        TInt aAttributeCount, TBool aTitleSupported, TBool aSubtitleSupported,
        TBool aSystemItemSupported, TBool aErrorSupported)
    {
    EUNIT_ASSERT_DESC(aMedia.Count() == aAttributeCount, "Incorrect attributes remaining");

    EUNIT_ASSERT_DESC(aMedia.IsSupported(KMPXMediaGeneralTitle) == aTitleSupported, "Incorrect Title attribute expectation");
    EUNIT_ASSERT_DESC(aMedia.IsSupported(KMPXMediaGeneralSubTitle) == aSubtitleSupported, "Incorrect Subtitle attribute expectation");
    EUNIT_ASSERT_DESC(aMedia.IsSupported(KGlxMediaGeneralSystemItem) == aSystemItemSupported, "Incorrect System item attribute expectation");
    EUNIT_ASSERT_DESC(aMedia.IsSupported(GlxErrorManager::ErrorAttribute()) == aErrorSupported, "Incorrect error attribute expectation");
    }

void T_CGlxGarbageCollector::CheckCache2MediaAttributes(const CGlxMedia& aMedia,
        TInt aAttributeCount, TBool aTitleSupported, TBool aSubtitleSupported,
        TBool aSystemItemSupported, TBool aErrorSupported)
    {
    CheckCache1MediaAttributes(aMedia, aAttributeCount, aTitleSupported, aSubtitleSupported, aSystemItemSupported, aErrorSupported);
    }

void T_CGlxGarbageCollector::CheckCache3MediaAttributes(const CGlxMedia& aMedia,
        TInt aAttributeCount, TBool aThumbnailSupported, TBool aTitleSupported, 
        TBool aDateSupported, TBool aSizeSupported, TBool aErrorSupported)
    {
    EUNIT_ASSERT_DESC(aMedia.Count() == aAttributeCount, "Incorrect attributes remaining");

    EUNIT_ASSERT_DESC(aMedia.IsSupported(KMPXMediaGeneralThumbnail1) == aThumbnailSupported, "Incorrect Thumbnail attribute expectation");
    EUNIT_ASSERT_DESC(aMedia.IsSupported(KMPXMediaGeneralTitle) == aTitleSupported, "Incorrect Title attribute expectation");
    EUNIT_ASSERT_DESC(aMedia.IsSupported(KMPXMediaGeneralDate) == aDateSupported, "Incorrect Date attribute expectation");
    EUNIT_ASSERT_DESC(aMedia.IsSupported(KMPXMediaGeneralSize) == aSizeSupported, "Incorrect Size attribute expectation");
    EUNIT_ASSERT_DESC(aMedia.IsSupported(GlxErrorManager::ErrorAttribute()) == aErrorSupported, "Incorrect error attribute expectation");
    }

void T_CGlxGarbageCollector::CheckCache1MediaErrors(const CGlxMedia& aMedia,
        TBool aError, TBool aTitleError, TBool aSubtitleError, TBool aSystemItemError)
    {
    EUNIT_ASSERT_DESC(GlxErrorManager::HasError(&aMedia) == aError, "Incorrect error expectation");

    EUNIT_ASSERT_DESC((GlxErrorManager::HasAttributeErrorL(&aMedia, KMPXMediaGeneralTitle) != KErrNone) == aTitleError, "Incorrect Title attribute error expectation");
    EUNIT_ASSERT_DESC((GlxErrorManager::HasAttributeErrorL(&aMedia, KMPXMediaGeneralSubTitle) != KErrNone) == aSubtitleError, "Incorrect Subtitle attribute error expectation");
    EUNIT_ASSERT_DESC((GlxErrorManager::HasAttributeErrorL(&aMedia, KGlxMediaGeneralSystemItem) != KErrNone) == aSystemItemError, "Incorrect System item attribute error expectation");
    }

void T_CGlxGarbageCollector::CheckCache2MediaErrors(const CGlxMedia& aMedia,
        TBool aError, TBool aTitleError, TBool aSubtitleError, TBool aSystemItemError)
    {
    CheckCache1MediaErrors(aMedia, aError, aTitleError, aSubtitleError, aSystemItemError);
    }

void T_CGlxGarbageCollector::CheckCache3MediaErrors(const CGlxMedia& aMedia,
        TBool aError, TBool aThumbnailError, TBool aTitleError, TBool aDateError, TBool aSizeError)
    {
    EUNIT_ASSERT_DESC(GlxErrorManager::HasError(&aMedia) == aError, "Incorrect error expectation");

    EUNIT_ASSERT_DESC((GlxErrorManager::HasAttributeErrorL(&aMedia, KMPXMediaGeneralThumbnail1) != KErrNone) == aThumbnailError, "Incorrect Thumbnail attribute error expectation");
    EUNIT_ASSERT_DESC((GlxErrorManager::HasAttributeErrorL(&aMedia, KMPXMediaGeneralTitle) != KErrNone) == aTitleError, "Incorrect Title attribute error expectation");
    EUNIT_ASSERT_DESC((GlxErrorManager::HasAttributeErrorL(&aMedia, KMPXMediaGeneralDate) != KErrNone) == aDateError, "Incorrect Date attribute error expectation");
    EUNIT_ASSERT_DESC((GlxErrorManager::HasAttributeErrorL(&aMedia, KMPXMediaGeneralSize) != KErrNone) == aSizeError, "Incorrect Size attribute error expectation");
    }

// Test media user
T_CGlxGarbageCollector::CGlxMediaUserTest* T_CGlxGarbageCollector::CGlxMediaUserTest::NewL(const RArray<TMPXAttribute>& aAttributes)
    {
    CGlxMediaUserTest* self = new (ELeave) CGlxMediaUserTest();
    CleanupStack::PushL(self);
    self->ConstructL(aAttributes);
    CleanupStack::Pop();
    return self;
    }

T_CGlxGarbageCollector::CGlxMediaUserTest::CGlxMediaUserTest() :
        iAttributes()
    {
    }

T_CGlxGarbageCollector::CGlxMediaUserTest::~CGlxMediaUserTest()
    {
    iAttributes.Close();
    }

void T_CGlxGarbageCollector::CGlxMediaUserTest::ConstructL(const RArray<TMPXAttribute>& aAttributes)
    {
    TInt attributeCount = aAttributes.Count();
    for (TInt i = 0; i < attributeCount; ++i)
        {
        iAttributes.AppendL(aAttributes[i]);
        }
    }

void T_CGlxGarbageCollector::CGlxMediaUserTest::GetRequiredAttributesL(
        TInt /*aIndex*/, 
        RArray<TMPXAttribute>& aAttributes)
    {
    TInt attributeCount = iAttributes.Count();
    for (TInt i = 0; i < attributeCount; ++i)
        {
        aAttributes.AppendL(iAttributes[i]);
        }
    }

void T_CGlxGarbageCollector::CGlxMediaUserTest::RemoveReference(TInt /*aIndex*/)
    {
    }

// Test timer
T_CGlxGarbageCollector::CTimerTest* T_CGlxGarbageCollector::CTimerTest::NewL(
        T_CGlxGarbageCollector* aTestGarbageCollector)
    {
    CTimerTest* self = new (ELeave) CTimerTest(aTestGarbageCollector);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

T_CGlxGarbageCollector::CTimerTest::CTimerTest(
        T_CGlxGarbageCollector* aTestGarbageCollector) :
    CTimer(EPriorityLow),
    iTestGarbageCollector(aTestGarbageCollector)
    {
    CActiveScheduler::Add(this);
    }

T_CGlxGarbageCollector::CTimerTest::~CTimerTest()
    {
    Cancel();
    }

void T_CGlxGarbageCollector::CTimerTest::ConstructL()
    {
    CTimer::ConstructL();
    }

void T_CGlxGarbageCollector::CTimerTest::StartTimer(TInt aSeconds)
    {
    After(aSeconds * KMicrosecondsInSeconds);
    }

void T_CGlxGarbageCollector::CTimerTest::RunL()
    {
    iTestGarbageCollector->TimerCallback();
    }

// Override CIdle implementation for test purposes,
// So that test can do things just after idle callbacks
EXPORT_C CIdle* CIdle::New(TInt aPriority)
    {
    CIdle *pI=new CIdle(aPriority);
    if (pI!=NULL)
        CActiveScheduler::Add(pI);
    return(pI);
    }

EXPORT_C CIdle* CIdle::NewL(TInt aPriority)
    {
    CIdle *pI=new(ELeave) CIdle(aPriority);
    CActiveScheduler::Add(pI);
    return(pI);
    }

EXPORT_C CIdle::CIdle(TInt aPriority)
	: CActive(aPriority)
    {
    }

EXPORT_C CIdle::~CIdle()
    {
    Cancel();
    }

EXPORT_C void CIdle::Start(TCallBack aCallBack)
    {
    iCallBack=aCallBack;
    iStatus=KRequestPending;
    TRequestStatus *pS=(&iStatus);
    User::RequestComplete(pS,0);
    SetActive();
    }

EXPORT_C void CIdle::RunL()
    {
    if (iCallBack.CallBack())
        {
        Start(iCallBack);

        if (T_CGlxGarbageCollector::iGarbageCollectorTest)
            {
            T_CGlxGarbageCollector::iGarbageCollectorTest->IdleCallbackCalledL();
            }
        }
    }

EXPORT_C void CIdle::DoCancel()
    {
    }


//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CGlxGarbageCollector,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "Cleanup - test1, subtest1",
    "CGlxGarbageCollector",
    "Cleanup - test1, subtest1",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_Cleanup_SubTest1L, Teardown)

EUNIT_TEST(
    "Cleanup - test1, subtest2",
    "CGlxGarbageCollector",
    "Cleanup - test1, subtest2",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_Cleanup_SubTest2L, Teardown)

EUNIT_TEST(
    "Cleanup - test1, subtest3",
    "CGlxGarbageCollector",
    "Cleanup - test1, subtest3",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_Cleanup_SubTest3L, Teardown)

EUNIT_TEST(
    "Cleanup - test1, subtest4",
    "CGlxGarbageCollector",
    "Cleanup - test1, subtest4",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_Cleanup_SubTest4L, Teardown)

EUNIT_TEST(
    "CleanupCaches - test2",
    "CGlxGarbageCollector",
    "CleanupCaches - test2",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupCachesL, Teardown)

EUNIT_TEST(
    "CleanupCache - test3, subtest1",
    "CGlxGarbageCollector",
    "CleanupCache - test3, subtest1",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupCache_SubTest1L, Teardown)

EUNIT_TEST(
    "CleanupCache - test3, subtest2",
    "CGlxGarbageCollector",
    "CleanupCache - test3, subtest2",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupCache_SubTest2L, Teardown)

EUNIT_TEST(
    "CleanupCache - test3, subtest3",
    "CGlxGarbageCollector",
    "CleanupCache - test3, subtest3",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupCache_SubTest3L, Teardown)

EUNIT_TEST(
    "CleanupCache - test3, subtest4",
    "CGlxGarbageCollector",
    "CleanupCache - test3, subtest4",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupCache_SubTest4L, Teardown)

EUNIT_TEST(
    "CleanupCache - test3, subtest5",
    "CGlxGarbageCollector",
    "CleanupCache - test3, subtest5",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupCache_SubTest5L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest1",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest1",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest1L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest2",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest2",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest2L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest3",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest3",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest3L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest4",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest4",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest4L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest5",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest5",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest5L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest6",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest6",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest6L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest7",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest7",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest7L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest8",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest8",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest8L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest9",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest9",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest9L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest10",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest10",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest10L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest11",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest11",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest11L, Teardown)

EUNIT_TEST(
    "CleanupMediaL - test4, subtest12",
    "CGlxGarbageCollector",
    "CleanupMediaL - test4, subtest12",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_CleanupMediaL_SubTest12L, Teardown)

EUNIT_TEST(
    "GetAttributesInUseL - test5",
    "CGlxGarbageCollector",
    "GetAttributesInUseL - test5",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_GetAttributesInUseLL, Teardown)

EUNIT_TEST(
    "DeleteOtherAttributes - test6, subtest1",
    "CGlxGarbageCollector",
    "DeleteOtherAttributes - test6, subtest1",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest1L, Teardown)

EUNIT_TEST(
    "DeleteOtherAttributes - test6, subtest2",
    "CGlxGarbageCollector",
    "DeleteOtherAttributes - test6, subtest2",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest2L, Teardown)

EUNIT_TEST(
    "DeleteOtherAttributes - test6, subtest3",
    "CGlxGarbageCollector",
    "DeleteOtherAttributes - test6, subtest3",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest3L, Teardown)

EUNIT_TEST(
    "DeleteOtherAttributes - test6, subtest4",
    "CGlxGarbageCollector",
    "DeleteOtherAttributes - test6, subtest4",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest4L, Teardown)

EUNIT_TEST(
    "DeleteOtherAttributes - test6, subtest5",
    "CGlxGarbageCollector",
    "DeleteOtherAttributes - test6, subtest5",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest5L, Teardown)

EUNIT_TEST(
    "DeleteOtherAttributes - test6, subtest6",
    "CGlxGarbageCollector",
    "DeleteOtherAttributes - test6, subtest6",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest6L, Teardown)

EUNIT_TEST(
    "DeleteOtherAttributes - test6, subtest7",
    "CGlxGarbageCollector",
    "DeleteOtherAttributes - test6, subtest7",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest7L, Teardown)

EUNIT_TEST(
    "DeleteOtherAttributes - test6, subtest8",
    "CGlxGarbageCollector",
    "DeleteOtherAttributes - test6, subtest8",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest8L, Teardown)

EUNIT_TEST(
    "DeleteOtherAttributes - test6, subtest9",
    "CGlxGarbageCollector",
    "DeleteOtherAttributes - test6, subtest9",
    "FUNCTIONALITY",
    SetupL, T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest9L, Teardown)
    

EUNIT_END_TEST_TABLE

//  END OF FILE
