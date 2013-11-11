/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


// INCLUDE FILES
#include "loadgen_appui.h"
#include "loadgen_mainview.h"
#include "loadgen.hrh"
#include "loadgen_model.h"
#include "loadgen_document.h"
#include <loadgen.rsg>

#include <avkon.hrh>
#include <AknQueryDialog.h>
#include <aknmessagequerydialog.h> 


// ================= MEMBER FUNCTIONS =======================

void CLoadGenAppUi::ConstructL()
    {
    // disable window server priority control for this application
    iEikonEnv->WsSession().ComputeMode( RWsSession::EPriorityControlDisabled );
    
    // set as system application to prevent getting shut down events 
    iEikonEnv->SetSystem(ETrue);
        

    BaseConstructL(EAknEnableSkin);

    iModel = static_cast<CLoadGenDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();

    CLoadGenMainView* mainView = new(ELeave) CLoadGenMainView;
    CleanupStack::PushL(mainView);
    mainView->ConstructL();
    AddViewL(mainView);           // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();          // mainView
    
    SetDefaultViewL(*mainView);

    // notify the model that everything has been constructed
    iModel->ActivateModelL();
    }

// --------------------------------------------------------------------------------------------

CLoadGenAppUi::~CLoadGenAppUi()
    {
    // notify the model that the application is closing 
    if (iModel)   
        TRAP_IGNORE(iModel->DeActivateModelL());
    }

// --------------------------------------------------------------------------------------------

void CLoadGenAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    if (aResourceId == R_LOADGEN_APP_MENU)
        {
        aMenuPane->SetItemDimmed(ELoadGenStopAll, !iModel->LoadItemsExists());
        aMenuPane->SetItemDimmed(ELoadGenSuspendAll, !iModel->LoadItemsExists());
        aMenuPane->SetItemDimmed(ELoadGenResumeAll, !iModel->LoadItemsExists());
        }
    }

// --------------------------------------------------------------------------------------------

TKeyResponse CLoadGenAppUi::HandleKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// --------------------------------------------------------------------------------------------

void CLoadGenAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case ELoadGenCmdLaunchPerfMon:
            {
            // launch Performance Monitor
            const TUid KUidPerfMon = { 0x20011385 };
            
            RWsSession ws;
            User::LeaveIfError( ws.Connect() );
            CleanupClosePushL(ws);
            
            RApaLsSession ls;
            User::LeaveIfError( ls.Connect() );
            CleanupClosePushL(ls);
            
            
            // try to find the task of PerfMon
            TApaTaskList tasklist(ws);
            TApaTask task = tasklist.FindApp(KUidPerfMon);

            if (task.Exists())
                {
                // task exists, bring it to foreground
                task.BringToForeground();
                }
            else
                {
                TApaAppInfo appInfo;
                User::LeaveIfError(ls.GetAppInfo(appInfo, KUidPerfMon));
                
                CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
                cmdLine->SetExecutableNameL(appInfo.iFullName);
                cmdLine->SetCommandL(EApaCommandBackground);

                // start the app
                User::LeaveIfError(ls.StartApp(*cmdLine));
                
                CleanupStack::PopAndDestroy(); //cmdLine                
                }
            
            CleanupStack::PopAndDestroy(2); //ws,ls
            }
            break;
            
        case ELoadGenCmdAbout:
            {
            CAknMessageQueryDialog* dialog = new(ELeave) CAknMessageQueryDialog;
            dialog->ExecuteLD(R_LOADGEN_ABOUT_DIALOG);
            }
            break;
            
        // a normal way to close an application
        case EAknCmdExit:
        case EEikCmdExit:
        case EAknSoftkeyExit: 
            {
            Exit();
            }
            break;

        default:
            break;      
        }
    }

// --------------------------------------------------------------------------------------------

// End of File  
