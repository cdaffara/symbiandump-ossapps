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
#include "profiler_gui_samplersettingsviewdlg.h"
#include "profiler_gui_model.h"
#include "profiler_gui_app.h"
#include "profiler_gui_settingsviewdlg.h"
#include "profiler_gui_maincontainer.h"
#include "profiler_gui.hrh"
#include <piprofilerui.rsg>

#include <coeutils.h>
#include <bautils.h>
#include <eikenv.h>
#include <AknQueryDialog.h>
#include <AknGlobalNote.h>
#include <aknmessagequerydialog.h> 
#include <e32math.h> 
#include <akntitle.h> 
#include <s32file.h>
#include <aknnotewrappers.h>
#include <sysutil.h>

// UIDs
#include <piprofiler/EngineUIDs.h>

#include <piprofiler/ProfilerTraces.h>
#include <piprofiler/ProfilerSession.h>

// LITERALS
_LIT(KAppName, "PI Profiler");
_LIT(KWarningNote, "NOTE: output changed!\n");
_LIT(KProfilerEngineExe, "PIProfilerEngine.exe");

// literals for default general setting values
_LIT8(KTraceOutput, "file_system");
_LIT8(KTraceDebugOutput, "debug_output");
_LIT8(KProfilerDefaultDrive, "E:\\data");
_LIT8(KProfilerDefaultPrefix, "Profiler_#");

// ===================================== MEMBER FUNCTIONS =====================================

