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

#ifndef MEMSPYVIEWKERNEL_H
#define MEMSPYVIEWKERNEL_H

// System includes
#include <hal.h>
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewBase.h"

// Classes referenced
class RMemSpySession;

class CMemSpyViewKernel : public CMemSpyViewBase
    {
public:
    CMemSpyViewKernel( RMemSpySession& aSession, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewKernel();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // Internal methods
    static TInt IndexByViewType( TMemSpyViewType aType );

private: // Data members
    };


#endif
