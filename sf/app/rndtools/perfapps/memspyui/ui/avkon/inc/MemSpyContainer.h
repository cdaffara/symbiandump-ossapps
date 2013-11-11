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

#ifndef MEMSPYCONTAINER_H
#define MEMSPYCONTAINER_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewBase.h"
#include "MemSpyViewObserver.h"

// Classes referenced
class CMemSpyEngine;
class CMemSpyProcess;
class CMemSpyThread;
class CMemSpyThreadInfoItemBase;
class CMemSpyEngineObjectContainer;
class MMemSpyContainerObserver;
//cigasto
class RMemSpySession;

class CMemSpyContainer : public CCoeControl, public MMemSpyViewObserver
    {
public:
    //CMemSpyContainer( CMemSpyEngine& aEngine, MMemSpyContainerObserver& aObserver );
	CMemSpyContainer( RMemSpySession& aSession, MMemSpyContainerObserver& aObserver );
    ~CMemSpyContainer();
    void ConstructL( const TRect& aRect );

public: // API
    inline TMemSpyViewType ActiveViewType() const { return iActiveView->ViewType(); }
    inline CMemSpyViewBase& ActiveView() const { return *iActiveView; }

public: // API
    void NavigateToParentViewL();
    void NavigateToChildViewL();
    void HandleCommandL( TInt aCommand );
    void SetNewActiveViewL( CMemSpyViewBase* aNewView );

public: // Command handlers
    void OnCmdViewRefreshL();

private: // From CCoeControl
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

private: // From MMemSpyViewObserver
    void HandleMemSpyViewEventL( TViewEventType aEvent, TMemSpyViewType aViewType, CMemSpyViewBase& aReportingView, TAny* aContext );

private: // Internal methods
    CMemSpyViewBase* PrepareTopLevelViewL();

private: // Member data
    //CMemSpyEngine& iEngine;
    RMemSpySession& iMemSpySession;
    MMemSpyContainerObserver& iObserver;
    CMemSpyViewBase* iActiveView;
    CMemSpyViewBase* iPreviousView;
    };


#endif
