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
* Description:   CGlxCache unit tests
*
*/



//  CLASS HEADER
#include "t_cglxcache.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediadrmdefs.h>

//  INTERNAL INCLUDES
#include "glxcachemanager.h"
#include "glxmedia.h"

#include "glxmediageneraldefs.h"

#include "glxpanic.h"

#include "t_cachetesthelpers.h"

const TInt KIdSpaceId = 1;

const TInt KMediaId1 = 1;
const TInt KMediaId2 = 2;
const TInt KMediaId3 = 3;
const TInt KMediaId4 = 4;
const TInt KMediaId5 = 5;

// CONSTRUCTION
T_CGlxCache* T_CGlxCache::NewL()
    {
    T_CGlxCache* self = T_CGlxCache::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CGlxCache* T_CGlxCache::NewLC()
    {
    T_CGlxCache* self = new( ELeave ) T_CGlxCache();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CGlxCache::~T_CGlxCache()
    {
    }

// Default constructor
T_CGlxCache::T_CGlxCache()
    {
    }

// Second phase construct
void T_CGlxCache::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void T_CGlxCache::SetupL(  )
    {
    iCGlxCache = new (ELeave) CGlxCache(TGlxIdSpaceId(KIdSpaceId));

    SetupCacheL();
    }
    

void T_CGlxCache::Teardown(  )
    {
    delete iCGlxCache; 
    iCGlxCache = NULL; 
    }
    

void T_CGlxCache::T_CGlxCache_IdSpaceIdL(  )
    {
    TGlxIdSpaceId idSpaceId = iCGlxCache->IdSpaceId( );
    EUNIT_PRINT(_L("Check IdSpaceId"));
    EUNIT_ASSERT_DESC( idSpaceId == TGlxIdSpaceId(KIdSpaceId), "Incorrect IdSpaceId");
    }
    
void T_CGlxCache::T_CGlxCache_MediaUpdatedLL(  )
    {
    // Create helper class
    T_CacheTestHelpers cacheTestHelper(iCGlxCache);

    // New media
    CMPXMedia* newMedia3 = CMPXMedia::NewL();
    CleanupStack::PushL(newMedia3);

    newMedia3->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(KMediaId3));

    EUNIT_PRINT(_L("Add new media"));
    iCGlxCache->MediaUpdatedL(*newMedia3);

    EUNIT_PRINT(_L("Check new media added"));
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool().Count() == 3, "Cache doesn't contain three items");

    TInt cachedNewMedia3Index = cacheTestHelper.ItemPool().FindInOrder(TGlxMediaId(KMediaId3), T_CacheTestHelpers::MediaItemOrderByKey);
    EUNIT_ASSERT_DESC(cachedNewMedia3Index != KErrNotFound, "New media not added to cache");

    CleanupStack::PopAndDestroy(newMedia3);

    // Multiple new media
    CMPXMedia* newMultipleMedia = CMPXMedia::NewL();
    CleanupStack::PushL(newMultipleMedia);

    CMPXMediaArray* newMediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL(newMediaArray);

    CMPXMedia* newMedia4 = CMPXMedia::NewL();
    CleanupStack::PushL(newMedia4);

    newMedia4->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(KMediaId4));

    newMediaArray->AppendL(newMedia4);
    CleanupStack::Pop(newMedia4);

    CMPXMedia* newMedia5 = CMPXMedia::NewL();
    CleanupStack::PushL(newMedia5);

    newMedia5->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(KMediaId5));

    newMediaArray->AppendL(newMedia5);
    CleanupStack::Pop(newMedia5);

    newMultipleMedia->SetCObjectValueL(KMPXMediaArrayContents, newMediaArray);
    newMultipleMedia->SetTObjectValueL(KMPXMediaArrayCount, newMediaArray->Count());

    EUNIT_PRINT(_L("Add multiple new media"));
    iCGlxCache->MediaUpdatedL(*newMultipleMedia);

    CleanupStack::PopAndDestroy(newMediaArray);
    CleanupStack::PopAndDestroy(newMultipleMedia);

    EUNIT_PRINT(_L("Check multiple new media added"));
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool().Count() == 5, "Cache doesn't contain five items");

    TInt cachedNewMedia4Index = cacheTestHelper.ItemPool().FindInOrder(TGlxMediaId(KMediaId4), T_CacheTestHelpers::MediaItemOrderByKey);
    TInt cachedNewMedia5Index = cacheTestHelper.ItemPool().FindInOrder(TGlxMediaId(KMediaId5), T_CacheTestHelpers::MediaItemOrderByKey);
    EUNIT_ASSERT_DESC((cachedNewMedia4Index != KErrNotFound) && (cachedNewMedia5Index != KErrNotFound), "Multiple new media not added to cache");
    }
    
