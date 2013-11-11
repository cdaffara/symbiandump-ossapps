/*
 * Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: The application UI class for ConnTest
*
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <AknsUtils.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h> 
#include <CMessageData.h>
#include <TSendingCapabilities.h> 

#include "ConnTestAppUi.h"
#include "ConnTestView.h"
#include <conntest.rsg>
#include "conntest.hrh"
#include "SettingData.h"
#include "ipdumpengine.h"

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CConnTestAppUi::ConstructL()
// Default Symbian two phase constructor
// ----------------------------------------------------------
//
void CConnTestAppUi::ConstructL()
    {
    RDebug::Print(_L("ConnTest: CConnTestAppUi::ConstructL()"));
    BaseConstructL();

    AknsUtils::SetAvkonSkinEnabledL( ETrue );

    // Show tabs for main views from resources
    CEikStatusPane* sp = StatusPane();

    // Fetch pointer to the default navi pane control
    iNaviPane = (CAknNavigationControlContainer*)sp->ControlL( 
            TUid::Uid(EEikStatusPaneUidNavi));

    // Tabgroup has been read from resource and it were pushed to the navi pane. 
    // Get pointer to the navigation decorator with the ResourceDecorator() function. 
    // Application owns the decorator and it has responsibility to delete the object.
    iDecoratedTabGroup = iNaviPane->ResourceDecorator();
    if (iDecoratedTabGroup)
        {
        iTabGroup = (CAknTabGroup*) iDecoratedTabGroup->DecoratedControl();
        }

    TUid uid;
    CConnTestView* view1;

    // Create views; each view has same UI, but the data differs.
    for(TInt i = 1; i <= KConnTestViews; i++)
        {
        RDebug::Print(_L("ConnTest: CConnTestAppUi::ConstructL() View=%d"), i);
        uid.iUid = i;
        view1 = new (ELeave) CConnTestView(uid, iSettingDataArray[i-1], iCustomPrefsDataArray[i-1]);
        CleanupStack::PushL( view1 );
        view1->ConstructL();
        AddViewL( view1 );      // transfer ownership to CAknViewAppUi
        CleanupStack::Pop(view1);    // view1

        iConnTestViews.Append(view1);
        // First view must be set to default, otherwise views and their
        // data are not in sync.
        if(i == 1)
            {
            SetDefaultViewL(*view1);
            }
        }
    }


// ----------------------------------------------------
// CConnTestAppUi::CConnTestAppUi()
// Constructor
// ----------------------------------------------------
//
CConnTestAppUi::CConnTestAppUi(
        TFixedArray<CSettingData*, KConnTestViews>& aSettings,
        TFixedArray<CCustomPrefsData*, KConnTestViews>& aCustomPrefs)
    : iSettingDataArray(aSettings), iCustomPrefsDataArray(aCustomPrefs)
    {
    }

// ----------------------------------------------------
// CConnTestAppUi::~CConnTestAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CConnTestAppUi::~CConnTestAppUi()
    {
    RDebug::Print(_L("ConnTest: CConnTestAppUi::~CConnTestAppUi"));
    if(iIpdumpEngine)
        {
        delete iIpdumpEngine;
        iIpdumpEngine = NULL;
        }
    delete iDecoratedTabGroup;
    iConnTestViews.Reset();
    // currently used with ->ExecuteLD, so deleted automatically elsewhere
    // delete iWarningDialog;
    }

// ----------------------------------------------------
// CConnTestAppUi::IsLogging() 
// Is the ip packet logging on
// ----------------------------------------------------
//
TBool CConnTestAppUi::IsLogging()
    {
    TBool res = EFalse;
    if(iIpdumpEngine)
        res = iIpdumpEngine->LoggingEnabled();
    return res;
    }

// ----------------------------------------------------
// CConnTestAppUi::HandleStatusPaneSizeChange() 
// This is called when ever the screen size has changed
// Takes care that the view components are resized
// ----------------------------------------------------
//
void CConnTestAppUi::HandleStatusPaneSizeChange()
    {
    CAknViewAppUi::HandleStatusPaneSizeChange();
    for (TInt i = 0; i < KConnTestViews; i++)
        {
        iConnTestViews[i]->SizeChanged();
        }
    }

// ------------------------------------------------------------------------------
// CConnTestAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CConnTestAppUi::DynInitMenuPaneL(TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CConnTestAppUi::HandleCommandL(TInt aCommand)
// Handles some of the users commands.
// ----------------------------------------------------
//
void CConnTestAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        // Parallel GET
        case EConnTestCmdParallelGet:
            {

            TInt viewCount = 2;
            CAknNumberQueryDialog* dlg =
            CAknNumberQueryDialog::NewL(viewCount);
            if( dlg->ExecuteLD(R_CONNTEST_PARALLEL_NUMBER_LAYOUT))
                {
                // ask engine to handle combining.
                if ((viewCount > 0) && (viewCount <= KConnTestViews))
                    {
                    ParallelGetL(viewCount);
                    }
                }
            }
            break;
            // Start logging
        case EConnTestCmdStartIpLog:
            StartIpLoggingL();
            break;
            // Stop logging
        case EConnTestCmdStopIpLog:
            StopIpLoggingL();
            break;
            // Send log
        case EConnTestCmdSendIpLog:
            SendLogFileL();
            break;

        case EEikCmdExit:
            SaveL();
            Exit();
            break;

        default:
            break;      
        }
    }

// ----------------------------------------------------
// CConnTestAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// Handles UI key events.
// ----------------------------------------------------
//
TKeyResponse CConnTestAppUi::HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode /*aType*/ )
    {
    if (!iTabGroup)
        {
        return EKeyWasNotConsumed;
        }

    TInt active = iTabGroup->ActiveTabIndex();
    TInt count = iTabGroup->TabCount();

    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
            if ( active > 0 )
                {
                active--;
                RDebug::Print(_L("ConnTest: CConnTestAppUi::HandleKeyEventL Switching to view %d"), active);
                iTabGroup->SetActiveTabByIndex( active );
                ActivateLocalViewL(TUid::Uid(iTabGroup->TabIdFromIndex(active)));
                }
            break;
        case EKeyRightArrow:
            if( (active + 1) < count )
                {
                active++;
                RDebug::Print(_L("ConnTest: CConnTestAppUi::HandleKeyEventL Switching to view %d"), active);
                iTabGroup->SetActiveTabByIndex( active );
                ActivateLocalViewL(TUid::Uid(iTabGroup->TabIdFromIndex(active)));
                }
            break;
        default:
            return EKeyWasNotConsumed;
            // break; get rid of cmpiler warning
        }

    return EKeyWasConsumed;
    }

