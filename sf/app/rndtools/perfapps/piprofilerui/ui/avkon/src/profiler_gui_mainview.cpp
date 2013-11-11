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
#include "profiler_gui.hrh"
#include "profiler_gui_mainview.h"
#include "profiler_gui_maincontainer.h"
#include "profiler_gui_document.h" 
#include "profiler_gui_model.h"
#include <piprofilerui.rsg>

#include <AknWaitDialog.h>
#include <AknGlobalNote.h>
#include <eikenv.h>
#include <aknViewAppUi.h> 
#include <akncontext.h>
#include <StringLoader.h>
#include <aknnavi.h>
#include <akntitle.h>
#include <barsread.h>
#include <aknnavide.h>
#include <aknmessagequerydialog.h> 

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CProfilerGuiMainView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CProfilerGuiMainView::ConstructL()
    {
    BaseConstructL( R_PROFILER_GUI_VIEW_MAIN );
    
    iModel = static_cast<CProfilerGuiDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();
    iNaviDecorator = NULL;
    iModel->SetMainView(this);
    }

// ---------------------------------------------------------
// CProfilerGuiMainView::~CProfilerGuiMainView()
// ---------------------------------------------------------
//
CProfilerGuiMainView::~CProfilerGuiMainView()
    {

    if(iNaviDecorator)
    	{
    	delete iNaviDecorator;
    	iNaviDecorator = NULL;
    	}
    
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    
    }

// ---------------------------------------------------------
// TUid CProfilerGuiMainView::Id()
// ---------------------------------------------------------
//
TUid CProfilerGuiMainView::Id() const
    {
    return KMainViewUID;
    }

// ---------------------------------------------------------
// TUid CProfilerGuiMainView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
// ---------------------------------------------------------
//
void CProfilerGuiMainView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    AppUi()->DynInitMenuPaneL(aResourceId, aMenuPane);
    }

  
// ---------------------------------------------------------
// CProfilerGuiMainView::HandleCommandL(TInt aCommand)
// ---------------------------------------------------------
//
void CProfilerGuiMainView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        // commands from sampler control sub menu 
        case EProfilerGuiCmdSamplerEnable:
            {
            iModel->DisableOrEnableSelectedOrHighlightedItemsL();
            break;
            }
        case EProfilerGuiCmdSamplerDisable:
            {
            iModel->DisableOrEnableSelectedOrHighlightedItemsL();
            break;
            }
        case EProfilerGuiCmdSamplerSettings:
            {
            // check if sampler settings dialog returned exit command
            iModel->EditSelectedSamplerL(iContainer->CurrentListBoxItemIndex());
            break;
            }
        case EProfilerGuiCmdSamplerInfo:
            {
            iModel->SelectedSamplerInfoL(iContainer->CurrentListBoxItemIndex());
            break;
            }
            
        case EProfilerGuiCmdStartAll:
        case EProfilerGuiCmdStartTimed:
            {
            // still using the old way of starting all the samplers simultaneously
            if( iModel->iState != MProfilerStatusObserver::ERunning )
            	{
            	// set state as "initializing" for a moment before profiling has started, 
            	// NOTE: engine changes to "Running" state
            	iModel->iState = MProfilerStatusObserver::EInitializing;
                // start the actual sampling process on Profiler Engine 
            	if( aCommand == EProfilerGuiCmdStartTimed )
            	    iModel->StartAllSamplerItemsL(CProfilerGuiModel::EProfilingModeTimed);
            	else
            	    iModel->StartAllSamplerItemsL();
            	}

			// prevent the control of sampler specific settings during the trace 
            iContainer->SetDimmed(ETrue);
            break;
            }
        case EProfilerGuiCmdStopAll:
            {
			// check if still profiling
			if( iModel->iState != MProfilerStatusObserver::EIdle )
				{
				// set to stopping mode
				iModel->iState = MProfilerStatusObserver::EStopping;
                // stop actual sampling process on Profiler Engine
				iModel->StopAllSamplerItemsL();
				}
            
			// return the control to sampler specific settings when tracing stopped
			iContainer->SetDimmed(EFalse);

            break;
            }
            
        default:
            {
            AppUi()->HandleCommandL(aCommand);
            break;
            }
        }
    }

// ---------------------------------------------------------
// CProfilerGuiMainView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CProfilerGuiMainView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CProfilerGuiMainView::DoActivateL(...)
// ---------------------------------------------------------
//
void CProfilerGuiMainView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    // show wait dialog
    CAknGlobalNote* waitDialog = CAknGlobalNote::NewLC();
    waitDialog->SetSoftkeys(R_AVKON_SOFTKEYS_EMPTY);
    TInt dialogId = waitDialog->ShowNoteL(EAknGlobalWaitNote, _L("Initializing samplers"));

    // setup the profiler engine status pane
    SetupStatusPaneL();
    
    // check if container creation succesfull
    if (!iContainer)
        {
        iContainer = new (ELeave) CProfilerGuiMainContainer;
        iModel->SetMainContainer(iContainer);
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        iModel->NotifyContainerReadyL();
        AppUi()->AddToStackL( *this, iContainer );
        } 
    
	// remove the wait dialog
	waitDialog->CancelNoteL(dialogId);
	CleanupStack::PopAndDestroy(); //waitDialog;
   }

