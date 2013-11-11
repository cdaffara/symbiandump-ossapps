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
* Description: This file contains CTestSetInsertMenuView class
* definition.
*
*/

// INCLUDE FILES
#include  <eikmenub.h>
#include  <aknViewAppUi.h>
#include  <aknlistquerydialog.h> 
#include  <aknnotewrappers.h>
#include  <barsread.h>	//TResourceReader
#include  <Stifui.rsg>
#include  "TestSetInsertMenuView.h"
#include  "TestSetInsertMenuContainer.h"
#include  "TestSetMenuView.h"
#include  "AppUIAppUi.h"
#include  "Stifui.hrh" 


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestSetInsertMenuView::ConstructL
// 
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CTestSetInsertMenuView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL(  R_INSERT_TESTCASES_VIEW );
    iCurrentTestCase = 0;

    }

// ----------------------------------------------------------------------------
// CTestSetInsertMenuView::~CTestSetInsertMenuView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestSetInsertMenuView::~CTestSetInsertMenuView()
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
// CTestSetInsertMenuView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CTestSetInsertMenuView::Id() const
    {
    return TUid::Uid(ETestSetInsertMenuViewId);

    }

// ----------------------------------------------------------------------------
// CTestSetInsertMenuView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CTestSetInsertMenuView::HandleCommandL(TInt aCommand)
    {   
    if ( aCommand >= ECmdFilterByModule && aCommand < ECmdFilterByTestCaseFile ) // 0x1000 - 0x1FFF
        {
        TInt moduleNumber = aCommand - ECmdFilterByModule;
        RRefArray<TDesC> allModules;
        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->Modules( allModules );
        if( KErrNone != ret )
            {
            allModules.Reset();
            allModules.Close();
            User::Leave( ret );
            }
        TName moduleName = allModules[ moduleNumber ];

        iContainer->FilterCasesByModuleL( moduleName );

        allModules.Reset();
        allModules.Close();
        }
	else if ( aCommand >= ECmdFilterByTestCaseFile && aCommand < ECmdNOFiltering )
        {
        TInt testCaseFileNumber = aCommand - ECmdFilterByTestCaseFile;
        RRefArray<TDesC> allTestCases;
        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->TestCaseFiles( allTestCases );
        if( KErrNone != ret )
            {
            allTestCases.Reset();
            allTestCases.Close();
            User::Leave( ret );
            }
        TFileName testCaseFileName = allTestCases[ testCaseFileNumber ];

        iContainer->FilterCasesByTCFileNameL( testCaseFileName );

        allTestCases.Reset();
        allTestCases.Close();
        }
	else if ( aCommand == ECmdNOFiltering )
        {
        iContainer->NoFilteringL();
        }
    else if ( aCommand == EAknCmdMark || aCommand == EAknCmdUnmark 
    	|| aCommand == EAknMarkAll || aCommand == EAknUnmarkAll )
    	{
    	iContainer->HandleMarkCommandL( aCommand );
    	}
    else
	    {
 	    switch ( aCommand )
	        {
	        case ECmdInsertSelectedCases:
	            {
                ShowInsertCasesDialogL();
	            break;
	            }
	        case EAknSoftkeyCancel:
	            {
	            //iCurrentTestCase = 0;
	            ( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(EAppUIGoToTestSetsMenu);
	            break;
	            }
	        default:
	            {
	            AppUi()->HandleCommandL( aCommand );
	            break;
	            }
	        }
	    }

    }

// ----------------------------------------------------------------------------
// CTestSetInsertMenuView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CTestSetInsertMenuView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }

    }

// ----------------------------------------------------------------------------
// CTestSetInsertMenuView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CTestSetInsertMenuView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
           ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("TestSetInsertMenuView: DoActivateL"));
        //message = _L("eng.open ret:");
        //message.AppendNum( ret , EDecimal );
        //AppUi()->iLogger->Log( message );
        iContainer = new (ELeave) CTestSetInsertMenuContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect(), this );
           ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("TestSetInsertMenuView: container constructed"));
        AppUi()->AddToStackL( *this, iContainer );
        }
   
   //testing
    iContainer->SetCurrentItemIndex(iCurrentTestCase);
	
	CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_TESTSET_INSERT);
	iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	np->PushL(*iNaviDecorator);
    }

