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
* Description: This file contains CView class definition.
*
*/

// INCLUDE FILES
#include  <aknViewAppUi.h>
#include  <Stifui.rsg>
#include  "AppUIAppUi.h"
#include  "View.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CView::ConstructL
// 
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CView::ConstructL()
    {
    iUIStore = ( (CAppUIAppUi*)AppUi() )->UIStoreHandler();
    }

// ----------------------------------------------------------------------------
// CView::~CView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CView::~CView()
    {
    }

// ----------------------------------------------------------------------------
// CView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CView::Id() const
    {
    return TUid::Uid(0);
    }

// ----------------------------------------------------------------------------
// CView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CView::HandleCommandL(TInt /*aCommand*/)
    {   
    }

// ----------------------------------------------------------------------------
// CView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CView::HandleClientRectChange()
    {
    }

// ----------------------------------------------------------------------------
// CView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/ )
	{
	}

// ----------------------------------------------------------------------------
// CView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CView::DoDeactivate()
    {
    }

// End of File
