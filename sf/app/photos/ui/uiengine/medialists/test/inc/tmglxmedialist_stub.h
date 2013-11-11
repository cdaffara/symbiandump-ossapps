/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Medialist stub
 *
*/



#ifndef __TMGLXMEDIALIST_STUB_H__
#define __TMGLXMEDIALIST_STUB_H__

/**
 * @internal reviewed 08/02/2008 by Rhodri Byles
 */

//  EXTERNAL INCLUDES
#include <eunitmacros.h>

#include <mglxmedialist.h>
#include <mglxmedialistobserver.h>

//  INTERNAL INCLUDES
namespace TMGlxMediaList_Stub_Config
    {
    const TBool KDefaultIsPopulated = ETrue;
    const TInt KDefaultSize = 3;
    const TInt KDefaultFocus = 1;
    }

/**
 * Observer interface for medialist stub method calls
 * The test class is informed about any calls to the 
 * corresponding adapter using this interface. 
 */
NONSHARABLE_CLASS( MGlxMediaList_Stub_Observer )
    {
    public:     // enums

        enum TMGlxMediaListMethodId
            {
            E_NotCalled,
            E_Close,
            E_TGlxMediaListId_Id_,
            E_TInt_Count_,
            E_TInt_FocusIndex_,
            E_void_SetFocusL_TInt_,
            E_const_TGlxMedia_r_Item_TInt_,
            E_TInt_Index_const_TGlxMediaId_r_,
            E_void_AddMediaListObserverL_MGlxMediaListObserver_p_,
            E_void_RemoveMediaListObserver_MGlxMediaListObserver_p_,
            E_void_AddContextL_const_MGlxFetchContext_p_TInt_,
            E_void_RemoveContext_const_MGlxFetchContext_p_,
            E_MMPXCollection_r_Collection_,
            E_CMPXCollectionPath_p_PathLC_,
            E_TBool_IsSelected_TInt_,
            E_void_SetSelectedL_TInt_TBool_,
            E_TInt_SelectionCount_,
            E_TInt_SelectedItemIndex_TInt_,
            E_void_CommandL_CMPXCommand_r_,
            E_void_CancelCommand_,
            E_void_SetFilterL_CMPXFilter_p_,
            E_CMPXFilter_p_Filter_,
            E_TGlxIdSpaceId_IdSpaceId_TInt_,
            E_TBool_IsPopulated
            };

    protected:     // Destructor

        /**
         * Destructor
         */
        virtual ~MGlxMediaList_Stub_Observer() {};

    public:     // Abstract methods

        /**
         * This method gets called when the media list stub is called
         * @param aMethodId the method id that was called
         */
        virtual void MGlxMediaList_MethodCalled(TMGlxMediaListMethodId aMethodId) = 0;

    };

//  CLASS DEFINITION
/**
 * Stub implementation of gallery medialist
 */
