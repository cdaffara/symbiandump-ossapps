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
#include "ut_cglxitemlist.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <mpxcollectionpath.h>
#include <glxpanic.h>
    
//  INTERNAL INCLUDES
#include "glxitemlist.h"
#include "_glxnotification.h"

// -----------------------------------------------------------------------------
// Constructors 
// -----------------------------------------------------------------------------
//
CGlxItemListReconstruction* CGlxItemListReconstruction::NewL( TInt aReservationCount )
    {
    CGlxItemListReconstruction* self = new (ELeave) CGlxItemListReconstruction();
    CleanupStack::PushL( self );
    self->ConstructL( aReservationCount );
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// Test reconstruction
// -----------------------------------------------------------------------------
//
TBool CGlxItemListReconstruction::ReconstructionEquals() const
    {
    return ItemsEqual();
    }
    
// -----------------------------------------------------------------------------
// Constructors & destructors
// -----------------------------------------------------------------------------
//
ut_CGlxItemList* ut_CGlxItemList::NewL()
    {
    ut_CGlxItemList* self = ut_CGlxItemList::NewLC();
    CleanupStack::Pop();

    return self;
    }

ut_CGlxItemList* ut_CGlxItemList::NewLC()
    {
    ut_CGlxItemList* self = new( ELeave ) ut_CGlxItemList();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
ut_CGlxItemList::~ut_CGlxItemList()
    {
    }

// Default constructor
ut_CGlxItemList::ut_CGlxItemList()
    {
    }

// Second phase construct
void ut_CGlxItemList::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
        
// -----------------------------------------------------------------------------
// Setup & teardown
// -----------------------------------------------------------------------------
//
void ut_CGlxItemList::SetupL(  )
    {
    }

void ut_CGlxItemList::Teardown(  )
    {
    DeleteMembers();
    }
    
// -----------------------------------------------------------------------------
// Test SetContentsL
// -----------------------------------------------------------------------------
//
void ut_CGlxItemList::T_SetContentsLL(  )
    {
        // Test population of empty list with empty list
        {
        TNotification::TData notifications[] = { { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8(""),
                                        _L8(""),
                                        notifications ) );  
        } 

        // Test additions /////////////////////////////////////////////////////////                                    

        // Test population of empty list
        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 0, 3 }, 
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8(""),
                                        _L8("abc"),
                                        notifications ) ); 
        }

        // Test population of empty list
        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 0, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8(""),
                                        _L8("a"),
                                        notifications ) ); 
        }
        // Test insert at end of list
        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 1, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("a"),
                                        _L8("ab"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 1, 2 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("a"),
                                        _L8("abc"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 2, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("ab"),
                                        _L8("abc"),
                                        notifications ) ); 
        }
        // Test insert at the beginning of the list
        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 0, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("b"),
                                        _L8("ab"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 0, 2 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("c"),
                                        _L8("abc"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 0, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("bc"),
                                        _L8("abc"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 0, 3 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("de"),
                                        _L8("abcde"),
                                        notifications ) ); 
        }

        // Test insert at the middle of the list
        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 1, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("ac"),
                                        _L8("abc"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 1, 2 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("ad"),
                                        _L8("abcd"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 2, 3 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abf"),
                                        _L8("abcdef"),
                                        notifications ) ); 
        }

        
        // Test multiple insertions
        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 0, 1 }, // a
                               { TNotification::EAdd, 2, 1 }, // c
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("b"),
                                        _L8("abc"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 0, 2 }, // ab
                               { TNotification::EAdd, 3, 2 }, // de
                               { TNotification::EAdd, 6, 2 }, // gh
                               { TNotification::EAdd, 9, 1 }, // j
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("cfi"),
                                        _L8("abcdefghij"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::EAdd, 2, 3 }, // cde
                               { TNotification::EAdd, 6, 1 }, // g
                               { TNotification::EAdd, 8, 1 }, // i
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abfhj"),
                                        _L8("abcdefghij"),
                                        notifications ) ); 
        }

        
        // Test removals //////////////////////////////////////////////////////////
                                          
        // Test list being made empty
        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 0, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("a"),
                                        _L8(""),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 0, 3 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abc"),
                                        _L8(""),
                                        notifications ) ); 
        }

        
        // Remove from end
        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 1, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("ab"),
                                        _L8("a"),
                                        notifications ) ); 
        }

        
        // Remove from end
        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 1, 5 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abcdef"),
                                        _L8("a"),
                                        notifications ) ); 
        }

        
        // Remove from beginning
        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 0, 5 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abcdef"),
                                        _L8("f"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 0, 1 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("af"),
                                        _L8("f"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 0, 3 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abcdef"),
                                        _L8("def"),
                                        notifications ) ); 
        }

        // Remove from middle
        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 1, 4 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abcdef"),
                                        _L8("af"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 2, 2 },
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abcdef"),
                                        _L8("abef"),
                                        notifications ) ); 
        }

        // Test multiple removals
        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 1, 2 }, // bc
                               { TNotification::ERemove, 2, 1 }, // e
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abcdef"),
                                        _L8("adf"),
                                        notifications ) ); 
        }

        {
        TNotification::TData notifications[] = 
                             { { TNotification::ERemove, 0, 1 }, // a
                               { TNotification::ERemove, 1, 1 }, // c
                               { TNotification::ERemove, 3, 3 }, // fgh
                               { TNotification::ERemove, 5, 1 }, // k
                               { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abcdefghijk"),
                                        _L8("bdeij"),
                                        notifications ) ); 
        }
        
        // Test simultaneous additions and removals ///////////////////////////////
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 1 }, // a
                                  { TNotification::EAdd,    0, 3 }, // 123
                                  { TNotification::EAdd,    4, 2 }, // 45
                                  { TNotification::ERemove, 7, 1 }, // d
                                  { TNotification::ERemove, 8, 2 }, // fg
                                  { TNotification::EAdd,    8, 3 }, // 678
                                  { TNotification::ERemove, 13, 2 }, // jk
                                  { TNotification::EAdd,    13, 5 }, // 90#¤%
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abcdefghijk"),
                                        _L8("123b45ce678hi90#¤%"),
                                        notifications ) );
        }

        // Test replacement ///////////////////////////////////////////////////////
        {
        TNotification::TData notifications[] = 
                                 { { TNotification::ERemove, 0, 26 }, // abcdefghijkolmnopqrstuvwxyz
                                   { TNotification::EAdd,    0, 10 }, // 1234567890
                                   { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("abcdefghijklmnopqrstuvwxyz"),
                                        _L8("1234567890"),
                                        notifications ) );
        }
        
        {
        TNotification::TData notifications[] = 
                               { { TNotification::ERemove,    0, 10 }, // 1234567890
                                 { TNotification::EAdd, 0, 26 }, // abcdefghijkolmnopqrstuvwxyz
                                 { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL ( _L8("1234567890"),
                                        _L8("abcdefghijklmnopqrstuvwxyz"),
                                        notifications ) );
        }
    // Test reorder ///////////////////////////////////////////////////////////
    // don't really care about the order of notifications, only that reconstruction 
    // matches the original during sync and at the end. During sync is checked by 
    // HandleItemsAdded and HandleItemsRemoved. After sync is checked here.
    EUNIT_ASSERT( TrySetContentsL ( _L8("abcdefghijklmnopqrstuvwxyz"),
                                    _L8("akjipebvwzcd") ) );
    EUNIT_ASSERT( TrySetContentsL ( _L8("akjipebvwzcd"),
                                    _L8("abcdefghijklmnopqrstuvwxyz") ) );
    EUNIT_ASSERT( TrySetContentsL ( _L8("cdabkoljnpqsrihyzx"),
                                    _L8("abcdefghijklmnopqrstuvwxyz") ) );
    EUNIT_ASSERT( TrySetContentsL ( _L8("abcdefghijklmnopqrstuvwxyz"),
                                    _L8("cdabkoljnpqsrihyzx") ) );
    EUNIT_ASSERT( TrySetContentsL ( _L8("abcdefghijklmnopqrstuvwxyz"),
                                    _L8("qrscdehizyjabklmnoptuvfgwx") ) );
    }

