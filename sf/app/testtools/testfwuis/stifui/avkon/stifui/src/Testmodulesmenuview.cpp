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
* Description: This file contains CTestModulesMenuView class 
* definition.
*
*/

// INCLUDE FILES
#include  <aknViewAppUi.h>
#include  <Stifui.rsg>

#include  <barsread.h>	//TResourceReader
#include  "TestModulesMenuView.h"
#include  "TestModulesMenuContainer.h"
#include  "Stifui.hrh"

#include "AppUIAppUi.h" // For ShowNotImplementedYet() method

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestModulesMenuView::ConstructL
// 
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CTestModulesMenuView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL( R_APPUI_TESTMODULESMENUVIEW );
    }

// ----------------------------------------------------------------------------
// CTestModulesMenuView::~CTestCaseOutputView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestModulesMenuView::~CTestModulesMenuView()
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
// CTestModulesMenuView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CTestModulesMenuView::Id() const
    {
    return TUid::Uid(ETestModulesMenuViewId); //KTestCaseMenuViewId;
    }

// ----------------------------------------------------------------------------
// CTestModulesMenuView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CTestModulesMenuView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        case EAknSoftkeyOk:
            {
            iEikonEnv->InfoMsg( _L("TestCase Menu ok") );
            break;
            }
        case EAknSoftkeyBack:
            {
            AppUi()->HandleCommandL(EAppUIGoBack);
            break;
            }
        // Next is removed from Stifui.rss file in 
        // RESOURCE MENU_PANE r_appui_testmodulesmenuview_menu's
        // items section. If want back to use remeve comment there
        //case ECmdOpenModule:
        //    {
            //TFileName fileName;
            //AknCommonDialogs::RunSelectDlgLD (fileName,
            //                                  R_MEMORY_SELECTION_DIALOG,
            //                                  R_FILE_SELECTION_DIALOG
            //
            //);
        //    ShowNotImplementedYet();
        //    break;
        //    }
        case ECmdAddModule:
            {
            TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->LoadAllModules();
            if( KErrNone != ret )
                {
                iEikonEnv->InfoMsg( _L("Module adding fails!") );
                break;
                }

            AppUi()->HandleCommandL( EAppUIGoBack );
            //ShowNotImplementedYet();
            break;
            }
        case ECmdRemoveModule:
            {
            TInt moduleNumber = iContainer->CurrentItemIndex();
            RRefArray<TDesC> allModules;
            TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->Modules( allModules );
            if( KErrNone != ret )
                {
                allModules.Reset();
                allModules.Close();
                User::Leave( ret );
                }
            // No modules exist...break...
            if( allModules.Count() == 0 )
                {
                iEikonEnv->InfoMsg( _L("No modules available!") );
                allModules.Reset();
                allModules.Close();
                break;
                }
            // Module found...remove module...
            ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->RemoveTestModule(
                                        allModules[ moduleNumber ] );
            if( KErrNone != ret )
                {
                User::Leave( ret );
                }
            allModules.Reset();
            allModules.Close();

            //iContainer->UpdateView();
            AppUi()->HandleCommandL( EAppUIGoBack );
            //ShowNotImplementedYet();
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
// CTestModulesMenuView::DynInitMenuPaneL
// 
// Initializes menu pane.
// ----------------------------------------------------------------------------
//
void CTestModulesMenuView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* /* aMenuPane */)
    {

    if (R_APPUI_TESTMODULESMENUVIEW_MENU == aResourceId)
        {
        /*switch ( iData->StartedCaseInOutputView()->State() )
            {
            case ENotStarted:
                {
                aMenuPane->SetItemDimmed( ECmdOpenModule, ETrue );
                aMenuPane->SetItemDimmed( ECmdAddModule, ETrue );
                aMenuPane->SetItemDimmed( ECmdRemoveModule, ETrue );
                break;
                }
            case ERunning:
                {
                aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
                break;
                }
            case EPaused:
                {
                aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
                break;
                }
            case EPassed:
            case EFailed:
            case EAborted:
            case EExecuted:
                {
                aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdAbortTestCase, ETrue );
                break;
                }
            }*/
        }
    }

// ----------------------------------------------------------------------------
// CTestModulesMenuView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CTestModulesMenuView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ----------------------------------------------------------------------------
// CTestModulesMenuView::HandleListBoxEventL
// 
// Handles listbox events.
// ----------------------------------------------------------------------------
//
void CTestModulesMenuView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
	{
	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{
        // Open or edit test module or ???
        }
    }

// ----------------------------------------------------------------------------
// CTestModulesMenuView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CTestModulesMenuView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CTestModulesMenuContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect(), this);
        AppUi()->AddToStackL( *this, iContainer );
        } 
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_MODULES);
	iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	np->PushL(*iNaviDecorator);
   }

// ----------------------------------------------------------------------------
// CTestModulesMenuView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CTestModulesMenuView::DoDeactivate()
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
