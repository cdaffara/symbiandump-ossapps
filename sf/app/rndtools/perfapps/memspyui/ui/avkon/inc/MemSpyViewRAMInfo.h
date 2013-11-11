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

#ifndef MEMSPYVIEWRAMINFO_H
#define MEMSPYVIEWRAMINFO_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewBase.h"

// Classes referenced


class CMemSpyViewRAMInfo : public CMemSpyViewBase
    {
public:
    CMemSpyViewRAMInfo( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();
    TBool HandleCommandL( TInt aCommand );

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_RAM; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdRAM; }

private: // Command handlers
    void OnCmdSetIconCacheStatusL( TBool aEnabled );

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // Data members
    };


#endif