// ---------------------------------------------------------
// CProfilerGuiMainView::DoDeactivate()
// ---------------------------------------------------------
//
void CProfilerGuiMainView::DoDeactivate()
    {
    TRAP_IGNORE(CleanupStatusPaneL());
    
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

void CProfilerGuiMainView::HandleStatusPaneSizeChange()
	{
	CAknView::HandleStatusPaneSizeChange();
	
	TInt result;
	TRAP(result, SetupStatusPaneL());
	}


void CProfilerGuiMainView::SetupStatusPaneL()
	{
	TUid naviPaneUid = TUid::Uid(EEikStatusPaneUidNavi);
	CEikStatusPaneBase::TPaneCapabilities subPaneNavi = StatusPane()->PaneCapabilities(naviPaneUid);
	if(subPaneNavi.IsPresent() && subPaneNavi.IsAppOwned())
		{
		CAknNavigationControlContainer* naviPane = static_cast<CAknNavigationControlContainer*> (StatusPane()->ControlL(naviPaneUid));
		if(iNaviDecorator)
			{
			delete iNaviDecorator;
			iNaviDecorator = NULL;
			}
		
		// get the initial status label
		HBufC* labelText = GetLabelTextLC(iModel->iState);
		iNaviDecorator = naviPane->CreateNavigationLabelL(*labelText);
		CleanupStack::PopAndDestroy(labelText);
		
		naviPane->PushL(*iNaviDecorator);
		}
	}

void CProfilerGuiMainView::CleanupStatusPaneL()
	{
	// destructor for navi pane, called by destructor
	TUid naviPaneUid = TUid::Uid(EEikStatusPaneUidNavi);
	CEikStatusPaneBase::TPaneCapabilities subPaneNavi = StatusPane()->PaneCapabilities(naviPaneUid);
	if(subPaneNavi.IsPresent() && subPaneNavi.IsAppOwned())
		{
		CAknNavigationControlContainer* naviPane = static_cast<CAknNavigationControlContainer*> (StatusPane()->ControlL(naviPaneUid));
		if(iNaviDecorator)
			{
			naviPane->Pop(iNaviDecorator);
			delete iNaviDecorator;
			iNaviDecorator = NULL;
			}
		}
	}

HBufC* CProfilerGuiMainView::GetLabelTextLC(TInt aStatus)
	{
	// get the predefined status text
	HBufC* labelText = NULL;
	switch(aStatus)
		{
		case MProfilerStatusObserver::EIdle:
			labelText = StringLoader::LoadLC(R_PROFILER_GUI_NAVI_TEXT_IDLE);
			break;
		case MProfilerStatusObserver::EInitializing:
			labelText = StringLoader::LoadLC(R_PROFILER_GUI_NAVI_TEXT_INITIALIZING);
			break;
		case MProfilerStatusObserver::ERunning:
			labelText = StringLoader::LoadLC(R_PROFILER_GUI_NAVI_TEXT_RUNNING);
			break;
		case MProfilerStatusObserver::EStopping:
			labelText = StringLoader::LoadLC(R_PROFILER_GUI_NAVI_TEXT_STOPPING);
			break;
	     case MProfilerStatusObserver::ERestarting:
            labelText = StringLoader::LoadLC(R_PROFILER_GUI_NAVI_TEXT_RUNNING);
            break;
	     default:
	        labelText = StringLoader::LoadLC(R_PROFILER_GUI_NAVI_TEXT_IDLE); 
	        break;
		}
	return labelText;
	}

void CProfilerGuiMainView::UpdateStatusPaneL( TInt aStatus )
	{
	TUid naviPaneUid = TUid::Uid(EEikStatusPaneUidNavi);
	CEikStatusPaneBase::TPaneCapabilities subPaneNavi = StatusPane()->PaneCapabilities(naviPaneUid);
	// check if navi pane is correctly initialized
	if(subPaneNavi.IsPresent() && subPaneNavi.IsAppOwned())
		{
		CAknNavigationControlContainer* naviPane = static_cast<CAknNavigationControlContainer*> (StatusPane()->ControlL(naviPaneUid));
		if(iNaviDecorator)
			{
			delete iNaviDecorator;
			iNaviDecorator = NULL;
			}
		HBufC* labelText;

		// get the right status label text 
		labelText = GetLabelTextLC(aStatus);
		iNaviDecorator = naviPane->CreateNavigationLabelL(*labelText);
		CleanupStack::PopAndDestroy(labelText);
		
		naviPane->PushL(*iNaviDecorator);
		}
	}


// End of File
