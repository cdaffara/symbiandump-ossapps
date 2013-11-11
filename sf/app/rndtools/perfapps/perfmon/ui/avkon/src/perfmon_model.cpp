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

#include "perfmon_model.h"
#include "perfmon_app.h"
#include "perfmon_settingsviewdlg.h"
#include "perfmon.hrh"
#include "perfmon_valuescontainer.h"
#include "perfmon_graphscontainer.h"
#include "perfmon_datapopupcontainer.h"
#include <perfmon.rsg>

#include <coeutils.h>
#include <bautils.h>
#include <eikenv.h>
#include <e32hal.h>
#include <u32std.h>
#include <s32file.h>
#include <akntitle.h> 
#include <eikspane.h>
#include <aknnotewrappers.h>  

_LIT(KAppName, "PerfMon");

// ===================================== MEMBER FUNCTIONS =====================================

CPerfMonModel* CPerfMonModel::NewL()
	{
	CPerfMonModel* self = new(ELeave) CPerfMonModel;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// --------------------------------------------------------------------------------------------

void CPerfMonModel::ConstructL()
	{
	iDrawState = EDrawStateInvalid;
	CPerfMonEngine::ConstructL();
	}

// --------------------------------------------------------------------------------------------

void CPerfMonModel::ActivateModelL()
	{
	// initialize the data popup container in top-right corner
	iDataPopupContainer = new(ELeave) CPerfMonDataPopupContainer;
	iDataPopupContainer->ConstructL(TRect(0,0,1,1));
    
	ActivateEngineL();
	}

// --------------------------------------------------------------------------------------------

void CPerfMonModel::DeActivateModelL()
	{
	DeActivateEngineL();
	
	if (iDataPopupContainer)
		{
		delete iDataPopupContainer;
		iDataPopupContainer = NULL;
		}
	}

// --------------------------------------------------------------------------------------------

void CPerfMonModel::SetValuesContainer(CPerfMonValuesContainer* aContainer)
    {
    iValuesContainer = aContainer;
    iDrawState = EDrawStateValues;
    }

// --------------------------------------------------------------------------------------------

void CPerfMonModel::SetGraphsContainer(CPerfMonGraphsContainer* aContainer)
    {
    iGraphsContainer = aContainer;
    iDrawState = EDrawStateGraphs;
    }
    
// --------------------------------------------------------------------------------------------

void CPerfMonModel::SendDrawEventToContainersL()
    {
    if (iDrawState == EDrawStateValues && iValuesContainer)
        iValuesContainer->DrawUpdate();
    else if (iDrawState == EDrawStateGraphs && iGraphsContainer)
        iGraphsContainer->DrawUpdate();
           
    if (iDataPopupContainer)
        iDataPopupContainer->DrawUpdate();
    }

void CPerfMonModel::HandleSettingsChangeL()
    {
    // set visibility and location of the data popup
    iDataPopupContainer->UpdateVisibility();
    iDataPopupContainer->SetPositionAndSize();
    CPerfMonEngine::HandleSettingsChangeL();
    }
            
// --------------------------------------------------------------------------------------------

TInt CPerfMonModel::LaunchSettingsDialogL()
    {
    // launch the settings dialog
    TPerfMonSettings newSettings = iSettings;
    
    CPerfMonSettingsViewDlg* dlg = CPerfMonSettingsViewDlg::NewL(newSettings);
    TInt returnValue = dlg->ExecuteLD(R_PERFMON_SETTINGS_DIALOG);
    
    // always save settings since the settings dialog does not provide a possibility to cancel
    iSettings = newSettings;
    SaveSettingsL();
    HandleSettingsChangeL();

    // make sure that the title of the application is correct
    CEikStatusPane* sp = iEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    tp->SetTextL(KAppName);
    
    return returnValue;
    }
	
// ---------------------------------------------------------------------------
    
TInt CPerfMonModel::SampleEntryPosToSettingPos(TInt aSampleEntryPos)
    {
    TInt settingPos(0); // return position of aSampleEntryPos in settings
    
    if (aSampleEntryPos >= iCPU0PositionInSamples && aSampleEntryPos < iRAMPositionInSamples)
        {
        settingPos = ESourceCPU;
        }
    else if (aSampleEntryPos == iRAMPositionInSamples)
        {
        settingPos = ESourceRAM;
        }
    else
        {
        settingPos = ESourceC + (aSampleEntryPos-iCDrivePositionInSamples);
        }
    
    return settingPos;
    } 

// ---------------------------------------------------------------------------

// End of File
