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
* Description: This file contains CMainMenuView class definition.
*
*/

// INCLUDE FILES
#include  <aknViewAppUi.h>
#include  <barsread.h>	//TResourceReader
#include  <Stifui.rsg>
#include  "MainMenuView.h"
#include  "MainMenuContainer.h" 
#include  "Stifui.hrh"
#include  "AppUIAppUi.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMainMenuView::ConstructL
// 
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
void CMainMenuView::ConstructL()
    {
    BaseConstructL( R_APPUI_MAINMENUVIEW );
    }

// ---------------------------------------------------------
// CMainMenuView::~CMainMenuView
// 
// Destructor
// ---------------------------------------------------------
//
CMainMenuView::~CMainMenuView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    }

// ---------------------------------------------------------
// TUid CMainMenuView::Id
// 
// Returns view´s id.
// ---------------------------------------------------------
//
TUid CMainMenuView::Id() const
    {
    return TUid::Uid(EMainMenuViewId); //KMainMenuViewId;
    }

// ---------------------------------------------------------
// CMainMenuView::HandleCommandL
// 
// Handles a command.
// ---------------------------------------------------------
//
void CMainMenuView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            AppUi()->HandleCommandL(EEikCmdExit);
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

// ---------------------------------------------------------
// CMainMenuView::HandleClientRectChange
//
// Handles client rect changes.
// ---------------------------------------------------------
//
void CMainMenuView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CMainMenuView::DoActivateL
//
// Initializes view when activated.
// ---------------------------------------------------------
//
void CMainMenuView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CMainMenuContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        } 

    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
		
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_MAIN);
	iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	
	np->PushL(*iNaviDecorator);
   }

// ---------------------------------------------------------
// CAppUIView::DoDeactivate
// 
// Deactivates view.
// ---------------------------------------------------------
//
void CMainMenuView::DoDeactivate()
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
