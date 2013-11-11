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
* Description: This file contains TMessageBoxUtil class definition.
*
*/

// INCLUDE FILES
#include "AppUIAppUi.h"

#include "MainMenuView.h"
#include "TestCaseMenuView.h"
#include "StartCasesView.h"
#include "TestModulesMenuView.h"
#include "StartedCasesMenuView.h"
#include "ShowStartedCasesView.h"
#include "TestCaseOutputView.h"
#include "StatisticsView.h"
#include "TestSetBaseMenuView.h"
#include "TestSetMenuView.h"
#include "TestSetInsertMenuView.h"
#include "TestSetStartedCasesView.h"
#include <aknmessagequerydialog.h> 
#include "version.h"

#include "CreatedTestSetMenuView.h"

#include <Stifui.rsg>
#include "Stifui.hrh"


#include <StifTestInterface.h>

#include <aknnotewrappers.h>
#include <StringLoader.h>

_LIT( KTestSet, "DefaultSet" );

static const TInt KCopyrightChar = 169;

// ----------------------------------------------------------
// CMessageBoxUtil::ShowNotImplementedYetL
// Displays message that executed funtion is not implemented yet.
// ----------------------------------------------------------
//
void TMessageBoxUtil::ShowNotImplementedYetL()
    {
    _LIT(message, "Not implemented yet");
    CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
    informationNote->ExecuteLD(message);
    }