CProfilerGuiModel* CProfilerGuiModel::NewL() 
	{
	CProfilerGuiModel* self = new(ELeave) CProfilerGuiModel;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// --------------------------------------------------------------------------------------------

CProfilerGuiModel::CProfilerGuiModel() : CActive(EPriorityStandard)
	{
	}
	
// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::ConstructL()
	{
	// initialize basic settings
	iDrawState = EDrawStateInvalid;
	iReferenceNumber = 0;
	iState = MProfilerStatusObserver::EIdle;
	
    iEnv = CEikonEnv::Static();

    // profiler engine specific initialization
    LaunchProfilerEngineL();
    
    // max sampler item list length is 64, i.e. max 64 sampler plugins loaded 
    iSamplerItemList = new(ELeave) CSamplerItemList(64);

    // initialize attribute arrays
    iSamplerAttributes = new(ELeave) CArrayFixFlat<TSamplerAttributes>(20); // max sampler count is 20
    
    // engine status checker
    iStatusChecker = CProfilerEngineStatusChecker::NewL();
    iStatusChecker->SetObserver(this);
 	
	CActiveScheduler::Add(this);
	}

void CProfilerGuiModel::UpdateUIRunningStateL()
    {
    // prevent the control of sampler specific settings during the trace 
    iMainContainer->SetDimmed(ETrue);
    
    // update status pane
    iMainView->UpdateStatusPaneL(iState);

    // show an info popup showing the logging method
    iMainContainer->ShowWriterInfoPopupL(KNullDesC);
    
    // refresh view
    RefreshViewL();
    }

// --------------------------------------------------------------------------------------------
void CProfilerGuiModel::NotifyContainerReadyL()
	{
	// load initial plugins
    LoadPluginsL();
    
    // get the initial state
    if( iStatusChecker->GetInitialState() == MProfilerStatusObserver::ERunning )
        {
        // set model state to restarting and grabbing an existing profiler process
        iState = MProfilerStatusObserver::ERestarting;
        
        // update status pane to correspond the running mode
        UpdateUIRunningStateL();

        // set model state to running
        iState = MProfilerStatusObserver::ERunning;

        }
	}

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::ActivateModelL()
	{
	// load general settings
	if( LoadGeneralSettingsL() != KErrNone )
	    {
	    LOGTEXT(_L("ProfilerGuiAppUi::ActivateModelL - could not connect profiler engine"));
	    }
	}

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::DeActivateModelL()
	{
	Cancel();
	
	// for a faster exit, send the application to background
    TApaTask selfTask(iEnv->WsSession());
    selfTask.SetWgId(iEnv->RootWin().Identifier());
    selfTask.SendToBackground();
	}
	
// --------------------------------------------------------------------------------------------

CProfilerGuiModel::~CProfilerGuiModel()
	{
    if (iSamplerItemList)
        {
        DeleteAllSamplerItems();
        delete iSamplerItemList;
        iSamplerItemList = NULL;
        }
    
    if(iStatusChecker)
        {
        iStatusChecker->Cancel();
        delete iStatusChecker;
        iStatusChecker = NULL;
        }
    
  
    if(iSamplerAttributes)
        {
        iSamplerAttributes->Reset();
        delete iSamplerAttributes;
        iSamplerAttributes = NULL;
        }
     
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::DoCancel()
	{
    
	}

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::RunL()
	{

	}
	
// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::SetMainContainer(CProfilerGuiMainContainer* aContainer)
    {
    iMainContainer = aContainer;
    iDrawState = EDrawStateMain;
    }

void CProfilerGuiModel::UpdateState(TInt aState)
	{
	iState = aState;
	}

TInt CProfilerGuiModel::FindProcessL(RProcess& aProc)
    {
    TProcessId engId;
    TFindProcess procName;
    procName.Find(_L("PIProfilerEngine.exe*"));
    TFullName aResult;
    TFullName aResult2;
    TInt err(KErrNone);

    // find the first appearance
    err = procName.Next(aResult);
    if(err != KErrNone)
        {
        // did not find any engine process
        return err;
        }
    else
        {
        err = aProc.Open(procName);
        if(err == KErrNone)
            {
            if(aProc.ExitCategory().Length() > 0)
                {
                aProc.Close();
                // process already exited => create a new one
                return KErrNotFound;
                }
            aProc.Close();
            }
        }
    
//    // check now if a second appearance exists in process list, 
//    // i.e. engine started from eshell => two engine processes appear in normal case
//    procName.Next(aResult2);
//
//    // check if aResult2 contained the second appearance of profiler engine
//    if(aResult2.CompareF(aResult) > 0)
//        {
//        // other process found, i.e. right process to communicate with, in case started from eshell
//        err = aProc.Open(procName);
//        if(err == KErrNone)
//            {
//            if(aProc.ExitCategory().Length() > 0)
//                {
//                // process already exited => create a new one
//                return KErrNotFound;
//                }
//            aProc.Close();
//            }
//        }
    
    return err;
    }

void CProfilerGuiModel::LaunchProfilerEngineL()
    {
    LOGTEXT(_L("CProfilerGuiModel::LaunchProfilerEngineL - start"));

    TRequestStatus stat = KRequestPending;
    RProcess proc;
    
    TInt err(KErrNone);
    
    // check if process exists
    err = FindProcessL(proc);
    
    // check if already exists and don't start a new eshell profiling
    if( err == KErrNotFound )
        {
        // try create new process
        err = proc.Create(KProfilerEngineExe, _L(""));

        // check if RProcess::Create() succeeded
        if( err == KErrNone )
            {
            // Trigger rendezvous on the supplied TRequestStatus object
            proc.Rendezvous(stat); 

            // kick off the engine process
            proc.Resume();
            
            // wait for the constructor to complete 
            User::WaitForRequest(stat); 
            
            // just lose the handle
            proc.Close();
            }
        }
    
    // Increase the client reference count in server:
    AttachClient();    
    }

//-----------------------------------------------------------------------------
// CProfilerGuiModel::TerminateProfilerL()
// Stops Profiler Engine if it has been launched by this launcher. 
//-----------------------------------------------------------------------------

void CProfilerGuiModel::TerminateProfilerL()
    {
    LOGTEXT(_L("CProfilerGuiModel::TerminateProfiler - entry"));
    
    // exit profiler engine 
    RProfiler::ExitProfiler();
        
    LOGTEXT(_L("CProfilerGuiModel::TerminateProfiler - exit"));
    
    }

void CProfilerGuiModel::AttachClient()
    {
    LOGTEXT(_L("CProfilerGuiModel::AttachClient - entry"));
    
    // 
    RProfiler::AttachClient();
        
    LOGTEXT(_L("CProfilerGuiModel::AttachClient - exit"));
    
    }

void CProfilerGuiModel::RemoveClient()
    {
    LOGTEXT(_L("CProfilerGuiModel::RemoveClient - entry"));
    
    // 
    RProfiler::RemoveClient();
        
    LOGTEXT(_L("CProfilerGuiModel::RemoveClient - exit"));
    
    }

void CProfilerGuiModel::AddNewSamplersL(CArrayFixFlat<TSamplerAttributes>& aAttributes)
	{
	TSamplerAttributes item;
	
	TInt count(aAttributes.Count());

	// loop the attribute array and insert them into view list
	for (TInt i(0);i<count;i++)
		{
    	// get a TSamplerAttributes from list at a time  
		item = aAttributes.At(i);

		iReferenceNumber++;
	
        // add item to the array
        AppendToSamplerItemListL(item);
        
        // update the listbox
        RefreshViewL(EFalse);
        
        // set item index to the begin	
        iMainContainer->ListBox()->SetCurrentItemIndexAndDraw(iMainContainer->ListBox()->Model()->NumberOfItems()-1);

		}
    // refresh again
    if(iReferenceNumber > 0)
        {
        RefreshViewL(ETrue);
        }
	}

TBool CProfilerGuiModel::CheckTraceLocationSanityL(TGeneralAttributes& aAttr, TBool aQuietCheck)
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());

    TBuf<32> drive;
    
    CnvUtfConverter::ConvertToUnicodeFromUtf8(drive, aAttr.iSaveFileDrive);

    TDriveUnit driveUnit = TDriveUnit(drive);

    TBool ret(EFalse);

    // check that the root folder is correct
    if (drive.Length() > 2 && BaflUtils::CheckFolder(fs, drive.Left(3)) == KErrNone )
        {
        // check then if drive has still some space
        if(!SysUtil::DiskSpaceBelowCriticalLevelL(&fs, 0, driveUnit))
            {
            ret = ETrue;
            }
        }
    
    // check if the sanity check failed
    if(!ret)
        {
        // show an error note
        if(aQuietCheck == EFalse)
            {
            CAknErrorNote* note = new(ELeave) CAknErrorNote();
            note->ExecuteLD(_L("Invalid path, check settings!"));
            }
        }
    fs.Close();
    return ret;
    }