NONSHARABLE_CLASS( TMGlxMediaList_Stub ) : public MGlxMediaList
    {
    public:     // Constructors and destructors

        /**
         * Constructor
         * @param aObserver, the observer for the medialist events
         */
        inline TMGlxMediaList_Stub( MGlxMediaList_Stub_Observer* aObserver );

        /**
         * Destructor
         */
        inline ~TMGlxMediaList_Stub();

    public:

        /**
         * This method can be used by test code to call 
         * HandleAttributesAvailableL of the 
         * observer of this media list.
         * @param aIndex the index for which the attributes are available
         */
        inline void NotifyAttributesAvailableL( TInt aIndex );

        /**
         * This method can be used by test code to call 
         * HandleError of the observer of this media list.
         * @param aError the error code
         */
        inline void NotifyError( TInt aError );

        /**
         * This method can be used by test code to call 
         * NotifyItemRemoved of the observer of this media list.
         * @param aStartIndex the first item index
         * @param aEndIndex the last item index
         */
        inline void NotifyItemRemoved( TInt aStartIndex, TInt aEndIndex );
        
        /**
         * This method can be used by test code to call 
         * HandlePopulatedL of the observer of this media list.
         */
        inline void NotifyHandlePopulatedL();
        
        /**
         * This method deletes the filter currently in use
         */
         inline void RemoveFilter();

    public: // from MGlxMediaList

        inline void Close();
        inline TGlxMediaListId Id() const;
        inline TInt Count( NGlxListDefs::TCountType aType ) const;
        inline TInt FocusIndex() const;
        inline void SetFocusL(NGlxListDefs::TFocusSetType aType, TInt aValue);
        inline const TGlxMedia& Item(TInt aIndex) const;
        inline TInt Index(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aId) const;
        inline void AddMediaListObserverL(MGlxMediaListObserver* aObserver);
        inline void RemoveMediaListObserver(MGlxMediaListObserver* aObserver);
        inline void AddContextL(const MGlxFetchContext* aContext, TInt aPriority);
        inline void RemoveContext(const MGlxFetchContext* aContext);
        inline MMPXCollection& Collection() const;
        inline CMPXCollectionPath* PathLC(NGlxListDefs::TPathType aType) const;
        inline TBool IsSelected(TInt aIndex) const;
        inline void SetSelectedL(TInt aIndex, TBool aSelected);
        inline TInt SelectionCount() const;
        inline TInt SelectedItemIndex(TInt aSelectionIndex) const;
        inline void CommandL(CMPXCommand& aCommand);
        inline void CancelCommand();
        inline void SetFilterL(CMPXFilter* aFilter);
        inline CMPXFilter* Filter() const;
        inline TGlxIdSpaceId IdSpaceId(TInt aIndex) const;
        inline TBool IsPopulated() const;
        
        ///@todo implement these methods
        void AddStaticItemL( CGlxMedia* /*aStaticItem*/,
            NGlxListDefs::TInsertionPosition /*aTargetPosition*/ ) {}
        void RemoveStaticItem(const TGlxMediaId& /*aItemId*/) {}
        void SetStaticItemsEnabled( TBool aEnabled ) { iStaticItemsEnabled = aEnabled; }
        TBool IsStaticItemsEnabled() const { return iStaticItemsEnabled; }
        
    public: // Data, public so that the test can specify the values it needs to be returned

        /// Array that contains the attributes
        RArray<TMPXAttribute> iAttributesAvailable;
        /// The selection
        RArray<TInt> iSelection;
        /// media properties object
        CGlxMedia* iGlxMediaProperties;
        /// media
        TGlxMedia iGlxMedia;
        /// Whether the list is populated or not
        TBool iIsPopulated;
        /// The item count of the list
        TInt iCount;
        /// The focus index
        TInt iFocus;
        /// Collection path
        CMPXCollectionPath* iMPXCollectionPath;
        // Filter in use
        CMPXFilter* iMPXFilter;

    private: // Data, needed for functionality

        RArray<MGlxMediaListObserver*> iObservers;
        MGlxMediaList_Stub_Observer* iMGlxMediaList_Observer;
        TBool iStaticItemsEnabled;
    };

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
TMGlxMediaList_Stub::TMGlxMediaList_Stub(
    MGlxMediaList_Stub_Observer* aObserver )
    : iGlxMediaProperties( NULL ),
    iGlxMedia( TGlxMediaId( 0 ), iGlxMediaProperties ),
    iIsPopulated( TMGlxMediaList_Stub_Config::KDefaultIsPopulated ),
    iCount( TMGlxMediaList_Stub_Config::KDefaultSize ),
    iFocus( TMGlxMediaList_Stub_Config::KDefaultFocus ),
    iMPXCollectionPath( NULL ),
    iMGlxMediaList_Observer( aObserver ),
    iMPXFilter(0)
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
TMGlxMediaList_Stub::~TMGlxMediaList_Stub()
    {
    iSelection.Close();
    iObservers.Close();
    delete iMPXFilter;
    }

// -----------------------------------------------------------------------------
// NotifyAttributesAvailableL
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::NotifyAttributesAvailableL( TInt aIndex )
    {
    EUNIT_PRINT( 
        _L( "TMGlxMediaList_Stub::Notifying attributes available %d" ),
            aIndex );

    // make the call to all registered observers
    for( TInt i=0; i<iObservers.Count(); i++ )
        {
        iObservers[ i ]->HandleAttributesAvailableL( 
            aIndex, iAttributesAvailable, this );
        }
    }

// -----------------------------------------------------------------------------
// NotifyError
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::NotifyError( TInt aError )
    {
    EUNIT_PRINT( _L("TMGlxMediaList_Stub::Notifying error %d"), aError );

    // make the call to all registered observers
    for( TInt i=0; i<iObservers.Count(); i++ )
        {
        iObservers[ i ]->HandleError( aError );
        }
    }

// -----------------------------------------------------------------------------
// NotifyItemRemoved
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::NotifyItemRemoved( 
    TInt aStartIndex, TInt aEndIndex )
    {
    EUNIT_PRINT( 
        _L("TMGlxMediaList_Stub::Notifying items removed %d->%d"), 
            aStartIndex, aEndIndex );

    // make the call to all registered observers
    for( TInt i=0; i<iObservers.Count(); i++ )
        {
        iObservers[ i ]->HandleItemRemovedL( 
            aStartIndex, aEndIndex, this );
        }
    }

