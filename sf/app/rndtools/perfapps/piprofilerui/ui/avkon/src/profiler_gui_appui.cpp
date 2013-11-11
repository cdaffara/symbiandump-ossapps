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
* Description:   
*
*/



// INCLUDE FILES
#include "profiler_gui_appui.h"
#include "profiler_gui_mainview.h"
#include "profiler_gui.hrh"
#include "profiler_gui_model.h"
#include "profiler_gui_document.h"
#include <piprofilerui.rsg>
#include <piprofiler/ProfilerTraces.h>
#include <piprofiler/ProfilerSession.h>
#include <piprofiler/ProfilerConfig.h>

#include <AknWaitDialog.h>
#include <AknGlobalNote.h>
#include <avkon.hrh>
#include <AknQueryDialog.h>
#include <aknmessagequerydialog.h> 
#include <pathinfo.h>


// ================= MEMBER FUNCTIONS =======================

void CProfilerGuiAppUi::ConstructL()
    {
    // set as system application to prevent getting shut down events 
    iEikonEnv->SetSystem(ETrue);
    
    BaseConstructL(EAknEnableSkin);
    
    // get model
    iModel = static_cast<CProfilerGuiDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();
	
	LOGTEXT(_L("ProfilerGuiAppUi::ConstructL - setting main view settings"));
    
    CProfilerGuiMainView* mainView = new(ELeave) CProfilerGuiMainView;
    CleanupStack::PushL(mainView);
    mainView->ConstructL();
    AddViewL(mainView);           // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();          // mainView

	LOGTEXT(_L("ProfilerGuiAppUi::ConstructL - setting default view"));

    SetDefaultViewL(*mainView);

    LOGTEXT(_L("ProfilerGuiAppUi::ConstructL - activating model"));

    // notify the model that everything has been constructed
    iModel->ActivateModelL();
    }

// --------------------------------------------------------------------------------------------

CProfilerGuiAppUi::~CProfilerGuiAppUi()
    {
    // notify the model that the application is closing 
    if (iModel)   
        TRAP_IGNORE(iModel->DeActivateModelL());

    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    if( aResourceId == R_PROFILER_START_SUBMENU )
        {
        CEikMenuPaneItem::SData itemData;
        _LIT(KMenuText, "Profiling for %d sec.");
        itemData.iText.Format(KMenuText, iModel->GeneralSettings().iTimedSamplingPeriod);// = _L("Profiling for %d sec.");  // Label text for the menu item
        itemData.iCommandId = EProfilerGuiCmdStartTimed;    // Command ID for the menu item
        itemData.iFlags = 0;
        itemData.iCascadeId = 0;
        aMenuPane->AddMenuItemL( itemData );
        }
    if (aResourceId == R_PROFILER_GUI_APP_MENU)
        {
        if(iModel->iState == MProfilerStatusObserver::ERunning)
        	{
			aMenuPane->SetItemDimmed(EProfilerGuiCmdStartAll, ETrue);
			aMenuPane->SetItemDimmed(EProfilerGuiCmdStopAll, EFalse);
			aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerControl, ETrue);
            aMenuPane->SetItemDimmed(EProfilerGuiCmdSettings, ETrue);
        	}
        else if(iModel->iState == MProfilerStatusObserver::EIdle)
        	{
			aMenuPane->SetItemDimmed(EProfilerGuiCmdStartAll, EFalse);
			aMenuPane->SetItemDimmed(EProfilerGuiCmdStopAll, ETrue);
            aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerControl, EFalse);
            aMenuPane->SetItemDimmed(EProfilerGuiCmdSettings, EFalse);
        	}
        else
        	{
        	// if initializing or stopping no start/stop actions allowed 
			aMenuPane->SetItemDimmed(EProfilerGuiCmdStartAll, ETrue);
			aMenuPane->SetItemDimmed(EProfilerGuiCmdStopAll, ETrue);
            aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerControl, ETrue);
            aMenuPane->SetItemDimmed(EProfilerGuiCmdSettings, ETrue);
        	}
        }
    
    if(aResourceId == R_PROFILER_SAMPLER_CONTROL_MENU)
        {
        // check if item hidden, i.e. sampler cannot be started/stopped
        if(!iModel->GetSelectedItemHidden())
            {
            // item enabled => dimm the "enable" item, and other way round
            if(iModel->GetSelectedItemEnabled())
                {
                aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerEnable, ETrue);
                aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerDisable, EFalse);
                }
            // item enabled => dimm the "enable" item, and other way round
            else
                {
                aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerEnable, EFalse);
                aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerDisable, ETrue);
                }

            }
        // item hidden => dimm both enable and disable items
        else
            {
            aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerEnable, ETrue);
            aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerDisable, ETrue);
            }
        
        // if sampler item has specific settings to control 
        if(iModel->GetSelectedItemHasSettings())
            {
            aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerSettings, EFalse);
            }
        else
            {
            aMenuPane->SetItemDimmed(EProfilerGuiCmdSamplerSettings, ETrue);
            }
        }
    }

// --------------------------------------------------------------------------------------------

TKeyResponse CProfilerGuiAppUi::HandleKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

void CProfilerGuiAppUi::HandleSystemEventL(const TWsEvent& aEvent)
    {
    switch (*(TApaSystemEvent*)(aEvent.EventData()))
        {
        case EApaSystemEventShutdown:
            // check if still profiling
            if( iModel->iState != MProfilerStatusObserver::EIdle )
                {
                // stop profiling process
                iModel->StopAllSamplerItemsL();
                }

            // terminate profiler engine
            iModel->TerminateProfilerL();
            break;
        default:
            break;
        }
    // call base class implementation
    CAknAppUi::HandleSystemEventL(aEvent);
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EProfilerGuiCmdSettings:
            {
            // check if exit command received
            if (iModel->LaunchSettingsDialogL() == EAknCmdExit)
                {
                // terminate profiler engine...
                iModel->TerminateProfilerL();
                
                // ... and exit
                Exit();
                }
            break;
            }
        case EProfilerGuiCmdAbout:
            {
	        CAknMessageQueryDialog* dialog = new(ELeave) CAknMessageQueryDialog;
            dialog->ExecuteLD(R_PROFILER_GUI_ABOUT_DIALOG);
            }
            break;
            
        // a normal way to close an application
        case EAknCmdExit:
        case EEikCmdExit:
        case EAknSoftkeyExit: 
			{
			// decrease the client reference count in the server:
			iModel->RemoveClient();
            // check if still profiling
            if( iModel->iState != MProfilerStatusObserver::EIdle && 
                    iModel->iState != MProfilerStatusObserver::EStopping )
                {
                // ask user if he wants to leave profiling running in background process 
                if( this->RunConfQueryL( NULL ) == 0 )
                    {
                    // stop profiling process
                    iModel->StopAllSamplerItemsL();

                    // terminate profiler engine
                    iModel->TerminateProfilerL();
                    }
                }
            else
                {
                // terminate profiler engine
                iModel->TerminateProfilerL();
                }
            
            Exit();
 			}
            break;

        default:
            break;      
        }
    }

TInt CProfilerGuiAppUi::RunConfQueryL( const TDesC* aOverrideText )
	{
	CAknQueryDialog* queryDialog = CAknQueryDialog::NewL();
	
	if(aOverrideText)
		{
		queryDialog->SetPromptL(*aOverrideText);
		}
	return queryDialog->ExecuteLD(R_PROFILER_GUI_CONF_QUERY);
	}


// End of File  
