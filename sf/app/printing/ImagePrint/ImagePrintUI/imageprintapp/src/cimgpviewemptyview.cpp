/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <aknViewAppUi.h>
#include <eikenv.h>
#include <eikspane.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <avkon.hrh>
#include <imageprintapp.rsg>

#include "cimgpviewemptyview.h"
#include "cimageprintappui.h"
#include "cimageprintblankcontainer.h"

// CONSTRUCTION
CIMGPViewEmptyView* CIMGPViewEmptyView::NewL()
    {
    CIMGPViewEmptyView* self = new (ELeave) CIMGPViewEmptyView;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Default constructor
CIMGPViewEmptyView::CIMGPViewEmptyView()
    {
    }

// 2nd phase constructor
void CIMGPViewEmptyView::ConstructL()
    {
    BaseConstructL( R_EMPTY_VIEW );
    }

// Destructor
CIMGPViewEmptyView::~CIMGPViewEmptyView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    }

// Returns view's ID
TUid CIMGPViewEmptyView::Id() const
    {
    return KIMGPViewEmptyViewId;
    }

// Handles view commands
void CIMGPViewEmptyView::HandleCommandL(
    TInt /*aCommand*/ )
    {
    AppUi()->HandleCommandL( EAknCmdExit );
    }

// Activates the view, creates the container and other UI controls
void CIMGPViewEmptyView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,
   TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    TRect rect( AppUi()->ClientRect() );
    iContainer = CImagePrintBlankContainer::NewL(rect);
    iContainer->SetMopParent( this );
    AppUi()->AddToStackL( *this, iContainer );
    }

// Deactivates the view, deletes print container
void CIMGPViewEmptyView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = 0;
        }
    }


void CIMGPViewEmptyView::SizeChanged()
    {
    //No Implementetion needed
    }   


void CIMGPViewEmptyView::HandleForegroundEventL( TBool aForeground )
    {
    if ( aForeground )
        {
        ViewScreenDeviceChangedL();
        }
    }

//  End of File