// --------------------------------------------------------------------------------------------

TInt CProfilerGuiModel::LaunchSettingsDialogL()
    {
    // launch the settings dialog
    TGeneralAttributes newSettings = iGeneralAttributes;
    TBool quietCheck(EFalse);
    
    CProfilerGuiSettingsViewDlg* dlg = CProfilerGuiSettingsViewDlg::NewL(newSettings);
    TInt returnValue = dlg->ExecuteLD(R_PROFILER_GUI_SETTINGS_DIALOG);
    
    // check if exit command => no error note to user
    if(returnValue == EAknCmdExit)
        quietCheck = ETrue;
    
    // always save settings since the settings dialog does not provide a possibility to cancel
    iGeneralAttributes.iTraceOutput.Copy(newSettings.iTraceOutput);
    iGeneralAttributes.iTraceFilePrefix.Copy(newSettings.iTraceFilePrefix);
    iGeneralAttributes.iTimedSamplingPeriod = newSettings.iTimedSamplingPeriod;

    // check if debug output selected no check of
    if(newSettings.iTraceOutput.CompareF(KTraceDebugOutput) != 0)
        {
        // Check save file drive sanity
        if(CheckTraceLocationSanityL(newSettings, quietCheck))
            {
            // save the new location
            iGeneralAttributes.iSaveFileDrive.Copy(newSettings.iSaveFileDrive);
            }
        }
    
    // saves the general settings to profiler engine
    SaveGeneralSettingsL();   
    
    // make sure that the title of the application is correct
    CEikStatusPane* sp = iEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    tp->SetTextL(KAppName);
    
    return returnValue;
    }


