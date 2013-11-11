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
#include  "StartCasesView.h"
#include  "StartCasesContainer.h"
#include  "AppUIAppUi.h"
#include  "Stifui.hrh" 


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CStartCasesView::ConstructL
// 
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CStartCasesView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL( R_APPUI_STARTCASESVIEW );
    iCurrentTestCase = 0;

    }

// ----------------------------------------------------------------------------
// CStartCasesView::~CStartCasesView
// 
// Destructor
// ----------------------------------------------------------------------------
//
CStartCasesView::~CStartCasesView()
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
// CStartCasesView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CStartCasesView::Id() const
    {
    return TUid::Uid(EStartCaseMenuViewId);

    }

// ----------------------------------------------------------------------------
// CStartCasesView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CStartCasesView::HandleCommandL(TInt aCommand)
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
	        case ECmdStartCases:
	            {
	            CheckListBoxSelectionsL( iContainer->ListBox() );
	            break;
	            }
	        case EAknSoftkeyBack:
	            {
	            iCurrentTestCase = 0;
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

    }

// ----------------------------------------------------------------------------
// CStartCasesView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CStartCasesView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }

    }

// ----------------------------------------------------------------------------
// CStartCasesView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CStartCasesView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("StartCasesView: DoActivateL"));
        iContainer = new (ELeave) CStartCasesContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect(), this );
           ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("StartCasesView: container constructed"));
        AppUi()->AddToStackL( *this, iContainer );
        }

    iContainer->SetCurrentItemIndex(iCurrentTestCase);
    
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_STARTCASES);
	iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	np->PushL(*iNaviDecorator);

    }

// ----------------------------------------------------------------------------
// CStartCasesView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CStartCasesView::DoDeactivate()
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
// CStartCasesView::DynInitMenuPaneL
// 
// Initializes menu pane.
// ----------------------------------------------------------------------------
//
void CStartCasesView::DynInitMenuPaneL(
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
            item.iText.Copy( modules[i].Left( item.iText.MaxLength() ) ) ;
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
            item.iText.Copy( testCaseFiles[i].Left( item.iText.MaxLength() ) );
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
// CStartCasesView::HandleListBoxEventL
// 
// Handles listbox events.
// ----------------------------------------------------------------------------
//
void CStartCasesView::HandleListBoxEventL( CEikListBox* aListBox, 
    TListBoxEvent aEventType )
	{
	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{
		CheckListBoxSelectionsL( aListBox );
        }
        
    }

// ----------------------------------------------------------------------------
// CStartCasesView::CheckListBoxSelectionsL
// 
// Checks listbox selections and launches query dialog to start test cases.
// ----------------------------------------------------------------------------
//
void CStartCasesView::CheckListBoxSelectionsL( CEikListBox* aListBox )
    {
    TInt i(0);
	TInt count = aListBox->SelectionIndexes()->Count();

	RArray<TInt> selectedIndices;
	CleanupClosePushL( selectedIndices );

	if ( count > 0 )
	{
		for( i = 0; i < count; i++ )
			{
			selectedIndices.InsertInOrderL( 
			    (*aListBox->SelectionIndexes())[i] );
			}
	}
	
	// Check count of selected items.
	if ( count == 1 )
		{
		iCurrentTestCase = selectedIndices[0];
       	StartTestCaseL();
		}
	else if (count > 1)
		{
       	StartTestCasesL( selectedIndices );	
		}
	else
		{
		iCurrentTestCase = iContainer->CurrentItemIndex();
       	StartTestCaseL();	
		}

    CleanupStack::PopAndDestroy();
    
    }
    
// ----------------------------------------------------------------------------
// CStartCasesView::StartTestCaseL
// 
// Starts test case which is selected in containers list box.
// First shows a list query if user wants just start test case or
// if he wants to start test case and view test case output.
// ----------------------------------------------------------------------------
//
void CStartCasesView::StartTestCaseL()
    {
    TInt selectedItem(0);
    CAknListQueryDialog* startDialog = new (ELeave) CAknListQueryDialog(&selectedItem);
    if ( startDialog->ExecuteLD(R_START_TESTCASE_LIST_QUERY) )
        {
        CTestInfo* testCaseInfo = iContainer->SelectedTestCaseInfo();

        User::LeaveIfNull( testCaseInfo );
        
        TInt testCaseIndex( 0 );

        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->StartTestCase( *testCaseInfo, testCaseIndex );
      
        if( KErrNone != ret )
            {
            User::Leave( ret );
            }

        // Increment the counter value
        ((CAppUIAppUi*)AppUi())->iUIStoreHandler->iExecutedTestCaseCount++;
     
        if ( selectedItem == 1 ) // if view output was selected
            {
            CStartedTestCase* startedCase = 
                &((CAppUIAppUi*)AppUi())->UIStoreHandler()->StartedTestCaseL( 
                testCaseIndex );
            ((CAppUIAppUi*)AppUi())->SetStartedTestCase( startedCase );
                
            AppUi()->HandleCommandL( ECmdViewOutput );

            }
        }
    }

// ----------------------------------------------------------------------------
// CStartCasesView::StartTestCasesL
// 
// Starts multiple test cases which are selected in containers list box.
// Shows a list query if user wants to start cases parallel or sequential.
// ----------------------------------------------------------------------------
//
void CStartCasesView::StartTestCasesL( RArray<TInt> aSelectedIndexes )
    {
    _LIT( KTempSet, "TempSet");
    TInt selectedItem(0);
    TInt i(0);
    TInt ret(0);
    
    CAknListQueryDialog* startDialog = new (ELeave) CAknListQueryDialog(&selectedItem);
    if ( startDialog->ExecuteLD(R_START_MULTIPLE_TESTCASES_LIST_QUERY) )
    	{
    	ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->CreateTestSet( KTempSet );
    	
        User::LeaveIfError( ret );
        
        for( i = 0; i < aSelectedIndexes.Count(); i++ )
        	{
            CTestInfo* testCaseInfo = iContainer->TestCaseInfo( aSelectedIndexes[i] );
            User::LeaveIfNull( testCaseInfo );
        
    		ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->AddToTestSet( KTempSet, *testCaseInfo );
    		if( KErrNone != ret )
    			{
    			User::Leave( ret );
    			}
    		// Increment the counter value
        	((CAppUIAppUi*)AppUi())->iUIStoreHandler->iExecutedTestCaseCount++;
	        }
	    
	    //start cases
	    if ( selectedItem == 0 )
	    	{
			ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->StartTestSet( 
		        ((CAppUIAppUi*)AppUi())->UIStoreHandler()->TestSetL( KTempSet ),
				i, CStartedTestSet::ESetParallel);
		    }
	    else if (selectedItem == 1)
		    {
	    	ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->StartTestSet(
	    	    ((CAppUIAppUi*)AppUi())->UIStoreHandler()->TestSetL( KTempSet ),
	    	    i, CStartedTestSet::ESetSequential);
	    	}

        ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->RemoveTestSet( KTempSet );
    	}
    }

// End of File
