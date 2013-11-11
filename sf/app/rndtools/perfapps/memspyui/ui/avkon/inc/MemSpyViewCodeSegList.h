/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MEMSPYVIEWCODESEGLIST_H
#define MEMSPYVIEWCODESEGLIST_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewBase.h"

// Classes referenced
class CMemSpyEngineCodeSegList;
class CMemSpyEngineCodeSegEntry;


class CMemSpyViewCodeSegBase : public CMemSpyViewBase
    {
public:
    CMemSpyViewCodeSegBase( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineCodeSegList* aList );
    ~CMemSpyViewCodeSegBase();

public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );
    void RefreshL();

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_CODESEG_LIST; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdCodeSeg; }

protected: // Command handlers
    void OnCmdCodeSegmentListingL();
    void OnCmdShowItemsAllL();
    void OnCmdShowItemsGlobalDataL();
    void OnCmdFilterByCapabilityL( TCapability aCapability, TBool aAllBinaries );

protected: // Data members
    CMemSpyEngineCodeSegList* iList;
    };




class CMemSpyViewCodeSegList : public CMemSpyViewCodeSegBase
    {
public:
    CMemSpyViewCodeSegList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    CMemSpyViewCodeSegList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineCodeSegList& aList );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aIndex );
    void HandleListBoxItemSelectedL( TInt aIndex );

private: // Data members
    CMemSpyEngineCodeSegEntry* iCurrentCodeSegment;
    };



class CMemSpyViewCodeSegDetails : public CMemSpyViewCodeSegBase
    {
public:
    CMemSpyViewCodeSegDetails( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineCodeSegList& aList, CMemSpyEngineCodeSegEntry& aSelectedCodeSegment );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // Data members
    CMemSpyEngineCodeSegEntry& iCodeSegment;
    };






class TMemSpyViewCodeSegFilter
    {
public:
    inline TMemSpyViewCodeSegFilter( TCapability aCapability, TBool aAllBinaries = ETrue )
    :   iCapability( aCapability ), iAllBinaries( aAllBinaries )
        {
        }

public:
    static TBool FilterItem( const CMemSpyEngineCodeSegEntry*& aItem, TAny* aRune );

private:
    TCapability iCapability;
    TBool iAllBinaries;
    };




#endif
