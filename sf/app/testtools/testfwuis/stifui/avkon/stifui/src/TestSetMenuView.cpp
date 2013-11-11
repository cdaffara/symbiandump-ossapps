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
* Description: This file contains CTestSetMenuView class ddefinition.
*
*/

// INCLUDE FILES
#include  <eikmenub.h>
#include  <aknViewAppUi.h>
#include  <aknlistquerydialog.h> 
#include  <aknnotewrappers.h>
#include  <barsread.h>              //TResourceReader
#include  <Stifui.rsg>
#include  "TestSetMenuView.h"
#include  "TestSetMenuContainer.h"
#include  "AppUIAppUi.h"
#include  "Stifui.hrh" 



// ================= MEMBER FUNCTIONS =========================================
// ----------------------------------------------------------------------------
// CTestSetMenuView::ConstructL
// 
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CTestSetMenuView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL( R_APPUI_TESTSETMENUVIEW );
    iCurrentTestCase = 0;
    iSaveNeeded = ETrue;
    iUIStore = ( (CAppUIAppUi*)iCoeEnv->AppUi() )->UIStoreHandler();
    }

// ----------------------------------------------------------------------------
// CTestSetMenuView::~CTestSetMenuView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestSetMenuView::~CTestSetMenuView()
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
// CTestSetMenuView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CTestSetMenuView::Id() const
    {
    return TUid::Uid(ETestSetMenuViewId);
    }

// ----------------------------------------------------------------------------
// CTestSetMenuView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CTestSetMenuView::HandleCommandL(TInt aCommand)
    {   
    if ( aCommand == EAknCmdMark || aCommand == EAknCmdUnmark 
        || aCommand == EAknMarkAll || aCommand == EAknUnmarkAll )
        {
        iContainer->HandleMarkCommandL( aCommand );
        }
    else
        {
        switch ( aCommand )
            {
            case ECmdStartTestSet:
                {
                const CTestSetInfo& testSetInfo = 
                    iUIStore->TestSetL( iCurrentTestSet );
                const RRefArray<const CTestInfo>* allCases = 
                    &testSetInfo.TestCases();
                
                TInt testCaseCount = allCases->Count();
                if (testCaseCount > 0 )
                    {
                    StartTestSetL();
                    }
                break;
                }
            case ECmdShowStartedTestSet:
                {
                AppUi()->HandleCommandL(ECmdShowStartedTestSet);
                break;
                }
            case ECmdSaveTestSet:
                {
                SaveCurrentTestSetL();
                break;
                }
            case ECmdInsertTestCases:
                {
                // iSaveNeeded is set from 
                // CTestSetInsertMenuView::ShowInsertCasesDialog()
                //iSaveNeeded = ETrue;
                AppUi()->HandleCommandL(ECmdInsertTestCases);
                break;
                }
            case ECmdRemoveTestCases:
                {
                RemoveSelectedTestCasesL();
                break;
                }
            case EAknSoftkeyBack:
                {
                //iCurrentTestCase = 0;
               // AppUi()->HandleCommandL(ECmdLoadTestSet/*EAppUIGoToTestSetsMenu*/);
                //( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(EAppUIGoToTestSetsMenu);
                SaveCurrentTestSetL();
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
// CTestSetMenuView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CTestSetMenuView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ----------------------------------------------------------------------------
// CTestSetMenuView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CTestSetMenuView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        ((CAppUIAppUi*)AppUi())->iLogger->Log( 
            _L("TestSetMenuView: DoActivateL") );
        iContainer = new (ELeave) CTestSetMenuContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect(), this );
        ((CAppUIAppUi*)AppUi())->iLogger->Log( 
            _L("TestSetMenuView: container constructed") );
        AppUi()->AddToStackL( *this, iContainer );
        }
   
    //testing
    iContainer->SetCurrentItemIndex(iCurrentTestCase);
    
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* np = 
        (CAknNavigationControlContainer *)sp->ControlL(
        TUid::Uid(EEikStatusPaneUidNavi));
    
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_TESTSET );
    iNaviDecorator = np->CreateNavigationLabelL( reader );
    CleanupStack::PopAndDestroy(); // resource reader
    np->PushL(*iNaviDecorator);

    }

