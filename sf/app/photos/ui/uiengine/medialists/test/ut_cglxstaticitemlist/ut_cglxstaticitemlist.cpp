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
* Description:   Unit tests
*
*/


 
//  CLASS HEADER
#include "ut_cglxstaticitemlist.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "glxstaticitemlist.h"
#include "_glxlisttesthelpers.h"

using namespace NGlxListDefs;

// Add no static items
NGlxListTestHelpers::TStaticItemInfo::TData info_add_none[] = 
    { { K_STATIC_ITEMS_END } };

// Add a static item at front
NGlxListTestHelpers::TStaticItemInfo::TData info_add_front_0[] = 
    { { '0', NGlxListDefs::EInsertFirst },
      { K_STATIC_ITEMS_END } };

// Add a static item at front
NGlxListTestHelpers::TStaticItemInfo::TData info_add_rear_0[] = 
    { { '0', NGlxListDefs::EInsertLast },
      { K_STATIC_ITEMS_END } };
      
// Add two static items at front
NGlxListTestHelpers::TStaticItemInfo::TData info_add_front_01[] = 
    { { '1', NGlxListDefs::EInsertFirst },
      { '0', NGlxListDefs::EInsertFirst },
      { K_STATIC_ITEMS_END } };

      
// Add two static items at front
NGlxListTestHelpers::TStaticItemInfo::TData info_add_front_0_rear_12[] = 
    { { '0', NGlxListDefs::EInsertFirst },
      { '1', NGlxListDefs::EInsertLast },
      { '2', NGlxListDefs::EInsertLast },
      { K_STATIC_ITEMS_END } };
      