TPtrC CProfilerGuiModel::GetWriterInfoNoteL(const TDesC& aNote)
	{
    TBuf<256> buf;
    TBuf<128> descBuf;
    
    buf.Zero();

    // get active profiler since it 
    RProfiler::GetActiveWriter(buf);
    
	if(iGeneralAttributes.iTraceOutput.CompareF(KTraceOutput) == 0)
		{
		if(iState == MProfilerStatusObserver::EInitializing ||
                iState == MProfilerStatusObserver::ERestarting )
			{
			descBuf.Zero();
			// set the additional note if available
			buf.Copy(aNote); 
			RProfiler::GetFileName(descBuf);
			buf.Append(_L("Writing to "));
			buf.Append(descBuf);
			}
		else if(iState == MProfilerStatusObserver::EIdle ||
		        iState == MProfilerStatusObserver::EStopping ||
		        iState == MProfilerStatusObserver::ERunning )
			{
			descBuf.Zero();
            // set the additional note if available
            buf.Copy(aNote); 
            RProfiler::GetFileName(descBuf);
			buf.Append(_L("Wrote trace data to "));
			buf.Append(descBuf);
			}
		else
		    {
	          buf.Copy(KNullDesC);
		    }
		}
	else if(iGeneralAttributes.iTraceOutput.CompareF(KTraceDebugOutput) == 0)
		{
		if(iState == MProfilerStatusObserver::EInitializing ||
                iState == MProfilerStatusObserver::ERestarting )
			{
			buf.Copy(_L("Writing to debug output..."));
			}
		else if( iState == MProfilerStatusObserver::EIdle || 
	             iState == MProfilerStatusObserver::EStopping ||
		         iState == MProfilerStatusObserver::ERunning )
			{
			buf.Copy(_L("Wrote trace data to debug output"));
			}
		else
		    {
		    buf.Copy(KNullDesC);
		    }
		}
	else
	    {
	    // should not reach this point
        buf.Copy(KNullDesC);
	    }
    
    return TPtrC(buf);	
    }

// --------------------------------------------------------------------------------------------

TInt CProfilerGuiModel::EditSamplerL(TSamplerAttributes& aItem)
    {
    // edit sampler specific settings i.e. attributes
    TSamplerAttributes& newSettings = aItem;
    TInt indexToReplace(iMainContainer->CurrentListBoxItemIndex());
    
    // create a new settings editor dialog
    CProfilerSamplerSettingsViewDlg* dlg = CProfilerSamplerSettingsViewDlg::NewL(newSettings);
    TInt returnValue = dlg->ExecuteLD(R_PROFILER_SAMPLER_SETTINGS_DIALOG);
    
    // save settings
    aItem = newSettings;

    // replace the old attribute container with saved values
    iSamplerItemList->Delete(indexToReplace);
    iSamplerItemList->InsertL(indexToReplace, newSettings);
    
    // save the settings to sampler item
    RProfiler::SetSamplerAttributes(newSettings);
    
    // make sure that the title of the application is correct
    CEikStatusPane* sp = iEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    tp->SetTextL(KAppName);

    return returnValue;
    }


