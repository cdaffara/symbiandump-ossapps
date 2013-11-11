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
* Description: This file contains CTestCaseOutputView class 
* definition.
*
*/

// INCLUDE FILES
#include  <eikmenub.h>
#include  <aknViewAppUi.h>
#include  <akntitle.h>
#include  <barsread.h>	//TResourceReader
#include  <Stifui.rsg>

//#include "UIStoreIf.h"         
//#include "UIStore.h"            
//#include "UIEngine.h"           
//#include "UIEngineContainer.h"  

#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStore.h>
#include <stifinternal/UIEngine.h>
#include <stifinternal/UIEngineContainer.h>

#include  "TestCaseOutputView.h"
#include  "TestCaseOutputContainer.h"
#include  "AppUIAppUi.h"
#include  "Stifui.hrh" 

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestCaseOutputView::ConstructL
// 
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL( R_APPUI_TESTCASEOUTPUTVIEW );
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputView::~CTestCaseOutputView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestCaseOutputView::~CTestCaseOutputView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    
    iCurrentTestCase = NULL;
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CTestCaseOutputView::Id() const
    {
    return TUid::Uid(ETestCaseOutputViewId);
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            AppUi()->HandleCommandL(EAppUIGoBack);
            break;
            }
        case ECmdPauseTestCase:
            {
            /*
            TInt index( 0 );
			index = ( ( CAppUIAppUi* )AppUi() )->GetTestCaseIndex();
             CStartedTestCase* startedTestCase = NULL;
             TRAPD( ret, startedTestCase = &iUIStore->StartedTestCaseL( index ) );
             if( ret != KErrNone )
                 {
                 // Leave
                 }
            */
            CStartedTestCase* startedTestCase = 
                ( ( CAppUIAppUi* )AppUi() )->GetStartedTestCase();
            startedTestCase->UIEngineContainer().PauseTest(); 
            break;
            }
        case ECmdResumeTestCase:
            {
            /*
            TInt index( 0 );
			index = ( ( CAppUIAppUi* )AppUi() )->GetTestCaseIndex();
             CStartedTestCase* startedTestCase = NULL;
             TRAPD( ret, startedTestCase = &iUIStore->StartedTestCaseL( index ) );
             if( ret != KErrNone )
                 {
                 // Leave
                 }
            */
            CStartedTestCase* startedTestCase = 
                ( ( CAppUIAppUi* )AppUi() )->GetStartedTestCase();
            startedTestCase->UIEngineContainer().ResumeTest();            
            break;
            }
        case ECmdAbortTestCase:
            {
            /*
            TInt index( 0 );
			index = ( ( CAppUIAppUi* )AppUi() )->GetTestCaseIndex();
             CStartedTestCase* startedTestCase = NULL;
             TRAPD( ret, startedTestCase = &iUIStore->StartedTestCaseL( index ) );
             if( ret != KErrNone )
                 {
                 // Leave
                 }
            */
            CStartedTestCase* startedTestCase = 
                ( ( CAppUIAppUi* )AppUi() )->GetStartedTestCase();
            startedTestCase->UIEngineContainer().CancelTest();             
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
// CTestCaseOutputView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }

    }

// ----------------------------------------------------------------------------
// CTestCaseOutputView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CTestCaseOutputContainer;
        iContainer->SetMopParent(this);

        iCurrentTestCase = ( ( CAppUIAppUi* )AppUi() )->GetStartedTestCase();
        iContainer->ConstructL( ClientRect(), iCurrentTestCase );
        
        AppUi()->AddToStackL( *this, iContainer );
        }
    PrintTestCaseStateL();
	
	CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_TESTCASE_OUTPUT);
	iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	np->PushL(*iNaviDecorator);
	
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
    // Because DoDeactivate method can't leave we must 
    // catch unexpected leaves.
	TInt ret = KErrNone;
    TRAP( ret,
    CAknTitlePane* tp=(CAknTitlePane*)sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle));
    if ( tp != NULL )
    	{
    	tp->SetTextToDefaultL(); // Set application name.
    	}
    ); // TRAPD end
    
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    
    iCurrentTestCase = NULL;

    }