void T_CGlxCache::T_CGlxCache_UpdateMediaLL(  )
    {
    // Create helper class
    T_CacheTestHelpers cacheTestHelper(iCGlxCache);

    // Create cache observer class
    CGlxCacheObserverTest* cacheObserver = CGlxCacheObserverTest::NewL();
    CleanupStack::PushL(cacheObserver);

    CGlxCacheManager* cacheManager = CGlxCacheManager::InstanceL();
    CleanupClosePushL(*cacheManager);

    cacheManager->AddObserverL(cacheObserver);

    // New media without id
    CMPXMedia* newMedia3 = CMPXMedia::NewL();
    CleanupStack::PushL(newMedia3);

    EUNIT_PRINT(_L("Add new media without id"));
    iCGlxCache->UpdateMediaL(*newMedia3);

    EUNIT_PRINT(_L("Check new media not added"));
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool().Count() == 2, "Cache doesn't contain two items");

    // New media without attributes
    newMedia3->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(KMediaId3));

    EUNIT_PRINT(_L("Add new media without attributes"));
    iCGlxCache->UpdateMediaL(*newMedia3);

    EUNIT_PRINT(_L("Check new media added without attributes"));
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool().Count() == 3, "Cache doesn't contain three items");
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool()[2]->Count() == 0, "Item contains attributes");

    TInt cachedNewMedia3Index = cacheTestHelper.ItemPool().FindInOrder(TGlxMediaId(KMediaId3), T_CacheTestHelpers::MediaItemOrderByKey);
    EUNIT_ASSERT_DESC(cachedNewMedia3Index != KErrNotFound, "New media not added to cache");

    EUNIT_PRINT(_L("Check observer not notified of new media attributes"));
    EUNIT_ASSERT_DESC(!cacheObserver->AttributesAvailableNotified(), "Observer notified of new media attributes");

    CleanupStack::PopAndDestroy(newMedia3);

    // New media with attributes
    CMPXMedia* newMedia4 = CMPXMedia::NewL();
    CleanupStack::PushL(newMedia4);

    newMedia4->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(KMediaId4));
    newMedia4->SetTObjectValueL<TSize>(KGlxMediaGeneralDimensions, TSize(640, 480));

    EUNIT_PRINT(_L("Add new media with attributes"));
    cacheObserver->ResetCalls();
    iCGlxCache->UpdateMediaL(*newMedia4);

    EUNIT_PRINT(_L("Check new media added with attributes"));
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool().Count() == 4, "Cache doesn't contain four items");
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool()[3]->Count() == 1, "Item doesn't contain attributes");

    EUNIT_PRINT(_L("Check observer notified of new media attributes"));
    EUNIT_ASSERT_DESC(cacheObserver->AttributesAvailableNotified(), "Observer not notified of new media attributes");
    EUNIT_ASSERT_DESC(cacheObserver->IdSpaceId().Value() == KIdSpaceId, "Observer not notified of correct new media IdSpaceId");
    EUNIT_ASSERT_DESC(cacheObserver->MediaId().Value() == KMediaId4, "Observer not notified of correct new media Id");

    EUNIT_ASSERT_DESC(TMPXAttribute::Match(cacheObserver->Attributes()[0], KGlxMediaGeneralDimensions), "Observer not notified of correct new media attribute");
    EUNIT_ASSERT_DESC(cacheObserver->Attributes().Count() == 1, "Observer not notified of correct new media attribute count");

    EUNIT_ASSERT_DESC(cacheObserver->Media() == cacheTestHelper.ItemPool()[3], "Observer not notified of correct new media pointer");

    CleanupStack::PopAndDestroy(newMedia4);

    // Create media user class
    CGlxMediaUserTest* mediaUser = CGlxMediaUserTest::NewL();
    CleanupStack::PushL(mediaUser);

    cacheTestHelper.ItemPool()[0]->ReserveUsersL(1);
    cacheTestHelper.ItemPool()[0]->AddUser(*mediaUser, KErrNotFound);

    // Update media with existing attributes
    CMPXMedia* Media1 = CMPXMedia::NewL();
    CleanupStack::PushL(Media1);
    Media1->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(KMediaId1));
    Media1->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, ETrue);

    EUNIT_PRINT(_L("Update media with existing attributes"));
    cacheObserver->ResetCalls();
    iCGlxCache->UpdateMediaL(*Media1);

    EUNIT_PRINT(_L("Check media updated with existing attributes"));
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool().Count() == 4, "Cache doesn't contain four items");
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool()[0]->Count() == 1, "Item doesn't contain 1 attribute");

    EUNIT_PRINT(_L("Check user not notified of media new attributes"));
    EUNIT_ASSERT_DESC(!mediaUser->AttributesAvailableNotified(), "User notified of updated media new attributes");
    EUNIT_ASSERT_DESC(!cacheObserver->AttributesAvailableNotified(), "Observer notified of updated media new attributes");

    // Update media with new attributes
    Media1->SetTObjectValueL<TSize>(KGlxMediaGeneralDimensions, TSize(1024, 768));

    EUNIT_PRINT(_L("Update media with new attributes"));
    cacheObserver->ResetCalls();
    mediaUser->ResetCalls();
    iCGlxCache->UpdateMediaL(*Media1);

    EUNIT_PRINT(_L("Check media updated with new attributes"));
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool().Count() == 4, "Cache doesn't contain four items");
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool()[0]->Count() == 2, "Item doesn't contain 2 attributes");

    EUNIT_PRINT(_L("Check user notified of updated media new attributes"));
    EUNIT_ASSERT_DESC(mediaUser->AttributesAvailableNotified(), "User not notified of updated media new attributes");
    EUNIT_ASSERT_DESC(mediaUser->IdSpaceId().Value() == KIdSpaceId, "User not notified of correct updated media IdSpaceId");
    EUNIT_ASSERT_DESC(mediaUser->MediaId().Value() == KMediaId1, "User not notified of correct updated media Id");

    EUNIT_ASSERT_DESC(TMPXAttribute::Match(mediaUser->Attributes()[0], KGlxMediaGeneralDimensions), "Observer not notified of correct updated media attribute");
    EUNIT_ASSERT_DESC(mediaUser->Attributes().Count() == 1, "Observer not notified of correct updated media attribute count");

    EUNIT_PRINT(_L("Check observer notified of updated media new attributes"));
    EUNIT_ASSERT_DESC(cacheObserver->AttributesAvailableNotified(), "Observer not notified of updated media new attributes");
    EUNIT_ASSERT_DESC(cacheObserver->IdSpaceId().Value() == KIdSpaceId, "Observer not notified of correct updated media IdSpaceId");
    EUNIT_ASSERT_DESC(cacheObserver->MediaId().Value() == KMediaId1, "Observer not notified of correct updated media Id");

    EUNIT_ASSERT_DESC(TMPXAttribute::Match(cacheObserver->Attributes()[0], KGlxMediaGeneralDimensions), "Observer not notified of correct updated media attribute");
    EUNIT_ASSERT_DESC(cacheObserver->Attributes().Count() == 1, "Observer not notified of correct updated media attribute count");

    EUNIT_ASSERT_DESC(cacheObserver->Media() == cacheTestHelper.ItemPool()[0], "Observer not notified of correct updated media pointer");

    cacheTestHelper.ItemPool()[0]->RemoveUser(*mediaUser);
    cacheManager->RemoveObserver(cacheObserver);

    CleanupStack::PopAndDestroy(Media1);
    CleanupStack::PopAndDestroy(mediaUser);
    CleanupStack::PopAndDestroy(cacheManager);
    CleanupStack::PopAndDestroy(cacheObserver);
    }
    