// ----------------------------------------------------------------------------
// CTestSetMenuView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CTestSetMenuView::DoDeactivate()
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
// CTestSetMenuView::DynInitMenuPaneL
// 
// Initializes menu pane.
// ----------------------------------------------------------------------------
//
void CTestSetMenuView::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
    {

    // Test modules are added to filter by test module submenu 
    // if the submenu is opened
    if (R_APPUI_FILTERBYMODULES_MENU == aResourceId)
        {
        RRefArray<TDesC> modules;
        TInt ret = iUIStore->Modules( modules );
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
            item.iText = modules[i];
            aMenuPane->AddMenuItemL(item);
            item.iCommandId++; // Command IDs 0x1000 - 0x1FFF are reserved
                               // for modules in hrh file.
            }
        
        modules.Reset();
        modules.Close();
        }

    // Test case files are added to filter by test case file submenu
    // if the submenu is opened.
    if (R_APPUI_FILTER_BY_TESTCASEFILE_MENU == aResourceId)
        {
        RRefArray<TDesC> testCaseFiles;
        TInt ret = iUIStore->TestCaseFiles( testCaseFiles );
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
            item.iCommandId++; // Command IDs 0x2000 - 0x2FFF are reserved for
                               // test case files in hrh file.
            }
        }
        
    }

// ----------------------------------------------------------------------------
// CTestSetMenuView::HandleListBoxEventL
// 
// Handles listbox events.
// ----------------------------------------------------------------------------
//
void CTestSetMenuView::HandleListBoxEventL(CEikListBox* /*aListBox*/, 
    TListBoxEvent aEventType)
    {

    if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
        {
        const CTestSetInfo& testSetInfo = 
            iUIStore->TestSetL( iCurrentTestSet );
        const RRefArray<const CTestInfo>* allCases = &testSetInfo.TestCases();

        TInt testCaseCount = allCases->Count();
           if (testCaseCount > 0 )
            {
            StartTestSetL();
            }
        }
   }
 
// ----------------------------------------------------------------------------
// CTestSetMenuView::CreateTestSet
// 
// Creates new test set.
// ----------------------------------------------------------------------------
//
TInt CTestSetMenuView::CreateTestSetL( const TDesC& aTestSetName )
    {
    TInt error = 0;
    TBuf<100> message;
    
    _LIT(KPath, "c:\\TestFramework\\");
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    TEntry entry;
    // we check if the c:\testframework directory exists
    // It is mandatory for this dir to exist if we want to save a test set.
    // This dir must be localised on the C drive of the device
    if(fs.Entry(KPath, entry) != KErrNone)
    	{	// if the dir does not exist 
    	TInt err = fs.MkDirAll(KPath);	// we create it
    	if(err != KErrNone)
    		{	// if of any reason it was impossible to create the dir - inform user about it
	    	CAknInformationNote* note = new (ELeave) CAknInformationNote(ETrue);
	    	note->ExecuteLD( _L("Could not create c:\\TestFramework dir!") );
    		}
    	}
    CleanupStack::PopAndDestroy(&fs);	// close and remove RFs object 
    
    error = iUIStore->LoadTestSet( aTestSetName );
    
    // If testset is either active or already created and saved.
 /*   if ( KErrNone == error)// || KErrAlreadyExists == error )
        {
        CEikonEnv::Static()->ReadResource( message, 
            R_OVERWRITE_TESTSET_QUESTION );
        CAknQueryDialog * dlg = 
            CAknQueryDialog::NewL(CAknQueryDialog::ENoTone);
        // Confirm overwrite
        if ( dlg->ExecuteLD( R_GENERAL_CONFIRMATION_DIALOG, message ) )
            {
            error = iUIStore->RemoveTestSet( aTestSetName );
            error = iUIStore->CreateTestSet( aTestSetName );
            if ( error == KErrNone )
                {
                iCurrentTestSet = aTestSetName;
                iSaveNeeded = ETrue;
                }
            }
        // Else load saved default test set
        else
            {
            iSaveNeeded = EFalse;
            iCurrentTestSet = aTestSetName;
            error = KErrAlreadyExists;
            }
        }
    else if ( KErrNotFound == error )
        {
        error = iUIStore->CreateTestSet( aTestSetName );
        if ( KErrNone == error )
            {
            iCurrentTestSet = aTestSetName;
            iSaveNeeded = ETrue;
            }
        }
    else if ( KErrPathNotFound == error )
          {
          error = iUIStore->CreateTestSet( aTestSetName );
          iCurrentTestSet = aTestSetName;
          iSaveNeeded = EFalse;
          }
    else
        {
        ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("Test set creation fails with error: %d"), error );
        CAknInformationNote* informationNote = new (ELeave) CAknInformationNote(ETrue);
        informationNote->ExecuteLD( _L("UNDEFINED ERROR!") );
        }*/
    
    error = iUIStore->CreateTestSet( aTestSetName );
    iCurrentTestSet = aTestSetName;
    iSaveNeeded = ETrue;
    return error;
    }