void CProfilerGuiModel::SamplerInfoL(TSamplerAttributes& aItem)
    {
    TBuf<64> header;
    TBuf<256> info;
    
    _LIT(KSamplerStr, "Sampler Info");
    
//    CnvUtfConverter::ConvertToUnicodeFromUtf8(header, aItem.iName);
    
    header.Append(KSamplerStr);

    CnvUtfConverter::ConvertToUnicodeFromUtf8(info, aItem.iDescription);

    CAknMessageQueryDialog* dialog = new(ELeave) CAknMessageQueryDialog;
    //dialog->ExecuteLD(info);
    dialog->PrepareLC( R_PROFILER_GUI_EMPTY_ABOUT_DIALOG );
    dialog->SetHeaderText(header);
    dialog->SetMessageTextL(info);
    dialog->RunLD(); 

    RefreshViewL(EFalse);
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::StartAllSamplerItemsL(TProfilingMode aProfilingMode)
    {
    TBuf<256> activeWriterDes;
    TBuf8<256> writer8;
    
    iState = MProfilerStatusObserver::EInitializing;
    
    RProfiler::TProfilingMode profilingMode = aProfilingMode == EProfilingModeTimed ? RProfiler::EProfilingModeTimed : RProfiler::EProfilingModeNormal;
    
    // update status pane to correspond the initializing mode
    // prevent the control of sampler specific settings during the trace 
    iMainContainer->SetDimmed(ETrue);
    
    // update status pane
    iMainView->UpdateStatusPaneL(iState);
    
    // refresh view
    RefreshViewL();
    
    // try to start profiling process through client-server interface
    if(RProfiler::StartSampling(profilingMode) == KErrNotFound)
        {
        
        // profiler stopped (e.g. from eshell) and must be restarted 
        LaunchProfilerEngineL();
        
        // set general attributes
        SaveGeneralSettingsL();
        
        // set sampler attributes
        for(TInt i(0);i<iSamplerAttributes->Count();i++)
            {
            // set the attributes for each sampler loaded in the UI
            RProfiler::SetSamplerAttributes(iSamplerAttributes->At(i));
            }
        
        // try to launch sampling again
        RProfiler::StartSampling(profilingMode);
        }
    
    // get selected writer
    RProfiler::GetActiveWriter(activeWriterDes);
    CnvUtfConverter::ConvertFromUnicodeToUtf8(writer8, activeWriterDes);
    
    // check that output mode has not changed for a problem with trace file name
    // problem cases:
    // - trace file name and/or path false
    // - disk full, cannot write to given location
    // - false drive, e.g. x:
    if(writer8.CompareF(iGeneralAttributes.iTraceOutput) != 0)
        {
        // save change also to general attributes
        iGeneralAttributes.iTraceOutput.Copy(writer8);
        // show an info popup showing the logging method
        iMainContainer->ShowWriterInfoPopupL(KWarningNote);
        }
    else
        {
        // show an info popup showing the logging method
        iMainContainer->ShowWriterInfoPopupL(KNullDesC);
        }
    
    // update the view
    RefreshViewL();    
    }
    
// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::DeleteAllSamplerItemsL()
    {
    DeleteAllSamplerItems();
    
    RefreshViewL();
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::StopAllSamplerItemsL()
    {
    // show an info popup showing the logging method
    iMainContainer->ShowWriterInfoPopupL(KNullDesC);
    
    // Stop profiling process through CS session
    RProfiler::StopSampling();
    
    RefreshViewL();
    }
    
// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::RefreshViewL(TBool aClearSelection)
    {
    // refresh the main list view
    if (iMainContainer && iDrawState == EDrawStateMain && iMainContainer->ListBox())
        {
        // clear selections if any
        iMainContainer->ListBox()->ClearSelection();
        
        // set item index to 0
        if (aClearSelection)
            iMainContainer->ListBox()->SetCurrentItemIndex(0); 
        
        // set text items
        iMainContainer->SetListBoxTextArrayL(GenerateListBoxItemTextArrayL());
        }
    }
        
// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::AppendToSamplerItemListL(TSamplerAttributes& aItem)
    {
    // append single sampler item into sampler item list
    if (iSamplerItemList)
        iSamplerItemList->AppendL( aItem );
    else
        User::Leave(KErrNotReady);
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::DisableOrEnableFromSamplerItemListL(TInt aIndex)
    {
    if (iSamplerItemList)
        {
        if (iSamplerItemList->Count() > aIndex && aIndex >= 0)
            {
            TSamplerAttributes& attr = iSamplerItemList->At(aIndex);
            if (attr.iEnabled)
                {
                attr.iEnabled = EFalse;
                }
            else if (!attr.iEnabled)
                {
                attr.iEnabled = ETrue;
                }
            // save settings to engine
            RProfiler::SetSamplerAttributes(attr);
            }
        else
            User::Leave(KErrNotFound);
        }
    else
        {
        User::Leave(KErrNotReady);
        }

    }

// --------------------------------------------------------------------------------------------

TInt CProfilerGuiModel::SamplerItemCount() const
    {
    TInt count(0);
    
    if (iSamplerItemList)
        count = iSamplerItemList->Count();
    
    return count;
    }
    
// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::DeleteAllSamplerItems()
    {
    // reset sampler array
    iSamplerItemList->Reset();    
    }  
          
// --------------------------------------------------------------------------------------------

CDesCArray* CProfilerGuiModel::GenerateListBoxItemTextArrayL()
    {
    CDesCArray* textArray = new(ELeave) CDesCArrayFlat(64);
    CleanupStack::PushL(textArray);

    _LIT(KEntryTemplateRedIcon,   "1\t%S\t\t");
    _LIT(KEntryTemplateGreenIcon, "2\t%S\t\t");
    
    TSamplerAttributes attr;
    TInt itemCount(SamplerItemCount()); 
    for (TInt i(0); i<itemCount; i++)
        {
        // add description from each entry
        TBuf<512> textEntry;
        TBuf<512> description; 

        attr = iSamplerItemList->At(i);
        
        description.Copy(attr.iName);
        
        if (attr.iEnabled)
            textEntry.Format(KEntryTemplateGreenIcon, &description);
        else
            textEntry.Format(KEntryTemplateRedIcon, &description);
        
        textArray->AppendL(textEntry);
        }    
    
    CleanupStack::Pop(); //textArray
    return textArray;
    }

// --------------------------------------------------------------------------------------------

TBool CProfilerGuiModel::GetSelectedItemEnabled()
    {
    TInt currentItemIndex = iMainContainer->CurrentListBoxItemIndex();
    
    if (SamplerItemCount() > currentItemIndex && currentItemIndex >= 0)
        {
        return iSamplerItemList->At(currentItemIndex).iEnabled;
        }
    return EFalse;
    }

// --------------------------------------------------------------------------------------------

TBool CProfilerGuiModel::GetSelectedItemHasSettings()
    {
    TInt currentItemIndex = iMainContainer->CurrentListBoxItemIndex();
    
    if (SamplerItemCount() > currentItemIndex && currentItemIndex >= 0)
        {
        if(iSamplerItemList->At(currentItemIndex).iItemCount != 0 || 
                iSamplerItemList->At(currentItemIndex).iSampleRate != KErrNotFound)
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------------------------

TBool CProfilerGuiModel::GetSelectedItemHidden()
    {
    TInt currentItemIndex = iMainContainer->CurrentListBoxItemIndex();
    
    if (SamplerItemCount() > currentItemIndex && currentItemIndex >= 0)
        {
        if(iSamplerItemList->At(currentItemIndex).iIsHidden)
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------------------------

TInt CProfilerGuiModel::EditSelectedSamplerL(TInt index)
    {
    TSamplerAttributes sampler;
    if(SamplerItemCount() > index && index >= 0)
        {
        sampler = iSamplerItemList->At(index);
        return EditSamplerL(sampler);
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::SelectedSamplerInfoL(TInt index)
    {
    if(SamplerItemCount() > index && index >= 0)
        {
        SamplerInfoL(iSamplerItemList->At(index));
        }
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::ShowItemActionMenuL()
    {
    TInt currentItemIndex = iMainContainer->CurrentListBoxItemIndex();
    
    if (SamplerItemCount() > currentItemIndex && currentItemIndex >= 0)
        {
        // show a query dialog    
        TInt queryIndex(0);
        CAknListQueryDialog* listQueryDlg = new(ELeave) CAknListQueryDialog(&queryIndex);
        TInt resource = 0;
        TSamplerAttributes sampler = iSamplerItemList->At(currentItemIndex);
        
        // check if sampler enabled
        if(sampler.iEnabled)
        	{
        	resource = R_ITEM_ACTION_QUERY_ENABLED;
            }
        else
        	{
        	resource = R_ITEM_ACTION_QUERY_DISABLED;
        	}
        
        // check if sampler plugin hidden => don't allow starting and stopping of sampling
        if(sampler.iIsHidden)
        	{
        	resource = R_ITEM_ACTION_QUERY_HIDDEN_START_STOP;
        	}
        else if(( sampler.iItemCount == 0 && 
        		sampler.iSampleRate == KErrNotFound ) && 
        		sampler.iEnabled )
            {
            resource = R_ITEM_ACTION_QUERY_HIDDEN_SETTINGS_ENABLED;
            }
        else if(( sampler.iItemCount == 0 && 
                sampler.iSampleRate == KErrNotFound ) && 
                !sampler.iEnabled )
            {
            resource = R_ITEM_ACTION_QUERY_HIDDEN_SETTINGS_DISABLED;
            }
        
        // execute query dialog with the given resource
        if (listQueryDlg->ExecuteLD(resource))
            {
            if(!sampler.iIsHidden)
            	{
            	if(sampler.iItemCount != 0 || 
            	        sampler.iSampleRate != KErrNotFound )
            	    {
                    // enable/disable sampler
                    if (queryIndex == EItemActionMenuTypeEnable)
                        {
                        DisableOrEnableSelectedOrHighlightedItemsL();
                        }
                    // edit sampler settings
                    else if (queryIndex == EItemActionMenuTypeEditSettings - 1)
                        {
                        EditSamplerL(sampler);
                        }
                    // info
                    else if (queryIndex == EItemActionMenuTypeInfo - 1)
                        {
                        SamplerInfoL(sampler);
                        }
            	    }
            	else
            	    {
                    // special case, use raw query indexes
                     if (queryIndex == 0)
                         {
                         DisableOrEnableSelectedOrHighlightedItemsL();
                         }
                     else if (queryIndex == 1)
                         {
                         SamplerInfoL(sampler);
                         }
            	    }
            	}
            else
				{
				if(sampler.iItemCount != 0 || 
				        sampler.iSampleRate != KErrNotFound)
					{
					// special case, use raw query indexes
					// edit sampler settings
					if (queryIndex == 0)
						{
						EditSamplerL(sampler);
						}
					// sampler info
					else if (queryIndex == 1)
						{
						SamplerInfoL(sampler);
						}
					}
				else
					{
					// only sampler info available if hidden and no setting items to be set
					if (queryIndex == 0)
						{
						SamplerInfoL(sampler);
						}
					
					}
				}
            }
        }
    }

void CProfilerGuiModel::DisableOrEnableSelectedOrHighlightedItemsL()
    {
    const CArrayFix<TInt>* selectionIndexes = iMainContainer->ListBoxSelectionIndexes();

    // by default use selected items
    if (selectionIndexes && selectionIndexes->Count() > 0)
        {
        TInt ref(0);
        TKeyArrayFix key(0, ECmpTUint16);
        TInt index(0);

        for (TInt i=0; i<SamplerItemCount(); i++)
            {
            ref = i;

            if (selectionIndexes->Find(ref, key, index) == 0)  
                {
                DisableOrEnableFromSamplerItemListL(i);
                }
            }
        
        RefreshViewL(EFalse);
        }
    
    // or if none selected, use the current item index
    else
        {
        TInt currentItemIndex = iMainContainer->CurrentListBoxItemIndex();
        
        if (SamplerItemCount() > currentItemIndex && currentItemIndex >= 0)
            {
            DisableOrEnableFromSamplerItemListL(currentItemIndex);
            
            RefreshViewL(EFalse);
            }
        }  
    }

void CProfilerGuiModel::LoadPluginsL()
	{
    // get samplers from Profiler Engine (client-server session) 
	// and add the to the samplers list for the first time
    LOGTEXT(_L("CProfilerGuiModel::LoadPlugins - get sampler plugins"));
    TInt err = RProfiler::GetSamplerAttributes(*iSamplerAttributes);
    
    // check if engine provided a list of samplers
    if( err != KErrNone )
        {
        // could not get samplers from engine
        LOGTEXT(_L("CProfilerGuiModel::LoadPlugins - failed to connect engine"));
        }
    else
        {
        LOGTEXT(_L("CProfilerGuiModel::LoadPlugins - adding new samplers into view"));
        AddNewSamplersL(*iSamplerAttributes);
        }
    
    LOGTEXT(_L("CProfilerGuiModel::LoadPlugins - exit"));
	}

// --------------------------------------------------------------------------------------------

TInt CProfilerGuiModel::LoadGeneralSettingsL()
    {
    // local variable for getting saved settings from profiler engine
    TGeneralAttributes generalAttr;
    TInt err(KErrNone);
    
    // before loading saved settings (from settings file) set the default values 
    iGeneralAttributes.iTraceOutput.Copy(KTraceOutput);
    iGeneralAttributes.iTraceFilePrefix.Copy(KProfilerDefaultPrefix);
    iGeneralAttributes.iSaveFileDrive.Copy(KProfilerDefaultDrive);
    iGeneralAttributes.iTimedSamplingPeriod = KDefaultTimedSamplingPeriod;
    
    // request to 
    err = RProfiler::GetGeneralAttributes(generalAttr);
    
    // check that request succesfull
    if( err != KErrNone )
        {
        // could not connect profiler engine, use 
        return err;
        }
    
    // check if saved settings different than the default
    if(generalAttr.iTraceOutput.MatchF(iGeneralAttributes.iTraceOutput) == KErrNotFound)
        {
        iGeneralAttributes.iTraceOutput.Copy(generalAttr.iTraceOutput);
        }

    if(generalAttr.iTraceFilePrefix.MatchF(iGeneralAttributes.iTraceFilePrefix) == KErrNotFound)
        {
        iGeneralAttributes.iTraceFilePrefix.Copy(generalAttr.iTraceFilePrefix);
        }

    if(generalAttr.iSaveFileDrive.MatchF(iGeneralAttributes.iSaveFileDrive) == KErrNotFound)
        {
        iGeneralAttributes.iSaveFileDrive.Copy(generalAttr.iSaveFileDrive);
        }
    
    if( generalAttr.iTimedSamplingPeriod > 0)
        {
        iGeneralAttributes.iTimedSamplingPeriod = generalAttr.iTimedSamplingPeriod;
        }
    
    return err;
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::SaveGeneralSettingsL()
    {
    TInt err(KErrNone);
    
    // save general attributes to Profiler Engine
    err = RProfiler::SetGeneralAttributes(iGeneralAttributes);
    
    // check if save failed
    if(err == KErrNotFound)
        {
        // profiler stopped (e.g. from eshell) and must be restarted 
        LaunchProfilerEngineL();
        
        err = RProfiler::SetGeneralAttributes(iGeneralAttributes);
        if(err != KErrNone)
            {
            // leave no use to continue
            User::Leave(err);
            }
        }
    }

void CProfilerGuiModel::SetMainView(CProfilerGuiMainView* aMainView)
    {
    iMainView = aMainView;
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiModel::HandleProfilerStatusChange( KProfilerStatus aStatus )
    {
    iState = aStatus;
    if( aStatus == EIdle )
        {
        iMainContainer->ShowWriterInfoPopupL(KNullDesC);
        // set plugin list back to enabled
        iMainContainer->SetDimmed(EFalse);
        }
    TRAP_IGNORE(iMainView->UpdateStatusPaneL(aStatus));
    }

void CProfilerGuiModel::HandleProfilerErrorL( TInt aError )
    {
    TBuf<64> errorMsg;
    _LIT(KErrorMessage, "Error: ");
    _LIT(KNoMemory, "Cannot write to file, check settings");
    
    errorMsg.Copy(KErrorMessage);
    // message from pwr sampler
    if( aError < -1000 )
        {
        errorMsg.Append(_L("Stop other power measurement tools!"));
        }
    else if( aError == KErrAlreadyExists || aError == 11 )
        {
        errorMsg.Append(_L("Close old Profiler before start!"));
        }
    else if( aError == KErrNotReady )
        {
        errorMsg.Append(_L("Memory card removed, failed to write!"));
        }
    else if( aError == KErrPathNotFound )
        {
        errorMsg.Append(_L("Given trace data location does not exist"));
        }
    else
        {
        if( aError == KErrNoMemory || 
                aError == KErrOverflow || 
                aError == KErrDirFull || 
                aError == KErrDiskFull || 
                aError == KErrNotReady )
            {
            errorMsg.Append(KNoMemory);
            }
        else
            {
            errorMsg.Append(_L("code: "));
            errorMsg.AppendNum(aError);
            }
        }
    // simply show an error note
    CAknErrorNote* note = new(ELeave) CAknErrorNote();
    note->ExecuteLD(errorMsg);
    
    // set state idle in all error cases
    iState = MProfilerStatusObserver::EIdle;
    
    // update status pane
    iMainView->UpdateStatusPaneL(iState);
    
    // set plugin list back to enabled
    iMainContainer->SetDimmed(EFalse);
    
    // refresh and set menus etc. in correct state
    RefreshViewL();
    }

// End of File
