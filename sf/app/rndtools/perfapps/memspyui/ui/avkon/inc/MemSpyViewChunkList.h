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

#ifndef MEMSPYVIEWCHUNKLIST_H
#define MEMSPYVIEWCHUNKLIST_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewBase.h"

// Classes referenced
class CMemSpyEngineChunkList;
class CMemSpyEngineChunkEntry;




class CMemSpyViewChunkBase : public CMemSpyViewBase
    {
public:
    CMemSpyViewChunkBase( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineChunkList* aList );
    ~CMemSpyViewChunkBase();

public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_CHUNK_LIST; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdChunk; }

protected: // Command handlers
    void OnCmdListingL();

protected: // Data members
    CMemSpyEngineChunkList* iList;
    };




class CMemSpyViewChunkList : public CMemSpyViewChunkBase
    {
public:
    CMemSpyViewChunkList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    CMemSpyViewChunkList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineChunkList& aList );
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
    CMemSpyEngineChunkEntry* iCurrentChunk;
    };



class CMemSpyViewChunkDetails : public CMemSpyViewChunkBase
    {
public:
    CMemSpyViewChunkDetails( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineChunkList& aList, CMemSpyEngineChunkEntry& aSelectedChunk );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // Data members
    CMemSpyEngineChunkEntry& iChunk;
    };



#endif
