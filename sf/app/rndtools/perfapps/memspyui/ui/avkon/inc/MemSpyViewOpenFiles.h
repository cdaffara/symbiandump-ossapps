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

#ifndef MEMSPYVIEWOPENFILES_H
#define MEMSPYVIEWOPENFILES_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewBase.h"

// Classes referenced


class CMemSpyViewOpenFiles : public CMemSpyViewBase
    {
public:
    CMemSpyViewOpenFiles( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewOpenFiles();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    CEikListBox* ConstructListBoxL();
    void RefreshL();
    TBool HandleCommandL( TInt aCommand );
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // Command handlers
    void OnCmdListOpenFilesL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aCurrentIndex );

private: // Internal
    static TBool CompareTEntryObjects( const TEntry& aLeft, const TEntry& aRight );

private: // Data members
    RArray<TThreadId> iThreadIds;
    const TThreadId* iActionedThreadId;
    RArray<TEntry> iFileNames;
    };


#endif
