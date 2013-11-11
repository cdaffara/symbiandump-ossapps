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
* Description: This file contains CStartCasesView class definition.
*
*/

// INCLUDE FILES
#include  <eikmenub.h>
#include  <aknViewAppUi.h>
#include  <aknlistquerydialog.h> 
#include  <barsread.h>	//TResourceReader
#include  <Stifui.rsg>
#include  "CreatedTestSetMenuView.h"
#include  "CreatedTestSetMenuContainer.h"
#include  "AppUIAppUi.h"
#include  "Stifui.hrh" 
#include "TestSetMenuView.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CStartCasesView::ConstructL
// 
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CCreatedTestSetMenuView::ConstructL()
    {
    CView::ConstructL();  
    BaseConstructL(R_APPUI_LOADTESTSETVIEW);
    iCurrentTestCase = 0;

    }

// ----------------------------------------------------------------------------
// CreatedTestSetMenuView::~CStartCasesView
// 
// Destructor
// ----------------------------------------------------------------------------
//
CCreatedTestSetMenuView::~CCreatedTestSetMenuView()
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
// CreatedTestSetMenuView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CCreatedTestSetMenuView::Id() const
    {
    return TUid::Uid(ECreatedTestSetMenuViewId);

    }

// ----------------------------------------------------------------------------
// CreatedTestSetMenuView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CCreatedTestSetMenuView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
            {
            case EAknSoftkeyBack:
                {
                AppUi()->HandleCommandL(EAppUIGoToTestSetsMenu);
                break;
                }
    		case EAknCmdOpen:
    			{
    		   		HandleListBoxEventL(iContainer->ListBox(), EEventEnterKeyPressed);	
    			break;
    			}
            default:
                {
                AppUi()->HandleCommandL( aCommand );
                break;
                }
            }

    }

// ----------------------------------------------------------------------------
//CreatedTestSetMenuView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CCreatedTestSetMenuView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }

    }

// ----------------------------------------------------------------------------
// CreatedTestSetMenuView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CCreatedTestSetMenuView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("CreatedTestSetMenuView: DoActivateL"));
        iContainer = new (ELeave) CCreatedTestSetMenuContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect(), this );
           ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("CreatedTestSetMenuView: container constructed"));
        AppUi()->AddToStackL( *this, iContainer );
        }

    iContainer->SetCurrentItemIndex(iCurrentTestCase);
    
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_LOADTESTSET);
	iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	np->PushL(*iNaviDecorator);

    }

// ----------------------------------------------------------------------------
// CreatedTestSetMenuView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CCreatedTestSetMenuView::DoDeactivate()
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


// ----------------------------------------------------------------------------
// CreatedTestSetMenuView::HandleListBoxEventL
// 
// Handles listbox events.
// ----------------------------------------------------------------------------
//
void CCreatedTestSetMenuView::HandleListBoxEventL( CEikListBox* aListBox, 
    TListBoxEvent aEventType )
	{
	if (  aEventType == MEikListBoxObserver::EEventEnterKeyPressed    )
		{
	 	// Checking with item from list was selected	
		CTextListBoxModel* tmp = (CTextListBoxModel*)aListBox->Model();
		TPtrC item = tmp->ItemText(aListBox->CurrentItemIndex());
		// Geting propper format of item from list (ex. 2008_8_10_13_16.set ). 
		TPtrC substracted(item.Left(item.Length()-2));
		substracted.Set(substracted.Right(substracted.Length()-2));
		// Creating new view containing 		
       	CTestSetMenuView* testSetMenu = (CTestSetMenuView*)AppUi()->View( TUid::Uid(ETestSetMenuViewId) ); 
       	User::LeaveIfError( testSetMenu->LoadTestSetL( substracted ) );
		AppUi()->HandleCommandL(ECmdGoToTestSetView);		
        }
 
    }

// End of File
