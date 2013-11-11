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
* Description: This file contains CShowStartedCasesView class
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
#include  "ShowStartedCasesView.h"
#include  "ShowStartedCasesContainer.h"
#include  "AppUIAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CShowStartedCasesView::ConstructL
// 
// Symbian OS two-phased constructor
// ----------------------------------------------------------------------------
//
void CShowStartedCasesView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL( R_APPUI_SHOWSTARTEDCASESVIEW );
    iFilterModule.Zero();
    iFilterTestCaseFile.Zero();
    iSelectedTestCase=0;
    iNaviDecorator = NULL;
    }

// ----------------------------------------------------------------------------
// CShowStartedCasesView::~CShowStartedCasesView()
// 
// Destructor
// ----------------------------------------------------------------------------
//
CShowStartedCasesView::~CShowStartedCasesView()
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
// TUid CShowStartedCasesView::Id()
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CShowStartedCasesView::Id() const
    {
    return TUid::Uid(EShowStartedCasesViewId);
    }

// ----------------------------------------------------------------------------
// CShowStartedCasesView::HandleCommandL(TInt aCommand)
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesView::HandleCommandL(TInt aCommand)
    {   
    if ( aCommand >= ECmdFilterByModule && aCommand < ECmdFilterByTestCaseFile )
        {
        TInt moduleNumber = aCommand - ECmdFilterByModule;
        RRefArray<TDesC> testModules;
        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->Modules( testModules );
        if( KErrNone != ret )
            {
            testModules.Reset();
            testModules.Close();
            User::Leave( ret );
            }
        iFilterModule = testModules[moduleNumber];
            
        iFilterTestCaseFile.Zero();
        
        iContainer->FilterCasesByModuleL(iFilterModule);
        //iContainer->ResetListBox();
        //iContainer->DrawListBox();
        
        testModules.Reset();
        testModules.Close();

        return;
        }

    else if ( aCommand >= ECmdFilterByTestCaseFile && aCommand < ECmdNOFiltering )
        {
        TInt testCaseFileNumber = aCommand - ECmdFilterByTestCaseFile;
        RRefArray<TDesC> testCaseFiles;
        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->TestCaseFiles( testCaseFiles );
        if( KErrNone != ret )
            {
            testCaseFiles.Reset();
            testCaseFiles.Close();
            User::Leave( ret );
            }

        iFilterTestCaseFile = testCaseFiles[ testCaseFileNumber ];
        iFilterModule.Zero();
        //iContainer->FilterCasesByTCFileNameL(testCaseFileName);
        iContainer->FilterCasesByTCFileNameL(iFilterTestCaseFile);
        iContainer->ResetListBox();
        iContainer->DrawListBox();

        testCaseFiles.Reset();
        testCaseFiles.Close();

        return;
        }

    else if ( aCommand == ECmdNOFiltering )
        {
        iFilterModule.Zero();
        iFilterTestCaseFile.Zero();
        iContainer->NoFilteringL();
        iContainer->ResetListBox();
        iContainer->DrawListBox();
        return;
        }

    // Handle rest possible commands
    switch ( aCommand )
        {
        case ECmdViewOutput:
            {
            ViewTestCaseOutputL();
            break;
            }
        case ECmdRemoveExecution:
            {
            iContainer->RemoveSelectedExecutionsL();
            break;
            }
        case ECmdRemoveAllExecutions:
            {
            iContainer->RemoveAllExecutionsInViewL();
            break;
            }
        case ECmdPauseTestCase:
            {
            TInt index = iContainer->CurrentItemIndex();
            RRefArray<CStartedTestCase> runningTestCases;
            TShowStartedCasesMode currentMode = (TShowStartedCasesMode)((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode;
			TInt ret = KErrNone;
			if(currentMode == EShowAllStartedCases)
				ret = iUIStore->StartedTestCases(runningTestCases, CUIStoreIf::EStatusAll);
			else if(currentMode == EShowOngoingCases)
				ret = iUIStore->StartedTestCases(runningTestCases, CUIStoreIf::EStatusRunning);
 			if( KErrNone != ret )
 			    {
 			    User::Leave( ret );
 			    }
 			runningTestCases[index].UIEngineContainer().PauseTest();
 			runningTestCases.Close();
            break;
            }
        case ECmdResumeTestCase:
            {
            TInt index = iContainer->CurrentItemIndex();
            RRefArray<CStartedTestCase> runningTestCases;
            TShowStartedCasesMode currentMode = (TShowStartedCasesMode)((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode;
			TInt ret = KErrNone;
			if(currentMode == EShowAllStartedCases)
				ret = iUIStore->StartedTestCases(runningTestCases, CUIStoreIf::EStatusAll);
			else if(currentMode == EShowOngoingCases || currentMode == EShowPausedCases)
				ret = iUIStore->StartedTestCases(runningTestCases, CUIStoreIf::EStatusRunning);
 			if( KErrNone != ret )
 			    {
 			    User::Leave( ret );
 			    }
 			TInt testCaseToControl = 0;
 			if(currentMode == EShowAllStartedCases || currentMode == EShowOngoingCases)
				{
				testCaseToControl = index;
				}
			else if(currentMode == EShowPausedCases)
				{
				TInt pausedTestCasesCounter = 0;
				for(int i = 0; i < runningTestCases.Count(); i++)
					{
					if(runningTestCases[i].UIEngineContainer().State() == CUIEngineContainer::EPaused)
						{
						if(pausedTestCasesCounter == index)
							{
							testCaseToControl = i;
							break;
							}
						pausedTestCasesCounter++;
						}
					}
				}
 			
 			runningTestCases[testCaseToControl].UIEngineContainer().ResumeTest();
 			runningTestCases.Close();
            break;
            }
        case ECmdAbortTestCase:
            { 
			TInt index = iContainer->CurrentItemIndex();
			RRefArray<CStartedTestCase> runningTestCases;
			TShowStartedCasesMode currentMode = (TShowStartedCasesMode)((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode;
			TInt ret = KErrNone;
			if(currentMode == EShowAllStartedCases)
				ret = iUIStore->StartedTestCases(runningTestCases, CUIStoreIf::EStatusAll);
			else if(currentMode == EShowOngoingCases || currentMode == EShowPausedCases)
				ret = iUIStore->StartedTestCases(runningTestCases, CUIStoreIf::EStatusRunning);
				
			if( KErrNone != ret )
			    {
			    User::Leave( ret );
			    }
			TInt testCaseToControl = 0;
			
			if(currentMode == EShowAllStartedCases || currentMode == EShowOngoingCases)
				{
				testCaseToControl = index;
				}
			else if(currentMode == EShowPausedCases)
				{
				TInt pausedTestCasesCounter = 0;
				for(int i = 0; i < runningTestCases.Count(); i++)
					{
					if(runningTestCases[i].UIEngineContainer().State() == CUIEngineContainer::EPaused)
						{
						if(pausedTestCasesCounter == index)
							{
							testCaseToControl = i;
							break;
							}
						pausedTestCasesCounter++;
						}
					}
				}
			
			runningTestCases[testCaseToControl].UIEngineContainer().CancelTest();
			runningTestCases.Close();
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

    }

// ----------------------------------------------------------------------------
// CShowStartedCasesView::HandleListBoxEventL
// 
// Handles listbox events.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
	{
	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{
        ViewTestCaseOutputL();
        }

    }

// ----------------------------------------------------------------------------
// CShowStartedCasesView::ViewTestCaseOutputL
// 
// Shows outputs of test case which is selected in Container.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesView::ViewTestCaseOutputL()
	{
    iSelectedTestCase = iContainer->CurrentItemIndex();
    
    // Sets index  of selected test case to AppUi
    //( ( CAppUIAppUi* )AppUi() )->SetTestCaseIndex( iSelectedTestCase );
    ( ( CAppUIAppUi* )AppUi() )->SetStartedTestCase( iContainer->CurrentTestCase() );
    
    AppUi()->HandleCommandL(ECmdViewOutput);
    }


// ----------------------------------------------------------------------------
// CShowStartedCasesView::HandleClientRectChange()
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ----------------------------------------------------------------------------
// CShowStartedCasesView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CShowStartedCasesContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect(), this );
        AppUi()->AddToStackL( *this, iContainer );
        } 

    // Check if filtering by module or by test case file is selected
    if ( iFilterModule.Length() )
        {
        iContainer->FilterCasesByModuleL(iFilterModule);
        }
    else if ( iFilterTestCaseFile.Length() )
        {
        iContainer->FilterCasesByTCFileNameL(iFilterTestCaseFile);
        }
    if ( iSelectedTestCase )
        iContainer->SetCurrentItemIndex(iSelectedTestCase);
    
    RefreshNaviTitleL();
    
    iContainer->DrawListBox();
   }


// ----------------------------------------------------------------------------
// CShowStartedCasesView::RefreshNaviTitle
// 
// Refreshes view name shown in the navi pane.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesView::RefreshNaviTitleL()
    {
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	TResourceReader reader;

    switch ( ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode )
        {
        case EShowAllStartedCases:
            iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_STARTED_ALL);
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
// CShowStartedCasesView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesView::DoDeactivate()
    {
    if ( iContainer )
        {
        TInt mode = ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode;
        if(mode != EShowOngoingCases && mode != EShowPausedCases
		    	&& mode != EShowCrashedAbortedCases  && mode != EShowAllStartedCases)
        	{
        	iContainer->SaveActiveLine();
        	}
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;        
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;

    }


// ----------------------------------------------------------------------------
// CShowStartedCasesView::DynInitMenuPaneL
// 
// Initializes menu pane.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesView::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    // options menu
    if ( R_APPUI_SHOWSTARTEDCASESVIEW_MENU == aResourceId )
        {
        CStartedTestCase* startedTestCase = iContainer->SelectedTestCase();
        if ( NULL != startedTestCase )
            {
            aMenuPane->SetItemDimmed( ECmdFilterMenu, EFalse );
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
            //aMenuPane->SetItemDimmed( ECmdFilterMenu, ETrue );
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

    // Test modules are added to filter by test module submenu if the submenu is opened
    if (R_APPUI_FILTERBYMODULES_MENU == aResourceId)
        {
        RRefArray<TDesC> testModules;
        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->Modules( testModules );
        if( KErrNone != ret )
            {
            testModules.Reset();
            testModules.Close();
            User::Leave( ret );
            }        
        TInt moduleCount = testModules.Count();

        TInt i;
        CEikMenuPaneItem::SData item;

        item.iCommandId = ECmdFilterByModule;
        item.iFlags = 0;
        item.iCascadeId = 0;

        for (i = 0; i < moduleCount; i++)
            {
            item.iText = testModules[i];
            aMenuPane->AddMenuItemL(item);
            item.iCommandId++; // command IDs 0x1000 - 0x1FFF are reserved for modules in hrh file
            }
        
        testModules.Reset();
        testModules.Close();

        }

    // Test case files are added to filter by test case file submenu if the submenu is opened
    if (R_APPUI_FILTER_BY_TESTCASEFILE_MENU == aResourceId)
        {
        RRefArray<TDesC> testCaseFiles;
        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->TestCaseFiles( testCaseFiles );
        if( KErrNone != ret )
            {
            testCaseFiles.Reset();
            testCaseFiles.Close();
            User::Leave( ret );
            }        
        TInt testCaseFileCount = testCaseFiles.Count();

        TInt i;
        CEikMenuPaneItem::SData item;

        item.iCommandId = ECmdFilterByTestCaseFile;
        item.iFlags = 0;
        item.iCascadeId = 0;

        for (i = 0; i < testCaseFileCount; i++)
            {
            item.iText = testCaseFiles[i];
            // If there´s no test case file, don´t add item to menu.
            if ( testCaseFiles[i].Length() > 0 )
                {
                aMenuPane->AddMenuItemL(item);
                }
            item.iCommandId++; // command IDs 0x2000 - 0x2FFF are reserved for test case files in hrh file
            }
        
        testCaseFiles.Reset();
        testCaseFiles.Close();

        }

    }

// ----------------------------------------------------------------------------
// CShowStartedCasesView::TestCaseStateChangedL
// 
// Handles status changes of test cases in view.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesView::TestCaseStateChangedL()
    {
    if ( iContainer )
        {

        //CStartedTestCase* currentlySelectedTest = iContainer->SelectedTestCase();

        TInt numOfTestsInViewBeforeStateChange = iContainer->TestsInViewCount();

            //((CStartedTestsListBoxModel*)iContainer->iListBox->Model())->MdcaCount();

        iContainer->ConstructListBoxModelL();

        // Check if filtering by module or by test case file is selected
        if ( iFilterModule.Length() )
            {
            iContainer->FilterCasesByModuleL(iFilterModule);
            }
        else if ( iFilterTestCaseFile.Length() )
            {
            iContainer->FilterCasesByTCFileNameL(iFilterTestCaseFile);
            }

        TInt numOfTestsInViewAfterStateChange = iContainer->TestsInViewCount();
            //((CStartedTestsListBoxModel*)iContainer->iListBox->Model())->MdcaCount();

        if ( numOfTestsInViewAfterStateChange >= numOfTestsInViewBeforeStateChange )
            iContainer->HandleItemAdditionL();
        else
            iContainer->HandleItemRemovalL();
        
        if(iContainer->CurrentItemIndex() < 0)
        	{
        	iContainer->SetCurrentItemIndex(0);
        	}
        iContainer->DrawListBox();

        }
    }

// End of File