// ----------------------------------------------------------------------------
// CTestSetInsertMenuView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CTestSetInsertMenuView::DoDeactivate()
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
// CTestSetInsertMenuView::DynInitMenuPaneL
// 
// Initializes menu pane.
// ----------------------------------------------------------------------------
//
void CTestSetInsertMenuView::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
    {

    // Test modules are added to filter by test module submenu if the submenu is opened
    if (R_APPUI_FILTERBYMODULES_MENU == aResourceId)
        {
        RRefArray<TDesC> modules;
        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->Modules( modules );
        if( KErrNone != ret )
            {
            modules.Reset();
            modules.Close();
            User::Leave( ret );
            }

        TInt moduleCount = modules.Count();
        
        TInt i;
        CEikMenuPaneItem::SData item;

        item.iCommandId = ECmdFilterByModule;
        item.iFlags = 0;
        item.iCascadeId = 0;

        for (i = 0; i < moduleCount; i++)
            {
            //item.iText = modules[i].iModuleName;
            item.iText = modules[i];
            aMenuPane->AddMenuItemL(item);
            item.iCommandId++; // command IDs 0x1000 - 0x1FFF are reserved for modules in hrh file
            }
        
        modules.Reset();
        modules.Close();

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
            aMenuPane->AddMenuItemL(item);
            item.iCommandId++; // command IDs 0x2000 - 0x2FFF are reserved for test case files in hrh file
            }
        }

    }

// ----------------------------------------------------------------------------
// CTestSetInsertMenuView::HandleListBoxEventL
// 
// Handles listbox events.
// ----------------------------------------------------------------------------
//
void CTestSetInsertMenuView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
	{

	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{
        ShowInsertCasesDialogL();
		}
		
	}

// ----------------------------------------------------------------------------
// CTestSetInsertMenuView::ShowInsertCasesDialogL
// 
// Show confirmation dialog for inserting test cases.
// ----------------------------------------------------------------------------
//
void CTestSetInsertMenuView::ShowInsertCasesDialogL()
    {
    TInt i(0);
    TBuf<50> message;
    CEikListBox* listBox = iContainer->ListBox();
	TInt count = listBox->SelectionIndexes()->Count();
	
	if ( count > 0 )
	    {
    	CEikonEnv::Static()->ReadResource( message, R_INSERT_CONFIRMATION_QUESTION );
    		
    	CAknQueryDialog * dlg = CAknQueryDialog::NewL(CAknQueryDialog::ENoTone);
    	if ( dlg->ExecuteLD( R_GENERAL_CONFIRMATION_DIALOG, message ) )
    		{
    		CEikListBox* newListBox = iContainer->ListBox();
		    TInt newCount = newListBox->SelectionIndexes()->Count();
    		RArray<TInt> selectedIndices;
		    CleanupClosePushL( selectedIndices );
		    if ( newCount > 0 )
    			{
    			for( i = 0; i < newCount; i++ )
	    			{
    				selectedIndices.InsertInOrderL( (*newListBox->SelectionIndexes())[i] );
		    		}	
    				
	    		AddCasesToTestSet( selectedIndices );
	    		// Test cases added, set save needed flag to true.
		    	((CTestSetMenuView*)AppUi()->View( 
		    	    TUid::Uid(ETestSetMenuViewId) ))->SetSaveNeeded(ETrue);
			    }
    		CleanupStack::PopAndDestroy();
	    	
		    // Test cases added -> go to test set menu
    		AppUi()->HandleCommandL(EAppUIGoBack);
	    	}
	    }
	else
	    {
	    CAknWarningNote* dialog = new(ELeave)CAknWarningNote();
	    CEikonEnv::Static()->ReadResource( message, R_INSERT_NOCASES_TEXT );
	    dialog->ExecuteLD(message);
	    }
	    
    }

// ----------------------------------------------------------------------------
// CTestSetInsertMenuView::AddCasesToTestSet
// 
// Adds selected test cases to the current test set.
// ----------------------------------------------------------------------------
//
void CTestSetInsertMenuView::AddCasesToTestSet( RArray<TInt> aSelectedIndexes )
	{
	TInt ret(0);
	TInt i(0);
	RRefArray<CTestInfo> allCases;
	CTestSetMenuView* testSetMenuView = 
    	(CTestSetMenuView*)((CAppUIAppUi*)AppUi())->View( TUid::Uid(ETestSetMenuViewId) );

	ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->TestCases(allCases);
    
	for ( i = 0; i < aSelectedIndexes.Count(); i++ )
		{
		ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->AddToTestSet( 
    		testSetMenuView->CurrentTestSet(),
   			allCases[ aSelectedIndexes[i] ] );
		}    
    if( ret != KErrNone )		
        {
        RDebug::Print( _L("AddCasesToTestSet failed with value: %d"), ret );
        }
    
	allCases.Reset();
	allCases.Close();

	}

// End of File
