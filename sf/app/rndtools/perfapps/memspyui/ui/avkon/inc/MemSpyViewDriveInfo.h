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

#ifndef MEMSPYVIEWDRIVEINFO_H
#define MEMSPYVIEWDRIVEINFO_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// Engine includes
#include <memspy/engine/memspyenginehelperfilesystem.h>

// User includes
#include "MemSpyViewBase.h"

// Classes referenced




class CMemSpyViewDriveInfoBase : public CMemSpyViewBase
    {
public:
    CMemSpyViewDriveInfoBase( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewDriveInfoBase();
    void BaseConstructL();

public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );

protected: // Data members
    CMemSpyEngineDriveList* iList;
    };




class CMemSpyViewDriveList : public CMemSpyViewDriveInfoBase, public MMemSpyEngineDriveListObserver
    {
public:
    CMemSpyViewDriveList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    CMemSpyViewDriveList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, TDriveNumber aDriveNumber );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // From CMemSpyViewBase
    void RefreshL();
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aIndex );
    void HandleListBoxItemSelectedL( TInt aIndex );

private: // From MMemSpyEngineDriveListObserver
    void HandleDriveListChangedL( const CMemSpyEngineDriveList& aList );

private: // Data members
    TBool iUseDriveNumber;
    TDriveNumber iDriveNumber;
    CMemSpyEngineDriveEntry* iCurrentDrive;
    };



class CMemSpyViewDriveInfo : public CMemSpyViewDriveInfoBase, public MMemSpyEngineDriveEntryObserver
    {
public:
    CMemSpyViewDriveInfo( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, TDriveNumber aDriveNumber );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // From MMemSpyEngineDriveEntryObserver
    void HandleDriveEntryChangedL( const CMemSpyEngineDriveEntry& aEntry );

private: // Data members
    const TDriveNumber iDriveNumber;
    CMemSpyEngineDriveEntry* iDriveInfo;
    };



#endif