void T_CGlxCache::T_CGlxCache_HandleItemModifiedL(  )
    {
    // Create helper class
    T_CacheTestHelpers cacheTestHelper(iCGlxCache);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);

    attributes.AppendL(KMPXMediaDrmProtected);

    EUNIT_PRINT(_L("Modify item attributes"));
    iCGlxCache->HandleItemModified(TGlxMediaId(KMediaId1), attributes);

    CleanupStack::PopAndDestroy(&attributes);

    EUNIT_PRINT(_L("Check attribute deleted as result of item modification"));
    TInt Media1AttributeCount = cacheTestHelper.ItemPool()[0]->Count();
    TInt Media2AttributeCount = cacheTestHelper.ItemPool()[1]->Count();
    
    EUNIT_ASSERT_DESC(Media1AttributeCount == 0, "Attribute not deleted for item modified");
    EUNIT_ASSERT_DESC(Media2AttributeCount == 1, "Attribute deleted for item not modified");
    }
    
void T_CGlxCache::T_CGlxCache_MediaL(  )
    {
    // Create helper class
    T_CacheTestHelpers cacheTestHelper(iCGlxCache);

    EUNIT_PRINT(_L("Obtain non-existing media by id"));
    CGlxMedia* media = iCGlxCache->Media(TGlxMediaId(KMediaId3));

    EUNIT_PRINT(_L("Check returned media"));
    EUNIT_ASSERT_DESC(media == NULL, "Media returned");

    EUNIT_PRINT(_L("Obtain media by id"));
    media = iCGlxCache->Media(TGlxMediaId(KMediaId1));

    EUNIT_PRINT(_L("Check returned media"));
    EUNIT_ASSERT_DESC(media == cacheTestHelper.ItemPool()[0], "Incorrect media returned");
    }
    
