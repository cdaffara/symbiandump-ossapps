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

#ifndef MEMSPYVIEWWINDOWGROUPS_H
#define MEMSPYVIEWWINDOWGROUPS_H

// System includes
#include <hal.h>
#include <coecntrl.h>
#include <aknlists.h>

// Engine includes
#include <memspy/engine/memspyenginehelperwindowserver.h>

// User includes
#include "MemSpyViewBase.h"

// Classes referenced


class CMemSpyViewWindowGroups : public CMemSpyViewBase
    {
public:
    CMemSpyViewWindowGroups( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewWindowGroups();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    CEikListBox* ConstructListBoxL();
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_WINDOW_GROUPS; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdWindowGroups; }

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    TBool HandleCommandL( TInt aCommand );

private: // Command handlers
    void OnCmdSwitchToL();
    void OnCmdEndL( TInt aCommand );

private: // Internal methods
    void DetailsL();
    void AppendFormatString( TPtr& aPtr, TRefByValue<const TDesC> aFmt, ... );

private: // Data members
    MMemSpyEngineWindowGroupList* iWindowGroupList;
    };


#endif
