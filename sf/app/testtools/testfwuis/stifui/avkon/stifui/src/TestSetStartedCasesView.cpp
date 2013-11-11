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
* Description: This file contains CTestSetStartedCasesView class
* definition.
*
*/

// INCLUDE FILES
#include  <eikmenub.h>
#include  <aknViewAppUi.h>
#include  <barsread.h>	//TResourceReader
#include  <Stifui.rsg>

//#include  "UIEngineContainer.h"  
#include <stifinternal/UIEngineContainer.h>

#include  "Stifui.hrh" 

#include  "TestSetStartedCasesView.h"
#include  "TestSetStartedCasesContainer.h"
#include  "AppUIAppUi.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestSetStartedCasesView::ConstructL
// 
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL( R_TESTSET_STARTEDCASESVIEW );
    iFilterModule.Zero();
    iFilterTestCaseFile.Zero();
    iSelectedTestCase=0;
    iNaviDecorator = NULL;
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesView::~CTestSetStartedCasesView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestSetStartedCasesView::~CTestSetStartedCasesView()
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
// CTestSetStartedCasesView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CTestSetStartedCasesView::Id() const
    {
    return TUid::Uid(ETestSetStartedCasesViewId);
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesView::HandleCommandL(TInt aCommand)
    {   
	TBool refreshListBox = EFalse;
    // Handle rest possible commands
    switch ( aCommand )
        {
        case ECmdShowAllStartedCases:
            {
            ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode = EShowAllStartedCases;
            refreshListBox = ETrue;
            break;
            }
        case ECmdShowOngoingCases:
            {
            ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode = EShowOngoingCases;
            refreshListBox = ETrue;
            break;
            }
        case ECmdShowPausedCases:
            {
            ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode = EShowPausedCases;
            refreshListBox = ETrue;
            break;
            }
        case ECmdShowPassedCases:
            {
            ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode = EShowPassedCases;
            refreshListBox = ETrue;
            break;
            }
        case ECmdShowFailedCases:
            {
            ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode = EShowFailedCases;
            refreshListBox = ETrue;
            break;
            }
        case ECmdShowCrashedAbortedCases:
            {
            ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode = EShowCrashedAbortedCases;
            refreshListBox = ETrue;
            break;
            }
        case ECmdShowStatistics:
            {
            //EShowStatistics
            TMessageBoxUtil::ShowNotImplementedYetL();
            break;
            }
        case ECmdViewOutput:
            {
            ViewTestCaseOutputL();
            break;
            }
        case ECmdPauseTestCase:
            {
            CStartedTestCase* startedTestCase = iContainer->SelectedTestCase();
            if ( NULL != startedTestCase )
                {
                startedTestCase->UIEngineContainer().PauseTest();
                }
            break;
            }
        case ECmdResumeTestCase:
            {
            CStartedTestCase* startedTestCase = iContainer->SelectedTestCase();
            if ( NULL != startedTestCase )
                {
                startedTestCase->UIEngineContainer().ResumeTest();
                }
            break;
            }
        case ECmdAbortTestCase:
            {
            CStartedTestCase* startedTestCase = iContainer->SelectedTestCase();
            if ( NULL != startedTestCase )
                {
                startedTestCase->UIEngineContainer().CancelTest();
                }
            break;
            }
        case EAknSoftkeyOk:
            {
            iEikonEnv->InfoMsg( _L("TestCase Menu ok") );
            break;
            }
        case EAknSoftkeyBack:
            {
            // Remove possible filterings so that they does not affect when coming again to this view
            iFilterModule.Zero();
            iFilterTestCaseFile.Zero();
            iSelectedTestCase=0; //Reset selected test case information
            AppUi()->HandleCommandL(EAppUIGoBack);
            break;
            }
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
        
        if ( refreshListBox )
        	{
        	RefreshNaviTitleL();
        	iContainer->ConstructListBoxModelL();
        	iContainer->DrawListBox();
        	}

    }
    

// ----------------------------------------------------------------------------
// CTestSetStartedCasesView::RefreshNaviTitleL
// 
// Refreshes view name shown in the navi pane.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesView::RefreshNaviTitleL()
    {
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	TResourceReader reader;

    switch ( ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode )
        {
        case EShowAllStartedCases:
            iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_STARTED);
            break;
        case EShowOngoingCases:
            iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_STARTED_ONGOING);
            break;
        case EShowPausedCases:
            iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_STARTED_PAUSED);
            break;
        case EShowPassedCases:
            iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_STARTED_PASSED);
            break;
        case EShowFailedCases:
            iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_STARTED_FAILED);
            break;
        case EShowCrashedAbortedCases:
            iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_STARTED_CRASHED_ABORTED);
            break;
        default:
            iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_TESTSET);
            break;
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;        
    iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	np->PushL(*iNaviDecorator);
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesView::HandleListBoxEventL
// 
// Handles listbox events.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
	{
	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{
        ViewTestCaseOutputL();
        }

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesView::ViewTestCaseOutput
// 
// Shows outputs of test case which is selected in Container.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesView::ViewTestCaseOutputL()
	{
    iSelectedTestCase = iContainer->CurrentItemIndex();
    
    // Sets index  of selected test case to AppUi
    ( ( CAppUIAppUi* )AppUi() )->SetStartedTestCase( iContainer->CurrentTestCase() );
    
    AppUi()->HandleCommandL(ECmdViewOutput);
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CTestSetStartedCasesContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect(), this );
        AppUi()->AddToStackL( *this, iContainer );
        } 

    if ( iSelectedTestCase )
        iContainer->SetCurrentItemIndex(iSelectedTestCase);

    iContainer->DrawListBox();
    
    RefreshNaviTitleL();
   }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesView::DoDeactivate()
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
// CTestSetStartedCasesView::DynInitMenuPaneL
// 
// Initializes menu pane.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesView::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
    {

    // options menu
    if ( R_TESTSET_STARTEDCASESVIEW_MENU == aResourceId )
        {
        CStartedTestCase* startedTestCase = iContainer->SelectedTestCase();
        if ( NULL != startedTestCase )
            {
            switch ( startedTestCase->Status() )
                {
                // test case running
                case CUIStoreIf::EStatusRunning:
                    {
                    switch ( startedTestCase->UIEngineContainer().State() )
                        {
                        case CUIEngineContainer::ERunning:
                        case CUIEngineContainer::EPaused:
                            {
                            aMenuPane->SetItemDimmed( ECmdShowSetMenu, EFalse );
                            break;
                            }
                        //case CUIEngineContainer::ENotStarted:
                        //case CUIEngineContainer::EExecuted:
                        //case CUIEngineContainer::EFinished:
                        default:
                            {
                            aMenuPane->SetItemDimmed( ECmdShowSetMenu, ETrue );
                            }
                        }
                    break;
                    }
                        
                // test case not running
                //case CUIStoreIf::EStatusPassed:
                //case CUIStoreIf::EStatusFailed:
                //case CUIStoreIf::EStatusAborted:
                //case CUIStoreIf::EStatusExecuted
                default:
                    {
                    aMenuPane->SetItemDimmed( ECmdShowSetMenu, ETrue );
                    }
                }
            }
        else
            {
            aMenuPane->SetItemDimmed( ECmdViewOutput, ETrue );
            aMenuPane->SetItemDimmed( ECmdShowSetMenu, ETrue );
            }
        }

    // test case control menu
    if ( R_TESTCASE_CONTROL_SUBMENU == aResourceId )
        {
        CStartedTestCase* startedTestCase = iContainer->SelectedTestCase();
        
        if ( NULL != startedTestCase )
            {
            if ( startedTestCase->Status() == CUIStoreIf::EStatusRunning )
                {
                switch ( startedTestCase->UIEngineContainer().State() )
                    {
                    case CUIEngineContainer::ERunning:
                        {
                        aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
                        aMenuPane->SetItemDimmed( ECmdPauseTestCase, EFalse );
                        aMenuPane->SetItemDimmed( ECmdAbortTestCase, EFalse );
                        break;
                        }
                    case CUIEngineContainer::EPaused:
                        {
                        aMenuPane->SetItemDimmed( ECmdResumeTestCase, EFalse );
                        aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
                        aMenuPane->SetItemDimmed( ECmdAbortTestCase, EFalse );
                        break;
                        }
                    //case CUIEngineContainer::ENotStarted:
                    //case CUIEngineContainer::EExecuted:
                    //case CUIEngineContainer::EFinished:
                    default:
                        {
                        aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
                        aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
                        aMenuPane->SetItemDimmed( ECmdAbortTestCase, ETrue );
                        break;
                        }
                    }
                }
            else
                {
                aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdAbortTestCase, ETrue );
                }
            }
        else
            {
            aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
            aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
            aMenuPane->SetItemDimmed( ECmdAbortTestCase, ETrue );
            }
        }
            
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesView::TestCaseStateChangedL
// 
// Handles status changes of test cases in view.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesView::TestCaseStateChangedL()
    {
    if ( iContainer )
        {

        //CStartedTestCase* currentlySelectedTest = iContainer->SelectedTestCase();

        TInt numOfTestsInViewBeforeStateChange = iContainer->TestsInViewCount();

            //((CStartedTestsListBoxModel*)iContainer->iListBox->Model())->MdcaCount();

        iContainer->ConstructListBoxModelL();

        // Check if filtering by module or by test case file is selected
        /*
        if ( iFilterModule.Length() )
            {
            iContainer->FilterCasesByModuleL(iFilterModule);
            }
        else if ( iFilterTestCaseFile.Length() )
            {
            iContainer->FilterCasesByTCFileNameL(iFilterTestCaseFile);
            }
        */

        TInt numOfTestsInViewAfterStateChange = iContainer->TestsInViewCount();
            //((CStartedTestsListBoxModel*)iContainer->iListBox->Model())->MdcaCount();

        if ( numOfTestsInViewAfterStateChange >= numOfTestsInViewBeforeStateChange )
            iContainer->HandleItemAdditionL();
        else
            iContainer->HandleItemRemovalL();

        iContainer->DrawListBox();

        }
    }

// End of File