void T_CGlxCache::T_CGlxCache_FindItemForceCreateLL(  )
    {
    // Create helper class
    T_CacheTestHelpers cacheTestHelper(iCGlxCache);

    // Find existing instance
    EUNIT_PRINT(_L("Find existing media"));
    CGlxMedia* media = iCGlxCache->FindItemForceCreateL(TGlxMediaId(KMediaId1));

    EUNIT_PRINT(_L("Check returned media"));
    EUNIT_ASSERT_DESC(media == cacheTestHelper.ItemPool()[0], "Incorrect media returned");

    // Find non-existing media
    EUNIT_PRINT(_L("Find non-existing media"));
    media = iCGlxCache->FindItemForceCreateL(TGlxMediaId(KMediaId3));

    EUNIT_PRINT(_L("Check returned media"));
    EUNIT_ASSERT_DESC(media == cacheTestHelper.ItemPool()[2], "Incorrect media returned");
    }
    
void T_CGlxCache::T_CGlxCache_CountL(  )
    {
    EUNIT_PRINT(_L("Obtain media count"));
    TInt count = iCGlxCache->Count();

    EUNIT_PRINT(_L("Check media count"));
    EUNIT_ASSERT_DESC(count == 2, "Incorrect media count");
    }
    
void T_CGlxCache::T_CGlxCache_Media2L(  )
    {
    EUNIT_PRINT(_L("Obtain media by index"));
    CGlxMedia& media = iCGlxCache->Media(1);

    EUNIT_PRINT(_L("Check returned media"));
    EUNIT_ASSERT_DESC(media.Id().Value() == KMediaId2, "Incorrect media returned");
    }
    
void T_CGlxCache::T_CGlxCache_Delete_SubTest1L(  )
    {
#ifdef _DEBUG
    // Delete index less than 0
    EUNIT_PRINT(_L("Delete index less than 0"));
    EUNIT_ASSERT_PANIC_DESC(iCGlxCache->Delete(-1), "Images & videos", EGlxPanicIllegalArgument, "Incorrect panic");
#endif // _DEBUG
    }

void T_CGlxCache::T_CGlxCache_Delete_SubTest2L(  )
    {
#ifdef _DEBUG
    // Delete index greater than number of items
    EUNIT_PRINT(_L("Delete index greater than number of items"));
    EUNIT_ASSERT_PANIC_DESC(iCGlxCache->Delete(2), "Images & videos", EGlxPanicIllegalArgument, "Incorrect panic");
#endif // _DEBUG
    }

