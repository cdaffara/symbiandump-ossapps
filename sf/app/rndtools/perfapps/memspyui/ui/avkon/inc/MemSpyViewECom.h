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

#ifndef MEMSPYVIEWECOM_H
#define MEMSPYVIEWECOM_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>
#include <eikimage.h>
#include <AknDialog.h>

// User includes
#include "MemSpyViewBase.h"

// Engine includes
#include <memspy/engine/memspyenginehelperecom.h>


class CMemSpyViewECom : public CMemSpyViewBase
    {
public:
    CMemSpyViewECom( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aIndex );
    void HandleListBoxItemSelectedL( TInt aIndex );

private: // Data members
    CMemSpyEngineEComCategory* iCurrent;
    };



class CMemSpyViewEComCategory : public CMemSpyViewBase
    {
public:
    CMemSpyViewEComCategory( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineEComCategory& aCategory );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aIndex );
    void HandleListBoxItemSelectedL( TInt aIndex );

private: // Data members
    CMemSpyEngineEComCategory& iCategory;
    CMemSpyEngineEComInterface* iCurrent;
    };




class CMemSpyViewEComInterface : public CMemSpyViewBase
    {
public:
    CMemSpyViewEComInterface( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineEComInterface& aInterface );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aIndex );
    void HandleListBoxItemSelectedL( TInt aIndex );

private: // Data members
    CMemSpyEngineEComInterface& iInterface;
    CMemSpyEngineEComImplementation* iCurrent;
    };






class CMemSpyViewEComImplementation : public CMemSpyViewBase
    {
public:
    CMemSpyViewEComImplementation( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineEComImplementation& aImplementation );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // Data members
    CMemSpyEngineEComImplementation& iImplementation;
    };





#endif
