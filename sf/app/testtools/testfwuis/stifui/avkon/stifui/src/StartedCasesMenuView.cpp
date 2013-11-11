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
* Description: This file contains CStartedCasesMenuView class
* definition.
*
*/

// INCLUDE FILES
#include  <aknViewAppUi.h>
#include  <barsread.h>	//TResourceReader
#include  <Stifui.rsg>
#include  "StartedCasesMenuView.h"
#include  "StartedCasesMenuContainer.h"
#include  "Stifui.hrh" 
#include  "AppUIAppUi.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CStartedCasesMenuView::ConstructL
// 
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL( R_APPUI_STARTEDCASESMENUVIEW );
    }

// ----------------------------------------------------------------------------
// CStartedCasesMenuView::~CStartedCasesMenuView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CStartedCasesMenuView::~CStartedCasesMenuView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    }

// ----------------------------------------------------------------------------
// CStartedCasesMenuView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CStartedCasesMenuView::Id() const
    {
    return TUid::Uid(EStartedCasesMenuViewId); //KTestCaseMenuViewId;
    }

// ----------------------------------------------------------------------------
// CStartedCasesMenuView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            AppUi()->HandleCommandL(EAppUIGoBack);
            break;
            }
		case EAknCmdOpen:
			{
			TInt a = iContainer->GetActiveLine();
			iContainer->HandleSelectedListItemL( a );
			}
			break;              
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CStartedCasesMenuView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ----------------------------------------------------------------------------
// CStartedCasesMenuView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CStartedCasesMenuContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        } 
        
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_STARTEDCASES);
	iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	np->PushL(*iNaviDecorator);
   }

// ----------------------------------------------------------------------------
// CStartedCasesMenuView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuView::DoDeactivate()
    {
    if ( iContainer )
        {
        iContainer->SaveActiveLine();
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    }

// End of File