// ----------------------------------------------------------------------------
// CTestCaseOutputView::DynInitMenuPaneL
// 
// Initializes menu pane.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputView::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    CStartedTestCase* startedTestCase = 
        ( ( CAppUIAppUi* )AppUi() )->GetStartedTestCase();

    if (R_APPUI_TESTCASEOUTPUTVIEW_MENU == aResourceId)
        {
  
        if ( startedTestCase->Status() == CUIStoreIf::EStatusRunning )
            {
            if( startedTestCase->UIEngineContainer().State() == CUIEngineContainer::ENotStarted )
                {
                aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdAbortTestCase, ETrue );
                }
            else if( startedTestCase->UIEngineContainer().State() == CUIEngineContainer::ERunning )
                {
                aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdPauseTestCase, EFalse );
                }
            else if( startedTestCase->UIEngineContainer().State() == CUIEngineContainer::EPaused )
                {
                aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdResumeTestCase, EFalse );
                }
            else if( startedTestCase->UIEngineContainer().State() == CUIEngineContainer::EExecuted )
                {
                aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdAbortTestCase, ETrue );
                }
            else if( startedTestCase->UIEngineContainer().State() == CUIEngineContainer::EFinished )
                {
                aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
                aMenuPane->SetItemDimmed( ECmdAbortTestCase, ETrue );
                }
                                                                                     
            }
		else
            {
            aMenuPane->SetItemDimmed( ECmdPauseTestCase, ETrue );
            aMenuPane->SetItemDimmed( ECmdResumeTestCase, ETrue );
            aMenuPane->SetItemDimmed( ECmdAbortTestCase, ETrue );
            }
        }
        
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputView::OutputUpdate
// 
// Receives output update notification from AppUI.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputView::OutputUpdateL( CStartedTestCase* /*aTestCase*/ )
    {
    PrintTestCaseStateL();
    iContainer->OutputUpdateL();

    }

// ----------------------------------------------------------------------------
// CTestCaseOutputView::TestCaseStateChangedL
// 
// Receives test case state changed notification from AppUI.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputView::TestCaseStateChangedL()
    {
    PrintTestCaseStateL();
    iContainer->OutputUpdateL();

    }

// ----------------------------------------------------------------------------
// CTestCaseOutputView::PrintTestCaseState
// 
// Prints test case state to title pane.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputView::PrintTestCaseStateL()
    {
    TBuf<50> statusMessage;
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* tp=(CAknTitlePane*)sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle));

    if ( iCurrentTestCase != NULL )
        {
        TUint status = iCurrentTestCase->Status();

        if( status & CUIStoreIf::EStatusRunning )
            {
            CEikonEnv::Static()->ReadResource( statusMessage,
                R_TESTCASE_STATE_RUNNING );
            }
        else if( status & CUIStoreIf::EStatusExecuted && 
                            status & CUIStoreIf::EStatusPassed )
            {
            CEikonEnv::Static()->ReadResource( statusMessage, 
                R_TESTCASE_STATE_PASSED );
            }
        else if( status & CUIStoreIf::EStatusExecuted && 
                            status & CUIStoreIf::EStatusFailed )
            {
            CEikonEnv::Static()->ReadResource( statusMessage, 
                R_TESTCASE_STATE_FAILED );
            }
        else if( status & CUIStoreIf::EStatusAborted )
            {
            CEikonEnv::Static()->ReadResource( statusMessage, 
                R_TESTCASE_STATE_CRASHED_ABORTED );
            }
        else if( status & CUIStoreIf::EStatusCrashed )
            {
            CEikonEnv::Static()->ReadResource( statusMessage, 
                R_TESTCASE_STATE_CRASHED_ABORTED );
            }
        else
            {
            CEikonEnv::Static()->ReadResource( statusMessage, 
                R_TESTCASE_STATE_UNKNOWN );
            }
        
        tp->SetTextL( statusMessage );
        tp->DrawNow();

        }

    }

// End of File
