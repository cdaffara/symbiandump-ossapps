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
* Description: This file contains CStatisticsView class 
* definition.
*
*/

// INCLUDE FILES
#include  <eikmenub.h>
#include  <aknViewAppUi.h>
#include  <aknlistquerydialog.h> 
#include  <barsread.h>	//TResourceReader
#include  <Stifui.rsg>
#include  "AppUIAppUi.h"
#include  "Stifui.hrh" 
#include  "StatisticsView.h"
#include  "StatisticsContainer.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CStatisticsView::ConstructL
// 
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CStatisticsView::ConstructL()
    {
    CView::ConstructL();
    BaseConstructL( R_APPUI_STATISTICSVIEW );
    iCurrentTestCase = 0;
    }

// ----------------------------------------------------------------------------
// CStatisticsView::~CStatisticsView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CStatisticsView::~CStatisticsView()
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
// CStatisticsView::Id
// 
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CStatisticsView::Id() const
    {
    return TUid::Uid(EStatisticsViewId);
    }

// ----------------------------------------------------------------------------
// CStatisticsView::HandleCommandL
// 
// Handles a command.
// ----------------------------------------------------------------------------
//
void CStatisticsView::HandleCommandL(TInt aCommand)
    {   
    if ( aCommand >= ECmdFilterByModule && aCommand < ECmdFilterByTestCaseFile ) // 0x1000 - 0x1FFF
        {
        TInt moduleNumber = aCommand - ECmdFilterByModule;

        RRefArray<TDesC> moduleName;
        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->Modules( moduleName );
        if( KErrNone != ret )
            {
            moduleName.Reset();
            moduleName.Close();
            User::Leave( ret );
            }
        //iFilterModule = testModules[moduleNumber];

        iContainer->FilterCasesByModuleL( moduleName[moduleNumber] );

        moduleName.Reset();
        moduleName.Close();

        return;
        }

    if ( aCommand >= ECmdFilterByTestCaseFile && aCommand < ECmdNOFiltering )
        {
        TInt testCaseFileNumber = aCommand - ECmdFilterByTestCaseFile;
        RRefArray<TDesC> testCaseFileName;
        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->TestCaseFiles( testCaseFileName );
        if( KErrNone != ret )
            {
            testCaseFileName.Reset();
            testCaseFileName.Close();
            User::Leave( ret );
            }

        iContainer->FilterCasesByTCFileNameL( testCaseFileName[testCaseFileNumber] );

        testCaseFileName.Reset();
        testCaseFileName.Close();

        return;
        }

    if ( aCommand == ECmdNOFiltering )
        {
        iContainer->NoFilteringL();
        return;
        }

    switch ( aCommand )
        {
        case ECmdStartCases:
            {
            iCurrentTestCase = iContainer->CurrentItemIndex();
            StartTestCasesL();
            break;
            }
        case EAknSoftkeyBack:
            {
            AppUi()->HandleCommandL( EAppUIGoBack );
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
// CStatisticsView::HandleClientRectChange
// 
// Handles client rect changes.
// ----------------------------------------------------------------------------
//
void CStatisticsView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ----------------------------------------------------------------------------
// CStatisticsView::DoActivateL
// 
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CStatisticsView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
           ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("StatisticsView: DoActivateL"));
        //message = _L("eng.open ret:");
        //message.AppendNum( ret , EDecimal );
        //AppUi()->iLogger->Log( message );
        iContainer = new (ELeave) CStatisticsContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect(), this );
           ((CAppUIAppUi*)AppUi())->iLogger->Log(_L("StatisticsView: container constructed"));
        AppUi()->AddToStackL( *this, iContainer );
        }
    iContainer->SetCurrentItemIndex(iCurrentTestCase);
	
	CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
	CAknNavigationControlContainer* np= (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_NAVITITLE_STARTED_STATS);
	iNaviDecorator = np->CreateNavigationLabelL( reader );
	CleanupStack::PopAndDestroy(); // resource reader
	np->PushL(*iNaviDecorator);
    }

// ----------------------------------------------------------------------------
// CStatisticsView::DoDeactivate
// 
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CStatisticsView::DoDeactivate()
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
// CStatisticsView::DynInitMenuPaneL
// 
// Initializes menu pane.
// ----------------------------------------------------------------------------
//
void CStatisticsView::DynInitMenuPaneL(
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

        testCaseFiles.Reset();
        testCaseFiles.Close();
        }

    }

// ----------------------------------------------------------------------------
// CStatisticsView::HandleListBoxEventL
// 
// Handles a list box event.
// ----------------------------------------------------------------------------
//
void CStatisticsView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
	{
	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{
        //iCurrentTestCase = aListBox->CurrentItemIndex();
        //iCurrentTestCase = iContainer->CurrentItemIndex();
        //StartTestCases();
        }
    }

// ----------------------------------------------------------------------------
// CStatisticsView::StartTestCases
// 
// Starts test case(s) which is selected in containers list box.
// First shows a list query if user wants just start test case or
// if he wants to start test case and view test case output.
// ----------------------------------------------------------------------------
//
void CStatisticsView::StartTestCasesL()
    {
    TInt selectedItem(0);
    CAknListQueryDialog* startDialog = new (ELeave) CAknListQueryDialog(&selectedItem);
    if ( startDialog->ExecuteLD(R_START_TESTCASE_LIST_QUERY) )
        {
        RRefArray<CTestInfo> testInfo;
        TInt ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->TestCases( testInfo );
        if( KErrNone != ret )
            {
            testInfo.Reset();
            testInfo.Close();
            User::Leave( ret );
            }
        TInt testCaseNumber = testInfo[0].TestCaseNum();
        TInt testCaseIndex( 0 );

        //CUIEngineContainer* container = NULL;
        ret = ((CAppUIAppUi*)AppUi())->UIStoreHandler()->StartTestCase( testInfo[testCaseNumber], testCaseIndex );
        if( KErrNone != ret )
            {
            User::Leave( ret );
            }
        testInfo.Reset();
        testInfo.Close();

        // Increment the counter value
        ((CAppUIAppUi*)AppUi())->iUIStoreHandler->iExecutedTestCaseCount++;

        }

    }

// End of File