void T_CGlxCache::T_CGlxCache_Delete_SubTest3L(  )
    {
    // Create helper class
    T_CacheTestHelpers cacheTestHelper(iCGlxCache);

    // Create item without users
    CGlxMedia* media3 = new (ELeave) CGlxMedia(TGlxMediaId(KMediaId3));
    CleanupStack::PushL(media3);

    media3->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);
    cacheTestHelper.ItemPool().AppendL(media3);

    CleanupStack::Pop(media3);

    // Delete item without users
    EUNIT_PRINT(_L("Delete item without users"));
    iCGlxCache->Delete(2);

    EUNIT_PRINT(_L("Check item deleted"));
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool().Count() == 2, "Cache doesn't contain two items");

    TInt media1Index = cacheTestHelper.ItemPool().FindInOrder(TGlxMediaId(KMediaId1), T_CacheTestHelpers::MediaItemOrderByKey);
    TInt media2Index = cacheTestHelper.ItemPool().FindInOrder(TGlxMediaId(KMediaId2), T_CacheTestHelpers::MediaItemOrderByKey);
    EUNIT_ASSERT_DESC((media1Index != KErrNotFound) && (media2Index != KErrNotFound), "Incorrect item deleted from cache");
    }
    
void T_CGlxCache::T_CGlxCache_Delete_SubTest4L(  )
    {
#ifdef _DEBUG
    // Create helper class
    T_CacheTestHelpers cacheTestHelper(iCGlxCache);

    // Create media user class
    CGlxMediaUserTest* mediaUser = CGlxMediaUserTest::NewL();
    CleanupStack::PushL(mediaUser);

    cacheTestHelper.ItemPool()[0]->ReserveUsersL(1);
    cacheTestHelper.ItemPool()[0]->AddUser(*mediaUser, KErrNotFound);

    // Delete item where user doesn't have reference to media
    EUNIT_PRINT(_L("Delete item where user doesn't have reference to media"));
    EUNIT_ASSERT_PANIC_DESC(iCGlxCache->Delete(0), "Images & videos", EGlxPanicLogicError, "Item deleted where user had reference to media");
#endif // _DEBUG
    }

void T_CGlxCache::T_CGlxCache_Delete_SubTest5L(  )
    {
    // Create helper class
    T_CacheTestHelpers cacheTestHelper(iCGlxCache);

    // Create media user class
    CGlxMediaUserTest* mediaUser = CGlxMediaUserTest::NewL();
    CleanupStack::PushL(mediaUser);

    cacheTestHelper.ItemPool()[1]->ReserveUsersL(1);
    cacheTestHelper.ItemPool()[1]->AddUser(*mediaUser, KErrNotFound);

    // Delete item where user has reference to media
    EUNIT_PRINT(_L("Delete item where user has reference to media"));
    iCGlxCache->Delete(1);

    EUNIT_PRINT(_L("Check item deleted"));
    EUNIT_ASSERT_DESC(cacheTestHelper.ItemPool().Count() == 1, "Cache doesn't contain one item");

    TInt media1Index = cacheTestHelper.ItemPool().FindInOrder(TGlxMediaId(KMediaId1), T_CacheTestHelpers::MediaItemOrderByKey);
    EUNIT_ASSERT_DESC(media1Index != KErrNotFound, "Incorrect item deleted from cache");

    EUNIT_PRINT(_L("Check user called to remove reference"));
    EUNIT_ASSERT_DESC(mediaUser->RemoveReferenceCalled(), "User not called to remove reference");

    CleanupStack::PopAndDestroy(mediaUser);
    }

void T_CGlxCache::T_CGlxCache_ReserveUsersLL(  )
    {
    // iCGlxCache->ReserveUsersL( <add parameters here > );
    EUNIT_ASSERT_DESC( EFalse, "Generated assert, replace with real");
    }
    
void T_CGlxCache::SetupCacheL()
    {
    // Create helper class
    T_CacheTestHelpers cacheTestHelper(iCGlxCache);

    CGlxMedia* media1 = new (ELeave) CGlxMedia(TGlxMediaId(KMediaId1));
    CleanupStack::PushL(media1);

    media1->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, EFalse);
    cacheTestHelper.ItemPool().AppendL(media1);

    CleanupStack::Pop(media1);

    CGlxMedia* media2 = new (ELeave) CGlxMedia(TGlxMediaId(KMediaId2));
    CleanupStack::PushL(media2);

    media2->SetTObjectValueL<TSize>(KGlxMediaGeneralDimensions, TSize(320, 240));
    cacheTestHelper.ItemPool().AppendL(media2);

    CleanupStack::Pop(media2);
    }

