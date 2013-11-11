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

#ifndef MEMSPYVIEWBASE_H
#define MEMSPYVIEWBASE_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>
#include <memspyui.rsg>

// User includes
#include "MemSpy.hrh"
#include "MemSpyViewType.h"
#include "MemSpyViewObserver.h"
#include "MemSpySettings.h"

// Classes referenced
class CMemSpyEngine;
class CMemSpyContainer;
class MMemSpyViewObserver;
//cigasto
class RMemSpySession;

class CMemSpyViewBase : public CCoeControl, public MCoeControlObserver, public MEikListBoxObserver
    {
public:
    //CMemSpyViewBase( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
	CMemSpyViewBase( RMemSpySession& aSession, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewBase();
    virtual void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // API
    virtual TMemSpyViewType ViewType() const = 0;
    virtual CMemSpyViewBase* PrepareParentViewL();
    virtual CMemSpyViewBase* PrepareChildViewL();
    virtual void RefreshL();
    virtual TBool HandleCommandL( TInt aCommand );

public: // Menu framework
    virtual void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
    virtual TUint MenuCascadeResourceId() const;
    virtual TInt MenuCascadeCommandId() const;

private: // Command handlers
    virtual void OnCmdViewOutputToSinkL();

protected: // Construction support
    void SetTitleL( const TDesC& aText );
    TPtrC TitleL() const;
    virtual CEikListBox* ConstructListBoxL();
    virtual void SetListBoxModelL() = 0;

protected: // Internal framework
    virtual void HandleListBoxItemActionedL( TInt aIndex );
    virtual void HandleListBoxItemSelectedL( TInt aIndex );
 
protected: // Event reporting
    void ReportEventL( MMemSpyViewObserver::TViewEventType aEvent, TAny* aContext = NULL );
    void SetListBoxCurrentItemIndexL( TInt aIndex );

protected: // Utility methods
    CMemSpyContainer& Container();
    CMemSpySettings& Settings();
    const CMemSpySettings& Settings() const;

public: // From CCoeControl
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

protected: // From CCoeControl
    void Draw( const TRect& aRect ) const;
    void SizeChanged();
    void FocusChanged( TDrawNow aDrawNow );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;

protected: // From MCoeControlObserver
    void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );

protected: // From MEikListBoxObserver
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType);

protected: // Member data
    //CMemSpyEngine& iEngine;
    RMemSpySession& iMemSpySession;
    MMemSpyViewObserver& iObserver;
    //
    CEikListBox* iListBox;

private:
    CMemSpySettings* iSettings;
    };


#endif