// -----------------------------------------------------------------------------
// NotifyHandlePopulatedL
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::NotifyHandlePopulatedL()
    {
    EUNIT_PRINT( 
        _L("TMGlxMediaList_Stub::Notifying handle populated") );

    // make the call to all registered observers
    for( TInt i=0; i<iObservers.Count(); i++ )
        {
        iObservers[ i ]->HandlePopulatedL( this );
        }
    }

// -----------------------------------------------------------------------------
// Close
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::Close()
    {
    // inform the test case 
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled( 
            MGlxMediaList_Stub_Observer::E_Close );
        }
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// Id
// -----------------------------------------------------------------------------
TGlxMediaListId TMGlxMediaList_Stub::Id() const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_TGlxMediaListId_Id_ );
        }
    // just return this pointer as id
    return TGlxMediaListId((unsigned int)(void*)this);	
    }

// -----------------------------------------------------------------------------
// Count
// -----------------------------------------------------------------------------
TInt TMGlxMediaList_Stub::Count( NGlxListDefs::TCountType /*aType*/ ) const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_TInt_Count_ );
        }
    // return the specified count
    return iCount;
    }

// -----------------------------------------------------------------------------
// FocusIndex
// -----------------------------------------------------------------------------
TInt TMGlxMediaList_Stub::FocusIndex() const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled( 
            MGlxMediaList_Stub_Observer::E_TInt_FocusIndex_ );
        }
    // return given focus
    return iFocus;
    }

// -----------------------------------------------------------------------------
// SetFocusL
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::SetFocusL( 
    NGlxListDefs::TFocusSetType /*aType*/, TInt /*aValue*/ )
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_void_SetFocusL_TInt_ );
        }
    // make an alloc to cause a leave in memory tests
    TInt* memAlloc = new (ELeave) TInt;
    delete memAlloc;
    }

// -----------------------------------------------------------------------------
// Item
// -----------------------------------------------------------------------------
const TGlxMedia& TMGlxMediaList_Stub::Item( TInt /*aIndex*/ ) const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_const_TGlxMedia_r_Item_TInt_ );
        }
    // return the glxmedia object
    return iGlxMedia;
    }

// -----------------------------------------------------------------------------
// Index
// -----------------------------------------------------------------------------
TInt TMGlxMediaList_Stub::Index(
    const TGlxIdSpaceId& /*aIdSpaceId*/, const TGlxMediaId& /*aId*/ ) const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled( 
            MGlxMediaList_Stub_Observer::E_TInt_Index_const_TGlxMediaId_r_ );
        }
    // Default return value generated by EUnit Wizard
    return 0;
    }
    
// -----------------------------------------------------------------------------
// AddMediaListObserverL
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::AddMediaListObserverL(
    MGlxMediaListObserver* aObserver )
    {
    EUNIT_PRINT( _L("TMGlxMediaList_Stub::AddMediaListObserverL" ) );
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_void_AddMediaListObserverL_MGlxMediaListObserver_p_ );
        }
    // add the observer to array
    iObservers.AppendL( aObserver );

    // make an alloc to cause a leave in memory tests
    TInt* memAlloc = new (ELeave) TInt;
    delete memAlloc;
    }
    
// -----------------------------------------------------------------------------
// RemoveMediaListObserver
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::RemoveMediaListObserver(
    MGlxMediaListObserver* aObserver )
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled( 
            MGlxMediaList_Stub_Observer::E_void_RemoveMediaListObserver_MGlxMediaListObserver_p_ );
        }

    // find the observer
    TInt index = iObservers.Find( aObserver );
    // was it found
    if( index > KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// AddContextL
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::AddContextL(
    const MGlxFetchContext* /*aContext*/, TInt /*aPriority */)
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled( 
            MGlxMediaList_Stub_Observer::E_void_AddContextL_const_MGlxFetchContext_p_TInt_ );
        }

    // make an alloc to cause a leave in memory tests
    TInt* memAlloc = new (ELeave) TInt;
    delete memAlloc;
    }
    
// -----------------------------------------------------------------------------
// RemoveContext
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::RemoveContext(
    const MGlxFetchContext* /*aContext */)
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled( 
            MGlxMediaList_Stub_Observer::E_void_RemoveContext_const_MGlxFetchContext_p_ );
        }
    }

