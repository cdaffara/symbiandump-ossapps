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

#ifndef MEMSPYVIEWTHREADINFOITEMSERVER_H
#define MEMSPYVIEWTHREADINFOITEMSERVER_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>
#include <badesca.h>

// User includes
#include "MemSpyViewThreadInfoItemGeneric.h"

// Classes referenced
class TMemSpyDriverHandleInfoGeneric;
class RMemSpySession;

class CMemSpyViewThreadInfoItemServer : public CMemSpyViewThreadInfoItemGeneric
    {
public:
    CMemSpyViewThreadInfoItemServer( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType );

public: // From CMemSpyViewBase
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );
    void HandleListBoxItemSelectedL( TInt aCurrentIndex );

public: // From CMemSpyViewBase
    CMemSpyViewBase* PrepareChildViewL();

private: // Data members
    TMemSpyDriverHandleInfoGeneric iCurrentInfoItemDetails;
    };



class CMemSpyViewThreadInfoItemServerDetails : public CMemSpyViewThreadInfoItemGeneric
    {
public:
    CMemSpyViewThreadInfoItemServerDetails( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer, const TMemSpyDriverHandleInfoGeneric& aInfoItemDetails );

public: // From CMemSpyViewBase
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareChildViewL();
    CMemSpyViewBase* PrepareParentViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aCurrentIndex );
    void HandleListBoxItemSelectedL( TInt aCurrentIndex );

private: // Internal methods

private: // Member data
    const TMemSpyDriverHandleInfoGeneric iInfoItemDetails;
    };



class CMemSpyViewThreadInfoItemServerSessions : public CMemSpyViewThreadInfoItemGeneric
    {
public:
    CMemSpyViewThreadInfoItemServerSessions( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer, const TMemSpyDriverHandleInfoGeneric& aInfoItemDetails );

public: // From CMemSpyViewBase
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // Member data
    const TMemSpyDriverHandleInfoGeneric iInfoItemDetails;
    };



#endif
