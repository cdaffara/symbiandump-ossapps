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


// INCLUDE FILES
#include "perfmon.hrh"
#include "perfmon_graphsview.h"
#include "perfmon_graphscontainer.h"
#include "perfmon_document.h" 
#include "perfmon_model.h"
#include <perfmon.rsg>

#include <eikenv.h>
#include <aknViewAppUi.h> 


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPerfMonGraphsView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CPerfMonGraphsView::ConstructL()
    {
    BaseConstructL( R_PERFMON_VIEW_GRAPHS );
    iModel = static_cast<CPerfMonDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();
    }

// ---------------------------------------------------------
// CPerfMonGraphsView::~CPerfMonGraphsView()
// ---------------------------------------------------------
//
CPerfMonGraphsView::~CPerfMonGraphsView()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------
// TUid CPerfMonGraphsView::Id()
// ---------------------------------------------------------
//
TUid CPerfMonGraphsView::Id() const
    {
    return KGraphsViewUID;
    }

// ---------------------------------------------------------
// TUid CPerfMonGraphsView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
// ---------------------------------------------------------
//
void CPerfMonGraphsView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    AppUi()->DynInitMenuPaneL(aResourceId, aMenuPane);
    }

  
// ---------------------------------------------------------
// CPerfMonGraphsView::HandleCommandL(TInt aCommand)
// ---------------------------------------------------------
//
void CPerfMonGraphsView::HandleCommandL(TInt aCommand)
    {   
/*
    switch ( aCommand )
        {
        case EPerfMonCmdFileBack:
            {
            iModel->FileUtils()->MoveUpOneLevelL();
            break;
            }

        default:
            {
*/
            AppUi()->HandleCommandL( aCommand );
/*
            break;
            }
        }
*/
    }

// ---------------------------------------------------------
// CPerfMonGraphsView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CPerfMonGraphsView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CPerfMonGraphsView::DoActivateL(...)
// ---------------------------------------------------------
//
void CPerfMonGraphsView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CPerfMonGraphsContainer;
        iModel->SetGraphsContainer(iContainer);
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        } 
   }

// ---------------------------------------------------------
// CPerfMonGraphsView::DoDeactivate()
// ---------------------------------------------------------
//
void CPerfMonGraphsView::DoDeactivate()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// End of File