// ----------------------------------------------------------
// Display error note.
// @param aMessage Error message to display.
// ----------------------------------------------------------
//
void TMessageBoxUtil::ShowErrorNoteL( const TDesC& aMessage )
	{
	CAknErrorNote* errorNote = new (ELeave) CAknErrorNote(ETrue);
	// set timeout to 5 sec	
	errorNote->SetTimeout( (CAknNoteDialog::TTimeout)5000000 );
	errorNote->ExecuteLD( aMessage );
	}

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CAppUIAppUi::ConstructL()
// ?implementation_description
// ----------------------------------------------------------
//
void CAppUIAppUi::ConstructL()
    {
    TInt error(0);

    BaseConstructL( EAknEnableSkin );

    // connection to setting server needs to be open as long as STIF Series 60 UI APP is alive
    TInt ret = iSettingServer.Connect();
    if ( ret != KErrNone )
        {
        User::Leave( ret );
        }
    
    iLogger = CStifLogger::NewL( _L("E\x3a\\"), _L("stif_tfw_ui"),
                                CStifLogger::ETxt, CStifLogger::EFile,
                                ETrue, EFalse, ETrue, EFalse, EFalse );

    iLogger->Log( _L("--- UI log starts ---") );
    iLogger->Log( _L(" ") );
    iLogger->Log( _L("appui: ConstructL") );

	// Create CData object which handles all test cases data and running of them
    iUIStoreHandler = CUIStoreHandler::NewL( this );
    
    // CUIStore open 
    error = UIStoreHandler()->Open( KDefaultPathAndIni );
    if ( KErrNone != error )
    	{
    	User::Leave( error );
    	}

    CMainMenuView* mainMenuView = new (ELeave) CMainMenuView;
    CleanupStack::PushL( mainMenuView );
    mainMenuView->ConstructL();
    AddViewL( mainMenuView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // MainMenuView

    iLogger->Log(_L("appui:mainview "));

    CCreatedTestSetMenuView* testCreatedTestSetMenuView = new (ELeave) CCreatedTestSetMenuView;
        CleanupStack::PushL( testCreatedTestSetMenuView );
       testCreatedTestSetMenuView->ConstructL();
        AddViewL( testCreatedTestSetMenuView );      // transfer ownership to CAknViewAppUi
        CleanupStack::Pop();    // testCaseMenuView
    CTestCaseMenuView* testCaseMenuView = new (ELeave) CTestCaseMenuView;
    CleanupStack::PushL( testCaseMenuView );
    testCaseMenuView->ConstructL();
    AddViewL( testCaseMenuView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // testCaseMenuView

    iLogger->Log(_L("appui:caseview "));

    CStartCasesView* startCasesView = new (ELeave) CStartCasesView;
    CleanupStack::PushL( startCasesView );
    startCasesView->ConstructL();
    AddViewL( startCasesView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // startCasesView

    iLogger->Log(_L("appui:startcaseview "));

    CStartedCasesMenuView* startedCasesMenuView = new (ELeave) CStartedCasesMenuView;
    CleanupStack::PushL( startedCasesMenuView );
    startedCasesMenuView->ConstructL();
    AddViewL( startedCasesMenuView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // startedCasesMenuView

    iLogger->Log(_L("appui:started cases view created"));

    CShowStartedCasesView* showStartedCasesView = new (ELeave) CShowStartedCasesView;
    CleanupStack::PushL( showStartedCasesView );
    showStartedCasesView->ConstructL();
    AddViewL( showStartedCasesView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // showStartedCasesView

    iLogger->Log(_L("appui: show started cases view created"));

    CTestCaseOutputView* testCaseOutputView = new (ELeave) CTestCaseOutputView;
    CleanupStack::PushL( testCaseOutputView );
    testCaseOutputView->ConstructL();
    AddViewL( testCaseOutputView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // testCaseOutputView

    iLogger->Log(_L("appui: test case output view created"));

    CTestModulesMenuView* testModulesMenuView = new (ELeave) CTestModulesMenuView;
    CleanupStack::PushL( testModulesMenuView );
    testModulesMenuView->ConstructL();
    AddViewL( testModulesMenuView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // testModulesMenuView

    iLogger->Log(_L("appui:modulesview "));

    CStatisticsView* statisticsMenuView = new (ELeave) CStatisticsView;
    CleanupStack::PushL( statisticsMenuView );
    statisticsMenuView->ConstructL();
    AddViewL( statisticsMenuView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // statisticsMenuView

    iLogger->Log(_L("appui:modulesview "));

	//
	CTestSetBaseMenuView* testSetBaseMenuView = new (ELeave) CTestSetBaseMenuView;
    CleanupStack::PushL( testSetBaseMenuView );
    testSetBaseMenuView->ConstructL();
    AddViewL( testSetBaseMenuView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // testSetBaseMenuView

    iLogger->Log(_L("appui:testsetsbaseview "));
    
    CTestSetMenuView* testSetMenuView = new (ELeave) CTestSetMenuView;
    CleanupStack::PushL( testSetMenuView );
    testSetMenuView->ConstructL();
    AddViewL( testSetMenuView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // testSetMenuView

    iLogger->Log(_L("appui:testsetsview "));
	
	CTestSetInsertMenuView* testSetInsertMenuView = new (ELeave) CTestSetInsertMenuView;
    CleanupStack::PushL( testSetInsertMenuView );
    testSetInsertMenuView->ConstructL();
    AddViewL( testSetInsertMenuView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // testSetInsertMenuView
   	
   	iLogger->Log(_L("appui:testsetinsertview "));
   	
   	CTestSetStartedCasesView* testSetStartedCasesView = new (ELeave) CTestSetStartedCasesView;
    CleanupStack::PushL( testSetStartedCasesView );
    testSetStartedCasesView->ConstructL();
    AddViewL( testSetStartedCasesView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // testSetStartedCasesView

    iLogger->Log(_L("appui:testsetStartedCasesView "));
    

    SetDefaultViewL( *mainMenuView );

    iLogger->Log(_L("appui:setdefview "));
    
    iStartedTestCase = NULL;
    iStartedTestSet = -1;
    
    iPreviousPositionListValid = ETrue;
    }

// ----------------------------------------------------
// CAppUIAppUi::~CAppUIAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CAppUIAppUi::~CAppUIAppUi()
    {
    
    if ( iLogger )
    {
    iLogger->Log( _L(" ") );
    iLogger->Log( _L("--- UI log ends ---") );    	
    }

    delete iLogger;
    iSettingServer.Close();
    UIStoreHandler()->Close();
    delete iUIStoreHandler;
    
    // Closing of array of positions of focus in various menus
    iPreviousFocusPosition.Close();
    // iPreviousFocusPosition is no longer valid and cannot be used any more
    iPreviousPositionListValid = EFalse;
    //RemoveView(EMainMenuViewId);
    }

// ------------------------------------------------------------------------------
// CAppUIAppUi::Data( )
// Just returns pointer to CUIStore object, which is created by CAppUIAppUi
// ------------------------------------------------------------------------------
//
CUIStore* CAppUIAppUi::UIStoreHandler()
    {
    return &iUIStoreHandler->UIStore();

    }

// ------------------------------------------------------------------------------
// CAppUIAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CAppUIAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {

    }

// ----------------------------------------------------
// CAppUIAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ?implementation_description
// ----------------------------------------------------
//
TKeyResponse CAppUIAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
    {
	TInt currentView;

	if ( iView )
	{
    currentView = (iView->Id()).iUid;
 			
    
    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
			{
			switch ( currentView )
				{
				case ETestModulesMenuViewId:
					{
                    HandleCommandL( EAppUIGoToTestCaseMenu );
					break;
					}
				case ETestSetBaseMenuViewId:
					{
                    HandleCommandL( EAppUIGoToModulesMenu );
					ActivateLocalViewL(TUid::Uid(ETestModulesMenuViewId));
					break;
					}
                }
            break;
			}
        case EKeyRightArrow:
			{
			switch ( currentView )
				{
				case ETestCaseMenuViewId:
					{
                    HandleCommandL( EAppUIGoToModulesMenu ); // TestCaseMenu --> TestModuleMenu
					ActivateLocalViewL(TUid::Uid(ETestModulesMenuViewId));
					break;
					}
				case ETestModulesMenuViewId:
					{
					HandleCommandL( EAppUIGoToTestSetsMenu );
					ActivateLocalViewL(TUid::Uid(ETestSetBaseMenuViewId));
					break;
					}
				case ETestSetsMenuViewId:
					{
					break;
					}

				}
            break;
			}

        default:
			{
            return EKeyWasNotConsumed;
            //break; 
			}
        }
	}

    return EKeyWasConsumed;

    }

// ----------------------------------------------------
// CAppUIAppUi::HandleCommandL(TInt aCommand)
// ?implementation_description
// ----------------------------------------------------
//
void CAppUIAppUi::HandleCommandL(TInt aCommand)
    {
	TInt currentView, newView;
    TInt error(0);
    
    currentView = (iView->Id()).iUid;

    switch ( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            {
            PrepareToExit();
            Exit();
			newView = currentView;
            break;
            }
        case EAppUIGoBack:
            {
			switch ( currentView )
				{
				case ETestCaseMenuViewId:
					{
					newView = EMainMenuViewId;  // Test Cases Menu --> Main Menu
					break;
					}
				case EStartCaseMenuViewId:
					{
					newView = ETestCaseMenuViewId; // Start Cases Menu --> Test Cases Menu
                    //iTabGroup->SetActiveTabByIndex(0);
					break;
					}
				case ETestModulesMenuViewId:
					{
					newView = EMainMenuViewId; // Test Modules Menu --> Main Menu
					break;
					}
				case EStartedCasesMenuViewId:
					{
					newView = ETestCaseMenuViewId; // Started Cases Menu --> Test Case Menu
					break;
					}
				case EShowStartedCasesViewId:
					{
					newView = EStartedCasesMenuViewId; // Show started cases --> Started Cases Menu
					break;
					}
				case ETestCaseOutputViewId:
					{
                    newView = iPreviousView; // Test case output view --> previous view
					break;
					}
				case EStatisticsViewId:
					{
                    newView = EStartedCasesMenuViewId; // Statistic view --> Started Cases Menu
					break;
					}
				case ETestSetBaseMenuViewId:
					{
					newView = EMainMenuViewId;  // Test Sets Base Menu --> Main Menu
					break;
					}
				case ETestSetMenuViewId:
					{
					newView = ETestSetBaseMenuViewId;  // Test Sets Menu --> Test Sets Base Menu
					break;
					}
				case ETestSetInsertMenuViewId:
					{
					newView = ETestSetMenuViewId;  // Test Set Insert Menu --> Test Sets Menu
					break;
					}
				case ETestSetStartedCasesViewId:
					{
					newView = ETestSetMenuViewId;  // Test Set Started Cases Menu --> Test Sets Menu
					break;
					}
				default:
					{
					newView = currentView; // Back does not work. This should not happen ever
					break;
					}
				}
            break;
			}
		case EAppUIGoToMainMenu:
			{
			newView = EMainMenuViewId;
			break;
			}
		case EAppUIGoToTestCaseMenu:
			{
			newView = ETestCaseMenuViewId;
			break;
			}
		case ECmdGoToStartCasesView:
			{
			newView = EStartCaseMenuViewId;
			break;
			}
		case ECmdGoToStartedCasesView:
			{
			newView = EStartedCasesMenuViewId;
			break;
			}
		case EAppUIGoToModulesMenu:
			{
			newView = ETestModulesMenuViewId;
            //iTabGroup->SetActiveTabByIndex(1);
			break;
			}
		case EAppUIGoToTestSetsMenu:
			{
            newView = ETestSetBaseMenuViewId;
            
            //ShowNotImplementedYet();
            //newView = currentView;
			break;
			}
        case ECmdCreateTestSet:
	        {
	        CTestSetMenuView* testSetMenu = (CTestSetMenuView*)View( TUid::Uid(ETestSetMenuViewId) );
	        error = testSetMenu->CreateTestSetL( KTestSet );
	        if ( KErrNone == error ||  KErrAlreadyExists == error)
	            {
	            newView = ETestSetInsertMenuViewId;
	            currentView = ETestSetMenuViewId; // Store previous view for Back function
	            }
	        else
	            {
	            newView = ETestSetMenuViewId;
	            }
            break;
    	    }
    	case ECmdLoadTestSet:
	        {
	        newView = ECreatedTestSetMenuViewId; /*ETestSetBaseMenuViewId;*/
	        break;
			}
    	case ECmdGoToTestSetView:
    		{

    		newView = ETestSetMenuViewId; 
    		break;
    		}
        case ECmdInsertTestCases:
	        {
    		newView = ETestSetInsertMenuViewId;
    		break;
        	}
        case ECmdShowStartedTestSet:
	        {
    		newView = ETestSetStartedCasesViewId;
    		iShowStartedCasesMode = EShowAllStartedCases;
    		break;
        	}
        case ECmdShowAllStartedCases:
            newView = EShowStartedCasesViewId;
            iShowStartedCasesMode = EShowAllStartedCases;
			break;
        case ECmdShowOngoingCases:
            newView = EShowStartedCasesViewId;
            iShowStartedCasesMode = EShowOngoingCases;
        	break;
        case ECmdShowPassedCases:
            newView = EShowStartedCasesViewId;
            iShowStartedCasesMode = EShowPassedCases;
			break;
        case ECmdShowPausedCases:
            newView = EShowStartedCasesViewId;
            iShowStartedCasesMode = EShowPausedCases;
			break;
        case ECmdShowFailedCases:
            newView = EShowStartedCasesViewId;
            iShowStartedCasesMode = EShowFailedCases;
			break;
        case ECmdShowCrashedAbortedCases:
            newView = EShowStartedCasesViewId;
            iShowStartedCasesMode = EShowCrashedAbortedCases;
			break;
        case ECmdShowStatistics:
            newView = EStatisticsViewId;
			break;
        case ECmdViewOutput:
            newView = ETestCaseOutputViewId;
            break;
        case ECmdShowAbout:
        	{
        	CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog;
        	TBuf<200> version;
        	version.Format(_L("STIF UI - Version %d.%d.%d - "), STIFUI_MAJOR_VERSION, STIFUI_MINOR_VERSION, STIFUI_BUILD_VERSION);
        	version.Append(TO_UNICODE(STIFUI_REL_DATE));
        	version.Append(_L("\n"));
        	
        	TInt stifMajorV;
        	TInt stifMinorV;
        	TInt stifBuildV;
        	TBuf<30> relDate;
        	TStifUtil::STIFVersion(stifMajorV, stifMinorV, stifBuildV, relDate);
        	
        	TBuf<40> stifVersion;
        	stifVersion.Format(_L("STIF - Version %d.%d.%d - "), stifMajorV, stifMinorV, stifBuildV);
        	stifVersion.Append(relDate);
        	stifVersion.Append(_L("\n"));
        	
        	version.Append(stifVersion);
        	
        	version.Append(_L("Copyright "));
			version.Append( KCopyrightChar );
			version.Append(_L(" 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved."));
        	
        	dlg->SetMessageText(version);
        	dlg->ExecuteLD(R_STIFUI_ABOUT_DIALOG);
        	newView = currentView;
        	break;
        	}
        default:
			{
            TMessageBoxUtil::ShowNotImplementedYetL();
			newView = currentView;
            break;
			}
        }

	if( newView != currentView )
		{
        iPreviousView = currentView; // Store previous view for Back function
		ActivateLocalViewL( TUid::Uid(newView) );

		/*switch ( newView )
			{
			case EMainMenuViewId:
            case EStartedCasesMenuViewId:
            case EStartCaseMenuViewId:
            case ETestCaseOutputViewId:
				{
		        //StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
                //iTabGroup->DimTab (ETestCaseMenuTab, ETrue);
                //iTabGroup->DimTab (ETestModulesMenuTab, ETrue);
                //iTabGroup->DimTab (ETestSetsMenuTab, ETrue);

                //StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_EMPTY);
				//StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_IDLE);
				break;
				}
			case ETestCaseMenuViewId:
			case ETestModulesMenuViewId:
			case ETestSetsMenuViewId:
				{
				//StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
                //iTabGroup->DimTab (ETestCaseMenuTab, EFalse);
                //iTabGroup->DimTab (ETestModulesMenuTab, EFalse);
                //iTabGroup->DimTab (ETestSetsMenuTab, EFalse);
                break;
				}
			default:
				{
                break;
				}
			}*/
		}

    }

/**
* Receives output update notification from UI Storehandler
* @param
* @return
*/
void CAppUIAppUi::OutputUpdateL( CStartedTestCase* aTestCase, TInt /* aStatus */ )
    {
    if( iView->Id().iUid == ETestCaseOutputViewId )
        {
        ( ( CTestCaseOutputView* )iView )->OutputUpdateL( aTestCase );
        }
    else if( iView->Id().iUid == EShowStartedCasesViewId )
        {
        ( ( CShowStartedCasesView* )iView )->TestCaseStateChangedL();
        }
    else if( iView->Id().iUid == ETestCaseOutputViewId )
        {
        ( ( CTestCaseOutputView* )iView )->TestCaseStateChangedL();
        }
    }
  
/**
* Sets index of selected test case for viewing test case output
* @param
* @return
*/    
void CAppUIAppUi::SetStartedTestCase( CStartedTestCase* aStartedTestCase )
    {
    iStartedTestCase = aStartedTestCase;
    }    
  
/**
* Returns index of selected test case for viewing test case output
* @param
* @return
*/    
CStartedTestCase* CAppUIAppUi::GetStartedTestCase( )
    {
    return iStartedTestCase;
    }        

/**
 * Stores focus position of focus from a specyfic view to iPreviousFocusPosition object.
 * Value is than used to retrieve previous position of focus in the menu
 * @param aViewId - identification of view
 * @param aPosition - value to store for a specyfic view
 */
void CAppUIAppUi::SaveFocusPosition(TAppUIViewNumber aViewId, TInt aPosition)
	{
    TInt err;
	if(!iPreviousPositionListValid)
		{
		return;
		}
	for(TInt counter = iPreviousFocusPosition.Count(); counter <= aViewId; counter++)
		{
		err = iPreviousFocusPosition.Append(0);
		if (err != KErrNone)
		    {
            iLogger->Log(_L("Previous position was not saved correctly"));
		
		    }
		}
	iPreviousFocusPosition[aViewId] = aPosition;
	}

/**
 * Restores focus position of focus for a specyfic view from iPreviousFocusPosition object.
 * @param aViewId - identification of view
 * @return - previous position of focus in the requested view
 */
TInt CAppUIAppUi::GetFocusPosition(TAppUIViewNumber aViewId)
	{
	if(iPreviousFocusPosition.Count() > aViewId)
		{
		return iPreviousFocusPosition[aViewId];
		}
	else
		{
		return KErrNotFound;
		}
	}

// End of File