// -----------------------------------------------------------------------------
// Collection
// -----------------------------------------------------------------------------
MMPXCollection& TMGlxMediaList_Stub::Collection() const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled( 
            MGlxMediaList_Stub_Observer::E_MMPXCollection_r_Collection_ );
        }

    // this method is not called in current tests,
    // this is just to make the code compile
    MMPXCollection* empty = NULL;
    return *empty;
    }

// -----------------------------------------------------------------------------
// PathLC
// -----------------------------------------------------------------------------
CMPXCollectionPath* TMGlxMediaList_Stub::PathLC(NGlxListDefs::TPathType /*aType*/) const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled( 
            MGlxMediaList_Stub_Observer::E_CMPXCollectionPath_p_PathLC_ );
        }
        
    // make an alloc to cause a leave in memory tests
    TInt* memAlloc = new (ELeave) TInt;
    delete memAlloc;
    // put the path to cleanupstack
    CleanupStack::PushL( iMPXCollectionPath );
    // so just make the code compile
    return iMPXCollectionPath;
    }

// -----------------------------------------------------------------------------
// IsSelected
// -----------------------------------------------------------------------------
TBool TMGlxMediaList_Stub::IsSelected( TInt /*aIndex */) const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_TBool_IsSelected_TInt_ );
        }
        
    // Default return value generated by EUnit Wizard
    return EFalse;
    }

// -----------------------------------------------------------------------------
// SetSelectedL
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::SetSelectedL( 
    TInt /*aIndex*/, TBool /*aSelected */)
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_void_SetSelectedL_TInt_TBool_ );
        }

    // make an alloc to cause a leave in memory tests
    TInt* memAlloc = new (ELeave) TInt;
    delete memAlloc;
    }

// -----------------------------------------------------------------------------
// SelectionCount
// -----------------------------------------------------------------------------
TInt TMGlxMediaList_Stub::SelectionCount() const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_TInt_SelectionCount_ );
        }
        
    return 0;
    }

// -----------------------------------------------------------------------------
// SelectedItemIndex
// -----------------------------------------------------------------------------
TInt TMGlxMediaList_Stub::SelectedItemIndex(TInt /*aSelectionIndex*/) const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_TInt_SelectedItemIndex_TInt_ );
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CommandL
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::CommandL( CMPXCommand& /*aCommand */)
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_void_CommandL_CMPXCommand_r_ );
        }

    // make an alloc to cause a leave in memory tests
    TInt* memAlloc = new (ELeave) TInt;
    delete memAlloc;
    }

// -----------------------------------------------------------------------------
// CancelCommand
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::CancelCommand()
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_void_CancelCommand_ );
        }
    }

// -----------------------------------------------------------------------------
// SetFilterL
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::SetFilterL(CMPXFilter* aFilter)
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_void_SetFilterL_CMPXFilter_p_ );
        }

    if (iMPXFilter)
        {
        delete iMPXFilter;
        iMPXFilter = NULL;
        }
    iMPXFilter = CMPXFilter::NewL(*aFilter);

    // make an alloc to cause a leave in memory tests
    TInt* memAlloc = new (ELeave) TInt;
    delete memAlloc;
    }

// -----------------------------------------------------------------------------
// Filter
// -----------------------------------------------------------------------------
CMPXFilter* TMGlxMediaList_Stub::Filter() const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_CMPXFilter_p_Filter_ );
        }
    // return the filter (set by the test case)
    return iMPXFilter;
    }

// -----------------------------------------------------------------------------
// RemoveFilter
// -----------------------------------------------------------------------------
void TMGlxMediaList_Stub::RemoveFilter()
    {
    delete iMPXFilter;
    iMPXFilter = NULL;
    }
    
// -----------------------------------------------------------------------------
// IdSpaceId
// -----------------------------------------------------------------------------
TGlxIdSpaceId TMGlxMediaList_Stub::IdSpaceId(TInt /*aIndex*/) const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_TGlxIdSpaceId_IdSpaceId_TInt_ );
        }
    return KGlxIdNone;
    }

// -----------------------------------------------------------------------------
// IsPopulated
// -----------------------------------------------------------------------------
TBool TMGlxMediaList_Stub::IsPopulated() const
    {
    // inform the test case
    if( iMGlxMediaList_Observer )
        {
        iMGlxMediaList_Observer->MGlxMediaList_MethodCalled(
            MGlxMediaList_Stub_Observer::E_TBool_IsPopulated );
        }
    // return the specified value
    return iIsPopulated;
    }

#endif      //  __TMGLXMEDIALIST_STUB_H__

// End of file