// ----------------------------------------------------------------------------
// CTestSetMenuView::LoadTestSetL
// 
// Loads saved test set.
// ----------------------------------------------------------------------------
//
TInt CTestSetMenuView::LoadTestSetL( const TDesC& aTestSetName )
    {
    TInt error = 0;
    TBuf<100> message;
    error = iUIStore->LoadTestSet( aTestSetName );
    if ( KErrNone == error || KErrAlreadyExists == error )
        {
        iCurrentTestSet = aTestSetName;
        iSaveNeeded = EFalse;
        }
    else
        {
        CEikonEnv::Static()->ReadResource( message, 
            R_LOAD_TESTSET_FAILED );
        
        CAknInformationNote* informationNote = new (ELeave) CAknInformationNote(ETrue);
        informationNote->ExecuteLD(message);
        }
    
    return error;
    }

// ----------------------------------------------------------------------------
// CTestSetMenuView::SaveCurrentTestSet
// 
// Saves current test set.
// ----------------------------------------------------------------------------
//
TInt CTestSetMenuView::SaveCurrentTestSetL()
    {
    TInt error = 0;
    TBuf<100> message;
    
    CEikonEnv::Static()->ReadResource( message, 
        R_SAVE_TESTSET_QUESTION );
        
    CAknQueryDialog * dlg = 
        CAknQueryDialog::NewL(CAknQueryDialog::ENoTone);
    
   if ( dlg->ExecuteLD( R_GENERAL_CONFIRMATION_DIALOG, message ) )
       {
        error = iUIStore->SaveTestSet2( iCurrentTestSet );
        iSaveNeeded = EFalse;
        
      }
   AppUi()->HandleCommandL(EAppUIGoBack);

    return error;
    }

// ----------------------------------------------------------------------------
// CTestSetMenuView::RemoveActiveTestSet
// 
// Removes current test set.
// ----------------------------------------------------------------------------
//
TInt CTestSetMenuView::RemoveActiveTestSet()
    {
    TInt error = 0;
    
    error = iUIStore->RemoveTestSet( iCurrentTestSet );
    
    return error;
    }

// ----------------------------------------------------------------------------
// CTestSetMenuView::StartTestSetL
// 
// Shows confirmation dialog and verify if user really want to start test set.
// ----------------------------------------------------------------------------
//
void CTestSetMenuView::StartTestSetL()
    {
    _LIT( KErrorStartingTestSet, 
        "TestSetMenuView: StartTestSetL() fails (%d)" );
    
    TInt ret(KErrNone);
    TInt index(0);
    TInt selectedItem(0);
    
    // Default mode is parallel.
    CStartedTestSet::TSetType mode = CStartedTestSet::ESetParallel;
    
    CAknListQueryDialog* startDialog = 
        new (ELeave) CAknListQueryDialog(&selectedItem);
    
    // Show confirmation dialog.
    if ( startDialog->ExecuteLD(R_START_MULTIPLE_TESTCASES_LIST_QUERY) )
        {
        index = ((CAppUIAppUi*)AppUi())->iStartedTestSet;
        // Check mode
        if ( 1 == selectedItem )
            {
            mode = CStartedTestSet::ESetSequential;
            }
        
        // Start test set cases.
        ret = iUIStore->StartTestSet( 
            iUIStore->TestSetL( iCurrentTestSet ), index, mode );
            
        if ( ret != KErrNone )
            {
            ((CAppUIAppUi*)AppUi())->iStartedTestSet = -1;
            ((CAppUIAppUi*)AppUi())->iLogger->Log( 
                KErrorStartingTestSet, ret );
            }
        else
            {
            ((CAppUIAppUi*)AppUi())->iStartedTestSet = index;
            }
        }
        
    }
   
// ----------------------------------------------------------------------------
// CTestSetMenuView::RemoveSelectedTestCasesL
// 
// Removes marked test cases from test set.
// ----------------------------------------------------------------------------
//
void CTestSetMenuView::RemoveSelectedTestCasesL()
    {
    TInt positionToRemove(0);
    
    const CTestSetInfo& testSetInfo = iUIStore->TestSetL( iCurrentTestSet );
    const RRefArray<const CTestInfo>* allCases = &testSetInfo.TestCases();
    CEikListBox* listBox = iContainer->ListBox();

    iSaveNeeded = ETrue;
    positionToRemove = listBox->CurrentItemIndex();
    // Remove selected test case from test set.
    TInt ret = iUIStore->RemoveFromTestSet(iCurrentTestSet, allCases->operator[](positionToRemove));
    iContainer->RemoveListBoxItemL(positionToRemove);
    
    if( ret != KErrNone )
        {
        RDebug::Print( _L("RemoveFromTestSet failed with value: %d"), ret );
        }
    }

// End of File