// -----------------------------------------------------------------------------
// Constructors 
// -----------------------------------------------------------------------------
//
CGlxStaticItemListReconstruction* CGlxStaticItemListReconstruction::NewL( TInt aReservationCount )
    {
    CGlxStaticItemListReconstruction* self = new (ELeave) CGlxStaticItemListReconstruction();
    CleanupStack::PushL( self );
    self->ConstructL( aReservationCount );
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// Reconstruction
// -----------------------------------------------------------------------------
//
TBool CGlxStaticItemListReconstruction::ReconstructionEquals() const
    {
    return ItemsEqual();
    }

// -----------------------------------------------------------------------------
// Constructors & destructors
// -----------------------------------------------------------------------------
//
ut_CGlxStaticItemList* ut_CGlxStaticItemList::NewL()
    {
    ut_CGlxStaticItemList* self = ut_CGlxStaticItemList::NewLC();
    CleanupStack::Pop();

    return self;
    }

ut_CGlxStaticItemList* ut_CGlxStaticItemList::NewLC()
    {
    ut_CGlxStaticItemList* self = new( ELeave ) ut_CGlxStaticItemList();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
ut_CGlxStaticItemList::~ut_CGlxStaticItemList()
    {
    }

// Default constructor
ut_CGlxStaticItemList::ut_CGlxStaticItemList()
    {
    }

// Second phase construct
void ut_CGlxStaticItemList::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// -----------------------------------------------------------------------------
// Setup & Teardown
// -----------------------------------------------------------------------------
//
void ut_CGlxStaticItemList::SetupL(  )
    {
    }
    

void ut_CGlxStaticItemList::Teardown(  )
    {
    DeleteMembers();
    }
    
// -----------------------------------------------------------------------------
// Test SetContentsL(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxStaticItemList::T_SetContentsLL(  )
    {
    TNotification::TData notificationsStaticItemsDisabled[] = 
                            { { TNotification::ERemove, 0, 1 }, // a
                              { TNotification::EAdd,    0, 3 }, // ABC
                              { TNotification::EAdd,    4, 2 }, // DE
                              { TNotification::ERemove, 7, 1 }, // d
                              { TNotification::ERemove, 8, 2 }, // fg
                              { TNotification::EAdd,    8, 3 }, // FGH
                              { END_MARKER } };
    TNotification::TData notificationsStaticItemsEnabled[] = 
                            { { TNotification::ERemove, 1, 1 }, // a
                              { TNotification::EAdd,    1, 3 }, // ABC
                              { TNotification::EAdd,    5, 2 }, // DE
                              { TNotification::ERemove, 8, 1 }, // d
                              { TNotification::ERemove, 9, 2 }, // fg
                              { TNotification::EAdd,    9, 3 }, // FGH
                              { END_MARKER } };
    EUNIT_ASSERT( TrySetContentsL ( _L8("abcdefg"),         // original list
                                    _L8("ABCbDEceFGH"),     // new list
                                    _L8("0ABCbDEceFGH12"),  // new list with static items
                                    notificationsStaticItemsEnabled, 
                                    notificationsStaticItemsDisabled,
                                    info_add_front_0_rear_12 ) );
    }
    
// -----------------------------------------------------------------------------
// Test RemoveReference(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxStaticItemList::T_RemoveReferenceL(  )
    {
    TestRemoveReferenceL();
    }

// -----------------------------------------------------------------------------
// Test Remove(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxStaticItemList::T_RemoveL() 
    {
        { // Test removing first item
        TNotification::TData notificationStaticItemsDisabled[] = 
                               { { TNotification::ERemove, 0, 1 }, 
                                 { END_MARKER } };
        TNotification::TData notificationStaticItemsEnabled[] = 
                               { { TNotification::ERemove, 1, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryRemoveL( _L8("abcdefg"),
                                  'a', EFalse,
                                  _L8("bcdefg"),
                                  _L8("0bcdefg12"),
                                  notificationStaticItemsDisabled,
                                  notificationStaticItemsEnabled,
                                  info_add_front_0_rear_12 ) );
        }

        { // Test removing middle item
        TNotification::TData notificationStaticItemsDisabled[] = 
                               { { TNotification::ERemove, 2, 1 }, 
                                 { END_MARKER } };
        TNotification::TData notificationStaticItemsEnabled[] = 
                               { { TNotification::ERemove, 3, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryRemoveL( _L8("abcdefg"),
                                  'c', EFalse,
                                  _L8("abdefg"),
                                  _L8("0abdefg12"),
                                  notificationStaticItemsDisabled,
                                  notificationStaticItemsEnabled,
                                  info_add_front_0_rear_12 ) );
        }

        { // Test removing last item
        TNotification::TData notificationStaticItemsDisabled[] = 
                               { { TNotification::ERemove, 6, 1 }, 
                                 { END_MARKER } };
        TNotification::TData notificationStaticItemsEnabled[] = 
                               { { TNotification::ERemove, 7, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryRemoveL( _L8("abcdefg"),
                                  'g', EFalse,
                                  _L8("abcdef"),
                                  _L8("0abcdef12"),
                                  notificationStaticItemsDisabled,
                                  notificationStaticItemsEnabled,
                                  info_add_front_0_rear_12 ) );
        }
        
        { // Test removing front static item
        TNotification::TData notificationStaticItemsDisabled[] = 
                               { { END_MARKER } };
        TNotification::TData notificationStaticItemsEnabled[] = 
                               { { TNotification::ERemove, 0, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryRemoveL( _L8("abcdefg"),
                                  '0', ETrue,
                                  _L8("abcdefg"),
                                  _L8("abcdefg12"),
                                  notificationStaticItemsDisabled,
                                  notificationStaticItemsEnabled,
                                  info_add_front_0_rear_12 ) );
        }
        
        { // Test removing rear static item 1
        TNotification::TData notificationStaticItemsDisabled[] = 
                               { { END_MARKER } };
        TNotification::TData notificationStaticItemsEnabled[] = 
                               { { TNotification::ERemove, 8, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryRemoveL( _L8("abcdefg"),
                                  '1', ETrue,
                                  _L8("abcdefg"),
                                  _L8("0abcdefg2"),
                                  notificationStaticItemsDisabled,
                                  notificationStaticItemsEnabled,
                                  info_add_front_0_rear_12 ) );
        }
        
        { // Test removing rear static item 2
        TNotification::TData notificationStaticItemsDisabled[] = 
                               { { END_MARKER } };
        TNotification::TData notificationStaticItemsEnabled[] = 
                               { { TNotification::ERemove, 9, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryRemoveL( _L8("abcdefg"),
                                  '2', ETrue,
                                  _L8("abcdefg"),
                                  _L8("0abcdefg1"),
                                  notificationStaticItemsDisabled,
                                  notificationStaticItemsEnabled,
                                  info_add_front_0_rear_12 ) );
        }
        
        { // Test removing non-existent static item
        TNotification::TData notificationStaticItemsDisabled[] = 
                               { { END_MARKER } };
        TNotification::TData notificationStaticItemsEnabled[] = 
                               { { END_MARKER } };
        EUNIT_ASSERT( TryRemoveL( _L8("abcdefg"),
                                  '#', ETrue,
                                  _L8("abcdefg"),
                                  _L8("0abcdefg12"),
                                  notificationStaticItemsDisabled,
                                  notificationStaticItemsEnabled,
                                  info_add_front_0_rear_12 ) );
        }
    }

// -----------------------------------------------------------------------------
// Test AddStaticItemL(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxStaticItemList::T_AddStaticItemLL(  )
    {
        { // Test adding first static item to front, empty list
        TNotification::TData notifications[] = 
                               { { TNotification::EAdd, 0, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8(""),
                                  '0', NGlxListDefs::EInsertFirst,
                                  _L8("0"),
                                  notifications, notifications,
                                  info_add_none ) );
        }

        { // Test adding first static item to rear, empty list
        TNotification::TData notifications[] = 
                               { { TNotification::EAdd, 0, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8(""),
                                  '0', NGlxListDefs::EInsertLast,
                                  _L8("0"),
                                  notifications, notifications,
                                  info_add_none ) );
        }

        { // Test adding first static item to rear, empty list
        TNotification::TData notifications[] = 
                               { { TNotification::EAdd, 0, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8(""),
                                  '0', NGlxListDefs::EInsertLast,
                                  _L8("0"),
                                  notifications, notifications,
                                  info_add_none ) );
        }

        { // Test adding first static item to front
        TNotification::TData notifications[] = 
                               { { TNotification::EAdd, 0, 1 }, 
                                 { END_MARKER } };
        TNotification::TData notificationsWithAllStaticItems[] = 
                               { { TNotification::EAdd, 0, 2 }, 
                                 { TNotification::EAdd, 2, 2 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8(""),
                                  '3', NGlxListDefs::EInsertFirst,
                                  _L8("3012"),
                                  notifications, notificationsWithAllStaticItems,
                                  info_add_front_0_rear_12 ) );
        }
        
        { // Test adding first static item to rear
        TNotification::TData notifications[] = 
                               { { TNotification::EAdd, 3, 1 }, 
                                 { END_MARKER } };
        TNotification::TData notificationsWithAllStaticItems[] = 
                               { { TNotification::EAdd, 0, 1 }, 
                                 { TNotification::EAdd, 1, 3 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8(""),
                                  '3', NGlxListDefs::EInsertLast,
                                  _L8("0123"),
                                  notifications, notificationsWithAllStaticItems,
                                  info_add_front_0_rear_12 ) );
        }

        { // Test adding first static item to rear
        TNotification::TData notifications[] = 
                               { { TNotification::EAdd, 4, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8("abcd"),
                                  '0', NGlxListDefs::EInsertLast,
                                  _L8("abcd0"),
                                  notifications, notifications,
                                  info_add_none ) );
        }

        { // Test adding static item to front
        TNotification::TData notifications[] = 
                               { { TNotification::EAdd, 0, 1 }, 
                                 { END_MARKER } };
        TNotification::TData notificationsWithAllStaticItems[] = 
                               { { TNotification::EAdd, 0, 2 }, 
                                 { TNotification::EAdd, 6, 2 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8("abcd"),
                                  '3', NGlxListDefs::EInsertFirst,
                                  _L8("30abcd12"),
                                  notifications, notificationsWithAllStaticItems,
                                  info_add_front_0_rear_12 ) );
        }

    // Test adding a static item that already exists

        { // Test adding existing static item to front
        TNotification::TData notifications[] = 
                               { { END_MARKER } };
        TNotification::TData notificationsWithAllStaticItems[] = 
                               { { TNotification::EAdd, 0, 1 }, 
                                 { TNotification::EAdd, 5, 2 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8("abcd"),
                                  '0', NGlxListDefs::EInsertFirst,
                                  _L8("0abcd12"),
                                  notifications, notificationsWithAllStaticItems,
                                  info_add_front_0_rear_12 ) );
        }

        { // Test adding existing static item to rear
        TNotification::TData notifications[] = 
                               { { END_MARKER } };
        TNotification::TData notificationsWithAllStaticItems[] = 
                               { { TNotification::EAdd, 0, 1 }, 
                                 { TNotification::EAdd, 5, 2 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8("abcd"),
                                  '2', NGlxListDefs::EInsertLast,
                                  _L8("0abcd12"),
                                  notifications, notificationsWithAllStaticItems,
                                  info_add_front_0_rear_12 ) );
        }

    // Test adding a static item that already exists, but in different end of list
        { // Test adding existing static item to front, when old on is at rear
        TNotification::TData notifications[] = 
                               { { TNotification::ERemove, 5, 1 }, 
                                 { TNotification::EAdd, 0, 1 }, 
                                 { END_MARKER } };
        TNotification::TData notificationsWithAllStaticItems[] = 
                               { { TNotification::EAdd, 0, 2 }, 
                                 { TNotification::EAdd, 6, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8("abcd"),
                                  '1', NGlxListDefs::EInsertFirst,
                                  _L8("10abcd2"),
                                  notifications, notificationsWithAllStaticItems,
                                  info_add_front_0_rear_12 ) );
        }

        { // Test adding existing static item to rear, when old on is at front
        TNotification::TData notifications[] = 
                               { { TNotification::ERemove, 0, 1 }, 
                                 { TNotification::EAdd, 6, 1 }, 
                                 { END_MARKER } };
        TNotification::TData notificationsWithAllStaticItems[] = 
                               { { TNotification::EAdd, 4, 3 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryAddStaticItemL( _L8("abcd"),
                                  '0', NGlxListDefs::EInsertLast,
                                  _L8("abcd120"),
                                  notifications, notificationsWithAllStaticItems,
                                  info_add_front_0_rear_12 ) );
        }
    }
    
// -----------------------------------------------------------------------------
// Test SetStaticItemsEnabled(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxStaticItemList::T_SetStaticItemsEnabledL(  )
    {
    // Test list with no normal items
        { // Add to front
        TNotification::TData notificationsForEnable[] = 
                             { { TNotification::EAdd, 0, 1 },
                               { END_MARKER } };
        TNotification::TData notificationsForDisable[] = 
                             { { TNotification::ERemove, 0, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetStaticItemsEnabledL( _L8(""), 
                                                 _L8("0"),
                                                 info_add_front_0,
                                                 notificationsForEnable,
                                                 notificationsForDisable ) );    
        }

        { // Add to front
        TNotification::TData notificationsForEnable[] = 
                             { { TNotification::EAdd, 0, 2 },
                               { END_MARKER } };
        TNotification::TData notificationsForDisable[] = 
                             { { TNotification::ERemove, 0, 2 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetStaticItemsEnabledL( _L8(""), 
                                                 _L8("01"),
                                                 info_add_front_01,
                                                 notificationsForEnable,
                                                 notificationsForDisable ) );    
        }

        { // Add to front and rear
        TNotification::TData notificationsForEnable[] = 
                             { { TNotification::EAdd, 0, 1 },
                               { TNotification::EAdd, 1, 2 },
                               { END_MARKER } };
        TNotification::TData notificationsForDisable[] = 
                             { { TNotification::ERemove, 0, 1 },
                               { TNotification::ERemove, 0, 2 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetStaticItemsEnabledL( _L8(""), 
                                                 _L8("012"),
                                                 info_add_front_0_rear_12,
                                                 notificationsForEnable,
                                                 notificationsForDisable ) );    
        }
        
    // Test list with also normal items
        { // Add to front
        TNotification::TData notificationsForEnable[] = 
                             { { TNotification::EAdd, 0, 1 },
                               { END_MARKER } };
        TNotification::TData notificationsForDisable[] = 
                             { { TNotification::ERemove, 0, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetStaticItemsEnabledL( _L8("abcde"), 
                                                 _L8("0abcde"),
                                                 info_add_front_0,
                                                 notificationsForEnable,
                                                 notificationsForDisable ) );    
        }

        { // Add to front
        TNotification::TData notificationsForEnable[] = 
                             { { TNotification::EAdd, 0, 2 },
                               { END_MARKER } };
        TNotification::TData notificationsForDisable[] = 
                             { { TNotification::ERemove, 0, 2 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetStaticItemsEnabledL( _L8("abcde"), 
                                                 _L8("01abcde"),
                                                 info_add_front_01,
                                                 notificationsForEnable,
                                                 notificationsForDisable ) );    
        }

        { // Add to front and rear
        TNotification::TData notificationsForEnable[] = 
                             { { TNotification::EAdd, 0, 1 },
                               { TNotification::EAdd, 6, 2 },
                               { END_MARKER } };
        TNotification::TData notificationsForDisable[] = 
                             { { TNotification::ERemove, 0, 1 },
                               { TNotification::ERemove, 5, 2 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetStaticItemsEnabledL( _L8("abcde"), 
                                                 _L8("0abcde12"),
                                                 info_add_front_0_rear_12,
                                                 notificationsForEnable,
                                                 notificationsForDisable ) );    
        }
    }

// -----------------------------------------------------------------------------
// Test Count(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxStaticItemList::T_CountL(  )
    {
    TestCountL();
    }
    
// -----------------------------------------------------------------------------
// Test Item(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxStaticItemList::T_ItemL(  )
    {
    TestItemL();
    }
    
// -----------------------------------------------------------------------------
// Test Index(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxStaticItemList::T_IndexL(  )
    {
    TestIndexL();
    }
    
// -----------------------------------------------------------------------------
// Test IdSpaceId
// -----------------------------------------------------------------------------
//
void ut_CGlxStaticItemList::T_IdSpaceIdL()
    {
    TestIdSpaceIdL();
    }    
    
// -----------------------------------------------------------------------------
// Test table
// -----------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    ut_CGlxStaticItemList,
    "Static item list test suite.",
    "UNIT" )
    
EUNIT_TEST(
    "AddStaticItemL",
    "CGlxStaticItemList",
    "AddStaticItemL",
    "FUNCTIONALITY",
    SetupL, T_AddStaticItemLL, Teardown)
        
EUNIT_TEST(
    "SetStaticItemsEnabled",
    "CGlxStaticItemList",
    "SetStaticItemsEnabled",
    "FUNCTIONALITY",
    SetupL, T_SetStaticItemsEnabledL, Teardown)
    
EUNIT_TEST(
    "SetContentsL",
    "CGlxStaticItemList",
    "SetContentsL",
    "FUNCTIONALITY",
    SetupL, T_SetContentsLL, Teardown)
    
EUNIT_TEST(
    "Remove",
    "CGlxStaticItemList",
    "Remove",
    "FUNCTIONALITY",
    SetupL, T_RemoveL, Teardown)
    
EUNIT_TEST(
    "RemoveReference",
    "CGlxStaticItemList",
    "RemoveReference",
    "FUNCTIONALITY",
    SetupL, T_RemoveReferenceL, Teardown)

EUNIT_TEST(
    "Count",
    "CGlxStaticItemList",
    "Count",
    "FUNCTIONALITY",
    SetupL, T_CountL, Teardown)
    
EUNIT_TEST(
    "Item",
    "CGlxStaticItemList",
    "Item",
    "FUNCTIONALITY",
    SetupL, T_ItemL, Teardown)
    
EUNIT_TEST(
    "Index",
    "CGlxStaticItemList",
    "Index",
    "FUNCTIONALITY",
    SetupL, T_IndexL, Teardown)
    
EUNIT_TEST(
    "Id space id",
    "CGlxStaticItemList",
    "IdSpaceId",
    "FUNCTIONALITY",
    SetupL, T_IdSpaceIdL, Teardown)
        
EUNIT_END_TEST_TABLE

//  END OF FILE