// Test cache observer
T_CGlxCache::CGlxCacheObserverTest* T_CGlxCache::CGlxCacheObserverTest::NewL()
    {
    CGlxCacheObserverTest* self = new (ELeave) CGlxCacheObserverTest();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

T_CGlxCache::CGlxCacheObserverTest::CGlxCacheObserverTest() :
        iAttributesAvailableNotified(EFalse),
        iIdSpaceId(KGlxIdNone),
        iMediaId(KGlxIdNone),
        iAttributes(),
        iMedia(NULL)
    {
    }

T_CGlxCache::CGlxCacheObserverTest::~CGlxCacheObserverTest()
    {
    iAttributes.Close();
    }

void T_CGlxCache::CGlxCacheObserverTest::ConstructL()
    {
    }

TBool T_CGlxCache::CGlxCacheObserverTest::AttributesAvailableNotified() const
    {
    return iAttributesAvailableNotified;
    }

void T_CGlxCache::CGlxCacheObserverTest::ResetCalls()
    {
    iAttributesAvailableNotified = EFalse;
    }

const TGlxIdSpaceId& T_CGlxCache::CGlxCacheObserverTest::IdSpaceId() const
    {
    return iIdSpaceId;
    }

const TGlxMediaId& T_CGlxCache::CGlxCacheObserverTest::MediaId() const
    {
    return iMediaId;
    }

const RArray<TMPXAttribute>& T_CGlxCache::CGlxCacheObserverTest::Attributes() const
    {
    return iAttributes;
    }

const CGlxMedia* T_CGlxCache::CGlxCacheObserverTest::Media() const
    {
    return iMedia;
    }

void T_CGlxCache::CGlxCacheObserverTest::HandleAttributesAvailableL(
        const TGlxIdSpaceId& aIdSpaceId, 
        const TGlxMediaId& aMediaId, 
        const RArray<TMPXAttribute>& aAttributes, 
        const CGlxMedia* aMedia)
    {
    iIdSpaceId = aIdSpaceId;
    iMediaId = aMediaId;

    iAttributes.Reset();
    TInt attributeCount = aAttributes.Count();
    for (TInt i = 0; i < attributeCount; ++i)
        {
        TMPXAttribute attribute = aAttributes[i];
        iAttributes.AppendL(attribute);
        }

    iMedia = const_cast<CGlxMedia*>(aMedia);

    iAttributesAvailableNotified = ETrue;
    }

// Test media user
T_CGlxCache::CGlxMediaUserTest* T_CGlxCache::CGlxMediaUserTest::NewL()
    {
    CGlxMediaUserTest* self = new (ELeave) CGlxMediaUserTest();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

T_CGlxCache::CGlxMediaUserTest::CGlxMediaUserTest() :
        iAttributesAvailableNotified(EFalse),
        iRemoveReferenceCalled(EFalse),
        iIdSpaceId(KGlxIdNone),
        iMediaId(KGlxIdNone),
        iAttributes()
    {
    }

T_CGlxCache::CGlxMediaUserTest::~CGlxMediaUserTest()
    {
    iAttributes.Close();
    }

void T_CGlxCache::CGlxMediaUserTest::ConstructL()
    {
    }

TBool T_CGlxCache::CGlxMediaUserTest::AttributesAvailableNotified() const
    {
    return iAttributesAvailableNotified;
    }

TBool T_CGlxCache::CGlxMediaUserTest::RemoveReferenceCalled() const
    {
    return iRemoveReferenceCalled;
    }

void T_CGlxCache::CGlxMediaUserTest::ResetCalls()
    {
    iAttributesAvailableNotified = EFalse;
    iRemoveReferenceCalled = EFalse;
    }

const TGlxIdSpaceId& T_CGlxCache::CGlxMediaUserTest::IdSpaceId() const
    {
    return iIdSpaceId;
    }

const TGlxMediaId& T_CGlxCache::CGlxMediaUserTest::MediaId() const
    {
    return iMediaId;
    }

const RArray<TMPXAttribute>& T_CGlxCache::CGlxMediaUserTest::Attributes() const
    {
    return iAttributes;
    }

void T_CGlxCache::CGlxMediaUserTest::HandleAttributesAvailableL(
        const TGlxIdSpaceId& aIdSpaceId, 
        const TGlxMediaId& aItemId, 
        const RArray<TMPXAttribute>& aAttributes)
    {
    iIdSpaceId = aIdSpaceId;
    iMediaId = aItemId;

    iAttributes.Reset();
    TInt attributeCount = aAttributes.Count();
    for (TInt i = 0; i < attributeCount; ++i)
        {
        TMPXAttribute attribute = aAttributes[i];
        iAttributes.AppendL(attribute);
        }

    iAttributesAvailableNotified = ETrue;
    }

void T_CGlxCache::CGlxMediaUserTest::RemoveReference(CGlxMedia& aMedia)
    {
    // This is related to T_CGlxCache_Delete_SubTest4L and T_CGlxCache_Delete_SubTest5L
    // T_CGlxCache_Delete_SubTest4L: Delete item where user doesn't have reference to media -> Don't remove reference
    // T_CGlxCache_Delete_SubTest5L: Delete item where user has reference to media -> Remove reference
    if (aMedia.Id().Value() == KMediaId2)
        {
        aMedia.RemoveUser(*this);
        }

    iRemoveReferenceCalled = ETrue;
    }
    
    
void T_CGlxCache::CGlxMediaUserTest::RemoveReference(TInt aIndex)
	{
	
	}


void T_CGlxCache::CGlxMediaUserTest:: HandleAttributesAvailableL(TInt aIndex,
 			 const RArray<TMPXAttribute>& aAttributes)
 	{
 	
 	}

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CGlxCache,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "IdSpaceId - test1",
    "CGlxCache",
    "IdSpaceId - test1",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_IdSpaceIdL, Teardown)
    
EUNIT_TEST(
    "MediaUpdatedL - test2",
    "CGlxCache",
    "MediaUpdatedL - test2",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_MediaUpdatedLL, Teardown)
    
EUNIT_TEST(
    "UpdateMediaL - test3",
    "CGlxCache",
    "UpdateMediaL - test3",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_UpdateMediaLL, Teardown)
    
EUNIT_TEST(
    "HandleItemModified - test4",
    "CGlxCache",
    "HandleItemModified - test4",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_HandleItemModifiedL, Teardown)
    
EUNIT_TEST(
    "Media - test5",
    "CGlxCache",
    "Media - test5",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_MediaL, Teardown)
    
EUNIT_TEST(
    "FindItemForceCreateL - test6",
    "CGlxCache",
    "FindItemForceCreateL - test6",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_FindItemForceCreateLL, Teardown)
    
EUNIT_TEST(
    "Count - test7",
    "CGlxCache",
    "Count - test7",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_CountL, Teardown)
    
EUNIT_TEST(
    "Media - test8",
    "CGlxCache",
    "Media - test8",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_Media2L, Teardown)
    
EUNIT_TEST(
    "Delete - test9, subtest1",
    "CGlxCache",
    "Delete - test9, subtest1",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_Delete_SubTest1L, Teardown)

EUNIT_TEST(
    "Delete - test9, subtest2",
    "CGlxCache",
    "Delete - test9, subtest2",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_Delete_SubTest2L, Teardown)

EUNIT_TEST(
    "Delete - test9, subtest3",
    "CGlxCache",
    "Delete - test9, subtest3",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_Delete_SubTest3L, Teardown)

EUNIT_TEST(
    "Delete - test9, subtest4",
    "CGlxCache",
    "Delete - test9, subtest4",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_Delete_SubTest4L, Teardown)

EUNIT_TEST(
    "Delete - test9, subtest5",
    "CGlxCache",
    "Delete - test9, subtest5",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_Delete_SubTest5L, Teardown)
    
/*EUNIT_TEST(
    "ReserveUsersL - test10",
    "CGlxCache",
    "ReserveUsersL - test10",
    "FUNCTIONALITY",
    SetupL, T_CGlxCache_ReserveUsersLL, Teardown)*/
    

EUNIT_END_TEST_TABLE

//  END OF FILE
