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

#include "MemSpyContainer.h"

// System includes
#include <eikrted.h>  // for example label control
#include <txtrich.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>

// User includes
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"



/*
CMemSpyContainer::CMemSpyContainer( CMemSpyEngine& aEngine, MMemSpyContainerObserver& aObserver  )
:   iEngine( aEngine ), iObserver( aObserver )
    {
    }
*/
CMemSpyContainer::CMemSpyContainer( RMemSpySession& aSession, MMemSpyContainerObserver& aObserver  )
:   iMemSpySession( aSession ), iObserver( aObserver )
    {
    }

CMemSpyContainer::~CMemSpyContainer()
    {
    delete iPreviousView;
    delete iActiveView;
    }


void CMemSpyContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    SetRect(aRect);
    PrepareTopLevelViewL();
    ActivateL();
    }


void CMemSpyContainer::NavigateToParentViewL()
    {
    CMemSpyViewBase* parentView = iActiveView->PrepareParentViewL();
    //
    if  ( parentView == NULL )
        {
        // Parent can be null, in which case, revert to top-level view
        PrepareTopLevelViewL();
        }
    else
        {
        SetNewActiveViewL( parentView );
        }
    }


void CMemSpyContainer::NavigateToChildViewL()
    {
    CMemSpyViewBase* childView = iActiveView->PrepareChildViewL();
    //
    if  ( childView != NULL )
        {
        SetNewActiveViewL( childView );
        }
    }


void CMemSpyContainer::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
	case EMemSpyCmdViewRefresh:
        OnCmdViewRefreshL();
        break;
    default:
        if  ( iActiveView )
            {
            iActiveView->HandleCommandL( aCommand );
            }
        break;
        }
    }


void CMemSpyContainer::OnCmdViewRefreshL()
    {
    if  ( iActiveView )
        {
        TRAPD(err, iActiveView->RefreshL());
        if  ( err != KErrNone )
            {
            // Error during view refreshing (perhaps the thread doesn't exist anymore).
            // Try to replace the active view with its parent...
            NavigateToParentViewL();
            }
        }
    }


void CMemSpyContainer::SizeChanged()
    {
    if  ( iActiveView )
        {
        iActiveView->SetRect( Rect() );
        }
    }


TInt CMemSpyContainer::CountComponentControls() const
    {
    return 1;
    }


CCoeControl* CMemSpyContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return iActiveView;
    }


void CMemSpyContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    //
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }


TKeyResponse CMemSpyContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;
    //
    if  ( iActiveView )
        {
        response = iActiveView->OfferKeyEventL( aKeyEvent, aType );
        }
    //
    return response;
    }


void CMemSpyContainer::HandleMemSpyViewEventL( TViewEventType aEvent, TMemSpyViewType /*aViewType*/, CMemSpyViewBase& aReportingView, TAny* /*aContext*/ )
    {
    if  ( aEvent == EEventItemActioned )
        {
        // Get the view to generate a child
        CMemSpyViewBase* child = aReportingView.PrepareChildViewL();
        if  ( child == NULL )
            {
            // View doesn't have a child - treat it as item activation and
            // report event to observer.

            }
        else
            {
            // Child view becomes the active one
            SetNewActiveViewL( child );
            }
        }
    }


CMemSpyViewBase* CMemSpyContainer::PrepareTopLevelViewL()
    {
    //CMemSpyViewMainMenu* view = new(ELeave) CMemSpyViewMainMenu( iEngine, *this );
	CMemSpyViewMainMenu* view = new(ELeave) CMemSpyViewMainMenu( iMemSpySession, *this );
    CleanupStack::PushL( view );
    view->ConstructL( Rect(), *this );
    SetNewActiveViewL( view );
    CleanupStack::Pop( view );
    return view;
    }


void CMemSpyContainer::SetNewActiveViewL( CMemSpyViewBase* aNewView )
    {
    delete iPreviousView;
    //
    iPreviousView = iActiveView;
    if  ( iPreviousView )
        {
        iPreviousView->MakeVisible( EFalse );
        iPreviousView->SetFocus( EFalse );
        }
    //
    iActiveView = aNewView;
    iActiveView->RefreshL();
    iActiveView->SetFocus( ETrue );
    //
    ReportEventL( MCoeControlObserver::EEventStateChanged );
    }