// -----------------------------------------------------------------------------
// Test Remove(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxItemList::T_RemoveL() 
    {
    // Test by creating a list, and removing items from it
        {
        TNotification::TData notifications[] = 
                               { { TNotification::ERemove, 0, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryRemoveL( _L8("abcdefg"),
                                  'a',
                                  _L8("bcdefg"),
                                  notifications ) );
        }

        {
        TNotification::TData notifications[] = 
                               { { TNotification::ERemove, 2, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryRemoveL( _L8("abcdefg"),
                                  'c',
                                  _L8("abdefg"),
                                  notifications ) );
        }

        {
        TNotification::TData notifications[] = 
                               { { TNotification::ERemove, 6, 1 }, 
                                 { END_MARKER } };
        EUNIT_ASSERT( TryRemoveL( _L8("abcdefg"),
                                  'g',
                                  _L8("abcdef"),
                                  notifications ) );
        }
    }
    
// -----------------------------------------------------------------------------
// Test RemoveReference(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxItemList::T_RemoveReferenceL(  )
    {
    // Create the item list
    CreateListL( _L8("abcdefg"), _L8("tfttfft") );

    EUNIT_ASSERT( IsLinkedToMedia( _L8("tfttfft") ) );
    EUNIT_ASSERT( TryRemoveReference( 0, _L8("ffttfft") ) );
    EUNIT_ASSERT( TryRemoveReference( 2, _L8("ffftfft") ) );
    EUNIT_ASSERT( TryRemoveReference( 3, _L8("fffffft") ) );
    EUNIT_ASSERT( TryRemoveReference( 6, _L8("fffffff") ) );
    EUNIT_ASSERT_PANIC_DESC( TryRemoveReference( KErrNotFound, _L8("tfttfft") ), 
                             "Images & videos", EGlxPanicIllegalArgument, 
                             "Incorrect panic" );
    }

// -----------------------------------------------------------------------------
// Test Index(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxItemList::T_IndexL(  )
    {
    // Create the item list
    CreateListL( _L8("abcdefg") );
    
    EUNIT_ASSERT_DESC( iList->Index(KListIdSpace, TGlxMediaId('a') ) == 0, "index match a" );
    EUNIT_ASSERT_DESC( iList->Index(KListIdSpace, TGlxMediaId('c') ) == 2, "index match c" );
    EUNIT_ASSERT_DESC( iList->Index(KListIdSpace, TGlxMediaId('g') ) == 6, "index match g" );
    EUNIT_ASSERT_DESC( iList->Index(KListIdSpace, TGlxMediaId('h') ) == KErrNotFound, "index no match" );
    }
    
// -----------------------------------------------------------------------------
// Test Count()
// -----------------------------------------------------------------------------
//
void ut_CGlxItemList::T_CountL(  )
    {
    // test count of new list
    CreateReconstructionL();
    iList = CGlxItemList::NewL( TGlxIdSpaceId( 1 ), *iReconstruction, *this );
    EUNIT_ASSERT_DESC( iList->Count() == 0, "count 0" );
    
    // test count of non-empty list
    CreateListL( _L8("abcdefg") );
    EUNIT_ASSERT_DESC( iList->Count() == 7, "count 7" );
    }
    
// -----------------------------------------------------------------------------
// Test Item(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxItemList::T_ItemL(  )
    {
    // Create the item list
    CreateListL( _L8("abcdefg") );
    // test item
    EUNIT_ASSERT_DESC( iList->Item(0).Id() == TGlxMediaId('a'), "item a" );
    EUNIT_ASSERT_DESC( iList->Item(2).Id() == TGlxMediaId('c'), "item c" );
    EUNIT_ASSERT_DESC( iList->Item(6).Id() == TGlxMediaId('g'), "item g" );
    }
    
// -----------------------------------------------------------------------------
// Test IdSpaceId
// -----------------------------------------------------------------------------
//
void ut_CGlxItemList::T_IdSpaceIdL()
    {
    TestIdSpaceIdL();
    }
    
// -----------------------------------------------------------------------------
// Test table
// -----------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    ut_CGlxItemList,
    "Add test suite description here.",
    "UNIT" )
    
EUNIT_TEST(
    "SetContentsL",
    "CGlxItemList",
    "SetContentsL",
    "FUNCTIONALITY",
    SetupL, T_SetContentsLL, Teardown)
    
EUNIT_TEST(
    "Remove",
    "CGlxItemList",
    "Remove",
    "FUNCTIONALITY",
    SetupL, T_RemoveL, Teardown)
    
EUNIT_TEST(
    "RemoveReference",
    "CGlxItemList",
    "RemoveReference",
    "FUNCTIONALITY",
    SetupL, T_RemoveReferenceL, Teardown)
    
EUNIT_TEST(
    "Index",
    "CGlxItemList",
    "Index",
    "FUNCTIONALITY",
    SetupL, T_IndexL, Teardown)    
    
EUNIT_TEST(
    "Count",
    "CGlxItemList",
    "Count",
    "FUNCTIONALITY",
    SetupL, T_CountL, Teardown)
    
EUNIT_TEST(
    "Item",
    "CGlxItemList",
    "Item",
    "FUNCTIONALITY",
    SetupL, T_ItemL, Teardown)
    
EUNIT_TEST(
    "Id space id",
    "CGlxItemList",
    "IdSpaceId",
    "FUNCTIONALITY",
    SetupL, T_IdSpaceIdL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
