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
* Description: This file contains CTestCaseMenuView class
* definition.
*
*/

// INCLUDE FILES
#include  <aknViewAppUi.h>
#include  <barsread.h>	//TResourceReader
#include  <Stifui.rsg>
#include  "TestCaseMenuView.h"
#include  "TestCaseMenuContainer.h"
#include  "Stifui.hrh" 
#include  "AppUIAppUi.h" // loggerin kayttoa varten, muuten ei tarvittaisi

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestCaseMenuView::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CTestCaseMenuView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL( R_APPUI_TESTCASEMENUVIEW );
    }

// ----------------------------------------------------------------------------
// CTestCaseMenuView::~CTestCaseMenuView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestCaseMenuView::~CTestCaseMenuView()
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

// ----------------------------------------------------------------------------
// CTestCaseMenuView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CTestCaseMenuView::Id() const
    {
    return TUid::Uid(ETestCaseMenuViewId); //KTestCaseMenuViewId;
    }

// ----------------------------------------------------------------------------
// CTestCaseMenuView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CTestCaseMenuView::HandleCommandL(TInt aCommand)
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
// CTestCaseMenuView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CTestCaseMenuView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ----------------------------------------------------------------------------
// CTestCaseMenuView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CTestCaseMenuView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("TestCaseMenu: DoActivateL"));
        //message = _L("eng.open ret:");
        //message.AppendNum( ret , EDecimal );
        //AppUi->iLogger->Log( message );
        iContainer = new (ELeave) CTestCaseMenuContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("TestCaseMenu: container constructed"));
        AppUi()->AddToStackL( *this, iContainer );
        } 
    
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_TESTCASES);
	iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	np->PushL(*iNaviDecorator);
   }

// ----------------------------------------------------------------------------
// CTestCaseMenuView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CTestCaseMenuView::DoDeactivate()
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