// ----------------------------------------------------------------------------
// CConnTestView::ParallelGet
// Executes multiple HTTP gets over socket
// ----------------------------------------------------------------------------
//
void CConnTestAppUi::ParallelGetL(TInt aCount)
    {
    for (TInt i = 0; i < aCount; i++)
        {
        if (!iConnTestViews[i]->IsReadyForHTTPGet())
            {
            _LIT(KWarning, "Not enough open connections" );
            iWarningDialog = new (ELeave) CAknWarningNote(&iWarningDialog);
            // notice, will be deleted automatically
            iWarningDialog->ExecuteLD(KWarning);
            return;
            }
        iConnTestViews[i]->HandleCommandL(EConnTestCmdSendPerformanceHttpGet);
        }
    }

// ----------------------------------------------------
// CConnTestAppUi::SendLogFileL() 
// Launches the send ui for the ip log
// ----------------------------------------------------
//
void CConnTestAppUi::SendLogFileL()
    {
    // Empty implementation, CSendUi dependency removed
    }

// ----------------------------------------------------
// CConnTestAppUi::StartIpLoggingL() 
// Starts the logging of the ip traffic
// ----------------------------------------------------
//
void CConnTestAppUi::StartIpLoggingL()
{
    _LIT(KLogEnabled, "IP logging enabled: " );
    _LIT(KLogError, "Error %d starting logging, Protocol not loaded. Try rebooting." );

    if(!iIpdumpEngine)
    {
        iIpdumpEngine = CIpdumpEngine::NewL();                   
    }
    if(!iIpdumpEngine->LoggingEnabled())
    {
        
        TRAPD(r, iIpdumpEngine->EnableLoggingL());
        if(r == KErrNone)
        {
        	TBuf<256> tt;
			tt.Format(KLogEnabled);
		    tt.Append(iIpdumpEngine->LogFileName());

            CAknInformationNote* note = new (ELeave) CAknInformationNote();
            note->ExecuteLD(tt); 
        }
        else
        {
        	TBuf<64> tt;
			tt.Format(KLogError, r);
	        iWarningDialog = new (ELeave) CAknWarningNote(&iWarningDialog);
	        // notice, will be deleted automatically
            iWarningDialog->ExecuteLD(tt);
        }
    }
}

// ----------------------------------------------------
// CConnTestAppUi::StopIpLoggingL() 
// Stops the logging of the ip traffic
// ----------------------------------------------------
//
void CConnTestAppUi::StopIpLoggingL()
{
    _LIT(KLogDisabled, "IP logging disabled" );
    if(iIpdumpEngine && iIpdumpEngine->LoggingEnabled())
    {
		iIpdumpEngine->DisableLogging();
        CAknInformationNote* note = new (ELeave) CAknInformationNote();
        note->ExecuteLD(KLogDisabled); 
    }
}

// End of File  
