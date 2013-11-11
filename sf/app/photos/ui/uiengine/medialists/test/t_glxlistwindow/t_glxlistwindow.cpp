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
#include "t_glxlistwindow.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
 
//  INTERNAL INCLUDES
#include "glxlistwindow.h"

// -----------------------------------------------------------------------------
// Constructors & destructors
// -----------------------------------------------------------------------------
//
t_CGlxListWindow* t_CGlxListWindow::NewL()
    {
    t_CGlxListWindow* self = t_CGlxListWindow::NewLC();
    CleanupStack::Pop();

    return self;
    }

t_CGlxListWindow* t_CGlxListWindow::NewLC()
    {
    t_CGlxListWindow* self = new( ELeave ) t_CGlxListWindow();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
t_CGlxListWindow::~t_CGlxListWindow()
    {
    }

// Default constructor
t_CGlxListWindow::t_CGlxListWindow()
    {
    }

// Second phase construct
void t_CGlxListWindow::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// -----------------------------------------------------------------------------
// Setup & Teardown
// -----------------------------------------------------------------------------
//
void t_CGlxListWindow::SetupL(  )
    {
    }

void t_CGlxListWindow::Teardown(  )
    {
    }    

// return string length
TInt strlen( const char* aString )
    {
    // poor man's strlen...
    TInt count = 0;
    const char* tmp = aString;
    while ( *tmp ) 
    	{
    	count++;
    	tmp++;
    	}
    return count;
    }

// entry object for the window
class CEntry : public CBase
    {
public: 
    ~CEntry()
        {
        delete iOwned;
        }
        
    void ConstructL() 
        {
        iOwned = HBufC::NewL( 1 );
        }
    
    void SetId( TChar aId ) 
        {
        iId = aId;
        iId.LowerCase();
        }
        
    TChar Id() const
        {
        return iId;
        }
    
    TInt iCleanupCount;
private:
    TChar iId;
    HBufC* iOwned; // have an owned object to make sure destructor is being called
    };
    
// window class
class CTestWindow : public CGlxListWindow,
                    public MGlxWindowObjectFactory
    {
public:
    CTestWindow( const char* aList ) 
            : CGlxListWindow( static_cast< MGlxWindowObjectFactory& >( *this ) )
        {
        iList = aList;
        iOldList = iList;
        }
        
    ~CTestWindow() 
        {
        iEntries.Close();
        }
        
    // From MGlxWindowObjectFactory
	CBase* CreateObjectL() const 
        {
        CEntry* entry = new ( ELeave ) CEntry;
        CleanupStack::PushL( entry );
        entry->ConstructL();
        const_cast< CTestWindow* >( this )->iEntries.AppendL( entry );
        CleanupStack::Pop( entry );
        return entry;
        } 
	
    // From MGlxWindowObjectFactory
	void SetupObject( TInt aListIndex, CBase& aEntry ) 
        {
        CEntry& entry = static_cast< CEntry& >( aEntry );
        // initialise the entry by setting the id from the original list
        entry.SetId( iList[ aListIndex ] );
        }
	
    // From MGlxWindowObjectFactory
	void CleanupObject( TInt aListIndex, CBase& aEntry ) 
        {
        CEntry& entry = static_cast< CEntry& >( aEntry );
       
        EUNIT_ASSERT_DESC( iOldList[ aListIndex ] == entry.Id(), "list index is correct" );
        
        // clean up the entry 
        entry.SetId( 0 );
        // check how many times entry is cleaned up so can check that object 
        // is not cleaned up any more often that it needs to 
        entry.iCleanupCount++; 
        }
    
    const CEntry* operator[]( TInt aIndex ) const
        {
        return static_cast< const CEntry* >( At( aIndex ) );
        }
        
    const char* iList; // main list of items. not own
    const char* iOldList; // list of items before change. not own
    RPointerArray< CEntry > iEntries; // not own. owned by base class
    };

struct TWindowInfo
    {
    TWindowInfo() 
        {
        iStartIndex = KErrNotFound;
        iFocusIndex = KErrNotFound;
        iWindowSize = 0;
        }
    TInt iStartIndex;
    TInt iFocusIndex;
    TInt iWindowSize;
    TInt iTotalSize;
    };
    
TWindowInfo AnalyzeWindow( const char* aWindow )
    {
    TWindowInfo info;
    TInt index = 0;
    TChar ch = 0;
    // gapBefore is to set the start index in case list is "###----###"
    // i.e., gapBefore is true if there is a gap in the window before 
    // the index
    TBool gapBefore = EFalse;
    while ( 0 != ( ch = aWindow[ index ] ) )
        {
        if ( '-' == ch ) // is item a gap (outside window)
            {
            gapBefore = ETrue;
            }
        else
            {
            info.iWindowSize++;

            if ( ch.IsUpper() ) // focus index?
                {
                ASSERT( KErrNotFound == info.iFocusIndex ); // Check not multiple focus indexes defined
                info.iFocusIndex = index;
                }

            if ( gapBefore ||                        // this is for "###----S##"
                 KErrNotFound == info.iStartIndex )  // this is for "--S####---"
                {
                info.iStartIndex = index;
                gapBefore = EFalse;
                }
            }
        index++;
        }
    info.iTotalSize = index;
    return info;
    }

// aWindow: "---abcDe---" 
//          - is non-window
//          a,b,c,e is id
//          D focus
void AssertWindow( const CTestWindow& aWindow, const char* aWindowString )
    {
    TWindowInfo info = AnalyzeWindow( aWindowString );
    
    TGlxWindowIterator iterator = aWindow.Iterator();
    TInt index = iterator++;
    // check start index is correct
    EUNIT_ASSERT_DESC( index == info.iStartIndex, "start index is correct" );
    
    TInt size = 0;
    // check items in window are correct
    while ( KErrNotFound != index )
        {
        TChar ch = aWindowString[ index ];
        ch.LowerCase();
        EUNIT_ASSERT_DESC( ch == aWindow[ index ]->Id(), "item is correct" );
        size++;
        index = iterator++;
        }
        
    // check window length is correct
    EUNIT_ASSERT_DESC( size == info.iWindowSize, "window size is correct" );
    }
    
enum TChange 
    {
    ENone,
    EAdd,
    ERemove
    };    
       
/** Convert an index to an index before a change happened */
TInt indexBeforeChange( TInt aIndex, TChange aChange, TInt aFirstChanged,
        TInt aChangeCount )
    {
    TInt index = aIndex;
    if ( aFirstChanged <= aIndex )
        {
        if ( aChange == EAdd ) 
            {
            if ( aIndex <= aFirstChanged + aChangeCount )
                {
                index = KErrNotFound;
                }
            else 
                {
                index -= aChangeCount;
                }
            }
        else if ( aChange == ERemove ) 
            {
            index += aChangeCount;
            }
        }
    return index;
    }
    
/** Makes sure objects were not cleaned up unnecessarily */
void AssertReuse( const CTestWindow& aWindow, const char* aWindowBefore, 
        const char* aWindowAfter, TChange aChange, TInt aChangedIndex,
        TInt aChangeCount )
    {
    TGlxWindowIterator iterator = aWindow.Iterator();
    TInt index = 0;
    while ( KErrNotFound != ( index = iterator++ ) )
        {
        TInt indexBefore = indexBeforeChange( index, aChange, aChangedIndex, aChangeCount );
        if ( KErrNotFound != indexBefore )
            {
            TChar chBefore = aWindowBefore[ indexBefore ];
            chBefore.LowerCase();
            TChar chAfter = aWindowAfter[ index ];
            chAfter.LowerCase();
            if ( chBefore != '-' )
                {
                ASSERT( chBefore == chAfter ); // error is test case?
                ASSERT( aWindow[ index ]->Id() == chAfter ); // error is test code?
                EUNIT_ASSERT_DESC( 0 == aWindow[ index ]->iCleanupCount, "reused item was not cleaned up" );
                }
            }
        }
    }

/** Creates a window */
CTestWindow* CreateWindowLC( const char* aListString, TInt aFrontOffset, 
        TInt aRearOffset, const char* aWindowString )
    {
    CTestWindow* window = new ( ELeave ) CTestWindow( aListString );
    CleanupStack::PushL( window );
    window->ConstructL();
    window->SetRangeOffsetsL( aFrontOffset, aRearOffset );
    AssertWindow( *window, "" ); // kind of unnecessary...
    
    // populate initial list
    TWindowInfo info = AnalyzeWindow( aWindowString );
    /// @todo combine these 3 lines to a single call to AddObjects
    if ( info.iTotalSize > 0 )
        {
        window->AddObjects( info.iFocusIndex, info.iTotalSize, 
            0, info.iTotalSize - 1 );
        }
    
    // assert initial window
    AssertWindow( *window, aWindowString );

    return window;    
    }

/** Cleans up a window */
void Cleanup( CTestWindow& aWindow )
    {
    aWindow.Cleanup();
    // test cleanup ok
    for ( TInt i = 0; i < aWindow.iEntries.Count(); i++ )
        {
        EUNIT_ASSERT_DESC( aWindow.iEntries[ i ]->Id() == 0, "cleaned up correctly" );
        }       
    }
 
void VerifyAndCleanup( CTestWindow& aWindow, const char* aWindowBefore, 
        const char* aWindowAfter, TChange aChange = ENone, 
        TInt aChangedIndex = KErrNotFound, TInt aChangeCount = 0 )
    {
    // assert window after change
    AssertWindow( aWindow, aWindowAfter );
    // makes sure reused items have not been cleaned up during update
    AssertReuse( aWindow, aWindowBefore, aWindowAfter, aChange, aChangedIndex, aChangeCount );
    Cleanup( aWindow );
    }
    
/** 
 * Notation: * focus 
 *           - items outside of window
 *           any other char is the id of an item
 */
void TestUpdateL( const char* aListBeforeChange, const char* aWindowBefore, 
        const char* aWindowAfter, TInt aFrontOffset, TInt aRearOffset, 
        TChange aChange = ENone, TInt aChangedIndex = KErrNotFound, 
        const char* aListAfterChange = NULL ) 
    {
    // window and list strings must have the same length
    ASSERT( strlen( aListBeforeChange ) == strlen( aWindowBefore ) );
    if ( aListAfterChange ) 
        {
        ASSERT( strlen( aWindowAfter ) == strlen( aListAfterChange ) );
        }
        
    // create window object
    CTestWindow* window = CreateWindowLC( aListBeforeChange, aFrontOffset, 
        aRearOffset, aWindowBefore );
    
    // apply changes
    TInt changeCount = 0;
    if ( EAdd == aChange )
        {
        window->iList = aListAfterChange;
        changeCount = strlen( aListAfterChange ) - strlen( aListBeforeChange );
        TWindowInfo info = AnalyzeWindow( aWindowAfter );
        window->AddObjects( info.iFocusIndex, info.iTotalSize,       // focus, size
            aChangedIndex, aChangedIndex + changeCount - 1 ); // first index, last index
        window->iOldList = window->iList; // iOldList is used to check cleanup
        }
    else if ( ERemove == aChange )
        {
        window->iList = aListAfterChange;
        changeCount = strlen( aListBeforeChange ) - strlen( aListAfterChange );
        TWindowInfo info = AnalyzeWindow( aWindowAfter );
        window->RemoveObjects(  info.iFocusIndex, info.iTotalSize, // focus, size
            aChangedIndex, aChangedIndex + changeCount - 1 ); // first index, last index
        window->iOldList = window->iList; // iOldList is used to check cleanup
        }
    if ( ENone == aChange )
        {
        TWindowInfo info = AnalyzeWindow( aWindowAfter );
        window->SetFocusIndex( info.iFocusIndex, info.iTotalSize );
        }
        
    VerifyAndCleanup( *window, aWindowBefore, aWindowAfter, aChange, 
        aChangedIndex, changeCount );
    CleanupStack::PopAndDestroy( window );
    }
 
/** Test SetRangeOffsetsL */   
void TestSetRangesL( const char* aList, 
        const char* aWindowBefore, TInt aFrontOffsetBefore, TInt aRearOffsetBefore, 
        const char* aWindowAfter, TInt aFrontOffsetAfter, TInt aRearOffsetAfter )
    {
    // window and list strings must have the same length
    ASSERT( strlen( aList ) == strlen( aWindowBefore ) );
    ASSERT( strlen( aWindowAfter ) == strlen( aWindowBefore ) );
        
    // create window object
    CTestWindow* window = CreateWindowLC( aList, aFrontOffsetBefore, aRearOffsetBefore, 
        aWindowBefore );
    
    TWindowInfo info = AnalyzeWindow( aWindowAfter );
    window->SetRangeOffsetsL( info.iFocusIndex, info.iTotalSize, 
        aFrontOffsetAfter, aRearOffsetAfter );
        
    VerifyAndCleanup( *window, aWindowBefore, aWindowAfter );
    
    CleanupStack::PopAndDestroy( window );
    }
    
// -----------------------------------------------------------------------------
void t_CGlxListWindow::T_SetRangeOffsetsLL(  )
    {
    TestSetRangesL( "abcdefghijk", "---deFghi--", -2, 3, "--cdeFghij-", -3, 4 );
    TestSetRangesL( "abcdefghijk", "--cdeFghij-", -3, 4, "---deFghi--", -2, 3 );
    TestSetRangesL( "abcdefghijk", "--cdeFghij-", -3, 4, "-----F-----", 0, 0 );
    TestSetRangesL( "abcdefghijk", "--cdeFghij-", -3, 4, "abcdeFghijk", -10, 10 );
    }
    
// -----------------------------------------------------------------------------
void t_CGlxListWindow::T_SetFocusIndexL(  )
    {
    TestUpdateL( "abcd", "-B--", "-B--", 0, 0 );
    // when whole list fits to window
    // ... set focus on list with 1 item
    TestUpdateL( "a", "A", "A", -2, 3 );
    // ... set focus on list with multiple items
    TestUpdateL( "abcde", "Abcde", "abcDe", -2, 3 );
    // ... set focus when list is as long max length of window
    TestUpdateL( "abcdef", "Abcdef", "abcDef", -2, 3 );
    TestUpdateL( "abcdef", "Abcdef", "abcdeF", -2, 3 );
    TestUpdateL( "abcdef", "abcdeF", "Abcdef", -2, 3 );
    TestUpdateL( "abcdef", "abcdEf", "abCdef", -2, 3 );
    // ... set focus to same item again
    TestUpdateL( "abcdef", "abCdef", "abCdef", -2, 3 );
    TestUpdateL( "abcdef", "Abcdef", "abcdeF", -2, 3 );
    TestUpdateL( "abcdef", "Abcdef", "abcdeF", -2, 3 );
    
    // when whole list does not fit to window
    TestUpdateL( "abcdefg", "Abcd-fg", "ab-deFg", -2, 3 );
    TestUpdateL( "abcdefg", "abc-efG", "-bcDefg", -2, 3 );
    TestUpdateL( "abcdefg", "abCdef-", "Abcd-fg", -2, 3 );
    // ... set focus on list with 2 items
    TestUpdateL( "abcdefghijk", "---deFghi--", "----efGhij-", -2, 3 );
    TestUpdateL( "abcdefghijk", "---deFghi--", "abc-----ijK", -2, 3 );
    TestUpdateL( "abcdefghijk", "---deFghi--", "---deFghi--", -2, 3 );
                
    TestUpdateL( "abcdefghijk", "---deFghi--", "abc-----ijK", -2, 3 );
    TestUpdateL( "abcdefghijk", "---deFghi--", "ab-----hiJk", -2, 3 );
    TestUpdateL( "abcdefghijk", "---deFghi--", "Abcd-----jk", -2, 3 );
    TestUpdateL( "abcdefghijk", "---deFghi--", "aBcde-----k", -2, 3 );
    TestUpdateL( "abcdefghijk", "abc-----ijK", "abc-----ijK", -2, 3 );
    TestUpdateL( "abcdefghijk", "abc-----ijK", "ab-----hiJk", -2, 3 );
    TestUpdateL( "abcdefghijk", "abc-----ijK", "Abcd-----jk", -2, 3 );
    TestUpdateL( "abcdefghijk", "abc-----ijK", "aBcde-----k", -2, 3 );
    TestUpdateL( "abcdefghijk", "Abcd-----jk", "abc-----ijK", -2, 3 );
    TestUpdateL( "abcdefghijk", "Abcd-----jk", "ab-----hiJk", -2, 3 );
    TestUpdateL( "abcdefghijk", "Abcd-----jk", "Abcd-----jk", -2, 3 );
    TestUpdateL( "abcdefghijk", "Abcd-----jk", "aBcde-----k", -2, 3 );
    // non-overlapping old and new
    TestUpdateL( "abcdefghijk", "Abc------jk", "----efGhi--", -2, 2 );
    TestUpdateL( "abcdefghijk", "Abc------jk", "---deFgh---", -2, 2 );
    }
    
// -----------------------------------------------------------------------------
void t_CGlxListWindow::T_AddObjectsL(  )
    {
    TestUpdateL( "abcd", "-B--", "---B---", 0, 0, EAdd, 0, "23abcde" );
    // add to empty list
    TestUpdateL( "", "", "Abcd-----jk", -2, 3, EAdd, 0, "abcdefghijk");
    TestUpdateL( "", "", "----efGhij-", -2, 3, EAdd, 0, "abcdefghijk");
    TestUpdateL( "", "", "abc-----ijK", -2, 3, EAdd, 0, "abcdefghijk");
    // add to a list that does not fill the window
    TestUpdateL( "abc", "Abc", "Abcde", -2, 3, EAdd, 3, "abcde");
    TestUpdateL( "abc", "Abc", "Adebc", -2, 3, EAdd, 1, "adebc");
    // add before list
    TestUpdateL( "abc", "Abc", "Abc-ef", -2, 2, EAdd, 3, "abcdef");
    TestUpdateL( "abcdefghi", "---deFghi", "-----deFghi", -2, 3, EAdd, 1, "a12bcdefghi");
    TestUpdateL( "abcdefghi", "---deFghi", "-----deFghi", -2, 3, EAdd, 3, "abc12defghi");
    TestUpdateL( "abcdefghi", "abc---ghI", "abc-----ghI", -2, 3, EAdd, 1, "abcde12fghi");
    TestUpdateL( "abcdefghi", "abc---ghI", "abc-----ghI", -2, 3, EAdd, 3, "abcdef12ghi");
    TestUpdateL( "abcdefghi", "Abcd---hi", "Abcd-----hi", -2, 3, EAdd, 6, "abcdef12ghi");
    TestUpdateL( "abcdefghi", "Abcd---hi", "Abcd-----hi", -2, 3, EAdd, 7, "abcdefg12hi");
    // add before focus, on list
    TestUpdateL( "abcdefghi", "---deFghi", "-----2eFghi", -2, 3, EAdd, 4, "abcd12efghi");
    TestUpdateL( "abcdefghi", "---deFghi", "-----12Fghi", -2, 3, EAdd, 5, "abcde12fghi");
    TestUpdateL( "abcdefghi", "abc---ghI", "abc-----2hI", -2, 3, EAdd, 7, "abcdefg12hi");
    TestUpdateL( "abcdefghi", "abc---ghI", "abc-----12I", -2, 3, EAdd, 8, "abcdefgh12i");
    TestUpdateL( "abcdefghi", "Abcd---hi", "Abcd-----2i", -2, 3, EAdd, 8, "abcdefgh12i");
    TestUpdateL( "abcdefghi", "Abcd---hi", "Abcd-----12", -2, 3, EAdd, 9, "abcdefghi12");
    // add after focus, on list
    TestUpdateL( "abcdefghi", "---deFghi", "---deF12g--", -2, 3, EAdd, 6, "abcdef12ghi");
    TestUpdateL( "abcdefghi", "---deFghi", "---deFg12--", -2, 3, EAdd, 7, "abcdefg12hi");
    TestUpdateL( "abcdefghi", "---deFghi", "---deFgh1--", -2, 3, EAdd, 8, "abcdefgh12i");
    TestUpdateL( "abcdefghi", "---deFghi", "---deFghi--", -2, 3, EAdd, 9, "abcdefghi12");
    TestUpdateL( "abcdefghi", "abc---ghI", "12a-----ghI", -2, 3, EAdd, 0, "12abcdefghi");
    TestUpdateL( "abcdefghi", "abc---ghI", "a12-----ghI", -2, 3, EAdd, 1, "a12bcdefghi");
    TestUpdateL( "abcdefghi", "abc---ghI", "ab1-----ghI", -2, 3, EAdd, 2, "ab12cdefghi");
    TestUpdateL( "abcdefghi", "abc---ghI", "abc-----ghI", -2, 3, EAdd, 3, "abc12defghi");
    // add after list
    TestUpdateL( "abcdefghi", "---deFgh-", "---deFgh---", -2, 2, EAdd, 9, "abcdefghi12");
    }
    
// -----------------------------------------------------------------------------
void t_CGlxListWindow::T_RemoveObjectsL(  )
    {
    TestUpdateL( "abcd", "--C-", "C--", 0, 0, ERemove, 0, "cde" );
    // test remove before list
    TestUpdateL( "abcdefghi", "---deFgh-", "-deFgh-", -2, 2, ERemove, 0, "cdefghi");
    // test remove before focus, on list
    TestUpdateL( "abcdefghi", "---deFgh-", "-beFgh-", -2, 2, ERemove, 2, "abefghi");
    TestUpdateL( "abcdefghi", "---deFgh-", "-bcFgh-", -2, 2, ERemove, 3, "abcfghi");
    // test remove after focus, on list
    TestUpdateL( "abcdefghi", "---deFgh-", "a--deFg", -2, 2, ERemove, 7, "abcdefg");
    // test remove after list
    TestUpdateL( "abcdefghi", "-bcDef---", "-bcDef-", -2, 2, ERemove, 7, "abcdefg");
    // test remove all
    TestUpdateL( "abcdefghi", "---deFgh-", "", -2, 2, ERemove, 0, "");
    TestUpdateL( "abcdefghi", "Abc----hi", "", -2, 2, ERemove, 0, "");
    TestUpdateL( "abcdefghi", "ab----ghI", "", -2, 2, ERemove, 0, "");
    // with 0 ranges
    TestUpdateL( "abcdefghi", "--------I", "", 0, 0, ERemove, 0, "");
    TestUpdateL( "abcdefghi", "---D-----", "---D---", 0, 0, ERemove, 7, "abcdefg");
    }

// -----------------------------------------------------------------------------
// Test table
// -----------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    t_CGlxListWindow,
    "List window test suite",
    "UNIT" )    

EUNIT_TEST(
    "SetRangeOffsetsL",
    "CGlxListWindow",
    "SetRangeOffsetsL",
    "FUNCTIONALITY",
    SetupL, T_SetRangeOffsetsLL, Teardown)

EUNIT_TEST(
    "SetFocusIndex",
    "CGlxListWindow",
    "SetFocusIndex",
    "FUNCTIONALITY",
    SetupL, T_SetFocusIndexL, Teardown)
    
EUNIT_TEST(
    "AddObjects",
    "CGlxListWindow",
    "AddObjects",
    "FUNCTIONALITY",
    SetupL, T_AddObjectsL, Teardown)
    
EUNIT_TEST(
    "RemoveObjects",
    "CGlxListWindow",
    "RemoveObjects",
    "FUNCTIONALITY",
    SetupL, T_RemoveObjectsL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
