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
* Description: This file contains CTestSetBaseMenuView class
* definition.
*
*/

// INCLUDE FILES
#include  <aknViewAppUi.h>
#include  <aknlists.h> 
#include  <barsread.h>	//TResourceReader
#include  <Stifui.rsg>
#include  "TestSetBaseMenuView.h"
#include  "TestSetBaseMenuContainer.h"
#include  "Stifui.hrh" 
#include  "AppUIAppUi.h" // loggerin kayttoa varten, muuten ei tarvittaisi

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTestSetBaseMenuView::ConstructL
// 
// Symbian OS two-phased constructor.
// ---------------------------------------------------------
//
void CTestSetBaseMenuView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL( R_APPUI_TESTSETBASEMENUVIEW );
    }

// ----------------------------------------------------------------------------
// CTestSetBaseMenuView::~CTestSetBaseMenuView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestSetBaseMenuView::~CTestSetBaseMenuView()
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
// CTestSetBaseMenuView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CTestSetBaseMenuView::Id() const
    {
    return TUid::Uid(ETestSetBaseMenuViewId); //KTestSetBaseMenuViewId;
    }

// ----------------------------------------------------------------------------
// CTestSetBaseMenuView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CTestSetBaseMenuView::HandleCommandL(TInt aCommand)
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
			break;
			}
		case ECmdCreateTestSet:
            {
            AppUi()->HandleCommandL(ECmdCreateTestSet);
            break;
            }
		case ECmdLoadTestSet:
            {
            AppUi()->HandleCommandL(ECmdLoadTestSet);
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
// CTestSetBaseMenuView::HandleListBoxEventL
// 
// Handles a list box event.
// ----------------------------------------------------------------------------
//
void CTestSetBaseMenuView::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
	{
	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{
	    switch ( aListBox->CurrentItemIndex() )
			{
		    case 0:
		    	HandleCommandL( ECmdCreateTestSet );
				break;
			case 1:
				HandleCommandL( ECmdLoadTestSet );
				break;
			}
		}
	}
	
		
// ----------------------------------------------------------------------------
// CTestSetBaseMenuView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CTestSetBaseMenuView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ----------------------------------------------------------------------------
// CTestSetBaseMenuView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CTestSetBaseMenuView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("TestSetBaseMenu: DoActivateL"));
        //message = _L("eng.open ret:");
        //message.AppendNum( ret , EDecimal );
        //AppUi->iLogger->Log( message );
        iContainer = new (ELeave) CTestSetBaseMenuContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect(), this );
        ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("TestSetBaseMenu: container constructed"));
        AppUi()->AddToStackL( *this, iContainer );
        } 
	
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_TESTSET_BASE);
	iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	np->PushL(*iNaviDecorator);

   }

// ----------------------------------------------------------------------------
// CTestSetBaseMenuView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CTestSetBaseMenuView::DoDeactivate()
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
