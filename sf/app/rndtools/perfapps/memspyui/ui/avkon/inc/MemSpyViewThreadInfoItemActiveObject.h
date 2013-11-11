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

#ifndef MEMSPYVIEWTHREADINFOITEMACTIVEOBJECT_H
#define MEMSPYVIEWTHREADINFOITEMACTIVEOBJECT_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewThreadInfoItemGeneric.h"

// Classes referenced
class CMemSpyEngineActiveObjectArray;
class RMemSpySession;


class CMemSpyViewThreadInfoItemActiveObjectBase : public CMemSpyViewThreadInfoItemGeneric
    {
public:
    CMemSpyViewThreadInfoItemActiveObjectBase( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType );
    
public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_ACTIVE_OBJECTS; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdActiveObject; }
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

protected:
    CMemSpyEngineActiveObjectArray& ActiveObjectArray() const;

private: // Command handlers
    void OnCmdWriteAOListingL();
    };



class CMemSpyViewThreadInfoItemActiveObject : public CMemSpyViewThreadInfoItemActiveObjectBase
    {
public:
    CMemSpyViewThreadInfoItemActiveObject( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareChildViewL();
    };




class CMemSpyViewThreadInfoItemActiveObjectDetails : public CMemSpyViewThreadInfoItemActiveObjectBase
    {
public:
    CMemSpyViewThreadInfoItemActiveObjectDetails( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer, TAny* aObjectAddress );

public: // From CMemSpyViewBase
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    
private: // Data members
    TAny* iObjectAddress;
    };


#endif
