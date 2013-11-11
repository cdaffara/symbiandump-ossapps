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
#include "perfmon_valuesview.h"
#include "perfmon_valuescontainer.h"
#include "perfmon_document.h" 
#include "perfmon_model.h"
#include <perfmon.rsg>

#include <eikenv.h>
#include <aknViewAppUi.h> 


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPerfMonValuesView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CPerfMonValuesView::ConstructL()
    {
    BaseConstructL( R_PERFMON_VIEW_VALUES );
    iModel = static_cast<CPerfMonDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();
    }

// ---------------------------------------------------------
// CPerfMonValuesView::~CPerfMonValuesView()
// ---------------------------------------------------------
//
CPerfMonValuesView::~CPerfMonValuesView()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------
// TUid CPerfMonValuesView::Id()
// ---------------------------------------------------------
//
TUid CPerfMonValuesView::Id() const
    {
    return KValuesViewUID;
    }

// ---------------------------------------------------------
// TUid CPerfMonValuesView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
// ---------------------------------------------------------
//
void CPerfMonValuesView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    AppUi()->DynInitMenuPaneL(aResourceId, aMenuPane);
    }

  
// ---------------------------------------------------------
// CPerfMonValuesView::HandleCommandL(TInt aCommand)
// ---------------------------------------------------------
//
void CPerfMonValuesView::HandleCommandL(TInt aCommand)
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
// CPerfMonValuesView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CPerfMonValuesView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CPerfMonValuesView::DoActivateL(...)
// ---------------------------------------------------------
//
void CPerfMonValuesView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CPerfMonValuesContainer;
        iModel->SetValuesContainer(iContainer);
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        } 
   }

// ---------------------------------------------------------
// CPerfMonValuesView::DoDeactivate()
// ---------------------------------------------------------
//
void CPerfMonValuesView::DoDeactivate()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// End of File
